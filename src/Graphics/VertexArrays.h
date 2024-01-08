#include <glad/glad.h>
#include <PBR.h>

#include <Buffer.h>

namespace pbr {

enum class AttribType { Byte = 0, Short = 1, UInt = 2, Float = 3 };

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
};

struct ElementBuffer {
    unsigned int handle = 0;
    unsigned int numIndices = 0;
    AttribType type = AttribType::UInt;
};

class VertexArrays {
public:
    VertexArrays() { glCreateVertexArrays(1, &handle); }
    ~VertexArrays() {
        if (handle != 0) {
            glDeleteVertexArrays(1, &handle);
        }
    }

    VertexArrays(VertexArrays&& rhs);

    unsigned int id() const { return handle; }

    void addVertexBuffer(const Buffer& buffer, std::span<BufferLayoutEntry> layout,
                         std::size_t offset, std::size_t stride,
                         unsigned int numVertices);
    void addElementBuffer(const Buffer& buffer, unsigned int numIndices,
                          AttribType type = AttribType::UInt);

    void draw() const;

public:
    unsigned int handle = 0;
    ElementBuffer elementBuffer = {};
    unsigned int numVerts = 0;
    std::vector<VertexBufferEntry> vertexBuffers;
};

} // namespace pbr