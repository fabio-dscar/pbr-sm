#ifndef __PBR_BOUNDS_H__
#define __PBR_BOUNDS_H__

#include <PBRMath.h>

namespace pbr {
namespace math {

class Ray;
class BSphere;

class PBR_SHARED BBox3 {
public:
    static const BBox3 UNBOUNDED;

    BBox3();
    BBox3(const Vec3& pt);
    BBox3(const Vec3& min, const Vec3& max);

    const Vec3& min() const;
    const Vec3& max() const;
    const Vec3& operator[](uint32 i) const;

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

    bool intersectRay(const Ray& ray, float* t) const;

private:
    Vec3 _min;
    Vec3 _max;
};

PBR_SHARED BBox3 expand(const BBox3& box, const Vec3& pt);
PBR_SHARED BBox3 expand(const BBox3& box1, const BBox3& box2);
PBR_SHARED BBox3 intersection(const BBox3& box1, const BBox3& box2);
PBR_SHARED bool overlaps(const BBox3& box1, const BBox3& box2);

PBR_SHARED BBox3 transform(const Matrix4x4& mat, const BBox3& box);

class PBR_SHARED BSphere {
public:
    static const BSphere UNBOUNDED;

    BSphere();
    BSphere(const Vec3& center, float radius);

    const Vec3& center() const;
    float radius() const;
    float area() const;

    bool contains(const Vec3& pos) const;
    bool isBounded() const;

private:
    Vec3 _center;
    float _radius;
};

PBR_SHARED BSphere transform(const Matrix4x4& mat, const BSphere& bSphere);
} // namespace math
} // namespace pbr

#endif