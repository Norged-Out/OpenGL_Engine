#include "engine/Mesh.h"
#include "engine/Shader.h"
#include <glm/gtc/matrix_transform.hpp>
#include <string>

// Constructor that generates a Mesh, need to initialze vbo and ebo
Mesh::Mesh(const std::vector <Vertex>& vert, 
			const std::vector <GLuint>& inds, 
			const std::vector<std::shared_ptr<Texture>>& texs)
	: vertices(vert), indices(inds), textures(texs), vbo(vertices), ebo(indices) {
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

	// unbind to prevent accidental modification
	vao.Unbind(); vbo.Unbind(); ebo.Unbind();
}

void Mesh::setModelMatrix(const glm::mat4& m) {
	modelMatrix = m;
}

const glm::mat4& Mesh::getModelMatrix() const {
	return modelMatrix;
}

void Mesh::resetTransform() {
	modelMatrix = glm::mat4(1.0f);
}

void Mesh::setPosition(const glm::vec3& pos) {
	modelMatrix = glm::translate(glm::mat4(1.0f), pos);
}

void Mesh::setRotation(float angle, const glm::vec3& axis) {
	modelMatrix = glm::rotate(modelMatrix, glm::radians(angle), axis);
}

void Mesh::setScale(const glm::vec3& scale) {
	modelMatrix = glm::scale(modelMatrix, scale);
}

void Mesh::Draw(Shader& shader) {
	// Keep track of how many of each type of textures we have
	unsigned int numDiffuse = 0;
	unsigned int numSpecular = 0;

	// bind textures in order
	for (unsigned int i = 0; i < textures.size(); i++) {
		std::string num;
		std::string type = textures[i]->type;
		if (type == "diffuse") {
			num = std::to_string(numDiffuse++);
		}
		else if (type == "specular") {
			num = std::to_string(numSpecular++);
		}
		textures[i]->texUnit(shader, (type + num).c_str(), i);
		textures[i]->Bind();
	}

	// Draw the actual mesh
	vao.Bind();
	glDrawElements(drawMode, indices.size(), GL_UNSIGNED_INT, 0);
	vao.Unbind();

}