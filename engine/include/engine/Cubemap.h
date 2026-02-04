#pragma once

#include <glad/glad.h>

class Cubemap {
public:
    GLuint ID = 0;
    int size = 0;

    Cubemap(int resolution);
    void Bind(GLuint unit = 0) const;
};
