#include "engine/Geometry.h"
#include "engine/Texture.h"
#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>


std::unique_ptr<Mesh> Geometry::createSphereMesh(int stacks, int slices) {
    std::vector<Vertex> vertices;
    std::vector<GLuint>  indices;
    vertices.reserve((stacks + 1) * (slices + 1));

    // setup vertices for the sphere
    for (int y = 0; y <= stacks; ++y) {
        float vTex = (float) y / stacks; // vertical texture
        float phi = vTex * (float) M_PI; // latitude 0..PI
        float cp = cosf(phi), sp = sinf(phi);

        for (int x = 0; x <= slices; ++x) {
            float uTex = (float)x / slices; // horizontal texture
            float theta = uTex * 2.0f * (float) M_PI; // longitude 0..2PI
            float ct = cosf(theta), st = sinf(theta);

            // unit sphere parametric form
            glm::vec3 n = glm::vec3(ct * sp, cp, st * sp);
            Vertex vert;
            vert.position = n; // radius 1
            vert.normal = n; // unit normal = position
            vert.color = glm::vec3(1.0f);
            vert.texUV = glm::vec2(uTex, vTex);

            glm::vec3 tangent = glm::normalize(glm::vec3(-st, 0.0f, ct)); // points in direction of increasing u
            vert.tangent = glm::vec4(tangent, 1.0f);

            vertices.push_back(vert);
        }
    }

    // setup indices for the sphere
    for (int y = 0; y < stacks; ++y) {
        for (int x = 0; x < slices; ++x) {
            int a = y * (slices + 1) + x; // current row
            int b = (y + 1) * (slices + 1) + x; // next row
            // two triangles per quad
            indices.push_back(a);            
            indices.push_back(a + 1);
            indices.push_back(b);

            indices.push_back(a + 1);
            indices.push_back(b + 1);
            indices.push_back(b);
        }
    }

    std::vector<std::shared_ptr<Texture>> textures;
    std::cout << "[Sphere] Vertices: " << vertices.size()
          << " | Indices: " << indices.size() << std::endl;

    return std::make_unique<Mesh>(vertices, indices, textures);
}

std::unique_ptr<Mesh> Geometry::createTexturedSphere(bool applyTextures, const std::string& texPath, int stacks, int slices) {
    std::unique_ptr<Mesh> sphereMesh = createSphereMesh(stacks, slices);
    if (!applyTextures) {
        sphereMesh->textures.clear();
        return sphereMesh;
    }

    std::cout << "[Sphere] Loading textures from: " << texPath << std::endl;

    auto diffuse  = std::make_shared<Texture>((texPath + "/diffuse.png").c_str(), "diffuse",  0, GL_UNSIGNED_BYTE);    
    auto normal   = std::make_shared<Texture>((texPath + "/normal.png").c_str(),  "normal",   2, GL_UNSIGNED_BYTE);
    auto roughness = std::make_shared<Texture>((texPath + "/rough.png").c_str(), "roughness", 3, GL_UNSIGNED_BYTE);
    //auto ao       = std::make_shared<Texture>((texPath + "/ao.png").c_str(),         "ao",       5, GL_UNSIGNED_BYTE);

    sphereMesh->textures = { diffuse, normal, roughness  /*, ao*/ };
    std::cout << "[Sphere] Textures bound: " << sphereMesh->textures.size() << std::endl;

    return sphereMesh;
}


void Geometry::generateTangents(std::vector<Vertex>& vertices,
    const std::vector<GLuint>& indices) {
    // Reset tangents
    for (auto& v : vertices) {
        v.tangent = glm::vec4(0.0f);
    }
    // Per-triangle tangent accumulation 
    for (size_t i = 0; i < indices.size(); i += 3) {
        // Fetch the three vertices of the triangle
        Vertex& v0 = vertices[indices[i + 0]];
        Vertex& v1 = vertices[indices[i + 1]];
        Vertex& v2 = vertices[indices[i + 2]];

        // Edges of the triangle in object space
        glm::vec3 e1 = v1.position - v0.position;
        glm::vec3 e2 = v2.position - v0.position;

        // Edges of the triangle in UV space
        glm::vec2 deltaUV1 = v1.texUV - v0.texUV;
        glm::vec2 deltaUV2 = v2.texUV - v0.texUV;

        // Compute determinant of the UV matrix
        float determinant = deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y;
        if (fabs(determinant) < 1e-6f) continue;
        float invDet = 1.0f / determinant;

        // Compute the tangent vector for this triangle
        glm::vec3 tangent = invDet * (deltaUV2.y * e1 - deltaUV1.y * e2);
        // Compute the bitangent as well
        glm::vec3 bitangent = invDet * (-deltaUV2.x * e1 + deltaUV1.x * e2);

        // Accumulate the tangent into each vertex of the triangle
        v0.tangent += glm::vec4(tangent, 0.0f);
        v1.tangent += glm::vec4(tangent, 0.0f);
        v2.tangent += glm::vec4(tangent, 0.0f);

        // Calculate handedness (sign) for normal mapping
        float h0 = glm::dot(glm::cross(v0.normal, tangent), bitangent) < 0.0f ? -1.0f : 1.0f;
        float h1 = glm::dot(glm::cross(v1.normal, tangent), bitangent) < 0.0f ? -1.0f : 1.0f;
        float h2 = glm::dot(glm::cross(v2.normal, tangent), bitangent) < 0.0f ? -1.0f : 1.0f;

        // Accumulate handedness into the w component of the tangent
        v0.tangent.w += h0;
        v1.tangent.w += h1;
        v2.tangent.w += h2;
    }
    // Per-vertex normalization & orthogonalization
    for (auto& v : vertices) {
        // Skip vertices that never received tangent data
        if (glm::length(v.tangent) == 0.0f) continue;
        glm::vec3 t(v.tangent);
        // Gram-Schmidt orthogonalize tangent with respect to normal
        t = glm::normalize(t - v.normal * glm::dot(v.normal, t));
        float handedness = (v.tangent.w < 0.0f) ? -1.0f : 1.0f; // sign
        v.tangent = glm::vec4(t, handedness);
    }
}