#include <Vector3.h>

#include <cmath>
#include <algorithm>

#include <PBRMath.h>

using namespace pbr;
using namespace pbr::math;

Vector3::Vector3(const Vector4& v) : x(v.x), y(v.y), z(v.z) {
    if (v.w != 0) {
        x /= v.w;
        y /= v.w;
        z /= v.w;
    }
}

Vector3 Vector3::operator+(const Vector3& v) const {
    return {x + v.x, y + v.y, z + v.z};
}

Vector3& Vector3::operator+=(const Vector3& v) {
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
}

Vector3 Vector3::operator-(const Vector3& v) const {
    return Vector3(x - v.x, y - v.y, z - v.z);
}

Vector3& Vector3::operator-=(const Vector3& v) {
    x -= v.x;
    y -= v.y;
    z -= v.z;
    return *this;
}

Vector3 Vector3::operator*(float scalar) const {
    return {scalar * x, scalar * y, scalar * z};
}

Vector3& Vector3::operator*=(float scalar) {
    x *= scalar;
    y *= scalar;
    z *= scalar;
    return *this;
}

Vector3 Vector3::operator/(float scalar) const {
    return {x / scalar, y / scalar, z / scalar};
}

Vector3& Vector3::operator/=(float scalar) {
    x /= scalar;
    y /= scalar;
    z /= scalar;
    return *this;
}

Vector3 Vector3::operator-() const {
    return {-x, -y, -z};
}

bool Vector3::operator==(const Vector3& v) const {
    return (x == v.x && y == v.y && z == v.z);
}

bool Vector3::operator!=(const Vector3& v) const {
    return !(*this == v);
}

float Vector3::operator[](uint32 idx) const {
    if (idx == 0)
        return x;

    if (idx == 1)
        return y;

    return z;
}

float& Vector3::operator[](uint32 idx) {
    if (idx == 0)
        return x;

    if (idx == 1)
        return y;

    return z;
}

float Vector3::lengthSqr() const {
    return x * x + y * y + z * z;
}

float Vector3::length() const {
    return std::sqrt(lengthSqr());
}

void Vector3::normalize() {
    float lenSqr = lengthSqr();
    if (lenSqr > 0)
        *this /= std::sqrt(lenSqr);
}

float Vector3::min() const {
    return std::min(x, std::min(y, z));
}

float Vector3::max() const {
    return std::max(x, std::max(y, z));
}

unsigned int Vector3::maxDim() const {
    if (x > y) {
        if (x > z)
            return 0;
        else
            return 2;
    } else {
        if (y > z)
            return 1;
        else
            return 2;
    }
}

unsigned int Vector3::minDim() const {
    if (x < y) {
        if (x < z)
            return 0;
        else
            return 2;
    } else {
        if (y < z)
            return 1;
        else
            return 2;
    }
}

bool Vector3::isInfinite() const {
    return std::abs(x) == FLOAT_INFINITY || std::abs(y) == FLOAT_INFINITY ||
           std::abs(z) == FLOAT_INFINITY;
}

bool Vector3::isZero() const {
    return x == 0 && y == 0 && z == 0;
}

std::istream& math::operator>>(std::istream& is, Vector3& v) {
    is >> v.x;
    is >> v.y;
    is >> v.z;
    return is;
}

std::ostream& math::operator<<(std::ostream& os, const Vector3& v) {
    os << "Vector3: [" << v.x << ", " << v.y << ", " << v.z << "]";
    return os;
}

Vector3 math::operator*(float scalar, const Vector3& v) {
    return v * scalar;
}

Vector3 math::abs(const Vector3& v) {
    return {std::abs(v.x), std::abs(v.y), std::abs(v.z)};
}

Vector3 math::normalize(const Vector3& v) {
    float lenSqr = v.lengthSqr();
    if (lenSqr > 0)
        return v / std::sqrt(lenSqr);
    return {0};
}

Vector3 math::min(const Vector3& v1, const Vector3& v2) {
    return {std::min(v1.x, v2.x), std::min(v1.y, v2.y), std::min(v1.z, v2.z)};
}

Vector3 math::max(const Vector3& v1, const Vector3& v2) {
    return {std::max(v1.x, v2.x), std::max(v1.y, v2.y), std::max(v1.z, v2.z)};
}

float math::dot(const Vector3& v1, const Vector3& v2) {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

float math::absDot(const Vector3& v1, const Vector3& v2) {
    return std::abs(math::dot(v1, v2));
}

Vector3 math::cross(const Vector3& v1, const Vector3& v2) {
    return {(v1.y * v2.z) - (v1.z * v2.y), (v1.z * v2.x) - (v1.x * v2.z),
            (v1.x * v2.y) - (v1.y * v2.x)};
}

float math::distance(const Vector3& v1, const Vector3& v2) {
    Vector3 v = v1 - v2;
    return v.length();
}

Vector3 math::pow(const Vector3& v, float exp) {
    return {std::pow(v.x, exp), std::pow(v.y, exp), std::pow(v.z, exp)};
}

void math::basisFromVector(const Vector3& v1, Vector3* v2, Vector3* v3) {
    // Reference: [Duff et. al, 2017] - "Building an Orthonormal Basis, Revisited"
    const float sign = std::copysign(1.0f, v1.z);
    const float a = -1.0f / (sign + v1.z);
    const float b = v1.x * v1.y * a;

    *v2 = Vector3(1.0f + sign * v1.x * v1.x * a, sign * b, -sign * v1.x);
    *v3 = Vector3(b, sign + v1.y * v1.y * a, -v1.y);
}