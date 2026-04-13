// ==================================================
// Author: Priyansh Nayak
// Description: Imported model wrapper built on top of Assimp-loaded meshes
// ==================================================

#pragma once


#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <limits>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "engine/MathUtils.h"

class Shader;
class Mesh;
class Texture;
class Material;

// Forward declare Assimp structs
struct aiScene;
struct aiNode;
struct aiMesh;
struct aiMaterial;
struct aiString;

class Model
{
public:
    // constructors
    // Params: const std::string& path
    explicit Model(const std::string& path);
    Model(const std::string& path, const std::vector<std::string>& skipNames);

    // Prevent copying
    Model(const Model&) = delete;
    Model& operator=(const Model&) = delete;

    // simple setters for TRS
    // Params: const glm::vec3& pos
    void setPosition(const glm::vec3& pos);
    void setRotation(float angleDeg, const glm::vec3& axis); // axis-angle
    // Description: setScale
    // Params: const glm::vec3& s
    void setScale(const glm::vec3& s);

    // additional usage for Rotation
    // Params: const glm::quat& q
    void setRotationQuat(const glm::quat& q);
    void setRotationEuler(float pitchDeg, float yawDeg, float rollDeg,
                      MathUtils::RotationOrder order = MathUtils::RotationOrder::YXZ);
    // Description: getRotationQuat
    // Params: none
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
    // Params: Shader& shader
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
    // Description: shouldSkipMesh
    // Params: const std::string& name
    bool shouldSkipMesh(const std::string& name) const;

	// procedure to load model
    // Params: const std::string& path
    void loadModel(const std::string& path);
    void processNode(aiNode* node, const aiScene* scene);
    // Description: processMesh
    // Params: aiMesh* mesh, const aiScene* scene
    std::shared_ptr<Mesh> processMesh(aiMesh* mesh, const aiScene* scene);
    
	// Texture loading
    // Params: const aiString& path, const aiScene* scene
    std::shared_ptr<Texture> loadTexture(const aiString& path, const aiScene* scene);
    void attachTextures(std::shared_ptr<Material> materialObj,
        aiMaterial* material, const aiScene* scene);
};
