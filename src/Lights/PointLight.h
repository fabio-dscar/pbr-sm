#ifndef __PBR_POINTLIGHT_H__
#define __PBR_POINTLIGHT_H__

#include <Light.h>

namespace pbr {

class PointLight : public Light {
public:
    PointLight(const Color& emission, float intensity);
    PointLight(const Color& emission, float intensity, const Vec3& position);
    PointLight(const Color& emission, float intensity, const Mat4& toWorld);

    LightType type() const override { return LightType::Point; }
    void toData(LightData& data) const override;
};

} // namespace pbr

#endif