#include "engine/MathUtils.h"
#include <algorithm> // for std::clamp

// Euler angles (degrees) to quaternion with explicit order
glm::quat MathUtils::eulerToQuat(float pitchDeg, float yawDeg, float rollDeg,
                                 RotationOrder order) {
    glm::quat qX = glm::angleAxis(glm::radians(pitchDeg), glm::vec3(1, 0, 0));
    glm::quat qY = glm::angleAxis(glm::radians(yawDeg),   glm::vec3(0, 1, 0));
    glm::quat qZ = glm::angleAxis(glm::radians(rollDeg),  glm::vec3(0, 0, 1));

    switch (order) {
        case RotationOrder::XYZ: return qZ * qY * qX;
        case RotationOrder::XZY: return qY * qZ * qX;
        case RotationOrder::YXZ: return qZ * qX * qY;
        case RotationOrder::YZX: return qX * qZ * qY;
        case RotationOrder::ZXY: return qY * qX * qZ;
        case RotationOrder::ZYX: return qX * qY * qZ;
        default: return glm::quat(1, 0, 0, 0);
    }
}

// Quaternion spherical interpolation
glm::quat MathUtils::slerp(const glm::quat& a, const glm::quat& b, float t) {
    t = clamp01(t);
    return glm::normalize(glm::slerp(a, b, t));
}

// Scalar linear interpolation
float MathUtils::lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

// Clamp value to [0, 1]
float MathUtils::clamp01(float t) {
    return std::clamp(t, 0.0f, 1.0f);
}

// Smoothstep easing (ease-in-out)
float MathUtils::easeInOut(float t) {
    t = clamp01(t);
    return t * t * (3.0f - 2.0f * t);
}