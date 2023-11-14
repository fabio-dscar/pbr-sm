#ifndef __PBR_MATRIX2X2_H__
#define __PBR_MATRIX2X2_H__

#include <PBR.h>

namespace pbr {
namespace math {

class Vector2;
class Matrix3x3;

class PBR_SHARED Matrix2x2 {
public:
    union {
        struct {
            float m11, m21;
            float m12, m22;
        };

        float m[2][2]; // Column major storage _m[col][row]
    };

    Matrix2x2();
    explicit Matrix2x2(float scalar);
    Matrix2x2(float m11, float m12, float m21, float m22);
    Matrix2x2(const Vector2& col0, const Vector2& col1);
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

    float operator()(uint32 i, uint32 j) const;
    float& operator()(uint32 i, uint32 j);

    float trace() const;
    float det() const;
};

// Standard input/ouput
PBR_SHARED std::istream& operator>>(std::istream& is, Matrix2x2& mat);
PBR_SHARED std::ostream& operator<<(std::ostream& os, const Matrix2x2& mat);

PBR_SHARED Vector2 operator*(const Vector2& v, const Matrix2x2& mat);
PBR_SHARED Matrix2x2 operator*(float scalar, const Matrix2x2& mat);

PBR_SHARED Matrix2x2 transpose(const Matrix2x2& mat);
PBR_SHARED Matrix2x2 inverse(const Matrix2x2& mat);

} // namespace math
} // namespace pbr

#endif