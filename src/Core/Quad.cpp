#include <Quad.h>

#include <Geometry.h>
#include <RenderInterface.h>
#include <Resources.h>
#include <Material.h>

#include <cassert>

using namespace pbr;

void Quad::prepare() {
    _geometry = Resource.get<Geometry>("unitQuad");
    _bbox = _geometry->bbox();
}

BBox3 Quad::bbox() const {
    return Transform(objToWorld(), _bbox);
}

BSphere Quad::bSphere() const {
    return Transform(objToWorld(), BSphere(_position, INVSQRT2));
}

bool Quad::intersect(const Ray& ray) const {
    return false;
}

bool Quad::intersect(const Ray& ray, RayHitInfo& info) const {
    return false;
}