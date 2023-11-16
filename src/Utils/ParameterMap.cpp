#include <ParameterMap.h>

using namespace std;
using namespace pbr;
using namespace pbr::math;

bool ParameterMap::hasFloat(const string& name) const {
    return _floats.find(name) != _floats.end();
};

bool ParameterMap::hasRGB(const string& name) const {
    return _rgb.find(name) != _rgb.end();
};

bool ParameterMap::hasTexture(const string& name) const {
    return _textures.find(name) != _textures.end();
};

bool ParameterMap::hasBlinn() const {
    return _matInfo.find("blinn") != _matInfo.end();
};

bool ParameterMap::hasUnreal() const {
    return _matInfo.find("unreal") != _matInfo.end();
};

void ParameterMap::setFloat(const string& name, float value) {
    _floats.insert({name, value});
};

void ParameterMap::setRGB(const string& name, const Vec3& value) {
    _rgb.insert({name, value});
};

void ParameterMap::setTexture(const string& name, const string& value) {
    _textures.insert({name, value});
};

void ParameterMap::setMatInfo(const string& name, const string& value) {
    _matInfo.insert({name, value});
};

std::optional<float> ParameterMap::getFloat(const string& name) const {
    auto it = _floats.find(name);
    if (it == _floats.end())
        return {};
    return it->second;
}

std::optional<Vec3> ParameterMap::getRGB(const string& name) const {
    auto it = _rgb.find(name);
    if (it == _rgb.end())
        return {};
    return it->second;
}

std::optional<string> ParameterMap::getTexture(const string& name) const {
    auto it = _textures.find(name);
    if (it == _textures.end())
        return {};
    return it->second;
}

std::optional<string> ParameterMap::getMatInfo(const string& name) const {
    auto it = _matInfo.find(name);
    if (it == _matInfo.end())
        return {};
    return it->second;
}
