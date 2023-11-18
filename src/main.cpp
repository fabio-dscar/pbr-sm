#include <PBRApp.h>
#include <Resources.h>

using namespace pbr;

std::unique_ptr<PBRApp> app;

void display() {
    app->render();
}

void reshape(int w, int h) {
    app->reshape(w, h);
}

void idle() {
    app->refresh();
}

void timer(int value) {
    app->tickPerSecond();
    app->setTimerCallback(timer, 1000, value);
}

void cleanup() {
    app->cleanup();
}

void mouseMotion(int x, int y) {
    app->processMouseMotion(x, y);
}

void mouseMotionPassive(int x, int y) {
    app->updateMouse(x, y);
}

void keyPress(unsigned char key, int x, int y) {
    app->processKeyPress(key, x, y);
}

void keyUp(unsigned char key, int x, int y) {
    app->processKeyUp(key, x, y);
}

void mouseClick(int button, int state, int x, int y) {
    app->processMouseClick(button, state, x, y);
}

int main(int argc, char* argv[]) {
    app = std::make_unique<PBRApp>("PBR Demo", 1920, 1080);

    app->init(argc, argv);
    app->setReshapeCallback(reshape);
    app->setDisplayCallback(display);
    app->setIdleCallback(idle);
    app->setMouseMotionCallback(mouseMotion);
    app->setPassiveMouseMotionCallback(mouseMotionPassive);
    app->setKeyPressCallback(keyPress);
    app->setKeyUpCallback(keyUp);
    app->setMouseButtonCallback(mouseClick);
    app->setTimerCallback(timer, 0, 0);
    app->setCloseCallback(cleanup);

    app->loop();

    exit(EXIT_SUCCESS);
}