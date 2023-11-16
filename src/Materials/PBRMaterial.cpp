#include <PBRMaterial.h>

#include <Resources.h>
#include <Texture.h>
#include <Shader.h>
#include <Skybox.h>
#include <RenderInterface.h>

#include <span>
#include <vector>

using namespace pbr;

constexpr int64 NOT_SET = -1;

PBRMaterial::PBRMaterial()
    : Material(), _texHandles(8), _diffuse(NOT_SET), _f0(0.04f), _metallic(NOT_SET),
      _roughness(NOT_SET) {

    _prog = Resource.getShader("unreal")->id();

    _brdfTex = Resource.getTexture("brdf")->rrid();

    RRID nullTex = Resource.getTexture("null")->rrid();

    _diffuseTex = nullTex;
    _normalTex = nullTex;
    _metallicTex = nullTex;
    _roughTex = nullTex;
}

PBRMaterial::PBRMaterial(const Color& diff, float metallic, float roughness)
    : Material(), _texHandles(8), _diffuse(diff), _f0(0.04f), _metallic(metallic),
      _roughness(roughness) {

    _prog = Resource.getShader("unreal")->id();

    _brdfTex = Resource.getTexture("brdf")->rrid();

    RRID nullTex = Resource.getTexture("null")->rrid();

    _diffuseTex = nullTex;
    _normalTex = nullTex;
    _metallicTex = nullTex;
    _roughTex = nullTex;
}

void PBRMaterial::prepare() {
    auto v =
        RHI.queryTexHandles({_diffuseTex, _normalTex, _metallicTex, _roughTex, _brdfTex});

    _texHandles[0] = v[0];
    _texHandles[1] = v[1];
    _texHandles[2] = v[2];
    _texHandles[3] = v[3];
    _texHandles[4] = 0;
    _texHandles[7] = v[4];
}

void PBRMaterial::update(const Skybox& skybox) {
    _irradianceTex = skybox.irradianceTex();
    _ggxTex = skybox.ggxTex();

    _texHandles[5] = RHI.queryTexHandle(_irradianceTex);
    _texHandles[6] = RHI.queryTexHandle(_ggxTex);
}

void PBRMaterial::uploadData() const {
    RHI.setVector3(DIFFUSE_VEC, Vec3(_diffuse.r, _diffuse.g, _diffuse.b));
    RHI.setVector3(SPECULAR_VEC, Vec3(_f0.r, _f0.g, _f0.b));
    RHI.setFloat(METALLIC_FLOAT, _metallic);
    RHI.setFloat(ROUGHNESS_FLOAT, _roughness);

    RHI.bindTextures(1, 8, _texHandles);
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

void PBRMaterial::setSpecular(const Color& spec) {
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

float PBRMaterial::metallic() const {
    return _metallic;
}

float PBRMaterial::roughness() const {
    return _roughness;
}

Color PBRMaterial::specular() const {
    return _f0;
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