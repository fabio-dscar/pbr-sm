#include <Light.h>

using namespace pbr;

Light::Light(const Color& emission, float intensity)
    : SceneObject(), _emission(emission), _intensity(intensity) {}

Light::Light(const Vec3& position, const Color& emission, float intensity)
    : SceneObject(position), _emission(emission), _intensity(intensity) {}

Light::Light(const Mat4& lightToWorld, const Color& emission, float intensity)
    : SceneObject(lightToWorld), _emission(emission), _intensity(intensity) {}

bool Light::isOn() const {
    return _on;
}

bool Light::castShadows() const {
    return _shadows;
}

float Light::intensity() const {
    return _intensity;
}

Color Light::emission() const {
    return _emission;
}

sref<Shape> Light::shape() const {
    return nullptr;
}