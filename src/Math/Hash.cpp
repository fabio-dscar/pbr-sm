#include <Hash.h>

namespace std {
    size_t hash<Vec2>::operator()(const Vec2& v) const {
        size_t seed = 0;
        hash<float> hasher;
        HashCombine(seed, hasher(v.x));
        HashCombine(seed, hasher(v.y));
        return seed;
    }

    size_t hash<Vec3>::operator()(const Vec3& v) const {
        size_t seed = 0;
        hash<float> hasher;
        HashCombine(seed, hasher(v.x));
        HashCombine(seed, hasher(v.y));
        HashCombine(seed, hasher(v.z));
        return seed;
    }

    size_t hash<Vec4>::operator()(const Vec4& v) const {
        size_t seed = 0;
        hash<float> hasher;
        HashCombine(seed, hasher(v.x));
        HashCombine(seed, hasher(v.y));
        HashCombine(seed, hasher(v.z));
        HashCombine(seed, hasher(v.w));
        return seed;
    }

    size_t hash<Quat>::operator()(const Quat& q) const {
        size_t seed = 0;
        hash<float> hasher;
        HashCombine(seed, hasher(q.x));
        HashCombine(seed, hasher(q.y));
        HashCombine(seed, hasher(q.z));
        HashCombine(seed, hasher(q.w));
        return seed;
    }
}