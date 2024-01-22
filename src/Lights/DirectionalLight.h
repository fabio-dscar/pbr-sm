#ifndef __PBR_DIRLIGHT_H__
#define __PBR_DIRLIGHT_H__

#include <Light.h>

namespace pbr {

class DirectionalLight : public Light {
public:
    DirectionalLight(const Color& emission, float intensity);
    DirectionalLight(const Color& emission, float intensity, const Vec3& dir);
    DirectionalLight(const Color& emission, float intensity, const Mat4& orientation);

    Vec3 direction() const;

    LightType type() const override { return LightType::Directional; }
    void toData(LightData& data) const override;
};

} // namespace pbr

#endif