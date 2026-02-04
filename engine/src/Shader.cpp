#include "engine/Shader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cerrno>

// Reads a text file and outputs a string with everything in the text file
std::string get_file_contents(const char* filename) {
	std::ifstream in(filename, std::ios::binary);
	if (in)	{
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return(contents);
	}
	throw(errno);
}

// Constructor that build the Shader Program from 2 different shaders
Shader::Shader(const char* vertexFile, const char* fragmentFile) {
	// Read vertexFile and fragmentFile and store the strings
	std::string vertexCode = get_file_contents(vertexFile);
	std::string fragmentCode = get_file_contents(fragmentFile);

	// Convert the shader source strings into character arrays
	const char* vertexSource = vertexCode.c_str();
	const char* fragmentSource = fragmentCode.c_str();

	// Compile Vertex Shader

	// Create Vertex Shader Object and get its reference
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	// Attach Vertex Shader source to the Vertex Shader Object
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	// Compile the Vertex Shader into machine code
	glCompileShader(vertexShader);
	checkCompileErrors(vertexShader, "VERTEX");

	// Compile Fragment Shader

	// Create Fragment Shader Object and get its reference
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	// Attach Fragment Shader source to the Fragment Shader Object
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	// Compile the Vertex Shader into machine code
	glCompileShader(fragmentShader);
	checkCompileErrors(fragmentShader, "FRAGMENT");

	// Link Shaders

	// Create Shader Program Object and get its reference
	ID = glCreateProgram();
	// Attach the Vertex and Fragment Shaders to the Shader Program
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);
	// Wrap-up/Link all the shaders together into the Shader Program
	glLinkProgram(ID);
	checkCompileErrors(ID, "PROGRAM");

	// Delete the now useless Vertex and Fragment Shader objects
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

}

// Activates the Shader Program
void Shader::Activate() {
	glUseProgram(ID);
}

// Deletes the Shader Program
void Shader::Delete() {
	glDeleteProgram(ID);
}

// Uniform Helper Functions

GLint Shader::getUniformLocation(const std::string& name) const {
	auto it = uniformCache.find(name);
	if (it != uniformCache.end()) return it->second;

	GLint loc = glGetUniformLocation(ID, name.c_str());
	// Cache even if -1 (lets us skip repeated GL calls)
	uniformCache[name] = loc;
	return loc;
}

void Shader::setBool(const std::string& name, bool value) const {
	glUniform1i(getUniformLocation(name), (int)value);
}

void Shader::setInt(const std::string& name, int value) const {
	glUniform1i(getUniformLocation(name), value);
}

void Shader::setFloat(const std::string& name, float value) const {
	glUniform1f(getUniformLocation(name), value);
}

void Shader::setMat4(const std::string& name, const float* mat) const {
	glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, mat);
}

void Shader::setMat4(const std::string& name, const glm::mat4& m) const {
	glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &m[0][0]);
}

void Shader::setVec2(const std::string& name, const glm::vec2 v) const {
	glUniform2f(getUniformLocation(name), v.x, v.y);
}

//void Shader::setVec3(const std::string& name, float x, float y, float z) const {
//	glUniform3f(getUniformLocation(name), x, y, z);
//}

void Shader::setVec3(const std::string& name, const glm::vec3& v) const {
	glUniform3f(getUniformLocation(name), v.x, v.y, v.z);
}

//void Shader::setVec4(const std::string& name, float x, float y, float z, float w) const {
//	glUniform4f(getUniformLocation(name), x, y, z, w);
//}

void Shader::setVec4(const std::string& name, const glm::vec4& v) const {
	glUniform4f(getUniformLocation(name), v.x, v.y, v.z, v.w);
}


// Error Handling

void Shader::checkCompileErrors(GLuint shader, const std::string& type) {
	GLint success;
	GLchar infoLog[1024];

	if (type != "PROGRAM") {
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cerr << "SHADER COMPILATION ERROR (" << type << "):\n"
				<< infoLog << std::endl;
		}
	}
	else {
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cerr << "PROGRAM LINKING ERROR:\n"
				<< infoLog << std::endl;
		}
	}
}