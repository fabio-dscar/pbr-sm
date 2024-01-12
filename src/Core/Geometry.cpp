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

Geometry::Geometry(std::vector<Vertex>&& vertices, std::vector<uint32>&& indices) {
    _vertices = std::move(vertices);
    _indices = std::move(indices);

    computeTangents();
    removeRedundantVerts();
}

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
        LOG_ERROR("Failed to compute tangents.");
}

std::unique_ptr<Geometry> pbr::genUnitSphere(unsigned int widthSegments,
                                             unsigned int heightSegments) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    unsigned count = 0;

    std::vector<std::vector<unsigned>> grid;
    for (unsigned iy = 0; iy <= heightSegments; iy++) {
        float v = (float)iy / heightSegments;

        std::vector<unsigned> auxIdx;
        for (unsigned ix = 0; ix <= widthSegments; ix++) {
            float u = (float)ix / widthSegments;

            Vertex vert;

            // Position
            vert.position.x = std::cos(u * 2.0f * PI) * std::sin(v * PI);
            vert.position.y = std::cos(v * PI);
            vert.position.z = std::sin(u * 2.0f * PI) * std::sin(v * PI);

            // Normal
            vert.normal = normalize(vert.position);

            // Texture Coords
            vert.uv = {u, 1 - v};

            auxIdx.push_back(count++);
            vertices.push_back(vert);
        }

        grid.push_back(auxIdx);
    }

    for (unsigned iy = 0; iy < heightSegments; iy++) {
        for (unsigned ix = 0; ix < widthSegments; ix++) {

            unsigned a = grid[iy][ix + 1];
            unsigned b = grid[iy][ix];
            unsigned c = grid[iy + 1][ix];
            unsigned d = grid[iy + 1][ix + 1];

            if (iy != 0) {
                indices.push_back(a);
                indices.push_back(d);
                indices.push_back(b);
            }

            if (iy != heightSegments - 1) {
                indices.push_back(b);
                indices.push_back(d);
                indices.push_back(c);
            }
        }
    }

    return std::make_unique<Geometry>(std::move(vertices), std::move(indices));
}

void Geometry::removeRedundantVerts() {
    Geometry other;

    std::unordered_map<Vertex, uint32_t> uniqueVertices{};
    for (const auto index : _indices) {
        auto vertex = _vertices[index];

        if (uniqueVertices.count(vertex) == 0) {
            uniqueVertices[vertex] = static_cast<uint32>(other._vertices.size());
            other.addVertex(vertex);
        }

        other._indices.push_back(uniqueVertices[vertex]);
    }

    this->swap(other);
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
    Vec3 normal{0, 1, 0};
    Vec4 tangent{1, 0, 0, 1};

    return std::make_unique<Geometry>(
        std::vector<Vertex>{
            {{-0.5f, 0.0f, 0.5f},  normal, {0, 0}, tangent},
            {{0.5f, 0.0f, 0.5f},   normal, {1, 0}, tangent},
            {{-0.5f, 0.0f, -0.5f}, normal, {0, 1}, tangent},
            {{0.5f, 0.0f, -0.5f},  normal, {1, 1}, tangent}
    },
        std::vector<unsigned>{1, 2, 0, 1, 3, 2});
}