#ifndef __PBR_MATH_H__
#define __PBR_MATH_H__

// Includes for math objects
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

#if PBR_DOUBLE
typedef double Float;

static constexpr const Float FLOAT_EPSILON = 1e-10;
static constexpr const Float ONE_MINUS_EPSILON = 1.0 - FLOAT_EPSILON;
#else
typedef float Float;

static constexpr Float FLOAT_EPSILON = 1e-5f; // 1e-6
static constexpr Float ONE_MINUS_EPSILON = 1.0f - FLOAT_EPSILON;
#endif

static constexpr Float FLOAT_INFINITY = std::numeric_limits<Float>::infinity();
static constexpr Float FLOAT_LOWEST = std::numeric_limits<Float>::lowest();
static constexpr Float FLOAT_MAXIMUM = std::numeric_limits<Float>::max();

namespace math {

static constexpr Float PI = 3.14159265358979323846;
static constexpr Float INVPI = 0.31830988618379067154;
static constexpr Float INV2PI = 0.15915494309189533577;
static constexpr Float INV4PI = 0.07957747154594766788;
static constexpr Float PIOVER2 = 1.57079632679489661923;
static constexpr Float PIOVER4 = 0.78539816339744830961;
static constexpr Float SQRTINVPI = 0.56418958354775628694;
static constexpr Float SQRT2 = 1.41421356237309504880;
static constexpr Float INVLOG2 = 1.44269504088896338700;

template<typename T, typename U, typename V>
inline constexpr PBR_SHARED T clamp(T val, U low, V high) {
    if (val < low)
        return low;
    else if (val > high)
        return high;
    else
        return val;
}

template<typename T>
inline constexpr PBR_SHARED T clamp(T val, T low, T high) {
    return clamp<T, T, T>(val, low, high);
}

template<typename T>
inline constexpr PBR_SHARED T mod(T x, T y) {
    T mod = x - (x / y) * y;

    if (mod < 0)
        mod += y;

    return (T)mod;
}

template<>
inline constexpr Float mod(Float x, Float y) {
    return std::fmod(x, y);
}

inline constexpr Float max(Float x, Float y) {
    return std::max(x, y);
}

inline constexpr Float min(Float x, Float y) {
    return std::min(x, y);
}

inline constexpr Float acosSafe(Float x) {
    return std::acos(clamp(x, -1.0, 1.0));
}

inline constexpr Float radians(Float degrees) {
    return (PI / 180.0) * degrees;
}

inline constexpr Float lerp(Float t, Float v1, Float v2) {
    return (1 - t) * v1 + t * v2;
}

typedef Vector2 Vec2;
typedef Vector3 Vec3;
typedef Vector4 Vec4;

typedef Matrix2x2 Mat2;
typedef Matrix3x3 Mat3;
typedef Matrix4x4 Mat4;

} // namespace math
} // namespace pbr

#endif