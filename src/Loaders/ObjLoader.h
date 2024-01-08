#ifndef __PBR_OBJLOADER_H__
#define __PBR_OBJLOADER_H__

#include <PBR.h>
#include <Geometry.h>

#include <filesystem>

namespace fs = std::filesystem;

namespace pbr {

struct ObjFile {
    std::string objName;
    std::vector<Vertex> vertices{};
    std::vector<unsigned int> indices{};
};

PBR_SHARED std::optional<ObjFile> LoadObjFile(const fs::path& filePath);

} // namespace pbr

#endif