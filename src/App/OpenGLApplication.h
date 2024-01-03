#ifndef __PBR_OGLAPP_H__
#define __PBR_OGLAPP_H__

#include <PBR.h>

struct GLFWwindow;

namespace pbr {

enum MouseButton { LEFT = 0, RIGHT = 1, MIDDLE = 2 };

class OpenGLApplication {
public:
    OpenGLApplication(const std::string& title, int width, int height);
    virtual ~OpenGLApplication() = default;

    void init();
    void loop();

    void setTitle(const std::string& title);

    virtual void cleanup() = 0;
    virtual void prepare() = 0;
    virtual void drawScene() = 0;
    virtual void tickPerSecond() = 0;
    virtual void update(float dt) = 0;

    virtual void processKeys(int key, int scancode, int action, int mods);
    virtual void reshape(int w, int h);
    virtual void processMouseClick(int button, int action, int mods);
    virtual void processMouseMotion(double x, double y);

    bool isMousePressed(MouseButton button) const {
        return _mouseBtns[button];
    }

    bool isKeyPressed(int key) const {
        return _keys[key];
    }

protected:
    std::string _title;

    int _frameCount = 0;

    int _width;
    int _height;

    double _mouseX;
    double _mouseY;

    double _clickX;
    double _clickY;

    double _mouseDx;
    double _mouseDy;

    bool _keys[255];
    bool _mouseBtns[3];

private:
    void render();
    void updateMouse(double x, double y);
    void setCallbacks();

    double _oldTimeSinceStart = 0;
    double _secondCount = 0;

    GLFWwindow* _window;
};

} // namespace pbr

#endif