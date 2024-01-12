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
    {"uint",        {Tag::UInt}                          },
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

    scene = std::make_unique<pbr::Scene>();

    ParseContext ctx{.tag = Tag::Scene};
    parseXml(root.value(), ctx);

    return std::move(scene);
}

void SceneLoader::instantiate(ParseContext& ctx) {
    switch (ctx.tag) {
    case Tag::Camera:
        scene->addCamera(CreateCamera(ctx.entry));
        break;
    case Tag::Light:
        scene->addLight(CreateLight(ctx.entry));
        break;
    case Tag::Skybox:
        skyboxes.emplace_back(CreateSkybox(ctx.entry));
        break;
    case Tag::Mesh:
        ctx.entry.insert("material", &ctx.material);
        scene->addShape(CreateMesh(ctx.entry));
        break;
    default:
        break;
    }
}

void SceneLoader::parseChildren(const XMLElement& xmlEl, ParseContext& ctx) {
    auto type = xmlEl.attr<std::string>("type");
    ctx.entry.insert("type", type);
    for (const auto& el : xmlEl.children())
        parseXml(el, ctx);
}

void SceneLoader::parseAndInstantiate(Tag tag, const XMLElement& xmlEl) {
    ParseContext ctx{.tag = tag};
    if (tag == Tag::Mesh || tag == Tag::Skybox)
        ctx.entry.insert("parentDir", parentDir.string());
    parseChildren(xmlEl, ctx);
    instantiate(ctx);
}

void SceneLoader::parseXml(const XMLElement& xmlEl, ParseContext& ctx) {
    auto elName = xmlEl.name();
    auto tag = GetTagFromString(elName);
    if (!tag.has_value())
        THROW("Unknown element {}.", elName);

    TagInfo tagInfo = *tag;
    if (tagInfo.topLevel && ctx.tag != Tag::Scene)
        THROW("<{}> can't be used as a nested element.", elName);
    else if (!tagInfo.topLevel && ctx.tag == Tag::Scene)
        THROW("<{}> can't be used as a top level element in the scene description.",
              elName);

    CheckMandatoryAttrs(xmlEl, tagInfo.attrs);

    switch (tagInfo.tag) {
    case Tag::Scene:
        QCHECK(ctx.tag == Tag::Scene);
        for (const auto& el : xmlEl.children())
            parseXml(el, ctx);
        break;
    case Tag::String:
    case Tag::Texture:
        parseSimple<std::string>(xmlEl, ctx.entry);
        break;
    case Tag::UInt:
        parseSimple<unsigned int>(xmlEl, ctx.entry);
        break;
    case Tag::Float:
        parseSimple<float>(xmlEl, ctx.entry);
        break;
    case Tag::Vec3:
        parseSimple<Vec3>(xmlEl, ctx.entry);
        break;
    case Tag::Rgb:
        parseSimple<Color>(xmlEl, ctx.entry);
        break;
    case Tag::Material:
        {
            QCHECK(ctx.tag == Tag::Mesh);
            ParseContext matCtx{.tag = Tag::Material};
            parseChildren(xmlEl, matCtx);
            matCtx.entry.insert("parentdir", parentDir.string());
            ctx.material = std::move(matCtx.entry);
        }
        break;
    case Tag::Camera:
    case Tag::Light:
    case Tag::Skybox:
    case Tag::Mesh:
        parseAndInstantiate(tagInfo.tag, xmlEl);
        break;
    case Tag::Transform:
        {
            ParseContext tformCtx{.tag = Tag::Transform};
            auto name = xmlEl.attr<std::string>("name");
            auto val = xmlEl.attr<Mat4>("value"); // Some value or identity mat

            tformCtx.transform = val;
            for (const auto& el : xmlEl.children())
                parseXml(el, tformCtx);
            ctx.entry.insert(name, tformCtx.transform);
        }
        break;
    case Tag::Scale:
        {
            auto s = xmlEl.attr<Vec3>("value");
            ctx.transform = Scale(s) * ctx.transform;
        }
        break;
    case Tag::Translation:
        {
            auto t = xmlEl.attr<Vec3>("value");
            ctx.transform = Translation(t) * ctx.transform;
        }
        break;
    case Tag::Rotation:
        {
            auto axis = xmlEl.attr<Vec3>("axis");
            auto degs = xmlEl.attr<float>("value");
            ctx.transform = RotationAxis(Radians(degs), axis) * ctx.transform;
        }
        break;
    case Tag::LookAt:
        {
            auto eye = xmlEl.attr<Vec3>("eye");
            ctx.entry.insert("eye", eye);

            auto at = xmlEl.attr<Vec3>("at");
            ctx.entry.insert("at", at);

            auto up = xmlEl.attr<Vec3>("up");
            ctx.entry.insert("up", up);
        }
        break;
    default:
        THROW("Unknown element <{}>.", elName);
        break;
    };
}