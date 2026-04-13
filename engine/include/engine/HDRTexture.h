// ==================================================
// Author: Priyansh Nayak
// Description: HDR texture loader used as input for cubemap generation
// ==================================================

#pragma once

#include <string>
#include <glad/glad.h>

// Loads an HDR equirectangular texture from disk
class HDRTexture{
public:
    GLuint ID;
    int width = 0;
    int height = 0;

    // Description: HDRTexture
    // Params: const std::string& path
    HDRTexture(const std::string& path);
    void Bind(GLuint unit = 0) const;
};

