#include <SceneLoader.h>

#include <Scene.h>
#include <Camera.h>
#include <Light.h>
#include <Mesh.h>

using namespace pbr;
using namespace std::literals;

namespace {

struct TagInfo {
    Tag tag = Tag::Unknown;
    std::vector<std::string> attrs{"name"s, "value"s};
    bool topLevel = false;
};

const std::unordered_map<std::string, TagInfo> TagInfoMap{
    {"float",       {Tag::Float}                         },
    {"string",      {Tag::String}                        },
    {"vec3",        {Tag::Vec3}                          },
    {"rgb",         {Tag::Rgb}                           },
    {"transform",   {Tag::Transform, {"name"s}}          },
    {"scene",       {Tag::Scene, {}, true}               },
    {"camera",      {Tag::Camera, {"type"s}, true}       },
    {"skybox",      {Tag::Skybox, {}, true}              },
    {"light",       {Tag::Light, {"type"s}, true}        },
    {"material",    {Tag::Material, {"type"s}}           },
    {"texture",     {Tag::Texture}                       },
    {"mesh",        {Tag::Mesh, {"type"s}, true}         },
    {"rotation",    {Tag::Rotation, {"axis"s, "value"s}} },
    {"scale",       {Tag::Scale, {"value"s}}             },
    {"lookat",      {Tag::LookAt, {"eye"s, "up"s, "at"s}}},
    {"translation", {Tag::Translation, {"value"s}}       }
};

std::optional<TagInfo> GetTagFromString(const std::string& tagName) {
    auto it = TagInfoMap.find(tagName);
    if (it != TagInfoMap.end())
        return it->second;
    return std::nullopt;
}

void CheckMandatoryAttrs(const XMLElement& xmlEl, std::span<const std::string> attrs) {
    for (const auto& attr : attrs) {
        if (!xmlEl.hasAttr(attr))
            THROW("Missing attribute {} in element {}.", attr, xmlEl.name());
    }
}

} // namespace

std::unique_ptr<Scene> SceneLoader::parse(const fs::path& filePath) {
    XMLDoc doc;
    auto root = doc.open(filePath);
    if (!root) {
        LOG_ERROR("Error loading scene XML file");
        return nullptr;
    }

    parentDir = filePath.parent_path();

    parseXml(root.value());
    instantiateScene();

    return std::move(scene);
}

void SceneLoader::instantiateScene() {
    scene = std::make_unique<pbr::Scene>();

    for (auto& entry : maps) {
        switch (entry.tag) {
        case Tag::Camera:
            scene->addCamera(CreateCamera(entry.map));
            break;
        case Tag::Light:
            scene->addLight(CreateLight(entry.map));
            break;
        case Tag::Skybox:
            entry.map.insert("parentdir", parentDir.string());
            skyboxes.emplace_back(CreateSkybox(entry.map));
            break;
        case Tag::Mesh:
            entry.map.insert("parentdir", parentDir.string());
            scene->addShape(CreateMesh(entry.map));
            break;
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

void SceneLoader::parseXml(const XMLElement& xmlEl, MapEntry* entry) {
    auto elName = xmlEl.name();
    auto tag = GetTagFromString(elName);
    if (!tag.has_value())
        THROW("Unknown element {}.", elName);

    TagInfo tagInfo = *tag;
    if (tagInfo.topLevel && entry)
        THROW("<{}> can't be used as a nested element.", elName);
    else if (!tagInfo.topLevel && !entry)
        THROW("<{}> can't be used as a top level element in the scene description.",
              elName);

    CheckMandatoryAttrs(xmlEl, tagInfo.attrs);

    switch (tagInfo.tag) {
    case Tag::Scene:
        for (const auto& el : xmlEl.children())
            parseXml(el);
        break;
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
    case Tag::Material:
        {
            QCHECK(entry->tag == Tag::Mesh);
            auto newEntry = parseChildren(Tag::Material, xmlEl);
            entry->map.insert<ParameterMap*>("material", &(newEntry->map));
        }
        break;
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
    case Tag::Transform:
        {
            auto name = xmlEl.attr<std::string>("name");
            auto val = xmlEl.attr<Mat4>("value"); // Some value or identity mat

            // Push starting matrix and accumulate transformations
            stack.push_back(val);

            for (const auto& el : xmlEl.children())
                parseXml(el);

            entry->map.insert(name, stack.back());
            stack.pop_back();
        }
        break;
    case Tag::Scale:
        {
            auto s = xmlEl.attr<Vec3>("value");
            multStack(Scale(s));
        }
        break;
    case Tag::Translation:
        {
            auto t = xmlEl.attr<Vec3>("value");
            multStack(Translation(t));
        }
        break;
    case Tag::Rotation:
        {
            auto axis = xmlEl.attr<Vec3>("axis");
            auto degs = xmlEl.attr<float>("value");
            multStack(RotationAxis(Radians(degs), axis));
        }
        break;
    case Tag::LookAt:
        {
            auto eye = xmlEl.attr<Vec3>("eye");
            entry->map.insert("eye", eye);

            auto at = xmlEl.attr<Vec3>("at");
            entry->map.insert("at", at);

            auto up = xmlEl.attr<Vec3>("up");
            entry->map.insert("up", up);
        }
        break;
    default:
        THROW("Unknown element <{}>.", elName);
        break;
    };
}