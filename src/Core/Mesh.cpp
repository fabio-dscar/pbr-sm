#include <Mesh.h>

#include <Geometry.h>
#include <RenderInterface.h>
#include <Resources.h>
#include <Material.h>

#include <cassert>

using namespace pbr;

Mesh::Mesh(const std::string& objPath) : Shape() {
    _geometry = std::make_shared<Geometry>();
    
    ObjFile objFile;
    loadObj(objPath, objFile);
    fromObjFile(*_geometry, objFile);
}

Mesh::Mesh(const std::string& objPath, const Mat4& objToWorld) : Shape(objToWorld) {
    _geometry = std::make_shared<Geometry>();

    ObjFile objFile;
    loadObj(objPath, objFile);
    fromObjFile(*_geometry, objFile);

    Resource.addGeometry(objFile.objName, _geometry);
}

void Mesh::prepare() {
    _bbox = _geometry->bbox();

    RHI.uploadGeometry(*_geometry);
}

void Mesh::draw() {
    assert(_material != nullptr);

    updateMatrix();
    
    _material->use();

    RHI.setMatrix4(MODEL_MATRIX,  objToWorld());
    RHI.setMatrix3(NORMAL_MATRIX, normalMatrix());

    _material->uploadData();

    RHI.drawGeometry(_geometry->rrid());
}

BBox3 Mesh::bbox() const {
    return transform(objToWorld(), _bbox);
}

BSphere Mesh::bSphere() const {
    return _bbox.sphere();
}

bool Mesh::intersect(const Ray& ray) const {
    return false;
}

bool Mesh::intersect(const Ray& ray, RayHitInfo& info) const {
    return false;
}
