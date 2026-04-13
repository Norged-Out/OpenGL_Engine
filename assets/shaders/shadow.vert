// ==================================================
// Author: Priyansh Nayak
// Description: Vertex shader for the light-space shadow pass
// ==================================================

#version 330 core

layout (location = 0) in vec3 aPos; // Vertex position

// Model matrix from the mesh
uniform mat4 model;

// Light projection * view
uniform mat4 lightSpaceMatrix;

// Description: Shader entry point
// Params: none
void main()
{
    // Transform vertex into light clip space
    vec4 lightClipPos = lightSpaceMatrix * model * vec4(aPos, 1.0);
    gl_Position = lightClipPos;
}

