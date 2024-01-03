#include <Light.h>

#include <PointLight.h>
#include <DirectionalLight.h>
#include <SpotLight.h>
#include <SphereLight.h>
#include <TubeLight.h>

#include <SceneLoader.h>

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

std::unique_ptr<Light> pbr::CreateLight(const ParameterMap& params) {
    // TODO: Better constructors for the lights

    auto type = params.lookup("type", "point"s);

    auto intensity = params.lookup("intensity", 1.0f);
    auto emission = params.lookup("emission", Color{1.0});
    auto pos = params.lookup("position", Vec3{0.0});

    if (type == "point") {
        return std::make_unique<PointLight>(pos, emission, intensity);
    } else if (type == "directional") {
        return std::make_unique<DirectionalLight>(emission, intensity);
    } else if (type == "spot") {
        auto cutoff = radians(params.lookup("cutoff", 36.5f));
        auto outerCutoff = radians(params.lookup("outerCutoff", 40.0f));
        return std::make_unique<SpotLight>(emission, intensity, cutoff, outerCutoff);
    }

    // Area lights
    auto radius = params.lookup("radius", 0.2f);
    if (type == "sphere") {
        return std::make_unique<SphereLight>(emission, intensity, radius);
    } else if (type == "tube") {
        return std::make_unique<TubeLight>(emission, intensity, radius);
    }

    return nullptr;
}
