#include <Resources.h>

#include <Geometry.h>
#include <Shape.h>
#include <Shader.h>

using namespace pbr;

Resources& Resources::get() {
    static Resources _inst;
    return _inst;
}

void Resources::initialize() {

}

void Resources::addGeometry(const std::string& name, const sref<Geometry>& geometry) {
    _geometry[name] = geometry;
}

void Resources::addShape(const std::string& name, const sref<Shape>& shape) {
    _shapes[name] = shape;
}

void Resources::addShader(const std::string& name, const sref<Shader>& shader) {
    _shaders[name] = shader;
}

void Resources::addTexture(const std::string& name, const sref<Texture>& texture) {
    _textures[name] = texture;
}

bool Resources::deleteGeometry(const std::string& name) {
    auto it = _geometry.find(name);
    if (it != _geometry.end()) {
        _geometry.erase(name);
        return true;
    }

    return false;
}

bool Resources::deleteShape(const std::string& name) {
    auto it = _shapes.find(name);
    if (it != _shapes.end()) {
        _shapes.erase(name);
        return true;
    }

    return false;
}

bool Resources::deleteShader(const std::string& name) {
    auto it = _shaders.find(name);
    if (it != _shaders.end()) {
        _shaders.erase(name);
        return true;
    }

    return false;
}

bool Resources::deleteTexture(const std::string& name) {
    auto it = _textures.find(name);
    if (it != _textures.end()) {
        _textures.erase(name);
        return true;
    }

    return false;
}

Shader* Resources::getShader(const std::string& name) {
    return _shaders.at(name).get();
}

Geometry* Resources::getGeometry(const std::string& name) {
    return _geometry.at(name).get();
}

Shape* Resources::getShape(const std::string& name) {
    return _shapes.at(name).get();
}

Texture* Resources::getTexture(const std::string& name) {
    return _textures.at(name).get();
}

void Resources::cleanup() {
    _geometry.clear();
    _shapes.clear();
    _shaders.clear();
    _textures.clear();
}