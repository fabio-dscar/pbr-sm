#ifndef __PBR_UTILS_H__
#define __PBR_UTILS_H__

#include <PBR.h>

#include <filesystem>
#include <optional>
#include <format>

namespace pbr {

class Shape;
class Material;
class ParameterMap;

namespace Utils {

std::optional<std::string> ReadTextFile(const std::string& filePath,
                                        std::ios_base::openmode mode);
void throwError(const std::string& error);

RRID loadTexture(const std::string& path);
std::unique_ptr<Shape> loadSceneObject(const std::string& folder);
std::unique_ptr<Material> buildMaterial(const std::filesystem::path& path,
                                        const ParameterMap& map);



void ExitWithErrorMsg(const std::string& message);

template<typename... Args>
void ExitWithError(const std::format_string<Args...>& fmt, Args&&... args) {
    ExitWithErrorMsg(std::format(fmt, std::forward<Args>(args)...));
}

} // namespace Utils
} // namespace pbr

#endif