#pragma once

#include "engine/Shader.h"
#include "engine/Cubemap.h"
#include "engine/Camera.h"

class Skybox {
public:
    Skybox(Cubemap& cubemap);
    ~Skybox();

    void Draw(const Camera& camera, Shader& shader);

private:
    unsigned int VAO = 0;
    unsigned int VBO = 0;

    Cubemap& environment;

    void initCube();
};
