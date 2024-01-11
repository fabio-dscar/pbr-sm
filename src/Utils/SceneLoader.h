#ifndef __PBR_LOADER_H__
#define __PBR_LOADER_H__

#include <PBR.h>
#include <PBRMath.h>
#include <Spectrum.h>

#include <XMLDoc.h>
#include <Transform.h>
#include <Scene.h>
#include <Skybox.h>

#include <memory>
#include <filesystem>

#include <ParameterMap.h>

namespace fs = std::filesystem;

namespace pbr {

enum class Tag {
    Unknown,
    Float,
    String,
    Vec3,
    Rgb,
    Transform,
    Rotation,
    Scale,
    Translation,
    LookAt,
    Scene,
    Camera,
    Skybox,
    Light,
    Material,
    Texture,
    Mesh
};

class SceneLoader {
public:
    std::unique_ptr<Scene> parse(const fs::path& filePath);

    std::vector<Skybox> getSkyboxes() const { return skyboxes; }

private:
    struct ParseContext {
        Tag tag = Tag::Unknown;
        ParameterMap entry = {};
        ParameterMap material = {};
        Mat4 transform = {};
    };

    void instantiate(ParseContext& ctx);

    template<typename T>
    void parseSimple(const XMLElement xmlEl, ParameterMap& map) const {
        auto name = xmlEl.attr<std::string>("name");
        auto val = xmlEl.attr<T>("value");
        map.insert(name, val);
    }

    void parseXml(const XMLElement& xmlEl, ParseContext& ctx);
    void parseChildren(const XMLElement& xmlEl, ParseContext& ctx);
    void parseAndInstantiate(Tag tag, const XMLElement& xmlEl);

    fs::path parentDir = {};

    std::vector<Skybox> skyboxes;
    std::unique_ptr<Scene> scene = nullptr;
};

} // namespace pbr

#endif // __LOADER_H__