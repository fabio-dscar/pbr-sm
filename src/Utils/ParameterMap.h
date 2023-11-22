#ifndef __PBR_PMAP_H__
#define __PBR_PMAP_H__

#include <PBR.h>
#include <PBRMath.h>
#include <Spectrum.h>

#include <optional>
#include <unordered_map>

namespace pbr {

class ParameterMap {
public:
    bool hasFloat(const std::string& name) const;
    bool hasRGB(const std::string& name) const;
    bool hasTexture(const std::string& name) const;
    bool hasBlinn() const;
    bool hasUnreal() const;

    void setFloat(const std::string& name, float value);
    void setRGB(const std::string& name, const Color& value);
    void setTexture(const std::string& name, const std::string& value);
    void setMatInfo(const std::string& name, const std::string& value);

    std::optional<float> getFloat(const std::string& name) const;
    std::optional<Color> getRGB(const std::string& name) const;
    std::optional<std::string> getTexture(const std::string& name) const;
    std::optional<std::string> getMatInfo(const std::string& name) const;

private:
    template<template<class, class> class T, typename V>
    std::optional<V> fetchValue(const T<std::string, V>& map,
                                const std::string& name) const {
        auto it = map.find(name);
        if (it == map.end())
            return {};
        return it->second;
    }

    template<typename K, typename V>
    using map = std::unordered_map<K, V>;

    map<std::string, float> _floats;
    map<std::string, Color> _rgb;
    map<std::string, std::string> _textures;
    map<std::string, std::string> _matInfo;
};

} // namespace pbr

#endif