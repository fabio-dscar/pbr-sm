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

std::optional<std::string> util::ReadTextFile(const fs::path& filePath) {
    std::ifstream file(filePath, std::ios_base::in | std::ios_base::ate);
    if (file.fail()) {
        perror(filePath.c_str());
        return {};
    }

    std::string contents;
    contents.reserve(static_cast<std::size_t>(file.tellg()));
    file.seekg(0, std::ios::beg);

    contents.assign((std::istreambuf_iterator<char>(file)),
                    std::istreambuf_iterator<char>());

    return contents;
}

std::unique_ptr<Shape> util::LoadSceneObject(const std::string& folder) {
    fs::path objRoot = "Objects" / fs::path{std::format("Objects/{}", folder)};

    fs::path objFile{objRoot / std::format("{}.obj", folder)};
    auto obj = std::make_unique<Mesh>(objFile);

    LoadXML loader(objRoot / "material.xml");
    auto mat = BuildMaterial(objRoot, loader.getMap());
    mat->prepare();

    obj->prepare();
    obj->setMaterial(std::move(mat));

    return obj;
}

RRID util::LoadTexture(const fs::path& path) {
    if (!fs::exists(path)) {
        LOG_ERROR("Couldn't find texture {}. Assigning 'unset' texture.", path.string());
        return Resource.getTexture("null")->rrid();
    }

    Image image(path);
    return RHI.createTextureImmutable(image, TexSampler{});
}

std::unique_ptr<Material> util::BuildMaterial(const fs::path& objRoot,
                                              const OParameterMap& map) {
    auto mat = std::make_unique<PBRMaterial>();

    if (auto tex = map.getTexture("diffuse"))
        mat->setDiffuse(LoadTexture(objRoot / tex.value()));
    else
        mat->setDiffuse(map.getRGB("diffuse").value_or(Color{0.5f}));

    if (auto tex = map.getTexture("normal"))
        mat->setNormal(LoadTexture(objRoot / tex.value()));

    mat->setReflectivity(map.getFloat("specular").value_or(0.5f));

    if (auto tex = map.getTexture("roughness"))
        mat->setRoughness(LoadTexture(objRoot / tex.value()));
    else
        mat->setRoughness(map.getFloat("roughness").value_or(0.2f));

    if (auto tex = map.getTexture("metallic"))
        mat->setMetallic(LoadTexture(objRoot / tex.value()));
    else
        mat->setMetallic(map.getFloat("metallic").value_or(0.5f));

    if (auto tex = map.getTexture("ao"))
        mat->setOcclusion(LoadTexture(objRoot / tex.value()));

    if (auto tex = map.getTexture("emissive"))
        mat->setEmissive(LoadTexture(objRoot / tex.value()));

    if (auto tex = map.getTexture("clearnormal"))
        mat->setEmissive(LoadTexture(objRoot / tex.value()));

    return mat;
}