#ifndef PBR_OGLAPP_H
#define PBR_OGLAPP_H

#include <PBR.h>

struct GLFWwindow;

namespace pbr {

enum class MouseButton : int { Left = 0, Right = 1, Middle = 2 };
enum class KeyState : int { Released = 0, Pressed = 1, Repeat = 2 };
consteval bool EnumHasConversion(MouseButton);
consteval bool EnumHasConversion(KeyState);

struct MouseState {
    double x = 0.0;
    double y = 0.0;
    std::array<KeyState, 3> buttons = {KeyState::Released};
};

constexpr int MaxKeyNum = 512;
struct KeyboardState {
    std::array<KeyState, MaxKeyNum> keys = {KeyState::Released};
};

class OpenGLApplication {
public:
    OpenGLApplication(const std::string& title, int width, int height, int msaaSamples);
    virtual ~OpenGLApplication() = default;

    void setTitle(const std::string& title);

    virtual void cleanup() = 0;
    virtual void renderScene() = 0;
    virtual void tickPerSecond() = 0;
    virtual void update(float dt) = 0;

    virtual void processKeys(int key, int scancode, int action, int mods);
    virtual void reshape(int w, int h);
    virtual void processMouseClick(int button, int action, int mods);
    virtual void processMouseMotion(double x, double y);

    bool isMousePressed(MouseButton button) const;
    bool isKeyPressed(int key) const;
    bool checkKey(int key, KeyState state) const;

    void loop();

protected:
    std::string _title;

    int _frameCount = 0;

    int _width;
    int _height;
    int _msaaSamples;

    double _mouseDx;
    double _mouseDy;

    MouseState _mouse;
    KeyboardState _keyboard;

private:
    void init();
    void updateTime();
    void render();
    void updateMouse(double x, double y);
    void setCallbacks();

    double _time = 0;
    double _deltaTime = 0;
    double _secondsTimer = 0;

    GLFWwindow* _window;
};

} // namespace pbr

#endif