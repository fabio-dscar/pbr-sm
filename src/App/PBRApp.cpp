#include <PBRApp.h>

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

using namespace pbr;

void initializeEngine() {
    // Initialize resource manager
    Resource.initialize();

    // Initialize render hardware interface
    RHI.initialize();
}

PBRApp::PBRApp(const std::string& title, int width, int height)
    : OpenGLApplication(title, width, height), _skyToggle(true), _selectedShape(nullptr),
      _showGUI(true), _skybox(1), _f0(0.04f) {}

void PBRApp::reshape(int w, int h) {
    OpenGLApplication::reshape(w, h);
    _camera->updateDimensions(w, h);
}

void PBRApp::prepare() {
    std::cout << std::endl;
    std::cout << "[INFO] Initializing renderer..." << std::endl;

    initializeEngine();

    ImGui_Init(_width, _height);

    // Initialize renderer
    _renderer.prepare();
    _exposure = _renderer.exposure();
    _gamma = _renderer.gamma();
    memcpy(_toneParams, _renderer.toneParams(), sizeof(float) * 7);

    // Load cubemaps
    std::cout << "[INFO] Loading cubemaps..." << std::endl;

    vec<std::string> folders = {"Pinetree", "Ruins", "WalkOfFame", "WinterForest"};
    for (const std::string& str : folders)
        _skyboxes.emplace_back("PBR/" + str);

    for (Skybox& sky : _skyboxes)
        sky.initialize();

    // Create camera and add it to the scene
    _camera =
        std::make_shared<Perspective>(_width, _height, Vec3(-3, 3, -3), Vec3(0, 0, 0),
                                      Vec3(0, 1, 0), 0.1f, 500.0f, 60.0f);
    _scene.addCamera(_camera);

    // Load meshes
    std::cout << "[INFO] Loading meshes and materials..." << std::endl;

    sref<Shape> obj = Utils::loadSceneObject("sphere");
    obj->setPosition(Vec3(-20.0f, 0.0f, 0.0f));
    obj->_prog = -1;
    obj->updateMatrix();
    _scene.addShape(obj);

    sref<Shape> gun = Utils::loadSceneObject("gun");
    gun->setScale(5.5f, 5.5f, 5.5f);
    gun->updateMatrix();
    gun->_prog = -1;
    _scene.addShape(gun);

    sref<Shape> prev = Utils::loadSceneObject("preview");
    prev->setScale(1.0f, 1.0f, 1.0f);
    prev->setPosition(Vec3(20.0f, 0.0f, 0.0f));
    prev->updateMatrix();
    prev->_prog = -1;
    _scene.addShape(prev);

    sref<Shape> spec = Utils::loadSceneObject("specular");
    spec->setScale(1.0f, 1.0f, 1.0f);
    spec->setPosition(Vec3(-10.0f, 0.0f, 0.0f));
    spec->updateMatrix();
    spec->_prog = -1;
    _scene.addShape(spec);

    sref<Shape> rough = Utils::loadSceneObject("rough");
    rough->setScale(1.0f, 1.0f, 1.0f);
    rough->setPosition(Vec3(10.0f, 0.0f, 0.0f));
    rough->updateMatrix();
    rough->_prog = -1;
    _scene.addShape(rough);

    changeSkybox(_skybox);

    std::cout << "[INFO] Assets finished loading..." << std::endl;
}

void PBRApp::drawScene() {
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
    if (_mouseBtns[2]) {
        _camera->updateOrientation(_mouseDy * dt * 0.75f, _mouseDx * dt * 0.75f);
        _camera->updateViewMatrix();
    }

    Vector3 moveDir(0);
    if (_keys['w']) {
        moveDir += -_camera->front();
    } else if (_keys['s']) {
        moveDir += _camera->front();
    }

    if (_keys['d']) {
        moveDir += _camera->right();
    } else if (_keys['a']) {
        moveDir += -_camera->right();
    }

    if (moveDir != Vector3(0)) {
        _camera->setPosition(_camera->position() + normalize(moveDir) * dt * 7.0f);
        _camera->updateViewMatrix();
    }

    // Update renderer parameters
    _renderer.setExposure(_exposure);
    _renderer.setGamma(_gamma);
    _renderer.setToneParams(_toneParams);
    _renderer.setSkyboxDraw(_skyToggle);
    _renderer.setPerturbNormals(_perturbNormals);
}

void PBRApp::cleanup() {}

void PBRApp::processKeyPress(unsigned char key, int x, int y) {
    OpenGLApplication::processKeyPress(key, x, y);

    if (key == 'h')
        _showGUI = !_showGUI;

    if (key == 'p')
        takeSnapshot();
}

Ray PBRApp::pixelToRay(int x, int y) {
    Vec2 pixel = Vec2(x, y);

    // Create ray from pixel
    Vec3 rayNDS =
        Vec3((2.0f * pixel.x) / _width - 1.0f, 1.0f - (2.0f * pixel.y) / _height, 1.0f);

    Vec4 rayClip = Vec4(rayNDS.x, rayNDS.y, -1.0, 1.0);

    Vec4 rayEye = inverse(_camera->projMatrix()) * rayClip;
    rayEye = Vec4(rayEye.x, rayEye.y, -1.0, 0.0);

    Vec3 rayWorld = (inverse(_camera->viewMatrix()) * rayEye);
    rayWorld = normalize(rayWorld);

    return {_camera->position(), rayWorld};
}

void PBRApp::processMouseClick(int button, int state, int x, int y) {
    OpenGLApplication::processMouseClick(button, state, x, y);

    // Middle button press
    if (_mouseBtns[1]) {
        Ray ray = pixelToRay(x, y);

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
}

void PBRApp::drawInterface() {
    ImGui_NewFrame(_mouseX, _mouseY, _mouseBtns);

    // Environment window
    ImGui::Begin("Environment");
    ImGui::Checkbox("Draw Skybox", &_skyToggle);
    // ImGui::Checkbox("Perturb Normals", &_perturbNormals);
    if (ImGui::Combo("Current Environment", &_skybox,
                     "Pinetree\0Ruins\0Walk of Fame\0Winter Forest\0"))
        changeSkybox(_skybox);
    ImGui::End();

    // Tone map window
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
        ImGui::Begin("Selected Object");

        if (_selMat->diffuseTex() < 0)
            if (ImGui::ColorEdit3("Diffuse", (float*)&_diffuse))
                _selMat->setDiffuse(_diffuse);

        if (ImGui::ColorEdit3("Specular", (float*)&_f0))
            _selMat->setSpecular(_f0);

        if (_selMat->metallicTex() < 0)
            if (ImGui::SliderFloat("Metallic", &_metallic, 0.0f, 1.0f))
                _selMat->setMetallic(_metallic);

        if (_selMat->roughTex() < 0)
            if (ImGui::SliderFloat("Roughness", &_roughness, 0.0f, 1.0f))
                _selMat->setRoughness(_roughness);

        ImGui::End();
    }

    // Information window
    ImGui::Begin("Information");
    ImGui::TextWrapped(
        "Press right click and move the mouse to orient the camera. WASD for movement.");
    ImGui::TextWrapped("Press 'H' to toggle GUI visibility.");
    ImGui::TextWrapped("Press 'P' to take a snapshot! Do not forget to hide the GUI by "
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
    sref<Image> img = RHI.getImage(0, 0, _width, _height);
    img->flipY();
    img->saveImage("snapshot.png");
}