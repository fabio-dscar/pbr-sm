#ifndef __PBR_OGLAPP_H__
#define __PBR_OGLAPP_H__

#include <string>

namespace pbr {

class OpenGLApplication {
public:
    OpenGLApplication(const std::string& title, int width, int height);
    virtual ~OpenGLApplication() = default;

    void init(int argc, char* argv[]);
    void setTitle(const std::string& title);
    void render();
    void loop() const;
    void refresh() const;

    void updateMouse(int x, int y);

    void setCloseCallback(void (*close)());
    void setDisplayCallback(void (*display)());
    void setReshapeCallback(void (*reshape)(int, int));
    void setIdleCallback(void (*idle)());
    void setCleanupCallback(void (*cleanup)());
    void setTimerCallback(void (*timer)(int), unsigned int time, int value);
    void setMouseMotionCallback(void (*mouseMove)(int, int));
    void setPassiveMouseMotionCallback(void (*mouseMove)(int, int));
    void setKeyPressCallback(void (*keyPress)(unsigned char, int, int));
    void setKeyUpCallback(void (*keyUp)(unsigned char, int, int));
    void setMouseButtonCallback(void (*mouseBtn)(int button, int state, int x, int y));

    virtual void idle() const;
    virtual void reshape(int w, int h);
    virtual void cleanup();
    virtual void processMouseMotion(int x, int y);
    virtual void processMouseClick(int button, int state, int x, int y);
    virtual void processKeyPress(unsigned char key, int x, int y);
    virtual void processKeyUp(unsigned char key, int x, int y);
    virtual void prepare() = 0;
    virtual void drawScene() = 0;
    virtual void tickPerSecond() = 0;
    virtual void update(float dt);

protected:
    std::string _title;

    int _frameCount = 0;

    int _width;
    int _height;

    int _mouseX;
    int _mouseY;

    int _clickX;
    int _clickY;

    float _mouseDx;
    float _mouseDy;

    // Keyboard buttons state
    bool _keys[255];

    // Mouse buttons state
    bool _mouseBtns[3];

private:
    int _windowHandle = -1;

    int _oldTimeSinceStart;
    float _accumTime;

    static constexpr float FIXED_DELTA_TIME = 0.01f;
};

} // namespace pbr

#endif