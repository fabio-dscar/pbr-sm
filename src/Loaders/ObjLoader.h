#ifndef __PBR_OBJLOADER_H__
#define __PBR_OBJLOADER_H__

#include <PBR.h>
#include <Geometry.h>
#include <Hash.h>

namespace pbr {

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