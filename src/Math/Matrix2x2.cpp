#include <Matrix2x2.h>

#include <Vector2.h>
#include <Matrix3x3.h>

using namespace pbr;
using namespace pbr::math;

Matrix2x2::Matrix2x2() : m11(1), m12(0), 
                         m21(0), m22(1) {}

Matrix2x2::Matrix2x2(float scalar) : m{scalar, scalar, scalar, scalar} {}

Matrix2x2::Matrix2x2(float m11, float m12, float m21, float m22)
    : m11(m11), m12(m12), m21(m21), m22(m22) {}

Matrix2x2::Matrix2x2(const Vector2& col0, const Vector2& col1)
    : m11(col0.x), m12(col1.x), m21(col0.y), m22(col1.y) {}

Matrix2x2::Matrix2x2(const Matrix3x3& mat)
    : m11(mat.m11), m12(mat.m12), m21(mat.m21), m22(mat.m22) {}

Matrix2x2 Matrix2x2::operator*(float scalar) const {
    return Matrix2x2(scalar * m11, scalar * m12, scalar * m21, scalar * m22);
}

Matrix2x2& Matrix2x2::operator*=(float scalar) {
    m11 *= scalar;
    m12 *= scalar;
    m21 *= scalar;
    m22 *= scalar;
    return *this;
}

Vector2 Matrix2x2::operator*(const Vector2& v) const {
    return Vector2(m11 * v.x + m12 * v.y, m21 * v.x + m22 * v.y);
}

Matrix2x2 Matrix2x2::operator*(const Matrix2x2& mat) const {
    return Matrix2x2(m11 * mat.m11 + m12 * mat.m21, m11 * mat.m12 + m12 * mat.m22,
                     m21 * mat.m11 + m22 * mat.m21, m21 * mat.m12 + m22 * mat.m22);
}

Matrix2x2 Matrix2x2::operator+(const Matrix2x2& mat) const {
    return Matrix2x2(m11 + mat.m11, m12 + mat.m12, m21 + mat.m21, m22 + mat.m22);
}

Matrix2x2& Matrix2x2::operator+=(const Matrix2x2& mat) {
    m11 += mat.m11;
    m12 += mat.m12;
    m21 += mat.m21;
    m22 += mat.m22;
    return *this;
}

Matrix2x2 Matrix2x2::operator-(const Matrix2x2& mat) const {
    return Matrix2x2(m11 - mat.m11, m12 - mat.m12, m21 - mat.m21, m22 - mat.m22);
}

Matrix2x2& Matrix2x2::operator-=(const Matrix2x2& mat) {
    m11 -= mat.m11;
    m12 -= mat.m12;
    m21 -= mat.m21;
    m22 -= mat.m22;
    return *this;
}

Matrix2x2 Matrix2x2::operator-() const {
    return Matrix2x2(-m11, -m12, -m21, -m22);
}

bool Matrix2x2::operator==(const Matrix2x2& mat) const {
    return m11 == mat.m11 && m12 == mat.m12 && m21 == mat.m21 && m22 == mat.m22;
}

bool Matrix2x2::operator!=(const Matrix2x2& mat) const {
    return !(*this == mat);
}

float Matrix2x2::operator()(uint32 i, uint32 j) const {
    return m[j][i];
}

float& Matrix2x2::operator()(uint32 i, uint32 j) {
    return m[j][i];
}

std::istream& math::operator>>(std::istream& is, Matrix2x2& mat) {
    Vector2 col1, col2;

    is >> col1;
    is >> col2;

    mat = Matrix2x2(col1, col2);

    return is;
}

std::ostream& math::operator<<(std::ostream& os, const Matrix2x2& mat) {
    auto val = [](float v) -> float {
        if (std::abs(v) < 1e-6)
            return 0;
        return v;
    };

    os << std::right << "| " << std::setw(2) << val(mat.m11) << "  " << std::left
       << std::setw(2) << val(mat.m12) << " |\n";
    os << std::right << "| " << std::setw(2) << val(mat.m21) << "  " << std::left
       << std::setw(2) << val(mat.m22) << " |\n";
    return os;
}

float Matrix2x2::trace() const {
    return m11 * m22;
}

float Matrix2x2::det() const {
    return m11 * m22 - m12 * m21;
}

// Non-member functions
Vector2 math::operator*(const Vector2& v, const Matrix2x2& mat) {
    return Vector2(v.x * mat.m11 + v.y * mat.m21, v.x * mat.m12 + v.y * mat.m22);
}

Matrix2x2 math::operator*(float scalar, const Matrix2x2& mat) {
    return mat * scalar;
}

Matrix2x2 math::transpose(const Matrix2x2& mat) {
    return Matrix2x2(mat.m11, mat.m21, mat.m12, mat.m22);
}

Matrix2x2 math::inverse(const Matrix2x2& mat) {
    const float det = mat.det();
    if (det == 0)
        return Matrix2x2(0);

    const float invDet = 1.0f / det;
    const float inv11 = invDet * mat.m22;
    const float inv12 = invDet * -mat.m12;
    const float inv21 = invDet * -mat.m21;
    const float inv22 = invDet * mat.m11;

    return Matrix2x2(inv11, inv12, inv21, inv22);
}