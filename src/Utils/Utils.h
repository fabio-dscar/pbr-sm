#ifndef __PBR_UTIL_H__
#define __PBR_UTIL_H__

#include <PBR.h>

#include <filesystem>
#include <optional>
#include <format>

namespace fs = std::filesystem;

namespace pbr {

class Shape;
class Material;
class OParameterMap;

namespace util {

std::optional<std::string> ReadTextFile(const fs::path& filePath);

RRID LoadTexture(const fs::path& path);
std::unique_ptr<Shape> LoadSceneObject(const std::string& folder);
std::unique_ptr<Material> BuildMaterial(const fs::path& path, const OParameterMap& map);

inline void PrintMsg(std::ostream& stream, const std::string& header,
                     const std::string& message) {
    stream << header << message << '\n';
}

template<typename... Args>
void Print(const std::format_string<Args...>& fmt, Args&&... args) {
    PrintMsg(std::cout, "", std::format(fmt, std::forward<Args>(args)...));
}

template<typename... Args>
void PrintError(const std::format_string<Args...>& fmt, Args&&... args) {
    PrintMsg(std::cerr, "[ERROR] ", std::format(fmt, std::forward<Args>(args)...));
}

} // namespace util
} // namespace pbr

#endif