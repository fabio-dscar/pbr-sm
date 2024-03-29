#include <Vector2.h>
#include <Vector3.h>

using namespace pbr;
using namespace pbr::math;

Vector2::Vector2(const Vector3& v) : x(v.x), y(v.y) {}

Vector2 Vector2::operator+(const Vector2& v) const {
    return {x + v.x, y + v.y};
}

Vector2& Vector2::operator+=(const Vector2& v) {
    x += v.x;
    y += v.y;
    return *this;
}

Vector2 Vector2::operator-(const Vector2& v) const {
    return {x - v.x, y - v.y};
}

Vector2& Vector2::operator-=(const Vector2& v) {
    x -= v.x;
    y -= v.y;
    return *this;
}

Vector2 Vector2::operator*(float scalar) const {
    return {scalar * x, scalar * y};
}

Vector2& Vector2::operator*=(float scalar) {
    x *= scalar;
    y *= scalar;
    return *this;
}

Vector2 Vector2::operator/(float scalar) const {
    return {x / scalar, y / scalar};
}

Vector2& Vector2::operator/=(float scalar) {
    x /= scalar;
    y /= scalar;
    return *this;
}

Vector2 Vector2::operator-() const {
    return {-x, -y};
}

bool Vector2::operator==(const Vector2& v) const {
    return (x == v.x && y == v.y);
}

bool Vector2::operator!=(const Vector2& v) const {
    return !(*this == v);
}

float Vector2::operator[](unsigned int idx) const {
    if (idx == 0)
        return x;

    return y;
}

float& Vector2::operator[](unsigned int idx) {
    if (idx == 0)
        return x;

    return y;
}

float Vector2::lengthSqr() const {
    return x * x + y * y;
}

float Vector2::length() const {
    return std::sqrt(lengthSqr());
}

void Vector2::normalize() {
    float lenSqr = lengthSqr();
    if (lenSqr > 0)
        *this /= std::sqrt(lenSqr);
}

float Vector2::min() const {
    return std::min(x, y);
}

float Vector2::max() const {
    return std::max(x, y);
}

unsigned int Vector2::maxDim() const {
    if (x > y)
        return 0;

    return 1;
}

unsigned int Vector2::minDim() const {
    if (x < y)
        return 0;

    return 1;
}

bool Vector2::isZero() const {
    return x == 0 && y == 0;
}

std::istream& math::operator>>(std::istream& is, Vector2& v) {
    is >> v.x;
    is >> v.y;
    return is;
}

std::ostream& math::operator<<(std::ostream& os, const Vector2& v) {
    os << "Vector2: [" << v.x << ", " << v.y << "]";
    return os;
}

Vector2 math::operator*(float scalar, const Vector2& v) {
    return v * scalar;
}

Vector2 math::Abs(const Vector2& v) {
    return {std::abs(v.x), std::abs(v.y)};
}

Vector2 math::Normalize(const Vector2& v) {
    float lenSqr = v.lengthSqr();
    if (lenSqr > 0)
        return v / std::sqrt(lenSqr);
    return {0};
}

Vector2 math::Min(const Vector2& v1, const Vector2& v2) {
    return {std::min(v1.x, v2.x), std::min(v1.y, v2.y)};
}

Vector2 math::Max(const Vector2& v1, const Vector2& v2) {
    return {std::max(v1.x, v2.x), std::max(v1.y, v2.y)};
}

float math::Dot(const Vector2& v1, const Vector2& v2) {
    return v1.x * v2.x + v1.y * v2.y;
}

float math::AbsDot(const Vector2& v1, const Vector2& v2) {
    return std::abs(math::Dot(v1, v2));
}

float math::Distance(const Vector2& v1, const Vector2& v2) {
    Vector2 v = v1 - v2;
    return v.length();
}

Vector2 math::Pow(const Vector2& v, float exp) {
    return {std::pow(v.x, exp), std::pow(v.y, exp)};
}