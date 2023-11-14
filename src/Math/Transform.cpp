#include <Transform.h>

#include <PBRMath.h>

using namespace pbr;
using namespace pbr::math;

Matrix4x4 math::translation(const Vector3& tr) {
    Matrix4x4 ret;

    ret(0, 3) = tr.x;
    ret(1, 3) = tr.y;
    ret(2, 3) = tr.z;

    return ret;
}

Matrix4x4 math::skewX(float m) {
    Matrix4x4 ret;

    ret(0, 1) = m;

    return ret;
}

Matrix4x4 math::scale(const Vector3& scale) {
    Matrix4x4 ret;

    ret(0, 0) = scale.x;
    ret(1, 1) = scale.y;
    ret(2, 2) = scale.z;

    return ret;
}

Matrix4x4 math::scale(float x, float y, float z) {
    return scale(Vector3(x, y, z));
}

Matrix4x4 math::rotationX(float rads) {
    const float sin = std::sin(rads);
    const float cos = std::cos(rads);

    Matrix4x4 rotX;
    rotX(1, 1) = cos;
    rotX(1, 2) = -sin;
    rotX(2, 1) = sin;
    rotX(2, 2) = cos;

    return rotX;
}

Matrix4x4 math::rotationY(float rads) {
    const float sin = std::sin(rads);
    const float cos = std::cos(rads);

    Matrix4x4 rotY;
    rotY(0, 0) = cos;
    rotY(0, 2) = sin;
    rotY(2, 0) = -sin;
    rotY(2, 2) = cos;

    return rotY;
}

Matrix4x4 math::rotationZ(float rads) {
    const float sin = std::sin(rads);
    const float cos = std::cos(rads);

    Matrix4x4 rotZ;
    rotZ(0, 0) = cos;
    rotZ(0, 1) = -sin;
    rotZ(1, 0) = sin;
    rotZ(1, 1) = cos;

    return rotZ;
}

Matrix4x4 math::rotationAxis(float rads, const Vector3& a) {
    const float sin = std::sin(rads);
    const float cos = std::cos(rads);

    Matrix3x3 I;
    Matrix3x3 K = {0, -a.z, a.y, a.z, 0, -a.x, -a.y, a.x, 0};

    return Matrix4x4(I + sin * K + (1.0f - cos) * K * K);
}

Matrix4x4 math::orthographic(float l, float r, float b, float t, float n, float f) {
    Matrix4x4 mat;

    mat.m11 = 2.f / (r - l);
    mat.m22 = 2.f / (t - b);
    mat.m33 = -2.f / (f - n);

    mat.m14 = -(r + l) / (r - l);
    mat.m24 = -(t + b) / (t - b);
    mat.m34 = -(f + n) / (f - n);
    mat.m44 = 1.0f;

    return mat;
}

Matrix4x4 math::perspective(float fov, float aspect, float near, float far) {
    float tanFov = std::tan(radians(fov / 2.0f));

    float xScale = 1.0f / (tanFov * aspect);
    float yScale = 1.0f / tanFov;

    Matrix4x4 persp;

    persp.m11 = xScale;
    persp.m22 = yScale;
    persp.m33 = -(far + near) / (far - near);
    persp.m44 = 0;

    persp.m34 = -2.0f * far * near / (far - near);
    persp.m43 = -1;

    return persp;
}

Matrix4x4 math::lookAt(const Vector3& eye, const Vector3& center, const Vector3& up) {
    Vector3 n = eye - center;

    // If eye == center
    if (n.lengthSqr() == 0)
        n.z = 1;

    n.normalize();

    Vector3 u = normalize(cross(up, n));
    Vector3 v = cross(n, u);

    Vector3 tr = Vector3(dot(eye, u), dot(eye, v), dot(eye, n));

    Matrix4x4 mat;
    mat.m11 = u.x;
    mat.m12 = u.y;
    mat.m13 = u.z;
    mat.m21 = v.x;
    mat.m22 = v.y;
    mat.m23 = v.z;
    mat.m31 = n.x;
    mat.m32 = n.y;
    mat.m33 = n.z;

    mat.m14 = -tr.x;
    mat.m24 = -tr.y;
    mat.m34 = -tr.z;

    return mat;
}
