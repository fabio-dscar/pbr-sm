#ifndef PBR_SKYBOX_H
#define PBR_SKYBOX_H

#include <PBR.h>

namespace pbr {

class Texture;
class Program;
class Geometry;

class Skybox {
public:
    Skybox(const std::string& name, const sref<Texture>& cube, const sref<Texture>& irr,
           const sref<Texture>& spec);

    const std::string& name() const { return _name; }

    void draw() const;
    void set() const;

private:
    std::string _name;

    std::shared_ptr<Program> _prog;
    std::shared_ptr<Geometry> _geometry;

    std::shared_ptr<Texture> _cube;
    std::shared_ptr<Texture> _irradiance;
    std::shared_ptr<Texture> _specular;
};

Skybox CreateSkybox(const ParameterMap& params);

} // namespace pbr

#endif