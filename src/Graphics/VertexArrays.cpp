#include <VertexArrays.h>

#include <glad/glad.h>

#include <Buffer.h>

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
    : handle(std::exchange(rhs.handle, 0)), elementBuffer(std::move(rhs.elementBuffer)),
      numVerts(rhs.numVerts), vertexBuffers(std::move(rhs.vertexBuffers)) {}

VertexArrays& VertexArrays::operator=(VertexArrays&& rhs) {
    handle = std::exchange(rhs.handle, 0);
    numVerts = std::exchange(rhs.numVerts, 0);
    elementBuffer = std::move(rhs.elementBuffer);
    vertexBuffers = std::move(rhs.vertexBuffers);
    return *this;
}

void VertexArrays::addVertexBuffer(Buffer&& buffer, std::span<BufferLayoutEntry> layout,
                                   std::size_t offset, std::size_t stride,
                                   unsigned int numVertices) {
    numVerts = numVertices;
    const auto bufferHandle = buffer.id();

    vertexBuffers.emplace_back(
        offset, stride, std::vector<BufferLayoutEntry>{layout.begin(), layout.end()},
        std::make_unique<Buffer>(std::move(buffer)));

    const auto bindIndex = vertexBuffers.size();
    glVertexArrayVertexBuffer(handle, bindIndex, bufferHandle, offset, stride);

    for (const auto& entry : layout) {
        glEnableVertexArrayAttrib(handle, entry.index);
        glVertexArrayAttribBinding(handle, entry.index, bindIndex);
        glVertexArrayAttribFormat(handle, entry.index, entry.numElems,
                                  ToOglType(entry.type), GL_FALSE, entry.offset);
    }
}

void VertexArrays::addElementBuffer(Buffer&& buffer, unsigned int numIndices,
                                    AttribType type) {
    elementBuffer = {buffer.id(), numIndices, type,
                     std::make_unique<Buffer>(std::move(buffer))};
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