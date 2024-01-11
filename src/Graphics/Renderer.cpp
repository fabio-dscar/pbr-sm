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

void Renderer::setPerturbNormals(bool state) {
    _perturbNormals = state ? 1 : 0;
}

void Renderer::uploadUniformBuffer(const Scene& scene, const Camera& camera) {
    // Renderer
    auto rd = _uniformBuffer.getBind<RendererData>(RENDERER_BUFFER_IDX);
    rd->gamma = _gamma;
    rd->exposure = _exposure;
    rd->tonemap = _toneMap;

    rd->A = _toneParams[0];
    rd->B = _toneParams[1];
    rd->C = _toneParams[2];
    rd->D = _toneParams[3];
    rd->E = _toneParams[4];
    rd->F = _toneParams[5];
    rd->W = _toneParams[6];

    rd->envIntensity = _envIntensity;
    rd->perturbNormals = _perturbNormals ? 1 : 0;
    rd->envLighting = _envLighting ? 1 : 0;

    // Camera
    auto cd = _uniformBuffer.getBind<CameraData>(CAMERA_BUFFER_IDX);
    cd->viewMatrix = camera.viewMatrix();
    cd->projMatrix = camera.projMatrix();
    cd->viewPos = camera.position();
    cd->viewProjMatrix = camera.viewProjMatrix();

    // Lights
    auto ld = _uniformBuffer.getBind<LightData>(LIGHTS_BUFFER_IDX);
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
    auto rdSize = AlignUniformBuffer(sizeof(RendererData));
    auto cdSize = AlignUniformBuffer(sizeof(CameraData));
    auto ldSize = AlignUniformBuffer(sizeof(LightData) * NumLights);

    auto cdOffset = rdSize;
    auto ldOffset = cdOffset + cdSize;

    auto uboSize = AlignUniformBuffer(rdSize + cdSize + ldSize);
    _uniformBuffer.create(BufferType::Uniform, 3, uboSize, Write | Persistent | Coherent);

    _uniformBuffer.registerBind(RENDERER_BUFFER_IDX, 0, rdSize);
    _uniformBuffer.registerBind(CAMERA_BUFFER_IDX, cdOffset, cdSize);
    _uniformBuffer.registerBind(LIGHTS_BUFFER_IDX, ldOffset, ldSize);
}

void Renderer::render(const Scene& scene, const Camera& camera) {
    _uniformBuffer.wait();
    _uniformBuffer.rebind();

    uploadUniformBuffer(scene, camera);

    drawShapes(scene);

    if (_drawSkybox)
        drawSkybox(scene);

    _uniformBuffer.lockAndSwap();
}
