#include <Vector4.h>

#include <cmath>
#include <algorithm>

#include <Vector3.h>

using namespace pbr;
using namespace pbr::math;

/* ============================================================================
        Vector4 Constructors
==============================================================================*/
Vector4::Vector4() : x(0), y(0), z(0), w(0) {}
Vector4::Vector4(float scalar) : x(scalar), y(scalar), z(scalar), w(scalar) {}
Vector4::Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
Vector4::Vector4(const Vector3& v, float w) : x(v.x), y(v.y), z(v.z), w(w) {}

/* ============================================================================
        Vector4 Math Operators
==============================================================================*/
Vector4 Vector4::operator+(const Vector4& v) const {
    return Vector4(x + v.x, y + v.y, z + v.z, w + v.w);
}

Vector4& Vector4::operator+=(const Vector4& v) {
    x += v.x;
    y += v.y;
    z += v.z;
    w += v.w;
    return *this;
}

Vector4 Vector4::operator-(const Vector4& v) const {
    return Vector4(x - v.x, y - v.y, z - v.z, w - v.w);
}

Vector4& Vector4::operator-=(const Vector4& v) {
    x -= v.x;
    y -= v.y;
    z -= v.z;
    w -= v.w;
    return *this;
}

Vector4 Vector4::operator*(float scalar) const {
    return Vector4(scalar * x, scalar * y, scalar * z, scalar * w);
}

Vector4& Vector4::operator*=(float scalar) {
    x *= scalar;
    y *= scalar;
    z *= scalar;
    w *= scalar;
    return *this;
}

Vector4 Vector4::operator/(float scalar) const {
    return Vector4(x / scalar, y / scalar, z / scalar, w / scalar);
}

Vector4& Vector4::operator/=(float scalar) {
    x /= scalar;
    y /= scalar;
    z /= scalar;
    w /= scalar;
    return *this;
}

Vector4 Vector4::operator-() const {
    return Vector4(-x, -y, -z, -w);
}

bool Vector4::operator==(const Vector4& v) const {
    return (x == v.x && y == v.y && z == v.z && w == v.w);
}

bool Vector4::operator!=(const Vector4& v) const {
    return !(*this == v);
}

/* ============================================================================
        Vector4 Access Methods
==============================================================================*/
float Vector4::operator[](uint32 idx) const {
    if (idx == 0)
        return x;

    if (idx == 1)
        return y;

    if (idx == 2)
        return z;

    return w;
}

float& Vector4::operator[](uint32 idx) {
    if (idx == 0)
        return x;

    if (idx == 1)
        return y;

    if (idx == 2)
        return z;

    return w;
}

/* ============================================================================
        Vector4 Member Methods
==============================================================================*/
float Vector4::lengthSqr() const {
    return x * x + y * y + z * z + w * w;
}

float Vector4::length() const {
    return std::sqrt(lengthSqr());
}

void Vector4::normalize() {
    float lenSqr = lengthSqr();
    if (lenSqr > 0)
        *this /= std::sqrt(lenSqr);
}

float Vector4::min() const {
    return std::min(x, std::min(y, std::min(z, w)));
}

float Vector4::max() const {
    return std::max(x, std::max(y, std::max(z, w)));
}

std::istream& math::operator>>(std::istream& is, Vector4& v) {
    is >> v.x;
    is >> v.y;
    is >> v.z;
    is >> v.w;
    return is;
}

std::ostream& math::operator<<(std::ostream& os, const Vector4& v) {
    os << "Vector4: [" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << "]\n";
    return os;
}

/* ============================================================================
        Vector4 Non-Member Functions
==============================================================================*/
Vector4 math::operator*(float scalar, const Vector4& v) {
    return v * scalar;
}

Vector4 math::abs(const Vector4& v) {
    return Vector4(std::abs(v.x), std::abs(v.y), std::abs(v.z), std::abs(v.w));
}

Vector4 math::normalize(const Vector4& v) {
    float lenSqr = v.lengthSqr();
    if (lenSqr > 0)
        return v / std::sqrt(lenSqr);
    return Vector4(0);
}

float math::dot(const Vector4& v1, const Vector4& v2) {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
}

float math::absDot(const Vector4& v1, const Vector4& v2) {
    return std::abs(math::dot(v1, v2));
}

float math::distance(const Vector4& v1, const Vector4& v2) {
    Vector4 v = v1 - v2;
    return v.length();
}