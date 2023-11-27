#include <TubeLight.h>

using namespace pbr;

void TubeLight::toData(LightData& data) const {
    Vec3 pt1 = {-0.5f, 0, 0};
    Vec3 pt2 = {0.5f, 0, 0};

    data.type = _on ? LightType::LIGHT_TUBE : LightType::LIGHT_NONE;
    data.emission = _intensity * _emission;
    data.position = objToWorld() * pt1;
    data.auxB = objToWorld() * pt2;
    data.auxA = _radius;
}