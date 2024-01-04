#ifndef __PBR_SPOTLIGHT_H__
#define __PBR_SPOTLIGHT_H__

#include <Light.h>

namespace pbr {

class PBR_SHARED SpotLight : public Light {
public:
    SpotLight();
    SpotLight(const Color& emission, float intensity);
    SpotLight(const Color& emission, float intensity, const Vec3& position, float cutoff,
              float outerCutoff);
    SpotLight(const Color& emission, float intensity, const Mat4& toWorld, float cutoff,
              float outerCutoff);

    float cutOff() const;
    float outerCutOff() const;
    Vec3 direction() const;

    void toData(LightData& data) const override;

private:
    float _cutoff = Radians(36.5f);
    float _outerCutoff = Radians(40.0f);
};

} // namespace pbr

#endif