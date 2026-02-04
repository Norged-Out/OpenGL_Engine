#pragma once

#include <glad/glad.h>
class Shader;

class Texture
{
public:
	GLuint ID;
	const char* type;
	GLuint slot;
	Texture(const char* image, const char* texType, GLuint slot, GLenum pixelType);
	// for embedded textures:
	Texture(const unsigned char* data, size_t size, const char* texType, GLuint slot, GLenum pixelType);

	~Texture() {
		if (ID != 0) Delete();
	}

	// Prevent copying
	Texture(const Texture&) = delete;
	Texture& operator=(const Texture&) = delete;

	// Assigns a texture unit to a texture
	void texUnit(Shader& shader, const char* uniform, GLuint unit);
	// Binds a texture
	void Bind();
	// Unbinds a texture
	void Unbind();
	// Deletes a texture
	void Delete();
};