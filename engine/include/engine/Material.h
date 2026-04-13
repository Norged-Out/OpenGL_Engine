// ==================================================
// Author: Priyansh Nayak
// Description: Material bundle describing the textures used by a mesh
// ==================================================

#pragma once

#include <unordered_map>
#include <memory>
#include <string>
#include "engine/Texture.h"

class Shader;

class Material
{
public:
    // Description: setTexture
    // Params: const std::string& type, std::shared_ptr<Texture> tex
    void setTexture(const std::string& type, std::shared_ptr<Texture> tex);
    void bind(Shader& shader);
    // Description: setFiltering
    // Params: GLenum minFilter, GLenum magFilter
    void setFiltering(GLenum minFilter, GLenum magFilter);
    void setWrapping(GLenum wrapS, GLenum wrapT);

    static std::shared_ptr<Material> CreateMat(
        const std::string& diffusePath,
        const std::string& normalPath,
        const std::string& roughnessPath,
        const std::string& metallicPath = "",
        const std::string& aoPath = "");

private:
    std::unordered_map<std::string, std::shared_ptr<Texture>> textures;
};
