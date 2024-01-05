#include <SceneObject.h>

#include <Transform.h>

using namespace pbr;

SceneObject::SceneObject(const Vec3& position)
    : _orientation(), _position(position), _objToWorld() {}

SceneObject::SceneObject(const Mat4& objToWorld)
    : _orientation(), _objToWorld(objToWorld) {

    _position = Vec3(_objToWorld.m14, _objToWorld.m24, _objToWorld.m34);
}

const Vec3& SceneObject::position() const {
    return _position;
}

const Vec3& SceneObject::scale() const {
    return _scale;
}

const Quat& SceneObject::orientation() const {
    return _orientation;
}

const Mat4& SceneObject::objToWorld() const {
    return _objToWorld;
}

sref<SceneObject> SceneObject::parent() const {
    return _parent;
}

void SceneObject::updateMatrix() {
    _objToWorld = Translation(_position) * Mat4(_orientation) * Scale(_scale);
}

void SceneObject::setPosition(const Vec3& position) {
    _position = position;
}

void SceneObject::setScale(float x, float y, float z) {
    _scale = Vec3(x, y, z);
}

void SceneObject::setOrientation(const Quat& quat) {
    _orientation = quat;
}

void SceneObject::setObjToWorld(const Matrix4x4& mat) {
    _objToWorld = mat;
}