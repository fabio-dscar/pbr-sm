#ifndef __PBR_SPHERE_H__
#define __PBR_SPHERE_H__

#include <Shape.h>

using namespace pbr::math;

namespace pbr {

    class Sphere : public Shape {
    public:
        Sphere(const Vec3& pos, float radius);
        Sphere(const Mat4& objToWorld, float radius);

        void prepare() override;
        void draw() override;

        BBox3   bbox()    const override;
        BSphere bSphere() const override;

        bool intersect(const Ray& ray) const override;
        bool intersect(const Ray& ray, RayHitInfo& info) const override;

    private:
        float _radius;
    };

}

#endif