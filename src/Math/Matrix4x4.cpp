#include <Matrix4x4.h>

#include <PBRMath.h>
#include <Vector3.h>
#include <Vector4.h>
#include <Matrix3x3.h>
#include <Quat.h>

using namespace pbr;
using namespace pbr::math;

// clang-format off
Matrix4x4::Matrix4x4() : m{{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}} {}

Matrix4x4::Matrix4x4(float s) : m{{s, s, s, s}, {s, s, s, s}, {s, s, s, s}, {s, s, s, s}} {}

Matrix4x4::Matrix4x4(float m11, float m12, float m13, float m14, float m21, float m22,
                     float m23, float m24, float m31, float m32, float m33, float m34,
                     float m41, float m42, float m43, float m44)
    : m{{m11, m21, m31, m41}, {m12, m22, m32, m42}, {m13, m23, m33, m43}, {m14, m24, m34, m44}} {}

Matrix4x4::Matrix4x4(const Vector4& col0, const Vector4& col1, const Vector4& col2,
                     const Vector4& col3)
    : m{{col0.x, col0.y, col0.z, col0.w}, {col1.x, col1.y, col1.z, col1.w}, 
        {col2.x, col2.y, col2.z, col2.w}, {col3.x, col3.y, col3.z, col3.w}} {}

Matrix4x4::Matrix4x4(const Matrix3x3& mat)
        : m{{mat(0, 0), mat(1, 0), mat(2, 0), 0}, 
            {mat(0, 1), mat(1, 1), mat(2, 1), 0}, 
            {mat(0, 2), mat(1, 2), mat(2, 2), 0}, 
            {        0,         0,         0, 1}} {}

// clang-format on

Matrix4x4::Matrix4x4(const Quat& quat) {
    *this = quat.toMatrix();
}

Matrix4x4 Matrix4x4::operator*(float s) const {
    return {s * m[0][0], s * m[1][0], s * m[2][0], s * m[3][0], s * m[0][1], s * m[1][1],
            s * m[2][1], s * m[3][1], s * m[0][2], s * m[1][2], s * m[2][2], s * m[3][2],
            s * m[0][3], s * m[1][3], s * m[2][3], s * m[3][3]};
}

Matrix4x4& Matrix4x4::operator*=(float scalar) {
    m[0][0] *= scalar;
    m[1][0] *= scalar;
    m[2][0] *= scalar;
    m[3][0] *= scalar;
    m[0][1] *= scalar;
    m[1][1] *= scalar;
    m[2][1] *= scalar;
    m[3][1] *= scalar;
    m[0][2] *= scalar;
    m[1][2] *= scalar;
    m[2][2] *= scalar;
    m[3][2] *= scalar;
    m[0][3] *= scalar;
    m[1][3] *= scalar;
    m[2][3] *= scalar;
    m[3][3] *= scalar;
    return *this;
}

Vector3 Matrix4x4::operator*(const Vector3& v) const {
    return {m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z + m[3][0],
            m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z + m[3][1],
            m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z + m[3][2]};
}

Vector4 Matrix4x4::operator*(const Vector4& v) const {
    return {m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z + m[3][0] * v.w,
            m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z + m[3][1] * v.w,
            m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z + m[3][2] * v.w,
            m[0][3] * v.x + m[1][3] * v.y + m[2][3] * v.z + m[3][3] * v.w};
}

Matrix4x4 Matrix4x4::operator*(const Matrix4x4& mat) const {
    Matrix4x4 ret;
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            ret(i, j) = m[0][i] * mat(0, j) + m[1][i] * mat(1, j) + m[2][i] * mat(2, j) +
                        m[3][i] * mat(3, j);
    return ret;
}

Matrix4x4 Matrix4x4::operator+(const Matrix4x4& mat) const {
    return {m[0][0] + mat.m[0][0], m[1][0] + mat.m[1][0], m[2][0] + mat.m[2][0],
            m[3][0] + mat.m[3][0], m[0][1] + mat.m[0][1], m[1][1] + mat.m[1][1],
            m[2][1] + mat.m[2][1], m[3][1] + mat.m[3][1], m[0][2] + mat.m[0][2],
            m[1][2] + mat.m[1][2], m[2][2] + mat.m[2][2], m[3][2] + mat.m[3][2],
            m[0][3] + mat.m[0][3], m[1][3] + mat.m[1][3], m[2][3] + mat.m[2][3],
            m[3][3] + mat.m[3][3]};
}

Matrix4x4& Matrix4x4::operator+=(const Matrix4x4& mat) {
    m[0][0] += mat.m[0][0];
    m[1][0] += mat.m[1][0];
    m[2][0] += mat.m[2][0];
    m[3][0] += mat.m[3][0];
    m[0][1] += mat.m[0][1];
    m[1][1] += mat.m[1][1];
    m[2][1] += mat.m[2][1];
    m[3][1] += mat.m[3][1];
    m[0][2] += mat.m[0][2];
    m[1][2] += mat.m[1][2];
    m[2][2] += mat.m[2][2];
    m[3][2] += mat.m[3][2];
    m[0][3] += mat.m[0][3];
    m[1][3] += mat.m[1][3];
    m[2][3] += mat.m[2][3];
    m[3][3] += mat.m[3][3];
    return *this;
}

Matrix4x4 Matrix4x4::operator-(const Matrix4x4& mat) const {
    return {m[0][0] - mat.m[0][0], m[1][0] - mat.m[1][0], m[2][0] - mat.m[2][0],
            m[3][0] - mat.m[3][0], m[0][1] - mat.m[0][1], m[1][1] - mat.m[1][1],
            m[2][1] - mat.m[2][1], m[3][1] - mat.m[3][1], m[0][2] - mat.m[0][2],
            m[1][2] - mat.m[1][2], m[2][2] - mat.m[2][2], m[3][2] - mat.m[3][2],
            m[0][3] - mat.m[0][3], m[1][3] - mat.m[1][3], m[2][3] - mat.m[2][3],
            m[3][3] - mat.m[3][3]};
}

Matrix4x4& Matrix4x4::operator-=(const Matrix4x4& mat) {
    m[0][0] -= mat.m[0][0];
    m[1][0] -= mat.m[1][0];
    m[2][0] -= mat.m[2][0];
    m[3][0] -= mat.m[3][0];
    m[0][1] -= mat.m[0][1];
    m[1][1] -= mat.m[1][1];
    m[2][1] -= mat.m[2][1];
    m[3][1] -= mat.m[3][1];
    m[0][2] -= mat.m[0][2];
    m[1][2] -= mat.m[1][2];
    m[2][2] -= mat.m[2][2];
    m[3][2] -= mat.m[3][2];
    m[0][3] -= mat.m[0][3];
    m[1][3] -= mat.m[1][3];
    m[2][3] -= mat.m[2][3];
    m[3][3] -= mat.m[3][3];
    return *this;
}

Matrix4x4 Matrix4x4::operator-() const {
    return {-m[0][0], -m[1][0], -m[2][0], -m[3][0], -m[0][1], -m[1][1],
            -m[2][1], -m[3][1], -m[0][2], -m[1][2], -m[2][2], -m[3][2],
            -m[0][3], -m[1][3], -m[2][3], -m[3][3]};
}

bool Matrix4x4::operator==(const Matrix4x4& mat) const {
    return m[0][0] == mat.m[0][0] && m[1][0] == mat.m[1][0] && m[2][0] == mat.m[2][0] &&
           m[3][0] == mat.m[3][0] && m[0][1] == mat.m[0][1] && m[1][1] == mat.m[1][1] &&
           m[2][1] == mat.m[2][1] && m[3][1] == mat.m[3][1] && m[0][2] == mat.m[0][2] &&
           m[1][2] == mat.m[1][2] && m[2][2] == mat.m[2][2] && m[3][2] == mat.m[3][2] &&
           m[0][3] == mat.m[0][3] && m[1][3] == mat.m[1][3] && m[2][3] == mat.m[2][3] &&
           m[3][3] == mat.m[3][3];
}

bool Matrix4x4::operator!=(const Matrix4x4& mat) const {
    return !(*this == mat);
}

float Matrix4x4::operator()(uint32 i, uint32 j) const {
    return m[j][i];
}

float& Matrix4x4::operator()(uint32 i, uint32 j) {
    return m[j][i];
}

float Matrix4x4::trace() const {
    return m[0][0] + m[1][1] + m[2][2] + m[3][3];
}

float Matrix4x4::det() const {
    // Apply Sarrus rule to the top row
    const float det0 = m[1][1] * m[2][2] * m[3][3] - m[1][1] * m[3][2] * m[2][3] -
                       m[1][2] * m[2][1] * m[3][3] + m[1][2] * m[3][1] * m[2][3] +
                       m[1][3] * m[2][1] * m[3][2] - m[1][3] * m[3][1] * m[2][2];
    const float det1 = -m[0][1] * m[2][2] * m[3][3] + m[0][1] * m[3][2] * m[2][3] +
                       m[0][2] * m[2][1] * m[3][3] - m[0][2] * m[3][1] * m[2][3] -
                       m[0][3] * m[2][1] * m[3][2] + m[0][3] * m[3][1] * m[2][2];
    const float det2 = m[0][1] * m[1][2] * m[3][3] - m[0][1] * m[3][2] * m[1][3] -
                       m[0][2] * m[1][1] * m[3][3] + m[0][2] * m[3][1] * m[1][3] +
                       m[0][3] * m[1][1] * m[3][2] - m[0][3] * m[3][1] * m[1][2];
    const float det3 = -m[0][1] * m[1][2] * m[2][3] + m[0][1] * m[2][2] * m[1][3] +
                       m[0][2] * m[1][1] * m[2][3] - m[0][2] * m[2][1] * m[1][3] -
                       m[0][3] * m[1][1] * m[2][2] + m[0][3] * m[2][1] * m[1][2];

    return m[0][0] * det0 + m[1][0] * det1 + m[2][0] * det2 + m[3][0] * det3;
}

std::istream& math::operator>>(std::istream& is, Matrix4x4& mat) {
    Vector4 col1, col2, col3, col4;

    is >> col1;
    is >> col2;
    is >> col3;
    is >> col4;

    mat = Matrix4x4(col1, col2, col3, col4);

    return is;
}

std::ostream& math::operator<<(std::ostream& os, const Matrix4x4& mat) {
    auto val = [](float v) -> float {
        if (std::abs(v) < FLOAT_EPSILON)
            return 0;
        return v;
    };

    os << std::right << "| " << std::setw(2) << val(mat(0, 0)) << "  " << std::left
       << std::setw(2) << val(mat(0, 1)) << " " << std::setw(2) << val(mat(0, 2)) << " "
       << std::setw(2) << val(mat(0, 3)) << " |" << std::endl;
    os << std::right << "| " << std::setw(2) << val(mat(1, 0)) << "  " << std::left
       << std::setw(2) << val(mat(1, 1)) << " " << std::setw(2) << val(mat(1, 2)) << " "
       << std::setw(2) << val(mat(1, 3)) << " |" << std::endl;
    os << std::right << "| " << std::setw(2) << val(mat(2, 0)) << "  " << std::left
       << std::setw(2) << val(mat(2, 1)) << " " << std::setw(2) << val(mat(2, 2)) << " "
       << std::setw(2) << val(mat(2, 3)) << " |" << std::endl;
    os << std::right << "| " << std::setw(2) << val(mat(3, 0)) << "  " << std::left
       << std::setw(2) << val(mat(3, 1)) << " " << std::setw(2) << val(mat(3, 2)) << " "
       << std::setw(2) << val(mat(3, 3)) << " |" << std::endl;
    return os;
}

Vector3 math::operator*(const Vector3& v, const Matrix4x4& mat) {
    return {mat(0, 0) * v.x + mat(1, 0) * v.y + mat(2, 0) * v.z + mat(3, 0),
            mat(0, 1) * v.x + mat(1, 1) * v.y + mat(2, 1) * v.z + mat(3, 1),
            mat(0, 2) * v.x + mat(1, 2) * v.y + mat(2, 2) * v.z + mat(3, 2)};
}

Matrix4x4 math::operator*(float scalar, const Matrix4x4& mat) {
    return mat * scalar;
}

Matrix4x4 math::Transpose(const Matrix4x4& mat) {
    return {mat(0, 0), mat(1, 0), mat(2, 0), mat(3, 0), mat(0, 1), mat(1, 1),
            mat(2, 1), mat(3, 1), mat(0, 2), mat(1, 2), mat(2, 2), mat(3, 2),
            mat(0, 3), mat(1, 3), mat(2, 3), mat(3, 3)};
}

Matrix4x4 math::Inverse(const Matrix4x4& mat) {
    Matrix4x4 inv;

    inv(0, 0) = mat(1, 1) * mat(2, 2) * mat(3, 3) + mat(1, 2) * mat(2, 3) * mat(3, 1) +
                mat(1, 3) * mat(2, 1) * mat(3, 2) - mat(1, 1) * mat(2, 3) * mat(3, 2) -
                mat(1, 2) * mat(2, 1) * mat(3, 3) - mat(1, 3) * mat(2, 2) * mat(3, 1);
    inv(0, 1) = mat(0, 1) * mat(2, 3) * mat(3, 2) + mat(0, 2) * mat(2, 1) * mat(3, 3) +
                mat(0, 3) * mat(2, 2) * mat(3, 1) - mat(0, 1) * mat(2, 2) * mat(3, 3) -
                mat(0, 2) * mat(2, 3) * mat(3, 1) - mat(0, 3) * mat(2, 1) * mat(3, 2);
    inv(0, 2) = mat(0, 1) * mat(1, 2) * mat(3, 3) + mat(0, 2) * mat(1, 3) * mat(3, 1) +
                mat(0, 3) * mat(1, 1) * mat(3, 2) - mat(0, 1) * mat(1, 3) * mat(3, 2) -
                mat(0, 2) * mat(1, 1) * mat(3, 3) - mat(0, 3) * mat(1, 2) * mat(3, 1);
    inv(0, 3) = mat(0, 1) * mat(1, 3) * mat(2, 2) + mat(0, 2) * mat(1, 1) * mat(2, 3) +
                mat(0, 3) * mat(1, 2) * mat(2, 1) - mat(0, 1) * mat(1, 2) * mat(2, 3) -
                mat(0, 2) * mat(1, 3) * mat(2, 1) - mat(0, 3) * mat(1, 1) * mat(2, 2);

    inv(1, 0) = mat(1, 0) * mat(2, 3) * mat(3, 2) + mat(1, 2) * mat(2, 0) * mat(3, 3) +
                mat(1, 3) * mat(2, 2) * mat(3, 0) - mat(1, 0) * mat(2, 2) * mat(3, 3) -
                mat(1, 2) * mat(2, 3) * mat(3, 0) - mat(1, 3) * mat(2, 0) * mat(3, 2);
    inv(1, 1) = mat(0, 0) * mat(2, 2) * mat(3, 3) + mat(0, 2) * mat(2, 3) * mat(3, 0) +
                mat(0, 3) * mat(2, 0) * mat(3, 2) - mat(0, 0) * mat(2, 3) * mat(3, 2) -
                mat(0, 2) * mat(2, 0) * mat(3, 3) - mat(0, 3) * mat(2, 2) * mat(3, 0);
    inv(1, 2) = mat(0, 0) * mat(1, 3) * mat(3, 2) + mat(0, 2) * mat(1, 0) * mat(3, 3) +
                mat(0, 3) * mat(1, 2) * mat(3, 0) - mat(0, 0) * mat(1, 2) * mat(3, 3) -
                mat(0, 2) * mat(1, 3) * mat(3, 0) - mat(0, 3) * mat(1, 0) * mat(3, 2);
    inv(1, 3) = mat(0, 0) * mat(1, 2) * mat(2, 3) + mat(0, 2) * mat(1, 3) * mat(2, 0) +
                mat(0, 3) * mat(1, 0) * mat(2, 2) - mat(0, 0) * mat(1, 3) * mat(2, 2) -
                mat(0, 2) * mat(1, 0) * mat(2, 3) - mat(0, 3) * mat(1, 2) * mat(2, 0);

    inv(2, 0) = mat(1, 0) * mat(2, 1) * mat(3, 3) + mat(1, 1) * mat(2, 3) * mat(3, 0) +
                mat(1, 3) * mat(2, 0) * mat(3, 1) - mat(1, 0) * mat(2, 3) * mat(3, 1) -
                mat(1, 1) * mat(2, 0) * mat(3, 3) - mat(1, 3) * mat(2, 1) * mat(3, 0);
    inv(2, 1) = mat(0, 0) * mat(2, 3) * mat(3, 1) + mat(0, 1) * mat(2, 0) * mat(3, 3) +
                mat(0, 3) * mat(2, 1) * mat(3, 0) - mat(0, 0) * mat(2, 1) * mat(3, 3) -
                mat(0, 1) * mat(2, 3) * mat(3, 0) - mat(0, 3) * mat(2, 0) * mat(3, 1);
    inv(2, 2) = mat(0, 0) * mat(1, 1) * mat(3, 3) + mat(0, 1) * mat(1, 3) + mat(3, 0) +
                mat(0, 3) * mat(1, 0) * mat(3, 1) - mat(0, 0) * mat(1, 3) * mat(3, 1) -
                mat(0, 1) * mat(1, 0) * mat(3, 3) - mat(0, 3) * mat(1, 1) * mat(3, 0);
    inv(2, 3) = mat(0, 0) * mat(1, 3) * mat(2, 1) + mat(0, 1) * mat(1, 0) * mat(2, 3) +
                mat(0, 3) * mat(1, 1) * mat(2, 0) - mat(0, 0) * mat(1, 1) * mat(2, 3) -
                mat(0, 1) * mat(1, 3) * mat(2, 0) - mat(0, 3) * mat(1, 0) * mat(2, 1);

    inv(3, 0) = mat(1, 0) * mat(2, 2) * mat(3, 1) + mat(1, 1) * mat(2, 0) * mat(3, 2) +
                mat(1, 2) * mat(2, 1) * mat(3, 0) - mat(1, 0) * mat(2, 1) * mat(3, 2) -
                mat(1, 1) * mat(2, 2) * mat(3, 0) - mat(1, 2) * mat(2, 0) * mat(3, 1);
    inv(3, 1) = mat(0, 0) * mat(2, 1) * mat(3, 2) + mat(0, 1) * mat(2, 2) * mat(3, 0) +
                mat(0, 2) * mat(2, 0) * mat(3, 1) - mat(0, 0) * mat(2, 2) * mat(3, 1) -
                mat(0, 1) * mat(2, 0) * mat(3, 2) - mat(0, 2) * mat(2, 1) * mat(3, 0);
    inv(3, 2) = mat(0, 0) * mat(1, 2) * mat(3, 1) + mat(0, 1) * mat(1, 0) * mat(3, 2) +
                mat(0, 2) * mat(1, 1) * mat(3, 0) - mat(0, 0) * mat(1, 1) * mat(3, 2) -
                mat(0, 1) * mat(1, 2) * mat(3, 0) - mat(0, 2) * mat(1, 0) * mat(3, 1);
    inv(3, 3) = mat(0, 0) * mat(1, 1) * mat(2, 2) + mat(0, 1) * mat(1, 2) * mat(2, 0) +
                mat(0, 2) * mat(1, 0) * mat(2, 1) - mat(0, 0) * mat(1, 2) * mat(2, 1) -
                mat(0, 1) * mat(1, 0) * mat(2, 2) - mat(0, 2) * mat(1, 1) * mat(2, 0);

    float det = mat(0, 0) * inv(0, 0) + mat(0, 1) * inv(1, 0) + mat(0, 2) * inv(2, 0) +
                mat(0, 3) * inv(3, 0);

    if (det == 0)
        return Matrix4x4(0);

    return (1.0f / det) * inv;
}