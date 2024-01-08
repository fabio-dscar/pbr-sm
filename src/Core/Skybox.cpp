#include <Skybox.h>

#include <Resources.h>
#include <RenderInterface.h>
#include <Texture.h>
#include <Shader.h>
#include <Geometry.h>

#include <ParameterMap.h>

#include <Utils.h>

using namespace pbr;

Skybox::Skybox(RRID cubeProg, RRID cubeTex)
    : _cubeProg(cubeProg), _geoId(-1), _cubeTex(cubeTex) {}

Skybox::Skybox(const Texture& cube, const Texture& irr, const Texture& spec)
    : _cubeTex(cube.id()), _irradianceTex(irr.id()), _ggxTex(spec.id()) {

    _cubeProg = Resource.get<Program>("skybox")->id();
    _geoId = Resource.get<Geometry>("unitCube")->rrid();
}

void Skybox::draw() const {
    RHI.useProgram(_cubeProg);

    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_CUBE_MAP, _cubeTex);

    // RHI.bindTexture(_cubeTex);

    glCullFace(GL_FRONT);
    RHI.drawGeometry(_geoId);
    glCullFace(GL_BACK);

    RHI.useProgram(0);
}

RRID Skybox::irradianceTex() const {
    return _irradianceTex;
}

RRID Skybox::cubeTex() const {
    return _cubeTex;
}

RRID Skybox::ggxTex() const {
    return _ggxTex;
}

Skybox pbr::CreateSkybox(const ParameterMap& params) {
    fs::path parentDir = params.lookup("parentdir", ""s);
    auto optFolder = params.lookup<std::string>("folder");

    CHECK(optFolder.has_value());

    auto folder = optFolder.value();
    auto fullPath = parentDir / folder;

    auto cube = CreateNamedCubemap(folder + "_cube", fullPath / "cube.cube");
    auto irr = CreateNamedCubemap(folder + "_irradiance", fullPath / "irradiance.cube");
    auto spec = CreateNamedCubemap(folder + "_ggx", fullPath / "ggx.cube",
                                   {.min = Filter::LinearMipLinear});

    return {*cube, *irr, *spec};
}