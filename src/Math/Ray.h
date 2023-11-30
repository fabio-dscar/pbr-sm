#ifndef __PBR_RAY_H__
#define __PBR_RAY_H__

#include <PBRMath.h>

namespace pbr {

class SceneObject;

namespace math {

struct RayHitInfo {
    SceneObject* obj;
    Vec3 point;
    Vec3 normal;
    float dist;
};

class PBR_SHARED Ray {
public:
    Ray(const Vec3& origin, const Vec3& dir) : _origin(origin), _dir(dir) {}
    Ray(const Vec3& origin, const Vec3& dir, float tMin, float tMax)
        : _origin(origin), _dir(dir), _tMin(tMin), _tMax(tMax) {}

    const Vec3& origin() const;
    const Vec3& direction() const;

    void setMaxT(float tMax);

    Vec3 operator()(float t) const;

    float tMin() const;
    float tMax() const;

    bool inRange(float t) const;

private:
    Vec3 _origin{0};
    Vec3 _dir;
    float _tMin = FLOAT_EPSILON;
    float _tMax = FLOAT_INFINITY;
};

} // namespace math
} // namespace pbr

#endif