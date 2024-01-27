#ifndef PBR_SPOTLIGHT_H
#define PBR_SPOTLIGHT_H

#include <Light.h>

namespace pbr {

class SpotLight : public Light {
public:
    SpotLight();
    SpotLight(const Color& emission, float intensity);
    SpotLight(const Color& emission, float intensity, const Vec3& position, float cutoff,
              float outerCutoff);
    SpotLight(const Color& emission, float intensity, const Mat4& toWorld, float cutoff,
              float outerCutoff);

    float cutOff() const;
    float outerCutOff() const;

    void setCutoffs(float cutoff, float outerCutoff) {
        _cutoff = cutoff;
        _outerCutoff = std::max(outerCutoff, cutoff);
    }

    Vec3 direction() const;

    LightType type() const override { return LightType::Spot; }
    void toData(LightData& data) const override;

private:
    float _cutoff = Radians(36.5f);
    float _outerCutoff = Radians(40.0f);
};

} // namespace pbr

#endif