#ifndef __PBR_SHADER_H__
#define __PBR_SHADER_H__

#include <PBR.h>

namespace pbr {

template<class T>
using vec = std::vector<T>;

enum ShaderType {
    VERTEX_SHADER = 0,
    FRAGMENT_SHADER = 1,
    GEOMETRY_SHADER = 2,
    COMPUTE_SHADER = 3
};

class PBR_SHARED ShaderSource {
public:
    ShaderSource(ShaderType type, const std::string& filePath);
    ~ShaderSource();

    uint32 id() const;
    ShaderType type() const;

    const std::string& name() const;
    const std::string& source() const;

    bool compile();

private:
    std::string _name;
    std::string _source;
    uint32 _id = 0;
    ShaderType _type;
};

class PBR_SHARED Shader {
public:
    explicit Shader(const std::string& name);

    uint32 id() const;
    void addShader(const ShaderSource& source);
    void link();

    const std::string& name() const;
    const std::vector<uint32>& shaders() const;

    void registerUniform(const std::string& name);
    void registerUniformBlock(const std::string& name);

private:
    std::vector<uint32> _shaders;
    std::vector<int32> _uniforms;
    std::vector<uint32> _uniformBlocks;
    std::string _name;
    uint32 _id = 0;
};

} // namespace pbr

#endif