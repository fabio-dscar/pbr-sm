#include <Scene.h>

#include <Shape.h>
#include <Skybox.h>
#include <Ray.h>

using namespace pbr;

std::optional<Shape*> Scene::intersect(const Ray& ray) {
    std::optional<Shape*> obj = std::nullopt;

    float tMax = FLOAT_INFINITY;
    for (const auto& shape : _shapes) {
        if (auto t = shape->intersect(ray, tMax)) {
            tMax = std::min(tMax, *t);
            obj = shape.get();
        }
    }

    return obj;
}

void Scene::sortShapes(const Vec3& pos) {
    std::sort(_shapes.begin(), _shapes.end(),
              [&pos](const sref<Shape>& a, const sref<Shape>& b) -> bool {
                  float distA = (a->position() - pos).lengthSqr();
                  float distB = (b->position() - pos).lengthSqr();
                  return distA < distB;
              });
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
}

const std::vector<sref<Camera>>& Scene::cameras() const {
    return _cameras;
}

const std::vector<sref<Shape>>& Scene::shapes() const {
    return _shapes;
}

const std::vector<sref<Light>>& Scene::lights() const {
    return _lights;
}

const Skybox& Scene::skybox() const {
    return *_skybox;
}

bool Scene::hasSkybox() const {
    return _skybox != nullptr;
}