#include <Shape.h>

#include <RenderInterface.h>
#include <Geometry.h>
#include <Material.h>

using namespace pbr;

Shape::Shape(const Vec3& position) : SceneObject(position) {}
Shape::Shape(const Mat4& objToWorld) : SceneObject(objToWorld) {}

const sref<Geometry>& Shape::geometry() const {
    return _geometry;
}

const sref<Material>& Shape::material() const {
    return _material;
}

void Shape::updateMatrix() {
    SceneObject::updateMatrix();
    _normalMatrix = transpose(inverse(Mat3(_objToWorld)));
}

const Mat3& Shape::normalMatrix() const {
    return _normalMatrix;
}

void Shape::setMaterial(const sref<Material>& mat) {
    _material = mat;
}

void Shape::updateMaterial(const Skybox& skybox) {
    _material->update(skybox);
}

void Shape::draw() {
    updateMatrix();

    _material->use();

    RHI.setMatrix4(MODEL_MATRIX, objToWorld());
    RHI.setMatrix3(NORMAL_MATRIX, normalMatrix());

    _material->uploadData();

    RHI.drawGeometry(_geometry->rrid());
}