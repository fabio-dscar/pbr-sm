#ifndef __PBR_SPHERELIGHT_H__
#define __PBR_SPHERELIGHT_H__

#include <Light.h>

namespace pbr {

class SphereLight : public Light {
public:
    SphereLight() = default;
    SphereLight(const Color& emission, float intensity, const Vec3& position,
                float radius);
    SphereLight(const Color& emission, float intensity, const Mat4& toWorld,
                float radius);

    void toData(LightData& data) const override;

private:
    float _radius = 1.0f;
};

} // namespace pbr

#endif