#ifndef PBR_MATERIAL_H
#define PBR_MATERIAL_H

#include <PBR.h>

namespace pbr {

class ParameterMap;
class Program;

class Material {
public:
    virtual ~Material() = default;

    void use() const;
    RRID program() const;

    virtual void prepare() = 0;
    virtual void uploadData() const = 0;

protected:
    std::shared_ptr<Program> _program;
};

std::shared_ptr<Material> CreateMaterial(const ParameterMap& params);

} // namespace pbr

#endif