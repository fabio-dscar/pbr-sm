#include <Material.h>

#include <RenderInterface.h>
#include <ParameterMap.h>
#include <PBRMaterial.h>
#include <Shader.h>
#include <Utils.h>

using namespace pbr;
using namespace util;

void Material::use() const {
    _program->use();
}

RRID Material::program() const {
    return _program->id();
}

std::shared_ptr<Material> pbr::CreateMaterial(const ParameterMap& params) {
    auto type = params.lookup("type", "pbr"s);
    fs::path parent = params.lookup("parentdir", ""s);

    auto mat = std::make_unique<PBRMaterial>();

    if (auto tex = params.lookup<std::string>("diffuse"))
        mat->setDiffuse(LoadTexture(parent / tex.value()));
    else
        mat->setDiffuse(params.lookup<Color>("diffuse", Color{0.5f}));

    if (auto tex = params.lookup<std::string>("normal"))
        mat->setNormal(LoadTexture(parent / tex.value()));

    mat->setReflectivity(params.lookup<float>("specular", 0.5f));

    if (auto tex = params.lookup<std::string>("roughness"))
        mat->setRoughness(LoadTexture(parent / tex.value()));
    else
        mat->setRoughness(params.lookup<float>("roughness", 0.2f));

    if (auto tex = params.lookup<std::string>("metallic"))
        mat->setMetallic(LoadTexture(parent / tex.value()));
    else
        mat->setMetallic(params.lookup<float>("metallic", 0.5f));

    if (auto tex = params.lookup<std::string>("ao"))
        mat->setOcclusion(LoadTexture(parent / tex.value()));

    if (auto tex = params.lookup<std::string>("emissive"))
        mat->setEmissive(LoadTexture(parent / tex.value()));

    if (auto tex = params.lookup<std::string>("clearnormal"))
        mat->setEmissive(LoadTexture(parent / tex.value()));

    return mat;
}