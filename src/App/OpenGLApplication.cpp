#include <OpenGLApplication.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <sstream>

#include <format>
#include <Utils.h>

using namespace pbr;

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

    // Initialize OpenGL state
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_TRUE);
    glDepthRange(0.0, 1.0);
    glClearDepth(1.0);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    glEnable(GL_MULTISAMPLE);

    prepare();
}

void OpenGLApplication::loop() {
    while (!glfwWindowShouldClose(_window)) {
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

void OpenGLApplication::render() {
    double timeSinceStart = glfwGetTime() * 1000.0f;
    double deltaTime = timeSinceStart - _oldTimeSinceStart;
    _oldTimeSinceStart = timeSinceStart;

    _secondCount += deltaTime;
    if (_secondCount > 1000) {
        tickPerSecond();
        _secondCount -= 1000;
    }

    float dt = deltaTime / 1000.0f;

    // Limit the delta time to avoid large intervals
    dt = std::min(dt, 0.25f);

    update(dt);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawScene();
    ++_frameCount;

    _mouseDx = 0;
    _mouseDy = 0;
}

void OpenGLApplication::processMouseMotion(double x, double y) {
    updateMouse(x, y);
}

void OpenGLApplication::processKeys(int key, int scancode, int action, int mods) {
    if (key >= 255)
        return;

    if (action == GLFW_PRESS || action == GLFW_REPEAT)
        _keys[key] = true;
    else
        _keys[key] = false;
}

void OpenGLApplication::processMouseClick(int button, int action, int mods) {
    if (button > 2)
        return;

    if (action == GLFW_PRESS)
        _mouseBtns[button] = true;
    else
        _mouseBtns[button] = false;

    _clickX = _mouseX;
    _clickY = _mouseY;
}

void OpenGLApplication::updateMouse(double x, double y) {
    double dx = -x + _mouseX;
    double dy = y - _mouseY;

    _mouseX = x;
    _mouseY = y;

    _mouseDx = (_mouseDx + dx) / 2.0f;
    _mouseDy = (_mouseDy + dy) / 2.0f;
}