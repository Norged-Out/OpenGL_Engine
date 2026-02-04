#include "engine/HDRConverter.h"
#include "engine/Shader.h"
#include "engine/HDRTexture.h"
#include "engine/Cubemap.h"
#include <glm/gtc/matrix_transform.hpp>

#ifndef ENGINE_SHADER_DIR
#error ENGINE_SHADER_DIR not defined
#endif

HDRConverter::HDRConverter(int cubemapSize) : size(cubemapSize) {
	shader = new Shader(
		(std::string(ENGINE_SHADER_DIR) + "hdr2cubemap.vert").c_str(),
		(std::string(ENGINE_SHADER_DIR) + "hdr2cubemap.frag").c_str()
	);
	initFramebuffer();
	initMatrices();
}

HDRConverter::~HDRConverter() {
	if (shader) delete shader;
	if (fbo) glDeleteFramebuffers(1, &fbo);
	if (rbo) glDeleteRenderbuffers(1, &rbo);
}

void HDRConverter::initFramebuffer() {
	// Create framebuffer
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	// Create renderbuffer for depth testing
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	// Depth buffer for rendering
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, size, size);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);
	// Unbind
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void HDRConverter::initMatrices() {
	// 90-degree fov
	projection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	// View matrices for the 6 cube directions
	views = {
		glm::lookAt(glm::vec3(0,0,0), glm::vec3(1, 0, 0), glm::vec3(0,-1, 0)), // +X
		glm::lookAt(glm::vec3(0,0,0), glm::vec3(-1, 0, 0), glm::vec3(0,-1, 0)), // -X
		glm::lookAt(glm::vec3(0,0,0), glm::vec3(0, 1, 0), glm::vec3(0, 0, 1)), // +Y
		glm::lookAt(glm::vec3(0,0,0), glm::vec3(0,-1, 0), glm::vec3(0, 0,-1)), // -Y
		glm::lookAt(glm::vec3(0,0,0), glm::vec3(0, 0, 1), glm::vec3(0,-1, 0)), // +Z
		glm::lookAt(glm::vec3(0,0,0), glm::vec3(0, 0,-1), glm::vec3(0,-1, 0))  // -Z
	};
}

static void renderCube() {
	static GLuint cubeVAO = 0;
	static GLuint cubeVBO = 0;

	if (cubeVAO == 0){
		// 36 vertices, position only
		float vertices[] = {
			// back face
			 -1,-1,-1,  1, 1,-1,  1,-1,-1,
			 1, 1,-1, -1,-1,-1, -1, 1,-1,
			// front face
			-1,-1, 1,  1,-1, 1,  1, 1, 1,
			 1, 1, 1, -1, 1, 1, -1,-1, 1,
			// left face
			-1, 1, 1, -1, 1,-1, -1,-1,-1,
			-1,-1,-1, -1,-1, 1, -1, 1, 1,
			// right face
			 1, 1, 1,  1,-1,-1,  1, 1,-1,
			 1,-1,-1,  1, 1, 1,  1,-1, 1,
			// bottom face
			-1,-1,-1,  1,-1,-1,  1,-1, 1,
			 1,-1, 1, -1,-1, 1, -1,-1,-1,
			// top face
			-1, 1,-1,  1, 1, 1,  1, 1,-1,
			 1, 1, 1, -1, 1,-1, -1, 1, 1
		};

		glGenVertexArrays(1, &cubeVAO);
		glGenBuffers(1, &cubeVBO);

		glBindVertexArray(cubeVAO);
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		// Only position attribute
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	}

	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

void HDRConverter::convert(const HDRTexture& src, Cubemap& dst) {
	// Render into cubemap resolution
	GLint prevViewport[4];
	glGetIntegerv(GL_VIEWPORT, prevViewport);
	glViewport(0, 0, size, size);
	// Bind framebuffer to render offscreen
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	GLboolean wasCulling = glIsEnabled(GL_CULL_FACE);
	glDisable(GL_CULL_FACE);
	// Bind shader and set uniforms
	shader->Activate();
	shader->setInt("eqrMap", 0);
	shader->setMat4("projection", projection);
	// Bind the source HDR texture
	src.Bind(0);
	// Render once per cubemap face
	for (unsigned i = 0; i < 6; ++i) {
		// Set view matrix for current face
		shader->setMat4("view", views[i]);
		// Attach the corresponding cubemap face as the render target
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, dst.ID, 0);
		// Clear previous contents
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Render cube: each fragment computes a direction vector
		renderCube();
	}
	// Restore default framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	if (wasCulling) glEnable(GL_CULL_FACE);
	else glDisable(GL_CULL_FACE);
	// Generate mipmaps for smoother reflections/refractions
	glBindTexture(GL_TEXTURE_CUBE_MAP, dst.ID);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	// Restore previous state
	glViewport(prevViewport[0], prevViewport[1], prevViewport[2], prevViewport[3]);
}