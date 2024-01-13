#include <Perspective.h>

#include <Transform.h>

using namespace pbr;

Perspective::Perspective(int width, int height, const Vec3& eye, const Vec3& at,
                         const Vec3& up, float n, float f, float fov)
    : Camera(width, height, eye, at, up, n, f), _fov(fov) {

    _projMatrix = math::Perspective(fov, aspect(), n, f);
}

float Perspective::fov() const {
    return _fov;
}

void Perspective::updateDimensions(int w, int h) {
    Camera::updateDimensions(w, h);
    _projMatrix = math::Perspective(_fov, aspect(), _near, _far);
}