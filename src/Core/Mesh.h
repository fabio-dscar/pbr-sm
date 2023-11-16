#ifndef __PBR_MESH_H__
#define __PBR_MESH_H__

#include <Shape.h>

namespace pbr {

    class Mesh : public Shape {
    public:
        Mesh(const std::string& objFile);
        Mesh(const std::string& objFile, const Mat4& objToWorld);

        void prepare() override;
        void draw()    override;

        BBox3   bbox()    const override;
        BSphere bSphere() const override;

        bool intersect(const Ray& ray) const override;
        bool intersect(const Ray& ray, RayHitInfo& info) const override;

    private:
        BBox3 _bbox;
    };

}

#endif