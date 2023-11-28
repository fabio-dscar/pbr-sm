#include <ObjLoader.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include <filesystem>

using namespace pbr;
using namespace pbr::math;
using namespace std::filesystem;

namespace std {

template<>
struct hash<ObjVertex> {
    size_t operator()(const ObjVertex& vertex) const {
        return ((hash<Vec3>()(vertex.pos) ^ (hash<Vec3>()(vertex.normal) << 1)) >> 1) ^
               (hash<Vec2>()(vertex.texCoord) << 1);
    }
};

} // namespace std

void pbr::fromObjFile(Geometry& geo, const ObjFile& objFile) {
    geo.setIndices(objFile.indices);

    for (const ObjVertex& v : objFile.vertices)
        geo.addVertex({v.pos, v.normal, v.texCoord, {}});

    geo.computeTangents();
    geo.removeRedundantVerts();
}

bool pbr::loadObj(const std::string& filePath, ObjFile& obj) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string err;

    path fp = path(filePath);
    if (!std::filesystem::exists(fp))
        return false;

    obj.objName = fp.filename();
    obj.indices.clear();
    obj.vertices.clear();

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filePath.c_str())) {
        throw std::runtime_error(err);
    }

    uint32 count = 0;
    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            ObjVertex vertex = {};

            vertex.pos = {attrib.vertices[3 * index.vertex_index + 0],
                          attrib.vertices[3 * index.vertex_index + 1],
                          attrib.vertices[3 * index.vertex_index + 2]};

            if (attrib.normals.size() > 0) {
                vertex.normal = {attrib.normals[3 * index.normal_index + 0],
                                 attrib.normals[3 * index.normal_index + 1],
                                 attrib.normals[3 * index.normal_index + 2]};
            }

            if (attrib.texcoords.size() > 0) {
                vertex.texCoord = {attrib.texcoords[2 * index.texcoord_index + 0],
                                   1.0f - attrib.texcoords[2 * index.texcoord_index + 1]};
            }

            obj.vertices.push_back(vertex);

            obj.indices.push_back(count);
            count++;
        }
    }

    return true;
}