#ifndef PBR_SPECTRUM_H
#define PBR_SPECTRUM_H

#include <PBR.h>
#include <PBRMath.h>

using namespace pbr::math;

namespace pbr {

static const float Gamma = 2.2f;

class RGBSpectrum {
public:
    float r = 0, g = 0, b = 0;

    RGBSpectrum() = default;
    RGBSpectrum(float r, float g, float b) : r(r), g(g), b(b) {}
    explicit RGBSpectrum(float s) : r(s), g(s), b(s) {}
    explicit RGBSpectrum(const Vec3& v);

    RGBSpectrum operator+(const RGBSpectrum& rgb) const;
    RGBSpectrum& operator+=(const RGBSpectrum& rgb);

    RGBSpectrum operator-(const RGBSpectrum& rgb) const;
    RGBSpectrum& operator-=(const RGBSpectrum& rgb);

    RGBSpectrum operator*(float scalar) const;
    RGBSpectrum& operator*=(float scalar);

    RGBSpectrum operator*(const RGBSpectrum& rgb) const;
    RGBSpectrum& operator*=(const RGBSpectrum& rgb);

    RGBSpectrum operator/(float scalar) const;
    RGBSpectrum& operator/=(float scalar);

    RGBSpectrum operator/(const RGBSpectrum& scalar) const;
    RGBSpectrum& operator/=(const RGBSpectrum& scalar);

    float operator[](unsigned int idx) const;
    float& operator[](unsigned int idx);

    float max() const;
    float min() const;

    void clamp(float low, float high);
    bool isBlack() const;
    float lum() const;

    static const RGBSpectrum BLACK;
};

// Standard input/ouput
std::istream& operator>>(std::istream& is, RGBSpectrum& spectrum);
std::ostream& operator<<(std::ostream& os, const RGBSpectrum& spectrum);

RGBSpectrum operator*(float scalar, const RGBSpectrum& rgb);

RGBSpectrum clamp(const RGBSpectrum& rgb, float low, float high);
RGBSpectrum exp(const RGBSpectrum& rgb);
RGBSpectrum lerp(const RGBSpectrum& rgb1, const RGBSpectrum& rgb2, float t);

typedef RGBSpectrum Color;

} // namespace pbr

#endif