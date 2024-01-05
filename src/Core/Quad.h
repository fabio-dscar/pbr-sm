#ifndef __PBR_QUAD_H_
#define __PBR_QUAD_H_

#include <Shape.h>

using namespace pbr::math;

namespace pbr {

class Quad : public Shape {
public:
    void prepare() override;

    BBox3 bbox() const override;
    BSphere bSphere() const override;

    bool intersect(const Ray& ray) const override;
    bool intersect(const Ray& ray, RayHitInfo& info) const override;

private:
    BBox3 _bbox;
    RRID _geoId = -1;
};

} // namespace pbr

#endif