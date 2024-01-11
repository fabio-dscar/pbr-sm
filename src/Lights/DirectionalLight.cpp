#include <DirectionalLight.h>

#include <Transform.h>
#include <Utils.h>

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
    setOrientation(RotationAlign(DefaultDir, lightDir));
}

DirectionalLight::DirectionalLight(const Color& emission, float intensity,
                                   const Mat4& orientation)
    : Light(emission, intensity) {
    setOrientation(Quat{orientation});
}

Vec3 DirectionalLight::direction() const {
    return normalize(Rotate(_orientation, DefaultDir));
}

void DirectionalLight::toData(LightData& data) const {
    using enum LightType;
    data.type = _on ? ToUnderlying(Directional) : ToUnderlying(None);
    data.emission = _intensity * _emission;
    data.position = direction();
}
