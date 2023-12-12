#ifndef __PBR_SHADER_H__
#define __PBR_SHADER_H__

#include <glad/glad.h>
#include <PBR.h>

#include <filesystem>
#include <span>

namespace pbr {

enum ShaderType {
    VERTEX_SHADER = GL_VERTEX_SHADER,
    FRAGMENT_SHADER = GL_FRAGMENT_SHADER,
    GEOMETRY_SHADER = GL_GEOMETRY_SHADER,
    COMPUTE_SHADER = GL_COMPUTE_SHADER
};

static const std::string VerDirective = "#version 450 core\n\n";
static const std::filesystem::path ShaderFolder = "./glsl";

class PBR_SHARED ShaderSource {
public:
    ShaderSource(const std::string& name, ShaderType type, const std::string& src)
        : name(name), source({src}), type(type) {
        handleIncludes();
    }

    uint32 id() const;

    void compile(const std::string& defines = VerDirective);

private:
    void handleIncludes();

    std::string name;
    std::string source;
    uint32 handle = 0;
    ShaderType type;
};

class PBR_SHARED Program {
public:
    explicit Program(const std::string& name) : name(name) {}
    ~Program();

    void addShader(const ShaderSource& src);
    uint32 id() const { return handle; }
    void link();
    void cleanShaders();

private:
    std::vector<uint32> sourceHandles;
    std::string name;
    uint32 handle = 0;
};

ShaderSource LoadShaderFile(const std::string& filePath);
ShaderSource LoadShaderFile(ShaderType type, const std::string& filePath);
std::unique_ptr<Program> CompileAndLinkProgram(const std::string& name,
                                               std::span<std::string> sourceNames,
                                               std::span<std::string> definesList = {});

std::string BuildDefinesBlock(std::span<std::string> defines);
std::string GetShaderLog(unsigned int handle);
std::string GetProgramError(unsigned int handle);

} // namespace pbr

#endif