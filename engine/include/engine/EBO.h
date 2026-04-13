// ==================================================
// Author: Priyansh Nayak
// Description: Element buffer wrapper used for indexed mesh drawing
// ==================================================

#pragma once

#include <glad/glad.h>
#include <vector>

class EBO
{
public:
	// ID reference of Elements Buffer Object
	GLuint ID;
	// Constructor that generates a Elements Buffer Object and links it to indices
	// Params: const std::vector<GLuint>& indices
	EBO(const std::vector<GLuint>& indices);
	// Destructor
	~EBO() {
		if (ID != 0) Delete();
	}

	// Prevent copying
	EBO(const EBO&) = delete;
	EBO& operator=(const EBO&) = delete;

	// Binds the EBO
	// Params: none
	void Bind();
	// Unbinds the EBO
	// Params: none
	void Unbind();
	// Deletes the EBO
	// Params: none
	void Delete();
};
