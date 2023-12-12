#ifndef __PBR_RI_H__
#define __PBR_RI_H__

#include <glad/glad.h>

#include <PBR.h>
#include <PBRMath.h>
#include <Image.h>
#include <Buffer.h>

#include <span>

#define RHI RenderInterface::get()

using namespace pbr::math;

namespace pbr {

class Geometry;
class TexSampler;
class Texture;
class Program;
class ShaderSource;

template<class T>
using vec = std::vector<T>;

enum SkyboxUniform { ENV_MAP = 1 };

struct RHIGeometry {
    RRID buffer;
    sref<Geometry> geo;
};

struct RHIVertArray {
    GLuint id;
    GLsizei numIndices;
    GLsizei numVertices;
    vec<GLuint> buffers;
};

struct RHIProgram {
    GLuint id;
};

struct RHITexture {
    GLuint id;
    GLenum target;
    GLenum intFormat;
    GLenum format;
    GLenum pType;
    sref<Texture> tex;
};

struct RHIBuffer {
    GLuint id;
    GLenum target;
};

enum BufferType { BUFFER_VERTEX = 0, BUFFER_INDEX = 1, BUFFER_SHARED = 2 };

enum BufferUsage { STATIC = 0, STREAM = 1, DYNAMIC = 2 };

enum AttribType : uint32 {
    ATTRIB_BYTE = 0,
    ATTRIB_SHORT = 1,
    ATTRIB_UINT = 2,
    ATTRIB_FLOAT = 3
};

struct BufferLayoutEntry {
    uint32 index;
    uint32 numElems;
    AttribType type;
    std::size_t stride;
    std::size_t offset;
};

struct BufferLayout {
    uint32 numEntries;
    BufferLayoutEntry* entries;
};

class RenderInterface {
public:
    static RenderInterface& get();

    void initialize();

    /* ===================================================================================
            Textures
    =====================================================================================*/
    RRID createTextureImmutable(const Image& img, const TexSampler& sampler);

    RRID createTexture(const Image& img, const TexSampler& sampler);
    RRID createTexture(ImageType type, ImageFormat fmt, uint32 width, uint32 height,
                       uint32 depth, const TexSampler& sampler);

    RRID createCubemap(const Cubemap& cube, const TexSampler& sampler);

    sref<Texture> getTexture(RRID id);
    bool readTexture(RRID id, Image& img);
    bool readCubemap(RRID id, Cubemap& cube);

    void generateMipmaps(RRID id);
    void setTextureData(RRID id, uint32 level, const void* pixels);
    void deleteTexture(RRID id);

    void bindTexture(RRID id);
    void bindTexture(uint32 slot, RRID id);
    void bindTextures(uint32 first, uint32 size, std::span<const uint32> texs);

    std::vector<uint32> queryTexHandles(std::vector<RRID> texs);
    uint32 queryTexHandle(RRID tex);
    /* ===================================================================================
            Shaders
    =====================================================================================*/
    uint32 compileShader(const ShaderSource& source);
    bool deleteShader(const ShaderSource& source);
    RRID linkProgram(const Program& shader);
    std::string getProgramError(int shader);

    void useProgram(RRID id);

    void setFloat(const std::string& name, float val);
    void setVector3(const std::string& name, const Vec3& vec);
    void setVector4(const std::string& name, const Vec4& vec);
    void setMatrix3(const std::string& name, const Mat3& mat);
    void setMatrix4(const std::string& name, const Mat4& mat);
    void setSampler(const std::string& name, uint32 id);

    void setFloat(int32 loc, float val);
    void setVector3(int32 loc, const Vec3& vec);
    void setVector4(int32 loc, const Vec4& vec);
    void setMatrix3(int32 loc, const Mat3& mat);
    void setMatrix4(int32 loc, const Mat4& mat);
    void setSampler(int32 loc, uint32 id);

    void setBufferBlock(const std::string& name, uint32 binding);

    int32 uniformLocation(RRID id, const std::string& name);
    uint32 uniformBlockLocation(RRID id, const std::string& name);

    /* ===================================================================================
            Geometry
    =====================================================================================*/
    void drawGeometry(RRID id);
    RRID uploadGeometry(Geometry& geo);

    /* ===================================================================================
             Buffers
    =====================================================================================*/
    RRID createVertexArray();
    bool deleteVertexArray(RRID id);

    RRID createBufferImmutable(BufferType type, BufferUsage usage, std::size_t size,
                               const void* data);

    RRID createBuffer(BufferType type, BufferUsage usage, std::size_t size, const void* data);
    void bindBufferBase(RRID id, uint32 index);
    void bindBufferRange(RRID id, uint32 index, std::size_t offset, std::size_t size);
    void setBufferLayout(RRID id, uint32 idx, AttribType type, uint32 numElems,
                         uint32 stride, std::size_t offset);
    void setBufferLayout(RRID id, const BufferLayout& layout);
    bool updateBuffer(RRID id, std::size_t size, const void* data);
    bool deleteBuffer(RRID id);


    bool isOpenGLError();
    void checkOpenGLError(const std::string& error);

    std::unique_ptr<Image> getImage(int32 x, int32 y, int32 w, int32 h) const;

private:
    RenderInterface() = default;

    void initMainShaders();
    void initCommonMeshes();

    RRID _currProgram;

    vec<RHIVertArray> _vertArrays;
    vec<RHIBuffer> _buffers;
    vec<RHIProgram> _programs;
    vec<RHITexture> _textures;
};

} // namespace pbr

#endif