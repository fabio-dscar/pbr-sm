#ifndef __PBR_SHADER_H__
#define __PBR_SHADER_H__

#include <PBR.h>

#include <filesystem>
#include <span>

namespace fs = std::filesystem;

namespace pbr {

enum class ShaderType : unsigned int;

class PBR_SHARED ShaderSource {
public:
    ShaderSource(const std::string& name, ShaderType type, const std::string& src);

    void setVersion(const std::string& ver);
    void include(const std::string& source);

    unsigned int id() const;
    void compile(const std::string& defines = "");

private:
    std::string getVersion();
    bool hasVersionDir();

    void handleIncludes();

    std::string name;
    std::string source;
    unsigned int handle = 0;
    ShaderType type;
};

class PBR_SHARED Program {
public:
    explicit Program(const std::string& name) : name(name) {}
    ~Program();

    void addShader(const ShaderSource& src);
    unsigned int id() const { return handle; }
    void link();
    void cleanShaders();

private:
    std::vector<unsigned int> sourceHandles;
    std::string name;
    unsigned int handle = 0;
};

ShaderSource LoadShaderFile(const fs::path& filePath);
ShaderSource LoadShaderFile(ShaderType type, const fs::path& filePath);

std::unique_ptr<Program> CompileAndLinkProgram(const std::string& name,
                                               std::span<std::string> sourceNames,
                                               std::span<std::string> definesList = {});

std::string BuildDefinesBlock(std::span<std::string> defines);
std::string GetShaderLog(unsigned int handle);
std::string GetProgramError(unsigned int handle);

} // namespace pbr

#endif