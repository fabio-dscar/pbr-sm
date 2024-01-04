#ifndef __PBR_TUBELIGHT_H__
#define __PBR_TUBELIGHT_H__

#include <Light.h>

namespace pbr {

// Length 1 with endpoints:
// [-0.5, 0, 0] and [0.5, 0, 0]
class TubeLight : public Light {
public:
    TubeLight() = default;
    TubeLight(const Color& emission, float intensity, const Vec3& position, float radius);
    TubeLight(const Color& emission, float intensity, const Mat4& toWorld, float radius);

    void toData(LightData& data) const override;

private:
    float _radius = 1.0f;
};

} // namespace pbr

#endif // __TUBELIGHT_H__