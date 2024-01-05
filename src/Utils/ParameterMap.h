#ifndef __PBR_PARAMETERMAP_H__
#define __PBR_PARAMETERMAP_H__

#include <PBR.h>
#include <PBRMath.h>
#include <Spectrum.h>

#include <variant>

using namespace pbr::math;
using namespace std::literals;

namespace pbr {

class ParameterMap;

using ParameterType = std::variant<float, std::string, Vec3, Color, Mat4, ParameterMap*>;

class ParameterMap {
public:
    template<typename T>
    void insert(const std::string& name, T param) {
        map.emplace(name, std::move(param));
    }

    template<typename T>
    T lookup(const std::string& name, T defVal) const {
        auto opt = lookup<T>(name);
        if (!opt.has_value())
            return defVal;
        return *opt;
    }

    template<typename T>
    std::optional<T> lookup(const std::string& name) const {
        auto it = map.find(name);
        if (it == map.end())
            return std::nullopt;

        auto param = it->second;
        if (std::holds_alternative<T>(param))
            return std::get<T>(param);

        return std::nullopt;
    }

private:
    std::unordered_map<std::string, ParameterType> map{};
};

} // namespace pbr

#endif // __PBR_PARAMETERMAP_H__