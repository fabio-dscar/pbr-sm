#ifndef __PBR_VECTOR2_H__
#define __PBR_VECTOR2_H__

#include <PBR.h>

namespace pbr {
namespace math {

// Forward declaration
class Vector3;

class Vector2 {
public:
    float x = 0, y = 0;

    Vector2() = default;
    Vector2(float s) : x(s), y(s) {}
    Vector2(float x, float y) : x(x), y(y) {}
    explicit Vector2(const Vector3& v);

    Vector2 operator*(float scalar) const;
    Vector2& operator*=(float scalar);

    Vector2 operator/(float scalar) const;
    Vector2& operator/=(float scalar);

    Vector2 operator+(const Vector2& v) const;
    Vector2& operator+=(const Vector2& v);

    Vector2 operator-(const Vector2& v) const;
    Vector2& operator-=(const Vector2& v);

    Vector2 operator-() const;

    bool operator==(const Vector2& v) const;
    bool operator!=(const Vector2& v) const;

    float operator[](uint32 idx) const;
    float& operator[](uint32 idx);

    float lengthSqr() const;
    float length() const;

    void normalize();

    float min() const;
    float max() const;
    unsigned int maxDim() const;
    unsigned int minDim() const;

    bool isZero() const;
};

std::istream& operator>>(std::istream& is, Vector2& v);
std::ostream& operator<<(std::ostream& os, const Vector2& v);

Vector2 operator*(float scalar, const Vector2& v);

Vector2 abs(const Vector2& v);
Vector2 pow(const Vector2& v, float exp);
Vector2 normalize(const Vector2& v);
Vector2 min(const Vector2& v1, const Vector2& v2);
Vector2 max(const Vector2& v1, const Vector2& v2);

float distance(const Vector2& v1, const Vector2& v2);
float dot(const Vector2& v1, const Vector2& v2);
float absDot(const Vector2& v1, const Vector2& v2);
} // namespace math
} // namespace pbr

#endif