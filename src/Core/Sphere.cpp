#include <Sphere.h>

#include <Geometry.h>
#include <RenderInterface.h>
#include <Resources.h>
#include <Material.h>

#include <cassert>

using namespace pbr;

Sphere::Sphere(const Vec3& pos, float radius) : Shape(pos), _radius(radius) {}
Sphere::Sphere(const Mat4& objToWorld, float radius)
    : Shape(objToWorld), _radius(radius) {}

void Sphere::prepare() {
    this->setScale(_radius, _radius, _radius);
    _geoId = Resource.getGeometry("unitSphere")->rrid();
}

void Sphere::draw() {
    assert(_material != nullptr);

    updateMatrix();

    _material->use();

    RHI.setMatrix4(MODEL_MATRIX, objToWorld());
    RHI.setMatrix3(NORMAL_MATRIX, normalMatrix());

    _material->uploadData();

    RHI.drawGeometry(_geoId);

    RHI.useProgram(0);
}

BBox3 Sphere::bbox() const {
    const Vec3 min = _position - Vec3(_radius);
    const Vec3 max = _position + Vec3(_radius);

    return BBox3(min, max);
}

BSphere Sphere::bSphere() const {
    return BSphere(_position, _radius);
}

bool Sphere::intersect(const Ray& ray) const {
    float t;
    return bSphere().intersectRay(ray, &t);
}

bool Sphere::intersect(const Ray& ray, RayHitInfo& info) const {
    // Ray-sphere intersection
    return false;
}