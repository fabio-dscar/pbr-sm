#ifndef PBR_UTIL_H
#define PBR_UTIL_H

#include <PBR.h>

#include <filesystem>
#include <optional>
#include <format>

#include <glad/glad.h>

namespace fs = std::filesystem;

namespace pbr {

class Image;
class CubeImage;

namespace util {

std::optional<std::string> ReadTextFile(const fs::path& filePath);

std::unique_ptr<Image> LoadImage(const fs::path& filePath);
void SaveImage(const fs::path& filePath, const Image& image);

std::unique_ptr<CubeImage> LoadCubemap(const fs::path& filePath);

RRID LoadTexture(const fs::path& path);

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

void GLAPIENTRY OpenGLErrorCallback(GLenum source, GLenum type, GLuint id,
                                    GLenum severity, GLsizei length,
                                    const GLchar* message, const void* userParam);

} // namespace util

template<typename T>
    requires(std::is_enum_v<T> and requires(T e) { EnableBitmaskOperators(e); })
constexpr auto operator|(const T lhs, const T rhs) {
    using underlying = std::underlying_type_t<T>;
    return static_cast<T>(static_cast<underlying>(lhs) | static_cast<underlying>(rhs));
}

template<typename T>
    requires(std::is_enum_v<T> and requires(T e) { EnableBitmaskOperators(e); })
constexpr auto operator&(const T lhs, const T rhs) {
    using underlying = std::underlying_type_t<T>;
    return static_cast<T>(static_cast<underlying>(lhs) & static_cast<underlying>(rhs));
}

template<typename T>
    requires(std::is_enum_v<T> and requires(T e) { EnableConversion(e); })
constexpr auto ToUnderlying(const T enumVal) {
    using underlying = std::underlying_type_t<T>;
    return static_cast<underlying>(enumVal);
}

} // namespace pbr

#endif