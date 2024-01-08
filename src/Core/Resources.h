#ifndef __PBR_RESOURCES_H__
#define __PBR_RESOURCES_H__

#include <PBR.h>

#define Resource Resources::inst()

namespace pbr {

class Geometry;
class Program;
class Texture;

template<class KT, class T>
using map = std::unordered_map<KT, T>;

class Resources {
public:
    static Resources& inst() {
        static Resources _inst;
        return _inst;
    }

    template<typename T>
    void add(const std::string& name, sref<T> res) {
        if constexpr (std::is_same_v<Texture, T>)
            addResource(name, std::move(res), _textures);
        else if constexpr (std::is_same_v<Program, T>)
            addResource(name, std::move(res), _shaders);
        else if constexpr (std::is_same_v<Geometry, T>)
            addResource(name, std::move(res), _geometry);
    }

    template<typename T>
    sref<T> get(const std::string& name) {
        if constexpr (std::is_same_v<Texture, T>)
            return getResource(name, _textures);
        else if constexpr (std::is_same_v<Program, T>)
            return getResource(name, _shaders);
        else if constexpr (std::is_same_v<Geometry, T>)
            return getResource(name, _geometry);
    }

private:
    template<typename T>
    void addResource(const std::string& name, sref<T> res,
                     map<std::string, sref<T>>& resMap) {
        resMap.emplace(name, std::move(res));
    }

    template<typename T>
    sref<T> getResource(const std::string& name,
                        const map<std::string, sref<T>>& resMap) const {
        auto it = resMap.find(name);
        if (it != resMap.end())
            return it->second;
        return nullptr;
    }

    Resources() = default;

    map<std::string, sref<Geometry>> _geometry;
    map<std::string, sref<Program>> _shaders;
    map<std::string, sref<Texture>> _textures;
};

} // namespace pbr

#endif