// ==================================================
// Author: Priyansh Nayak
// Description: Mesh draw logic, attribute setup, and material binding
// ==================================================

#include "engine/Mesh.h"
#include "engine/Shader.h"
#include <glm/gtc/matrix_transform.hpp>
#include <string>

// Constructor that generates a Mesh, need to initialze vbo and ebo
Mesh::Mesh(const std::vector <Vertex>& verts, 
			const std::vector <GLuint>& inds, 
			std::shared_ptr<Material> mat)
	: vertices(verts), indices(inds), material(mat),
	vbo(vertices.data(), vertices.size() * sizeof(Vertex)), ebo(indices) {
	// bind vao since default constructor is already called
	vao.Bind();
	ebo.Bind(); // sync with vao

	// link vertex positions (3 floats)
	vao.LinkVBO(vbo, 0, 3, sizeof(Vertex), (void*)0);
	// link normals (3 floats, start after first 3)
	vao.LinkVBO(vbo, 1, 3, sizeof(Vertex), (void*)(3 * sizeof(float)));
	// link vertex colors (3 floats, start after first 6)
	vao.LinkVBO(vbo, 2, 3, sizeof(Vertex), (void*)(6 * sizeof(float)));
	// link texture coordinates (2 floats, start after first 9)
	vao.LinkVBO(vbo, 3, 2, sizeof(Vertex), (void*)(9 * sizeof(float)));
	// link tangent (4 floats, start after first 11)
	vao.LinkVBO(vbo, 4, 4, sizeof(Vertex), (void*)(11 * sizeof(float)));

	// unbind to prevent accidental modification
	vao.Unbind(); vbo.Unbind(); ebo.Unbind();
}

void Mesh::Draw(Shader& shader) {
    // Ask material to bind all textures
    if (material) material->bind(shader);

    // Draw the actual mesh
    vao.Bind();
    glDrawElements(drawMode,
                   static_cast<GLsizei>(indices.size()),
                   GL_UNSIGNED_INT,
                   0);
    vao.Unbind();
}
