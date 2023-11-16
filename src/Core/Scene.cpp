#include <Scene.h>

#include <Shape.h>
#include <Skybox.h>
#include <Ray.h>

using namespace pbr;

std::optional<Shape*> Scene::intersect(const Ray& ray) {
    RayHitInfo info;
    float t;
    info.dist = FLOAT_INFINITY;
    info.obj = nullptr;

    std::cout << "o: " << ray.origin() << "\n";
    std::cout << "d: " << ray.direction() << "\n";

    for (const auto& shape : _shapes) {
        BBox3 bbox = shape->bbox();
        std::cout << bbox.sizes() << "\n";
        if (bbox.intersectRay(ray, &t)) {
            if (t < info.dist) {
                info.dist = t;
                info.obj = shape.get();
            }
        }
    }

    if (!info.obj)
        return {};

    return static_cast<Shape*>(info.obj);
}

void Scene::addCamera(const sref<Camera>& camera) {
    _cameras.push_back(camera);
}

void Scene::addShape(const sref<Shape>& shape) {
    _bbox.expand(shape->bbox());
    _shapes.push_back(shape);
}

void Scene::addLight(const sref<Light>& light) {
    _lights.push_back(light);
}

void Scene::setEnvironment(const Skybox& skybox) {
    _skybox = &skybox;

    for (auto& shape : _shapes)
        shape->updateMaterial(skybox);
}

const vec<sref<Camera>>& Scene::cameras() const {
    return _cameras;
}

const vec<sref<Shape>>& Scene::shapes() const {
    return _shapes;
}

const vec<sref<Light>>& Scene::lights() const {
    return _lights;
}

const Skybox& Scene::skybox() const {
    return *_skybox;
}

bool Scene::hasSkybox() const {
    return _skybox != nullptr;
}