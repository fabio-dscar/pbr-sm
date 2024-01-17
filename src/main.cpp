#include <PBRApp.h>

#include <Utils.h>

using namespace pbr;

int main(int argc, char* argv[]) {
    try {
        auto opts = ParseArgs(argc, argv);
        auto app = std::make_unique<PBRApp>("PBR Demo", opts);
        app->loop();
    } catch (std::runtime_error& err) {
        pbr::util::PrintError("{}", err.what());
        return 1;
    }
}