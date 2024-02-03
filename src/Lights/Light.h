#ifndef PBR_LIGHT_H
#define PBR_LIGHT_H

#include <SceneObject.h>
#include <Spectrum.h>

namespace pbr {

enum class LightType : int {
    None = 0,
    Point = 1,
    Spot = 2,
    Directional = 3,
    Sphere = 4,
    Tube = 5
};

consteval bool EnableConversion(LightType);

// Light data for shader blocks
// CARE: data is properly aligned to std140, do not change
struct LightData {
    alignas(16) Vec3 position;
    float auxA;
    alignas(16) Color emission;
    int type;
    alignas(16) Vec3 auxB;
    float auxC;
};

class Light : public SceneObject {
public:
    Light() = default;
    Light(const Color& emission, float intensity);
    Light(const Color& emission, float intensity, const Vec3& position);
    Light(const Color& emission, float intensity, const Mat4& lightToWorld);
    virtual ~Light() = default;

    void toggle() { _on = !_on; }
    bool isOn() const { return _on; }

    float intensity() const { return _intensity; }
    void setIntensity(float intensity) { _intensity = intensity; }

    Color emission() const { return _emission; }
    void setEmission(const Color& emission) { _emission = emission; }

    virtual LightType type() const = 0;
    virtual void toData(LightData& data) const = 0;

protected:
    Color _emission{1.0f}; // Normalized emission
    float _intensity = 1.0f;
    bool _on = true;
};

std::unique_ptr<Light> CreateLight(const ParameterMap& params);

} // namespace pbr

#endif