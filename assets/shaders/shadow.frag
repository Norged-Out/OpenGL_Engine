#version 330 core

layout (location = 0) out vec4 momentOut; // Output RGBA moments for MSM mode

uniform int shadowMode = 0; // 0 = baseline depth path, 1 = MSM moment-writing path
uniform bool useSignedMSMDepth = true; // Toggle signed vs unsigned depth storage for comparison work

void main()
{
    // Baseline check
    if (shadowMode == 0) return;

    // Use the exact rasterized depth written by the rasterizer
    float z = clamp(gl_FragCoord.z, 0.0, 1.0);

    // Signed depth is the current default, but the UI can switch back for comparison
    if (useSignedMSMDepth) {
        z = z * 2.0 - 1.0;
    }

    // Store the first four moments of that depth
    float z2 = z * z;
    momentOut = vec4(z, z2, z2 * z, z2 * z2);
}
