#include "engine/Camera.h"
#include "engine/Shader.h"

Camera::Camera(int width, int height, glm::vec3 position) {
	Camera::width = width;
	Camera::height = height;
	Position = position;
}

void Camera::updateMatrix(float nearPlane, float farPlane) {
	// Makes camera look in the right direction from the right position
	view = glm::lookAt(Position, Position + Orientation, Up);
	// Adds perspective to the scene
	projection = glm::mat4(1.0f) = glm::perspective(
		glm::radians(FOV),
		(float)width / (float)height,
		nearPlane, farPlane);

	// Sets new camera matrix
	cameraMatrix = projection * view;
}

void Camera::Matrix(Shader& shader, const char* uniform) const {
	// Exports the camera matrix to the Vertex Shader
	shader.setMat4(uniform, cameraMatrix); // uses cached location
}

void Camera::setSize(int newWidth, int newHeight) {
	width = newWidth;
	height = newHeight > 0 ? newHeight : 1; // avoid divide-by-zero
}

void Camera::OnScroll(double yoffset) {
	// + offset = scroll up
	FOV -= static_cast<float>(yoffset) * zoomSpeed;
	if (FOV < minFOV) FOV = minFOV;
	if (FOV > maxFOV)FOV = maxFOV;
}

void Camera::updateFromAngles() {
    // Build forward from yaw/pitch (degrees)
    float cy = cos(glm::radians(yaw));
    float sy = sin(glm::radians(yaw));
    float cp = cos(glm::radians(pitch));
    float sp = sin(glm::radians(pitch));

    glm::vec3 dir;
    dir.x = cy * cp;
    dir.y = sp;
    dir.z = sy * cp;
    Orientation = glm::normalize(dir);

    // Recompute Up from WorldUp to keep an orthonormal basis
    glm::vec3 right = glm::normalize(glm::cross(Orientation, WorldUp));
    Up = glm::normalize(glm::cross(right, Orientation));
}

void Camera::Inputs(GLFWwindow* window, float dt) {

	// Sprint multiplier
	float mult = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) ? 2.5f : 1.0f;
	float vel = speed * mult * dt;

	// Movement with normalized diagonal
	glm::vec3 right = glm::normalize(glm::cross(Orientation, Up));
	glm::vec3 move(0.0f);

	// Handle key inputs
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)       move += Orientation;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)       move -= Orientation;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)       move += right;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)       move -= right;
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)	move += Up;
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)		move -= Up;

	if (glm::length(move) > 0.0f)
		Position += glm::normalize(move) * vel;

	// Reset zoom
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) FOV = defaultFOV;

	// Handles mouse inputs
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
		// Hides mouse cursor
		if (!mouseCaptured) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  // hides + centers internally
			glfwGetCursorPos(window, &lastX, &lastY);                     // prime deltas
			mouseCaptured = true;
		}

		// Stores the coordinates of the cursor
		double mouseX, mouseY;
		// Fetches the coordinates of the cursor
		glfwGetCursorPos(window, &mouseX, &mouseY);
		// Pixels moved
		double dx = mouseX - lastX;
		double dy = mouseY - lastY;
		lastX = mouseX; lastY = mouseY;

		// Accumulate angles
		yaw   -= (float)dx * sensitivity;
		pitch -= (float)dy * sensitivity;
		pitch = glm::clamp(pitch, -85.0f, 85.0f); // prevent flip

		updateFromAngles(); // rebuild Orientation
	}
	else {
		// Unhides cursor since camera is not looking around anymore
		if (mouseCaptured) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			mouseCaptured = false;
		}
	}
}

void Camera::ToggleCinema(const glm::vec3& center) {
	if (camMode == CamMode::Free) {
		// stash free-cam pose
		savedPos = Position;
		savedOrientation = Orientation;

		// set orbit center (x,z) and approximate height from current pos
		orbitCenter = center;

		glm::vec2 toCam(Position.x - center.x, Position.z - center.z);
		orbitRadius = glm::length(toCam);
		if (orbitRadius < 1.0f) orbitRadius = 40.0f; // fallback

		// angle in XZ plane
		orbitAngle = std::atan2(toCam.y, toCam.x);

		// height relative to center
		orbitHeight = Position.y - center.y;

		camMode = CamMode::Cinema;
	}
	else {
		// restore free-cam
		camMode = CamMode::Free;
		Position = savedPos;
		Orientation = savedOrientation;
	}
}

void Camera::UpdateWithMode(GLFWwindow* window, float dt) {
	if (camMode == CamMode::Free) {
		Inputs(window, dt);
		return;
	}

	// Cinema orbit
	orbitAngle += orbitSpeed * dt;

	float camX = orbitCenter.x + orbitRadius * std::cos(orbitAngle);
	float camZ = orbitCenter.z + orbitRadius * std::sin(orbitAngle);
	float camY = orbitCenter.y + orbitHeight;

	Position = glm::vec3(camX, camY, camZ);

	glm::vec3 target = orbitCenter;
	Orientation = glm::normalize(target - Position);
}