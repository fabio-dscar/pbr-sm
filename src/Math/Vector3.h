#ifndef PBR_VECTOR3_H
#define PBR_VECTOR3_H

#include <PBR.h>

namespace pbr {
namespace math {

class Vector4;

class Vector3 {
public:
    float x = 0, y = 0, z = 0;

    Vector3() = default;
    Vector3(float s) : x(s), y(s), z(s) {}
    Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
    Vector3(const Vector4& v);

    Vector3 operator*(float scalar) const;
    Vector3& operator*=(float scalar);

    Vector3 operator/(float scalar) const;
    Vector3& operator/=(float scalar);

    Vector3 operator+(const Vector3& v) const;
    Vector3& operator+=(const Vector3& v);

    Vector3 operator-(const Vector3& v) const;
    Vector3& operator-=(const Vector3& v);

    Vector3 operator-() const;

    bool operator==(const Vector3& v) const;
    bool operator!=(const Vector3& v) const;

    float operator[](unsigned int idx) const;
    float& operator[](unsigned int idx);

    float lengthSqr() const;
    float length() const;

    void normalize();

    float min() const;
    float max() const;
    unsigned int maxDim() const;
    unsigned int minDim() const;

    bool isInfinite() const;
    bool isZero() const;
};

std::istream& operator>>(std::istream& is, Vector3& v);
std::ostream& operator<<(std::ostream& os, const Vector3& v);

Vector3 operator*(float scalar, const Vector3& v);

Vector3 Abs(const Vector3& v);
Vector3 Cross(const Vector3& v1, const Vector3& v2);
Vector3 Pow(const Vector3& v, float exp);
Vector3 Normalize(const Vector3& v);
Vector3 Min(const Vector3& v1, const Vector3& v2);
Vector3 Max(const Vector3& v1, const Vector3& v2);

float Distance(const Vector3& v1, const Vector3& v2);
float Dot(const Vector3& v1, const Vector3& v2);
float AbsDot(const Vector3& v1, const Vector3& v2);

void BasisFromVector(const Vector3& v1, Vector3* v2, Vector3* v3);

} // namespace math
} // namespace pbr

#endif