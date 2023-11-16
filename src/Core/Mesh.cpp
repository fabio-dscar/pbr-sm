#include <Mesh.h>

#include <Geometry.h>
#include <RenderInterface.h>
#include <Resources.h>
#include <Material.h>

using namespace pbr;

Mesh::Mesh(const std::string& objPath) : Shape() {
    _geometry = std::make_shared<Geometry>();
    
    // Load Obj file
    ObjFile objFile;
    loadObj(objPath, objFile);
    fromObjFile(*_geometry, objFile);
}

Mesh::Mesh(const std::string& objPath, const Mat4& objToWorld) : Shape(objToWorld) {
    _geometry = std::make_shared<Geometry>();

    // Load Obj file
    ObjFile objFile;
    loadObj(objPath, objFile);
    fromObjFile(*_geometry, objFile);

    // Register geometry in the resource manager
    Resource.addGeometry(objFile.objName, _geometry);
}

void Mesh::prepare() {
    // Calculate bounding box
    _bbox = _geometry->bbox();

    // Upload geometry to the GPU
    RHI.uploadGeometry(_geometry);
}

void Mesh::draw() {
    updateMatrix();

    //if (_prog == -1)
    _material->use();
    //else
    //    RHI.useProgram(_prog);

    RHI.setMatrix4("ModelMatrix",  objToWorld());
    RHI.setMatrix3("NormalMatrix", normalMatrix());

    if (_material)
        _material->uploadData();

    RHI.drawGeometry(_geometry->rrid());

    RHI.useProgram(0);
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
