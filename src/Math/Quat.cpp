#include <Quat.h>

#include <PBRMath.h>

using namespace pbr;
using namespace pbr::math;

Quat::Quat(const Matrix4x4& mat) {
    const Matrix3x3 m = Matrix3x3(mat);

    float trace = m.trace();
    if (trace > 0.f) {
        float s = std::sqrt(trace + 1.0f);
        w = s / 2.0f;
        s = 0.5f / s;
        x = s * (m(2, 1) - m(1, 2));
        y = s * (m(0, 2) - m(2, 0));
        z = s * (m(1, 0) - m(0, 1));
    } else {
        // Compute largest of x, y, or z, then remaining components
        const int nxt[3] = {1, 2, 0};
        float q[3];
        int i = 0;
        if (m(1, 1) > m(0, 0))
            i = 1;
        if (m(2, 2) > m(i, i))
            i = 2;

        int j = nxt[i];
        int k = nxt[j];
        float s = std::sqrt(1.0f + (m(i, i) - (m(j, j) + m(k, k))));
        q[i] = s * 0.5f;
        if (s != 0.f)
            s = 0.5f / s;
        w = (m(k, j) - m(j, k)) * s;
        q[j] = (m(i, j) + m(j, i)) * s;
        q[k] = (m(i, k) + m(k, i)) * s;

        x = q[0];
        y = q[1];
        z = q[2];
    }
}

Quat Quat::operator+(const Quat& q) const {
    return {x + q.x, y + q.y, z + q.z, w + q.w};
}

Quat& Quat::operator+=(const Quat& q) {
    w += q.w;
    x += q.x;
    y += q.y;
    z += q.z;

    return *this;
}

Quat Quat::operator-(const Quat& q) const {
    return {x - q.x, y - q.y, z - q.z, w - q.w};
}

Quat& Quat::operator-=(const Quat& q) {
    w -= q.w;
    x -= q.x;
    y -= q.y;
    z -= q.z;

    return *this;
}

Quat Quat::operator*(float scalar) const {
    return {x * scalar, y * scalar, z * scalar, w * scalar};
}

Quat& Quat::operator*=(float scalar) {
    x *= scalar;
    y *= scalar;
    z *= scalar;
    w *= scalar;

    return *this;
}

Quat Quat::operator*(const Quat& q) const {
    Quat r;
    r.w = w * q.w - x * q.x - y * q.y - z * q.z;
    r.x = x * q.w + w * q.x + y * q.z - z * q.y;
    r.y = y * q.w + w * q.y + z * q.x - x * q.z;
    r.z = z * q.w + w * q.z + x * q.y - y * q.x;

    return r;
}

Quat& Quat::operator*=(const Quat& q) {
    Quat r = (*this * q);
    *this = r;

    return *this;
}

Quat Quat::operator/(float scalar) const {
    return {x / scalar, y / scalar, z / scalar, w / scalar};
}

Quat& Quat::operator/=(float scalar) {
    x /= scalar;
    y /= scalar;
    z /= scalar;
    w /= scalar;

    return *this;
}

float Quat::operator[](uint32 idx) const {
    if (idx == 0)
        return w;

    if (idx == 1)
        return x;

    if (idx == 2)
        return y;

    return z;
}

float& Quat::operator[](uint32 idx) {
    if (idx == 0)
        return w;

    if (idx == 1)
        return x;

    if (idx == 2)
        return y;

    return z;
}

Quat Quat::conj() const {
    return {-x, -y, -z, w};
}

float Quat::lengthSqr() const {
    return w * w + x * x + y * y + z * z;
}

float Quat::length() const {
    return std::sqrt(lengthSqr());
}

void Quat::normalize() {
    float lenSqr = lengthSqr();
    if (lenSqr > 0)
        *this /= std::sqrt(lenSqr);
}

Matrix4x4 Quat::toMatrix() const {
    float xx = x * x, yy = y * y, zz = z * z;
    float xy = x * y, xz = x * z, yz = y * z;
    float wx = x * w, wy = y * w, wz = z * w;

    Matrix4x4 m;
    m(0, 0) = 1.0f - 2.0f * (yy + zz);
    m(0, 1) = 2.0f * (xy - wz);
    m(0, 2) = 2.0f * (xz + wy);

    m(1, 0) = 2.0f * (xy + wz);
    m(1, 1) = 1.0f - 2.0f * (xx + zz);
    m(1, 2) = 2.0f * (yz - wx);

    m(2, 0) = 2.0f * (xz - wy);
    m(2, 1) = 2.0f * (yz + wx);
    m(2, 2) = 1.0f - 2.0f * (xx + yy);

    return m;
}

std::istream& math::operator>>(std::istream& is, Quat& q) {
    is >> q.x;
    is >> q.y;
    is >> q.z;
    is >> q.w;
    return is;
}

std::ostream& math::operator<<(std::ostream& os, const Quat& q) {
    os << "Quat: [" << q.x << ", " << q.y << ", " << q.z << ", " << q.w << "]\n";
    return os;
}

Quat math::operator*(float scalar, const Quat& q) {
    return q * scalar;
}

float math::Dot(const Quat& q1, const Quat& q2) {
    return q1.w * q2.w + q1.x * q2.x + q1.y * q2.y + q1.z * q2.z;
}

Quat math::Normalize(const Quat& q) {
    float lenSqr = q.lengthSqr();
    if (lenSqr > 0)
        return q / std::sqrt(lenSqr);
    return Quat(Vector3(0), 0);
}

Quat math::Slerp(float t, const Quat& q1, const Quat& q2) {
    float cosTheta = Dot(q1, q2);
    if (cosTheta > ONE_MINUS_EPSILON)
        return Normalize((1 - t) * q1 + t * q2);
    else {
        float theta = SafeAcos(cosTheta);
        float thetap = theta * t;

        Quat qperp = Normalize(q2 - q1 * cosTheta);

        return q1 * std::cos(thetap) + qperp * std::sin(thetap);
    }
}

Vector3 math::Rotate(const Quat& q, const Vector3& v) {
    Vector3 u(q.x, q.y, q.z);
    float s = q.w;

    return 2.0f * Dot(u, v) * u + (s * s - Dot(u, u)) * v + 2.0f * s * Cross(u, v);
}

Quat math::AxisAngle(const Vector3& axis, float angle) {
    auto cos = std::cos(angle * 0.5f);
    auto sin = std::sin(angle * 0.5f);

    return {sin * axis, cos};
}

Quat math::RotationAlign(const Vector3& from, const Vector3& to) {
    auto cosTheta = Dot(from, to);

    // Handle from = -to
    if (cosTheta < -0.999999f) {
        const Vector3 xAxis{1, 0, 0};
        const Vector3 yAxis{0, 1, 0};

        // Find non colinear vector and compute a perpendicular axis
        Vector3 aux = std::abs(from.x) > std::abs(from.y) ? yAxis : xAxis;
        Vector3 perp = Cross(from, aux);
        return Normalize(AxisAngle(perp, math::PI));
    }

    auto axis = Cross(from, to);
    return Normalize(Quat{axis.x, axis.y, axis.z, 1.0f + cosTheta});
}