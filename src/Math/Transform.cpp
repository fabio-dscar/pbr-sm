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

    mat(0, 0) = 2.f / (r - l);
    mat(1, 1) = 2.f / (t - b);
    mat(2, 2) = -2.f / (f - n);

    mat(0, 3) = -(r + l) / (r - l);
    mat(1, 3) = -(t + b) / (t - b);
    mat(2, 3) = -(f + n) / (f - n);
    mat(3, 3) = 1.0f;

    return mat;
}

Matrix4x4 math::Perspective(float fov, float aspect, float near, float far) {
    float tanFov = std::tan(Radians(fov / 2.0f));

    float xScale = 1.0f / (tanFov * aspect);
    float yScale = 1.0f / tanFov;

    Matrix4x4 persp;

    persp(0, 0) = xScale;
    persp(1, 1) = yScale;
    persp(2, 2) = -(far + near) / (far - near);
    persp(3, 3) = 0;

    persp(2, 3) = -2.0f * far * near / (far - near);
    persp(3, 2) = -1;

    return persp;
}

Matrix4x4 math::LookAt(const Vector3& eye, const Vector3& center, const Vector3& up) {
    Vector3 n = eye - center;

    // If eye == center
    if (n.lengthSqr() == 0)
        n.z = 1;

    n.normalize();

    Vector3 u = Normalize(Cross(up, n));
    Vector3 v = Cross(n, u);

    Vector3 tr = Vector3(Dot(eye, u), Dot(eye, v), Dot(eye, n));

    Matrix4x4 mat;
    mat(0, 0) = u.x;
    mat(0, 1) = u.y;
    mat(0, 2) = u.z;
    mat(1, 0) = v.x;
    mat(1, 1) = v.y;
    mat(1, 2) = v.z;
    mat(2, 0) = n.x;
    mat(2, 1) = n.y;
    mat(2, 2) = n.z;

    mat(0, 3) = -tr.x;
    mat(1, 3) = -tr.y;
    mat(2, 3) = -tr.z;

    return mat;
}

void math::Decompose(const Matrix4x4& mat, Vector3& tr, Quat& rot, Vector3& scale) {
    constexpr int MaxIterations = 100;
    constexpr float Epsilon = 0.0001;

    auto FrobeniusNorm = [](const Matrix3x3& mat) -> float {
        float sum = 0;
        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++)
                sum += mat(i, j) * mat(i, j);
        return std::sqrt(sum);
    };

    tr = {mat(0, 3), mat(1, 3), mat(2, 3)};

    float norm, diffNorm;
    int iterCount = 0;
    Mat3 M{mat}, R{mat};
    do {
        Mat3 Rnext;
        Mat3 Rit = Inverse(Transpose(R));

        norm = std::sqrt(FrobeniusNorm(Inverse(R)) / FrobeniusNorm(R));

        CHECK(norm != 0);

        Rnext = 0.5 * (norm * R + (1.0 / norm) * Rit);
        diffNorm = FrobeniusNorm(R - Rnext);
        R = Rnext;
    } while (++iterCount < MaxIterations && diffNorm > Epsilon);

    rot = Quat{Mat4{R}};

    Mat3 scaleMat = Inverse(R) * M;
    scale = {scaleMat(0, 0), scaleMat(1, 1), scaleMat(2, 2)};
}