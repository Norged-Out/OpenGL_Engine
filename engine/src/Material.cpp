#include "engine/Material.h"
#include "engine/Shader.h"

void Material::setTexture(const std::string& type, std::shared_ptr<Texture> tex) {
    // Assigns a texture to this material under a semantic name
    textures[type] = tex;
}

void Material::bind(Shader& shader) {
    GLuint slot = 0;
    for (auto& [type, tex] : textures){
        std::string uniform = type + "0"; // e.g. "diffuse" -> "diffuse0"
        // Set the shader uniform to the correct texture slot
        shader.setInt(uniform.c_str(), slot);
        // Bind the texture to the corresponding slot
        tex->Bind(slot);
        slot++;
    }
}

std::shared_ptr<Material> Material::CreateMat(
    const std::string& diffusePath,
    const std::string& normalPath,
    const std::string& roughnessPath,
    const std::string& metallicPath,
    const std::string& aoPath)
{
    auto material = std::make_shared<Material>();

    if (!diffusePath.empty())
    {
        material->setTexture("diffuse",
            std::make_shared<Texture>(
                diffusePath.c_str(),
                GL_UNSIGNED_BYTE));
    }

    if (!normalPath.empty())
    {
        material->setTexture("normal",
            std::make_shared<Texture>(
                normalPath.c_str(),
                GL_UNSIGNED_BYTE));
    }

    if (!roughnessPath.empty())
    {
        material->setTexture("roughness",
            std::make_shared<Texture>(
                roughnessPath.c_str(),
                GL_UNSIGNED_BYTE));
    }

    if (!metallicPath.empty())
    {
        material->setTexture("metallic",
            std::make_shared<Texture>(
                metallicPath.c_str(),
                GL_UNSIGNED_BYTE));
    }

    if (!aoPath.empty())
    {
        material->setTexture("ao",
            std::make_shared<Texture>(
                aoPath.c_str(),
                GL_UNSIGNED_BYTE));
    }

    return material;
}