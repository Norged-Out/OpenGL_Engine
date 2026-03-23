#version 330 core

in vec3 texDir;
out vec4 fragColor;

uniform samplerCube environmentMap;
uniform float skyboxExposure = 1.0;

void main() {
    vec3 color = texture(environmentMap, texDir).rgb;

    // Simple exposure control for HDR sky
    color *= skyboxExposure;

    fragColor = vec4(color, 1.0);
}
