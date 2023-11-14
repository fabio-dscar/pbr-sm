#ifndef __PBR_HASH_H__
#define __PBR_HASH_H__

#include <PBRMath.h>

using namespace pbr::math;

/* ===========================================================================
        Definition of hash functions for some of the math library classes.
 =============================================================================*/

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

// Implementation
namespace pbr {
    namespace math {
        inline void hashCombine(size_t& seed, size_t hash) {
            hash += 0x9e3779b9 + (seed << 6) + (seed >> 2);
            seed ^= hash;
        }
    }
}

namespace std {
    size_t hash<Vec2>::operator()(const Vec2& v) const {
        size_t seed = 0;
        hash<float> hasher;
        hashCombine(seed, hasher(v.x));
        hashCombine(seed, hasher(v.y));
        return seed;
    }

    size_t hash<Vec3>::operator()(const Vec3& v) const {
        size_t seed = 0;
        hash<float> hasher;
        hashCombine(seed, hasher(v.x));
        hashCombine(seed, hasher(v.y));
        hashCombine(seed, hasher(v.z));
        return seed;
    }

    size_t hash<Vec4>::operator()(const Vec4& v) const {
        size_t seed = 0;
        hash<float> hasher;
        hashCombine(seed, hasher(v.x));
        hashCombine(seed, hasher(v.y));
        hashCombine(seed, hasher(v.z));
        hashCombine(seed, hasher(v.w));
        return seed;
    }

    size_t hash<Quat>::operator()(const Quat& q) const {
        size_t seed = 0;
        hash<float> hasher;
        hashCombine(seed, hasher(q.x));
        hashCombine(seed, hasher(q.y));
        hashCombine(seed, hasher(q.z));
        hashCombine(seed, hasher(q.w));
        return seed;
    }
}

#endif