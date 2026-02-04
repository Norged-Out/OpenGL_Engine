#include "engine/VAO.h"
#include "engine/VBO.h"

// Constructor that generates a VAO ID
VAO::VAO() {
	glGenVertexArrays(1, &ID);
}

// Links a VBO Attribute to the VAO using a certain layout for float attributes
void VAO::LinkVBO(VBO& VBO, GLuint layout, GLint numComponents, GLsizei stride, const void* offset) {
	VBO.Bind();
	glVertexAttribPointer(layout, numComponents, GL_FLOAT, GL_FALSE, stride, offset);
	glEnableVertexAttribArray(layout);
	VBO.Unbind();
}

// Binds the VAO
void VAO::Bind() {
	glBindVertexArray(ID);
}

// Unbinds the VAO
void VAO::Unbind() {
	glBindVertexArray(0);
}

// Deletes the VAO
void VAO::Delete() {
	glDeleteVertexArrays(1, &ID);
}