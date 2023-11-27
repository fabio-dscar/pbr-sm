#include "Resources.h"
#include <Utils.h>

#include <filesystem>
#include <fstream>
#include <iostream>

#include <LoadXML.h>
#include <Mesh.h>
#include <PBRMaterial.h>
#include <RenderInterface.h>
#include <Texture.h>

#include <format>

using namespace pbr;

namespace fs = std::filesystem;

bool Utils::readFile(const std::string& filePath, std::ios_base::openmode mode,
                     std::string& str) {

    std::ifstream file(filePath, mode);
    if (file.fail()) {
        perror(filePath.c_str());
        return false;
    }

    file.seekg(0, std::ios::end);
    str.reserve((size_t)file.tellg());
    file.seekg(0, std::ios::beg);

    str.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    return true;
}

void Utils::throwError(const std::string& error) {
    std::cerr << error << std::endl;
    std::cin.get();
    exit(EXIT_FAILURE);
}

std::unique_ptr<Shape> Utils::loadSceneObject(const std::string& folder) {
    fs::path objRoot{std::format("Objects/{}", folder)};

    fs::path objFile{objRoot / std::format("{}.obj", folder)};
    auto obj = std::make_unique<Mesh>(objFile);

    LoadXML loader(objRoot / "material.xml");
    auto mat = buildMaterial(objRoot, loader.getMap());
    mat->prepare();

    obj->prepare();
    obj->setMaterial(std::move(mat));

    return obj;
}

RRID Utils::loadTexture(const std::string& path) {
    if (!std::filesystem::exists(path)) {
        std::cout << std::format("[INFO] Couldn't find texture {}\n", path);
        std::cout << std::format("[INFO] Assigning 'unset' texture...\n", path);
        return Resource.getTexture("null")->rrid();
    }

    Image image(path);
    return RHI.createTextureImmutable(image, TexSampler{});
}

std::unique_ptr<Material> Utils::buildMaterial(const fs::path& objRoot,
                                               const ParameterMap& map) {
    auto mat = std::make_unique<PBRMaterial>();

    if (auto tex = map.getTexture("diffuse"))
        mat->setDiffuse(loadTexture(objRoot / tex.value()));
    else
        mat->setDiffuse(map.getRGB("diffuse").value_or(Color{0.5f}));

    if (auto tex = map.getTexture("normal"))
        mat->setNormal(loadTexture(objRoot / tex.value()));

    mat->setSpecular(map.getRGB("specular").value_or(Color{0.04f}));

    if (auto tex = map.getTexture("roughness"))
        mat->setRoughness(loadTexture(objRoot / tex.value()));
    else
        mat->setRoughness(map.getFloat("roughness").value_or(0.2f));

    if (auto tex = map.getTexture("metallic"))
        mat->setMetallic(loadTexture(objRoot / tex.value()));
    else
        mat->setMetallic(map.getFloat("metallic").value_or(0.5f));

    return mat;
}