#include <OpenGLApplication.h>

// #include <GL/glew.h>
#include <glad/glad.h>
#include <GL/freeglut.h>

#include <iostream>
#include <sstream>

using namespace pbr;

OpenGLApplication::OpenGLApplication(const std::string& title, int width, int height)
    : _title(title), _width(width), _height(height), _frameCount(0), _windowHandle(-1) {}

void OpenGLApplication::init(int argc, char* argv[]) {
    // Setup glut
    glutInit(&argc, argv);
    glutInitContextVersion(4, 4);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
    glutInitWindowSize(_width, _height);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA | GLUT_MULTISAMPLE);
    _windowHandle = glutCreateWindow(_title.c_str());
    if (_windowHandle < 1) {
        std::cerr << "ERROR: Could not create a new rendering window.\n";
        exit(EXIT_FAILURE);
    }
    glutIgnoreKeyRepeat(1);

    // Setup glad
    int glver = gladLoadGL();
    if (glver == 0) {
        std::cerr << "Failed to initialize OpenGL context\n";
        exit(EXIT_FAILURE);
    }

    // Print system info
    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* vendor = glGetString(GL_VENDOR);
    const GLubyte* version = glGetString(GL_VERSION);
    const GLubyte* glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
    std::cerr << "OpenGL Renderer: " << renderer << " (" << vendor << ")\n";
    std::cerr << "OpenGL version " << version << '\n';
    std::cerr << "GLSL version " << glslVersion << '\n';

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

void OpenGLApplication::updateFPS() {
    std::ostringstream oss;
    oss << _title << ": " << _frameCount << " FPS @ (" << _width << "x" << _height << ")";
    std::string s = oss.str();
    glutSetWindow(_windowHandle);
    glutSetWindowTitle(s.c_str());
    _frameCount = 0;
}

void OpenGLApplication::setTitle(const std::string& title) {
    _title = title;
}

void OpenGLApplication::reshape(int w, int h) {
    _width = w;
    _height = h;
    glViewport(0, 0, w, h);
}

void OpenGLApplication::update(float dt) {}

void OpenGLApplication::render() {
    int timeSinceStart = glutGet(GLUT_ELAPSED_TIME);
    int deltaTime = timeSinceStart - _oldTimeSinceStart;
    _oldTimeSinceStart = timeSinceStart;

    float dt = (float)deltaTime / 1000.0f;

    // Limit the delta time to avoid large intervals
    if (dt > 0.25f)
        dt = 0.25f;

    // --------------------------------------
    //   Update step
    // --------------------------------------
    update(dt);

    // --------------------------------------
    //   Render step
    // --------------------------------------
    ++_frameCount;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawScene();
    glutSwapBuffers();

    _mouseDx = 0;
    _mouseDy = 0;
}

void OpenGLApplication::loop() const {
    glutMainLoop();
}

void OpenGLApplication::idle() const {
    glutPostRedisplay();
}

void OpenGLApplication::cleanup() {}

void OpenGLApplication::processMouseMotion(int x, int y) {
    updateMouse(x, y);
}

void OpenGLApplication::processKeyPress(unsigned char key, int x, int y) {
    _keys[key] = true;
}

void OpenGLApplication::processKeyUp(unsigned char key, int x, int y) {
    _keys[key] = false;
}

void OpenGLApplication::processMouseClick(int button, int state, int x, int y) {
    if (state == GLUT_DOWN)
        _mouseBtns[button] = true;
    else
        _mouseBtns[button] = false;

    _clickX = x;
    _clickY = y;
}

void OpenGLApplication::updateMouse(int x, int y) {
    int dx = -x + _mouseX;
    int dy = y - _mouseY;

    _mouseX = x;
    _mouseY = y;

    _mouseDx = (_mouseDx + (float)dx) / 2.0f;
    _mouseDy = (_mouseDy + (float)dy) / 2.0f;
}

void OpenGLApplication::refresh() const {
    glutPostRedisplay();
}

void OpenGLApplication::setDisplayCallback(void (*display)()) {
    glutDisplayFunc(display);
}

void OpenGLApplication::setCloseCallback(void (*close)()) {
    glutCloseFunc(close);
}

void OpenGLApplication::setReshapeCallback(void (*reshape)(int, int)) {
    glutReshapeFunc(reshape);
}

void OpenGLApplication::setIdleCallback(void (*idle)()) {
    glutIdleFunc(idle);
}

void OpenGLApplication::setCleanupCallback(void (*cleanup)()) {
    glutCloseFunc(cleanup);
}

void OpenGLApplication::setTimerCallback(void (*idle)(int), unsigned int time,
                                         int value) {
    glutTimerFunc(time, idle, value);
}

void OpenGLApplication::setPassiveMouseMotionCallback(void (*mouseMove)(int, int)) {
    glutPassiveMotionFunc(mouseMove);
}

void OpenGLApplication::setMouseMotionCallback(void (*mouseMove)(int, int)) {
    glutMotionFunc(mouseMove);
}

void OpenGLApplication::setKeyPressCallback(void (*keyPress)(unsigned char, int, int)) {
    glutKeyboardFunc(keyPress);
}

void OpenGLApplication::setKeyUpCallback(void (*keyUp)(unsigned char, int, int)) {
    glutKeyboardUpFunc(keyUp);
}

void OpenGLApplication::setMouseButtonCallback(void (*mouseBtn)(int button, int state,
                                                                int x, int y)) {
    glutMouseFunc(mouseBtn);
}