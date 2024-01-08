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
    SceneLoader() { stack.push_back(Mat4{}); }

    std::unique_ptr<Scene> parse(const fs::path& filePath);

    std::vector<Skybox> getSkyboxes() const { return skyboxes; }

private:
    struct MapEntry {
        Tag tag = Tag::Unknown;
        ParameterMap map = {};
    };

    void instantiateScene();

    MapEntry* createMapEntry(Tag tag) {
        maps.emplace_back(tag);
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

    fs::path parentDir = {};

    std::vector<Mat4> stack = {};
    std::vector<MapEntry> maps = {};

    std::vector<Skybox> skyboxes;
    std::unique_ptr<Scene> scene = nullptr;
};

} // namespace pbr

#endif // __LOADER_H__