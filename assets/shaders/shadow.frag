#version 330 core

layout (location = 0) out vec4 momentOut; // Output RGBA moments for MSM mode

uniform int shadowMode = 0; // 0 = baseline depth path, 1 = MSM moment-writing path

void main()
{
    // Baseline check
    if (shadowMode == 0) return;

    // Use the exact rasterized depth and remap it into signed depth
    float z = clamp(gl_FragCoord.z, 0.0, 1.0);
    z = z * 2.0 - 1.0;

    // Store the first four moments of that depth
    float z2 = z * z;
    momentOut = vec4(z, z2, z2 * z, z2 * z2);
}
