#ifndef PBR_RI_H
#define PBR_RI_H

#include <glad/glad.h>

#include <PBR.h>
#include <PBRMath.h>
#include <Texture.h>

#include <span>
#include <filesystem>

#define RHI RenderInterface::get()

using namespace pbr::math;
namespace fs = std::filesystem;

namespace pbr {

class VertexArrays;
class Geometry;
class Image;

enum class CullMode : int { Front = 0, Back  = 1};

consteval bool EnableConversion(CullMode);

class RenderInterface {
public:
    static RenderInterface& get() {
        static RenderInterface _inst;
        return _inst;
    }

    void initialize();

    void setCullFace(CullMode mode);

    void bindTextures(unsigned int first, unsigned int size,
                      std::span<const unsigned int> texs);

    void setFloat(int loc, float val);
    void setVector3(int loc, const Vec3& vec);
    void setVector4(int loc, const Vec4& vec);
    void setMatrix3(int loc, const Mat3& mat);
    void setMatrix4(int loc, const Mat4& mat);
    void setSampler(int loc, unsigned int id);

    bool isOpenGLError();
    void checkOpenGLError(const std::string& error);

    std::size_t alignUniformBuffer(std::size_t structSize) {
        return (structSize + uniformBufferAlignment - 1) & ~(uniformBufferAlignment - 1);
    }

private:
    RenderInterface() = default;

    void initOpenGLState();
    void initMainShaders();

    GLint uniformBufferAlignment;
};

std::unique_ptr<VertexArrays> CreateVertexArrays(const Geometry& geo);

std::shared_ptr<Texture> CreateNamedTexture(const std::string& name, const Image& img,
                                            const TexSampler& sampler = {});
std::shared_ptr<Texture> CreateNamedTexture(const std::string& name, const fs::path& path,
                                            const TexSampler& sampler = {});
std::shared_ptr<Texture> CreateNamedCubemap(const std::string& name, const fs::path& path,
                                            const TexSampler& sampler = {});

Image ReadMainFramebuffer(int x, int y, int width, int height);

} // namespace pbr

#endif