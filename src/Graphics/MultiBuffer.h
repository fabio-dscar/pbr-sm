#ifndef __PBR_MULTIBUFFER_H__
#define __PBR_MULTIBUFFER_H__

#include <Buffer.h>

namespace pbr {

struct BufferBind {
    uint32 bindIdx = 0;
    std::size_t offset = 0;
    std::size_t size = 0;
};

// Round robin N-buffered buffer
class MultiBuffer : private Buffer {
public:
    MultiBuffer() = default;

    void create(EBufferType type, uint32 num, std::size_t size, uint32 flags) {
        numRounds = num;
        baseSize = size;
        Buffer::create(type, baseSize * numRounds, flags, NULL);
    }

    template<typename T>
    T* get() const {
        return Buffer::get<T>(currIdx * baseSize);
    }

    template<typename T>
    T* getBind(uint32 idx) const {
        std::size_t offset = binds[idx].offset; 
        return Buffer::get<T>(currIdx * baseSize + offset);
    }

    void lock() { lockRange(currIdx * baseSize, baseSize); }
    void wait() { waitRange(currIdx * baseSize, baseSize); }
    void swap() { currIdx = (currIdx + 1) % numRounds; }

    void registerBind(uint32 idx, std::size_t offset, std::size_t size) {
        binds.emplace_back(idx, offset, size);
    }

    void rebind() {
        glBindBuffer(target, handle);
        for (const auto& bind : binds)
            bindRange(bind.bindIdx, bind.offset + currIdx * baseSize, bind.size);
        glBindBuffer(target, handle);
    }

private:
    std::size_t baseSize = 0;
    uint32 numRounds = 0;
    uint32 currIdx = 0;
    std::vector<BufferBind> binds;
};

} // namespace pbr

#endif // __MULTIBUFFER_H__