#ifndef __PBR_PMAP_H__
#define __PBR_PMAP_H__

#include <PBR.h>
#include <PBRMath.h>

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
    void setRGB(const std::string& name, const math::Vec3& value);
    void setTexture(const std::string& name, const std::string& value);
    void setMatInfo(const std::string& name, const std::string& value);

    std::optional<float> getFloat(const std::string& name) const;
    std::optional<math::Vec3> getRGB(const std::string& name) const;
    std::optional<std::string> getTexture(const std::string& name) const;
    std::optional<std::string> getMatInfo(const std::string& name) const;

private:
    std::unordered_map<std::string, float> _floats;
    std::unordered_map<std::string, math::Vec3> _rgb;
    std::unordered_map<std::string, std::string> _textures;
    std::unordered_map<std::string, std::string> _matInfo;
};

} // namespace pbr

#endif