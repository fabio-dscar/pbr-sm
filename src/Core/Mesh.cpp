#include <Mesh.h>

#include <Geometry.h>
#include <ObjLoader.h>
#include <RenderInterface.h>
#include <Resources.h>
#include <Material.h>
#include <PBRMaterial.h>
#include <ParameterMap.h>

#include <filesystem>

using namespace pbr;
namespace fs = std::filesystem;

Mesh::Mesh(const std::string& objPath) : Shape() {
    auto objFile = LoadObjFile(objPath);
    CHECK(objFile.has_value());

    _geometry = std::make_shared<Geometry>(std::move((*objFile).vertices),
                                           std::move((*objFile).indices));
}

Mesh::Mesh(const std::shared_ptr<Geometry>& geometry, const Mat4& objToWorld)
    : Shape(objToWorld) {

    _geometry = geometry;
}

void Mesh::prepare() {
    _bbox = _geometry->bbox();

    RHI.uploadGeometry(*_geometry);
}

BBox3 Mesh::bbox() const {
    return Transform(objToWorld(), _bbox);
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

std::unique_ptr<Shape> pbr::CreateMesh(const ParameterMap& params) {
    fs::path parentDir = params.lookup("parentdir", ""s);
    auto fileName = params.lookup<std::string>("filename");

    CHECK(fileName.has_value());

    auto fullPath = parentDir / *fileName;

    auto objFile = LoadObjFile(fullPath);
    if (!objFile.has_value())
        return nullptr;

    auto geo = std::make_shared<Geometry>(std::move((*objFile).vertices),
                                          std::move((*objFile).indices));

    std::shared_ptr<Material> mat = std::make_shared<PBRMaterial>();
    auto mapref = params.lookup<ParameterMap*>("material", nullptr);
    if (mapref)
        mat = CreateMaterial(*mapref);
    mat->prepare();

    auto toWorld = params.lookup("toWorld", Mat4{});

    auto mesh = std::make_unique<Mesh>(geo, toWorld);
    mesh->prepare();
    mesh->setMaterial(mat);
    mesh->updateMatrix();

    return mesh;
}