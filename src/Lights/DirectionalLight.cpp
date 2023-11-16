#include <DirectionalLight.h>

using namespace pbr;

DirectionalLight::DirectionalLight(const Color& emission, float intensity)
    : Light(emission, intensity) {}

Vec3 DirectionalLight::direction() const {
    // By default it is pointing downwards
    const Vec3 n = Vec3(0, -1, 0);
    return rotate(_orientation, n);
}

void DirectionalLight::toData(LightData& data) const {
    data.state = _on;
    data.type = LightType::LIGHTYPE_DIR;
    data.emission = _intensity * _emission;
    data.position = direction();
}
