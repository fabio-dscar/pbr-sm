#ifndef __PBR_SPHERELIGHT_H__
#define __PBR_SPHERELIGHT_H__

#include <Light.h>

namespace pbr {

class SphereLight : public Light {
public:
    SphereLight() = default;
    SphereLight(const Color& emission, float intensity, float radius)
        : Light(emission, intensity), _radius(radius) {}

    void toData(LightData& data) const override;

private:
    float _radius = 1.0f;
};

} // namespace pbr

#endif