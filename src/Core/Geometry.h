#ifndef __PBR_GEOMETRY_H__
#define __PBR_GEOMETRY_H__

#include <PBR.h>
#include <PBRMath.h>
#include <Bounds.h>

// #include <Utils.h>

using namespace pbr::math;

namespace pbr {

struct Vertex {
    Vec3 position;
    Vec3 normal;
    Vec2 uv;
    Vec4 tangent;

    bool operator==(const Vertex& v) const {
        return position == v.position && normal == v.normal && uv == v.uv &&
               tangent == tangent;
    }
};

class PBR_SHARED Geometry {
public:
    RRID rrid() const;
    void setRRID(RRID id);

    const std::vector<Vertex>& vertices() const;
    const std::vector<uint32>& indices() const;

    void addVertex(const Vertex& vertex);
    void addIndex(uint32 idx);

    void setVertices(const std::vector<Vertex>& vertices);
    void setIndices(const std::vector<uint32>& indices);

    uint32 getNumFaces() { return _indices.size() / 3; }

    Vertex getVertex(uint32 faceIdx, uint32 vertIdx) {
        uint32 idx = _indices[faceIdx * 3 + vertIdx];
        return _vertices[idx];
    }

    void addTangent(uint32 faceIdx, uint32 vertIdx, const Vec3& tan, float sign = 1.0f) {
        uint32 idx = _indices[faceIdx * 3 + vertIdx];
        _vertices[idx].tangent = {tan.x, tan.y, tan.z, sign};
    }

    BBox3 bbox() const;
    BSphere bSphere() const;

    void computeTangents();
    void removeRedundantVerts();

private:
    RRID _id = -1;
    std::vector<uint32> _indices;
    std::vector<Vertex> _vertices;
};

PBR_SHARED std::unique_ptr<Geometry> genUnitSphere(uint32 widthSegments,
                                                   uint32 heightSegments);
PBR_SHARED std::unique_ptr<Geometry> genUnitCube();
PBR_SHARED std::unique_ptr<Geometry> genUnitQuad();

// Obj file
struct ObjVertex {
    Vec3 pos;
    Vec3 normal;
    Vec2 texCoord;
    Vec4 tangent;

    bool operator==(const ObjVertex& v) const {
        return pos == v.pos && normal == v.normal && texCoord == v.texCoord &&
               tangent == tangent;
    }
};

struct ObjFile {
    std::string objName;
    std::vector<ObjVertex> vertices;
    std::vector<uint32> indices;
};

PBR_SHARED bool loadObj(const std::string& filePath, ObjFile& obj);
PBR_SHARED void fromObjFile(Geometry& geo, const ObjFile& objFile);

} // namespace pbr

#endif