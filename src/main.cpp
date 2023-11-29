#include <PBRApp.h>

int main() {
    auto app = std::make_unique<pbr::PBRApp>("PBR Demo", 1920, 1080);

    app->init();
    app->loop();

    exit(EXIT_SUCCESS);
}