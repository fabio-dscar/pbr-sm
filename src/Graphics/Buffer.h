#ifndef __PBR_BUFFER_H__
#define __PBR_BUFFER_H__

#include <glad/glad.h>
#include <PBR.h>

namespace pbr {

struct BufferRangeLock {
    GLsync lock = 0;
    std::size_t start = 0;
    std::size_t size = 0;

    bool overlaps(std::size_t rhsStart, std::size_t rhsSize) {
        return (start + size) > rhsStart && (rhsStart + rhsSize) > start;
    }
};

enum class EBufferType : uint32 { ARRAY = 0, ELEMENT = 1, UNIFORM = 2 };

// In nanoseconds
static constexpr int FenceTimeout = 1.0 / 30.0 * 1e9;

class Buffer {
public:
    Buffer() = default;
    Buffer(EBufferType type, std::size_t size, uint32 flags, void* data = nullptr);
    ~Buffer();

    void create(EBufferType type, std::size_t size, uint32 flags, void* data);

    void bindRange(uint32 index, std::size_t offset, std::size_t size);

    void waitRange(std::size_t start, std::size_t size);
    void lockRange(std::size_t start, std::size_t size);

    template<typename T>
    T* get(std::size_t offset = 0) const {
        return reinterpret_cast<T*>(ptr + offset);
    }

protected:
    std::vector<BufferRangeLock> locks;
    std::byte* ptr = nullptr;
    std::size_t size = 0;
    uint32 target = 0;
    uint32 handle = 0;
    uint32 flags = 0;

private:
    void wait(GLsync* pSync);
};

} // namespace pbr

#endif