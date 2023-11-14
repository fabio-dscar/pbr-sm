#ifndef __PBR_SPOTLIGHT_H__
#define __PBR_SPOTLIGHT_H__

#include <Light.h>

namespace pbr {

class PBR_SHARED SpotLight : public Light {
public:
    SpotLight();
    SpotLight(const Color& emission, float intensity);
    SpotLight(const Color& emission, float intensity, float cutoff, float outerCutoff);

    float cutOff() const;
    float outerCutOff() const;
    Vec3 direction() const;

    void toData(LightData& data) const override;

private:
    float _cutoff;
    float _outerCutoff;
};

} // namespace pbr

#endif