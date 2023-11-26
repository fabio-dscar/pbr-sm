#include <Buffer.h>

using namespace pbr;

const GLenum OGLBufferTarget[] = {GL_ARRAY_BUFFER, GL_ELEMENT_ARRAY_BUFFER,
                                  GL_UNIFORM_BUFFER};

Buffer::Buffer(EBufferType type, std::size_t size, uint32 flags, void* data) {
    create(type, size, flags, data);
}

Buffer::~Buffer() {
    glDeleteBuffers(1, &handle);
}

void Buffer::create(EBufferType type, std::size_t pSize, uint32 pFlags, void* data) {
    target = OGLBufferTarget[static_cast<uint32>(type)];
    flags = pFlags;
    size = pSize;
    glCreateBuffers(1, &handle);
    glNamedBufferStorage(handle, size, data, flags);

    if (flags & GL_MAP_PERSISTENT_BIT)
        ptr = reinterpret_cast<std::byte*>(glMapNamedBufferRange(handle, 0, size, flags));
}

void Buffer::wait(GLsync* pSync) {
    GLenum res = glClientWaitSync(*pSync, 0, 0);
    if (res == GL_ALREADY_SIGNALED || res == GL_CONDITION_SATISFIED)
        glDeleteSync(*pSync);
}

void Buffer::waitRange(std::size_t start, std::size_t pSize) {
    std::vector<BufferRangeLock> swapLocks;
    for (auto it = locks.begin(); it != locks.end(); ++it) {
        if (it->overlaps(start, pSize)) {
            wait(&it->lock);
        } else {
            swapLocks.push_back(*it);
        }
    }
    locks.swap(swapLocks);
}

void Buffer::lockRange(std::size_t start, std::size_t pSize) {
    GLsync syncName = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    locks.emplace_back(syncName, start, pSize);
}

void Buffer::bindRange(uint32 index, std::size_t offset, std::size_t bSize) {
    glBindBufferRange(target, index, handle, offset, bSize);
}