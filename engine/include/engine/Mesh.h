#pragma once

#include <vector> 
#include <memory>
#include <glad/glad.h> 
#include <glm/glm.hpp> 
#include "engine/VBO.h"
#include "engine/VAO.h"
#include "engine/EBO.h"
#include "engine/Texture.h"
class Shader;

class Mesh
{
public:
	std::vector <Vertex> vertices;
	std::vector <GLuint> indices;
	std::vector<std::shared_ptr<Texture>> textures;
	// Store model matrix for simple transformations
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	GLenum drawMode = GL_TRIANGLES; // default, but can be changed per mesh

	// Initializes the mesh
	Mesh(const std::vector <Vertex>& vertices,
		 const std::vector <GLuint>& indices,
		 const std::vector<std::shared_ptr<Texture>>& textures);

	~Mesh() {
		vao.Delete();
		vbo.Delete();
		ebo.Delete();
	}

	// simple helpers
	void setModelMatrix(const glm::mat4& m);
	const glm::mat4& getModelMatrix() const;
	void resetTransform();
	void setPosition(const glm::vec3& pos);
	void setRotation(float angle, const glm::vec3& axis);
	void setScale(const glm::vec3& scale);

	// Draws the mesh
	void Draw(Shader& shader);

private:
	// to be used by Draw
	VAO vao;
    VBO vbo;
    EBO ebo;
};