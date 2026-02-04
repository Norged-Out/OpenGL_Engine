#include "engine/Model.h"
#include "engine/Shader.h"
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

static std::string toLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
        [](unsigned char c) { return (char)std::tolower(c); });
    return s;
}

static std::string findFirstMatchingTexture(
    const std::string& dir,
    const std::initializer_list<const char*>& keywords)
{
    if (!fs::exists(dir) || !fs::is_directory(dir)) return "";

    for (const auto& entry : fs::directory_iterator(dir)) {
        if (!entry.is_regular_file()) continue;

        std::string name = toLower(entry.path().filename().string());
        for (const char* k : keywords) {
            if (name.find(k) != std::string::npos) {
                return entry.path().string();
            }
        }
    }
    return "";
}



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

// alt Constructor to force diffuse/specular textures
Model::Model(const std::string& path,
    const std::string& diffusePath,
    const std::string& specularPath)
    : diffusePath(diffusePath), specularPath(specularPath) {
    loadModel(path);
}


void Model::setPosition(const glm::vec3& pos) { position = pos; }

void Model::setRotation(float angleDeg, const glm::vec3& axis) {
    rotation = glm::angleAxis(glm::radians(angleDeg), glm::normalize(axis));
}

void Model::setScale(const glm::vec3& s) { scale = s; }


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


std::shared_ptr<Texture> Model::LoadTexture(const aiString& path,
    const char* typeName, const aiScene* scene, GLuint slot) {

    std::string key = path.C_Str();

    // cache hit
    auto it = textureCache.find(key);
    if (it != textureCache.end())
        return it->second;

    std::shared_ptr<Texture> tex;

    if (!key.empty() && key[0] == '*') {
        std::cout << "[Texture] Embedded texture detected: "
            << key << "\n";
    }
    else {
        std::cout << "[Texture] External texture detected: "
            << directory << "/" << key << "\n";
    }


    // embedded texture
    if (!key.empty() && key[0] == '*') {
        int idx = std::atoi(key.c_str() + 1);
        const aiTexture* aiTex = scene->mTextures[idx];

        const unsigned char* bytes =
            reinterpret_cast<const unsigned char*>(aiTex->pcData);

        size_t size = aiTex->mWidth;
        tex = std::make_shared<Texture>(bytes, size, typeName, slot, GL_UNSIGNED_BYTE);
    }
    // external texture
    else {
        std::string fullPath = directory + "/" + key;
        tex = std::make_shared<Texture>(fullPath.c_str(), typeName, slot, GL_UNSIGNED_BYTE);
    }

    textureCache[key] = tex;
    return tex;
}


void Model::AttachTextures(std::vector<std::shared_ptr<Texture>>& textures,
    aiMaterial* material, const aiScene* scene) {
    std::cout << "\n[Material] Processing material\n";

    static const std::vector<std::pair<aiTextureType, const char*>> types = {
        { aiTextureType_BASE_COLOR, "diffuse" },
        { aiTextureType_DIFFUSE,    "diffuse" },
        { aiTextureType_SPECULAR,   "specular" },
        { aiTextureType_NORMALS,    "normal" },
    };

    GLuint slot = 0;

	// Load with Assimp material textures
    for (const auto& pair : types) {
        aiTextureType type = pair.first;
        const char* name = pair.second;

        for (unsigned i = 0; i < material->GetTextureCount(type); ++i) {
            aiString path;
            material->GetTexture(type, i, &path);

            std::cout << "[Material] Requested texture: \""
                << path.C_Str()
                << "\" (type = " << name << ")\n";

            auto tex = LoadTexture(path, name, scene, slot++);
            textures.push_back(tex);
        }
    }

	// Fallback to forced textures if none loaded
    if (textures.empty() && scene->mNumTextures == 0) {
        std::cout << "[Texture] No material textures; trying fallback folder\n";

        std::string diffuseFile = findFirstMatchingTexture(
            texturesDir, { "basecolor","albedo","diffuse","color" }
        );
        std::string normalFile = findFirstMatchingTexture(
            texturesDir, { "normal","nrm" }
        );

        GLuint slot = 0;

        if (!diffuseFile.empty()) {
            std::cout << "[Texture] Fallback diffuse: " << diffuseFile << "\n";
            textures.push_back(std::make_shared<Texture>(
                diffuseFile.c_str(), "diffuse", slot++, GL_UNSIGNED_BYTE));
        }

        if (!normalFile.empty()) {
            std::cout << "[Texture] Fallback normal: " << normalFile << "\n";
            textures.push_back(std::make_shared<Texture>(
                normalFile.c_str(), "normal", slot++, GL_UNSIGNED_BYTE));
        }
    }

}



std::shared_ptr<Mesh> Model::processMesh(aiMesh* mesh, const aiScene* scene) {
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    std::vector<std::shared_ptr<Texture>> textures;

    // extract vertex data
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
        
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
        AttachTextures(textures, material, scene);
    }

    // construct Mesh in place once and transfer ownership into Model
    return std::make_shared<Mesh>(vertices, indices, textures);
}