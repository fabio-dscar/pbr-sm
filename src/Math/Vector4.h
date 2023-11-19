#ifndef __PBR_VECTOR4_H__
#define __PBR_VECTOR4_H__

#include <PBR.h>

namespace pbr {
namespace math {

class Vector3;

class PBR_SHARED Vector4 {
public:
    float x, y, z, w;

    Vector4();
    Vector4(float scalar);
    Vector4(float x, float y, float z, float w);
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

    float operator[](uint32 idx) const;
    float& operator[](uint32 idx);

    float lengthSqr() const;
    float length() const;

    void normalize();

    float min() const;
    float max() const;
};

PBR_SHARED std::istream& operator>>(std::istream& is, Vector4& v);
PBR_SHARED std::ostream& operator<<(std::ostream& os, const Vector4& v);

PBR_SHARED Vector4 operator*(float scalar, const Vector4& v);

PBR_SHARED Vector4 abs(const Vector4& v);
PBR_SHARED Vector4 normalize(const Vector4& v);

PBR_SHARED float distance(const Vector4& v1, const Vector4& v2);
PBR_SHARED float dot(const Vector4& v1, const Vector4& v2);
PBR_SHARED float absDot(const Vector4& v1, const Vector4& v2);

} // namespace math
} // namespace pbr

#endif