// ==================================================
// Author: Priyansh Nayak
// Description: Vertex array wrapper for reusable attribute-state binding
// ==================================================

#pragma once

#include <glad/glad.h>
class VBO;

class VAO
{
public:
	// ID reference for the Vertex Array Object
	GLuint ID;
	// Constructor that generates a VAO ID
	// Params: none
	VAO();
	// Destructor to clean up
	~VAO() {
		if (ID != 0) Delete();
	}
	// Prevent copying
	VAO(const VAO&) = delete;
	VAO& operator=(const VAO&) = delete;

	// Links a VBO to the VAO using a certain layout for float attributes
	// Params: VBO& VBO, GLuint layout, GLint numComponents, GLsizei stride, const void* offset
	void LinkVBO(VBO& VBO, GLuint layout, GLint numComponents, GLsizei stride, const void* offset);

	// Binds the VAO
	// Params: none
	void Bind();
	// Unbinds the VAO
	// Params: none
	void Unbind();
	// Deletes the VAO
	// Params: none
	void Delete();
};
