// ==================================================
// Author: Priyansh Nayak
// Description: Vertex buffer wrapper used by engine mesh data
// ==================================================

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
	// Params: const void* data, size_t sizeBytes, GLenum usage = GL_STATIC_DRAW
	VBO(const void* data, size_t sizeBytes, GLenum usage = GL_STATIC_DRAW);
	// Destructor
	~VBO() {
		if (ID != 0) Delete();
	}

	// Prevent copying
	VBO(const VBO&) = delete;
	VBO& operator=(const VBO&) = delete;

	// Binds the VBO
	// Params: none
	void Bind();
	// Unbinds the VBO
	// Params: none
	void Unbind();
	// Deletes the VBO
	// Params: none
	void Delete();
};
