#pragma once

#include <glad/glad.h>
#include <string>
class Shader;

// TODO: Texture should not own slot or semantic.
//       Move binding policy to Material/Mesh.
enum class TextureSlot {
    Diffuse  = 0,
    Specular = 1,
    Normal   = 2,
    Roughness= 3,
    Metallic = 4,
    AO       = 5
};

class Texture
{
public:
	GLuint ID;
	GLuint slot;
	Texture(const char* image, GLuint slot, GLenum pixelType);
	// for embedded textures:
	Texture(const unsigned char* data, size_t size, GLuint slot, GLenum pixelType);

	~Texture() {
		if (ID != 0) Delete();
	}

	// Prevent copying
	Texture(const Texture&) = delete;
	Texture& operator=(const Texture&) = delete;

	// Assigns a texture unit to a texture
	void texUnit(Shader& shader, const char* uniform, GLuint unit);
	// Binds a texture
	void Bind(); // Bind to existing texture unit
	void Bind(GLuint unit); // Bind to a specific texture unit
	// Unbinds a texture
	void Unbind();
	// Deletes a texture
	void Delete();

	// Helper function to map texture semantic to fixed slot + uniform
	static inline bool getTextureSlot(const std::string& type, GLuint& outSlot,
		std::string& uniform){
		if (type == "diffuse")   { outSlot = 0; uniform = "diffuse0"; }
		else if (type == "specular") { outSlot = 1; uniform = "specular0"; }
		else if (type == "normal")   { outSlot = 2; uniform = "normal0"; }
		else if (type == "roughness"){ outSlot = 3; uniform = "roughness0"; }
		else if (type == "metallic") { outSlot = 4; uniform = "metallic0"; }
		else if (type == "ao")       { outSlot = 5; uniform = "ao0"; }
		else return false;
		return true;
	}
};