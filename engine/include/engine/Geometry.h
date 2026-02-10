#pragma once

#include <memory>
#include <vector>
#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "engine/Mesh.h"
class Texture;

class Geometry {
public:

    // -------- Mesh generation --------

    // Creates a unit sphere mesh (positions, normals, UVs, tangents, indices)
    std::unique_ptr<Mesh> createSphereMesh(int stacks = 16, int slices = 24);

    // -------- High-level helpers --------

    // Creates a sphere and optionally attaches textures
    // If applyTextures == false, returns a pure geometry mesh
    std::unique_ptr<Mesh> createTexturedSphere(bool applyTextures, 
        const std::string& texPath = "", int stacks = 16, int slices = 24);

    // -------- Tangent utilities --------

    // Generic tangent generator (reusable for any indexed mesh)
    void GenerateTangents(std::vector<Vertex>& vertices, const std::vector<GLuint>& indices);
};