#pragma once

#include <glad/glad.h>
#include <vector>

class EBO
{
public:
	// ID reference of Elements Buffer Object
	GLuint ID;
	// Constructor that generates a Elements Buffer Object and links it to indices
	EBO(const std::vector<GLuint>& indices);
	// Destructor
	~EBO() {
		if (ID != 0) Delete();
	}

	// Prevent copying
	EBO(const EBO&) = delete;
	EBO& operator=(const EBO&) = delete;

	// Binds the EBO
	void Bind();
	// Unbinds the EBO
	void Unbind();
	// Deletes the EBO
	void Delete();
};