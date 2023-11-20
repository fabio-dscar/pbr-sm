#include <SphereLight.h>

using namespace pbr;

void SphereLight::toData(LightData& data) const {
    data.type = _on ? LightType::LIGHT_SPHERE : LightType::LIGHT_NONE;
    data.emission = _intensity * _emission;
    data.position = position();
    data.auxA = _radius;
}