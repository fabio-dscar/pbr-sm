#ifndef PBR_OBJLOADER_H
#define PBR_OBJLOADER_H

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

std::optional<ObjFile> LoadObjFile(const fs::path& filePath);

} // namespace pbr

#endif