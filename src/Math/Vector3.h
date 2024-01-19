#ifndef __PBR_VECTOR3_H__
#define __PBR_VECTOR3_H__

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

    float operator[](uint32 idx) const;
    float& operator[](uint32 idx);

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

Vector3 abs(const Vector3& v);
Vector3 cross(const Vector3& v1, const Vector3& v2);
Vector3 pow(const Vector3& v, float exp);
Vector3 normalize(const Vector3& v);
Vector3 min(const Vector3& v1, const Vector3& v2);
Vector3 max(const Vector3& v1, const Vector3& v2);

float distance(const Vector3& v1, const Vector3& v2);
float dot(const Vector3& v1, const Vector3& v2);
float absDot(const Vector3& v1, const Vector3& v2);

void basisFromVector(const Vector3& v1, Vector3* v2, Vector3* v3);

} // namespace math
} // namespace pbr

#endif