#include <SceneLoader.h>

#include <Scene.h>
#include <Camera.h>
#include <Light.h>

using namespace pbr;

struct TagInfo {
    Tag tag;
    std::vector<std::string> attrs;
};

// clang-format off
const std::unordered_map<std::string, Tag> TagMap{
    {"float", Tag::Float},       {"string", Tag::String},       {"vec3", Tag::Vec3},
    {"rgb", Tag::Rgb},           {"transform", Tag::Transform}, {"scene", Tag::Scene},
    {"camera", Tag::Camera},     {"skybox", Tag::Skybox},       {"light", Tag::Light},
    {"material", Tag::Material}, {"texture", Tag::Texture},     {"mesh", Tag::Mesh},
    {"rotation", Tag::Rotation}, {"scale", Tag::Scale},         {"lookat", Tag::LookAt},
    {"translation", Tag::Translation}};
// clang-format on

std::optional<Tag> GetTagFromString(const std::string& tagName) {
    auto it = TagMap.find(tagName);
    if (it != TagMap.end())
        return it->second;
    return std::nullopt;
}

std::unique_ptr<Scene> SceneLoader::parse(const std::string& fileName) {
    XMLDoc doc;

    auto root = doc.open(fileName);
    if (!root) {
        LOG_ERROR("Error loading scene XML file");
        return nullptr;
    }

    parseXml(root.value());
    instantiateScene();

    return std::move(scene);
}

void SceneLoader::instantiateScene() {
    using enum Tag;

    scene = std::make_unique<pbr::Scene>();

    for (const auto& entry : maps) {
        switch (entry.tag) {
        case Camera:
            scene->addCamera(CreateCamera(entry.map));
            break;
        case Light:
            scene->addLight(CreateLight(entry.map));
            break;
        case Skybox:

        default:
            break;
        }
    }
}

SceneLoader::MapEntry* SceneLoader::parseChildren(Tag tag, const XMLElement& xmlEl) {
    auto newEntry = createMapEntry(tag);
    auto type = xmlEl.attr<std::string>("type");
    newEntry->map.insert("type", type);
    for (const auto& el : xmlEl.children())
        parseXml(el, newEntry);
    return newEntry;
}

void SceneLoader::multStack(const Mat4& mat) {
    auto idx = stack.size() - 1;
    stack[idx] = mat * stack[idx];
}

// clang-format off
void SceneLoader::parseXml(const XMLElement& xmlEl, MapEntry* entry) {
    auto tag = GetTagFromString(xmlEl.name());
    if (!tag.has_value()) {
        LOG_ERROR("Unknown tag");
        return;
    }

    switch (*tag) {
    case Tag::Scene: {
        CHECK(entry == nullptr);
        for (const auto& el : xmlEl.children())
            parseXml(el);
    } break;
    case Tag::String:
    case Tag::Texture:
        parseSimple<std::string>(xmlEl, *entry);
        break;
    case Tag::Float:
        parseSimple<float>(xmlEl, *entry);
        break;
    case Tag::Vec3:
        parseSimple<Vec3>(xmlEl, *entry);
        break;
    case Tag::Rgb:
        parseSimple<Color>(xmlEl, *entry);
        break;
    case Tag::Material: {
        QCHECK(entry->tag == Tag::Mesh);
        auto newEntry = parseChildren(Tag::Material, xmlEl);
        entry->map.insert<ParameterMap*>("material", &newEntry->map);
    } break;
    case Tag::Camera:
        parseChildren(Tag::Camera, xmlEl);
        break;
    case Tag::Light:
        parseChildren(Tag::Light, xmlEl);
        break;
    case Tag::Skybox:
        parseChildren(Tag::Skybox, xmlEl);
        break;
    case Tag::Mesh:
        parseChildren(Tag::Mesh, xmlEl);
        break;
    case Tag::Transform: {
        auto name = xmlEl.attr<std::string>("name");
        auto val = xmlEl.attr<Mat4>("value"); // Some value or identity mat

        // Push starting matrix and accumulate transformations
        stack.push_back(val);

        for (const auto& el : xmlEl.children())
            parseXml(el);

        entry->map.insert(name, stack.back());
        stack.pop_back();
    } break;
    case Tag::Scale: {
        CHECK(xmlEl.hasAttr("value"));
        auto s = xmlEl.attr<Vec3>("value");
        multStack(scale(s));
    } break;
    case Tag::Translation: {
        CHECK(xmlEl.hasAttr("value"));
        auto t = xmlEl.attr<Vec3>("value");
        multStack(translation(t));
    } break;
    case Tag::Rotation: {
        CHECK(xmlEl.hasAttrs(std::vector{"axis"s, "value"s}));
        auto axis = xmlEl.attr<Vec3>("axis");
        auto degs = xmlEl.attr<float>("value");
        multStack(rotationAxis(radians(degs), axis));
    } break;

    case Tag::LookAt: {
        CHECK(xmlEl.hasAttrs(std::vector{"eye"s, "at"s, "up"s}));
        auto eye = xmlEl.attr<Vec3>("eye");
        entry->map.insert("eye", eye);

        auto at = xmlEl.attr<Vec3>("at");
        entry->map.insert("at", at);

        auto up = xmlEl.attr<Vec3>("up");
        entry->map.insert("up", up);

        for (const auto& el : xmlEl.children())
            parseXml(el);
    } break;
    default:
        break;
    };
}
// clang-format on