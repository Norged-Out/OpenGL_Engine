// ==================================================
// Author: Priyansh Nayak
// Description: Vertex shader for drawing the engine skybox cube
// ==================================================

#version 330 core

layout (location = 0) in vec3 aPos;

out vec3 texDir;

uniform mat4 view;
uniform mat4 projection;

// Description: Shader entry point
// Params: none
void main()
{
    texDir = aPos;

    vec4 pos = projection * view * vec4(aPos, 1.0);
    gl_Position = pos.xyww; // force depth = 1.0
}

