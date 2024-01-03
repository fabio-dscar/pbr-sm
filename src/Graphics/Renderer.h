#ifndef __PBR_RENDERER_H__
#define __PBR_RENDERER_H__

#include <PBR.h>
#include <Spectrum.h>
#include <span>
#include <Camera.h>
#include <Light.h>
#include <RingBuffer.h>

namespace pbr {

class Scene;

enum BufferIndices : uint32 {
    RENDERER_BUFFER_IDX = 1,
    CAMERA_BUFFER_IDX = 2,
    LIGHTS_BUFFER_IDX = 3
};

enum ToneMap : uint32 { PARAMETRIC = 0, ACES = 1, ACES_BOOSTED = 2, ACES_FAST = 3 };

// Buffer for shaders with renderer information
struct alignas(256) RendererData {
    float gamma;
    float exposure;
    int tonemap;

    // Tone curve control parameters
    float A, B, C, D, E, F, W;

    float envIntensity;
    int perturbNormals;
    int envLighting;
};

static constexpr uint32 NumLights = 5;

struct UniformBlockStruct {
    RendererData rd;
    CameraData cd;
    LightData ld[NumLights];
};

static constexpr uint32 UniformBlockSize = sizeof(UniformBlockStruct);

class PBR_SHARED Renderer {
public:
    void prepare();
    void render(const Scene& scene, const Camera& camera);

    float gamma() const;
    void setGamma(float gamma);

    float exposure() const;
    void setExposure(float exp);

    ToneMap toneMap() const { return _toneMap; }
    void setToneMap(ToneMap toneMap) { _toneMap = toneMap; }

    const float* toneParams() const;
    void setToneParams(std::span<float, 7> toneParams);

    void setSkyboxDraw(bool state);
    void setPerturbNormals(bool state);
    void setEnvLighting(bool state) { _envLighting = state; }
    void setEnvIntensity(float val) { _envIntensity = val; }

private:
    void bindBufferRanges();
    void uploadUniformBuffer(const Scene& scene, const Camera& camera);

    void drawShapes(const Scene& scene);
    void drawSkybox(const Scene& scene) const;

    float _gamma = pbr::Gamma;
    float _exposure = 3.0f;
    ToneMap _toneMap = PARAMETRIC;
    std::array<float, 7> _toneParams = {0.15f, 0.5f, 0.1f, 0.2f, 0.02f, 0.3f, 11.2f};

    bool _drawSkybox = true;
    bool _perturbNormals = true;
    bool _envLighting = true;
    float _envIntensity = 1.0f;

    RingBuffer _uniformBuffer{};
};

} // namespace pbr

#endif
