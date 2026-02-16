#pragma once

#include <memory>
#include <vector>
#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "engine/Mesh.h"
class Texture;

namespace Geometry {
    // -------- Mesh generation --------

    // Creates a simple cube mesh (positions, indices, normals, UVs, tangents)
    std::unique_ptr<Mesh> createCubeMesh(bool genTangents = true);

    // Creates a simple pyramid mesh (positions, indices, normals, UVs, tangents)
    std::unique_ptr<Mesh> createPyramidMesh(bool genTangents = true);

    // Creates a unit sphere mesh (positions, indices, normals, UVs, tangents)
    std::unique_ptr<Mesh> createSphereMesh(int stacks = 16, int slices = 24, bool genTangents = true);

    // Creates a Donut-shaped ring mesh (positions, indices, normals, UVs, tangents)
    std::unique_ptr<Mesh> createRingMesh(bool genTangents = true);    

    // Creates a simple plane mesh
    std::unique_ptr<Mesh> createPlaneMesh(float width = 1.0f, float height = 1.0f, 
        float uvScale = 1.0f, bool genTangents = true);

    // -------- Tangent utilities --------

    // Generic tangent generator (reusable for any indexed mesh)
    void generateTangents(std::vector<Vertex>& vertices, const std::vector<GLuint>& indices);
}