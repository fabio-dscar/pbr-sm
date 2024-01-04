#include <PointLight.h>

using namespace pbr;

PointLight::PointLight(const Color& emission, float intensity)
    : Light(emission, intensity) {}

PointLight::PointLight(const Color& emission, float intensity, const Vec3& position)
    : Light(emission, intensity, position) {}

PointLight::PointLight(const Color& emission, float intensity, const Mat4& toWorld)
    : Light(emission, intensity, toWorld) {}

void PointLight::toData(LightData& data) const {
    data.type = _on ? ToInt(LightType::Point) : ToInt(LightType::None);
    data.emission = _intensity * _emission;
    data.position = position();
}