#ifndef __PBR_TRANSFORM_H__
#define __PBR_TRANSFORM_H__

#include <PBR.h>
#include <PBRMath.h>

namespace pbr {
namespace math {

inline Matrix4x4 Translation(const Vector3& tr) {
    Matrix4x4 ret;
    ret(0, 3) = tr.x;
    ret(1, 3) = tr.y;
    ret(2, 3) = tr.z;
    return ret;
}

inline Matrix4x4 SkewX(float m) {
    Matrix4x4 ret;
    ret(0, 1) = m;
    return ret;
}

inline Matrix4x4 Scale(float x, float y, float z) {
    Matrix4x4 ret;
    ret(0, 0) = x;
    ret(1, 1) = y;
    ret(2, 2) = z;
    return ret;
}

inline Matrix4x4 Scale(const Vector3& scale) {
    return Scale(scale.x, scale.y, scale.z);
}

inline Matrix4x4 RotationX(float rads) {
    float sin = std::sin(rads);
    float cos = std::cos(rads);

    Matrix4x4 rotX;
    rotX(1, 1) = cos;
    rotX(1, 2) = -sin;
    rotX(2, 1) = sin;
    rotX(2, 2) = cos;

    return rotX;
}

inline Matrix4x4 RotationY(float rads) {
    const float sin = std::sin(rads);
    const float cos = std::cos(rads);

    Matrix4x4 rotY;
    rotY(0, 0) = cos;
    rotY(0, 2) = sin;
    rotY(2, 0) = -sin;
    rotY(2, 2) = cos;

    return rotY;
}

inline Matrix4x4 RotationZ(float rads) {
    const float sin = std::sin(rads);
    const float cos = std::cos(rads);

    Matrix4x4 rotZ;
    rotZ(0, 0) = cos;
    rotZ(0, 1) = -sin;
    rotZ(1, 0) = sin;
    rotZ(1, 1) = cos;

    return rotZ;
}

inline Matrix4x4 Transform(const Vector3& tr, const Vector3& s, const Matrix4x4& rot) {
    return Translation(tr) * rot * Scale(s);
}

Matrix4x4 RotationAxis(float rads, const Vector3& axis);
Matrix4x4 Orthographic(float l, float r, float b, float t, float n, float f);
Matrix4x4 Perspective(float fov, float aspect, float n, float f);
Matrix4x4 LookAt(const Vector3& eye, const Vector3& center, const Vector3& up);
void Decompose(const Matrix4x4& mat, Vector3& tr, Quat& rot, Vector3& scale);

} // namespace math
} // namespace pbr

#endif