#include <RenderInterface.h>

#include <Resources.h>
#include <Geometry.h>
#include <Texture.h>
#include <Buffer.h>
#include <VertexArrays.h>
#include <Shader.h>
#include <PBRMaterial.h>
#include <Utils.h>

using namespace pbr;
using namespace pbr::math;
using namespace pbr::util;
using namespace std::literals;

namespace {

enum SkyboxUniform { ENV_MAP = 1 };

void BindNamedTexture(const std::string& name, unsigned int texUnit) {
    glActiveTexture(GL_TEXTURE0 + texUnit);
    auto handle = Resource.get<Texture>(name)->id();
    glBindTextures(texUnit, 1, &handle);
}

} // namespace

// clang-format off
void RenderInterface::initialize() {
    initOpenGLState();

    // Create auxiliary 1x1 textures
    ImageFormat fmt{.pFmt = PixelFormat::U8, .width = 1, .height = 1, .nChannels = 3};
    CreateNamedTexture("null", Image{fmt, {0, 0, 0}});
    CreateNamedTexture("white", Image{fmt, {1, 1, 1}});
    CreateNamedTexture("planar", Image{fmt, {0.5, 0.5, 1}}); // [0 0 1] normal

    CreateNamedTexture("brdf", "PBR/brdf.img", 
                       {.s = Wrap::ClampEdge, .t = Wrap::ClampEdge});

    // Create common meshes
    Resource.add<Geometry>("unitCube", genUnitCube());
    Resource.add<Geometry>("unitSphere", genUnitSphere(128, 64));
    Resource.add<Geometry>("unitQuad", genUnitQuad());

    initMainShaders();
}
// clang-format on

void RenderInterface::initOpenGLState() {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_TRUE);
    glDepthRange(0.0, 1.0);
    glClearDepth(1.0);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    glEnable(GL_MULTISAMPLE);

    glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &uniformBufferAlignment);
}

void RenderInterface::initMainShaders() {
    // PBR shader
    auto defines = std::vector{"MULTISCATTERING"s, "HAS_CLEARCOAT"s};
    auto pbrSources = std::vector{"pbr.vs"s, "pbr.fs"s};
    auto pbrProg = CompileAndLinkProgram("pbr", pbrSources, defines);

    // Set fixed sampler uniforms
    using pbr::PBRUniform;
    pbrProg->setSampler(DIFFUSE_MAP, 1);
    pbrProg->setSampler(NORMAL_MAP, 2);
    pbrProg->setSampler(METALLIC_MAP, 3);
    pbrProg->setSampler(ROUGHNESS_MAP, 4);
    pbrProg->setSampler(OCCLUSION_MAP, 5);
    pbrProg->setSampler(EMISSIVE_MAP, 6);
    pbrProg->setSampler(CLEARCOAT_NORMAL_MAP, 7);
    pbrProg->setSampler(ENV_IRRADIANCE_MAP, 8);
    pbrProg->setSampler(ENV_GGX_MAP, 9);
    pbrProg->setSampler(ENV_BRDF_MAP, 10);
    BindNamedTexture("brdf", 10);

    Resource.add<Program>("pbr", std::move(pbrProg));

    // Skybox shader
    auto skyBoxSources = std::vector{"skybox.vs"s, "skybox.fs"s};
    auto skyProg = CompileAndLinkProgram("skybox", skyBoxSources);

    skyProg->setSampler(ENV_MAP, 5);

    Resource.add<Program>("skybox", std::move(skyProg));
}

void RenderInterface::bindTextures(uint32 first, uint32 size,
                                   std::span<const uint32> texs) {
    glBindTextures(first, size, texs.data());
}

void RenderInterface::setFloat(int32 loc, float val) {
    glUniform1f(loc, val);
}

void RenderInterface::setVector3(int32 loc, const Vec3& vec) {
    glUniform3fv(loc, 1, (const GLfloat*)&vec);
}

void RenderInterface::setVector4(int32 loc, const Vec4& vec) {
    glUniform4fv(loc, 1, (const GLfloat*)&vec);
}

void RenderInterface::setMatrix3(int32 loc, const Mat3& mat) {
    glUniformMatrix3fv(loc, 1, GL_FALSE, (const GLfloat*)&mat);
}

void RenderInterface::setMatrix4(int32 loc, const Mat4& mat) {
    glUniformMatrix4fv(loc, 1, GL_FALSE, (const GLfloat*)&mat);
}

void RenderInterface::setSampler(int32 loc, uint32 id) {
    glUniform1i(loc, id);
}

std::unique_ptr<VertexArrays> pbr::CreateVertexArrays(const Geometry& geo) {
    auto& verts = geo.vertices();
    auto& indices = geo.indices();

    auto vertexArrays = std::make_unique<VertexArrays>();

    Buffer vertexBuffer{BufferType::Array, sizeof(Vertex) * verts.size(),
                        BufferFlag::None, verts.data()};

    BufferLayoutEntry entries[] = {
        {0, 3, AttribType::Float, sizeof(Vertex), offsetof(Vertex, position)},
        {1, 3, AttribType::Float, sizeof(Vertex), offsetof(Vertex, normal)  },
        {2, 2, AttribType::Float, sizeof(Vertex), offsetof(Vertex, uv)      },
        {3, 4, AttribType::Float, sizeof(Vertex), offsetof(Vertex, tangent) }
    };

    vertexArrays->addVertexBuffer(std::move(vertexBuffer), entries, 0, sizeof(Vertex),
                                  verts.size());

    if (indices.size() > 0) {
        Buffer elementBuffer = {BufferType::Element,
                                sizeof(unsigned int) * indices.size(), BufferFlag::None,
                                indices.data()};
        vertexArrays->addElementBuffer(std::move(elementBuffer), indices.size(),
                                       AttribType::UInt);
    }

    return vertexArrays;
}

void RenderInterface::checkOpenGLError(const std::string& error) {
    if (isOpenGLError()) {
        std::cerr << error << std::endl;
        std::cin.get();
        exit(EXIT_FAILURE);
    }
}

bool RenderInterface::isOpenGLError() {
    bool isError = false;
    GLenum errCode;
    // const GLubyte* errString;
    while ((errCode = glGetError()) != GL_NO_ERROR) {
        isError = true;
        // errString = gluErrorString(errCode);
        // std::cerr << "OpenGL ERROR [" << errString << "]." << std::endl;
        std::cerr << "OpenGL ERROR [" << errCode << "]." << std::endl;
    }
    return isError;
}

std::shared_ptr<Texture> pbr::CreateNamedTexture(const std::string& name,
                                                 const Image& img,
                                                 const TexSampler& sampler) {
    auto tex = std::make_shared<Texture>(img, sampler);
    Resource.add(name, tex);
    return tex;
}

std::shared_ptr<Texture> pbr::CreateNamedTexture(const std::string& name,
                                                 const fs::path& path,
                                                 const TexSampler& sampler) {
    return CreateNamedTexture(name, *LoadImage(path), sampler);
}

std::shared_ptr<Texture> pbr::CreateNamedCubemap(const std::string& name,
                                                 const fs::path& path,
                                                 const TexSampler& sampler) {
    auto cube = LoadCubemap(path);
    auto tex = std::make_shared<Texture>(*cube, sampler);
    Resource.add(name, tex);
    return tex;
}

Image pbr::ReadMainFramebuffer(int x, int y, int width, int height) {
    Image img{{PixelFormat::U8, width, height, 3}, 1};
    glReadPixels(x, y, width, height, GL_RGB, GL_UNSIGNED_BYTE, img.data());
    return img;
}