#pragma once

#include <glad/glad.h>
#include <string>

class Texture
{
public:
	GLuint ID = 0;
	// Constructor for loading from file
	Texture(const char* imagePath, GLenum pixelType);
	// Constructor for embedded textures loaded from memory
	Texture(const unsigned char* data, size_t size, GLenum pixelType);

	~Texture() {
		if (ID != 0) Delete();
	}

	// Prevent copying
	Texture(const Texture&) = delete;
	Texture& operator=(const Texture&) = delete;

	// Bind to currently active texture unit
	void Bind(); 
	// Bind to a specific texture unit
	void Bind(GLuint unit); 
	// Unbinds a texture
	void Unbind();
	// Deletes a texture
	void Delete();

	// Setters for texture parameters
	void setFiltering(GLenum newMin, GLenum newMag);
	void setWrapping(GLenum newWrapS, GLenum newWrapT);

    // Optional getters
    GLenum getMinFilter() const { return minFilter; }
    GLenum getMagFilter() const { return magFilter; }
    GLenum getWrapS() const { return wrapS; }
    GLenum getWrapT() const { return wrapT; }

private:
	GLenum minFilter = GL_LINEAR_MIPMAP_LINEAR;
	GLenum magFilter = GL_LINEAR;
	GLenum wrapS = GL_REPEAT;
	GLenum wrapT = GL_REPEAT;
};