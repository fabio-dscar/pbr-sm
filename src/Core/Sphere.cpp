#include <Sphere.h>

#include <Geometry.h>
#include <RenderInterface.h>
#include <Resources.h>
#include <Material.h>

using namespace pbr;

Sphere::Sphere(const Vec3& pos, float radius) : Shape(pos), _radius(radius) {}
Sphere::Sphere(const Mat4& objToWorld, float radius)
    : Shape(objToWorld), _radius(radius) {}

void Sphere::prepare() {
    _geometry = std::make_shared<Geometry>();

    // Generate geometry for sphere
    genSphereGeometry(*_geometry, 1.0f, 36, 18);

    this->setScale(_radius, _radius, _radius);

    std::cout << _geometry->vertices().size() << "\n";

    // Upload geometry to the GPU
    RHI.uploadGeometry(_geometry);
    Resource.addGeometry("sphere_1", _geometry);
}

void Sphere::draw() {
    updateMatrix();

    _material->use();

    RHI.setMatrix4("ModelMatrix", objToWorld());
    RHI.setMatrix3("NormalMatrix", normalMatrix());

    if (_material)
        _material->uploadData();

    RHI.drawGeometry(_geometry->rrid());

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
    // Ray-sphere intersection
    return false;
}

bool Sphere::intersect(const Ray& ray, RayHitInfo& info) const {
    // Ray-sphere intersection
    return false;
}