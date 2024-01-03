#ifndef __PBR_LIGHT_H__
#define __PBR_LIGHT_H__

#include <SceneObject.h>
#include <Spectrum.h>

namespace pbr {

class Shape;

enum class LightType : int {
    None = 0,
    Point = 1,
    Spot = 2,
    Directional = 3,
    Sphere = 4,
    Tube = 5
};

inline int ToInt(LightType type) {
    return static_cast<int>(type);
}

// Light data for shader blocks
// CARE: data is properly aligned, do not change
struct alignas(16) LightData {
    Vec3 position;
    float auxA;
    Color emission;
    int type;
    Vec3 auxB;
    float auxC;
};

class PBR_SHARED Light : public SceneObject {
public:
    Light() = default;
    Light(const Color& emission, float intensity);
    Light(const Vec3& position, const Color& emission, float intensity);
    Light(const Mat4& lightToWorld, const Color& emission, float intensity);
    virtual ~Light() = default;

    bool isOn() const;
    bool castShadows() const;
    float intensity() const;
    Color emission() const;

    virtual void toData(LightData& data) const = 0;
    virtual sref<Shape> shape() const;

protected:
    Color _emission{1.0f}; // Normalized emission
    float _intensity = 1.0f;
    bool _on = true;
    bool _shadows = true;
};

std::unique_ptr<Light> CreateLight(const ParameterMap& params);

} // namespace pbr

#endif