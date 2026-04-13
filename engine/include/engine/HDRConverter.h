// ==================================================
// Author: Priyansh Nayak
// Description: HDR-to-cubemap conversion interface for environment lighting
// ==================================================

#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <array>

class Shader;
class HDRTexture;
class Cubemap;

class HDRConverter {
public:
	// Description: HDRConverter
	// Params: int cubemapSize = 512
	explicit HDRConverter(int cubemapSize = 512);
	~HDRConverter();
	// Description: convert
	// Params: const HDRTexture& src, Cubemap& dst
	void convert(const HDRTexture& src, Cubemap& dst);

private:
	Shader* shader = nullptr;
	GLuint fbo = 0;
	GLuint rbo = 0;
	std::array<glm::mat4, 6> views;
	glm::mat4 projection;
	int size = 512;
	// Description: initFramebuffer
	// Params: none
	void initFramebuffer();
	void initMatrices();
};
