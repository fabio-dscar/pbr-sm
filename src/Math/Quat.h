#ifndef PBR_QUAT_H
#define PBR_QUAT_H

#include <PBR.h>
#include <Vector3.h>

namespace pbr {
namespace math {

class Matrix4x4;

class Quat {
public:
    float x = 0, y = 0, z = 0, w = 1;

    Quat() = default;
    Quat(const Vector3& v, float w) : x(v.x), y(v.y), z(v.z), w(w) {}
    Quat(float x, float y, float z, float w) : x(x), y(y), z(z), w(w){};
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

    float operator[](unsigned int idx) const;
    float& operator[](unsigned int idx);

    Quat conj() const;

    float lengthSqr() const;
    float length() const;

    void normalize();

    Matrix4x4 toMatrix() const;
};

std::istream& operator>>(std::istream& is, Quat& q);
std::ostream& operator<<(std::ostream& os, const Quat& q);

Quat operator*(float scalar, const Quat& q);

float Dot(const Quat& q1, const Quat& q2);
Quat Normalize(const Quat& q);

Quat Slerp(float t, const Quat& q1, const Quat& q2);
Vector3 Rotate(const Quat& q, const Vector3& v);
Quat AxisAngle(const Vector3& axis, float angle);
Quat RotationAlign(const Vector3& from, const Vector3& to);

} // namespace math
} // namespace pbr

#endif
