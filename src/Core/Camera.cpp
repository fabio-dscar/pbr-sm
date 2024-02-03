#include <Camera.h>

#include <PBRMath.h>
#include <Perspective.h>
#include <Transform.h>
#include <ParameterMap.h>

using namespace pbr;

Camera::Camera(int width, int height, const Vec3& eye, const Vec3& at, const Vec3& up,
               float n, float f)
    : _width(width), _height(height), _far(f), _near(n) {

    lookAt(eye, at, up);

    Vector3 viewDir = Normalize(at - eye);
    _pitch = std::asin(-viewDir.y);
    _yaw = std::atan2(viewDir.x, -viewDir.z);
}

int Camera::width() const {
    return _width;
}

int Camera::height() const {
    return _height;
}

float Camera::aspect() const {
    return static_cast<float>(_width) / _height;
}

float Camera::near() const {
    return _near;
}

float Camera::far() const {
    return _far;
}

Vec3 Camera::right() const {
    return Vec3(_objToWorld(0, 0), _objToWorld(0, 1), _objToWorld(0, 2));
}

Vec3 Camera::front() const {
    return Vec3(_objToWorld(2, 0), _objToWorld(2, 1), _objToWorld(2, 2));
}

Vec3 Camera::up() const {
    return Vec3(_objToWorld(1, 0), _objToWorld(1, 1), _objToWorld(1, 2));
}

void Camera::lookAt(const Vec3& eye, const Vec3& at, const Vec3& up) {
    _position = eye;
    _objToWorld = math::LookAt(eye, at, up);

    Mat4 matOrient = _objToWorld;
    matOrient(0, 3) = 0;
    matOrient(1, 3) = 0;
    matOrient(2, 3) = 0;
    matOrient(3, 3) = 1;
    _orientation = Quat(matOrient);
}

void Camera::lookAt(const Vec3& at) {
    _objToWorld = math::LookAt(position(), at, Normalize(Vec3(0.01, 1.0, 0.0)));
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
    Matrix4x4 rotX = RotationAxis(_pitch, {1, 0, 0});
    Matrix4x4 rotY = RotationAxis(_yaw, {0, 1, 0});

    Matrix4x4 orientation = rotX * rotY;

    _objToWorld = orientation * Translation(-_position);
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
    Vec2 rayNDS = 2.0 * Vec2{px.x / _width, -px.y / _height} + Vec2{-1.0f, 1.0f};

    Vec4 rayClip = {rayNDS.x, rayNDS.y, -1.0, 1.0};
    Vec4 rayEye = Inverse(_projMatrix) * rayClip;
    rayEye = {rayEye.x, rayEye.y, -1.0, 0.0};

    Vec3 rayWorld = Normalize(Inverse(_objToWorld) * rayEye);

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