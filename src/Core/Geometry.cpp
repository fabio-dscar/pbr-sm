#include <Geometry.h>
#include <Hash.h>
#include <PBRMath.h>
#include <cstdlib>

#include <unordered_map>
#include <filesystem>

#include <mikktspace.h>

#include <format>

using namespace pbr;
using namespace pbr::math;
using namespace std::filesystem;

namespace std {

template<>
struct hash<Vertex> {
    size_t operator()(const Vertex& vertex) const {
        return ((hash<Vec3>()(vertex.position) ^ (hash<Vec3>()(vertex.normal) << 1)) >>
                1) ^
               (hash<Vec2>()(vertex.uv) << 1) ^ (hash<Vec4>()(vertex.tangent) << 1);
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
    auto getNumFaces = [](const SMikkTSpaceContext* pContext) -> int {
        auto ptr = static_cast<Geometry*>(pContext->m_pUserData);
        return ptr->getNumFaces();
    };

    auto getNumVertsFace = [](const SMikkTSpaceContext*, const int) -> int { return 3; };

    auto getPosition = [](const SMikkTSpaceContext* pContext, float fvPosOut[],
                          const int iFace, const int iVert) {
        auto ptr = static_cast<Geometry*>(pContext->m_pUserData);
        auto vertex = ptr->getVertex(iFace, iVert);
        fvPosOut[0] = vertex.position.x;
        fvPosOut[1] = vertex.position.y;
        fvPosOut[2] = vertex.position.z;
    };

    auto getNormal = [](const SMikkTSpaceContext* pContext, float fvNormOut[],
                        const int iFace, const int iVert) {
        auto ptr = static_cast<Geometry*>(pContext->m_pUserData);
        auto vertex = ptr->getVertex(iFace, iVert);
        fvNormOut[0] = vertex.normal.x;
        fvNormOut[1] = vertex.normal.y;
        fvNormOut[2] = vertex.normal.z;
    };

    auto getTexCoord = [](const SMikkTSpaceContext* pContext, float fvTexcOut[],
                          const int iFace, const int iVert) {
        auto ptr = static_cast<Geometry*>(pContext->m_pUserData);
        auto vertex = ptr->getVertex(iFace, iVert);
        fvTexcOut[0] = vertex.uv.x;
        fvTexcOut[1] = vertex.uv.y;
    };

    auto setTSpace = [](const SMikkTSpaceContext* pContext, const float fvTangent[],
                        const float fSign, const int iFace, const int iVert) {
        auto ptr = static_cast<Geometry*>(pContext->m_pUserData);
        ptr->addTangent(iFace, iVert, Vec3(fvTangent[0], fvTangent[1], fvTangent[2]),
                        fSign);
    };

    SMikkTSpaceInterface it{.m_getNumFaces = getNumFaces,
                            .m_getNumVerticesOfFace = getNumVertsFace,
                            .m_getPosition = getPosition,
                            .m_getNormal = getNormal,
                            .m_getTexCoord = getTexCoord,
                            .m_setTSpaceBasic = setTSpace};

    SMikkTSpaceContext ctx{.m_pInterface = &it, .m_pUserData = this};

    int res = genTangSpaceDefault(&ctx);
    if (res == 0)
        std::cout << "Error computing tangents.\n";
}

std::unique_ptr<Geometry> pbr::genUnitSphere(uint32 widthSegments,
                                             uint32 heightSegments) {
    auto geo = std::make_unique<Geometry>();

    uint32 index = 0;
    Vertex vert;

    std::vector<std::vector<uint32>> grid;
    for (uint32 iy = 0; iy <= heightSegments; iy++) {
        float v = iy / heightSegments;

        std::vector<uint32> auxIdx;
        for (uint32 ix = 0; ix <= widthSegments; ix++) {
            float u = ix / widthSegments;

            // Position
            vert.position.x = std::cos(u * 2.0f * PI) * std::sin(v * PI);
            vert.position.y = std::cos(v * PI);
            vert.position.z = std::sin(u * 2.0f * PI) * std::sin(v * PI);

            // Normal
            vert.normal =
                normalize(Vec3(vert.position.x, vert.position.y, vert.position.z));

            // Texture Coords
            vert.uv = Vec2(u, 1 - v);

            auxIdx.push_back(index++);

            geo->addVertex(vert);
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
                geo->addIndex(a);
                geo->addIndex(b);
                geo->addIndex(d);
            }

            if (iy != heightSegments - 1) {
                geo->addIndex(b);
                geo->addIndex(c);
                geo->addIndex(d);
            }
        }
    }

    return geo;
}

void Geometry::removeRedundantVerts() {
    Geometry other;

    std::cout << std::format("Faces: {} Verts: {}\n", _indices.size() / 3,
                             _vertices.size());

    std::unordered_map<Vertex, uint32_t> uniqueVertices{};
    for (const auto index : _indices) {
        auto vertex = _vertices[index];

        if (uniqueVertices.count(vertex) == 0) {
            uniqueVertices[vertex] = static_cast<uint32>(other._vertices.size());
            other.addVertex(vertex);
        }

        other._indices.push_back(uniqueVertices[vertex]);
    }

    std::cout << std::format("Faces: {} Verts: {}\n\n", other._indices.size() / 3,
                             other._vertices.size());

    *this = std::move(other);
}

std::unique_ptr<Geometry> pbr::genUnitCube() {
    auto geo = std::make_unique<Geometry>();

    float vertices[] = {-1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f, -1.0f,
                        -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, 1.0f, 1.0f,
                        -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f, 1.0f};

    uint32 indices[] = {0, 1, 2, 1, 0, 3, 4, 5, 7, 7, 6, 4, 6, 3, 0, 0, 4, 6,
                        7, 2, 1, 2, 7, 5, 0, 2, 5, 5, 4, 0, 3, 7, 1, 7, 3, 6};

    for (uint32 i = 0; i < 8; i++) {
        geo->addVertex({.position = Vec3(vertices[3 * i], vertices[3 * i + 1],
                                         vertices[3 * i + 2])});
    }

    for (uint32 i = 0; i < 36; i++)
        geo->addIndex(indices[i]);

    return geo;
}

PBR_SHARED std::unique_ptr<Geometry> pbr::genUnitQuad() {
    auto geo = std::make_unique<Geometry>();

    Vec3 normal{0, 1, 0};
    Vec4 tangent{1, 0, 0, 1};

    geo->addVertex({.position = {-0.5f, 0.0f, 0.5f},
                    .normal = normal,
                    .uv = {0.0f, 0.0f},
                    .tangent = tangent});
    geo->addVertex({.position = {0.5f, 0.0f, 0.5f},
                    .normal = normal,
                    .uv = {1.0f, 0.0f},
                    .tangent = tangent});
    geo->addVertex({.position = {-0.5f, 0.0f, -0.5f},
                    .normal = normal,
                    .uv = {0.0f, 1.0f},
                    .tangent = tangent});
    geo->addVertex({.position = {0.5f, 0.0f, -0.5f},
                    .normal = normal,
                    .uv = {1.0f, 1.0f},
                    .tangent = tangent});

    geo->addIndex(1);
    geo->addIndex(2);
    geo->addIndex(0);
    geo->addIndex(1);
    geo->addIndex(3);
    geo->addIndex(2);

    return geo;
}