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

Mesh::Mesh(const std::shared_ptr<Geometry>& geometry, const Mat4& objToWorld)
    : Shape(objToWorld) {

    _geometry = geometry;
    _bbox = _geometry->bbox();
}

BBox3 Mesh::bbox() const {
    return Transform(objToWorld(), _bbox);
}

BSphere Mesh::bSphere() const {
    return _bbox.sphere();
}

std::optional<float> Mesh::intersect(const Ray& ray, float tMax) const {
    return bbox().intersectRay(ray, tMax);
}

std::unique_ptr<Shape> pbr::CreateMesh(const ParameterMap& params) {
    auto typeOpt = params.lookup<std::string>("type");
    CHECK(typeOpt.has_value());

    std::shared_ptr<Geometry> geo = nullptr;

    auto type = *typeOpt;
    if (type == "obj") {
        fs::path parentDir = params.lookup("parentdir", ""s);
        auto fileName = params.lookup<std::string>("filename");
        CHECK(fileName.has_value());

        auto fullPath = parentDir / *fileName;
        auto objFile = LoadObjFile(fullPath);
        if (!objFile.has_value())
            return nullptr;

        geo = std::make_unique<Geometry>(std::move((*objFile).vertices),
                                         std::move((*objFile).indices));
    } else if (type == "sphere") {
        auto widthSegments = params.lookup<unsigned int>("widthSegments", 128);
        auto heightSegments = params.lookup<unsigned int>("heightSegments", 64);
        geo = genUnitSphere(widthSegments, heightSegments);
    } else if (type == "quad") {
        geo = Resource.get<Geometry>("unitQuad");
    } else {
        FATAL("Unknown mesh type.");
    }

    if (!geo) {
        LOG_ERROR("Couldn't load mesh's geometry.");
        return nullptr;
    }

    std::shared_ptr<Material> mat = std::make_shared<PBRMaterial>();
    auto mapref = params.lookup<ParameterMap*>("material", nullptr);
    if (mapref)
        mat = CreateMaterial(*mapref);

    auto toWorld = params.lookup("toWorld", Mat4{});

    auto mesh = std::make_unique<Mesh>(geo, toWorld);
    mesh->setMaterial(mat);

    return mesh;
}