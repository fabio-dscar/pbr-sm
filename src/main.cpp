#include <PBRApp.h>

#include <Utils.h>

int main() {
    auto app = std::make_unique<pbr::PBRApp>("PBR Demo", 1920, 1080);
    try {
        app->init();
        app->loop();
    } catch (std::runtime_error& err) {
        pbr::util::PrintError("{}", err.what());
        app->cleanup();
        return 1;
    }
}