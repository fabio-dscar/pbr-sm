#include <GltfLoader.h>

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
// #define TINYGLTF_NOEXCEPTION // optional. disable exception handling.
#include <tiny_gltf.h>

using namespace pbr;
using namespace tinygltf;

bool pbr::loadGlTF(const std::string& filePath) {
    Model model;
    TinyGLTF loader;
    std::string err;
    std::string warn;

    loader.SetPreserveImageChannels(true);

    bool ret = loader.LoadASCIIFromFile(&model, &err, &warn, filePath);
    // bool ret = loader.LoadBinaryFromFile(&model, &err, &warn, filePath); // for binary
    //   glTF(.glb)

    if (!warn.empty()) {
        printf("Warn: %s\n", warn.c_str());
    }

    if (!err.empty()) {
        printf("Err: %s\n", err.c_str());
    }

    if (!ret) {
        printf("Failed to parse glTF\n");
        return -1;
    }

    return true;
}
