#include <Matrix2x2.h>

#include <Vector2.h>
#include <Matrix3x3.h>

using namespace pbr;
using namespace pbr::math;

// clang-format off
Matrix2x2::Matrix2x2() : m{{1, 0}, {0, 1}} {}

Matrix2x2::Matrix2x2(float s) : m{{s, s}, {s, s}} {}

Matrix2x2::Matrix2x2(float m11, float m12, float m21, float m22)
    : m{{m11, m21}, {m12, m22}} {}

Matrix2x2::Matrix2x2(const Matrix3x3& mat)
    : m{{mat(0, 0), mat(1, 0)},{mat(0, 1), mat(1, 1)}} {}
// clang-format on

Matrix2x2 Matrix2x2::operator*(float scalar) const {
    return {scalar * m[0][0], scalar * m[0][1], scalar * m[1][0], scalar * m[1][1]};
}

Matrix2x2& Matrix2x2::operator*=(float scalar) {
    m[0][0] *= scalar;
    m[1][0] *= scalar;
    m[0][1] *= scalar;
    m[1][1] *= scalar;
    return *this;
}

Vector2 Matrix2x2::operator*(const Vector2& v) const {
    return {m[0][0] * v.x + m[1][0] * v.y, m[0][1] * v.x + m[1][1] * v.y};
}

Matrix2x2 Matrix2x2::operator*(const Matrix2x2& mat) const {
    return {m[0][0] * mat.m[0][0] + m[1][0] * mat.m[0][1],
            m[0][0] * mat.m[1][0] + m[1][0] * mat.m[1][1],
            m[0][1] * mat.m[0][0] + m[1][1] * mat.m[0][1],
            m[0][1] * mat.m[1][0] + m[1][1] * mat.m[1][1]};
}

Matrix2x2 Matrix2x2::operator+(const Matrix2x2& mat) const {
    return {m[0][0] + mat.m[0][0], m[1][0] + mat.m[1][0], m[0][1] + mat.m[0][1],
            m[1][1] + mat.m[1][1]};
}

Matrix2x2& Matrix2x2::operator+=(const Matrix2x2& mat) {
    m[0][0] += mat.m[0][0];
    m[1][0] += mat.m[1][0];
    m[0][1] += mat.m[0][1];
    m[1][1] += mat.m[1][1];
    return *this;
}

Matrix2x2 Matrix2x2::operator-(const Matrix2x2& mat) const {
    return {m[0][0] - mat.m[0][0], m[1][0] - mat.m[1][0], m[0][1] - mat.m[0][1],
            m[1][1] - mat.m[1][1]};
}

Matrix2x2& Matrix2x2::operator-=(const Matrix2x2& mat) {
    m[0][0] -= mat.m[0][0];
    m[1][0] -= mat.m[1][0];
    m[0][1] -= mat.m[0][1];
    m[1][1] -= mat.m[1][1];
    return *this;
}

Matrix2x2 Matrix2x2::operator-() const {
    return {-m[0][0], -m[1][0], -m[0][1], -m[1][1]};
}

bool Matrix2x2::operator==(const Matrix2x2& mat) const {
    return m[0][0] == mat.m[0][0] && m[1][0] == mat.m[1][0] && m[0][1] == mat.m[0][1] &&
           m[1][1] == mat.m[1][1];
}

bool Matrix2x2::operator!=(const Matrix2x2& mat) const {
    return !(*this == mat);
}

float Matrix2x2::operator()(unsigned int i, unsigned int j) const {
    return m[j][i];
}

float& Matrix2x2::operator()(unsigned int i, unsigned int j) {
    return m[j][i];
}

std::istream& math::operator>>(std::istream& is, Matrix2x2& mat) {
    Vector2 col1, col2;

    is >> col1;
    is >> col2;

    mat = {col1.x, col1.y, col2.x, col2.y};

    return is;
}

std::ostream& math::operator<<(std::ostream& os, const Matrix2x2& mat) {
    auto val = [](float v) -> float {
        if (std::abs(v) < 1e-6)
            return 0;
        return v;
    };

    os << std::right << "| " << std::setw(2) << val(mat(0, 0)) << "  " << std::left
       << std::setw(2) << val(mat(0, 1)) << " |\n";
    os << std::right << "| " << std::setw(2) << val(mat(1, 0)) << "  " << std::left
       << std::setw(2) << val(mat(1, 1)) << " |\n";
    return os;
}

float Matrix2x2::trace() const {
    return m[0][0] * m[1][1];
}

float Matrix2x2::det() const {
    return m[0][0] * m[1][1] - m[1][0] * m[0][1];
}

Vector2 math::operator*(const Vector2& v, const Matrix2x2& mat) {
    return {v.x * mat(0, 0) + v.y * mat(1, 0), v.x * mat(0, 1) + v.y * mat(1, 1)};
}

Matrix2x2 math::operator*(float scalar, const Matrix2x2& mat) {
    return mat * scalar;
}

Matrix2x2 math::Transpose(const Matrix2x2& mat) {
    return {mat(0, 0), mat(1, 0), mat(0, 1), mat(1, 1)};
}

Matrix2x2 math::Inverse(const Matrix2x2& mat) {
    const float det = mat.det();
    if (det == 0)
        return Matrix2x2(0);

    const float invDet = 1.0f / det;
    const float inv11 = invDet * mat(1, 1);
    const float inv12 = invDet * -mat(0, 1);
    const float inv21 = invDet * -mat(1, 0);
    const float inv22 = invDet * mat(0, 0);

    return {inv11, inv12, inv21, inv22};
}