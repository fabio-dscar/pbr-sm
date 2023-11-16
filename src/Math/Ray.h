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
    Ray(const Vec3& origin, const Vec3& dir);
    Ray(const Vec3& origin, const Vec3& dir, float tMin, float tMax);

    const Vec3& origin() const;
    const Vec3& direction() const;

    void setMaxT(float tMax);

    Vec3 operator()(float t) const;

    float tMin() const;
    float tMax() const;

    bool inRange(float t) const;

private:
    Vec3 _origin;
    Vec3 _dir;
    float _tMin;
    float _tMax;
};

} // namespace math
} // namespace pbr

#endif