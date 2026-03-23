#pragma once

#include "engine/Cubemap.h"
#include "engine/Camera.h"

class Shader;

class Skybox {
public:
    Skybox(Cubemap& cubemap);
    ~Skybox();

    void Draw(const Camera& camera);

private:
    unsigned int VAO = 0;
    unsigned int VBO = 0;

    Cubemap& environment;
    Shader* shader = nullptr;

    void initCube();
};
