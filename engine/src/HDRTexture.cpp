#include "engine/HDRTexture.h"
#include <iostream>
#include <stb_image.h>
#include <filesystem>


HDRTexture::HDRTexture(const std::string& path) {
	// Load the HDR image data from file
	std::cout << "[HDRTexture] trying path: " << path << "\n";
	std::cout << "[HDRTexture] cwd: " << std::filesystem::current_path().string() << "\n";
	// Flips the image so it appears right side up
	int width, height, channels;
	stbi_set_flip_vertically_on_load(true);
	float* data = stbi_loadf(path.c_str(), &width, &height, &channels, 0);
	if (!data) {
		std::cerr << "Failed to load HDR texture: " << path << std::endl;
		std::cerr << "stbi reason: " << stbi_failure_reason() << "\n";
		ID = 0;
		return;
	}

	
	// Generates an OpenGL texture object
	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_2D, ID);

	// Configures the type of algorithm that is used to make the image smaller or bigger
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Configures the way the texture repeats
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Upload the HDR image data to the texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);

	// Free the image data
	stbi_image_free(data);
	std::cout << "[HDRTexture] Loaded " << path << "\n";
}

void HDRTexture::Bind(GLuint unit) const {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, ID);
}