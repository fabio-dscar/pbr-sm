#include <Shape.h>

#include <RenderInterface.h>
#include <Geometry.h>
#include <Material.h>

using namespace pbr;

Shape::Shape(const Vec3& position) : SceneObject(position) {
    updateMatrix();
}
Shape::Shape(const Mat4& objToWorld) : SceneObject(objToWorld) {
    updateMatrix();
}

const sref<Geometry>& Shape::geometry() const {
    return _geometry;
}

const sref<Material>& Shape::material() const {
    return _material;
}

void Shape::updateMatrix() {
    SceneObject::updateMatrix();
    _normalMatrix = Transpose(Inverse(Mat3(_objToWorld)));
}

const Mat3& Shape::normalMatrix() const {
    return _normalMatrix;
}

void Shape::setMaterial(const sref<Material>& mat) {
    _material = mat;
}

void Shape::draw() {
    updateMatrix();

    _material->use();

    RHI.setMatrix4(MODEL_MATRIX, objToWorld());
    RHI.setMatrix3(NORMAL_MATRIX, normalMatrix());

    _material->uploadData();
    _geometry->draw();
}