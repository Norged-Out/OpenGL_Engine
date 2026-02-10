#pragma once

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <cstddef>

class VBO
{
public:
	// Reference ID of the Vertex Buffer Object
	GLuint ID;
	// Constructor that generates a Vertex Buffer Object
	VBO(const void* data, size_t sizeBytes, GLenum usage = GL_STATIC_DRAW);
	// Destructor
	~VBO() {
		if (ID != 0) Delete();
	}

	// Prevent copying
	VBO(const VBO&) = delete;
	VBO& operator=(const VBO&) = delete;

	// Binds the VBO
	void Bind();
	// Unbinds the VBO
	void Unbind();
	// Deletes the VBO
	void Delete();
};