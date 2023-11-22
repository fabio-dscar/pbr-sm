#ifndef __PBR_UTILS_H__
#define __PBR_UTILS_H__

#include <PBR.h>

#include <filesystem>

namespace pbr {

class Shape;
class Material;
class ParameterMap;

namespace Utils {

bool readFile(const std::string& filePath, std::ios_base::openmode mode, std::string& str);
void throwError(const std::string& error);

RRID loadTexture(const std::string& path);
std::unique_ptr<Shape> loadSceneObject(const std::string& folder);
std::unique_ptr<Material> buildMaterial(const std::filesystem::path& path, const ParameterMap& map);

} // namespace Utils
} // namespace pbr

#endif