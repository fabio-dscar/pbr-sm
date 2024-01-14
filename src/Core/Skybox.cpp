#include <Skybox.h>

#include <Resources.h>
#include <RenderInterface.h>
#include <Texture.h>
#include <Shader.h>
#include <Geometry.h>

#include <ParameterMap.h>

using namespace pbr;

Skybox::Skybox(const std::string& name, const sref<Texture>& cube,
               const sref<Texture>& irr, const sref<Texture>& spec)
    : _name(name), _cube(cube), _irradiance(irr), _specular(spec) {

    _prog = Resource.get<Program>("skybox");
    _geometry = Resource.get<Geometry>("unitCube");
}

void Skybox::set() const {
    RHI.bindTextures(8, 2, std::array{_irradiance->id(), _specular->id()});
}

void Skybox::draw() const {
    _prog->use();

    RHI.bindTextures(5, 1, std::array{_cube->id()});

    glCullFace(GL_FRONT);
    _geometry->draw();
    glCullFace(GL_BACK);
}

Skybox pbr::CreateSkybox(const ParameterMap& params) {
    fs::path parentDir = params.lookup("parentdir", ""s);
    auto optFolder = params.lookup<std::string>("folder");

    CHECK(optFolder.has_value());

    auto folder = optFolder.value();
    auto fullPath = parentDir / folder;
    std::string fname = fullPath.filename();

    auto cube = CreateNamedCubemap(fname + "_cube", fullPath / "cube.cube");
    auto irr = CreateNamedCubemap(fname + "_irradiance", fullPath / "irradiance.cube");
    auto spec = CreateNamedCubemap(fname + "_specular", fullPath / "specular.cube",
                                   {.min = Filter::LinearMipLinear});

    return {fname, cube, irr, spec};
}