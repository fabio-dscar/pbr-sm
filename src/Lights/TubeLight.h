#ifndef PBR_TUBELIGHT_H
#define PBR_TUBELIGHT_H

#include <Light.h>

namespace pbr {

// Length 1 with endpoints:
// [-0.5, 0, 0] and [0.5, 0, 0]
class TubeLight : public Light {
public:
    TubeLight() = default;
    TubeLight(const Color& emission, float intensity, const Vec3& position, float radius);
    TubeLight(const Color& emission, float intensity, const Mat4& toWorld, float radius);

    float radius() const { return _radius; }
    void setRadius(float radius) { _radius = radius; }

    LightType type() const override { return LightType::Tube; }
    void toData(LightData& data) const override;

private:
    float _radius = 1.0f;
};

} // namespace pbr

#endif