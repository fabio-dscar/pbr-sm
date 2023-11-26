#include <Quad.h>

#include <Geometry.h>
#include <RenderInterface.h>
#include <Resources.h>
#include <Material.h>

#include <cassert>

using namespace pbr;

void Quad::prepare() {
    auto quad = Resource.getGeometry("unitQuad");
    _geoId = quad->rrid();
    _bbox = quad->bbox();
}

void Quad::draw() {
    assert(_material != nullptr);

    updateMatrix();

    _material->use();

    RHI.setMatrix4(MODEL_MATRIX, objToWorld());
    RHI.setMatrix3(NORMAL_MATRIX, normalMatrix());

    _material->uploadData();

    RHI.drawGeometry(_geoId);
    
    RHI.useProgram(0);
}

BBox3 Quad::bbox() const {
    return transform(objToWorld(), _bbox);
}

BSphere Quad::bSphere() const {
    return transform(objToWorld(), BSphere(_position, 1.0f / std::sqrt(2)));
}

bool Quad::intersect(const Ray& ray) const {
    return false;
}

bool Quad::intersect(const Ray& ray, RayHitInfo& info) const {
    return false;
}