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

    void processKeyPress(unsigned char key, int x, int y) override;
    void processMouseClick(int button, int state, int x, int y) override;
    void reshape(int w, int h) override;
    void tickPerSecond() override;

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

    Color _f0;
    Color _diffuse;

    sref<Camera> _camera;
    PBRMaterial* _selMat = nullptr;
    Shape* _selectedShape = nullptr;

    float _gamma;
    float _exposure;
    float _toneParams[7];
    float _metallic;
    float _roughness;

    int _fps = 0;

    int _skybox = 1;
    bool _showGUI = true;
    bool _showSky = true;
    bool _perturbNormals = true;
    bool _envLighting = true;
    float _envIntensity = 1.0f;

    float _accum = 0.0f;
};

} // namespace pbr

#endif