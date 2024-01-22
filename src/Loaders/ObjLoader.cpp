#include <ObjLoader.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include <filesystem>

using namespace pbr;
using namespace pbr::math;
using namespace std::filesystem;

std::optional<ObjFile> pbr::LoadObjFile(const fs::path& filePath) {
    if (!std::filesystem::exists(filePath)) {
        LOG_ERROR("Path {} does not exist.", filePath.string());
        return std::nullopt;
    }

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filePath.c_str())) {
        LOG_ERROR("Failed to load obj file {}: {}", filePath.string(), err);
        return std::nullopt;
    }

    ObjFile objFile;
    objFile.objName = filePath.filename();

    auto& vertices = objFile.vertices;
    auto& indices = objFile.indices;
    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            Vertex& vertex = vertices.emplace_back();

            vertex.position = {attrib.vertices[3 * index.vertex_index + 0],
                               attrib.vertices[3 * index.vertex_index + 1],
                               attrib.vertices[3 * index.vertex_index + 2]};

            if (attrib.normals.size() > 0) {
                vertex.normal = {attrib.normals[3 * index.normal_index + 0],
                                 attrib.normals[3 * index.normal_index + 1],
                                 attrib.normals[3 * index.normal_index + 2]};
            }

            if (attrib.texcoords.size() > 0) {
                vertex.uv = {attrib.texcoords[2 * index.texcoord_index + 0],
                             1.0f - attrib.texcoords[2 * index.texcoord_index + 1]};
            }

            indices.push_back(indices.size());
        }
    }

    return objFile;
}