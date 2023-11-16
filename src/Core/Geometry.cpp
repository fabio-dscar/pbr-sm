#include <Geometry.h>

#include <unordered_map>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
// #include <path.h>
#include <Hash.hpp>

#include <PBRMath.h>

#include <filesystem>

#undef min
#undef max

// using namespace filesystem;

using namespace pbr;
using namespace pbr::math;

using namespace std::filesystem;

namespace std {
template<>
struct hash<ObjVertex> {
    size_t operator()(ObjVertex const& vertex) const {
        return ((hash<Vec3>()(vertex.pos) ^ (hash<Vec3>()(vertex.normal) << 1)) >> 1) ^
               (hash<Vec2>()(vertex.texCoord) << 1);
    }
};
} // namespace std

RRID Geometry::rrid() const {
    return _id;
}

void Geometry::setRRID(RRID id) {
    _id = id;
}

void Geometry::addVertex(const Vertex& vertex) {
    _vertices.push_back(vertex);
}

void Geometry::addIndex(uint32 idx) {
    _indices.push_back(idx);
}

const std::vector<Vertex>& Geometry::vertices() const {
    return _vertices;
}

const std::vector<uint32>& Geometry::indices() const {
    return _indices;
}

void Geometry::setVertices(const std::vector<Vertex>& vertices) {
    _vertices.resize(vertices.size());
    std::copy(vertices.begin(), vertices.end(), _vertices.begin());
}

void Geometry::setIndices(const std::vector<uint32>& indices) {
    _indices.resize(indices.size());
    std::copy(indices.begin(), indices.end(), _indices.begin());
}

BBox3 Geometry::bbox() const {
    Vec3 pMin(FLOAT_INFINITY);
    Vec3 pMax(-FLOAT_INFINITY);

    for (const Vertex& v : _vertices) {
        pMin = math::min(v.position, pMin);
        pMax = math::max(v.position, pMax);
    }

    return BBox3(pMin, pMax);
}

BSphere Geometry::bSphere() const {
    const BBox3 box = bbox();
    return box.sphere();
}

void Geometry::computeTangents() {

    std::vector<Vec3> tan_s(_vertices.size(), Vec3(0, 0, 0));
    std::vector<Vec3> tan_t(_vertices.size(), Vec3(0, 0, 0));

    for (int i = 0; i < _indices.size(); i = i + 3) {
        Vertex v1 = _vertices[_indices[i]];
        Vertex v2 = _vertices[_indices[i + 1]];
        Vertex v3 = _vertices[_indices[i + 2]];

        Vec3 xyz1 = v2.position - v1.position;
        Vec3 xyz2 = v3.position - v1.position;

        Vec2 s = v2.uv - v1.uv;
        Vec2 t = v3.uv - v1.uv;

        float r = 1.0f / (s.x * t.y - s.y * t.x);

        Vec3 sdir =
            Vec3((t.y * xyz1.x - t.x * xyz2.x) * r, (t.y * xyz1.y - t.x * xyz2.y) * r,
                 (t.y * xyz1.z - t.x * xyz2.z) * r);

        Vec3 tdir =
            Vec3((s.x * xyz2.x - s.y * xyz1.x) * r, (s.x * xyz2.y - s.y * xyz1.y) * r,
                 (s.x * xyz2.z - s.y * xyz1.z) * r);

        tan_s[_indices[i]] += sdir;
        tan_s[_indices[i + 1]] += sdir;
        tan_s[_indices[i + 2]] += sdir;

        tan_t[_indices[i]] += tdir;
        tan_t[_indices[i + 1]] += tdir;
        tan_t[_indices[i + 2]] += tdir;
    }

    for (int i = 0; i < _vertices.size(); i++) {
        Vec3 normal = _vertices[i].normal;
        Vec3 t = tan_s[i];

        Vec3 tangent = normalize(t - normal * (dot(normal, t)));

        _vertices[i].tangent =
            (dot(cross(normal, t), tan_t[i]) < 0.0f) ? -tangent : tangent;
    }
}

void pbr::genSphereGeometry(Geometry& geo, float radius, uint32 widthSegments,
                            uint32 heightSegments) {
    uint32 index = 0;
    Vertex vert;

    std::vector<std::vector<uint32>> grid;
    for (uint32 iy = 0; iy <= heightSegments; iy++) {
        float v = iy / heightSegments;

        std::vector<uint32> auxIdx;
        for (uint32 ix = 0; ix <= widthSegments; ix++) {
            float u = ix / widthSegments;

            // Position
            vert.position.x = radius * std::cos(u * 2.0f * PI) * std::sin(v * PI);
            vert.position.y = radius * std::cos(v * PI);
            vert.position.z = radius * std::sin(u * 2.0f * PI) * std::sin(v * PI);

            // Normal
            vert.normal =
                normalize(Vec3(vert.position.x, vert.position.y, vert.position.z));

            // Texture Coords
            vert.uv = Vec2(u, 1 - v);

            auxIdx.push_back(index++);

            geo.addVertex(vert);
        }

        grid.push_back(auxIdx);
    }

    for (uint32 iy = 0; iy < heightSegments; iy++) {
        for (uint32 ix = 0; ix < widthSegments; ix++) {

            uint32 a = grid[iy][ix + 1];
            uint32 b = grid[iy][ix];
            uint32 c = grid[iy + 1][ix];
            uint32 d = grid[iy + 1][ix + 1];

            if (iy != 0) {
                geo.addIndex(a);
                geo.addIndex(b);
                geo.addIndex(d);
            }

            if (iy != heightSegments - 1) {
                geo.addIndex(b);
                geo.addIndex(c);
                geo.addIndex(d);
            }
        }
    }
}

void pbr::genBoxGeometry(Geometry& geo, uint32 widthSegments, uint32 heightSegments,
                         uint32 depthSegments) {}

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

    std::unordered_map<ObjVertex, uint32_t> uniqueVertices = {};
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

            if (uniqueVertices.count(vertex) == 0) {
                uniqueVertices[vertex] = static_cast<uint32>(obj.vertices.size());
                obj.vertices.push_back(vertex);
            }

            obj.indices.push_back(uniqueVertices[vertex]);

            /*
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
                            vertex.texCoord = {attrib.texcoords[2 * index.texcoord_index +
               0], 1.0f - attrib.texcoords[2 * index.texcoord_index + 1]};
                        }

                        obj.vertices.push_back(vertex);

                        obj.indices.push_back(count);
                        count++;*/
        }
    }

    return true;
}

void pbr::fromObjFile(Geometry& geo, const ObjFile& objFile) {
    geo.setIndices(objFile.indices);

    for (const ObjVertex& v : objFile.vertices)
        geo.addVertex({v.pos, v.normal, v.texCoord});

    geo.computeTangents();
}

void pbr::genUnitCubeGeometry(Geometry& geo) {
    float vertices[] = {
        // back face
        -1.0f, -1.0f, -1.0f, // bottom-left
        1.0f, 1.0f, -1.0f,   // top-right
        1.0f, -1.0f, -1.0f,  // bottom-right
        1.0f, 1.0f, -1.0f,   // top-right
        -1.0f, -1.0f, -1.0f, // bottom-left
        -1.0f, 1.0f, -1.0f,  // top-left
                             // front face
        -1.0f, -1.0f, 1.0f,  // bottom-left
        1.0f, -1.0f, 1.0f,   // bottom-right
        1.0f, 1.0f, 1.0f,    // top-right
        1.0f, 1.0f, 1.0f,    // top-right
        -1.0f, 1.0f, 1.0f,   // top-left
        -1.0f, -1.0f, 1.0f,  // bottom-left
                             // left face
        -1.0f, 1.0f, 1.0f,   // top-right
        -1.0f, 1.0f, -1.0f,  // top-left
        -1.0f, -1.0f, -1.0f, // bottom-left
        -1.0f, -1.0f, -1.0f, // bottom-left
        -1.0f, -1.0f, 1.0f,  // bottom-right
        -1.0f, 1.0f, 1.0f,   // top-right
                             // right face
        1.0f, 1.0f, 1.0f,    // top-left
        1.0f, -1.0f, -1.0f,  // bottom-right
        1.0f, 1.0f, -1.0f,   // top-right
        1.0f, -1.0f, -1.0f,  // bottom-right
        1.0f, 1.0f, 1.0f,    // top-left
        1.0f, -1.0f, 1.0f,   // bottom-left
                             // bottom face
        -1.0f, -1.0f, -1.0f, // top-right
        1.0f, -1.0f, -1.0f,  // top-left
        1.0f, -1.0f, 1.0f,   // bottom-left
        1.0f, -1.0f, 1.0f,   // bottom-left
        -1.0f, -1.0f, 1.0f,  // bottom-right
        -1.0f, -1.0f, -1.0f, // top-right
                             // top face
        -1.0f, 1.0f, -1.0f,  // top-left
        1.0f, 1.0f, 1.0f,    // bottom-right
        1.0f, 1.0f, -1.0f,   // top-right
        1.0f, 1.0f, 1.0f,    // bottom-right
        -1.0f, 1.0f, -1.0f,  // top-left
        -1.0f, 1.0f, 1.0f    // bottom-left
    };

    for (uint32 i = 0; i < 36; i++) {
        Vertex vert;
        vert.position = -Vec3(vertices[3 * i], vertices[3 * i + 1], vertices[3 * i + 2]);
        geo.addVertex(vert);
    }
}