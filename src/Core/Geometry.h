#ifndef __PBR_GEOMETRY_H__
#define __PBR_GEOMETRY_H__

#include <PBR.h>
#include <PBRMath.h>
#include <BBox.h>

using namespace pbr::math;

namespace pbr {

class VertexArrays;

struct Vertex {
    Vec3 position;
    Vec3 normal;
    Vec2 uv;
    Vec4 tangent;

    bool operator==(const Vertex& v) const {
        return position == v.position && normal == v.normal && uv == v.uv &&
               tangent == v.tangent;
    }
};

class PBR_SHARED Geometry {
public:
    Geometry() = default;
    Geometry(std::vector<Vertex>&& vertices, std::vector<unsigned int>&& indices);

    void swap(Geometry& rhs) noexcept {
        using std::swap;
        swap(_id, rhs._id);
        swap(_vertices, rhs._vertices);
        swap(_indices, rhs._indices);
    }

    RRID rrid() const;
    void setRRID(RRID id);

    const std::vector<Vertex>& vertices() const;
    const std::vector<unsigned int>& indices() const;

    void addVertex(const Vertex& vertex);
    void addIndex(unsigned int idx);

    unsigned int getNumFaces() { return _indices.size() / 3; }

    const Vertex& getVertex(unsigned int faceIdx, unsigned int vertIdx) const;
    void addTangent(unsigned int faceIdx, unsigned int vertIdx, const Vec3& tan,
                    float sign = 1.0f);

    BBox3 bbox() const;
    BSphere bSphere() const;

    bool isUploaded() const { return _id != -1; }

private:
    void computeTangents();
    void removeRedundantVerts();

    RRID _id = -1;
    std::vector<Vertex> _vertices;
    std::vector<unsigned int> _indices;
    std::shared_ptr<VertexArrays> _varrays;
};

inline void swap(Geometry& lhs, Geometry& rhs) noexcept {
    lhs.swap(rhs);
}

PBR_SHARED std::unique_ptr<Geometry> genUnitSphere(unsigned int widthSegments,
                                                   unsigned int heightSegments);
PBR_SHARED std::unique_ptr<Geometry> genUnitCube();
PBR_SHARED std::unique_ptr<Geometry> genUnitQuad();

} // namespace pbr

#endif