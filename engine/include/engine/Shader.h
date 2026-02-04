#pragma once

#include <glad/glad.h>
#include <string>
#include <glm/glm.hpp>     // glm::mat4 support
#include <unordered_map>   // cache

std::string get_file_contents(const std::string& filename);

class Shader {
public:
	// Reference ID of the Shader Program
	GLuint ID;
	// Constructor that build the Shader Program from 2 different shaders
	Shader(const std::string& vertexFile, const std::string& fragmentFile);

	// Activates the Shader Program
	void Activate();
	// Deletes the Shader Program
	void Delete();

	// Uniform helper methods
	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void setMat4(const std::string& name, const float* mat) const;
	void setMat4(const std::string& name, const glm::mat4& m) const;
	void setVec2(const std::string& name, const glm::vec2 v) const;
	//void setVec3(const std::string& name, float x, float y, float z) const;
	void setVec3(const std::string& name, const glm::vec3& v) const;
	//void setVec4(const std::string& name, float x, float y, float z, float w) const;
	void setVec4(const std::string& name, const glm::vec4& v) const;

	~Shader() {
		if (ID != 0) Delete();
	}

	// Prevent copying (avoid double-delete)
	Shader(const Shader&) = delete;
	Shader& operator=(const Shader&) = delete;

private:
	// cache of uniform locations to reduce calls
	mutable std::unordered_map<std::string, GLint> uniformCache;
	GLint getUniformLocation(const std::string& name) const;
	// error handler
	void checkCompileErrors(GLuint shader, const std::string& type);
};
