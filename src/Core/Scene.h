#ifndef PBR_SCENE_H
#define PBR_SCENE_H

#include <PBR.h>
#include <BBox.h>
#include <optional>

using namespace pbr::math;

namespace pbr {
namespace math {

class Ray;

}

class Camera;
class Shape;
class Light;
class Skybox;

class Scene {
public:
    std::optional<Shape*> intersect(const Ray& ray);

    void addCamera(const sref<Camera>& camera);
    void addShape(const sref<Shape>& shape);
    void addLight(const sref<Light>& light);

    void setEnvironment(const Skybox& skybox);

    const std::vector<sref<Camera>>& cameras() const;
    const std::vector<sref<Shape>>& shapes() const;
    const std::vector<sref<Light>>& lights() const;

    bool hasSkybox() const;
    const Skybox& skybox() const;

    void sortShapes(const Vec3& pos);

private:
    BBox3 _bbox{Vec3{0}};

    std::vector<sref<Camera>> _cameras;
    std::vector<sref<Shape>> _shapes;
    std::vector<sref<Light>> _lights;

    const Skybox* _skybox = nullptr;
};

} // namespace pbr

#endif