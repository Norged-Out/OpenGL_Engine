#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp> 
#include "engine/Mesh.h"
#include "engine/MathUtils.h"
class Shader;

class Model
{
public:
    // constructors
    explicit Model(const std::string& path);
    Model(const std::string& path, const std::vector<std::string>& skipNames);

    // Prevent copying
    Model(const Model&) = delete;
    Model& operator=(const Model&) = delete;

    // simple setters for TRS
    void setPosition(const glm::vec3& pos);
    void setRotation(float angleDeg, const glm::vec3& axis); // axis-angle
    void setScale(const glm::vec3& s);

    // additional usage for Rotation
    void setRotationQuat(const glm::quat& q);
    void setRotationEuler(float pitchDeg, float yawDeg, float rollDeg,
                      RotationOrder order = RotationOrder::YXZ);
    glm::quat getRotationQuat() const;

	glm::mat4 getModelMatrix() const {
        return glm::translate(glm::mat4(1.0f), position)
            * glm::mat4_cast(rotation)
            * glm::scale(glm::mat4(1.0f), scale);
    }

    // axis-aligned bounding box (model space)
    glm::vec3 getAABBMin() const { return aabbMin; }
    glm::vec3 getAABBMax() const { return aabbMax; }
    glm::vec3 getAABBCenter() const { return (aabbMin + aabbMax) * 0.5f; }
    glm::vec3 getAABBSize() const { return (aabbMax - aabbMin); }

    // draw the model's meshes
    void Draw(Shader& shader);

private:
    // local transform
    glm::vec3 position = glm::vec3(0.0f);
    glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);  // Identity quaternion
    glm::vec3 scale = glm::vec3(1.0f);

    // model space bounds
    glm::vec3 aabbMin = glm::vec3(std::numeric_limits<float>::max());
    glm::vec3 aabbMax = glm::vec3(-std::numeric_limits<float>::max());

	// the shared asset data
    std::unordered_map<std::string, std::shared_ptr<Texture>> textureCache;
	std::string modelPath;
    std::string directory;
    std::string texturesDir;
    std::vector<std::shared_ptr<Mesh>> meshes;

    // Skip some unwanted meshes
    std::vector<std::string> meshNameSkips;
    bool shouldSkipMesh(const std::string& name) const;

	// procedure to load model
    void loadModel(const std::string& path);
    void processNode(aiNode* node, const aiScene* scene);
    std::shared_ptr<Mesh> processMesh(aiMesh* mesh, const aiScene* scene);
    
	// Texture loading
    std::shared_ptr<Texture> LoadTexture(const aiString& path,
        const char* typeName, const aiScene* scene,GLuint slot);

    void AttachTextures(std::vector<std::shared_ptr<Texture>>& textures,
        aiMaterial* material, const aiScene* scene);
};