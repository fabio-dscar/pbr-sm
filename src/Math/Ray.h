#ifndef PBR_RAY_H
#define PBR_RAY_H

#include <PBRMath.h>

namespace pbr {
namespace math {

class Ray {
public:
    Ray(const Vec3& origin, const Vec3& dir) : _origin(origin), _dir(dir) {}

    const Vec3& origin() const { return _origin; }
    const Vec3& direction() const { return _dir; }

    Vec3 operator()(float t) const { return _origin + t * _dir; }

    float tMin() const { return _tMin; }

private:
    Vec3 _origin{0};
    Vec3 _dir;
    float _tMin = FLOAT_EPSILON;
};

} // namespace math
} // namespace pbr

#endif