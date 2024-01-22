#ifndef __PBR_VERTEXARRAYS_H__
#define __PBR_VERTEXARRAYS_H__

#include <PBR.h>
#include <Buffer.h>

namespace pbr {

enum class AttribType : int { Byte = 0, Short = 1, UInt = 2, Float = 3 };
consteval bool EnumHasConversion(AttribType);

struct BufferLayoutEntry {
    unsigned int index = 0;
    unsigned int numElems = 0;
    AttribType type = AttribType::Float;
    std::size_t stride = 0;
    std::size_t offset = 0;
};

struct VertexBufferEntry {
    std::size_t offset = 0; // offset to start
    std::size_t stride = 0;
    std::vector<BufferLayoutEntry> attribs;
    std::unique_ptr<Buffer> buffer = nullptr;
};

struct ElementBuffer {
    unsigned int handle = 0;
    unsigned int numIndices = 0;
    AttribType type = AttribType::UInt;
    std::unique_ptr<Buffer> buffer = nullptr;
};

class VertexArrays {
public:
    VertexArrays();
    ~VertexArrays();

    VertexArrays(VertexArrays&& rhs);
    VertexArrays& operator=(VertexArrays&& rhs);

    VertexArrays(const VertexArrays&) = delete;
    VertexArrays& operator=(const VertexArrays&) = delete;

    unsigned int id() const { return handle; }

    void addVertexBuffer(Buffer&& buffer, std::span<BufferLayoutEntry> layout,
                         std::size_t offset, std::size_t stride,
                         unsigned int numVertices);
    void addElementBuffer(Buffer&& buffer, unsigned int numIndices,
                          AttribType type = AttribType::UInt);

    void draw() const;

public:
    unsigned int handle = 0;
    ElementBuffer elementBuffer = {};
    unsigned int numVerts = 0;
    std::vector<VertexBufferEntry> vertexBuffers;
};

} // namespace pbr

#endif // __PBR_VERTEXARRAYS_H__