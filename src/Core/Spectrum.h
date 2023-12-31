#ifndef __PBR_SPECTRUM_H__
#define __PBR_SPECTRUM_H__

#include <PBR.h>
#include <PBRMath.h>

using namespace pbr::math;

namespace pbr {

static const float Gamma = 2.2f;

class PBR_SHARED RGBSpectrum {
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

    float operator[](uint32 idx) const;
    float& operator[](uint32 idx);

    float max() const;
    float min() const;

    void clamp(float low, float high);
    bool isBlack() const;
    float lum() const;

    static const RGBSpectrum BLACK;
};

// Standard input/ouput
PBR_SHARED std::istream& operator>>(std::istream& is, RGBSpectrum& spectrum);
PBR_SHARED std::ostream& operator<<(std::ostream& os, const RGBSpectrum& spectrum);

PBR_SHARED RGBSpectrum operator*(float scalar, const RGBSpectrum& rgb);

PBR_SHARED RGBSpectrum clamp(const RGBSpectrum& rgb, float low, float high);
PBR_SHARED RGBSpectrum exp(const RGBSpectrum& rgb);
PBR_SHARED RGBSpectrum lerp(const RGBSpectrum& rgb1, const RGBSpectrum& rgb2, float t);

typedef RGBSpectrum Color;

} // namespace pbr

#endif