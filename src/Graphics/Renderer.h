#ifndef __PBR_RENDERER_H__
#define __PBR_RENDERER_H__

#include <PBR.h>
#include <Spectrum.h>

namespace pbr {

class Scene;
class Camera;

static constexpr uint32 NUM_LIGHTS = 4;

enum BufferIndices : uint32 {
    CAMERA_BUFFER_IDX = 0,
    LIGHTS_BUFFER_IDX = 1,
    RENDERER_BUFFER_IDX = 2
};

// Buffer for shaders with renderer information
struct RendererBuffer {
    float gamma;
    float exp;

    // Uncharted tone curve control parameters
    float A, B, C, D, E, F, W;

    bool perturbNormals;
    uint8 pad[3];
    bool envLighting;
};

class PBR_SHARED Renderer {
public:
    void prepare();
    void render(const Scene& scene, const Camera& camera);

    float gamma() const;
    void setGamma(float gamma);

    float exposure() const;
    void setExposure(float exp);

    const float* toneParams() const;
    void setToneParams(float toneParams[7]);

    void setSkyboxDraw(bool state);
    void setPerturbNormals(bool state);

private:
    void uploadRendererBuffer();
    void uploadLightsBuffer(const Scene& scene);
    void uploadCameraBuffer(const Camera& camera);
    void drawShapes(const Scene& scene);
    void drawSkybox(const Scene& scene);

    float _gamma = pbr::Gamma;
    float _exposure = 3.0f;
    float _toneParams[7] = {0.15f, 0.5f, 0.1f, 0.2f, 0.02f, 0.3f, 11.2f};

    bool _drawSkybox = true;
    bool _perturbNormals = true;

    RRID _lightsBuffer;
    RRID _cameraBuffer;
    RRID _rendererBuffer;
};

} // namespace pbr

#endif
