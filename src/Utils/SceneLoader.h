#ifndef __PBR_LOADER_H__
#define __PBR_LOADER_H__

#include <PBR.h>
#include <PBRMath.h>
#include <Spectrum.h>

#include <XMLDoc.h>
#include <Transform.h>
#include <Scene.h>

#include <memory>

#include <ParameterMap.h>

using namespace pbr::math;
using namespace std::literals;

namespace pbr {

class Scene;

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

// enum class Tag {
//     Unknown,
//     Float,
//     String,
//     Vec3,
//     Rgb,
//     Transform,
//     Rotation,
//     Scale,
//     Translation,
//     LookAt,
//     Scene,
//     Camera,
//     Skybox,
//     Light,
//     Material,
//     Texture,
//     Mesh
// };

// struct TagInfo {
//     Tag tag;
//     std::vector<std::string> attrs;
// };

// // clang-format off
// const std::unordered_map<std::string, Tag> TagMap{
//     {"float", Tag::Float},       {"string", Tag::String},       {"vec3", Tag::Vec3},
//     {"rgb", Tag::Rgb},           {"transform", Tag::Transform}, {"scene", Tag::Scene},
//     {"camera", Tag::Camera},     {"skybox", Tag::Skybox},       {"light", Tag::Light},
//     {"material", Tag::Material}, {"texture", Tag::Texture},     {"mesh", Tag::Mesh},
//     {"rotation", Tag::Rotation}, {"scale", Tag::Scale},         {"lookat",
//     Tag::LookAt},
//     {"translation", Tag::Translation}};
// // clang-format on

// inline std::optional<Tag> GetTagFromString(const std::string& tagName) {
//     auto it = TagMap.find(tagName);
//     if (it != TagMap.end())
//         return it->second;
//     return std::nullopt;
// }

class SceneLoader {
public:
    SceneLoader() { stack.push_back(Mat4{}); }

    std::unique_ptr<Scene> parse(const std::string& fileName);

private:
    void instantiateScene();

    struct MapEntry {
        Tag tag = Tag::Unknown;
        ParameterMap map = {};
    };

    MapEntry* createMapEntry(Tag tag) {
        maps.push_back({tag, {}});
        return &maps.back();
    }

    template<typename T>
    void parseSimple(const XMLElement xmlEl, MapEntry& entry) {
        auto name = xmlEl.attr<std::string>("name");
        auto val = xmlEl.attr<T>("value");
        entry.map.insert(name, val);
    }

    void parseXml(const XMLElement& xmlEl, MapEntry* entry = nullptr);
    MapEntry* parseChildren(Tag tag, const XMLElement& xmlEl);

    void multStack(const Mat4& mat);

    std::vector<Mat4> stack = {};
    std::vector<MapEntry> maps = {};

    std::unique_ptr<Scene> scene = nullptr;
};

} // namespace pbr

#endif // __LOADER_H__