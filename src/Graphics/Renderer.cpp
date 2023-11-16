#include <Renderer.h>

#include <Camera.h>
#include <Light.h>
#include <Shape.h>
#include <Scene.h>
#include <Skybox.h>

#include <RenderInterface.h>

#include <cstring>

using namespace pbr;

void Renderer::setGamma(float gamma) {
    _gamma = gamma;
}

float Renderer::gamma() const {
    return _gamma;
}

float Renderer::exposure() const {
    return _exposure;
}

void Renderer::setExposure(float exp) {
    _exposure = exp;
}

void Renderer::setToneParams(float toneParams[7]) {
    std::memcpy(_toneParams, toneParams, sizeof(float) * 7);
}

const float* Renderer::toneParams() const {
    return &_toneParams[0];
}

void Renderer::setSkyboxDraw(bool state) {
    _drawSkybox = state;
}

void Renderer::setPerturbNormals(bool state) {
    _perturbNormals = state;
}

void Renderer::uploadLightsBuffer(const Scene& scene) {
    const vec<sref<Light>>& lights = scene.lights();

    // Create light buffer
    LightData data[NUM_LIGHTS];
    std::memset(data, 0, sizeof(LightData) * NUM_LIGHTS);

    // Copy light data to buffer
    // Only send NUM_LIGHTS at maximum
    uint32 numLights = min(NUM_LIGHTS, lights.size());
    for (uint32 l = 0; l < numLights; ++l)
        lights[l]->toData(data[l]);

    // Upload the buffer to the GPU
    RHI.updateBuffer(_lightsBuffer, sizeof(LightData) * NUM_LIGHTS, &data);
}

void Renderer::uploadCameraBuffer(const Camera& camera) {
    CameraData data;
    data.viewMatrix = camera.viewMatrix();
    data.projMatrix = camera.projMatrix();
    data.viewPos = camera.position();
    data.viewProjMatrix = camera.viewProjMatrix();

    // Upload the buffer to the GPU
    RHI.updateBuffer(_cameraBuffer, sizeof(CameraData), &data);
}

void Renderer::uploadRendererBuffer() {
    RendererBuffer data;
    data.gamma = _gamma;
    data.exp = _exposure;

    // Tone map parameters
    data.A = _toneParams[0];
    data.B = _toneParams[1];
    data.C = _toneParams[2];
    data.D = _toneParams[3];
    data.E = _toneParams[4];
    data.F = _toneParams[5];
    data.W = _toneParams[6];

    data.perturbNormals = _perturbNormals;
    data.envLighting = true;

    // Upload the buffer to the GPU
    RHI.updateBuffer(_rendererBuffer, sizeof(RendererBuffer), &data);
}

void Renderer::drawShapes(const Scene& scene) {
    for (const auto& shape : scene.shapes())
        shape->draw();
}

void Renderer::drawSkybox(const Scene& scene) {
    if (scene.hasSkybox()) {
        const Skybox& sky = scene.skybox();
        sky.draw();
    }
}

void Renderer::prepare() {
    // Create shared buffers in the GPU
    _lightsBuffer = RHI.createBuffer(BUFFER_SHARED, DYNAMIC, sizeof(LightData) * NUM_LIGHTS, 0);
    _cameraBuffer = RHI.createBuffer(BUFFER_SHARED, DYNAMIC, sizeof(CameraData), 0);
    _rendererBuffer = RHI.createBuffer(BUFFER_SHARED, DYNAMIC, sizeof(RendererBuffer), 0);

    // Bind buffers to known indices
    RHI.bindBufferBase(_cameraBuffer, CAMERA_BUFFER_IDX);
    RHI.bindBufferBase(_lightsBuffer, LIGHTS_BUFFER_IDX);
    RHI.bindBufferBase(_rendererBuffer, RENDERER_BUFFER_IDX);
}

void Renderer::render(const Scene& scene, const Camera& camera) {
    // Upload constant buffers to the GPU
    uploadRendererBuffer();
    uploadLightsBuffer(scene);
    uploadCameraBuffer(camera);

    // Draw scene objects
    drawShapes(scene);

    // Draw skybox
    if (_drawSkybox)
        drawSkybox(scene);
}
