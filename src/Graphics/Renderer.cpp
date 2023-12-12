#include "glad/glad.h"
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

const float* Renderer::toneParams() const {
    return _toneParams.data();
}

void Renderer::setSkyboxDraw(bool state) {
    _drawSkybox = state;
}

void Renderer::setPerturbNormals(bool state) {
    _perturbNormals = state ? 1 : 0;
}

void Renderer::uploadUniformBuffer(const Scene& scene, const Camera& camera) {
    auto ub = _uniformBuffer.get<UniformBlockStruct>();

    // Renderer
    ub->rd.gamma = _gamma;
    ub->rd.exposure = _exposure;
    ub->rd.tonemap = _toneMap;

    ub->rd.A = _toneParams[0];
    ub->rd.B = _toneParams[1];
    ub->rd.C = _toneParams[2];
    ub->rd.D = _toneParams[3];
    ub->rd.E = _toneParams[4];
    ub->rd.F = _toneParams[5];
    ub->rd.W = _toneParams[6];

    ub->rd.envIntensity = _envIntensity;
    ub->rd.perturbNormals = _perturbNormals ? 1 : 0;
    ub->rd.envLighting = _envLighting ? 1 : 0;

    // Camera
    ub->cd.viewMatrix = camera.viewMatrix();
    ub->cd.projMatrix = camera.projMatrix();
    ub->cd.viewPos = camera.position();
    ub->cd.viewProjMatrix = camera.viewProjMatrix();

    // Lights
    const auto& lights = scene.lights();

    uint32 numLights = min(NumLights, lights.size());
    for (uint32 l = 0; l < numLights; ++l)
        lights[l]->toData(ub->ld[l]);
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
    // Triple buffered uniform buffer
    _uniformBuffer.create(EBufferType::UNIFORM, 3, UniformBlockSize,
                          GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);

    _uniformBuffer.registerBind(RENDERER_BUFFER_IDX, 0, sizeof(RendererData));
    _uniformBuffer.registerBind(CAMERA_BUFFER_IDX, offsetof(UniformBlockStruct, cd),
                                sizeof(CameraData));
    _uniformBuffer.registerBind(LIGHTS_BUFFER_IDX, offsetof(UniformBlockStruct, ld),
                                sizeof(LightData) * NumLights);
}

void Renderer::render(const Scene& scene, const Camera& camera) {
    _uniformBuffer.wait();
    _uniformBuffer.rebind();

    uploadUniformBuffer(scene, camera);

    drawShapes(scene);

    if (_drawSkybox)
        drawSkybox(scene);

    _uniformBuffer.lock();
    _uniformBuffer.swap();
}
