#include "engine/Model.h"
#include "engine/Shader.h"
#include "engine/Texture.h"
#include <iostream>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/texture.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


#include <filesystem>
#include <algorithm>
namespace fs = std::filesystem;

// helper to extract model path
static std::string getModelDirectory(const std::string& modelPath) {
    size_t lastSlash = modelPath.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
        return modelPath.substr(0, lastSlash + 1);
    }
    return "";  // Model in current directory
}

// Constructor to load model
Model::Model(const std::string& path) {
    loadModel(path);
}

// alt Constructor for funky stuff
Model::Model(const std::string& path, const std::vector<std::string>& skipNames)
    : meshNameSkips(skipNames){
    loadModel(path);
}

void Model::setPosition(const glm::vec3& pos) { position = pos; }

void Model::setRotation(float angleDeg, const glm::vec3& axis) {
    rotation = glm::angleAxis(glm::radians(angleDeg), glm::normalize(axis));
}

void Model::setScale(const glm::vec3& s) { scale = s; }

// Quaternion handling
void Model::setRotationQuat(const glm::quat& q) {
    rotation = glm::normalize(q);
}

glm::quat Model::getRotationQuat() const {
    return rotation;
}

void Model::setRotationEuler(float pitchDeg, float yawDeg, float rollDeg, RotationOrder order) {
    rotation = glm::normalize(MathUtils::eulerToQuat(pitchDeg, yawDeg, rollDeg, order));
}

void Model::Draw(Shader& shader) {
    if (meshes.empty()) return; // guard
    glm::mat4 computedMatrix = getModelMatrix();  // Compute TRS from components
    // draws each mesh onto scene
    for (auto& mesh : meshes) {
        // combine model transform with mesh
        glm::mat4 finalMatrix = computedMatrix * mesh->getModelMatrix();
        // export the finalMatrix to the Vertex Shader of model
        shader.setMat4("model", finalMatrix);
        // issue the actual draw for this mesh
        mesh->Draw(shader);
    }
}

void Model::loadModel(const std::string& path) {
    // create Assimp importer
    Assimp::Importer importer;

    // import flags
    unsigned int flags =
        aiProcess_Triangulate           | // Ensures all faces are triangles
        aiProcess_GenNormals            | // Generates normals if missing
        aiProcess_JoinIdenticalVertices |  // Optimizes geometry
        aiProcess_PreTransformVertices  | // Bake node transforms into vertices
        aiProcess_OptimizeMeshes; // Merge tiny meshes to reduce draw calls

    // import the 3D model file
    const aiScene* scene = importer.ReadFile(path, flags);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "Error loading model: " << importer.GetErrorString() << std::endl;
        return;
    }
    modelPath = path;
    directory = getModelDirectory(path);
    texturesDir = directory + "textures/";
    std::cout << "[Model] model dir: " << directory << "\n";
    std::cout << "[Model] fallback textures dir: " << texturesDir << "\n";


    // begin recursively processing the model hierarchy
    processNode(scene->mRootNode, scene);
}


bool Model::shouldSkipMesh(const std::string& name) const {
    for (const auto& s : meshNameSkips) {
        if (name.find(s) != std::string::npos) return true;
    }
    return false;
}

void Model::processNode(aiNode* node, const aiScene* scene) {
    // process all the node's meshes (if any)
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

        // Debug: print mesh info
        std::string meshName = mesh->mName.C_Str();
        std::cout << "[Model] Mesh: " << meshName
            << " | Vertices: " << mesh->mNumVertices << std::endl;
;
        if (shouldSkipMesh(meshName)) {
            std::cout << "[Model] Skipping mesh: " << meshName << std::endl;
            continue;
        }

        for (unsigned i = 0; i < mesh->mNumVertices; ++i) {
            const aiVector3D& v = mesh->mVertices[i];
            // expand model-space AABB
            aabbMin.x = std::min(aabbMin.x, v.x);
            aabbMin.y = std::min(aabbMin.y, v.y);
            aabbMin.z = std::min(aabbMin.z, v.z);
            aabbMax.x = std::max(aabbMax.x, v.x);
            aabbMax.y = std::max(aabbMax.y, v.y);
            aabbMax.z = std::max(aabbMax.z, v.z);
        }

        // store mesh
        meshes.emplace_back(processMesh(mesh, scene));
    }
    // then do the same for each of its children
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

static std::string toLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
        [](unsigned char c) { return (char)std::tolower(c); });
    return s;
}

static std::string findFirstMatchingTexture(
    const std::string& dir,
    const std::vector<std::string>& keywords)
{
    if (!fs::exists(dir) || !fs::is_directory(dir)) return "";

    for (const auto& entry : fs::directory_iterator(dir)) {
        if (!entry.is_regular_file()) continue;

        std::string name = toLower(entry.path().filename().string());
        for (const std::string& k : keywords) {
            if (name.find(k) != std::string::npos) {
                return entry.path().string();
            }
        }
    }
    return "";
}

std::shared_ptr<Texture> Model::loadTexture(const aiString& path,
    const char* typeName, const aiScene* scene, GLuint slot) {
    // Cache key includes semantic to avoid collisions
    std::string key = std::string(typeName) + ":" + path.C_Str();

    // cache hit
    auto it = textureCache.find(key);
    if (it != textureCache.end())
        return it->second;

    std::shared_ptr<Texture> tex;
    const char* cpath = path.C_Str();

    // embedded texture
    if (cpath[0] == '*') {
        std::cout << "[Texture] Embedded texture detected: "
            << key << "\n";
        int idx = std::atoi(cpath + 1);
        const aiTexture* aiTex = scene->mTextures[idx];

        const unsigned char* bytes =
            reinterpret_cast<const unsigned char*>(aiTex->pcData);

        size_t size = aiTex->mWidth; // slot will be ignored
        tex = std::make_shared<Texture>(bytes, size, slot, GL_UNSIGNED_BYTE);
    }
    // external texture
    else {
        std::cout << "[Texture] External texture detected: "
            << directory << "/" << key << "\n";
        std::string fullPath = directory + "/" + cpath;
        tex = std::make_shared<Texture>(fullPath.c_str(), slot, GL_UNSIGNED_BYTE);
    }

    textureCache[key] = tex;
    return tex;
}

void Model::attachTextures(std::vector<std::shared_ptr<Texture>>& textures,
    aiMaterial* material, const aiScene* scene) {
    std::cout << "\n[Material] Processing material\n";
    
    // Define the texture types and their corresponding uniform names
    static const std::vector<std::pair<aiTextureType, const char*>> types = {
        // Base color
        { aiTextureType_BASE_COLOR,        "diffuse" },
        { aiTextureType_DIFFUSE,           "diffuse" },

        // Normal mapping
        { aiTextureType_NORMALS,           "normal" },

        // Specular workflow (legacy)
        { aiTextureType_SPECULAR,          "specular" },

        // Metallic–roughness workflow
        { aiTextureType_METALNESS,         "metallic" },
        { aiTextureType_DIFFUSE_ROUGHNESS, "roughness" },

        // Ambient occlusion
        { aiTextureType_AMBIENT_OCCLUSION, "ao" }
    };

    // Prevent duplicate semantics (one texture per type)
    // auto hasType = [&](const char* type) {
    //     return std::any_of(textures.begin(), textures.end(),
    //         [&](const std::shared_ptr<Texture>& t) {
    //             return std::strcmp(t->type, type) == 0;
    //         });
    // };

	// Load with Assimp material textures
    for (const auto& pair : types) {
        aiTextureType type = pair.first;
        const char* name = pair.second;
        //if (hasType(name)) continue;

        for (unsigned i = 0; i < material->GetTextureCount(type); ++i) {
            aiString path;
            material->GetTexture(type, i, &path);

            std::cout << "[Material] Requested texture: \""
                << path.C_Str()
                << "\" (type = " << name << ")\n";

            auto tex = loadTexture(path, name, scene, 0);
            textures.push_back(tex);
        }
    }

	// Fallback to forced textures if none loaded
    if (textures.empty() && scene->mNumTextures == 0) {
        std::cout << "[Texture] No material textures; trying fallback folder\n";

        auto tryAdd = [&](const char* semantic,
                          const std::vector<std::string>& hints)
        {
            //if (hasType(semantic)) return;
            std::string file = findFirstMatchingTexture(texturesDir, hints);
            if (!file.empty()) {
                std::cout << "[Texture] Fallback " << semantic << ": "
                          << file << "\n";
                textures.push_back(std::make_shared<Texture>(
                    file.c_str(), 0, GL_UNSIGNED_BYTE));
            }
        };

        tryAdd("diffuse",   { "basecolor","albedo","diffuse","color" });
        tryAdd("normal",    { "normal","nrm" });
        tryAdd("specular",  { "specular","spec" });
        tryAdd("roughness", { "rough","roughness" });
        tryAdd("metallic",  { "metal","metallic" });
        tryAdd("ao",        { "ao","ambient" });
    }
}

void Model::generateTangents(std::vector<Vertex>& vertices,
    const std::vector<GLuint>& indices) {
    // Per-triangle tangent accumulation 
    for (size_t i = 0; i < indices.size(); i += 3) {
        // Fetch the three vertices of the triangle
        Vertex& v0 = vertices[indices[i + 0]];
        Vertex& v1 = vertices[indices[i + 1]];
        Vertex& v2 = vertices[indices[i + 2]];

        // Edges of the triangle in object space
        glm::vec3 e1 = v1.position - v0.position;
        glm::vec3 e2 = v2.position - v0.position;

        // Edges of the triangle in UV space
        glm::vec2 deltaUV1 = v1.texUV - v0.texUV;
        glm::vec2 deltaUV2 = v2.texUV - v0.texUV;

        // Compute determinant of the UV matrix
        float determinant = deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y;
        if (fabs(determinant) < 1e-6f) continue;
        float invDet = 1.0f / determinant;

        // Compute the tangent vector for this triangle
        glm::vec3 tangent = invDet * (deltaUV2.y * e1 - deltaUV1.y * e2);

        // Accumulate the tangent into each vertex of the triangle
        v0.tangent += tangent;
        v1.tangent += tangent;
        v2.tangent += tangent;
    }
    // Per-vertex normalization & orthogonalization
    for (auto& v : vertices) {
        // Skip vertices that never received tangent data
        if (glm::length(v.tangent) == 0.0f) continue;
        // Gram-Schmidt orthogonalize tangent with respect to normal
        v.tangent = glm::normalize(
            v.tangent - v.normal * glm::dot(v.normal, v.tangent)
        );
    }
}

std::shared_ptr<Mesh> Model::processMesh(aiMesh* mesh, const aiScene* scene) {
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    std::vector<std::shared_ptr<Texture>> textures;

    // extract vertex data
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex{}; // zero-initialize
        
        // Vertex position
        vertex.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);

        // Normals (if they exist)
        if (mesh->HasNormals())
            vertex.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        else
            vertex.normal = glm::vec3(0.0f);

        // Texture coordinates
        if (mesh->HasTextureCoords(0))
            vertex.texUV = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        else
            vertex.texUV = glm::vec2(0.0f);

        // Optional: Vertex color
        vertex.color = glm::vec3(1.0f); // Default white

        // Optional: Tangent
        if (mesh->HasTangentsAndBitangents())
            vertex.tangent = glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
        else
            vertex.tangent = glm::vec3(0.0f);

        vertices.push_back(vertex);
    }

    // process indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(static_cast<GLuint>(face.mIndices[j]));
        }
    }

    // process textures
    if (mesh->mMaterialIndex >= 0) {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        attachTextures(textures, material, scene);
    }

    // generate tangents manually if not provided but UVs exist
    if (!mesh->HasTangentsAndBitangents() && mesh->HasTextureCoords(0)) {
        generateTangents(vertices, indices);
    }

    // construct Mesh in place once and transfer ownership into Model
    return std::make_shared<Mesh>(vertices, indices, textures);
}