#include <RenderInterface.h>

#include <Geometry.h>
#include <Utils.h>

#include <Image.h>
#include <Texture.h>
#include <Resources.h>

#include <Renderer.h>
#include <Shader.h>
#include <PBRMaterial.h>

#include <cstring>
#include <cassert>

#include <ios>
#include <ranges>
#include <format>

using namespace pbr;
using namespace pbr::math;

const GLenum OGLShaderTypes[] = {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, GL_GEOMETRY_SHADER,
                                 GL_COMPUTE_SHADER};

const GLenum OGLBufferTargets[] = {GL_ARRAY_BUFFER, GL_ELEMENT_ARRAY_BUFFER,
                                   GL_UNIFORM_BUFFER};

const GLenum OGLAttrTypes[] = {GL_BYTE, GL_SHORT, GL_UNSIGNED_INT, GL_FLOAT};

const GLenum OGLBufferUsage[] = {GL_STATIC_DRAW, GL_STREAM_DRAW, GL_DYNAMIC_DRAW};

const GLenum OGLTexTargets[] = {GL_TEXTURE_1D,
                                GL_TEXTURE_2D,
                                GL_TEXTURE_3D,
                                GL_TEXTURE_CUBE_MAP,
                                GL_TEXTURE_CUBE_MAP_POSITIVE_X,
                                GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
                                GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
                                GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
                                GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
                                GL_TEXTURE_CUBE_MAP_NEGATIVE_Z};

const GLenum OGLTexSizedFormats[] = {0,

                                     // Unsigned formats
                                     GL_R8, GL_RG8, GL_RGB8, GL_RGBA8,

                                     GL_R16, GL_RG16, GL_RGB16, GL_RGBA16,

                                     // Signed IMGFMTs
                                     GL_R8I, GL_RG8I, GL_RGB8I, GL_RGBA8I,

                                     GL_R16I, GL_RG16I, GL_RGB16I, GL_RGBA16I,

                                     // Float IMGFMTs
                                     GL_R16F, GL_RG16F, GL_RGB16F, GL_RGBA16F,

                                     GL_R32F, GL_RG32F, GL_RGB32F, GL_RGBA32F,

                                     // Signed integer IMGFMTs
                                     GL_R16I, GL_RG16I, GL_RGB16I, GL_RGBA16I,

                                     GL_R32I, GL_RG32I, GL_RGB32I, GL_RGBA32I,

                                     // Unsigned integer IMGFMTs
                                     GL_R16UI, GL_RG16UI, GL_RGB16UI, GL_RGBA16UI,

                                     GL_R32UI, GL_RG32UI, GL_RGB32UI, GL_RGBA32UI,

                                     // Packed formats
                                     0, 0, 0, 0, 0, 0,

                                     // Depth IMGFMTs
                                     GL_DEPTH_COMPONENT16, GL_DEPTH_COMPONENT24,
                                     GL_DEPTH24_STENCIL8, GL_DEPTH_COMPONENT32,

                                     // Compressed IMGFMTs
                                     0, 0, 0, 0, 0, 0};

const GLenum OGLTexSizedPixelFormats[] = {
    0,

    // Unsigned IMGFMTs
    GL_R8, GL_RG8, GL_RGB8, GL_RGBA8,

    GL_R16, GL_RG16, GL_RGB16, GL_RGBA16,

    // Signed IMGFMTs
    GL_R8_SNORM, GL_RG8_SNORM, GL_RGB8_SNORM, GL_RGBA8_SNORM,

    GL_R16_SNORM, GL_RG16_SNORM, GL_RGB16_SNORM, GL_RGBA16_SNORM,

    // Float IMGFMTs
    GL_R16F, GL_RG16F, GL_RGB16F, GL_RGBA16F,

    GL_R32F, GL_RG32F, GL_RGB32F, GL_RGBA32F,

    // Signed integer IMGFMTs
    GL_RED, GL_RG, GL_RGB, GL_RGBA,

    GL_RED, GL_RG, GL_RGB, GL_RGBA,

    // Unsigned integer IMGFMTs
    GL_RED, GL_RG, GL_RGB, GL_RGBA,

    GL_RED, GL_RG, GL_RGB, GL_RGBA,

    // Packed IMGFMTs
    0, 0, 0, 0, 0, 0,

    // Depth IMGFMTs
    GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_DEPTH_STENCIL, GL_DEPTH_COMPONENT,

    // Compressed IMGFMTs
    0, 0, 0, 0, 0, 0};

const GLenum OGLTexPixelFormats[] = {0,

                                     // Unsigned IMGFMTs
                                     GL_RED, GL_RG, GL_RGB, GL_RGBA,

                                     GL_RED, GL_RG, GL_RGB, GL_RGBA,

                                     // Signed IMGFMTs
                                     GL_RED, GL_RG, GL_RGB, GL_RGBA,

                                     GL_RED, GL_RG, GL_RGB, GL_RGBA,

                                     // Float IMGFMTs
                                     GL_RED, GL_RG, GL_RGB, GL_RGBA,

                                     GL_RED, GL_RG, GL_RGB, GL_RGBA,

                                     // Signed integer IMGFMTs
                                     GL_RED, GL_RG, GL_RGB, GL_RGBA,

                                     GL_RED, GL_RG, GL_RGB, GL_RGBA,

                                     // Unsigned integer IMGFMTs
                                     GL_RED, GL_RG, GL_RGB, GL_RGBA,

                                     GL_RED, GL_RG, GL_RGB, GL_RGBA,

                                     // Packed IMGFMTs
                                     0, 0, 0, 0, 0, 0,

                                     // Depth IMGFMTs
                                     GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT,
                                     GL_DEPTH_STENCIL, GL_DEPTH_COMPONENT,

                                     // Compressed IMGFMTs
                                     0, 0, 0, 0, 0, 0};

const GLenum OGLTexPixelTypes[] = {GL_UNSIGNED_BYTE,
                                   GL_BYTE,
                                   GL_UNSIGNED_SHORT,
                                   GL_SHORT,
                                   GL_UNSIGNED_INT,
                                   GL_INT,
                                   GL_FLOAT,
                                   GL_HALF_FLOAT,
                                   GL_UNSIGNED_BYTE_3_3_2,
                                   GL_UNSIGNED_BYTE_2_3_3_REV,
                                   GL_UNSIGNED_SHORT_5_6_5,
                                   GL_UNSIGNED_SHORT_5_6_5_REV,
                                   GL_UNSIGNED_SHORT_4_4_4_4,
                                   GL_UNSIGNED_SHORT_4_4_4_4_REV,
                                   GL_UNSIGNED_SHORT_5_5_5_1,
                                   GL_UNSIGNED_SHORT_1_5_5_5_REV,
                                   GL_UNSIGNED_INT_8_8_8_8,
                                   GL_UNSIGNED_INT_8_8_8_8_REV,
                                   GL_UNSIGNED_INT_10_10_10_2,
                                   GL_UNSIGNED_INT_2_10_10_10_REV};

const GLenum OGLTexFilters[] = {GL_NEAREST,
                                GL_LINEAR,
                                GL_NEAREST_MIPMAP_NEAREST,
                                GL_LINEAR_MIPMAP_NEAREST,
                                GL_NEAREST_MIPMAP_LINEAR,
                                GL_LINEAR_MIPMAP_LINEAR};

const GLenum OGLTexWrapping[] = {GL_REPEAT, GL_MIRRORED_REPEAT, GL_CLAMP_TO_EDGE,
                                 GL_CLAMP_TO_BORDER};

using namespace pbr;
using namespace std::literals;

RenderInterface& RenderInterface::get() {
    static RenderInterface _inst;
    return _inst;
}

void RenderInterface::initMainShaders() {
    _programs.push_back({0});
    _currProgram = 0;

    // PBR shader
    auto defines = std::vector{"MULTISCATTERING"s};
    auto pbrSources = std::vector{"pbr.vs"s, "pbr.fs"s};
    auto pbrProg = CompileAndLinkProgram("pbr", pbrSources, defines);

    // Set fixed sampler uniforms
    RHI.useProgram(pbrProg->id());
    using pbr::PBRUniform;
    RHI.setSampler(DIFFUSE_MAP, 1);
    RHI.setSampler(NORMAL_MAP, 2);
    RHI.setSampler(METALLIC_MAP, 3);
    RHI.setSampler(ROUGHNESS_MAP, 4);
    RHI.setSampler(OCCLUSION_MAP, 5);
    RHI.setSampler(EMISSIVE_MAP, 6);
    RHI.setSampler(CLEARCOAT_NORMAL_MAP, 7);
    RHI.setSampler(ENV_IRRADIANCE_MAP, 8);
    RHI.setSampler(ENV_GGX_MAP, 9);
    RHI.setSampler(ENV_BRDF_MAP, 10);
    RHI.useProgram(0);

    Resource.addShader("pbr", std::move(pbrProg));

    // Skybox shader
    auto skyBoxSources = std::vector{"skybox.vs"s, "skybox.fs"s};
    auto skyProg = CompileAndLinkProgram("skybox", skyBoxSources);

    RHI.useProgram(skyProg->id());
    RHI.setSampler(ENV_MAP, 5);
    RHI.useProgram(0);

    Resource.addShader("skybox", std::move(skyProg));
}

void RenderInterface::initCommonMeshes() {
    auto cube = genUnitCube();
    uploadGeometry(*cube);
    Resource.addGeometry("unitCube", std::move(cube));

    auto sphere = genUnitSphere(36, 16);
    uploadGeometry(*sphere);
    Resource.addGeometry("unitSphere", std::move(sphere));

    auto quad = genUnitQuad();
    uploadGeometry(*quad);
    Resource.addGeometry("unitQuad", std::move(quad));
}

void RenderInterface::initialize() {
    // Craete null texture
    Image null;
    uint8 nullTex[3] = {0, 0, 0};
    null.loadImage(ImageFormat::IMGFMT_RGB8, 1, 1, 1, nullTex);
    RRID nullId = createTextureImmutable(null, {});
    Resource.addTexture("null", RHI.getTexture(nullId));

    Image white;
    uint8 whiteTex[3] = {255, 255, 255};
    white.loadImage(ImageFormat::IMGFMT_RGB8, 1, 1, 1, whiteTex);
    RRID whiteId = createTextureImmutable(white, {});
    Resource.addTexture("white", RHI.getTexture(whiteId));

    Image planar;
    uint8 planarTex[3] = {127, 127, 255};
    planar.loadImage(ImageFormat::IMGFMT_RGB8, 1, 1, 1, planarTex);
    RRID planarId = createTextureImmutable(planar, {});
    Resource.addTexture("planar", RHI.getTexture(planarId));

    // Load BRDF precomputation
    Image brdf;
    brdf.loadImage("PBR/brdf.img");
    RRID brdfId = createTextureImmutable(brdf, {});
    Resource.addTexture("brdf", RHI.getTexture(brdfId));

    initMainShaders();
    initCommonMeshes();
}

RRID RenderInterface::uploadGeometry(Geometry& geo) {
    auto& verts = geo.vertices();
    auto& indices = geo.indices();

    RRID resId = createVertexArray();

    RHIVertArray& vertArray = _vertArrays[resId];
    glBindVertexArray(vertArray.id);

    // Create VBOs for vertex data and indices
    RRID vboIds[2] = {0, 0};
    vboIds[0] = createBufferImmutable(BUFFER_VERTEX, BufferUsage::STATIC,
                                      sizeof(Vertex) * verts.size(), &verts[0]);

    BufferLayoutEntry entries[] = {
        {0, 3, ATTRIB_FLOAT, sizeof(Vertex), offsetof(Vertex, position)},
        {1, 3, ATTRIB_FLOAT, sizeof(Vertex), offsetof(Vertex, normal)},
        {2, 2, ATTRIB_FLOAT, sizeof(Vertex), offsetof(Vertex, uv)},
        {3, 4, ATTRIB_FLOAT, sizeof(Vertex), offsetof(Vertex, tangent)}};

    BufferLayout layout = {4, &entries[0]};
    setBufferLayout(vboIds[0], layout);

    if (indices.size() > 0) {
        vboIds[1] = createBufferImmutable(BUFFER_INDEX, BufferUsage::STATIC,
                                          sizeof(uint32) * indices.size(), &indices[0]);
        glVertexArrayElementBuffer(vertArray.id, _buffers[vboIds[1]].id);
    }

    // Associate created VBOs with the VAO
    vertArray.buffers.push_back(vboIds[0]);
    vertArray.buffers.push_back(vboIds[1]);

    vertArray.numVertices = (GLsizei)verts.size();
    vertArray.numIndices = (GLsizei)indices.size();

    glBindVertexArray(0);

    // Associate RRID of the VAO with the geometry
    geo.setRRID(resId);

    return resId;
}

void RenderInterface::drawGeometry(RRID id) {
    /*if (id < 0 || id >= _vertArrays.size())
        return; // Error

    RHIVertArray& vao = _vertArrays[id];
    if (vao.id == 0)
        return; // Error*/

    RHIVertArray& vao = _vertArrays[id];
    glBindVertexArray(vao.id);

    if (vao.numIndices > 0)
        glDrawElements(GL_TRIANGLES, vao.numIndices, GL_UNSIGNED_INT, 0);
    else
        glDrawArrays(GL_TRIANGLES, 0, vao.numVertices);

    glBindVertexArray(0);
}

RRID RenderInterface::createVertexArray() {
    RHIVertArray vertArray;

    glGenVertexArrays(1, &vertArray.id);

    RRID resId = _vertArrays.size();
    _vertArrays.push_back(vertArray);

    return resId;
}

bool RenderInterface::deleteVertexArray(RRID id) {
    if (id < 0)
        return false; // Error

    RHIVertArray vao = _vertArrays[id];
    if (vao.id != 0) {
        glDeleteVertexArrays(1, &vao.id);
        vao.id = 0;
        vao.buffers.clear();
        vao.numIndices = 0;
        return true;
    }

    return false;
}

RRID RenderInterface::createBufferImmutable(BufferType type, BufferUsage usage,
                                            size_t size, const void* data) {
    RHIBuffer buffer;
    buffer.target = OGLBufferTargets[type];

    GLbitfield flags = 0;
    if (usage == BufferUsage::DYNAMIC)
        flags = GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT;

    glCreateBuffers(1, &buffer.id);
    glNamedBufferStorage(buffer.id, size, data, flags);

    RRID resId = _buffers.size();
    _buffers.push_back(buffer);

    return resId;
}

RRID RenderInterface::createBuffer(BufferType type, BufferUsage usage, size_t size,
                                   const void* data) {
    RHIBuffer buffer;
    buffer.target = OGLBufferTargets[type];

    glGenBuffers(1, &buffer.id);
    glBindBuffer(buffer.target, buffer.id);
    glBufferData(buffer.target, size, data, OGLBufferUsage[usage]);
    // glBindBuffer(buffer.target, 0);

    RRID resId = _buffers.size();
    _buffers.push_back(buffer);

    return resId;
}

void RenderInterface::bindBufferBase(RRID id, uint32 index) {
    if (id < 0 || id >= _buffers.size())
        return; // Error

    RHIBuffer buffer = _buffers[id];

    glBindBuffer(buffer.target, buffer.id);
    glBindBufferBase(buffer.target, index, buffer.id);
    glBindBuffer(buffer.target, 0);
}

void RenderInterface::bindBufferRange(RRID id, uint32 index, std::size_t offset,
                                      std::size_t size) {
    RHIBuffer buffer = _buffers[id];

    glBindBuffer(buffer.target, buffer.id);
    glBindBufferRange(buffer.target, index, buffer.id, offset, size);
    glBindBuffer(buffer.target, 0);
}

void RenderInterface::setBufferLayout(RRID id, uint32 idx, AttribType type,
                                      uint32 numElems, uint32 stride, size_t offset) {
    if (id < 0 || id >= _buffers.size())
        return; // Error

    RHIBuffer buffer = _buffers[id];

    if (buffer.id == 0 || buffer.target != BUFFER_VERTEX)
        return; // Error

    glBindBuffer(buffer.target, buffer.id);
    glEnableVertexAttribArray(idx);
    glVertexAttribPointer(idx, numElems, OGLAttrTypes[type], GL_FALSE, (GLsizei)stride,
                          (const void*)offset);
    glBindBuffer(buffer.target, 0);
}

void RenderInterface::setBufferLayout(RRID id, const BufferLayout& layout) {
    if (id < 0 || id >= _buffers.size())
        return; // Error

    RHIBuffer buffer = _buffers[id];
    if (buffer.id == 0 || buffer.target != OGLBufferTargets[BUFFER_VERTEX])
        return; // Error

    glBindBuffer(buffer.target, buffer.id);

    for (uint32 i = 0; i < layout.numEntries; ++i) {
        const BufferLayoutEntry& entry = layout.entries[i];

        glEnableVertexAttribArray(entry.index);
        glVertexAttribPointer(entry.index, entry.numElems, OGLAttrTypes[entry.type],
                              GL_FALSE, (GLsizei)entry.stride, (const void*)entry.offset);
    }

    glBindBuffer(buffer.target, 0);
}

bool RenderInterface::updateBuffer(RRID id, size_t size, const void* data) {
    assert(id > 0 && id < _buffers.size());

    RHIBuffer buffer = _buffers[id];
    glBindBuffer(buffer.target, buffer.id);
    glBufferData(buffer.target, size, NULL, GL_DYNAMIC_DRAW);
    glBufferSubData(buffer.target, 0, size, data);

    glBindBuffer(buffer.target, 0);

    return true;
}

bool RenderInterface::deleteBuffer(RRID id) {
    if (id < 0 || id >= _buffers.size())
        return false; // Error

    RHIBuffer buffer = _buffers[id];
    if (buffer.id != 0) {
        glDeleteBuffers(1, &buffer.id);
        buffer.id = 0;
        return true;
    }

    return false;
}

/*uint32 RenderInterface::compileShader(const ShaderSource& source) {
    // Create shader id
    GLuint id = glCreateShader(OGLShaderTypes[source.type()]);
    if (id == 0)
        std::cerr << "Could not create shader: " + source.name();

    // Set shader source and compile
    const char* c_str = source.source().c_str();
    glShaderSource(id, 1, &c_str, 0);
    glCompileShader(id);

    // Check if shader compiled
    GLint result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_TRUE)
        return id;

    // Check shader log for the error and print it
    GLint logLen;
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &logLen);

    char* log = new char[logLen];
    glGetShaderInfoLog(id, logLen, &logLen, log);

    std::string message(log);
    delete[] log;

    std::cerr << "Shader " << source.name() << " compilation log:\n" << message;

    // Cleanup the shader
    glDeleteShader(id);

    return id;
}

bool RenderInterface::deleteShader(const ShaderSource& source) {
    uint32 id = source.id();
    if (id != 0) {
        glDeleteShader(id);
        return true;
    }

    return false;
}

RRID RenderInterface::linkProgram(const Program& shader) {
    // Create program
    GLuint id = glCreateProgram();
    if (id == 0) {
        std::string message = getProgramError(id);
        Utils::throwError("Could not create program " + shader.name());
    }

    // Attach shaders
    for (GLuint sid : shader.shaders()) {
        glAttachShader(id, sid);

        // Check attachment error
        checkOpenGLError("Could not attach shader (" + std::to_string(sid) +
                         ") to program (" + std::to_string(id) + ") " + shader.name() +
                         ".");
    }

    glLinkProgram(id);

    GLint res;
    glGetProgramiv(id, GL_LINK_STATUS, &res);
    if (res != GL_TRUE) {
        // Check program log for the error and print it
        std::string message = getProgramError(id);
        Utils::throwError(message);

        // Detach shaders
        for (GLuint sid : shader.shaders())
            glDetachShader(id, sid);

        // Delete the program
        glDeleteProgram(id);

        return false;
    }

    // Detach shaders after successful linking
    for (GLuint sid : shader.shaders())
        glDetachShader(id, sid);

    RRID rrid = _programs.size();
    _programs.push_back({id});

    return rrid;
}

std::string RenderInterface::getProgramError(int shader) {
    GLint logLen;
    glGetProgramiv(shader, GL_INFO_LOG_LENGTH, &logLen);

    char* log = new char[logLen];
    glGetProgramInfoLog(shader, logLen, &logLen, log);

    std::string strLog(log);
    delete[] log;

    return strLog;
}*/

void RenderInterface::useProgram(RRID id) {
    glUseProgram(id);
    _currProgram = id;
}

void RenderInterface::setFloat(const std::string& name, float val) {
    GLuint id = _programs[_currProgram].id;
    GLint loc = glGetUniformLocation(id, name.c_str());
    glUniform1f(loc, val);
}

void RenderInterface::setVector3(const std::string& name, const Vec3& vec) {
    GLuint id = _programs[_currProgram].id;
    GLint loc = glGetUniformLocation(id, name.c_str());
    glUniform3fv(loc, 1, (const GLfloat*)&vec);
}

void RenderInterface::setVector4(const std::string& name, const Vec4& vec) {
    GLuint id = _programs[_currProgram].id;
    GLint loc = glGetUniformLocation(id, name.c_str());
    glUniform4fv(loc, 1, (const GLfloat*)&vec);
}

void RenderInterface::setMatrix3(const std::string& name, const Mat3& mat) {
    GLuint id = _programs[_currProgram].id;
    GLint loc = glGetUniformLocation(id, name.c_str());
    glUniformMatrix3fv(loc, 1, GL_FALSE, (const GLfloat*)&mat);
}

void RenderInterface::setMatrix4(const std::string& name, const Mat4& mat) {
    GLuint id = _programs[_currProgram].id;
    GLint loc = glGetUniformLocation(id, name.c_str());
    glUniformMatrix4fv(loc, 1, GL_FALSE, (const GLfloat*)&mat);
}

void RenderInterface::setSampler(const std::string& name, uint32 id) {
    GLuint pid = _programs[_currProgram].id;
    GLint loc = glGetUniformLocation(pid, name.c_str());
    glUniform1i(loc, id);
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

void RenderInterface::setBufferBlock(const std::string& name, uint32 binding) {
    GLuint id = _programs[_currProgram].id;
    GLint idx = glGetUniformBlockIndex(id, name.c_str());
    glUniformBlockBinding(id, idx, binding);
}

int32 RenderInterface::uniformLocation(RRID id, const std::string& name) {
    GLuint pid = _programs[id].id;
    return glGetUniformLocation(pid, name.c_str());
}

uint32 RenderInterface::uniformBlockLocation(RRID id, const std::string& name) {
    GLuint pid = _programs[id].id;
    return glGetUniformBlockIndex(pid, name.c_str());
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

RRID RenderInterface::createTextureImmutable(const Image& img,
                                             const TexSampler& sampler) {
    GLuint id = 0;
    GLenum target = OGLTexTargets[static_cast<uint32>(img.type())];

    RRID resId = _textures.size();

    glGenTextures(1, &id);
    glBindTexture(target, id);

    GLenum pType = OGLTexPixelTypes[static_cast<uint32>(img.compType())];
    GLenum oglSizedFmt = OGLTexSizedPixelFormats[static_cast<uint32>(img.format())];

    ImageType type = img.type();
    uint32 levels = img.numLevels();
    uint32 width = img.width();
    uint32 height = img.height();
    uint32 depth = img.depth();

    // Create immutable texture storage
    using enum ImageType;
    if (type == IMGTYPE_2D) {
        glTexStorage2D(target, levels, oglSizedFmt, width, height);
    } else if (type == IMGTYPE_1D)
        glTexStorage1D(target, levels, oglSizedFmt, width);
    else if (type == IMGTYPE_3D)
        glTexStorage3D(target, levels, oglSizedFmt, width, height, depth);

    // Upload all levels
    GLenum oglFmt = OGLTexPixelFormats[static_cast<uint32>(img.format())];
    for (uint32 lvl = 0; lvl < img.numLevels(); ++lvl) {
        uint32 w = mipDimension(img.width(), lvl);
        uint32 h = mipDimension(img.height(), lvl);
        uint32 d = mipDimension(img.depth(), lvl);

        if (type == IMGTYPE_2D)
            glTexSubImage2D(target, lvl, 0, 0, w, h, oglFmt, pType, img.data(lvl));
        else if (type == IMGTYPE_1D)
            glTexSubImage1D(target, lvl, 0, w, oglFmt, pType, img.data(lvl));
        else if (type == IMGTYPE_3D)
            glTexSubImage3D(target, lvl, 0, 0, 0, w, h, d, oglFmt, pType, img.data(lvl));
    }

    glTexParameteri(target, GL_TEXTURE_WRAP_S, OGLTexWrapping[sampler.sWrap()]);
    glTexParameteri(target, GL_TEXTURE_WRAP_T, OGLTexWrapping[sampler.tWrap()]);
    glTexParameteri(target, GL_TEXTURE_WRAP_R, OGLTexWrapping[sampler.rWrap()]);
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, OGLTexFilters[sampler.minFilter()]);
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, OGLTexFilters[sampler.magFilter()]);

    // Unbind texture
    glBindTexture(target, 0);

    TexFormat fmt{img.type(), img.format(), img.compType(), img.numLevels()};
    auto tex = std::make_shared<Texture>(resId, img.width(), img.height(), img.depth(),
                                         sampler, fmt);

    _textures.push_back({id, target, oglFmt, oglFmt, pType, tex});

    return resId;
}

RRID RenderInterface::createTexture(const Image& img, const TexSampler& sampler) {
    GLuint id = 0;
    GLenum target = OGLTexTargets[static_cast<uint32>(img.type())];

    RRID resId = _textures.size();

    glGenTextures(1, &id);
    glBindTexture(target, id);

    GLenum pType = OGLTexPixelTypes[static_cast<uint32>(img.compType())];
    GLenum oglFmt = OGLTexPixelFormats[static_cast<uint32>(img.format())];

    // Upload all levels
    ImageType type = img.type();
    for (uint32 lvl = 0; lvl < img.numLevels(); ++lvl) {
        uint32 w = mipDimension(img.width(), lvl);
        uint32 h = mipDimension(img.height(), lvl);
        uint32 d = mipDimension(img.depth(), lvl);

        using enum ImageType;
        if (type == IMGTYPE_2D)
            glTexImage2D(target, lvl, oglFmt, w, h, 0, oglFmt, pType, img.data(lvl));
        else if (type == IMGTYPE_1D)
            glTexImage1D(target, lvl, oglFmt, w, 0, oglFmt, pType, img.data(lvl));
        else if (type == IMGTYPE_3D)
            glTexImage3D(target, lvl, oglFmt, w, h, d, 0, oglFmt, pType, img.data(lvl));
    }

    glTexParameteri(target, GL_TEXTURE_WRAP_S, OGLTexWrapping[sampler.sWrap()]);
    glTexParameteri(target, GL_TEXTURE_WRAP_T, OGLTexWrapping[sampler.tWrap()]);
    glTexParameteri(target, GL_TEXTURE_WRAP_R, OGLTexWrapping[sampler.rWrap()]);
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, OGLTexFilters[sampler.minFilter()]);
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, OGLTexFilters[sampler.magFilter()]);

    // Unbind texture
    glBindTexture(target, 0);

    TexFormat fmt{img.type(), img.format(), img.compType(), img.numLevels()};
    /*fmt.imgFmt  = img.format();
    fmt.imgType = img.type();
    fmt.levels  = img.numLevels();
    fmt.pType   = img.compType();*/

    auto tex = std::make_shared<Texture>(resId, img.width(), img.height(), img.depth(),
                                         sampler, fmt);

    _textures.push_back({id, target, oglFmt, oglFmt, pType, tex});

    return resId;
}

// Create texture
RRID RenderInterface::createTexture(ImageType type, ImageFormat fmt, uint32 width,
                                    uint32 height, uint32 depth,
                                    const TexSampler& sampler) {
    using enum ImageType;

    GLuint id = 0;
    GLenum target = OGLTexTargets[static_cast<uint32>(type)];

    RRID resId = _textures.size();

    if (type == IMGTYPE_2D) {
        depth = 1;
    } else if (type == IMGTYPE_1D) {
        height = depth = 1;
    }

    if (type == IMGTYPE_2D && sampler.numSamples() > 0)
        target = GL_TEXTURE_2D_MULTISAMPLE;

    glGenTextures(1, &id);
    glBindTexture(target, id);

    GLenum intFormat = OGLTexSizedFormats[static_cast<uint32>(fmt)];
    if (type == IMGTYPE_2D && sampler.numSamples() > 0)
        glTexImage2DMultisample(target, sampler.numSamples(), intFormat, width, height,
                                GL_TRUE);

    // Set the sampler
    glTexParameteri(target, GL_TEXTURE_WRAP_S, OGLTexWrapping[sampler.sWrap()]);
    glTexParameteri(target, GL_TEXTURE_WRAP_T, OGLTexWrapping[sampler.tWrap()]);
    glTexParameteri(target, GL_TEXTURE_WRAP_R, OGLTexWrapping[sampler.rWrap()]);
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, OGLTexFilters[sampler.minFilter()]);
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, OGLTexFilters[sampler.magFilter()]);

    // Unbind texture
    glBindTexture(target, 0);

    TexFormat texFmt;
    texFmt.imgFmt = fmt;
    texFmt.imgType = type;
    texFmt.pType = formatToImgComp(fmt);
    texFmt.levels = 1;

    auto tex = std::make_shared<Texture>(resId, width, height, depth, sampler, texFmt);

    _textures.push_back({id, target, intFormat,
                         OGLTexPixelFormats[static_cast<uint32>(fmt)],
                         OGLTexPixelTypes[static_cast<uint32>(texFmt.pType)], tex});

    return resId;
}

RRID RenderInterface::createCubemap(const Cubemap& cube, const TexSampler& sampler) {
    using enum ImageType;

    GLuint id = 0;
    GLenum target = OGLTexTargets[static_cast<uint32>(IMGTYPE_CUBE)];

    RRID resId = _textures.size();

    GLenum pType = OGLTexPixelTypes[static_cast<uint32>(cube.compType())];
    GLenum oglFmt = OGLTexPixelFormats[static_cast<uint32>(cube.format())];

    glGenTextures(1, &id);
    glBindTexture(target, id);

    if (cube.numLevels() > 1) {
        glTexParameteri(target, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(target, GL_TEXTURE_MAX_LEVEL, 4);
        // glGenerateMipmap(target);
    }

    for (uint32 side = 0; side < 6; side++) {
        const Image* img = cube.face((CubemapFace)side);

        // Upload all levels for the face
        for (uint32 lvl = 0; lvl < img->numLevels(); ++lvl) {
            uint32 w = mipDimension(img->width(), lvl);
            uint32 h = mipDimension(img->height(), lvl);

            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + side, lvl, oglFmt, w, h, 0,
                         oglFmt, pType, img->data(lvl));
        }
    }

    glTexParameteri(target, GL_TEXTURE_WRAP_S, OGLTexWrapping[sampler.sWrap()]);
    glTexParameteri(target, GL_TEXTURE_WRAP_T, OGLTexWrapping[sampler.tWrap()]);
    glTexParameteri(target, GL_TEXTURE_WRAP_R, OGLTexWrapping[sampler.rWrap()]);
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, OGLTexFilters[sampler.minFilter()]);
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, OGLTexFilters[sampler.magFilter()]);

    // Unbind texture
    glBindTexture(target, 0);

    TexFormat fmt;
    fmt.imgFmt = cube.format();
    fmt.imgType = IMGTYPE_CUBE;
    fmt.levels = cube.numLevels();
    fmt.pType = cube.compType();

    auto tex =
        std::make_shared<Texture>(resId, cube.width(), cube.height(), 1, sampler, fmt);

    _textures.push_back({id, target, oglFmt, oglFmt, pType, tex});

    return resId;
}

sref<Texture> RenderInterface::getTexture(RRID id) {
    assert(id > 0 && id >= _textures.size());

    RHITexture tex = _textures[id];

    assert(tex.id != 0);

    return tex.tex;
}

bool RenderInterface::readTexture(RRID id, Image& img) {
    assert(id > 0 && id >= _textures.size());

    RHITexture tex = _textures[id];
    if (tex.id == 0)
        return false; // Error

    TexFormat fmt = tex.tex->format();
    if (fmt.imgType == ImageType::IMGTYPE_CUBE)
        return false; // Error

    img.init(fmt.imgFmt, tex.tex->width(), tex.tex->height(), tex.tex->depth(),
             fmt.levels);

    glBindTexture(tex.target, tex.id);
    for (uint32 lvl = 0; lvl < fmt.levels; ++lvl)
        glGetTexImage(tex.target, 0, tex.format, tex.pType, img.data(lvl));
    glBindTexture(tex.target, 0);

    return true;
}

bool RenderInterface::readCubemap(RRID id, Cubemap& cube) {
    if (id < 0 || id >= _textures.size())
        return false; // Error

    RHITexture tex = _textures[id];
    if (tex.id == 0)
        return false; // Error

    TexFormat fmt = tex.tex->format();
    if (fmt.imgType != ImageType::IMGTYPE_CUBE)
        return false; // Error

    cube.init(fmt.imgFmt, tex.tex->width(), tex.tex->height(), fmt.levels);

    glBindTexture(tex.target, tex.id);
    for (uint32 f = 0; f < 6; ++f)
        for (uint32 lvl = 0; lvl < fmt.levels; ++lvl)
            glGetTexImage(tex.target, 0, tex.format, tex.pType,
                          cube.data((CubemapFace)f, lvl));
    glBindTexture(tex.target, 0);

    return true;
}

void RenderInterface::generateMipmaps(RRID id) {
    assert(id > 0 && id < _textures.size());

    RHITexture ogltex = _textures[id];
    assert(ogltex.id != 0);

    glBindTexture(ogltex.target, ogltex.id);
    glGenerateMipmap(ogltex.target);
    glBindTexture(ogltex.target, 0);
}

void RenderInterface::setTextureData(RRID id, uint32 level, const void* pixels) {
    assert(id > 0 && id < _textures.size());

    RHITexture ogltex = _textures[id];
    assert(ogltex.id != 0);

    glBindTexture(ogltex.target, ogltex.id);

    GLsizei width = ogltex.tex->width();
    GLsizei height = ogltex.tex->height();
    GLsizei depth = ogltex.tex->depth();

    const TexFormat& fmt = ogltex.tex->format();
    GLenum type = OGLTexPixelTypes[static_cast<uint32>(fmt.pType)];

    using enum ImageType;
    if (fmt.imgType == IMGTYPE_2D)
        glTexImage2D(ogltex.target, level, ogltex.intFormat, width, height, 0,
                     ogltex.format, type, pixels);
    else if (fmt.imgType == IMGTYPE_1D)
        glTexImage1D(ogltex.target, level, ogltex.intFormat, width, 0, ogltex.format,
                     type, pixels);
    else if (fmt.imgType == IMGTYPE_3D)
        glTexImage3D(ogltex.target, level, ogltex.intFormat, width, height, depth, 0,
                     ogltex.format, type, pixels);

    glBindTexture(ogltex.target, 0);
}

void RenderInterface::deleteTexture(RRID id) {
    assert(id != -1);
    assert(id < _textures.size());

    GLuint oglId = _textures[id].id;
    if (oglId != 0) {
        glDeleteTextures(1, &oglId);
        _textures[id].id = 0;
    }
}

void RenderInterface::bindTexture(RRID id) {
    assert(id > 0 && id < _textures.size());

    const RHITexture& ogltex = _textures[id];
    glBindTexture(ogltex.target, ogltex.id);
}

void RenderInterface::bindTexture(uint32 slot, RRID id) {
    glActiveTexture(GL_TEXTURE0 + slot);
    bindTexture(id);
}

void RenderInterface::bindTextures(uint32 first, uint32 size,
                                   std::span<const uint32> texs) {
    glBindTextures(first, size, texs.data());
}

std::unique_ptr<Image> RenderInterface::getImage(int32 x, int32 y, int32 w,
                                                 int32 h) const {
    auto img = std::make_unique<Image>();
    img->init(ImageFormat::IMGFMT_RGB8, w, h, 1, 1);
    glReadPixels(x, y, w, h, GL_RGB, GL_UNSIGNED_BYTE, img->data());
    return img;
}

std::vector<uint32> RenderInterface::queryTexHandles(std::vector<RRID> texs) {
    std::vector<uint32> ret;
    for (auto id : texs) {
        if (id == -1)
            ret.push_back(0);
        else
            ret.push_back(_textures[id].id);
    }
    return ret;
}

uint32 RenderInterface::queryTexHandle(RRID tex) {
    return _textures[tex].id;
}