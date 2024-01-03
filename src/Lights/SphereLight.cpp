#include <SphereLight.h>

using namespace pbr;

void SphereLight::toData(LightData& data) const {
    data.type = _on ? ToInt(LightType::Sphere) : ToInt(LightType::None);
    data.emission = _intensity * _emission;
    data.position = position();
    data.auxA = _radius;
}