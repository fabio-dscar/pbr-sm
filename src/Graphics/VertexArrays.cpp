#include <VertexArrays.h>

#include <glad/glad.h>

using namespace pbr;

namespace {
const std::array OglAttribType{GL_BYTE, GL_SHORT, GL_UNSIGNED_INT, GL_FLOAT};

auto ToOglType(AttribType type) {
    return OglAttribType[static_cast<int>(type)];
}

} // namespace

VertexArrays::VertexArrays() {
    glCreateVertexArrays(1, &handle);
}

VertexArrays::~VertexArrays() {
    if (handle != 0) {
        glDeleteVertexArrays(1, &handle);
    }
}

VertexArrays::VertexArrays(VertexArrays&& rhs)
    : handle(std::exchange(rhs.handle, 0)), elementBuffer(rhs.elementBuffer),
      numVerts(rhs.numVerts), vertexBuffers(std::move(rhs.vertexBuffers)) {}

void VertexArrays::addVertexBuffer(const Buffer& buffer,
                                   std::span<BufferLayoutEntry> layout,
                                   std::size_t offset, std::size_t stride,
                                   unsigned int numVertices) {
    numVerts = numVertices;
    auto bindIndex = vertexBuffers.size();
    vertexBuffers.emplace_back(
        offset, stride, std::vector<BufferLayoutEntry>{layout.begin(), layout.end()});

    glVertexArrayVertexBuffer(handle, bindIndex, buffer.id(), offset, stride);

    for (const auto& entry : layout) {
        glEnableVertexArrayAttrib(handle, entry.index);
        glVertexArrayAttribBinding(handle, entry.index, bindIndex);
        glVertexArrayAttribFormat(handle, entry.index, entry.numElems,
                                  ToOglType(entry.type), GL_FALSE, entry.offset);
    }
}

void VertexArrays::addElementBuffer(const Buffer& buffer, unsigned int numIndices,
                                    AttribType type) {
    elementBuffer = {buffer.id(), numIndices, type};
    glVertexArrayElementBuffer(handle, elementBuffer.handle);
}

void VertexArrays::draw() const {
    glBindVertexArray(handle);
    if (elementBuffer.handle != 0)
        glDrawElements(GL_TRIANGLES, elementBuffer.numIndices,
                       ToOglType(elementBuffer.type), 0);
    else
        glDrawArrays(GL_TRIANGLES, 0, numVerts);
    glBindVertexArray(0);
}