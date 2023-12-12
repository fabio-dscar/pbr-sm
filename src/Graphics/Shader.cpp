#include <Shader.h>

#include <RenderInterface.h>
#include <Utils.h>

#include <regex>

using namespace pbr;
using namespace pbr::Utils;
using namespace std::filesystem;

void ShaderSource::handleIncludes() {
    std::regex rgx(R"([ ]*#[ ]*include[ ]+[\"<](.*)[\">].*)");
    std::smatch smatch;
    std::vector processed{name};

    while (std::regex_search(source, smatch, rgx)) {
        auto file = smatch[1].str();
        if (std::find(processed.begin(), processed.end(), file) != processed.end())
            ExitWithError("Recursively including '{}' at '{}'.", file, name);

        auto filePath = ShaderFolder / file;
        auto src = ReadTextFile(filePath, std::ios_base::in);
        if (!src)
            ExitWithError("Couldn't open included file '{}' in '{}'", file, name);

        source.replace(smatch.position(), smatch.length(), src.value());

        processed.push_back(file);
    }
}

uint32 ShaderSource::id() const {
    return handle;
}

void ShaderSource::compile(const std::string& defines) {
    handle = glCreateShader(type);
    if (handle == 0)
        ExitWithError("Could not create shader {}", name);

    const char* sources[] = {defines.c_str(), source.c_str()};
    glShaderSource(handle, 2, sources, 0);
    glCompileShader(handle);

    GLint result;
    glGetShaderiv(handle, GL_COMPILE_STATUS, &result);
    if (result != GL_TRUE) {
        std::string message = GetShaderLog(handle);
        ExitWithError("Shader {} compilation log:\n{}", name, message);
    }
}


Program::~Program() {
    if (handle != 0)
        glDeleteProgram(handle);
}

void Program::addShader(const ShaderSource& src) {
    sourceHandles.push_back(src.id());
}

void Program::link() {
    handle = glCreateProgram();
    if (handle == 0) {
        std::string message = GetProgramError(handle);
        ExitWithError("Could not create program {}", name);
    }

    for (GLuint sid : sourceHandles)
        glAttachShader(handle, sid);

    glLinkProgram(handle);

    GLint res;
    glGetProgramiv(handle, GL_LINK_STATUS, &res);
    if (res != GL_TRUE) {
        for (GLuint sid : sourceHandles)
            glDetachShader(handle, sid);

        std::string message = GetProgramError(handle);
        ExitWithErrorMsg(message);
    }

    // Detach shaders after successful linking
    for (GLuint sid : sourceHandles)
        glDetachShader(handle, sid);
}

void Program::cleanShaders() {
    for (auto sid : sourceHandles)
        if (glIsShader(sid) == GL_TRUE)
            glDeleteShader(sid);
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

ShaderSource pbr::LoadShaderFile(const std::string& fileName) {
    ShaderType type = FRAGMENT_SHADER;

    // Deduce type from extension, if possible
    auto ext = path(fileName).extension();
    if (ext == ".frag" || ext == ".fs")
        type = FRAGMENT_SHADER;
    else if (ext == ".vert" || ext == ".vs")
        type = VERTEX_SHADER;
    else
        ExitWithError("Couldn't deduce type for shader: {}", fileName);

    return LoadShaderFile(type, fileName);
}

ShaderSource pbr::LoadShaderFile(ShaderType type, const std::string& fileName) {
    auto filePath = ShaderFolder / fileName;
    auto source = ReadTextFile(ShaderFolder / fileName, std::ios_base::in);
    if (!source.has_value())
        ExitWithError("Couldn't load shader file {}", filePath.string());

    return {filePath.filename(), type, source.value()};
}

std::string pbr::BuildDefinesBlock(std::span<std::string> defines) {
    std::string defBlock = VerDirective;
    for (auto& def : defines) {
        if (!def.empty()) {
            defBlock.append("#define ");
            defBlock.append(def.begin(), def.end());
            defBlock.append("\n");
        }
    }
    return defBlock;
}

std::unique_ptr<Program> pbr::CompileAndLinkProgram(const std::string& name,
                                                    std::span<std::string> sourceNames,
                                                    std::span<std::string> definesList) {

    auto program = std::make_unique<Program>(name);
    auto defines = BuildDefinesBlock(definesList);

    for (auto& fname : sourceNames) {
        ShaderSource s = LoadShaderFile(fname);
        s.compile(defines);
        program->addShader(s);
    }

    program->link();
    program->cleanShaders();
    return program;
}