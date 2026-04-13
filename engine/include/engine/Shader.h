// ==================================================
// Author: Priyansh Nayak
// Description: Shader program wrapper for loading, binding, and uniform updates
// ==================================================

#pragma once

#include <glad/glad.h>
#include <string>
#include <glm/glm.hpp>     // glm::mat4 support
#include <unordered_map>   // cache

// Description: get_file_contents interface
// Params: const std::string& filename
std::string get_file_contents(const std::string& filename);

class Shader {
public:
	// Reference ID of the Shader Program
	GLuint ID;
	// Constructor that build the Shader Program from 2 different shaders
	// Params: const std::string& vertexFile, const std::string& fragmentFile
	Shader(const std::string& vertexFile, const std::string& fragmentFile);

	// Activates the Shader Program
	// Params: none
	void Activate();
	// Deletes the Shader Program
	// Params: none
	void Delete();

	// Uniform helper methods
	// Params: const std::string& name, bool value
	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	// Description: setFloat
	// Params: const std::string& name, float value
	void setFloat(const std::string& name, float value) const;
	void setMat4(const std::string& name, const float* mat) const;
	// Description: setMat4
	// Params: const std::string& name, const glm::mat4& m
	void setMat4(const std::string& name, const glm::mat4& m) const;
	void setVec2(const std::string& name, const glm::vec2 v) const;
	//void setVec3(const std::string& name, float x, float y, float z) const;
	// Description: setVec3
	// Params: const std::string& name, const glm::vec3& v
	void setVec3(const std::string& name, const glm::vec3& v) const;
	//void setVec4(const std::string& name, float x, float y, float z, float w) const;
	// Description: setVec4
	// Params: const std::string& name, const glm::vec4& v
	void setVec4(const std::string& name, const glm::vec4& v) const;

	~Shader() {
		if (ID != 0) Delete();
	}

	// Prevent copying (avoid double-delete)
	Shader(const Shader&) = delete;
	Shader& operator=(const Shader&) = delete;

private:
	// path to shader source files
	std::string vertexPath;
	std::string fragmentPath;
	// cache of uniform locations to reduce calls
	mutable std::unordered_map<std::string, GLint> uniformCache;
	// Description: getUniformLocation
	// Params: const std::string& name
	GLint getUniformLocation(const std::string& name) const;
	// error handler
	// Params: GLuint shader, const std::string& type
	void checkCompileErrors(GLuint shader, const std::string& type);
};

