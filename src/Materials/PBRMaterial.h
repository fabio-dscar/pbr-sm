#ifndef PBR_PBRMATERIAL_H
#define PBR_PBRMATERIAL_H

#include <Material.h>
#include <PBR.h>
#include <Spectrum.h>

using namespace pbr::math;

namespace pbr {

enum PBRUniform : int {
    DIFFUSE_MAP = 1,
    NORMAL_MAP = 2,
    METALLIC_MAP = 3,
    ROUGHNESS_MAP = 4,
    OCCLUSION_MAP = 5,
    EMISSIVE_MAP = 6,
    CLEARCOAT_NORMAL_MAP = 7,
    DIFFUSE_VEC = 8,
    METALLIC_FLOAT = 9,
    ROUGHNESS_FLOAT = 10,
    REFLECTIVITY_FLOAT = 11,
    CLEARCOAT_ROUGH_FLOAT = 12,
    CLEARCOAT_FLOAT = 13,

    ENV_IRRADIANCE_MAP = 15,
    ENV_GGX_MAP = 16,
    ENV_BRDF_MAP = 17
};

class PBRMaterial : public Material {
public:
    PBRMaterial();
    PBRMaterial(const Color& diff, float metallic, float roughness);

    void prepare() override {}
    void uploadData() const override;

    void setDiffuse(RRID diffTex);
    void setDiffuse(const Color& diffuse);

    void setNormal(RRID normalTex);
    void setReflectivity(float spec);

    void setMetallic(RRID metalTex);
    void setMetallic(float metallic);

    void setRoughness(RRID roughTex);
    void setRoughness(float roughness);

    void setClearCoat(float clearCoat);
    void setClearCoatRoughness(float roughness);

    void setOcclusion(RRID occlusionTex);
    void setEmissive(RRID emissiveTex);

    float metallic() const;
    float roughness() const;
    float reflectivity() const;
    float clearCoat() const;
    float clearCoatRough() const;
    Color diffuse() const;

    RRID diffuseTex() const;
    RRID normalTex() const;
    RRID metallicTex() const;
    RRID roughTex() const;
    RRID emissiveTex() const;

private:
    void init();

    std::array<RRID, 7> _maps;

    Color _diffuse;
    float _f0 = 0.5f;
    float _metallic = 1;
    float _roughness = 1;
    float _clearCoat = 0;
    float _clearCoatRough = 0;
};

} // namespace pbr

#endif