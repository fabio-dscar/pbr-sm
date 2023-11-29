#include "PBRMath.h"
#include <PBRApp.h>

#include <chrono>
#include <format>

#include <Resources.h>
#include <RenderInterface.h>

#include <Shape.h>
#include <Sphere.h>
#include <Mesh.h>
#include <Texture.h>
#include <Skybox.h>

#include <Camera.h>
#include <Perspective.h>

#include <Transform.h>

#include <PBRMaterial.h>

#include <GUI.h>

#include <Utils.h>
#include <Ray.h>

#include <PointLight.h>
#include <DirectionalLight.h>
#include <SpotLight.h>
#include <SphereLight.h>
#include <TubeLight.h>
#include <Quad.h>

using namespace pbr;

PBRApp::PBRApp(const std::string& title, int width, int height)
    : OpenGLApplication(title, width, height) {}

void initializeEngine() {
    // Initialize resource manager
    Resource.initialize();

    // Initialize render hardware interface
    RHI.initialize();
}

void PBRApp::initSkyboxes() {
    std::vector<std::string> folders = {"Pinetree", "Ruins", "WalkOfFame",
                                        "WinterForest"};
    for (const std::string& str : folders)
        _skyboxes.emplace_back("PBR/" + str);

    for (Skybox& sky : _skyboxes)
        sky.initialize();
}

void PBRApp::prepare() {
    std::cout << "\n[INFO] Initializing renderer...\n";

    initializeEngine();

    ImGui_Init(_width, _height);

    // Initialize renderer
    _renderer.prepare();
    _exposure = _renderer.exposure();
    _gamma = _renderer.gamma();
    std::memcpy(_toneParams, _renderer.toneParams(), sizeof(float) * 7);

    std::cout << "[INFO] Loading cubemaps..." << std::endl;

    initSkyboxes();

    std::cout << "[INFO] Loading meshes and materials..." << std::endl;

    _camera =
        std::make_shared<Perspective>(_width, _height, Vec3(-3, 3, -3), Vec3(0, 0, 0),
                                      Vec3(0, 1, 0), 0.1f, 500.0f, 60.0f);
    _scene.addCamera(_camera);

    /*sref<Shape> obj = Utils::loadSceneObject("sphere");
    obj->setPosition(Vec3(-20.0f, 0.0f, 0.0f));
    obj->_prog = -1;
    obj->updateMatrix();
    _scene.addShape(obj);*/

    sref<Shape> gun = Utils::loadSceneObject("gun");
    gun->setScale(5.5f, 5.5f, 5.5f);
    _scene.addShape(gun);

    sref<Shape> prev = Utils::loadSceneObject("helmet");
    prev->setScale(1.0f, 1.0f, 1.0f);
    prev->setPosition(Vec3(20.0f, 0.0f, 0.0f));
    _scene.addShape(prev);

    sref<Shape> spec = Utils::loadSceneObject("specular");
    spec->setScale(1.0f, 1.0f, 1.0f);
    spec->setPosition(Vec3(-10.0f, 0.0f, 0.0f));
    _scene.addShape(spec);

    sref<Shape> rough = Utils::loadSceneObject("dmg_helmet");
    rough->setScale(1.0f, 1.0f, 1.0f);
    rough->setPosition(Vec3(10.0f, 0.0f, 0.0f));
    _scene.addShape(rough);

    /*sref<Shape> obj = std::make_shared<Sphere>(Vec3(-20.0f, 0.0f, 0.0f), 1.0f);
    sref<PBRMaterial> m = std::make_shared<PBRMaterial>(Color(1.0, 0.0, 0.0), 0.0f,
    0.20f); m->setNormal(static_cast<PBRMaterial*>(rough->material().get())->normalTex());
    obj->setMaterial(m);
    obj->prepare();
    obj->setPosition(Vec3(-20.0f, 0.0f, 0.0f));
    obj->updateMatrix();
    obj->_prog = -1;
    _scene.addShape(obj);*/

    sref<Shape> obj = std::make_shared<Quad>();
    sref<PBRMaterial> m =
        std::make_shared<PBRMaterial>(Color(1.0, 0.0, 0.0), 0.0f, 0.20f);
    // m->setNormal(static_cast<PBRMaterial*>(rough->material().get())->normalTex());
    obj->setMaterial(m);
    m->prepare();
    obj->prepare();
    obj->setScale(45.0f, 1.0f, 45.0f);
    obj->setPosition({0.0f, -2.0f, 0.0f});
    _scene.addShape(obj);

    /*sref<Light> light = std::make_shared<PointLight>(Color(1.0f), 200.0f);
    light->setPosition(Vec3(-7.0f, 6.0f, 0.0f));
    _scene.addLight(light);

    sref<Light> light2 = std::make_shared<PointLight>(Color(1.0f, 0.0f, 1.0f), 200.0f);
    light2->setPosition(Vec3(7.0f, 6.0f, 0.0f));
    _scene.addLight(light2);

    sref<Light> light3 = std::make_shared<PointLight>(Color(1.0f, 1.0f, 0.0f), 200.0f);
    light3->setPosition(Vec3(-7.0f, -6.0f, 0.0f));
    _scene.addLight(light3);

    sref<Light> light4 = std::make_shared<PointLight>(Color(0.6f, 0.1f, 0.21f), 200.0f);
    light4->setPosition(Vec3(18.0f, 6.0f, 0.0f));
    _scene.addLight(light4);

    sref<Light> light5 = std::make_shared<PointLight>(Color(0.3f, 0.1f, 0.8f), 200.0f);
    light5->setPosition(Vec3(24.0f, 6.0f, 0.0f));
    _scene.addLight(light5);*/

    /*sref<Light> light5 =
        std::make_shared<SphereLight>(Color(1.0f, 1.0f, 1.0f), 2.0f, 3.0f);
    light5->setPosition({0.0f, 4.5f, 0.0f});
    // light5->setOrientation(Quat(rotationX(radians(45))));
    _scene.addLight(light5);*/

    sref<Light> light5 = std::make_shared<TubeLight>(Color(1.0f, 1.0f, 1.0f), 9.5f, 0.5f);
    light5->setPosition({-10.0f, 4.5f, 0.0f});
    light5->setScale(7, 0, 0);
    light5->updateMatrix();
    // light5->setOrientation(Quat(rotationX(radians(45))));
    _scene.addLight(light5);

    /*sref<Light> light6 = std::make_shared<PointLight>(Color(1.0f, 1.0f, 1.0f), 1.6f);
    light6->setPosition({0.0f, 4.5f, 0.0f});
    // light5->setOrientation(Quat(rotationX(radians(45))));
    _scene.addLight(light6);*/

    /*sref<Light> spot = std::make_shared<SpotLight>(Color(1.0f), 8.0f);
    spot->setPosition({0.0f, 4.0f, 0.0f});
    _scene.addLight(spot);*/

    std::cout << "[INFO] Assets finished loading..." << std::endl;

    changeSkybox(_skybox);
}

void PBRApp::reshape(int w, int h) {
    OpenGLApplication::reshape(w, h);
    _camera->updateDimensions(w, h);
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
    if (_mouseBtns[MouseButton::RIGHT]) {
        _camera->updateOrientation(_mouseDy * dt * 0.55f, _mouseDx * dt * 0.55f);
        _camera->updateViewMatrix();
    }

    Vector3 moveDir(0);
    if (_keys[static_cast<int>('W')]) {
        moveDir += -_camera->front();
    } else if (_keys[static_cast<int>('S')]) {
        moveDir += _camera->front();
    }

    if (_keys[static_cast<int>('D')]) {
        moveDir += _camera->right();
    } else if (_keys[static_cast<int>('A')]) {
        moveDir += -_camera->right();
    }

    if (moveDir != Vector3(0)) {
        _camera->setPosition(_camera->position() + normalize(moveDir) * dt * 6.0f);
        _camera->updateViewMatrix();
    }

    _renderer.setExposure(_exposure);
    _renderer.setGamma(_gamma);
    _renderer.setToneParams(_toneParams);
    _renderer.setSkyboxDraw(_showSky);
    _renderer.setPerturbNormals(_perturbNormals);
    _renderer.setEnvLighting(_envLighting);
    _renderer.setEnvIntensity(_envIntensity);

    _accum += dt;
    sref<Light> l = *_scene.lights().begin();
    l->setOrientation(Quat(rotationY(2 * PI * _accum * 0.5f)));
    l->setPosition({std::cos(PI * _accum * 0.35f) * 20,
                    /*1.0f + std::sin(PI * _accum * 1.2f) * 2.0f, */
                    3.5f, 3.0f});
    l->updateMatrix();
}

void PBRApp::cleanup() {}

void PBRApp::processKeys(int key, int scancode, int action, int mods) {
    OpenGLApplication::processKeys(key, scancode, action, mods);

    if (key == 'H' && _keys['H'])
        _showGUI = !_showGUI;

    if (key == 'P' && _keys['P'])
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
        _f0 = pbrMat->specular();
    }
}

void PBRApp::processMouseClick(int button, int action, int mods) {
    OpenGLApplication::processMouseClick(button, action, mods);

    if (_mouseBtns[MouseButton::MIDDLE])
        pickObject(_mouseX, _mouseY);
}

void PBRApp::drawInterface() {
    ImGui_NewFrame(_mouseX, _mouseY, _mouseBtns);

    ImGui::Begin("Environment");
    ImGui::Text("%d fps", _fps);
    ImGui::Checkbox("Draw Skybox", &_showSky);
    ImGui::Checkbox("Perturb Normals", &_perturbNormals);
    ImGui::Checkbox("Env Lighting", &_envLighting);
    ImGui::SliderFloat("Env Intensity", &_envIntensity, 0.0f, 1.0f);
    if (ImGui::Combo("Current Environment", &_skybox,
                     "Pinetree\0Ruins\0Walk of Fame\0Winter Forest\0"))
        changeSkybox(_skybox);
    ImGui::End();

    ImGui::Begin("Tone Map");

    ImGui::TextWrapped(
        "Tone function parameters to control the shape of the tone curve.");

    ImGui::SliderFloat("gamma", &_gamma, 0.0f, 4.0f);
    ImGui::SliderFloat("exposure", &_exposure, 0.0f, 8.0f);

    ImGui::Separator();

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
        _toneParams, 100, 0, NULL, FLOAT_MAXIMUM, FLOAT_MAXIMUM, ImVec2(320, 120));

    if (ImGui::Button("Restore defaults"))
        restoreToneDefaults();

    ImGui::End();

    // Selected object window
    if (_selectedShape != nullptr) {
        RRID whiteTex = Resource.getTexture("white")->rrid();

        ImGui::Begin("Selected Object");

        if (_selMat->diffuseTex() == whiteTex)
            if (ImGui::ColorEdit3("Diffuse", (float*)&_diffuse))
                _selMat->setDiffuse(_diffuse);

        if (ImGui::ColorEdit3("Specular", (float*)&_f0))
            _selMat->setSpecular(_f0);

        if (_selMat->metallicTex() == whiteTex)
            if (ImGui::SliderFloat("Metallic", &_metallic, 0.0f, 1.0f))
                _selMat->setMetallic(_metallic);

        if (_selMat->roughTex() == whiteTex)
            if (ImGui::SliderFloat("Roughness", &_roughness, 0.0f, 1.0f))
                _selMat->setRoughness(_roughness);

        ImGui::End();
    }

    ImGui::Begin("Information");
    ImGui::TextWrapped(
        "Press right click and move the mouse to orient the camera. WASD for movement.");
    ImGui::TextWrapped("Press 'H' to toggle GUI visibility.");
    ImGui::TextWrapped("Press 'P' to take a snapshot. Do not forget to hide the GUI by "
                       "pressing 'H' first, if desired.");
    ImGui::TextWrapped("By clicking the middle mouse button, it is possible to pick "
                       "objects and change some of their parameters.");
    ImGui::End();

    ImGui::Render();
}

void PBRApp::changeSkybox(int id) {
    _scene.setEnvironment(_skyboxes[id]);
}

void PBRApp::takeSnapshot() {
    using namespace std::chrono;
    auto now = system_clock::now();
    auto timestamp = duration_cast<seconds>(now.time_since_epoch()).count();

    auto img = RHI.getImage(0, 0, _width, _height);
    img->flipY();
    img->saveImage(std::format("snapshot_{}.png", timestamp));
}