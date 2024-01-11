#include <TubeLight.h>

#include <Utils.h>

using namespace pbr;

namespace {
const Vec3 EndPointA{-0.5f, 0, 0};
const Vec3 EndPointB{0.5f, 0, 0};
} // namespace

TubeLight::TubeLight(const Color& emission, float intensity, const Vec3& position,
                     float radius)
    : Light(emission, intensity, position), _radius(radius) {}

TubeLight::TubeLight(const Color& emission, float intensity, const Mat4& toWorld,
                     float radius)
    : Light(emission, intensity, toWorld), _radius(radius) {}

void TubeLight::toData(LightData& data) const {
    using enum LightType;
    data.type = _on ? ToUnderlying(Tube) : ToUnderlying(None);
    data.emission = _intensity * _emission;
    data.position = objToWorld() * EndPointA;
    data.auxB = objToWorld() * EndPointB;
    data.auxA = _radius;
}