#include "engine/VBO.h"

// Constructor that generates a Vertex Buffer Object and links it to vertices
VBO::VBO(const void* data, size_t sizeBytes, GLenum usage) {
	glGenBuffers(1, &ID);
	glBindBuffer(GL_ARRAY_BUFFER, ID);
	glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(sizeBytes), data, usage);
}

// Binds the VBO
void VBO::Bind() {
	glBindBuffer(GL_ARRAY_BUFFER, ID);
}

// Unbinds the VBO
void VBO::Unbind() {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// Deletes the VBO
void VBO::Delete() {
	glDeleteBuffers(1, &ID);
}