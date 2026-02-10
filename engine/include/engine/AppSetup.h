#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <string>
class Camera;

// Initializes GLFW + creates window + sets context
GLFWwindow* initWindow(int width, int height, const std::string& title);
// Loads GLAD + sets default OpenGL state
bool setupOpenGL();
// cleanup GLFW resources
void shutdownWindow(GLFWwindow* window);

// Camera wiring
void setupCamera(GLFWwindow* window, Camera& camera);

// ImGui lifecycle
void initImGui(GLFWwindow* window);
void shutdownImGui();