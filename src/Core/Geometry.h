#ifndef __PBR_GEOMETRY_H__
#define __PBR_GEOMETRY_H__

#include <PBR.h>
#include <PBRMath.h>
#include <Bounds.h>

//#include <Utils.h>

using namespace pbr::math;

namespace pbr {

    struct Vertex {
        Vec3 position;
        Vec3 normal;
        Vec2 uv;
        Vec3 tangent;
    };

    class PBR_SHARED Geometry {
    public:
        RRID rrid() const;
        void setRRID(RRID id);

        const std::vector<Vertex>& vertices() const;
        const std::vector<uint32>& indices()  const;

        void addVertex(const Vertex& vertex);
        void addIndex (uint32 idx);

        void setVertices(const std::vector<Vertex>& vertices);
        void setIndices (const std::vector<uint32>& indices);

        BBox3   bbox()    const;
        BSphere bSphere() const;

        void computeTangents();

    private:
        RRID _id = -1;
        std::vector<uint32> _indices;
        std::vector<Vertex> _vertices;
    };

    PBR_SHARED void genSphereGeometry(Geometry& geo, float radius, uint32 widthSegments, uint32 heightSegments);
    PBR_SHARED void genBoxGeometry(Geometry& geo, uint32 widthSegments, uint32 heightSegments, uint32 depthSegments);
    PBR_SHARED void genUnitCubeGeometry(Geometry& geo);

    // Obj file
    struct ObjVertex {
        Vec3 pos;
        Vec3 normal;
        Vec2 texCoord;

        bool operator==(const ObjVertex& v) const {
            return pos == v.pos &&
                   normal == v.normal &&
                   texCoord == v.texCoord;
        }
    };

    struct ObjFile {
        std::string            objName;
        std::vector<ObjVertex> vertices;
        std::vector<uint32>    indices;
    };

    PBR_SHARED bool loadObj(const std::string& filePath, ObjFile& obj);
    PBR_SHARED void fromObjFile(Geometry& geo, const ObjFile& objFile);
}

#endif