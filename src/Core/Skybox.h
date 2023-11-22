#ifndef __PBR_SKYBOX_H__
#define __PBR_SKYBOX_H__

#include <PBR.h>

namespace pbr {

class Geometry;

class Skybox {
public:
    explicit Skybox(const std::string& folder);
    Skybox(RRID cubeProg, RRID cubeTex);

    void initialize();
    void draw() const;

    RRID irradianceTex() const;
    RRID cubeTex() const;
    RRID ggxTex() const;

private:
    RRID _cubeProg;
    RRID _geoId;

    RRID _cubeTex;
    RRID _irradianceTex;
    RRID _ggxTex;
};

} // namespace pbr

#endif