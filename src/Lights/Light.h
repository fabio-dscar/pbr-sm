#ifndef __PBR_LIGHT_H__
#define __PBR_LIGHT_H__

#include <SceneObject.h>
#include <Spectrum.h>

namespace pbr {

class Shape;

// Used to pass the type id to shaders
enum LightType : uint8 {
    LIGHT_NONE = 0,
    LIGHT_POINT = 1,
    LIGHT_SPOT = 2,
    LIGHT_DIR = 3,
    LIGHT_SPHERE = 4
};

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

} // namespace pbr

#endif