#ifndef PBR_BBOX_H
#define PBR_BBOX_H

#include <PBRMath.h>

namespace pbr {
namespace math {

class Ray;
class BSphere;

class BBox3 {
public:
    BBox3() = default;
    BBox3(const Vec3& pt) : _min(pt), _max(pt) {}
    BBox3(const Vec3& min, const Vec3& max) : _min(min), _max(max) {}

    const Vec3& min() const;
    const Vec3& max() const;
    const Vec3& operator[](unsigned int i) const;

    Vec3 sizes() const;
    Vec3 center() const;
    float volume() const;
    float area() const;

    BSphere sphere() const;

    bool contains(const Vec3& pos) const;
    bool overlaps(const BBox3& box) const;
    void expand(float size);
    void expand(const Vec3& pt);
    void expand(const BBox3& box);
    void intersect(const BBox3& box);
    bool isBounded() const;

    std::optional<float> intersectRay(const Ray& ray, float tMax) const;

private:
    Vec3 _min{-FLOAT_INFINITY};
    Vec3 _max{FLOAT_INFINITY};
};

BBox3 Expand(const BBox3& box, const Vec3& pt);
BBox3 Expand(const BBox3& box1, const BBox3& box2);
BBox3 Intersection(const BBox3& box1, const BBox3& box2);
bool Overlaps(const BBox3& box1, const BBox3& box2);

BBox3 Transform(const Matrix4x4& mat, const BBox3& box);

class BSphere {
public:
    BSphere() = default;
    BSphere(const Vec3& center, float radius) : _center(center), _radius(radius) {}

    const Vec3& center() const;
    float radius() const;
    float area() const;

    bool contains(const Vec3& pos) const;
    bool isBounded() const;

    std::optional<float> intersectRay(const Ray& ray, float tMax) const;

private:
    Vec3 _center{0, 0, 0};
    float _radius = FLOAT_INFINITY;
};

BSphere Transform(const Matrix4x4& mat, const BSphere& bSphere);

} // namespace math
} // namespace pbr

#endif