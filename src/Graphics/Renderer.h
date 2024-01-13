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

enum BufferIndices : int {
    RENDERER_BUFFER_IDX = 0,
    CAMERA_BUFFER_IDX = 1,
    LIGHTS_BUFFER_IDX = 2
};

enum class ToneMap : int { Parametric = 0, Aces = 1, BoostedAces = 2, FastAces = 3 };
constexpr bool EnumHasConversion(ToneMap);

// Buffer for shaders with renderer information
struct RendererData {
    float gamma;
    float exposure;
    int tonemap;

    // Tone curve control parameters
    float A, B, C, D, E, F, W;

    float envIntensity;
};

static constexpr unsigned int NumLights = 5;

struct UniformBlockStruct {
    RendererData rd;
    CameraData cd;
    LightData ld[NumLights];
};

static constexpr unsigned int UniformBlockSize = sizeof(UniformBlockStruct);

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

    const std::array<float, 7>& toneParams() const;
    void setToneParams(std::span<float, 7> toneParams);

    void setSkyboxDraw(bool state);
    void setEnvIntensity(float val) { _envIntensity = val; }

private:
    void bindBufferRanges();
    void uploadUniformBuffer(const Scene& scene, const Camera& camera);

    void drawShapes(const Scene& scene);
    void drawSkybox(const Scene& scene) const;

    float _gamma = pbr::Gamma;
    float _exposure = 3.0f;
    ToneMap _toneMap = ToneMap::Parametric;
    std::array<float, 7> _toneParams = {0.15f, 0.5f, 0.1f, 0.2f, 0.02f, 0.3f, 11.2f};

    bool _drawSkybox = true;
    float _envIntensity = 1.0f;

    RingBuffer _uniformBuffer{};
};

} // namespace pbr

#endif
