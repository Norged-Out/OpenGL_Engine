#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <array>

class Shader;
class HDRTexture;
class Cubemap;

class HDRConverter {
public:
	explicit HDRConverter(int cubemapSize = 512);
	~HDRConverter();
	void convert(const HDRTexture& src, Cubemap& dst);

private:
	Shader* shader = nullptr;
	GLuint fbo = 0;
	GLuint rbo = 0;
	std::array<glm::mat4, 6> views;
	glm::mat4 projection;
	int size = 512;
	void initFramebuffer();
	void initMatrices();
};