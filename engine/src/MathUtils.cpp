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

/*
    Catmull–Rom spline formula (uniform, tension = 0.5):

    This computes a smooth cubic curve that:
    - passes THROUGH p1 when t = 0
    - passes THROUGH p2 when t = 1
    - uses p0 and p3 to infer tangents automatically
    - guarantees C¹ continuity (smooth velocity)

    The curve is a weighted combination of the four points,
    where the weights are polynomials in t.
*/
glm::vec3 MathUtils::catmullRom(
    const glm::vec3& p0, // previous control point
    const glm::vec3& p1, // start of current segment
    const glm::vec3& p2, // end of current segment
    const glm::vec3& p3, // next control point
    float t              // normalized time in [0, 1]
) {
    // Precompute powers of t for efficiency
    float t2 = t * t;    // t squared
    float t3 = t2 * t;   // t cubed   

    return 0.5f * (
        // When t = 0, this term ensures the curve starts at p1
        (2.0f * p1)

        // Linear term: controls initial direction (tangent at p1)
        + (-p0 + p2) * t

        // Quadratic term: controls curvature
        + (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t2

        // Cubic term: ensures smooth arrival at p2
        + (-p0 + 3.0f * p1 - 3.0f * p2 + p3) * t3
    );
}