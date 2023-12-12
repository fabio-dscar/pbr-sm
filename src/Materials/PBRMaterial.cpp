#include <PBRMaterial.h>

#include <Resources.h>
#include <Texture.h>
#include <Shader.h>
#include <Skybox.h>
#include <RenderInterface.h>

#include <span>
#include <vector>

using namespace pbr;

PBRMaterial::PBRMaterial()
    : Material(), _texHandles(9), _diffuse(1), _f0(0.5f), _metallic(1), _roughness(1) {

    init();
}

PBRMaterial::PBRMaterial(const Color& diff, float metallic, float roughness)
    : Material(), _texHandles(10), _diffuse(diff), _f0(0.5f), _metallic(metallic),
      _roughness(roughness) {

    init();
}

void PBRMaterial::init() {
    _prog = Resource.getShader("unreal")->id();
    _brdfTex = Resource.getTexture("brdf")->rrid();

    RRID nullTex = Resource.getTexture("null")->rrid();
    RRID whiteTex = Resource.getTexture("white")->rrid();
    RRID planarTex = Resource.getTexture("planar")->rrid();

    _diffuseTex = whiteTex;
    _normalTex = nullTex;
    _metallicTex = whiteTex;
    _roughTex = whiteTex;
    _aoTex = whiteTex;
    _emissiveTex = nullTex;
    _clearCoatNormTex = planarTex;
}

void PBRMaterial::prepare() {
    auto v = RHI.queryTexHandles({_diffuseTex, _normalTex, _metallicTex, _roughTex,
                                  _aoTex, _emissiveTex, _clearCoatNormTex, _brdfTex});

    _texHandles[0] = v[0];
    _texHandles[1] = v[1];
    _texHandles[2] = v[2];
    _texHandles[3] = v[3];
    _texHandles[4] = v[4];
    _texHandles[5] = v[5];
    _texHandles[6] = v[6];
    _texHandles[9] = v[7];
}

void PBRMaterial::update(const Skybox& skybox) {
    _irradianceTex = skybox.irradianceTex();
    _ggxTex = skybox.ggxTex();

    _texHandles[7] = RHI.queryTexHandle(_irradianceTex);
    _texHandles[8] = RHI.queryTexHandle(_ggxTex);
}

void PBRMaterial::uploadData() const {
    RHI.setVector3(DIFFUSE_VEC, Vec3(_diffuse.r, _diffuse.g, _diffuse.b));
    RHI.setFloat(REFLECTIVITY_FLOAT, _f0);
    RHI.setFloat(METALLIC_FLOAT, _metallic);
    RHI.setFloat(ROUGHNESS_FLOAT, _roughness);
    RHI.setFloat(CLEARCOAT_FLOAT, _clearCoat);
    RHI.setFloat(CLEARCOAT_ROUGH_FLOAT, _clearCoatRough);

    RHI.bindTextures(1, 10, _texHandles);
}

void PBRMaterial::setIrradianceTex(RRID id) {
    _irradianceTex = id;
}

void PBRMaterial::setBrdfTex(RRID id) {
    _brdfTex = id;
}

void PBRMaterial::setGGXTex(RRID id) {
    _ggxTex = id;
}

void PBRMaterial::setDiffuse(RRID diffTex) {
    _diffuseTex = diffTex;
}

void PBRMaterial::setDiffuse(const Color& diffuse) {
    _diffuse = diffuse;
}

void PBRMaterial::setNormal(RRID normalTex) {
    _normalTex = normalTex;
}

void PBRMaterial::setReflectivity(float spec) {
    _f0 = spec;
}

void PBRMaterial::setMetallic(RRID metalTex) {
    _metallicTex = metalTex;
}

void PBRMaterial::setMetallic(float metallic) {
    _metallic = metallic;
}

void PBRMaterial::setRoughness(RRID roughTex) {
    _roughTex = roughTex;
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
    _aoTex = occlusionTex;
}

void PBRMaterial::setEmissive(RRID emissiveTex) {
    _emissiveTex = emissiveTex;
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
    return _diffuseTex;
}

RRID PBRMaterial::normalTex() const {
    return _normalTex;
}

RRID PBRMaterial::metallicTex() const {
    return _metallicTex;
}

RRID PBRMaterial::roughTex() const {
    return _roughTex;
}