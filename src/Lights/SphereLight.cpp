#include <SphereLight.h>

using namespace pbr;

SphereLight::SphereLight(const Color& emission, float intensity, const Vec3& position,
                         float radius)
    : Light(emission, intensity, position), _radius(radius) {}
SphereLight::SphereLight(const Color& emission, float intensity, const Mat4& toWorld,
                         float radius)
    : Light(emission, intensity, toWorld), _radius(radius) {}

void SphereLight::toData(LightData& data) const {
    data.type = _on ? ToInt(LightType::Sphere) : ToInt(LightType::None);
    data.emission = _intensity * _emission;
    data.position = position();
    data.auxA = _radius;
}