#include <Hash.h>

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