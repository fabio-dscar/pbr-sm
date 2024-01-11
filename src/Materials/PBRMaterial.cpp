#include <PBRMaterial.h>

#include <Resources.h>
#include <Texture.h>
#include <Shader.h>
#include <Skybox.h>
#include <RenderInterface.h>

#include <span>
#include <vector>

using namespace pbr;

namespace {
consteval int Index(PBRUniform uniform) {
    return static_cast<int>(uniform) - 1;
}
} // namespace

PBRMaterial::PBRMaterial()
    : Material(), _diffuse(1), _f0(0.5f), _metallic(1), _roughness(1) {

    init();
}

PBRMaterial::PBRMaterial(const Color& diff, float metallic, float roughness)
    : Material(), _diffuse(diff), _f0(0.5f), _metallic(metallic),
      _roughness(roughness) {

    init();
}

void PBRMaterial::init() {
    _program = Resource.get<Program>("pbr");
    _prog = _program->id();

    RRID null = Resource.get<Texture>("null")->id();
    RRID white = Resource.get<Texture>("white")->id();
    RRID planar = Resource.get<Texture>("planar")->id();

    _maps[Index(DIFFUSE_MAP)] = white;
    _maps[Index(NORMAL_MAP)] = planar;
    _maps[Index(METALLIC_MAP)] = white;
    _maps[Index(ROUGHNESS_MAP)] = white;
    _maps[Index(OCCLUSION_MAP)] = white;
    _maps[Index(EMISSIVE_MAP)] = null;
    _maps[Index(CLEARCOAT_NORMAL_MAP)] = planar;
}

void PBRMaterial::uploadData() const {
    _program->setVector3(DIFFUSE_VEC, {_diffuse.r, _diffuse.g, _diffuse.b});
    _program->setFloat(REFLECTIVITY_FLOAT, _f0);
    _program->setFloat(METALLIC_FLOAT, _metallic);
    _program->setFloat(ROUGHNESS_FLOAT, _roughness);
    _program->setFloat(CLEARCOAT_FLOAT, _clearCoat);
    _program->setFloat(CLEARCOAT_ROUGH_FLOAT, _clearCoatRough);

    RHI.bindTextures(1, 7, _maps);
}

void PBRMaterial::setDiffuse(RRID diffTex) {
    _maps[Index(DIFFUSE_MAP)] = diffTex;
}

void PBRMaterial::setDiffuse(const Color& diffuse) {
    _diffuse = diffuse;
}

void PBRMaterial::setNormal(RRID normalTex) {
    _maps[Index(NORMAL_MAP)] = normalTex;
}

void PBRMaterial::setReflectivity(float spec) {
    _f0 = spec;
}

void PBRMaterial::setMetallic(RRID metalTex) {
    _maps[Index(METALLIC_MAP)] = metalTex;
}

void PBRMaterial::setMetallic(float metallic) {
    _metallic = metallic;
}

void PBRMaterial::setRoughness(RRID roughTex) {
    _maps[Index(ROUGHNESS_MAP)] = roughTex;
}

void PBRMaterial::setRoughness(float roughness) {
    _roughness = roughness;
}

void PBRMaterial::setClearCoat(float clearCoat) {
    _clearCoat = clearCoat;
}
void PBRMaterial::setClearCoatRoughness(float roughness) {
    _clearCoatRough = roughness;
}

void PBRMaterial::setOcclusion(RRID occlusionTex) {
    _maps[Index(OCCLUSION_MAP)] = occlusionTex;
}

void PBRMaterial::setEmissive(RRID emissiveTex) {
    _maps[Index(EMISSIVE_MAP)] = emissiveTex;
}

float PBRMaterial::metallic() const {
    return _metallic;
}

float PBRMaterial::roughness() const {
    return _roughness;
}

float PBRMaterial::reflectivity() const {
    return _f0;
}

float PBRMaterial::clearCoat() const {
    return _clearCoat;
}

float PBRMaterial::clearCoatRough() const {
    return _clearCoatRough;
}

Color PBRMaterial::diffuse() const {
    return _diffuse;
}

RRID PBRMaterial::diffuseTex() const {
    return _maps[Index(DIFFUSE_MAP)];
}

RRID PBRMaterial::normalTex() const {
    return _maps[Index(NORMAL_MAP)];
}

RRID PBRMaterial::metallicTex() const {
    return _maps[Index(METALLIC_MAP)];
}

RRID PBRMaterial::roughTex() const {
    return _maps[Index(ROUGHNESS_MAP)];
}

RRID PBRMaterial::emissiveTex() const {
    return _maps[Index(EMISSIVE_MAP)];
}