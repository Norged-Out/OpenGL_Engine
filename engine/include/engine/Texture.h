// ==================================================
// Author: Priyansh Nayak
// Description: 2D texture wrapper used by materials and model assets
// ==================================================

#pragma once

#include <glad/glad.h>
#include <string>

class Texture
{
public:
	GLuint ID = 0;
	// Constructor for loading from file
	// Params: const char* imagePath, GLenum pixelType
	Texture(const char* imagePath, GLenum pixelType);
	// Constructor for embedded textures loaded from memory
	// Params: const unsigned char* data, size_t size, GLenum pixelType
	Texture(const unsigned char* data, size_t size, GLenum pixelType);

	~Texture() {
		if (ID != 0) Delete();
	}

	// Prevent copying
	Texture(const Texture&) = delete;
	Texture& operator=(const Texture&) = delete;

	// Bind to currently active texture unit
	// Params: none
	void Bind(); 
	// Bind to a specific texture unit
	// Params: GLuint unit
	void Bind(GLuint unit); 
	// Unbinds a texture
	// Params: none
	void Unbind();
	// Deletes a texture
	// Params: none
	void Delete();

	// Setters for texture parameters
	// Params: GLenum newMin, GLenum newMag
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
