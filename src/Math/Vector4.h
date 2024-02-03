#ifndef PBR_VECTOR4_H
#define PBR_VECTOR4_H

#include <PBR.h>

namespace pbr {
namespace math {

class Vector3;

class Vector4 {
public:
    float x = 0, y = 0, z = 0, w = 1;

    Vector4() = default;
    Vector4(float s) : x(s), y(s), z(s), w(1) {}
    Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
    Vector4(const Vector3& v, float w);

    Vector4 operator*(float scalar) const;
    Vector4& operator*=(float scalar);

    Vector4 operator/(float scalar) const;
    Vector4& operator/=(float scalar);

    Vector4 operator+(const Vector4& v) const;
    Vector4& operator+=(const Vector4& v);

    Vector4 operator-(const Vector4& v) const;
    Vector4& operator-=(const Vector4& v);

    Vector4 operator-() const;

    bool operator==(const Vector4& v) const;
    bool operator!=(const Vector4& v) const;

    float operator[](unsigned int idx) const;
    float& operator[](unsigned int idx);

    float lengthSqr() const;
    float length() const;

    void normalize();

    float min() const;
    float max() const;

    bool isZero() const;
};

std::istream& operator>>(std::istream& is, Vector4& v);
std::ostream& operator<<(std::ostream& os, const Vector4& v);

Vector4 operator*(float scalar, const Vector4& v);

Vector4 Abs(const Vector4& v);
Vector4 Normalize(const Vector4& v);

float Distance(const Vector4& v1, const Vector4& v2);
float Dot(const Vector4& v1, const Vector4& v2);
float AbsDot(const Vector4& v1, const Vector4& v2);

} // namespace math
} // namespace pbr

#endif