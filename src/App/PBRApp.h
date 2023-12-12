#ifndef __PBR_PBRAPP_H__
#define __PBR_PBRAPP_H__

#include <OpenGLApplication.h>

#include <Scene.h>
#include <Renderer.h>
#include <Skybox.h>
#include <Spectrum.h>

namespace pbr {

class PBRMaterial;
class Skybox;

class PBRApp : public OpenGLApplication {
public:
    PBRApp(const std::string& title, int width, int height);

    void prepare() override;
    void drawScene() override;
    void update(float dt) override;
    void cleanup() override;

    void tickPerSecond() override;

    void processMouseClick(int button, int action, int mods) override;
    void processKeys(int key, int scancode, int action, int mods) override;
    void reshape(int w, int h) override;

private:
    void drawInterface();
    void restoreToneDefaults();
    void changeSkybox(int id);
    void takeSnapshot();
    void pickObject(int x, int y);
    void initSkyboxes();

    Scene _scene;
    Renderer _renderer;
    std::vector<Skybox> _skyboxes;

    Color _diffuse;

    sref<Camera> _camera;
    PBRMaterial* _selMat = nullptr;
    Shape* _selectedShape = nullptr;

    float _gamma;
    float _exposure;
    float _toneParams[7];
    float _metallic;
    float _roughness;
    float _f0;
    float _clearCoat;
    float _clearCoatRough;

    int _fps = 0;

    int _skybox = 0;
    bool _showGUI = true;
    bool _showSky = true;
    bool _perturbNormals = true;
    bool _envLighting = true;
    float _envIntensity = 1.0f;

    float _accum = 0.0f;
};

} // namespace pbr

#endif