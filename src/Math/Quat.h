#ifndef __PBR_QUAT_H__
#define __PBR_QUAT_H__

#include <PBR.h>

namespace pbr {
namespace math {

class Vector3;
class Matrix4x4;

class PBR_SHARED Quat {
public:
    float x, y, z, w;

    Quat();
    Quat(const Vector3& v, float w);
    Quat(float x, float y, float z, float w);
    explicit Quat(const Matrix4x4& mat);

    Quat operator+(const Quat& q) const;
    Quat& operator+=(const Quat& q);

    Quat operator-(const Quat& q) const;
    Quat& operator-=(const Quat& q);

    Quat operator*(float scalar) const;
    Quat& operator*=(float scalar);

    Quat operator*(const Quat& q) const;
    Quat& operator*=(const Quat& q);

    Quat operator/(float scalar) const;
    Quat& operator/=(float scalar);

    float operator[](uint32 idx) const;
    float& operator[](uint32 idx);

    Quat conj() const;

    float lengthSqr() const;
    float length() const;

    void normalize();

    Matrix4x4 toMatrix() const;
};

PBR_SHARED std::istream& operator>>(std::istream& is, Quat& q);
PBR_SHARED std::ostream& operator<<(std::ostream& os, const Quat& q);

PBR_SHARED Quat operator*(float scalar, const Quat& q);

PBR_SHARED float dot(const Quat& q1, const Quat& q2);

PBR_SHARED Quat normalize(const Quat& q);
PBR_SHARED Quat slerp(float t, const Quat& q1, const Quat& q2);

PBR_SHARED Vector3 rotate(const Quat& q, const Vector3& v);

} // namespace math
} // namespace pbr

#endif
