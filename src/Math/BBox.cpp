#include <BBox.h>

#include <Ray.h>

using namespace pbr;
using namespace pbr::math;

const Vec3& BBox3::min() const {
    return _min;
}

const Vec3& BBox3::max() const {
    return _max;
}

bool BBox3::contains(const Vec3& pos) const {
    return (pos.x <= _max.x && pos.x >= _min.x) && (pos.y <= _max.y && pos.y >= _min.y) &&
           (pos.z <= _max.z && pos.z >= _min.z);
}

const Vec3& BBox3::operator[](unsigned int i) const {
    if (i == 0)
        return _min;

    return _max;
}

Vec3 BBox3::sizes() const {
    return Abs(_max - _min);
}

Vec3 BBox3::center() const {
    return (Float)0.5 * (_max + _min);
}

Float BBox3::volume() const {
    Vec3 len = sizes();
    return len.x * len.y * len.z;
}

Float BBox3::area() const {
    Vec3 len = sizes();
    return 2.0 * (len.x * len.y + len.x * len.z + len.y * len.z);
}

BSphere BBox3::sphere() const {
    const Vec3 pos = center();
    const Float radius = Distance(_max, pos);

    return {pos, radius + FLOAT_EPSILON};
}

bool BBox3::overlaps(const BBox3& box) const {
    return (_max.x >= box[0].x) && (_min.x <= box[1].x) && (_max.y >= box[0].y) &&
           (_min.y <= box[1].y) && (_max.z >= box[0].z) && (_min.z <= box[1].z);
}

bool BBox3::isBounded() const {
    return !(_min.isInfinite() || _max.isInfinite());
}

void BBox3::expand(float size) {
    _min = _min + Vec3(-size);
    _max = _max + Vec3(size);
}

void BBox3::expand(const Vec3& pt) {
    _min = math::Min(_min, pt);
    _max = math::Max(_max, pt);
}

void BBox3::expand(const BBox3& box) {
    _min = math::Min(_min, box[0]);
    _max = math::Max(_max, box[1]);
}

void BBox3::intersect(const BBox3& box) {
    _min = math::Max(_min, box[0]);
    _max = math::Min(_max, box[1]);
}

std::optional<float> BBox3::intersectRay(const Ray& ray, float tMax) const {
    float tMin = ray.tMin();

    const Vec3& dir = ray.direction();
    const Vec3& orig = ray.origin();

    for (int axis = 0; axis < 3; axis++) {
        const float invDir = 1.0 / dir[axis];

        float tNear = (_min[axis] - orig[axis]) * invDir;
        float tFar = (_max[axis] - orig[axis]) * invDir;

        if (tNear > tFar)
            std::swap(tNear, tFar);

        tMin = std::max(tMin, tNear);
        tMax = std::min(tMax, tFar);

        if (tMin > tMax)
            return std::nullopt;
    }

    return tMin;
}

BBox3 math::Expand(const BBox3& box, const Vec3& pt) {
    return {Min(box.min(), pt), Max(box.max(), pt)};
}

BBox3 math::Expand(const BBox3& box1, const BBox3& box2) {
    return {Min(box1.min(), box2.min()), Max(box1.max(), box2.max())};
}

BBox3 math::Intersection(const BBox3& box1, const BBox3& box2) {
    return {Max(box1.min(), box2.min()), Min(box1.max(), box2.max())};
}

bool math::Overlaps(const BBox3& box1, const BBox3& box2) {
    return (box1[1].x >= box2[0].x) && (box1[0].x <= box2[1].x) &&
           (box1[1].y >= box2[0].y) && (box1[0].y <= box2[1].y) &&
           (box1[1].z >= box2[0].z) && (box1[0].z <= box2[1].z);
}

BBox3 math::Transform(const Matrix4x4& mat, const BBox3& box) {
    BBox3 ret(FLOAT_INFINITY, -FLOAT_INFINITY);

    ret.expand(mat * Vec4(box[0].x, box[0].y, box[0].z, 1.0f));
    ret.expand(mat * Vec4(box[1].x, box[0].y, box[0].z, 1.0f));
    ret.expand(mat * Vec4(box[0].x, box[1].y, box[0].z, 1.0f));
    ret.expand(mat * Vec4(box[0].x, box[0].y, box[1].z, 1.0f));
    ret.expand(mat * Vec4(box[0].x, box[1].y, box[1].z, 1.0f));
    ret.expand(mat * Vec4(box[1].x, box[1].y, box[0].z, 1.0f));
    ret.expand(mat * Vec4(box[1].x, box[0].y, box[1].z, 1.0f));
    ret.expand(mat * Vec4(box[1].x, box[1].y, box[1].z, 1.0f));

    return ret;
}

const Vec3& BSphere::center() const {
    return _center;
}

Float BSphere::radius() const {
    return _radius;
}

Float BSphere::area() const {
    return 4 * PI * _radius * _radius;
}

bool BSphere::contains(const Vec3& pos) const {
    Float d = Distance(pos, _center);
    return d < _radius;
}

bool BSphere::isBounded() const {
    return !_center.isInfinite() &&
           (_radius != -FLOAT_INFINITY || _radius != FLOAT_INFINITY);
}

std::optional<float> BSphere::intersectRay(const Ray& ray, float tMax) const {
    const Vec3 cdir = ray.origin() - _center;

    // Solve quadratic
    float B = Dot(cdir, ray.direction());
    float C = cdir.lengthSqr() - _radius * _radius;
    float detSq = B * B - C;
    if (detSq >= 0.0) {
        float det = std::sqrt(detSq);

        float t = -B - det;
        if (t >= ray.tMin() && t <= tMax)
            return t;

        t = -B + det;
        if (t >= ray.tMin() && t <= tMax)
            return t;
    }

    return std::nullopt;
}

BSphere math::Transform(const Matrix4x4& mat, const BSphere& bSphere) {
    return {mat * Vec4(bSphere.center(), 1.0f), bSphere.radius()};
}