#include <Matrix3x3.h>

#include <Vector3.h>
#include <Matrix4x4.h>

using namespace pbr;
using namespace pbr::math;

// clang-format off
Matrix3x3::Matrix3x3() : m{{1, 0, 0}, {0, 1, 0}, {0, 0, 1}} {}

Matrix3x3::Matrix3x3(float s) : m{{s, s, s}, {s, s, s}, {s, s, s}} {}

Matrix3x3::Matrix3x3(float m11, float m12, float m13, float m21, float m22, float m23,
                     float m31, float m32, float m33)
    : m{{m11, m21, m31}, {m12, m22, m32}, {m13, m23, m33}} {}

Matrix3x3::Matrix3x3(const Vector3& col0, const Vector3& col1, const Vector3& col2)
    : m{{col0.x, col0.y, col0.z}, {col1.x, col1.y, col1.z}, {col2.x, col2.y, col2.z}} {}

Matrix3x3::Matrix3x3(const Matrix4x4& mat)
    : m{{mat(0, 0), mat(1, 0), mat(2, 0)}, 
        {mat(0, 1), mat(1, 1), mat(2, 1)}, 
        {mat(0, 2), mat(1, 2), mat(2, 2)}} {}

// clang-format on

Matrix3x3 Matrix3x3::operator*(float s) const {
    return {s * m[0][0], s * m[1][0], s * m[2][0], s * m[0][1], s * m[1][1],
            s * m[2][1], s * m[0][2], s * m[1][2], s * m[2][2]};
}

Matrix3x3& Matrix3x3::operator*=(float scalar) {
    m[0][0] *= scalar;
    m[1][0] *= scalar;
    m[2][0] *= scalar;
    m[0][1] *= scalar;
    m[1][1] *= scalar;
    m[2][1] *= scalar;
    m[0][2] *= scalar;
    m[1][2] *= scalar;
    m[2][2] *= scalar;
    return *this;
}

Vector3 Matrix3x3::operator*(const Vector3& v) const {
    return {m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z,
            m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z,
            m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z};
}

Matrix3x3 Matrix3x3::operator*(const Matrix3x3& mat) const {
    Matrix3x3 ret;
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            ret(i, j) = m[0][i] * mat(0, j) + m[1][i] * mat(1, j) + m[2][i] * mat(2, j);
    return ret;
}

Matrix3x3 Matrix3x3::operator+(const Matrix3x3& mat) const {
    return {m[0][0] + mat(0, 0), m[1][0] + mat(0, 1), m[2][0] + mat(0, 2),
            m[0][1] + mat(1, 0), m[1][1] + mat(1, 1), m[2][1] + mat(1, 2),
            m[0][2] + mat(2, 0), m[1][2] + mat(2, 1), m[2][2] + mat(2, 2)};
}

Matrix3x3& Matrix3x3::operator+=(const Matrix3x3& mat) {
    m[0][0] += mat(0, 0);
    m[1][0] += mat(0, 1);
    m[2][0] += mat(0, 2);
    m[0][1] += mat(1, 0);
    m[1][1] += mat(1, 1);
    m[2][1] += mat(1, 2);
    m[0][2] += mat(2, 0);
    m[1][2] += mat(2, 1);
    m[2][2] += mat(2, 2);
    return *this;
}

Matrix3x3 Matrix3x3::operator-(const Matrix3x3& mat) const {
    return {m[0][0] - mat(0, 0), m[1][0] - mat(0, 1), m[2][0] - mat(0, 2),
            m[0][1] - mat(1, 0), m[1][1] - mat(1, 1), m[2][1] - mat(1, 2),
            m[0][2] - mat(2, 0), m[1][2] - mat(2, 1), m[2][2] - mat(2, 2)};
}

Matrix3x3& Matrix3x3::operator-=(const Matrix3x3& mat) {
    m[0][0] -= mat(0, 0);
    m[1][0] -= mat(0, 1);
    m[2][0] -= mat(0, 2);
    m[0][1] -= mat(1, 0);
    m[1][1] -= mat(1, 1);
    m[2][1] -= mat(1, 2);
    m[0][2] -= mat(2, 0);
    m[1][2] -= mat(2, 1);
    m[2][2] -= mat(2, 2);
    return *this;
}

Matrix3x3 Matrix3x3::operator-() const {
    return {-m[0][0], -m[1][0], -m[2][0], -m[0][1], -m[1][1],
            -m[2][1], -m[0][2], -m[1][2], -m[2][2]};
}

bool Matrix3x3::operator==(const Matrix3x3& mat) const {
    return m[0][0] == mat(0, 0) && m[1][0] == mat(0, 1) && m[2][0] == mat(0, 2) &&
           m[0][1] == mat(1, 0) && m[1][1] == mat(1, 1) && m[2][1] == mat(1, 2) &&
           m[0][2] == mat(2, 0) && m[1][2] == mat(2, 1) && m[2][2] == mat(2, 2);
}

bool Matrix3x3::operator!=(const Matrix3x3& mat) const {
    return !(*this == mat);
}

float Matrix3x3::operator()(uint32 i, uint32 j) const {
    return m[j][i];
}

float& Matrix3x3::operator()(uint32 i, uint32 j) {
    return m[j][i];
}

std::istream& math::operator>>(std::istream& is, Matrix3x3& mat) {
    Vector3 col1, col2, col3;

    is >> col1;
    is >> col2;
    is >> col3;

    mat = Matrix3x3(col1, col2, col3);

    return is;
}

std::ostream& math::operator<<(std::ostream& os, const Matrix3x3& mat) {
    auto val = [](float v) -> float {
        if (std::abs(v) < 1e-6)
            return 0;
        return v;
    };

    os << std::right << "| " << std::setw(2) << val(mat(0, 0)) << "  " << std::left
       << std::setw(2) << val(mat(0, 1)) << " " << std::setw(2) << val(mat(0, 2))
       << " |\n";
    os << std::right << "| " << std::setw(2) << val(mat(1, 0)) << "  " << std::left
       << std::setw(2) << val(mat(1, 1)) << " " << std::setw(2) << val(mat(1, 2))
       << " |\n";
    os << std::right << "| " << std::setw(2) << val(mat(2, 0)) << "  " << std::left
       << std::setw(2) << val(mat(2, 1)) << " " << std::setw(2) << val(mat(2, 2))
       << " |\n";
    return os;
}

float Matrix3x3::trace() const {
    return m[0][0] + m[1][1] + m[2][2];
}

float Matrix3x3::det() const {
    // Apply Sarrus rule to the top row
    const float det0 = (m[1][1] * m[2][2] - m[2][1] * m[1][2]);
    const float det1 = (m[2][1] * m[0][2] - m[0][1] * m[2][2]);
    const float det2 = (m[0][1] * m[1][2] - m[1][1] * m[0][2]);

    return m[0][0] * det0 + m[1][0] * det1 + m[2][0] * det2;
}

// Non-member functions
Vector3 math::operator*(const Vector3& v, const Matrix3x3& mat) {
    return {mat(0, 0) * v.x + mat(1, 0) * v.y + mat(2, 0) * v.z,
            mat(0, 1) * v.x + mat(1, 1) * v.y + mat(2, 1) * v.z,
            mat(0, 2) * v.x + mat(1, 2) * v.y + mat(2, 2) * v.z};
}

Matrix3x3 math::operator*(float scalar, const Matrix3x3& mat) {
    return mat * scalar;
}

Matrix3x3 math::Transpose(const Matrix3x3& mat) {
    return {mat(0, 0), mat(1, 0), mat(2, 0), mat(0, 1), mat(1, 1),
            mat(2, 1), mat(0, 2), mat(1, 2), mat(2, 2)};
}

Matrix3x3 math::Inverse(const Matrix3x3& mat) {
    const float det = mat.det();
    if (det == 0)
        return Matrix3x3(0);

    const float A = (mat(1, 1) * mat(2, 2) - mat(1, 2) * mat(2, 1));
    const float B = -(mat(1, 0) * mat(2, 2) - mat(1, 2) * mat(2, 0));
    const float C = (mat(1, 0) * mat(2, 1) - mat(1, 1) * mat(2, 0));
    const float D = -(mat(0, 1) * mat(2, 2) - mat(0, 2) * mat(2, 1));
    const float E = (mat(0, 0) * mat(2, 2) - mat(0, 2) * mat(2, 0));
    const float F = -(mat(0, 0) * mat(2, 1) - mat(0, 1) * mat(2, 0));
    const float G = (mat(0, 1) * mat(1, 2) - mat(0, 2) * mat(1, 1));
    const float H = -(mat(0, 0) * mat(1, 2) - mat(0, 2) * mat(1, 0));
    const float I = (mat(0, 0) * mat(1, 1) - mat(0, 1) * mat(1, 0));

    return (1.0f / det) * Matrix3x3(A, D, G, B, E, H, C, F, I);
}