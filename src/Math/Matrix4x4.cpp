#include <Matrix4x4.h>

#include <PBRMath.h>
#include <Vector3.h>
#include <Vector4.h>
#include <Matrix3x3.h>
#include <Quat.h>

using namespace pbr;
using namespace pbr::math;

Matrix4x4::Matrix4x4()
    : m11(1), m12(0), m13(0), m14(0), m21(0), m22(1), m23(0), m24(0), m31(0), m32(0),
      m33(1), m34(0), m41(0), m42(0), m43(0), m44(1) {}

Matrix4x4::Matrix4x4(float s) : m{{s, s, s, s}, {s, s, s, s}, {s, s, s, s}, {s, s, s, s}} {}

Matrix4x4::Matrix4x4(float m11, float m12, float m13, float m14, float m21, float m22,
                     float m23, float m24, float m31, float m32, float m33, float m34,
                     float m41, float m42, float m43, float m44)
    : m11(m11), m12(m12), m13(m13), m14(m14), m21(m21), m22(m22), m23(m23), m24(m24),
      m31(m31), m32(m32), m33(m33), m34(m34), m41(m41), m42(m42), m43(m43), m44(m44) {}

Matrix4x4::Matrix4x4(const Vector4& col0, const Vector4& col1, const Vector4& col2,
                     const Vector4& col3)
    : m11(col0.x), m12(col1.x), m13(col2.x), m14(col3.x), m21(col0.y), m22(col1.y),
      m23(col2.y), m24(col3.y), m31(col0.z), m32(col1.z), m33(col2.z), m34(col3.z),
      m41(col0.w), m42(col1.w), m43(col2.w), m44(col3.w) {}

Matrix4x4::Matrix4x4(const Matrix3x3& mat)
    : m11(mat.m11), m12(mat.m12), m13(mat.m13), m14(0), m21(mat.m21), m22(mat.m22),
      m23(mat.m23), m24(0), m31(mat.m31), m32(mat.m32), m33(mat.m33), m34(0), m41(0),
      m42(0), m43(0), m44(1) {}

Matrix4x4::Matrix4x4(const Quat& quat) {
    *this = quat.toMatrix();
}

Matrix4x4 Matrix4x4::operator*(float s) const {
    return {s * m11, s * m12, s * m13, s * m14,
            s * m21, s * m22, s * m23, s * m24,
            s * m31, s * m32, s * m33, s * m34,
            s * m41, s * m42, s * m43, s * m44};
}

Matrix4x4& Matrix4x4::operator*=(float scalar) {
    m11 *= scalar;
    m12 *= scalar;
    m13 *= scalar;
    m14 *= scalar;
    m21 *= scalar;
    m22 *= scalar;
    m23 *= scalar;
    m24 *= scalar;
    m31 *= scalar;
    m32 *= scalar;
    m33 *= scalar;
    m34 *= scalar;
    m41 *= scalar;
    m42 *= scalar;
    m43 *= scalar;
    m44 *= scalar;
    return *this;
}

Vector3 Matrix4x4::operator*(const Vector3& v) const {
    return {m11 * v.x + m12 * v.y + m13 * v.z + m14, 
            m21 * v.x + m22 * v.y + m23 * v.z + m24,
            m31 * v.x + m32 * v.y + m33 * v.z + m34};
}

Vector4 Matrix4x4::operator*(const Vector4& v) const {
    return {m11 * v.x + m12 * v.y + m13 * v.z + m14 * v.w,
            m21 * v.x + m22 * v.y + m23 * v.z + m24 * v.w,
            m31 * v.x + m32 * v.y + m33 * v.z + m34 * v.w,
            m41 * v.x + m42 * v.y + m43 * v.z + m44 * v.w};
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
    return {m11 + mat.m11, m12 + mat.m12, m13 + mat.m13, m14 + mat.m14,
            m21 + mat.m21, m22 + mat.m22, m23 + mat.m23, m24 + mat.m24,
            m31 + mat.m31, m32 + mat.m32, m33 + mat.m33, m34 + mat.m34,
            m41 + mat.m41, m42 + mat.m42, m43 + mat.m43, m44 + mat.m44};
}

Matrix4x4& Matrix4x4::operator+=(const Matrix4x4& mat) {
    m11 += mat.m11;
    m12 += mat.m12;
    m13 += mat.m13;
    m14 += mat.m14;
    m21 += mat.m21;
    m22 += mat.m22;
    m23 += mat.m23;
    m24 += mat.m24;
    m31 += mat.m31;
    m32 += mat.m32;
    m33 += mat.m33;
    m34 += mat.m34;
    m41 += mat.m41;
    m42 += mat.m42;
    m43 += mat.m43;
    m44 += mat.m44;
    return *this;
}

Matrix4x4 Matrix4x4::operator-(const Matrix4x4& mat) const {
    return {m11 - mat.m11, m12 - mat.m12, m13 - mat.m13, m14 - mat.m14,
            m21 - mat.m21, m22 - mat.m22, m23 - mat.m23, m24 - mat.m24,
            m31 - mat.m31, m32 - mat.m32, m33 - mat.m33, m34 - mat.m34,
            m41 - mat.m41, m42 - mat.m42, m43 - mat.m43, m44 - mat.m44};
}

Matrix4x4& Matrix4x4::operator-=(const Matrix4x4& mat) {
    m11 -= mat.m11;
    m12 -= mat.m12;
    m13 -= mat.m13;
    m14 -= mat.m14;
    m21 -= mat.m21;
    m22 -= mat.m22;
    m23 -= mat.m23;
    m24 -= mat.m24;
    m31 -= mat.m31;
    m32 -= mat.m32;
    m33 -= mat.m33;
    m34 -= mat.m34;
    m41 -= mat.m41;
    m42 -= mat.m42;
    m43 -= mat.m43;
    m44 -= mat.m44;
    return *this;
}

Matrix4x4 Matrix4x4::operator-() const {
    return {-m11, -m12, -m13, -m14, -m21, -m22, -m23, -m24,
            -m31, -m32, -m33, -m34, -m41, -m42, -m43, -m44};
}

bool Matrix4x4::operator==(const Matrix4x4& mat) const {
    return m11 == mat.m11 && m12 == mat.m12 && m13 == mat.m13 && m14 == mat.m14 &&
           m21 == mat.m21 && m22 == mat.m22 && m23 == mat.m23 && m24 == mat.m24 &&
           m31 == mat.m31 && m32 == mat.m32 && m33 == mat.m33 && m34 == mat.m34 &&
           m41 == mat.m41 && m42 == mat.m42 && m43 == mat.m43 && m44 == mat.m44;
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

    os << std::right << "| " << std::setw(2) << val(mat.m11) << "  " << std::left
       << std::setw(2) << val(mat.m12) << " " << std::setw(2) << val(mat.m13) << " "
       << std::setw(2) << val(mat.m14) << " |" << std::endl;
    os << std::right << "| " << std::setw(2) << val(mat.m21) << "  " << std::left
       << std::setw(2) << val(mat.m22) << " " << std::setw(2) << val(mat.m23) << " "
       << std::setw(2) << val(mat.m24) << " |" << std::endl;
    os << std::right << "| " << std::setw(2) << val(mat.m31) << "  " << std::left
       << std::setw(2) << val(mat.m32) << " " << std::setw(2) << val(mat.m33) << " "
       << std::setw(2) << val(mat.m34) << " |" << std::endl;
    os << std::right << "| " << std::setw(2) << val(mat.m41) << "  " << std::left
       << std::setw(2) << val(mat.m42) << " " << std::setw(2) << val(mat.m43) << " "
       << std::setw(2) << val(mat.m44) << " |" << std::endl;
    return os;
}

float Matrix4x4::trace() const {
    return m11 + m22 + m33 + m44;
}

float Matrix4x4::det() const {
    // Apply Sarrus rule to the top row
    const float det0 = m22 * m33 * m44 - m22 * m34 * m43 - m32 * m23 * m44 +
                       m32 * m24 * m43 + m42 * m23 * m34 - m42 * m24 * m33;
    const float det1 = -m21 * m33 * m44 + m21 * m34 * m43 + m31 * m23 * m44 -
                       m31 * m24 * m43 - m41 * m23 * m34 + m41 * m24 * m33;
    const float det2 = m21 * m32 * m44 - m21 * m34 * m42 - m31 * m22 * m44 +
                       m31 * m24 * m42 + m41 * m22 * m34 - m41 * m24 * m32;
    const float det3 = -m21 * m32 * m43 + m21 * m33 * m42 + m31 * m22 * m43 -
                       m31 * m23 * m42 - m41 * m22 * m33 + m41 * m23 * m32;

    return m11 * det0 + m12 * det1 + m13 * det2 + m14 * det3;
}

Vector3 math::operator*(const Vector3& v, const Matrix4x4& mat) {
    return {mat.m11 * v.x + mat.m21 * v.y + mat.m31 * v.z + mat.m41,
            mat.m12 * v.x + mat.m22 * v.y + mat.m32 * v.z + mat.m42,
            mat.m13 * v.x + mat.m23 * v.y + mat.m33 * v.z + mat.m43};
}

Matrix4x4 math::operator*(float scalar, const Matrix4x4& mat) {
    return mat * scalar;
}

Matrix4x4 math::transpose(const Matrix4x4& mat) {
    return {mat.m11, mat.m21, mat.m31, mat.m41, 
            mat.m12, mat.m22, mat.m32, mat.m42,
            mat.m13, mat.m23, mat.m33, mat.m43, 
            mat.m14, mat.m24, mat.m34, mat.m44};
}

Matrix4x4 math::inverse(const Matrix4x4& mat) {
    Matrix4x4 inv;

    inv(0, 0) = mat.m22 * mat.m33 * mat.m44 + mat.m23 * mat.m34 * mat.m42 +
                mat.m24 * mat.m32 * mat.m43 - mat.m22 * mat.m34 * mat.m43 -
                mat.m23 * mat.m32 * mat.m44 - mat.m24 * mat.m33 * mat.m42;
    inv(0, 1) = mat.m12 * mat.m34 * mat.m43 + mat.m13 * mat.m32 * mat.m44 +
                mat.m14 * mat.m33 * mat.m42 - mat.m12 * mat.m33 * mat.m44 -
                mat.m13 * mat.m34 * mat.m42 - mat.m14 * mat.m32 * mat.m43;
    inv(0, 2) = mat.m12 * mat.m23 * mat.m44 + mat.m13 * mat.m24 * mat.m42 +
                mat.m14 * mat.m22 * mat.m43 - mat.m12 * mat.m24 * mat.m43 -
                mat.m13 * mat.m22 * mat.m44 - mat.m14 * mat.m23 * mat.m42;
    inv(0, 3) = mat.m12 * mat.m24 * mat.m33 + mat.m13 * mat.m22 * mat.m34 +
                mat.m14 * mat.m23 * mat.m32 - mat.m12 * mat.m23 * mat.m34 -
                mat.m13 * mat.m24 * mat.m32 - mat.m14 * mat.m22 * mat.m33;

    inv(1, 0) = mat.m21 * mat.m34 * mat.m43 + mat.m23 * mat.m31 * mat.m44 +
                mat.m24 * mat.m33 * mat.m41 - mat.m21 * mat.m33 * mat.m44 -
                mat.m23 * mat.m34 * mat.m41 - mat.m24 * mat.m31 * mat.m43;
    inv(1, 1) = mat.m11 * mat.m33 * mat.m44 + mat.m13 * mat.m34 * mat.m41 +
                mat.m14 * mat.m31 * mat.m43 - mat.m11 * mat.m34 * mat.m43 -
                mat.m13 * mat.m31 * mat.m44 - mat.m14 * mat.m33 * mat.m41;
    inv(1, 2) = mat.m11 * mat.m24 * mat.m43 + mat.m13 * mat.m21 * mat.m44 +
                mat.m14 * mat.m23 * mat.m41 - mat.m11 * mat.m23 * mat.m44 -
                mat.m13 * mat.m24 * mat.m41 - mat.m14 * mat.m21 * mat.m43;
    inv(1, 3) = mat.m11 * mat.m23 * mat.m34 + mat.m13 * mat.m24 * mat.m31 +
                mat.m14 * mat.m21 * mat.m33 - mat.m11 * mat.m24 * mat.m33 -
                mat.m13 * mat.m21 * mat.m34 - mat.m14 * mat.m23 * mat.m31;

    inv(2, 0) = mat.m21 * mat.m32 * mat.m44 + mat.m22 * mat.m34 * mat.m41 +
                mat.m24 * mat.m31 * mat.m42 - mat.m21 * mat.m34 * mat.m42 -
                mat.m22 * mat.m31 * mat.m44 - mat.m24 * mat.m32 * mat.m41;
    inv(2, 1) = mat.m11 * mat.m34 * mat.m42 + mat.m12 * mat.m31 * mat.m44 +
                mat.m14 * mat.m32 * mat.m41 - mat.m11 * mat.m32 * mat.m44 -
                mat.m12 * mat.m34 * mat.m41 - mat.m14 * mat.m31 * mat.m42;
    inv(2, 2) = mat.m11 * mat.m22 * mat.m44 + mat.m12 * mat.m24 + mat.m41 +
                mat.m14 * mat.m21 * mat.m42 - mat.m11 * mat.m24 * mat.m42 -
                mat.m12 * mat.m21 * mat.m44 - mat.m14 * mat.m22 * mat.m41;
    inv(2, 3) = mat.m11 * mat.m24 * mat.m32 + mat.m12 * mat.m21 * mat.m34 +
                mat.m14 * mat.m22 * mat.m31 - mat.m11 * mat.m22 * mat.m34 -
                mat.m12 * mat.m24 * mat.m31 - mat.m14 * mat.m21 * mat.m32;

    inv(3, 0) = mat.m21 * mat.m33 * mat.m42 + mat.m22 * mat.m31 * mat.m43 +
                mat.m23 * mat.m32 * mat.m41 - mat.m21 * mat.m32 * mat.m43 -
                mat.m22 * mat.m33 * mat.m41 - mat.m23 * mat.m31 * mat.m42;
    inv(3, 1) = mat.m11 * mat.m32 * mat.m43 + mat.m12 * mat.m33 * mat.m41 +
                mat.m13 * mat.m31 * mat.m42 - mat.m11 * mat.m33 * mat.m42 -
                mat.m12 * mat.m31 * mat.m43 - mat.m13 * mat.m32 * mat.m41;
    inv(3, 2) = mat.m11 * mat.m23 * mat.m42 + mat.m12 * mat.m21 * mat.m43 +
                mat.m13 * mat.m22 * mat.m41 - mat.m11 * mat.m22 * mat.m43 -
                mat.m12 * mat.m23 * mat.m41 - mat.m13 * mat.m21 * mat.m42;
    inv(3, 3) = mat.m11 * mat.m22 * mat.m33 + mat.m12 * mat.m23 * mat.m31 +
                mat.m13 * mat.m21 * mat.m32 - mat.m11 * mat.m23 * mat.m32 -
                mat.m12 * mat.m21 * mat.m33 - mat.m13 * mat.m22 * mat.m31;

    float det = mat.m11 * inv(0, 0) + mat.m12 * inv(1, 0) + mat.m13 * inv(2, 0) +
                mat.m14 * inv(3, 0);

    if (det == 0)
        return Matrix4x4(0);

    return (1.0f / det) * inv;
}