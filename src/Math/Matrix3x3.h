#ifndef PBR_MATRIX3X3_H
#define PBR_MATRIX3X3_H

#include <PBR.h>

namespace pbr {
namespace math {

class Vector3;
class Matrix4x4;

class Matrix3x3 {
public:
    Matrix3x3();
    explicit Matrix3x3(float scalar);
    Matrix3x3(float m11, float m12, float m13, float m21, float m22, float m23, float m31,
              float m32, float m33);
    Matrix3x3(const Vector3& col0, const Vector3& col1, const Vector3& col2);
    explicit Matrix3x3(const Matrix4x4& mat);

    Matrix3x3 operator*(float scalar) const;
    Matrix3x3& operator*=(float scalar);

    Vector3 operator*(const Vector3& v) const;
    Matrix3x3 operator*(const Matrix3x3& mat) const;

    Matrix3x3 operator+(const Matrix3x3& mat) const;
    Matrix3x3& operator+=(const Matrix3x3& mat);

    Matrix3x3 operator-(const Matrix3x3& mat) const;
    Matrix3x3& operator-=(const Matrix3x3& mat);

    Matrix3x3 operator-() const;

    bool operator==(const Matrix3x3& mat) const;
    bool operator!=(const Matrix3x3& mat) const;

    float operator()(uint32 i, uint32 j) const;
    float& operator()(uint32 i, uint32 j);

    float trace() const;
    float det() const;

    const float* data() const { return &m[0][0]; }

private:
    float m[3][3];
};

std::istream& operator>>(std::istream& is, Matrix3x3& mat);
std::ostream& operator<<(std::ostream& os, const Matrix3x3& mat);

Vector3 operator*(const Vector3& v, const Matrix3x3& mat);
Matrix3x3 operator*(float scalar, const Matrix3x3& mat);

Matrix3x3 Transpose(const Matrix3x3& mat);
Matrix3x3 Inverse(const Matrix3x3& mat);

} // namespace math
} // namespace pbr

#endif