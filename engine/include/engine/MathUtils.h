#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

enum class RotationOrder {
    XYZ, XZY,
    YXZ, YZX,
    ZXY, ZYX
};

class MathUtils {
public:
    // Euler (degrees) to Quaternion with explicit order
    static glm::quat eulerToQuat(float pitchDeg, float yawDeg, float rollDeg,
                                 RotationOrder order);

    // Quaternion interpolation
    static glm::quat slerp(const glm::quat& a, const glm::quat& b, float t);

    // Scalar interpolation
    static float lerp(float a, float b, float t);

    // Clamp to [0, 1]
    static float clamp01(float t);

    // Smooth time remapping (smoothstep)
    static float easeInOut(float t);
};