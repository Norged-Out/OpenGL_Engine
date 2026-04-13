// ==================================================
// Author: Priyansh Nayak
// Description: Skybox renderer that owns its cubemap and shader state
// ==================================================

#pragma once

#include "engine/Cubemap.h"
#include "engine/Camera.h"

class Shader;

class Skybox {
public:
    // Description: Skybox
    // Params: Cubemap& cubemap
    Skybox(Cubemap& cubemap);
    ~Skybox();

    // Description: Draw
    // Params: const Camera& camera
    void Draw(const Camera& camera);

private:
    unsigned int VAO = 0;
    unsigned int VBO = 0;

    Cubemap& environment;
    Shader* shader = nullptr;

    // Description: initCube
    // Params: none
    void initCube();
};

