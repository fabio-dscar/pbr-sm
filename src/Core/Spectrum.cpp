#include <Spectrum.h>

#include <PBR.h>

using namespace pbr;

const RGBSpectrum RGBSpectrum::BLACK = RGBSpectrum(0);

RGBSpectrum::RGBSpectrum(const Vec3& v) : r(v.x), g(v.y), b(v.z) {}

RGBSpectrum RGBSpectrum::operator+(const RGBSpectrum& rgb) const {
    return {r + rgb.r, g + rgb.g, b + rgb.b};
}

RGBSpectrum& RGBSpectrum::operator+=(const RGBSpectrum& rgb) {
    r += rgb.r;
    g += rgb.g;
    b += rgb.b;
    return *this;
}

RGBSpectrum RGBSpectrum::operator-(const RGBSpectrum& rgb) const {
    return {r - rgb.r, g - rgb.g, b - rgb.b};
}

RGBSpectrum& RGBSpectrum::operator-=(const RGBSpectrum& rgb) {
    r -= rgb.r;
    g -= rgb.g;
    b -= rgb.b;
    return *this;
}

RGBSpectrum RGBSpectrum::operator*(float scalar) const {
    return {r * scalar, g * scalar, b * scalar};
}

RGBSpectrum& RGBSpectrum::operator*=(float scalar) {
    r *= scalar;
    g *= scalar;
    b *= scalar;
    return *this;
}

RGBSpectrum RGBSpectrum::operator*(const RGBSpectrum& rgb) const {
    return {r * rgb.r, g * rgb.g, b * rgb.b};
}

RGBSpectrum& RGBSpectrum::operator*=(const RGBSpectrum& rgb) {
    r *= rgb.r;
    g *= rgb.g;
    b *= rgb.b;
    return *this;
}

RGBSpectrum RGBSpectrum::operator/(float scalar) const {
    return RGBSpectrum(r / scalar, g / scalar, b / scalar);
}

RGBSpectrum& RGBSpectrum::operator/=(float scalar) {
    r /= scalar;
    g /= scalar;
    b /= scalar;
    return *this;
}

RGBSpectrum RGBSpectrum::operator/(const RGBSpectrum& rgb) const {
    return {r / rgb.r, g / rgb.g, b / rgb.b};
}

RGBSpectrum& RGBSpectrum::operator/=(const RGBSpectrum& rgb) {
    r /= rgb.r;
    g /= rgb.g;
    b /= rgb.b;
    return *this;
}

float RGBSpectrum::operator[](uint32 idx) const {
    if (idx == 0)
        return r;

    if (idx == 1)
        return g;

    return b;
}

float& RGBSpectrum::operator[](uint32 idx) {
    if (idx == 0)
        return r;

    if (idx == 1)
        return g;

    return b;
}

float RGBSpectrum::max() const {
    return std::max(r, std::max(g, b));
}

float RGBSpectrum::min() const {
    return std::min(r, std::min(g, b));
}

void RGBSpectrum::clamp(float low, float high) {
    r = math::Clamp(r, low, high);
    g = math::Clamp(g, low, high);
    b = math::Clamp(b, low, high);
}

bool RGBSpectrum::isBlack() const {
    return (r == 0 && g == 0 && b == 0);
}

float RGBSpectrum::lum() const {
    // RGB -> Y component of XYZ space
    return 0.212671 * r + 0.715160 * g + 0.072169 * b;
}

std::istream& pbr::operator>>(std::istream& is, RGBSpectrum& s) {
    is >> s.r;
    is >> s.g;
    is >> s.b;
    return is;
}

std::ostream& pbr::operator<<(std::ostream& os, const RGBSpectrum& s) {
    os << "RGBSpectrum: [" << s.r << ", " << s.g << ", " << s.b << "]";
    return os;
}

RGBSpectrum pbr::operator*(float scalar, const RGBSpectrum& rgb) {
    return rgb * scalar;
}

RGBSpectrum pbr::clamp(const RGBSpectrum& rgb, float low, float high) {
    float r = math::Clamp(rgb.r, low, high);
    float g = math::Clamp(rgb.g, low, high);
    float b = math::Clamp(rgb.b, low, high);
    return {r, g, b};
}

RGBSpectrum pbr::exp(const RGBSpectrum& rgb) {
    float r = std::exp(rgb.r);
    float g = std::exp(rgb.g);
    float b = std::exp(rgb.b);
    return {r, g, b};
}

RGBSpectrum pbr::lerp(const RGBSpectrum& rgb1, const RGBSpectrum& rgb2, float t) {
    return (1.0 - t) * rgb1 + t * rgb2;
}