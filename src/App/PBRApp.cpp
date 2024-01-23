#include <PBRApp.h>

#include <GLFW/glfw3.h>
#include <PBRMath.h>

#include <Resources.h>
#include <RenderInterface.h>

#include <Shape.h>
#include <Texture.h>
#include <Skybox.h>

#include <Camera.h>
#include <Perspective.h>

#include <Transform.h>

#include <PBRMaterial.h>

#include <GUI.h>
#include <imgui.h>

#include <Utils.h>
#include <Ray.h>

#include <SceneLoader.h>

#include <chrono>
#include <format>

#include <TubeLight.h>
#include <SphereLight.h>
#include <DirectionalLight.h>
#include <SpotLight.h>

using namespace pbr;
using namespace pbr::util;

namespace {

const std::unordered_map<LightType, std::string> LightTypeToString{
    {LightType::Directional, "Directional"},
    {LightType::Point,       "Point"      },
    {LightType::Spot,        "Spotlight"  },
    {LightType::Sphere,      "Sphere"     },
    {LightType::Tube,        "Tube"       }
};

}

PBRApp::PBRApp(const std::string& title, const CliOptions& opts)
    : OpenGLApplication(title, opts.width, opts.height, opts.msaaSamples) {
    prepare(opts);
}

void PBRApp::prepare(const CliOptions& opts) {
    Print("Initializing renderer");

    RHI.initialize();
    GuiInit(_width, _height);

    // Initialize renderer
    _renderer.prepare();
    _rendererParams.exposure = _renderer.exposure();
    _rendererParams.gamma = _renderer.gamma();
    _rendererParams.toneParams = _renderer.toneParams();

    Print("Loading scene");

    SceneLoader loader{};
    _scene = std::move(*loader.parse(opts.sceneFile));
    _skyboxes = loader.getSkyboxes();
    for (const auto& sky : _skyboxes)
        _skyboxOpts.append(sky.name() + '\0');

    Print("Finished loading assets");

    if (_scene.cameras().size() > 0)
        _camera = _scene.cameras().back().get();

    if (_skyboxes.size() > 0)
        changeSkybox(_skybox);

    const auto& lights = _scene.lights();
    if (lights.size() > 0)
        changeLight(lights[0].get());

    reshape(_width, _height);
}

void PBRApp::reshape(int w, int h) {
    OpenGLApplication::reshape(w, h);
    _camera->updateDimensions(w, h);
    GuiResize(w, h);
}

void PBRApp::tickPerSecond() {
    _fps = _frameCount;
    _frameCount = 0;
}

void PBRApp::renderScene() {
    _scene.sortShapes(_camera->position());
    _renderer.render(_scene, *_camera);

    if (_showGUI)
        drawInterface();
}

void PBRApp::restoreToneDefaults() {
    _rendererParams.gamma = 2.2f;
    _rendererParams.exposure = 3.0f;
    _rendererParams.toneParams[0] = 0.15f;
    _rendererParams.toneParams[1] = 0.5f;
    _rendererParams.toneParams[2] = 0.1f;
    _rendererParams.toneParams[3] = 0.2f;
    _rendererParams.toneParams[4] = 0.02f;
    _rendererParams.toneParams[5] = 0.3f;
    _rendererParams.toneParams[6] = 11.2f;
}

void PBRApp::update(float dt) {
    if (isMousePressed(MouseButton::Right)) {
        _camera->updateOrientation(_mouseDy * dt * 0.55f, _mouseDx * dt * 0.55f);
        _camera->updateViewMatrix();
    }

    Vector3 moveDir{0};
    if (isKeyPressed('W')) {
        moveDir += -_camera->front();
    } else if (isKeyPressed('S')) {
        moveDir += _camera->front();
    }

    if (isKeyPressed('D')) {
        moveDir += _camera->right();
    } else if (isKeyPressed('A')) {
        moveDir += -_camera->right();
    }

    if (!moveDir.isZero()) {
        _camera->setPosition(_camera->position() + Normalize(moveDir) * dt * 6.0f);
        _camera->updateViewMatrix();
    }

    _renderer.setExposure(_rendererParams.exposure);
    _renderer.setGamma(_rendererParams.gamma);
    _renderer.setToneParams(_rendererParams.toneParams);
    _renderer.setSkyboxDraw(_showSky);
    _renderer.setEnvIntensity(_envIntensity);
}

void PBRApp::cleanup() {}

void PBRApp::processKeys(int key, int scancode, int action, int mods) {
    OpenGLApplication::processKeys(key, scancode, action, mods);

    if (checkKey('H', KeyState::Pressed))
        _showGUI = !_showGUI;
    else if (checkKey('P', KeyState::Pressed))
        takeSnapshot();
}

void PBRApp::updateMaterial(Material* mat) {
    _selMat = reinterpret_cast<PBRMaterial*>(mat);

    _matParams.diffuse = _selMat->diffuse();
    _matParams.metallic = _selMat->metallic();
    _matParams.roughness = _selMat->roughness();
    _matParams.f0 = _selMat->reflectivity();
    _matParams.clearCoat = _selMat->clearCoat();
    _matParams.clearCoatRough = _selMat->clearCoatRough();
}

void PBRApp::pickObject(int x, int y) {
    Ray ray = _camera->traceRay(Vec2(x, y));
    if (auto shape = _scene.intersect(ray))
        updateMaterial(shape.value()->material().get());
}

void PBRApp::processMouseClick(int button, int action, int mods) {
    OpenGLApplication::processMouseClick(button, action, mods);

    if (isMousePressed(MouseButton::Middle))
        pickObject(_mouse.x, _mouse.y);
}

void PBRApp::changeToneMap(ToneMap toneMap) {
    _renderer.setToneMap(toneMap);
}

void PBRApp::changeLight(Light* light) {
    _selLight = light;

    _lightParams.on = light->isOn();
    _lightParams.emission = light->emission();
    _lightParams.intensity = light->intensity();
    _lightParams.position = light->position();

    switch (light->type()) {
    case LightType::Tube:
        _lightParams.radius = static_cast<TubeLight*>(_selLight)->radius();
        break;
    case LightType::Sphere:
        _lightParams.radius = static_cast<SphereLight*>(_selLight)->radius();
        break;
    case LightType::Spot:
        {
            SpotLight* spot = static_cast<SpotLight*>(_selLight);
            _lightParams.direction = spot->direction();
            _lightParams.cutOff = spot->cutOff();
            _lightParams.outerCutOff = spot->outerCutOff();
        }
        break;
    case LightType::Directional:
        _lightParams.direction = static_cast<DirectionalLight*>(_selLight)->direction();
        break;
    default:
        break;
    };
}

void PBRApp::renderLightsInterface() {
    ImGui::SetNextWindowPos({497, 10}, ImGuiCond_Once);
    ImGui::SetNextWindowSize({417, 171}, ImGuiCond_Once);
    ImGui::Begin("Lights");

    std::stringstream ss;
    const auto& lights = _scene.lights();
    for (std::size_t l = 0; l < lights.size(); ++l)
        ss << "Light" << l << '\0'; // std::format("Light{}\0", l);
    if (ImGui::Combo("Light", &_lightIdx, ss.str().c_str()))
        changeLight(lights[_lightIdx].get());

    if (_selLight) {
        const auto type = _selLight->type();
        ImGui::Text("Light::%s", LightTypeToString.at(_selLight->type()).c_str());

        if (ImGui::Checkbox("On", &_lightParams.on))
            _selLight->toggle();

        if (ImGui::SliderFloat("Intensity", &_lightParams.intensity, 0.0f, 50.0f))
            _selLight->setIntensity(_lightParams.intensity);

        if (ImGui::ColorEdit3("Emission",
                              reinterpret_cast<float*>(&_lightParams.emission)))
            _selLight->setEmission(_lightParams.emission);

        if (type != LightType::Directional) {
            if (ImGui::DragFloat3(
                    "position", reinterpret_cast<float*>(&_lightParams.position), 0.35f))
                _selLight->setPosition(_lightParams.position);
        }
    }

    ImGui::End();
}

void PBRApp::renderMaterialsInterface() {
    RRID whiteTex = Resource.get<Texture>("white")->id();

    ImGui::Begin("Selected Object");

    if (_selMat->diffuseTex() == whiteTex)
        if (ImGui::ColorEdit3("Diffuse", (float*)&_matParams.diffuse))
            _selMat->setDiffuse(_matParams.diffuse);

    if (ImGui::SliderFloat("Reflectivity", &_matParams.f0, 0.0f, 1.0f))
        _selMat->setReflectivity(_matParams.f0);

    if (_selMat->metallicTex() == whiteTex)
        if (ImGui::SliderFloat("Metallic", &_matParams.metallic, 0.0f, 1.0f))
            _selMat->setMetallic(_matParams.metallic);

    if (_selMat->roughTex() == whiteTex)
        if (ImGui::SliderFloat("Roughness", &_matParams.roughness, 0.0f, 1.0f))
            _selMat->setRoughness(_matParams.roughness);

    if (ImGui::SliderFloat("Clear Coat Roughness", &_matParams.clearCoatRough, 0.0f,
                           1.0f))
        _selMat->setClearCoatRoughness(_matParams.clearCoatRough);

    if (ImGui::SliderFloat("Clear Coat", &_matParams.clearCoat, 0.0f, 1.0f))
        _selMat->setClearCoat(_matParams.clearCoat);

    ImGui::End();
}

void PBRApp::drawInterface() {
    GuiBeginFrame(_mouse.x, _mouse.y, _mouse.buttons);

    ImGui::SetNextWindowPos({10, 10}, ImGuiCond_Once);
    ImGui::SetNextWindowSize({477, 124}, ImGuiCond_Once);
    ImGui::Begin("Environment");
    ImGui::Text("%g fps", _fps);
    ImGui::Checkbox("Draw Skybox", &_showSky);
    ImGui::SliderFloat("Env Intensity", &_envIntensity, 0.0f, 1.0f);

    if (_skyboxes.size() > 0)
        if (ImGui::Combo("Current Environment", &_skybox, _skyboxOpts.c_str()))
            changeSkybox(_skybox);
    ImGui::End();

    ImGui::SetNextWindowPos({10, 144}, ImGuiCond_Once);
    ImGui::SetNextWindowSize({477, 416}, ImGuiCond_Once);

    ImGui::Begin("Tone Map");

    if (ImGui::Combo("Tone Map", reinterpret_cast<int*>(&_rendererParams.toneMap),
                     "Parametric\0ACES\0ACES Boosted\0ACES Fast\0"))
        changeToneMap(_rendererParams.toneMap);

    ImGui::TextWrapped(
        "Tone function parameters to control the shape of the tone curve.");

    ImGui::SliderFloat("gamma", &_rendererParams.gamma, 0.0f, 4.0f);
    ImGui::SliderFloat("exposure", &_rendererParams.exposure, 0.0f, 8.0f);

    ImGui::Separator();

    if (_renderer.toneMap() == ToneMap::Parametric) {
        auto& toneParams = _rendererParams.toneParams;
        ImGui::SliderFloat("A", &toneParams[0], 0.0f, 2.0f);
        ImGui::SliderFloat("B", &toneParams[1], 0.0f, 2.0f);
        ImGui::SliderFloat("C", &toneParams[2], 0.0f, 2.0f);
        ImGui::SliderFloat("D", &toneParams[3], 0.0f, 2.0f);
        ImGui::SliderFloat("E", &toneParams[4], 0.0f, 0.2f);
        ImGui::SliderFloat("J", &toneParams[5], 0.0f, 2.0f);
        ImGui::SliderFloat("W", &toneParams[6], 0.0f, 30.0f);

        ImGui::Separator();

        ImGui::PlotLines(
            "Tone map curve",
            [](void* data, int idx) {
                float* p = (float*)data;
                float v = 0.03f * (float)idx;
                float scale = ((p[6] * (p[0] * p[6] + p[2] * p[1]) + p[3] * p[4]) /
                               (p[6] * (p[0] * p[6] + p[1]) + p[3] * p[5])) -
                              p[4] / p[5];
                return (((v * (p[0] * v + p[2] * p[1]) + p[3] * p[4]) /
                         (v * (p[0] * v + p[1]) + p[3] * p[5])) -
                        p[4] / p[5]) /
                       scale;
            },
            toneParams.data(), 100, 0, NULL, FLOAT_MAXIMUM, FLOAT_MAXIMUM,
            ImVec2{320, 100});

        if (ImGui::Button("Restore defaults"))
            restoreToneDefaults();
    }

    ImGui::End();

    ImGui::SetNextWindowPos({10, 570}, ImGuiCond_Once);
    ImGui::SetNextWindowSize({477, 130}, ImGuiCond_Once);

    ImGui::Begin("Keys and movement");
    ImGui::TextWrapped(
        "Press mouse right button while moving the mouse to orient the camera.");
    ImGui::TextWrapped("Mouse middle button - Pick object.");
    ImGui::TextWrapped("WASD - Camera movement.");
    ImGui::TextWrapped("H - Toggle GUI visibility.");
    ImGui::TextWrapped("P - Take a snapshot.");
    ImGui::End();

    // Selected object window
    if (_selMat != nullptr)
        renderMaterialsInterface();

    // Lights window
    if (_scene.lights().size() > 0)
        renderLightsInterface();

    ImGui::Render();
}

void PBRApp::changeSkybox(int id) {
    _skyboxes[id].set();
    _scene.setEnvironment(_skyboxes[id]);
}

void PBRApp::takeSnapshot() {
    using namespace std::chrono;
    auto now = system_clock::now();
    auto timestamp = duration_cast<seconds>(now.time_since_epoch()).count();

    auto img = ReadMainFramebuffer(0, 0, _width, _height);
    img.flipY();
    SaveImage(std::format("snapshot_{}.png", timestamp), img);
}