#ifndef __PBR_POINTLIGHT_H__
#define __PBR_POINTLIGHT_H__

#include <Light.h>

namespace pbr {

class PBR_SHARED PointLight : public Light {
public:
    PointLight(const Color& emission, float intensity);
    PointLight(const Vec3& position, const Color& emission, float intensity);

    void toData(LightData& data) const override;
};

} // namespace pbr

#endif