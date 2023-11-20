#ifndef __PBR_SHAPE_H__
#define __PBR_SHAPE_H__

#include <Bounds.h>
#include <SceneObject.h>

using namespace pbr::math;

namespace pbr {
namespace math {

class Ray;
struct RayHitInfo;

} // namespace math

class Material;
class Geometry;
class Skybox;

enum ShapeUniform : uint32 {
    MODEL_MATRIX = 10,
    NORMAL_MATRIX = 11
};

class Shape : public SceneObject {
public:
    Shape() = default;
    Shape(const Vec3& position);
    Shape(const Mat4& objToWorld);

    virtual ~Shape() = default;

    virtual void prepare() = 0;
    virtual void draw() = 0;

    const sref<Material>& material() const;
    const sref<Geometry>& geometry() const;

    const Mat3& normalMatrix() const;

    virtual BBox3 bbox() const = 0;
    virtual BSphere bSphere() const = 0;

    virtual bool intersect(const Ray& ray) const = 0;
    virtual bool intersect(const Ray& ray, RayHitInfo& info) const = 0;

    void updateMatrix() override;

    void setMaterial(const sref<Material>& mat);
    void updateMaterial(const Skybox& skybox);

    RRID _prog;

protected:
    sref<Geometry> _geometry = nullptr;
    sref<Material> _material = nullptr;

    Mat3 _normalMatrix;
};

} // namespace pbr

#endif