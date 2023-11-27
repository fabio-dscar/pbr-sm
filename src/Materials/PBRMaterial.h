#ifndef __PBR_PBRMATERIAL_H__
#define __PBR_PBRMATERIAL_H__

#include <Material.h>
#include <PBR.h>
#include <Spectrum.h>

using namespace pbr::math;

namespace pbr {

enum PBRUniform {
    DIFFUSE_MAP = 1,
    NORMAL_MAP = 2,
    METALLIC_MAP = 3,
    ROUGHNESS_MAP = 4,
    DIFFUSE_VEC = 5,
    METALLIC_FLOAT = 6,
    ROUGHNESS_FLOAT = 7,
    SPECULAR_VEC = 8,

    ENV_IRRADIANCE_MAP = 15,
    ENV_GGX_MAP = 16,
    ENV_BRDF_MAP = 17
};

class PBR_SHARED PBRMaterial : public Material {
public:
    PBRMaterial();
    PBRMaterial(const Color& diff, float metallic, float roughness);

    void prepare() override;
    void update(const Skybox& skybox) override;
    void uploadData() const override;

    void setDiffuse(RRID diffTex);
    void setDiffuse(const Color& diffuse);

    void setNormal(RRID normalTex);
    void setSpecular(const Color& spec);

    void setMetallic(RRID metalTex);
    void setMetallic(float metallic);

    void setRoughness(RRID roughTex);
    void setRoughness(float roughness);

    void setIrradianceTex(RRID id);
    void setBrdfTex(RRID id);
    void setGGXTex(RRID id);

    float metallic() const;
    float roughness() const;
    Color specular() const;
    Color diffuse() const;

    RRID diffuseTex() const;
    RRID normalTex() const;
    RRID metallicTex() const;
    RRID roughTex() const;

private:
    void init();

    std::vector<uint32> _texHandles;
    
    RRID _diffuseTex;
    RRID _normalTex;
    RRID _metallicTex;
    RRID _roughTex;

    // PBR maps
    RRID _irradianceTex;
    RRID _brdfTex;
    RRID _ggxTex;

    Color _diffuse;
    Color _f0;
    float _metallic = 1;
    float _roughness = 1;
};

} // namespace pbr

#endif