#ifndef __PBR_SKYBOX_H__
#define __PBR_SKYBOX_H__

#include <PBR.h>

namespace pbr {

class Texture;

class Skybox {
public:
    Skybox(const std::string& name, const Texture& cube, const Texture& irr,
           const Texture& spec);
    Skybox(RRID cubeProg, RRID cubeTex);

    const std::string& name() const { return _name; }

    void draw() const;
    void set() const;

    RRID irradianceTex() const;
    RRID cubeTex() const;
    RRID ggxTex() const;

private:
    std::string _name;

    RRID _cubeProg;
    RRID _geoId;

    RRID _cubeTex;
    RRID _irradianceTex;
    RRID _ggxTex;
};

Skybox CreateSkybox(const ParameterMap& params);

} // namespace pbr

#endif