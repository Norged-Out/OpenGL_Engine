#include "engine/Geometry.h"
#include "engine/Texture.h"
#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>

std::unique_ptr<Mesh> Geometry::createCubeMesh(bool genTangents) {
    std::vector<Vertex> vertices = {
        // +Z (Front)
        {{-0.5f,-0.5f, 0.5f},{0,0,1},{1,1,1},{0,0}},
        {{ 0.5f,-0.5f, 0.5f},{0,0,1},{1,1,1},{1,0}},
        {{ 0.5f, 0.5f, 0.5f},{0,0,1},{1,1,1},{1,1}},
        {{-0.5f, 0.5f, 0.5f},{0,0,1},{1,1,1},{0,1}},

        // -Z (Back)
        {{ 0.5f,-0.5f,-0.5f},{0,0,-1},{1,1,1},{0,0}},
        {{-0.5f,-0.5f,-0.5f},{0,0,-1},{1,1,1},{1,0}},
        {{-0.5f, 0.5f,-0.5f},{0,0,-1},{1,1,1},{1,1}},
        {{ 0.5f, 0.5f,-0.5f},{0,0,-1},{1,1,1},{0,1}},

        // +X (Right) 
        {{ 0.5f,-0.5f, 0.5f},{1,0,0},{1,1,1},{0,0}},
        {{ 0.5f,-0.5f,-0.5f},{1,0,0},{1,1,1},{1,0}},
        {{ 0.5f, 0.5f,-0.5f},{1,0,0},{1,1,1},{1,1}},
        {{ 0.5f, 0.5f, 0.5f},{1,0,0},{1,1,1},{0,1}},

        // -X (Left) 
        {{-0.5f,-0.5f,-0.5f},{-1,0,0},{1,1,1},{0,0}},
        {{-0.5f,-0.5f, 0.5f},{-1,0,0},{1,1,1},{1,0}},
        {{-0.5f, 0.5f, 0.5f},{-1,0,0},{1,1,1},{1,1}},
        {{-0.5f, 0.5f,-0.5f},{-1,0,0},{1,1,1},{0,1}},

        // +Y (Top)
        {{-0.5f, 0.5f, 0.5f},{0,1,0},{1,1,1},{0,0}},
        {{ 0.5f, 0.5f, 0.5f},{0,1,0},{1,1,1},{1,0}},
        {{ 0.5f, 0.5f,-0.5f},{0,1,0},{1,1,1},{1,1}},
        {{-0.5f, 0.5f,-0.5f},{0,1,0},{1,1,1},{0,1}},

        // -Y (Bottom)
        {{-0.5f,-0.5f,-0.5f},{0,-1,0},{1,1,1},{0,0}},
        {{ 0.5f,-0.5f,-0.5f},{0,-1,0},{1,1,1},{1,0}},
        {{ 0.5f,-0.5f, 0.5f},{0,-1,0},{1,1,1},{1,1}},
        {{-0.5f,-0.5f, 0.5f},{0,-1,0},{1,1,1},{0,1}},
    };

    std::vector<GLuint> indices;

    for (int i = 0; i < 6; ++i){
        int start = i * 4;

        indices.push_back(start + 0);
        indices.push_back(start + 1);
        indices.push_back(start + 2);

        indices.push_back(start + 0);
        indices.push_back(start + 2);
        indices.push_back(start + 3);
    }

    if (genTangents) Geometry::generateTangents(vertices, indices);

    std::cout << "[Cube] Vertices: " << vertices.size()
              << " | Indices: " << indices.size() << std::endl;

    return std::make_unique<Mesh>(vertices, indices, nullptr);
}

std::unique_ptr<Mesh> Geometry::createPyramidMesh(bool genTangents) {
    // full vertex setup
    std::vector<Vertex> vertices = {
        // base
        {{-0.5f, -0.5f, -0.5f}, {0,-1,0}, {1,1,1}, {0,0}},
        {{ 0.5f, -0.5f, -0.5f}, {0,-1,0}, {1,1,1}, {1,0}},
        {{ 0.5f, -0.5f,  0.5f}, {0,-1,0}, {1,1,1}, {1,1}},
        {{-0.5f, -0.5f,  0.5f}, {0,-1,0}, {1,1,1}, {0,1}},

        // front face (+Z)
        {{-0.5f,-0.5f, 0.5f}, {0,0.5f,0.9f}, {1,1,1}, {0,0}},
        {{ 0.5f,-0.5f, 0.5f}, {0,0.5f,0.9f}, {1,1,1}, {1,0}},
        {{ 0.0f, 0.5f, 0.0f}, {0,0.5f,0.9f}, {1,1,1}, {0.5f,1}},

        // right face (+X)
        {{ 0.5f,-0.5f, 0.5f}, {0.9f,0.5f,0}, {1,1,1}, {0,0}},
        {{ 0.5f,-0.5f,-0.5f}, {0.9f,0.5f,0}, {1,1,1}, {1,0}},
        {{ 0.0f, 0.5f, 0.0f}, {0.9f,0.5f,0}, {1,1,1}, {0.5f,1}},

        // back face (-Z)
        {{ 0.5f,-0.5f,-0.5f}, {0,0.5f,-0.9f}, {1,1,1}, {0,0}},
        {{-0.5f,-0.5f,-0.5f}, {0,0.5f,-0.9f}, {1,1,1}, {1,0}},
        {{ 0.0f, 0.5f, 0.0f}, {0,0.5f,-0.9f}, {1,1,1}, {0.5f,1}},

        // left face (-X)
        {{-0.5f,-0.5f,-0.5f}, {-0.9f,0.5f,0}, {1,1,1}, {0,0}},
        {{-0.5f,-0.5f, 0.5f}, {-0.9f,0.5f,0}, {1,1,1}, {1,0}},
        {{ 0.0f, 0.5f, 0.0f}, {-0.9f,0.5f,0}, {1,1,1}, {0.5f,1}},
    };

    // for EBO
    std::vector<GLuint> indices = {
        0,1,2, 0,2,3,      // base
        4,5,6,             // front
        7,8,9,             // right
        10,11,12,          // back
        13,14,15           // left
    };

    if (genTangents) Geometry::generateTangents(vertices, indices);
    std::cout << "[Pyramid] Vertices: " << vertices.size()
          << " | Indices: " << indices.size() << std::endl;
    return std::make_unique<Mesh>(vertices, indices, nullptr);
}

std::unique_ptr<Mesh> Geometry::createSphereMesh(int stacks, int slices, bool genTangents) {
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

    if (genTangents) Geometry::generateTangents(vertices, indices);
    std::cout << "[Sphere] Vertices: " << vertices.size()
          << " | Indices: " << indices.size() << std::endl;

    return std::make_unique<Mesh>(vertices, indices, nullptr);
}

std::unique_ptr<Mesh> Geometry::createRingMesh(bool genTangents) {
    std::vector<Vertex> vertices;
    std::vector<GLuint>  indices;
    float outerRadius = 1.7f;
    float innerRadius = 1.5f;
    float height = 0.4f;      // how thick the ring is
    int segments = 64;         // how smooth the circle is
    float uvScale = 2.0f;

    // generate top and bottom ring faces
    for (int i = 0; i <= segments; ++i) {
        float angle = (float)i / segments * 2.0f * M_PI;
        float c = cosf(angle);
        float s = sinf(angle);

        glm::vec3 normalUp(0.0f, 1.0f, 0.0f);
        glm::vec3 normalDown(0.0f, -1.0f, 0.0f);

        float bevel = 0.05f; // how curved the edges appear

        // top outer
        vertices.push_back({
            glm::vec3(c * (outerRadius - bevel), +height * 0.5f - bevel, s * (outerRadius - bevel)),
            glm::normalize(glm::vec3(c, 0.7f, s)),
            glm::vec3(1.0f),
            glm::vec2(c, s) * uvScale
            });

        // top inner
        vertices.push_back({
            glm::vec3(c * (innerRadius + bevel), +height * 0.5f - bevel, s * (innerRadius + bevel)),
            glm::normalize(glm::vec3(-c, 0.7f, -s)),
            glm::vec3(1.0f),
            glm::vec2(c, s) * uvScale
            });

        // bottom outer
        vertices.push_back({
            glm::vec3(c * (outerRadius - bevel), -height * 0.5f + bevel, s * (outerRadius - bevel)),
            glm::normalize(glm::vec3(c, -0.7f, s)),
            glm::vec3(1.0f),
            glm::vec2(c, s) * uvScale
            });

        // bottom inner
        vertices.push_back({
            glm::vec3(c * (innerRadius + bevel), -height * 0.5f + bevel, s * (innerRadius + bevel)),
            glm::normalize(glm::vec3(-c, -0.7f, -s)),
            glm::vec3(1.0f),
            glm::vec2(c, s) * uvScale
            });
    }

    // connect quads around ring sides
    int ringStride = 4;
    for (int i = 0; i < segments; ++i) {
        int i0 = i * ringStride;
        int i1 = (i + 1) * ringStride;

        // top face
        indices.push_back((GLuint)i0);
        indices.push_back((GLuint)(i0 + 1));
        indices.push_back((GLuint)(i1 + 1));
        indices.push_back((GLuint)i0);
        indices.push_back((GLuint)(i1 + 1));
        indices.push_back((GLuint)i1);

        // bottom face
        indices.push_back((GLuint)i0 + 2);
        indices.push_back((GLuint)(i1 + 3));
        indices.push_back((GLuint)(i0 + 3));
        indices.push_back((GLuint)i0 + 2);
        indices.push_back((GLuint)(i1 + 2));
        indices.push_back((GLuint)i1 + 3);

        // outer wall
        indices.push_back((GLuint)i0);
        indices.push_back((GLuint)(i1));
        indices.push_back((GLuint)(i1 + 2));
        indices.push_back((GLuint)i0);
        indices.push_back((GLuint)(i1 + 2));
        indices.push_back((GLuint)i0 + 2);

        // inner wall
        indices.push_back((GLuint)i0 + 1);
        indices.push_back((GLuint)(i0 + 3));
        indices.push_back((GLuint)(i1 + 3));
        indices.push_back((GLuint)i0 + 1);
        indices.push_back((GLuint)(i1 + 3));
        indices.push_back((GLuint)i1 + 1);
    }
   
    if (genTangents) Geometry::generateTangents(vertices, indices);
    std::cout << "[Ring] Vertices: " << vertices.size()
          << " | Indices: " << indices.size() << std::endl;

    return std::make_unique<Mesh>(vertices, indices, nullptr);
}

std::unique_ptr<Mesh> Geometry::createPlaneMesh(float width, float depth,
    float uvScale, bool genTangents) {
    float hw = width * 0.5f;
    float hd = depth * 0.5f;

    std::vector<Vertex> vertices = {

        // Bottom-left
        {{-hw, 0.0f, -hd}, {0,1,0}, {1,1,1}, {0.0f,      0.0f}},

        // Bottom-right
        {{ hw, 0.0f, -hd}, {0,1,0}, {1,1,1}, {uvScale,   0.0f}},

        // Top-right
        {{ hw, 0.0f,  hd}, {0,1,0}, {1,1,1}, {uvScale,   uvScale}},

        // Top-left
        {{-hw, 0.0f,  hd}, {0,1,0}, {1,1,1}, {0.0f,      uvScale}},
    };

    std::vector<GLuint> indices = {
        0, 2, 1,
        0, 3, 2
    };

    if (genTangents) Geometry::generateTangents(vertices, indices);

    std::cout << "[Plane] Vertices: " << vertices.size()
              << " | Indices: " << indices.size() << std::endl;

    return std::make_unique<Mesh>(vertices, indices, nullptr);
}


void Geometry::generateTangents(std::vector<Vertex>& vertices,
    const std::vector<GLuint>& indices) {
    std::cout << "[TangentGen] Generating tangents for " << vertices.size() << " vertices and "
              << indices.size() / 3 << " triangles..." << std::endl;
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