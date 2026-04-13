// ==================================================
// Author: Priyansh Nayak
// Description: Mesh wrapper that owns geometry buffers and material state
// ==================================================

#pragma once

#include <vector> 
#include <memory>
#include <glad/glad.h> 
#include <glm/glm.hpp> 
#include "engine/VBO.h"
#include "engine/VAO.h"
#include "engine/EBO.h"
#include "engine/Material.h"
class Shader;

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 color;
	glm::vec2 texUV;
	glm::vec4 tangent; // xyz = tangent, w = handedness
};

class Mesh
{
public:
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::shared_ptr<Material> material; // one material per mesh for simplicity

	// Store model matrix for simple transformations
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	GLenum drawMode = GL_TRIANGLES; // default, but can be changed per mesh

	// Initializes the mesh
	Mesh(const std::vector <Vertex>& vertices,
		 const std::vector <GLuint>& indices,
		 std::shared_ptr<Material> material = nullptr);

	~Mesh() {
		vao.Delete();
		vbo.Delete();
		ebo.Delete();
	}

	// Allows changing the material after creation
	void setMaterial(std::shared_ptr<Material> mat) { material = mat; }

	// Draws the mesh
	// Params: Shader& shader
	void Draw(Shader& shader);

private:
	// to be used by Draw
	VAO vao;
    VBO vbo;
    EBO ebo;
};
