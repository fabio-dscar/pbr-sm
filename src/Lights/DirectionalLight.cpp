#include <DirectionalLight.h>

#include <Transform.h>

using namespace pbr;

namespace {
// By default it is pointing downwards
const Vec3 DefaultDir{0, -1, 0};
} // namespace

DirectionalLight::DirectionalLight(const Color& emission, float intensity)
    : Light(emission, intensity) {}

DirectionalLight::DirectionalLight(const Color& emission, float intensity,
                                   const Vec3& dir)
    : Light(emission, intensity) {

    auto lightDir = normalize(dir);

    auto axis = cross(DefaultDir, lightDir);
    auto angle = std::acos(dot(DefaultDir, lightDir));

    setOrientation(Quat{rotationAxis(angle, axis)});
}

DirectionalLight::DirectionalLight(const Color& emission, float intensity,
                                   const Mat4& orientation)
    : Light(emission, intensity) {
    setOrientation(Quat{orientation});
}

Vec3 DirectionalLight::direction() const {
    return normalize(rotate(_orientation, DefaultDir));
}

void DirectionalLight::toData(LightData& data) const {
    data.type = _on ? ToInt(LightType::Directional) : ToInt(LightType::None);
    data.emission = _intensity * _emission;
    data.position = direction();
}
