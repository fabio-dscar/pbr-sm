#ifndef __PBR_PBRAPP_H__
#define __PBR_PBRAPP_H__

#include <OpenGLApplication.h>

#include <CliParser.h>
#include <Scene.h>
#include <Renderer.h>
#include <Skybox.h>
#include <Spectrum.h>

namespace pbr {

class PBRMaterial;
class Skybox;

class PBRApp : public OpenGLApplication {
public:
    PBRApp(const std::string& title, const CliOptions& opts);
    ~PBRApp() { cleanup(); }

    void renderScene() override;
    void update(float dt) override;
    void cleanup() override;

    void tickPerSecond() override;
    void processMouseClick(int button, int action, int mods) override;
    void processKeys(int key, int scancode, int action, int mods) override;
    void reshape(int w, int h) override;

private:
    void prepare(const CliOptions& opts);
    void drawInterface();
    void restoreToneDefaults();
    void changeSkybox(int id);
    void takeSnapshot();
    void pickObject(int x, int y);
    void updateMaterial(Material* mat);
    void changeToneMap(ToneMap toneMap);

    struct MaterialGuiParams {
        Color diffuse;
        float metallic;
        float roughness;
        float f0;
        float clearCoat;
        float clearCoatRough;
    };

    struct RendererGuiParams {
        float gamma;
        float exposure;
        ToneMap toneMap;
        std::array<float, 7> toneParams;
    };

    Scene _scene;
    Renderer _renderer;

    sref<Camera> _camera = nullptr;
    PBRMaterial* _selMat = nullptr;
    MaterialGuiParams _guiMat;
    RendererGuiParams _rendererParams;

    std::vector<Skybox> _skyboxes;
    std::string _skyboxOpts;
    int _skybox = 0;

    double _fps = 0;
    bool _showGUI = true;
    bool _showSky = true;
    float _envIntensity = 1.0f;
};

} // namespace pbr

#endif