#include <Renderer.h>

#include <Camera.h>
#include <Light.h>
#include <Shape.h>
#include <Scene.h>
#include <Skybox.h>

#include <RenderInterface.h>

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

void Renderer::setToneParams(std::span<float, 7> toneParams) {
    std::copy(std::begin(toneParams), std::end(toneParams), std::begin(_toneParams));
}

const std::array<float, 7>& Renderer::toneParams() const {
    return _toneParams;
}

void Renderer::setSkyboxDraw(bool state) {
    _drawSkybox = state;
}

void Renderer::uploadUniformBuffer(const Scene& scene, const Camera& camera) {
    // Renderer
    auto rd = _uniformBuffer.getBind<RendererData>(RENDERER_BUFFER);
    rd->gamma = _gamma;
    rd->exposure = _exposure;
    rd->envIntensity = _envIntensity;
    rd->tonemap = ToUnderlying(_toneMap);
    rd->A = _toneParams[0];
    rd->B = _toneParams[1];
    rd->C = _toneParams[2];
    rd->D = _toneParams[3];
    rd->E = _toneParams[4];
    rd->F = _toneParams[5];
    rd->W = _toneParams[6];

    // Camera
    auto cd = _uniformBuffer.getBind<CameraData>(CAMERA_BUFFER);
    cd->viewMatrix = camera.viewMatrix();
    cd->projMatrix = camera.projMatrix();
    cd->viewPos = camera.position();
    cd->viewProjMatrix = camera.viewProjMatrix();

    // Lights
    auto ld = _uniformBuffer.getBind<LightData>(LIGHTS_BUFFER);
    const auto& lights = scene.lights();

    auto numLights = Min(NumLights, lights.size());
    for (std::size_t l = 0; l < numLights; ++l)
        lights[l]->toData(ld[l]);
}

void Renderer::drawShapes(const Scene& scene) {
    for (const auto& shape : scene.shapes())
        shape->draw();
}

void Renderer::drawSkybox(const Scene& scene) const {
    if (scene.hasSkybox())
        scene.skybox().draw();
}

void Renderer::prepare() {
    using enum BufferFlag;

    // Get aligned sizes for separate uniform buffers according to OGL implementation and
    // put them all in a single contiguous buffer
    auto rdSize = RHI.alignUniformBuffer(sizeof(RendererData));
    auto cdSize = RHI.alignUniformBuffer(sizeof(CameraData));
    auto ldSize = RHI.alignUniformBuffer(sizeof(LightData) * NumLights);

    auto cdOffset = rdSize;
    auto ldOffset = cdOffset + cdSize;

    auto uboSize = RHI.alignUniformBuffer(rdSize + cdSize + ldSize);
    _uniformBuffer.create(BufferType::Uniform, 3, uboSize, Write | Persistent | Coherent);

    _uniformBuffer.registerBind(RENDERER_BUFFER, 0, rdSize);
    _uniformBuffer.registerBind(CAMERA_BUFFER, cdOffset, cdSize);
    _uniformBuffer.registerBind(LIGHTS_BUFFER, ldOffset, ldSize);
}

void Renderer::render(const Scene& scene, const Camera& camera) {
    _uniformBuffer.wait();
    _uniformBuffer.rebind();

    uploadUniformBuffer(scene, camera);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawShapes(scene);

    if (_drawSkybox)
        drawSkybox(scene);

    _uniformBuffer.lockAndSwap();
}
