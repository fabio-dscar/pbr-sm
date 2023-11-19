#include <Matrix3x3.h>

#include <Vector3.h>
#include <Matrix4x4.h>

using namespace pbr;
using namespace pbr::math;

Matrix3x3::Matrix3x3()
    : m11(1), m12(0), m13(0), m21(0), m22(1), m23(0), m31(0), m32(0), m33(1) {}

Matrix3x3::Matrix3x3(float s) : m{s, s, s, s, s, s, s, s, s} {}

Matrix3x3::Matrix3x3(float m11, float m12, float m13, float m21, float m22, float m23,
                     float m31, float m32, float m33)
    : m11(m11), m12(m12), m13(m13), m21(m21), m22(m22), m23(m23), m31(m31), m32(m32),
      m33(m33) {}

Matrix3x3::Matrix3x3(const Vector3& col0, const Vector3& col1, const Vector3& col2)
    : m11(col0.x), m12(col1.x), m13(col2.x), m21(col0.y), m22(col1.y), m23(col2.y),
      m31(col0.z), m32(col1.z), m33(col2.z) {}

Matrix3x3::Matrix3x3(const Matrix4x4& mat)
    : m11(mat.m11), m12(mat.m12), m13(mat.m13), m21(mat.m21), m22(mat.m22), m23(mat.m23),
      m31(mat.m31), m32(mat.m32), m33(mat.m33) {}

Matrix3x3 Matrix3x3::operator*(float s) const {
    return {s * m11, s * m12, s * m13, 
            s * m21, s * m22, s * m23, 
            s * m31, s * m32, s * m33};
}

Matrix3x3& Matrix3x3::operator*=(float scalar) {
    m11 *= scalar;
    m12 *= scalar;
    m13 *= scalar;
    m21 *= scalar;
    m22 *= scalar;
    m23 *= scalar;
    m31 *= scalar;
    m32 *= scalar;
    m33 *= scalar;
    return *this;
}

Vector3 Matrix3x3::operator*(const Vector3& v) const {
    return {m11 * v.x + m12 * v.y + m13 * v.z, 
            m21 * v.x + m22 * v.y + m23 * v.z,
            m31 * v.x + m32 * v.y + m33 * v.z};
}

Matrix3x3 Matrix3x3::operator*(const Matrix3x3& mat) const {
    Matrix3x3 ret;
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            ret(i, j) = m[0][i] * mat(0, j) + m[1][i] * mat(1, j) + m[2][i] * mat(2, j);
    return ret;
}

Matrix3x3 Matrix3x3::operator+(const Matrix3x3& mat) const {
    return {m11 + mat.m11, m12 + mat.m12, m13 + mat.m13, 
            m21 + mat.m21, m22 + mat.m22, m23 + mat.m23, 
            m31 + mat.m31, m32 + mat.m32, m33 + mat.m33};
}

Matrix3x3& Matrix3x3::operator+=(const Matrix3x3& mat) {
    m11 += mat.m11;
    m12 += mat.m12;
    m13 += mat.m13;
    m21 += mat.m21;
    m22 += mat.m22;
    m23 += mat.m23;
    m31 += mat.m31;
    m32 += mat.m32;
    m33 += mat.m33;
    return *this;
}

Matrix3x3 Matrix3x3::operator-(const Matrix3x3& mat) const {
    return {m11 - mat.m11, m12 - mat.m12, m13 - mat.m13, 
            m21 - mat.m21, m22 - mat.m22, m23 - mat.m23, 
            m31 - mat.m31, m32 - mat.m32, m33 - mat.m33};
}

Matrix3x3& Matrix3x3::operator-=(const Matrix3x3& mat) {
    m11 -= mat.m11;
    m12 -= mat.m12;
    m13 -= mat.m13;
    m21 -= mat.m21;
    m22 -= mat.m22;
    m23 -= mat.m23;
    m31 -= mat.m31;
    m32 -= mat.m32;
    m33 -= mat.m33;
    return *this;
}

Matrix3x3 Matrix3x3::operator-() const {
    return {-m11, -m12, -m13, -m21, -m22, -m23, -m31, -m32, -m33};
}

bool Matrix3x3::operator==(const Matrix3x3& mat) const {
    return m11 == mat.m11 && m12 == mat.m12 && m13 == mat.m13 && m21 == mat.m21 &&
           m22 == mat.m22 && m23 == mat.m23 && m31 == mat.m31 && m32 == mat.m32 &&
           m33 == mat.m33;
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

    os << std::right << "| " << std::setw(2) << val(mat.m11) << "  " << std::left
       << std::setw(2) << val(mat.m12) << " " << std::setw(2) << val(mat.m13) << " |\n";
    os << std::right << "| " << std::setw(2) << val(mat.m21) << "  " << std::left
       << std::setw(2) << val(mat.m22) << " " << std::setw(2) << val(mat.m23) << " |\n";
    os << std::right << "| " << std::setw(2) << val(mat.m31) << "  " << std::left
       << std::setw(2) << val(mat.m32) << " " << std::setw(2) << val(mat.m33) << " |\n";
    return os;
}

float Matrix3x3::trace() const {
    return m11 + m22 + m33;
}

float Matrix3x3::det() const {
    // Apply Sarrus rule to the top row
    const float det0 = (m22 * m33 - m23 * m32);
    const float det1 = (m23 * m31 - m21 * m33);
    const float det2 = (m21 * m32 - m22 * m31);

    return m11 * det0 + m12 * det1 + m13 * det2;
}

// Non-member functions
Vector3 math::operator*(const Vector3& v, const Matrix3x3& mat) {
    return {mat.m11 * v.x + mat.m21 * v.y + mat.m31 * v.z,
            mat.m12 * v.x + mat.m22 * v.y + mat.m32 * v.z,
            mat.m13 * v.x + mat.m23 * v.y + mat.m33 * v.z};
}

Matrix3x3 math::operator*(float scalar, const Matrix3x3& mat) {
    return mat * scalar;
}

Matrix3x3 math::transpose(const Matrix3x3& mat) {
    return {mat.m11, mat.m21, mat.m31, 
            mat.m12, mat.m22, mat.m32, 
            mat.m13, mat.m23, mat.m33};
}

Matrix3x3 math::inverse(const Matrix3x3& mat) {
    const float det = mat.det();
    if (det == 0)
        return Matrix3x3(0);

    const float A = (mat.m22 * mat.m33 - mat.m23 * mat.m32);
    const float B = -(mat.m21 * mat.m33 - mat.m23 * mat.m31);
    const float C = (mat.m21 * mat.m32 - mat.m22 * mat.m31);
    const float D = -(mat.m12 * mat.m33 - mat.m13 * mat.m32);
    const float E = (mat.m11 * mat.m33 - mat.m13 * mat.m31);
    const float F = -(mat.m11 * mat.m32 - mat.m12 * mat.m31);
    const float G = (mat.m12 * mat.m23 - mat.m13 * mat.m22);
    const float H = -(mat.m11 * mat.m23 - mat.m13 * mat.m21);
    const float I = (mat.m11 * mat.m22 - mat.m12 * mat.m21);

    return (1.0f / det) * Matrix3x3(A, D, G, B, E, H, C, F, I);
}