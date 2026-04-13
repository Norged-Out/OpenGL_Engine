// ==================================================
// Author: Priyansh Nayak
// Description: Window and OpenGL bootstrap helpers for engine-backed applications
// ==================================================

#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <string>
class Camera;

// Initializes GLFW + creates window + sets context
// Description: initWindow interface
// Params: int width, int height, const std::string& title
GLFWwindow* initWindow(int width, int height, const std::string& title);
// Loads GLAD + sets default OpenGL state
// Description: setupOpenGL interface
// Params: none
bool setupOpenGL();
// cleanup GLFW resources
// Description: shutdownWindow interface
// Params: GLFWwindow* window
void shutdownWindow(GLFWwindow* window);

// Camera wiring
// Description: setupCamera interface
// Params: GLFWwindow* window, Camera& camera
void setupCamera(GLFWwindow* window, Camera& camera);

// ImGui lifecycle
// Description: initImGui interface
// Params: GLFWwindow* window
void initImGui(GLFWwindow* window);
// Description: shutdownImGui interface
// Params: none
void shutdownImGui();
