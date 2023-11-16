#include <Utils.h>

#include <fstream>
#include <iostream>

#include <LoadXML.h>
#include <Mesh.h>
#include <PBRMaterial.h>
#include <RenderInterface.h>
#include <Texture.h>

using namespace pbr;

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

    str.assign((std::istreambuf_iterator<char>(file)), 
                std::istreambuf_iterator<char>());

    return true;
}

void Utils::throwError(const std::string& error) {
    std::cerr << error << std::endl;
    std::cin.get();
    exit(EXIT_FAILURE);
}

std::unique_ptr<Shape> Utils::loadSceneObject(const std::string& folder) {
    auto obj = std::make_unique<Mesh>("Objects/" + folder + "/" + folder + ".obj");

    LoadXML loader("Objects/" + folder + "/material.xml");
    auto mat = buildMaterial("Objects/" + folder, loader.getMap());
    mat->prepare();
    
    obj->prepare();
    obj->setMaterial(std::move(mat));

    return obj;
}

RRID Utils::loadTexture(const std::string& path) {
    Image image;
    TexSampler texSampler;

    image.loadImage(path);

    RRID rrid = RHI.createTextureImmutable(image, texSampler);

    return rrid;
}

std::unique_ptr<Material> Utils::buildMaterial(const std::string& path,
                                               const ParameterMap& map) {
    auto mat = std::make_unique<PBRMaterial>();

    if (auto diff = map.getRGB("diffuse"))
        mat->setDiffuse(Color(diff.value()));
    else if (auto tex = map.getTexture("diffuse"))
        mat->setDiffuse(loadTexture(path + "/" + tex.value()));
    else
        mat->setDiffuse(Color(0.5f));

    if (auto tex = map.getTexture("normal"))
        mat->setNormal(loadTexture(path + "/" + tex.value()));

    if (auto spec = map.getRGB("specular"))
        mat->setSpecular(Color(spec.value()));
    else
        mat->setSpecular(Color(0.04f));

    if (auto rough = map.getFloat("roughness"))
        mat->setRoughness(rough.value());
    else if (auto tex = map.getTexture("roughness"))
        mat->setRoughness(loadTexture(path + "/" + tex.value()));
    else
        mat->setRoughness(0.2f);

    if (auto metal = map.getFloat("metallic"))
        mat->setMetallic(metal.value());
    else if (auto tex = map.getTexture("metallic"))
        mat->setMetallic(loadTexture(path + "/" + tex.value()));
    else
        mat->setMetallic(0.5f);

    return mat;
}