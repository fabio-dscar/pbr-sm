#ifndef __PBR_TRANSFORM_H__
#define __PBR_TRANSFORM_H__

#include <PBR.h>
#include <PBRMath.h>

namespace pbr {
namespace math {



inline Matrix4x4 translation(const Vector3& tr) {
    Matrix4x4 ret;
    ret(0, 3) = tr.x;
    ret(1, 3) = tr.y;
    ret(2, 3) = tr.z;
    return ret;
}

inline Matrix4x4 skewX(float m) {
    Matrix4x4 ret;
    ret(0, 1) = m;
    return ret;
}

inline Matrix4x4 scale(const Vector3& scale) {
    Matrix4x4 ret;
    ret(0, 0) = scale.x;
    ret(1, 1) = scale.y;
    ret(2, 2) = scale.z;
    return ret;
}

inline Matrix4x4 scale(float x, float y, float z) {
    return scale({x, y, z});
}


inline Matrix4x4 rotationX(float rads) {
    float sin = std::sin(rads);
    float cos = std::cos(rads);

    Matrix4x4 rotX;
    rotX(1, 1) = cos;
    rotX(1, 2) = -sin;
    rotX(2, 1) = sin;
    rotX(2, 2) = cos;

    return rotX;
}

inline Matrix4x4 rotationY(float rads) {
    const float sin = std::sin(rads);
    const float cos = std::cos(rads);

    Matrix4x4 rotY;
    rotY(0, 0) = cos;
    rotY(0, 2) = sin;
    rotY(2, 0) = -sin;
    rotY(2, 2) = cos;

    return rotY;
}

inline Matrix4x4 rotationZ(float rads) {
    const float sin = std::sin(rads);
    const float cos = std::cos(rads);

    Matrix4x4 rotZ;
    rotZ(0, 0) = cos;
    rotZ(0, 1) = -sin;
    rotZ(1, 0) = sin;
    rotZ(1, 1) = cos;

    return rotZ;
}

PBR_SHARED Matrix4x4 rotationAxis(float rads, const Vector3& axis);

PBR_SHARED Matrix4x4 orthographic(float l, float r, float b, float t, float n, float f);
PBR_SHARED Matrix4x4 perspective(float fov, float aspect, float n, float f);
PBR_SHARED Matrix4x4 lookAt(const Vector3& eye, const Vector3& center, const Vector3& up);

} // namespace math
} // namespace pbr

#endif