#include <Skybox.h>

#include <Resources.h>
#include <RenderInterface.h>
#include <Texture.h>
#include <Shader.h>
#include <Geometry.h>

using namespace pbr;

Skybox::Skybox(RRID cubeProg, RRID cubeTex) : _cubeProg(cubeProg), _geoId(-1), _cubeTex(cubeTex) { }

Skybox::Skybox(const std::string& folder) {
    _cubeProg = Resource.getShader("skybox")->id();

    // Load cubemap
    TexSampler cubeSampler;
    cubeSampler.setFilterMode(FILTER_LINEAR, FILTER_LINEAR);
    cubeSampler.setWrapMode(WRAP_CLAMP_EDGE, WRAP_CLAMP_EDGE, WRAP_CLAMP_EDGE);

    Cubemap cube;
    cube.loadCubemap(folder + "/cube.cube");
    _cubeTex = RHI.createCubemap(cube, cubeSampler);
    Resource.addTexture("sky-" + folder, RHI.getTexture(_cubeTex));

    Cubemap irradianceCube;
    irradianceCube.loadCubemap(folder + "/irradiance.cube");
    _irradianceTex = RHI.createCubemap(irradianceCube, cubeSampler);
    Resource.addTexture("irradiance-" + folder, RHI.getTexture(_irradianceTex));

    TexSampler ggxSampler;
    ggxSampler.setFilterMode(FILTER_LINEAR_MIP_LINEAR, FILTER_LINEAR);
    ggxSampler.setWrapMode(WRAP_CLAMP_EDGE, WRAP_CLAMP_EDGE, WRAP_CLAMP_EDGE);

    Cubemap ggxCube;
    ggxCube.loadCubemap(folder + "/ggx.cube");
    _ggxTex = RHI.createCubemap(ggxCube, ggxSampler);
    Resource.addTexture("ggx-" + folder, RHI.getTexture(_ggxTex));
}

void Skybox::initialize() {
    _geo = std::make_shared<Geometry>();
    genUnitCubeGeometry(*_geo);
    _geoId = RHI.uploadGeometry(_geo);
}

void Skybox::draw() const {
    RHI.useProgram(_cubeProg);

    glActiveTexture(GL_TEXTURE5);
    RHI.bindTexture(_cubeTex);

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