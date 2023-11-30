#include <Ray.h>

using namespace pbr::math;

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