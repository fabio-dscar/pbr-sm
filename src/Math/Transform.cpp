#include <Transform.h>

#include <PBRMath.h>

using namespace pbr;
using namespace pbr::math;

Matrix4x4 math::RotationAxis(float rads, const Vector3& a) {
    const float sin = std::sin(rads);
    const float cos = std::cos(rads);

    Matrix3x3 I;
    Matrix3x3 K = {0, -a.z, a.y, a.z, 0, -a.x, -a.y, a.x, 0};

    return Matrix4x4(I + sin * K + (1.0f - cos) * K * K);
}

Matrix4x4 math::Orthographic(float l, float r, float b, float t, float n, float f) {
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

Matrix4x4 math::Perspective(float fov, float aspect, float near, float far) {
    float tanFov = std::tan(Radians(fov / 2.0f));

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

Matrix4x4 math::LookAt(const Vector3& eye, const Vector3& center, const Vector3& up) {
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
