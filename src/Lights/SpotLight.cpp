#include <SpotLight.h>

using namespace pbr;

SpotLight::SpotLight() : Light(), _cutoff(radians(36.5f)), _outerCutoff(radians(40.0f)) {}

SpotLight::SpotLight(const Color& emission, float intensity)
    : Light(emission, intensity), _cutoff(radians(36.5f)), _outerCutoff(radians(40.0f)) {}

SpotLight::SpotLight(const Color& emission, float intensity, float cutoff,
                     float outerCutoff)
    : Light(emission, intensity), _cutoff(cutoff), _outerCutoff(outerCutoff) {}

float SpotLight::cutOff() const {
    return _cutoff;
}

float SpotLight::outerCutOff() const {
    return _outerCutoff;
}

Vec3 SpotLight::direction() const {
    // By default it is pointing downwards
    const Vec3 n = Vec3(0, -1, 0);
    return normalize(rotate(_orientation, n));
}

void SpotLight::toData(LightData& data) const {
    data.type = _on ? ToInt(LightType::Spot) : ToInt(LightType::None);
    data.emission = _intensity * _emission;
    data.position = position();
    data.auxA = std::cos(_cutoff);
    data.auxC = std::cos(_outerCutoff);
    data.auxB = direction();
}
