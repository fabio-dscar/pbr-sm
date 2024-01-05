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

const Vec3& BBox3::operator[](uint32 i) const {
    if (i == 0)
        return _min;

    return _max;
}

Vec3 BBox3::sizes() const {
    return abs(_max - _min);
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
    const Float radius = distance(_max, pos);

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
    _min = math::min(_min, pt);
    _max = math::max(_max, pt);
}

void BBox3::expand(const BBox3& box) {
    _min = math::min(_min, box[0]);
    _max = math::max(_max, box[1]);
}

void BBox3::intersect(const BBox3& box) {
    _min = math::max(_min, box[0]);
    _max = math::min(_max, box[1]);
}

bool BBox3::intersectRay(const Ray& ray, float* t) const {
    float tMin = ray.tMin();
    float tMax = ray.tMax();

    Vec3 dir = ray.direction();
    Vec3 origin = ray.origin();

    float invDir, tNear, tFar;

    for (int axis = 0; axis < 3; axis++) {
        invDir = 1.0 / dir[axis];

        tNear = (_min[axis] - origin[axis]) * invDir;
        tFar = (_max[axis] - origin[axis]) * invDir;

        if (tNear > tFar)
            std::swap(tNear, tFar);

        if (tNear > tMin)
            tMin = tNear;
        if (tFar < tMax)
            tMax = tFar;

        if (tMin > tMax)
            return false;
    }

    *t = tMin;
    return true;
}

BBox3 math::Expand(const BBox3& box, const Vec3& pt) {
    return {min(box.min(), pt), max(box.max(), pt)};
}

BBox3 math::Expand(const BBox3& box1, const BBox3& box2) {
    return {min(box1.min(), box2.min()), max(box1.max(), box2.max())};
}

BBox3 math::Intersection(const BBox3& box1, const BBox3& box2) {
    return {max(box1.min(), box2.min()), min(box1.max(), box2.max())};
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
    Float d = distance(pos, _center);
    return d < _radius;
}

bool BSphere::isBounded() const {
    return !_center.isInfinite() &&
           (_radius != -FLOAT_INFINITY || _radius != FLOAT_INFINITY);
}

bool BSphere::intersectRay(const Ray& ray, float* t) const {
    const Vec3 cdir = ray.origin() - _center;

    // Solve quadratic
    float B = dot(cdir, ray.direction());
    float C = cdir.lengthSqr() - _radius * _radius;
    float detSq = B * B - C;
    if (detSq >= 0.0) {
        float det = std::sqrt(detSq);

        *t = -B - det;
        if (ray.inRange(*t))
            return true;

        *t = -B + det;
        if (ray.inRange(*t))
            return true;
    }

    return false;
}

BSphere math::Transform(const Matrix4x4& mat, const BSphere& bSphere) {
    return {mat * Vec4(bSphere.center(), 1.0f), bSphere.radius()};
}