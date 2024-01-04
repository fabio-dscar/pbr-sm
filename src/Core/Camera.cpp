#include <Camera.h>

#include <PBRMath.h>
#include <Perspective.h>
#include <Transform.h>
#include <ParameterMap.h>

using namespace pbr;

Camera::Camera(int32 width, int32 height, const Vec3& eye, const Vec3& at, const Vec3& up,
               float n, float f)
    : _width(width), _height(height), _far(f), _near(n) {

    lookAt(eye, at, up);

    Vector3 viewDir = normalize(at - eye);
    _pitch = asinf(-viewDir.y);
    _yaw = atan2f(viewDir.x, -viewDir.z);
}

int32 Camera::width() const {
    return _width;
}

int32 Camera::height() const {
    return _height;
}

float Camera::aspect() const {
    return (float)_width / (float)_height;
}

float Camera::near() const {
    return _near;
}

float Camera::far() const {
    return _far;
}

Vec3 Camera::right() const {
    return Vec3(_objToWorld.m11, _objToWorld.m12, _objToWorld.m13);
}

Vec3 Camera::front() const {
    return Vec3(_objToWorld.m31, _objToWorld.m32, _objToWorld.m33);
}

Vec3 Camera::up() const {
    return Vec3(_objToWorld.m21, _objToWorld.m22, _objToWorld.m23);
}

void Camera::lookAt(const Vec3& eye, const Vec3& at, const Vec3& up) {
    _position = eye;
    _objToWorld = math::lookAt(eye, at, up);

    Mat4 matOrient = _objToWorld;
    matOrient.m14 = 0;
    matOrient.m24 = 0;
    matOrient.m34 = 0;
    matOrient.m44 = 1;
    _orientation = Quat(matOrient);
}

void Camera::lookAt(const Vec3& at) {
    _objToWorld = math::lookAt(position(), at, normalize(Vec3(0.01, 1.0, 0.0)));
}

const Mat4& Camera::viewMatrix() const {
    return _objToWorld;
}

const Mat4& Camera::projMatrix() const {
    return _projMatrix;
}

Mat4 Camera::viewProjMatrix() const {
    return projMatrix() * viewMatrix();
}

void Camera::updateDimensions(int w, int h) {
    _width = w;
    _height = h;
}

void Camera::updateViewMatrix() {
    Matrix4x4 rotX = rotationAxis(_pitch, Vector3(1, 0, 0));
    Matrix4x4 rotY = rotationAxis(_yaw, Vector3(0, 1, 0));

    Matrix4x4 orientation = rotX * rotY;

    _objToWorld = orientation * translation(-_position);
    //_objToWorld = translation(-_position) * _orientation.toMatrix();
}

void Camera::updateOrientation(float dpdx, float dydx) {
    _pitch += dpdx;
    _yaw -= dydx;

    // Normalize angles
    _yaw = fmodf(_yaw, 2.0f * PI);
    if (_yaw < 0.0f)
        _yaw += 2.0f * PI;

    if (_pitch > PI / 2.0f)
        _pitch = PI / 2.0f;
    else if (_pitch < -PI / 2.0f)
        _pitch = -PI / 2.0f;
}

Ray Camera::traceRay(const Vec2& px) const {
    // Create ray from pixel
    Vec3 rayNDS =
        Vec3((2.0f * px.x) / _width - 1.0f, 1.0f - (2.0f * px.y) / _height, 1.0f);

    Vec4 rayClip = Vec4(rayNDS.x, rayNDS.y, -1.0, 1.0);

    Vec4 rayEye = inverse(_projMatrix) * rayClip;
    rayEye = Vec4(rayEye.x, rayEye.y, -1.0, 0.0);

    Vec3 rayWorld = (inverse(_objToWorld) * rayEye);
    rayWorld = normalize(rayWorld);

    return {_position, rayWorld};
}

std::unique_ptr<Camera> pbr::CreateCamera(const ParameterMap& params) {
    auto eye = params.lookup("eye", Vec3{0});
    auto at = params.lookup("at", Vec3{0, 0, -1});
    auto up = params.lookup("up", Vec3{0, 1, 0});

    auto type = params.lookup("type", "perspective"s);
    if (type == "perspective") {
        auto near = params.lookup("near", 0.1f);
        auto far = params.lookup("far", 500.0f);
        auto fov = params.lookup("fov", 60.0f);
        return std::make_unique<Perspective>(1, 1, eye, at, up, near, far, fov);
    }

    return nullptr;
}