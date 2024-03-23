#ifndef PBR_MATH_H
#define PBR_MATH_H

#include <PBR.h>
#include <Vector2.h>
#include <Vector3.h>
#include <Vector3.h>
#include <Vector4.h>
#include <Matrix2x2.h>
#include <Matrix3x3.h>
#include <Matrix4x4.h>
#include <Quat.h>

namespace pbr {

static constexpr float FLOAT_EPSILON = 1e-5f;
static constexpr float ONE_MINUS_EPSILON = 1.0f - FLOAT_EPSILON;

static constexpr float FLOAT_INFINITY = std::numeric_limits<float>::infinity();
static constexpr float FLOAT_LOWEST = std::numeric_limits<float>::lowest();
static constexpr float FLOAT_MAXIMUM = std::numeric_limits<float>::max();

namespace math {

static constexpr float PI = 3.14159265358979323846;
static constexpr float INVPI = 0.31830988618379067154;
static constexpr float INV2PI = 0.15915494309189533577;
static constexpr float INV4PI = 0.07957747154594766788;
static constexpr float PIOVER2 = 1.57079632679489661923;
static constexpr float PIOVER4 = 0.78539816339744830961;
static constexpr float SQRTINVPI = 0.56418958354775628694;
static constexpr float SQRT2 = 1.41421356237309504880;
static constexpr float INVSQRT2 = 0.7071067811865475244;
static constexpr float INVLOG2 = 1.44269504088896338700;

template<typename T, typename U, typename R = std::common_type_t<T, U>>
inline constexpr R Max(T x, U y) {
    return std::max(static_cast<R>(x), static_cast<R>(y));
}

template<typename T, typename U, typename R = std::common_type_t<T, U>>
inline constexpr R Min(T x, U y) {
    return std::min(static_cast<R>(x), static_cast<R>(y));
}

template<typename T, typename U, typename V>
inline constexpr T Clamp(T val, U low, V high) {
    return Max(low, Min(val, high));
}

template<typename T>
inline constexpr T Clamp(T val, T low, T high) {
    return Clamp<T, T, T>(val, low, high);
}

inline float SafeAcos(float x) {
    return std::acos(Clamp(x, -1.0, 1.0));
}

inline constexpr float Radians(float degrees) {
    return (PI / 180.0) * degrees;
}

inline constexpr float Degrees(float radians) {
    return (180.0 / PI) * radians;
}

inline constexpr float Lerp(float t, float v1, float v2) {
    return (1 - t) * v1 + t * v2;
}

using Vec2 = Vector2;
using Vec3 = Vector3;
using Vec4 = Vector4;

using Mat2 = Matrix2x2;
using Mat3 = Matrix3x3;
using Mat4 = Matrix4x4;

} // namespace math
} // namespace pbr

#endif