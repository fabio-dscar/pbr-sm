#ifndef __PBR_HASH_H__
#define __PBR_HASH_H__

#include <PBRMath.h>

using namespace pbr::math;

namespace std {
    template<>
    struct hash<Vec2> {
        size_t operator()(const Vec2& v) const;
    };

    template<>
    struct hash<Vec3> {
        size_t operator()(const Vec3& v) const;
    };

    template<>
    struct hash<Vec4> {
        size_t operator()(const Vec4& v) const;
    };

    template<>
    struct hash<Quat> {
        size_t operator()(const Quat& q) const;
    };
}

namespace pbr {
    namespace math {
        inline void hashCombine(size_t& seed, size_t hash) {
            hash += 0x9e3779b9 + (seed << 6) + (seed >> 2);
            seed ^= hash;
        }
    }
}

#endif