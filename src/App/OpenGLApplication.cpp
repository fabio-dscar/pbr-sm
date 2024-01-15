#include <OpenGLApplication.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <sstream>

#include <format>
#include <Utils.h>

using namespace pbr;

namespace {

KeyState ActionToKeyState(int action) {
    switch (action) {
    case GLFW_PRESS:
        return KeyState::Pressed;
    case GLFW_RELEASE:
        return KeyState::Released;
    case GLFW_REPEAT:
        return KeyState::Repeat;
    default:
        LOG_ERROR("Unknown key action {}.", action);
        return KeyState::Released;
    };
}

} // namespace

OpenGLApplication::OpenGLApplication(const std::string& title, int width, int height)
    : _title(title), _width(width), _height(height) {}

void OpenGLApplication::setCallbacks() {
    using OGLApp = OpenGLApplication;

    glfwSetKeyCallback(_window, [](GLFWwindow* w, int k, int s, int a, int m) {
        OGLApp* app = static_cast<OGLApp*>(glfwGetWindowUserPointer(w));
        app->processKeys(k, s, a, m);
    });

    glfwSetFramebufferSizeCallback(_window, [](GLFWwindow* win, int w, int h) {
        OGLApp* app = static_cast<OGLApp*>(glfwGetWindowUserPointer(win));
        app->reshape(w, h);
    });

    glfwSetMouseButtonCallback(_window, [](GLFWwindow* w, int b, int a, int m) {
        OGLApp* app = static_cast<OGLApp*>(glfwGetWindowUserPointer(w));
        app->processMouseClick(b, a, m);
    });

    glfwSetCursorPosCallback(_window, [](GLFWwindow* w, double x, double y) {
        OGLApp* app = static_cast<OGLApp*>(glfwGetWindowUserPointer(w));
        app->processMouseMotion(x, y);
    });
}

void OpenGLApplication::init() {
    glfwSetErrorCallback(
        [](int, const char* desc) { LOG_ERROR("Glfw Error: {}", desc); });

    if (!glfwInit())
        FATAL("Failed to initialize GLFW.");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 8);

    _window = glfwCreateWindow(_width, _height, _title.c_str(), NULL, NULL);
    if (!_window) {
        glfwTerminate();
        FATAL("Failed to create window.");
    }

    glfwSetWindowUserPointer(_window, this);
    setCallbacks();

    glfwMakeContextCurrent(_window);

    // Setup glad
    int glver = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    if (glver == 0)
        FATAL("Failed to load OpenGL functions.");

    glfwSwapInterval(1);

#ifdef DEBUG
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(util::OpenGLErrorCallback, 0);
#endif

    // Print system info
    auto renderer = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
    auto vendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
    auto version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    auto glslVer =
        reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION));
    LOGI("OpenGL Renderer: {} ({})", renderer, vendor);
    LOGI("OpenGL Version: {}", version);
    LOGI("GLSL Version {}", glslVer);

    prepare();
}

void OpenGLApplication::loop() {
    while (!glfwWindowShouldClose(_window)) {
        updateTime();
        render();

        glfwSwapBuffers(_window);
        glfwPollEvents();
    }

    glfwDestroyWindow(_window);
    glfwTerminate();

    cleanup();
}

void OpenGLApplication::setTitle(const std::string& title) {
    _title = title;
    glfwSetWindowTitle(_window, _title.c_str());
}

void OpenGLApplication::reshape(int w, int h) {
    _width = w;
    _height = h;
    glViewport(0, 0, w, h);
}

void OpenGLApplication::updateTime() {
    const double timeSinceStart = glfwGetTime();

    _deltaTime = timeSinceStart - _time;
    _time = timeSinceStart;

    _secondsTimer += _deltaTime;
    if (_secondsTimer > 1.0) {
        tickPerSecond();
        _secondsTimer -= 1.0;
    }

    ++_frameCount;
}

void OpenGLApplication::render() {
    update(std::min(_deltaTime, 0.25));
    renderScene();

    _mouseDx = 0;
    _mouseDy = 0;
}

void OpenGLApplication::processMouseMotion(double x, double y) {
    updateMouse(x, y);
}

void OpenGLApplication::processKeys(int key, int /*scancode*/, int action, int /*mods*/) {
    CHECK(key < MaxKeyNum);

    auto keyState = ActionToKeyState(action);
    _keyboard.keys[key] = keyState;
}

void OpenGLApplication::processMouseClick(int button, int action, int /*mods*/) {
    if (button > 2)
        return;

    auto state = ActionToKeyState(action);
    _mouse.buttons[button] = state;
}

void OpenGLApplication::updateMouse(double x, double y) {
    _mouseDx = -x + _mouse.x;
    _mouseDy = y - _mouse.y;

    _mouse.x = x;
    _mouse.y = y;
}

bool OpenGLApplication::isMousePressed(MouseButton button) const {
    return _mouse.buttons[ToUnderlying(button)] == KeyState::Pressed;
}

bool OpenGLApplication::isKeyPressed(int key) const {
    return _keyboard.keys[key] == KeyState::Pressed ||
           _keyboard.keys[key] == KeyState::Repeat;
}

bool OpenGLApplication::checkKey(int key, KeyState state) const {
    return _keyboard.keys[key] == state;
}