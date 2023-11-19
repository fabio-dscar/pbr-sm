#ifndef __PBR_VECTOR3_H__
#define __PBR_VECTOR3_H__

#include <PBR.h>

namespace pbr {
namespace math {

class Vector4;

class PBR_SHARED Vector3 {
public:
    float x, y, z;

    Vector3();
    Vector3(float scalar);
    Vector3(float x, float y, float z);
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
};

PBR_SHARED std::istream& operator>>(std::istream& is, Vector3& v);
PBR_SHARED std::ostream& operator<<(std::ostream& os, const Vector3& v);

PBR_SHARED Vector3 operator*(float scalar, const Vector3& v);

PBR_SHARED Vector3 abs(const Vector3& v);
PBR_SHARED Vector3 cross(const Vector3& v1, const Vector3& v2);
PBR_SHARED Vector3 pow(const Vector3& v, float exp);
PBR_SHARED Vector3 normalize(const Vector3& v);
PBR_SHARED Vector3 min(const Vector3& v1, const Vector3& v2);
PBR_SHARED Vector3 max(const Vector3& v1, const Vector3& v2);

PBR_SHARED float distance(const Vector3& v1, const Vector3& v2);
PBR_SHARED float dot(const Vector3& v1, const Vector3& v2);
PBR_SHARED float absDot(const Vector3& v1, const Vector3& v2);

PBR_SHARED void basisFromVector(const Vector3& v1, Vector3* v2, Vector3* v3);

} // namespace math
} // namespace pbr

#endif