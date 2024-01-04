#include <Light.h>

#include <PointLight.h>
#include <DirectionalLight.h>
#include <SpotLight.h>
#include <SphereLight.h>
#include <TubeLight.h>

#include <ParameterMap.h>

using namespace pbr;

Light::Light(const Color& emission, float intensity)
    : SceneObject(), _emission(emission), _intensity(intensity) {}

Light::Light(const Color& emission, float intensity, const Vec3& position)
    : SceneObject(position), _emission(emission), _intensity(intensity) {}

Light::Light(const Color& emission, float intensity, const Mat4& lightToWorld)
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

std::unique_ptr<Light> pbr::CreateLight(const ParameterMap& params) {
    auto type = params.lookup("type", "point"s);

    auto intensity = params.lookup("intensity", 1.0f);
    auto emission = params.lookup("emission", Color{1.0});
    auto pos = params.lookup("position", Vec3{0.0});

    if (type == "point") {
        return std::make_unique<PointLight>(emission, intensity, pos);
    } else if (type == "directional") {
        auto dir = params.lookup("dir", Vec3{0, -1, 0});
        return std::make_unique<DirectionalLight>(emission, intensity, dir);
    } else if (type == "spot") {
        auto cutoff = Radians(params.lookup("cutoff", 36.5f));
        auto outerCutoff = Radians(params.lookup("outerCutoff", 40.0f));
        return std::make_unique<SpotLight>(emission, intensity, pos, cutoff, outerCutoff);
    }

    // Area lights
    auto radius = params.lookup("radius", 0.2f);
    if (type == "sphere") {
        return std::make_unique<SphereLight>(emission, intensity, pos, radius);
    } else if (type == "tube") {
        return std::make_unique<TubeLight>(emission, intensity, pos, radius);
    }

    return nullptr;
}
