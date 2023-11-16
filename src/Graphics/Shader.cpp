#include <Shader.h>

#include <RenderInterface.h>
#include <Utils.h>

using namespace pbr;

const std::string SHADER_PATH = "Shaders/";

ShaderSource::ShaderSource(ShaderType type, const std::string& filePath) {
    _id = 0;
    _type = type;
    _name = filePath;

    std::string path = SHADER_PATH + filePath;
    if (!Utils::readFile(path, std::ios_base::in, _source))
        std::cerr << path;

    if (!compile())
        std::cerr << "Couldn't compile shader: " << path;
}

ShaderSource::~ShaderSource() {
    if (_id != 0)
        RHI.deleteShader(*this);
}

uint32 ShaderSource::id() const {
    return _id;
}

const std::string& ShaderSource::name() const {
    return _name;
}

const std::string& ShaderSource::source() const {
    return _source;
}

ShaderType ShaderSource::type() const {
    return _type;
}

bool ShaderSource::compile() {
    if (_id != 0)
        return false;

    _id = RHI.compileShader(*this);
    return _id != 0;
}

Shader::Shader(const std::string& name) : _id(0), _name(name) {}

uint32 Shader::id() const {
    return _id;
}

bool Shader::addShader(const ShaderSource& source) {
    _shaders.push_back(source.id());
    return true;
}

bool Shader::link() {
    _id = RHI.linkProgram(*this);
    return true;
}

const std::string& Shader::name() const {
    return _name;
}

const vec<uint32>& Shader::shaders() const {
    return _shaders;
}

void Shader::registerUniform(const std::string& name) {
    _uniforms.push_back(RHI.uniformLocation(_id, name));
}

void Shader::registerUniformBlock(const std::string& name) {
    _uniformBlocks.push_back(RHI.uniformBlockLocation(_id, name));
}