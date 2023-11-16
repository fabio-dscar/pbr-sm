#include <Ray.h>

using namespace pbr::math;

Ray::Ray(const Vec3& origin, const Vec3& dir)
    : _origin(origin), _dir(dir), _tMin(FLOAT_EPSILON), _tMax(FLOAT_INFINITY) {}

Ray::Ray(const Vec3& origin, const Vec3& dir, float tMin, float tMax)
    : _origin(origin), _dir(dir), _tMin(tMin), _tMax(tMax) {}

const Vec3& Ray::origin() const {
    return _origin;
}

const Vec3& Ray::direction() const {
    return _dir;
}

Vec3 Ray::operator()(float t) const {
    return _origin + t * _dir;
}

void Ray::setMaxT(float tMax) {
    _tMax = tMax;
}

float Ray::tMin() const {
    return _tMin;
}

float Ray::tMax() const {
    return _tMax;
}

bool Ray::inRange(float t) const {
    return t > _tMin && t < _tMax;
}