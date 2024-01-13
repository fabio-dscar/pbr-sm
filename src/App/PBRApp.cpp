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

#include <Utils.h>
#include <Ray.h>

#include <SceneLoader.h>

#include <chrono>
#include <format>

using namespace pbr;
using namespace pbr::util;

PBRApp::PBRApp(const std::string& title, int width, int height)
    : OpenGLApplication(title, width, height) {}

void PBRApp::prepare() {
    Print("Initializing renderer");

    RHI.initialize();

    GuiInit(_width, _height);

    // Initialize renderer
    _renderer.prepare();
    _exposure = _renderer.exposure();
    _gamma = _renderer.gamma();
    _toneParams = _renderer.toneParams();

    Print("Loading scene");

    SceneLoader loader{};
    _scene = std::move(*loader.parse("test.xml"));
    _skyboxes = loader.getSkyboxes();
    _camera = _scene.cameras()[0];
    reshape(_width, _height);

    for (const auto& sky : _skyboxes)
        _skyboxOpts.append(sky.name() + '\0');

    Print("Finished loading assets");

    changeSkybox(_skybox);
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

void PBRApp::drawScene() {
    _scene.sortShapes(_camera->position());
    _renderer.render(_scene, *_camera);

    if (_showGUI)
        drawInterface();
}

void PBRApp::restoreToneDefaults() {
    _gamma = 2.2f;
    _exposure = 3.0f;
    _toneParams[0] = 0.15f;
    _toneParams[1] = 0.5f;
    _toneParams[2] = 0.1f;
    _toneParams[3] = 0.2f;
    _toneParams[4] = 0.02f;
    _toneParams[5] = 0.3f;
    _toneParams[6] = 11.2f;
}

void PBRApp::update(float dt) {
    if (isMousePressed(MouseButton::RIGHT)) {
        _camera->updateOrientation(_mouseDy * dt * 0.55f, _mouseDx * dt * 0.55f);
        _camera->updateViewMatrix();
    }

    Vector3 moveDir(0);
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
        _camera->setPosition(_camera->position() + normalize(moveDir) * dt * 6.0f);
        _camera->updateViewMatrix();
    }

    _renderer.setExposure(_exposure);
    _renderer.setGamma(_gamma);
    _renderer.setToneParams(_toneParams);
    _renderer.setSkyboxDraw(_showSky);
    _renderer.setEnvIntensity(_envIntensity);

    _accum += dt;
}

void PBRApp::cleanup() {}

void PBRApp::processKeys(int key, int scancode, int action, int mods) {
    OpenGLApplication::processKeys(key, scancode, action, mods);

    if (key == 'H' && action == GLFW_PRESS)
        _showGUI = !_showGUI;
    else if (key == 'P' && action == GLFW_PRESS)
        takeSnapshot();
}

void PBRApp::pickObject(int x, int y) {
    Ray ray = _camera->traceRay(Vec2(x, y));

    if (auto shape = _scene.intersect(ray)) {
        _selectedShape = shape.value();
        PBRMaterial* pbrMat = (PBRMaterial*)_selectedShape->material().get();

        _selMat = pbrMat;

        _diffuse = pbrMat->diffuse();
        _metallic = pbrMat->metallic();
        _roughness = pbrMat->roughness();
        _f0 = pbrMat->reflectivity();
    }
}

void PBRApp::processMouseClick(int button, int action, int mods) {
    OpenGLApplication::processMouseClick(button, action, mods);

    if (isMousePressed(MouseButton::MIDDLE))
        pickObject(_mouseX, _mouseY);
}

void PBRApp::changeToneMap(ToneMap toneMap) {
    _renderer.setToneMap(toneMap);
}

void PBRApp::drawInterface() {
    GuiBeginFrame(_mouseX, _mouseY, _mouseBtns);

    ImGui::SetNextWindowPos({10, 10}, ImGuiCond_Once);
    ImGui::SetNextWindowSize({477, 142}, ImGuiCond_Once);
    ImGui::Begin("Environment");
    ImGui::Text("%d fps", _fps);
    ImGui::Text("%f, %f, %f", _camera->position().x, _camera->position().y,
                _camera->position().z);
    ImGui::Checkbox("Draw Skybox", &_showSky);
    ImGui::SliderFloat("Env Intensity", &_envIntensity, 0.0f, 1.0f);

    if (ImGui::Combo("Current Environment", &_skybox, _skyboxOpts.c_str()))
        changeSkybox(_skybox);
    ImGui::End();

    ImGui::SetNextWindowPos({10, 162}, ImGuiCond_Once);
    ImGui::SetNextWindowSize({477, 436}, ImGuiCond_Once);

    ImGui::Begin("Tone Map");

    if (ImGui::Combo("Tone Map", reinterpret_cast<int*>(&_toneMap),
                     "Parametric\0ACES\0ACES Boosted\0ACES Fast\0"))
        changeToneMap(_toneMap);

    ImGui::TextWrapped(
        "Tone function parameters to control the shape of the tone curve.");

    ImGui::SliderFloat("gamma", &_gamma, 0.0f, 4.0f);
    ImGui::SliderFloat("exposure", &_exposure, 0.0f, 8.0f);

    ImGui::Separator();

    if (_renderer.toneMap() == ToneMap::Parametric) {
        ImGui::SliderFloat("A", &_toneParams[0], 0.0f, 2.0f);
        ImGui::SliderFloat("B", &_toneParams[1], 0.0f, 2.0f);
        ImGui::SliderFloat("C", &_toneParams[2], 0.0f, 2.0f);
        ImGui::SliderFloat("D", &_toneParams[3], 0.0f, 2.0f);
        ImGui::SliderFloat("E", &_toneParams[4], 0.0f, 0.2f);
        ImGui::SliderFloat("J", &_toneParams[5], 0.0f, 2.0f);
        ImGui::SliderFloat("W", &_toneParams[6], 0.0f, 30.0f);

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
            _toneParams.data(), 100, 0, NULL, FLOAT_MAXIMUM, FLOAT_MAXIMUM,
            ImVec2(320, 120));

        if (ImGui::Button("Restore defaults"))
            restoreToneDefaults();
    }

    ImGui::End();

    // Selected object window
    if (_selectedShape != nullptr) {
        RRID whiteTex = Resource.get<Texture>("white")->id();

        ImGui::Begin("Selected Object");

        if (_selMat->diffuseTex() == whiteTex)
            if (ImGui::ColorEdit3("Diffuse", (float*)&_diffuse))
                _selMat->setDiffuse(_diffuse);

        if (ImGui::SliderFloat("Reflectivity", &_f0, 0.0f, 1.0f))
            _selMat->setReflectivity(_f0);

        if (_selMat->metallicTex() == whiteTex)
            if (ImGui::SliderFloat("Metallic", &_metallic, 0.0f, 1.0f))
                _selMat->setMetallic(_metallic);

        if (_selMat->roughTex() == whiteTex)
            if (ImGui::SliderFloat("Roughness", &_roughness, 0.0f, 1.0f))
                _selMat->setRoughness(_roughness);

        if (ImGui::SliderFloat("Clear Coat Roughness", &_clearCoatRough, 0.0f, 1.0f))
            _selMat->setClearCoatRoughness(_clearCoatRough);

        if (ImGui::SliderFloat("Clear Coat", &_clearCoat, 0.0f, 1.0f))
            _selMat->setClearCoat(_clearCoat);

        ImGui::End();
    }

    ImGui::Render();
}

void PBRApp::changeSkybox(int id) {
    if (_skyboxes.size() > 0) {
        _skyboxes[id].set();
        _scene.setEnvironment(_skyboxes[id]);
    }
}

void PBRApp::takeSnapshot() {
    using namespace std::chrono;
    auto now = system_clock::now();
    auto timestamp = duration_cast<seconds>(now.time_since_epoch()).count();

    auto img = ReadMainFramebuffer(0, 0, _width, _height);
    img.flipY();
    SaveImage(std::format("snapshot_{}.png", timestamp), img);
}