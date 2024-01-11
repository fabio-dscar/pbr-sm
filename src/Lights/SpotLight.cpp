#include <SpotLight.h>

#include <Utils.h>

using namespace pbr;

namespace {
// By default it is pointing downwards
const Vec3 DefaultDir{0, -1, 0};
} // namespace

SpotLight::SpotLight() : Light() {}

SpotLight::SpotLight(const Color& emission, float intensity)
    : Light(emission, intensity) {}

SpotLight::SpotLight(const Color& emission, float intensity, const Vec3& position,
                     float cutoff, float outerCutoff)
    : Light(emission, intensity, position), _cutoff(cutoff), _outerCutoff(outerCutoff) {}

float SpotLight::cutOff() const {
    return _cutoff;
}

float SpotLight::outerCutOff() const {
    return _outerCutoff;
}

Vec3 SpotLight::direction() const {
    return normalize(Rotate(_orientation, DefaultDir));
}

void SpotLight::toData(LightData& data) const {
    using enum LightType;
    data.type = _on ? ToUnderlying(Spot) : ToUnderlying(None);
    data.emission = _intensity * _emission;
    data.position = position();
    data.auxA = std::cos(_cutoff);
    data.auxC = std::cos(_outerCutoff);
    data.auxB = direction();
}
