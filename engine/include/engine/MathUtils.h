// ==================================================
// Author: Priyansh Nayak
// Description: Small math helpers used across transforms and scene setup
// ==================================================

#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>



namespace MathUtils {
    enum class RotationOrder {
        XYZ, XZY,
        YXZ, YZX,
        ZXY, ZYX
    };
    // Build a transformation matrix from position, rotation (axis-angle), and scale
    glm::mat4 buildTRS(const glm::vec3& pos,
                              const glm::vec3& rotAxis, float rotAngle,
                              const glm::vec3& scale);
    // Euler (degrees) to Quaternion with explicit order
    glm::quat eulerToQuat(float pitchDeg, float yawDeg, float rollDeg,
                                 RotationOrder order);

    // Quaternion interpolation
    // Params: const glm::quat& a, const glm::quat& b, float t
    glm::quat slerp(const glm::quat& a, const glm::quat& b, float t);

    // Scalar interpolation
    // Params: float a, float b, float t
    float lerp(float a, float b, float t);

    // Clamp to [0, 1]
    // Params: float t
    float clamp01(float t);

    // Smooth time remapping (smoothstep)
    // Params: float t
    float easeInOut(float t);

    // Catmull-Rom spline interpolation
    glm::vec3 catmullRom(
        const glm::vec3& p0,
        const glm::vec3& p1,
        const glm::vec3& p2,
        const glm::vec3& p3,
        float t
    );
}
