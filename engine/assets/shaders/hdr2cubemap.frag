#version 330 core

in vec3 localPos;

out vec4 fragColor;

uniform sampler2D eqrMap;

const vec2 invAtan = vec2(0.1591, 0.3183);

void main() {
    vec3 dir = normalize(localPos);
    vec2 uv = vec2(atan(dir.z, dir.x), asin(dir.y));
    uv *= invAtan;
    uv += 0.5;
    vec3 color = texture(eqrMap, uv).rgb;
    fragColor = vec4(color, 1.0);
}
