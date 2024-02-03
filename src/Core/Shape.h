#ifndef PBR_SHAPE_H
#define PBR_SHAPE_H

#include <BBox.h>
#include <SceneObject.h>

using namespace pbr::math;

namespace pbr {
namespace math {

class Ray;

} // namespace math

class Material;
class Geometry;

enum ShapeUniform : unsigned int { MODEL_MATRIX = 20, NORMAL_MATRIX = 21 };

class Shape : public SceneObject {
public:
    Shape() = default;
    Shape(const Vec3& position);
    Shape(const Mat4& objToWorld);

    virtual ~Shape() = default;

    virtual void prepare(){};
    virtual void draw();

    const sref<Material>& material() const;
    const sref<Geometry>& geometry() const;

    const Mat3& normalMatrix() const;

    virtual BBox3 bbox() const = 0;
    virtual BSphere bSphere() const = 0;

    virtual std::optional<float> intersect(const Ray& ray, float tMax) const = 0;

    void updateMatrix() override;

    void setMaterial(const sref<Material>& mat);

protected:
    sref<Geometry> _geometry = nullptr;
    sref<Material> _material = nullptr;

    Mat3 _normalMatrix;
};

} // namespace pbr

#endif