#ifndef PBR_MATRIX2X2_H
#define PBR_MATRIX2X2_H

#include <PBR.h>

namespace pbr {
namespace math {

class Vector2;
class Matrix3x3;

class Matrix2x2 {
public:
    Matrix2x2();
    explicit Matrix2x2(float scalar);
    Matrix2x2(float m11, float m12, float m21, float m22);
    explicit Matrix2x2(const Matrix3x3& mat);

    Matrix2x2 operator*(float scalar) const;
    Matrix2x2& operator*=(float scalar);

    Vector2 operator*(const Vector2& v) const;
    Matrix2x2 operator*(const Matrix2x2& mat) const;

    Matrix2x2 operator+(const Matrix2x2& mat) const;
    Matrix2x2& operator+=(const Matrix2x2& mat);

    Matrix2x2 operator-(const Matrix2x2& mat) const;
    Matrix2x2& operator-=(const Matrix2x2& mat);

    Matrix2x2 operator-() const;

    bool operator==(const Matrix2x2& mat) const;
    bool operator!=(const Matrix2x2& mat) const;

    float operator()(unsigned int i, unsigned int j) const;
    float& operator()(unsigned int i, unsigned int j);

    float trace() const;
    float det() const;

    const float* data() const { return &m[0][0]; }

private:
    float m[2][2];
};

std::istream& operator>>(std::istream& is, Matrix2x2& mat);
std::ostream& operator<<(std::ostream& os, const Matrix2x2& mat);

Vector2 operator*(const Vector2& v, const Matrix2x2& mat);
Matrix2x2 operator*(float scalar, const Matrix2x2& mat);

Matrix2x2 Transpose(const Matrix2x2& mat);
Matrix2x2 Inverse(const Matrix2x2& mat);

} // namespace math
} // namespace pbr

#endif