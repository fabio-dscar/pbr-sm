#ifndef __PBR_DIRLIGHT_H__
#define __PBR_DIRLIGHT_H__

#include <Light.h>

namespace pbr {

class PBR_SHARED DirectionalLight : public Light {
public:
    DirectionalLight(const Color& emission, float intensity);

    Vec3 direction() const;

    void toData(LightData& data) const override;
};

} // namespace pbr

#endif