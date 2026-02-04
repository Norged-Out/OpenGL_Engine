#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum class CamMode { Free, Cinema };

class Camera
{
public:
	// Stores the main vectors of the camera
	glm::vec3 Position;
	glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);
	const glm::vec3 WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);
	glm::mat4 cameraMatrix = glm::mat4(1.0f);

	// Stores the width and height of the window
	int width;
	int height;

	// Adjust the speed of the camera and it's sensitivity when looking around
	float speed = 2.5f;
	float sensitivity = 0.1f;

	// Euler angles (for stable yaw/pitch control)
	float yaw = -90.0f;       // facing -Z by default
	float pitch = 0.0f;

	// Mouse capture state (for cursor disabled mode)
	bool mouseCaptured = false;
	double lastX = 0.0, lastY = 0.0;

	// Zoom settings
	float FOV = 50.0f;
	float minFOV = 25.0f; // clamp
	float maxFOV = 75.0f; // clamp
	float zoomSpeed = 2.0f;
	const float defaultFOV = 50.0f;

	// Orbit settings
	CamMode camMode = CamMode::Free;

	// Save/restore free-cam state when switching modes
	glm::vec3 savedPos{0.0f};
	glm::vec3 savedOrientation{0.0f, 0.0f, -1.0f};

	// Cinema orbit state
	glm::vec3 orbitCenter{0.0f};
	float orbitAngle = 0.0f;     // radians
	float orbitSpeed = 0.25f;    // radians/sec (tweak)
	float orbitRadius = 40.0f;   // will be recomputed from terrain size each frame
	float orbitHeight = 15.0f;   // above mid terrain

	// Camera constructor to set up initial values
	Camera(int width, int height, glm::vec3 position);

	// Updates the camera matrix to the Vertex Shader
	void updateMatrix(float nearPlane, float farPlane);
	// Exports the camera matrix to a shader
	void Matrix(class Shader& shader, const char* uniform) const;
	// Updates stored window size
	void setSize(int newWidth, int newHeight);
	// Call from GLFW scroll callback
	void OnScroll(double yffset);
	// Handles camera inputs
	void Inputs(GLFWwindow* window, float dt);
	// Change camera modes
	void ToggleCinema(const glm::vec3& center);
	void UpdateWithMode(GLFWwindow* window, float dt);
private:
	void updateFromAngles(); // rebuild Orientation & Up from yaw/pitch
};