#include <PointLight.h>

using namespace pbr;

PointLight::PointLight(const Color& emission, float intensity)
    : Light(emission, intensity) {}

PointLight::PointLight(const Vec3& position, const Color& emission, float intensity)
    : Light(position, emission, intensity) {}

void PointLight::toData(LightData& data) const {
    data.state = _on;
    data.type = LightType::LIGHTYPE_POINT;
    data.emission = _intensity * _emission;
    data.position = position();
}