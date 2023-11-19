#include <Quat.h>

#include <PBRMath.h>

using namespace pbr;
using namespace pbr::math;

Quat::Quat() : x(0), y(0), z(0), w(1) {}
Quat::Quat(const Vector3& v, float w) : x(v.x), y(v.y), z(v.z), w(w) {}
Quat::Quat(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

Quat::Quat(const Matrix4x4& mat) {
    const Matrix3x3 m = Matrix3x3(mat);

    float trace = m.trace();
    if (trace > 0.f) {
        float s = std::sqrt(trace + 1.0f);
        w = s / 2.0f;
        s = 0.5f / s;
        x = s * (m.m32 - m.m23);
        y = s * (m.m13 - m.m31);
        z = s * (m.m21 - m.m12);
    } else {
        // Compute largest of x, y, or z, then remaining components
        const int nxt[3] = {1, 2, 0};
        float q[3];
        int i = 0;
        if (m.m[1][1] > m.m[0][0])
            i = 1;
        if (m.m[2][2] > m.m[i][i])
            i = 2;

        int j = nxt[i];
        int k = nxt[j];
        float s = std::sqrt(1.0f + (m.m[i][i] - (m.m[j][j] + m.m[k][k])));
        q[i] = s * 0.5f;
        if (s != 0.f)
            s = 0.5f / s;
        w = (m.m[j][k] - m.m[k][j]) * s;
        q[j] = (m.m[j][i] + m.m[i][j]) * s;
        q[k] = (m.m[k][i] + m.m[i][k]) * s;

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
    m.m11 = 1.0f - 2.0f * (yy + zz);
    m.m12 = 2.0f * (xy - wz);
    m.m13 = 2.0f * (xz + wy);

    m.m21 = 2.0f * (xy + wz);
    m.m22 = 1.0f - 2.0f * (xx + zz);
    m.m23 = 2.0f * (yz - wx);

    m.m31 = 2.0f * (xz - wy);
    m.m32 = 2.0f * (yz + wx);
    m.m33 = 1.0f - 2.0f * (xx + yy);

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

float math::dot(const Quat& q1, const Quat& q2) {
    return q1.w * q2.w + q1.x * q2.x + q1.y * q2.y + q1.z * q2.z;
}

Quat math::normalize(const Quat& q) {
    float lenSqr = q.lengthSqr();
    if (lenSqr > 0)
        return q / std::sqrt(lenSqr);
    return Quat(Vector3(0), 0);
}

Quat math::slerp(float t, const Quat& q1, const Quat& q2) {
    float cosTheta = dot(q1, q2);
    if (cosTheta > ONE_MINUS_EPSILON)
        return normalize((1 - t) * q1 + t * q2);
    else {
        float theta = acosSafe(cosTheta);
        float thetap = theta * t;

        Quat qperp = normalize(q2 - q1 * cosTheta);

        return q1 * std::cos(thetap) + qperp * std::sin(thetap);
    }
}

Vector3 math::rotate(const Quat& q, const Vector3& v) {
    Vector3 u(q.x, q.y, q.z);
    float s = q.w;

    return 2.0f * dot(u, v) * u + (s * s - dot(u, u)) * v + 2.0f * s * cross(u, v);
}