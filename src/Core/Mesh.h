#ifndef PBR_MESH_H
#define PBR_MESH_H

#include <Shape.h>

namespace pbr {

class Mesh : public Shape {
public:
    Mesh(const std::shared_ptr<Geometry>& geometry, const Mat4& objToWorld);

    BBox3 bbox() const override;
    BSphere bSphere() const override;

    std::optional<float> intersect(const Ray& ray, float tMax) const override;

private:
    BBox3 _bbox;
};

std::unique_ptr<Shape> CreateMesh(const ParameterMap& params);

} // namespace pbr

#endif