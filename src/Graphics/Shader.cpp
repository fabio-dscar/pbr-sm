#include <Shader.h>

#include <glad/glad.h>
#include <Utils.h>
#include <regex>

using namespace pbr;
using namespace pbr::util;
using namespace pbr::math;

namespace {

const std::string DefaultVer = "460 core\n\n";
const fs::path ShaderFolder = "./glsl";

} // namespace

enum class pbr::ShaderType : unsigned int {
    Vertex = GL_VERTEX_SHADER,
    Fragment = GL_FRAGMENT_SHADER,
    Geometry = GL_GEOMETRY_SHADER,
    Compute = GL_COMPUTE_SHADER
};

ShaderSource::ShaderSource(const std::string& name, ShaderType type,
                           const std::string& src)
    : name(name), source({src}), type(type) {

    if (!hasVersionDir())
        setVersion(DefaultVer);

    handleIncludes();
}

void ShaderSource::handleIncludes() {
    std::regex rgx(R"([ ]*#[ ]*include[ ]+[\"<](.*)[\">].*)");
    std::smatch smatch;
    std::vector processed{name};

    while (std::regex_search(source, smatch, rgx)) {
        auto file = smatch[1].str();
        if (std::find(processed.begin(), processed.end(), file) != processed.end())
            FATAL("Recursively including '{}' at '{}'.", file, name);

        auto filePath = ShaderFolder / file;
        auto src = ReadTextFile(filePath);
        if (!src)
            FATAL("Couldn't open included file '{}' in '{}'", file, name);

        source.replace(smatch.position(), smatch.length(), src.value());

        processed.push_back(file);
    }
}

bool ShaderSource::hasVersionDir() {
    return source.find("#version ") != std::string::npos;
}

void ShaderSource::setVersion(const std::string& ver) {
    auto start = source.find("#version");

    auto directive = std::format("#version {}\n", ver);
    if (start != std::string::npos) {
        auto end = source.find('\n', start);
        source.replace(start, end - start, directive);
    } else {
        source.insert(0, directive);
    }
}

void ShaderSource::include(const std::string& include) {
    auto start = source.find("#version");
    auto end = 0;

    if (start != std::string::npos)
        end = source.find('\n', start);

    source.insert(end + 1, include);
}

unsigned int ShaderSource::id() const {
    return handle;
}

void ShaderSource::compile(const std::string& defines) {
    handle = glCreateShader(static_cast<GLenum>(type));
    if (handle == 0)
        FATAL("Could not create shader {}", name);

    include(defines);

    const char* sources[] = {source.c_str()};
    glShaderSource(handle, 1, sources, 0);
    glCompileShader(handle);

    GLint result;
    glGetShaderiv(handle, GL_COMPILE_STATUS, &result);
    if (result != GL_TRUE)
        FATAL("Shader {} compilation log:\n{}", name, GetShaderLog(handle));
}

Program::~Program() {
    if (handle != 0)
        glDeleteProgram(handle);
}

void Program::addShader(const ShaderSource& src) {
    sourceHandles.push_back(src.id());
}

void Program::use() const {
    glUseProgram(handle);
}

void Program::link() {
    handle = glCreateProgram();
    if (handle == 0) {
        std::string message = GetProgramError(handle);
        FATAL("Could not create program {}", name);
    }

    for (GLuint sid : sourceHandles)
        glAttachShader(handle, sid);

    glLinkProgram(handle);

    for (GLuint sid : sourceHandles)
        glDetachShader(handle, sid);

    GLint res;
    glGetProgramiv(handle, GL_LINK_STATUS, &res);
    if (res != GL_TRUE)
        FATAL(GetProgramError(handle));
}

void Program::cleanShaders() {
    for (auto sid : sourceHandles)
        if (glIsShader(sid) == GL_TRUE)
            glDeleteShader(sid);
}

void Program::setFloat(int loc, float val) const {
    glProgramUniform1f(handle, loc, val);
}

void Program::setVector3(int loc, const Vec3& val) const {
    glProgramUniform3f(handle, loc, val.x, val.y, val.z);
}

void Program::setSampler(int loc, int val) const {
    glProgramUniform1i(handle, loc, val);
}

std::string pbr::GetShaderLog(unsigned int handle) {
    GLint logLen;
    glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &logLen);

    auto log = std::make_unique<char[]>(logLen);
    glGetShaderInfoLog(handle, logLen, &logLen, log.get());

    return {log.get()};
}

std::string pbr::GetProgramError(unsigned int handle) {
    GLint logLen;
    glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &logLen);

    auto log = std::make_unique<char[]>(logLen);
    glGetProgramInfoLog(handle, logLen, &logLen, log.get());

    return {log.get()};
}

ShaderSource pbr::LoadShaderFile(const fs::path& filePath) {
    using enum ShaderType;
    ShaderType type = Fragment;

    // Deduce type from extension, if possible
    auto ext = filePath.extension();
    if (ext == ".frag" || ext == ".fs")
        type = Fragment;
    else if (ext == ".vert" || ext == ".vs")
        type = Vertex;
    else
        FATAL("Couldn't deduce type for shader: {}", filePath.string());

    return LoadShaderFile(type, filePath);
}

ShaderSource pbr::LoadShaderFile(ShaderType type, const fs::path& filePath) {
    auto source = ReadTextFile(filePath);
    if (!source.has_value())
        FATAL("Couldn't load shader file {}", filePath.string());

    return {filePath.filename().string(), type, source.value()};
}

std::string pbr::BuildDefinesBlock(std::span<std::string> defines) {
    std::string defBlock = "";
    for (const auto& def : defines) {
        if (!def.empty())
            defBlock.append(std::format("#define {}\n", def));
    }
    return defBlock;
}

std::unique_ptr<Program> pbr::CompileAndLinkProgram(const std::string& name,
                                                    std::span<std::string> sourceNames,
                                                    std::span<std::string> definesList) {

    auto program = std::make_unique<Program>(name);
    auto defines = BuildDefinesBlock(definesList);

    for (const auto& fname : sourceNames) {
        ShaderSource s = LoadShaderFile(ShaderFolder / fname);
        s.compile(defines);
        program->addShader(s);
    }

    program->link();
    program->cleanShaders();
    return program;
}