#ifndef __PBR_TEXTURE_H__
#define __PBR_TEXTURE_H__

#include <PBR.h>
#include <PBRMath.h>

#include <Image.h>

using namespace pbr::math;

namespace pbr {

enum TexWrapMode {
    WRAP_REPEAT = 0,
    WRAP_MIRRORED_REPEAT = 1,
    WRAP_CLAMP_EDGE = 2,
    WRAP_CLAMP_BORDER = 3
};

enum TexFilterMode {
    FILTER_NEAREST = 0,
    FILTER_LINEAR = 1,
    FILTER_NEAREST_MIP_NEAREST = 2,
    FILTER_LINEAR_MIP_NEAREST = 3,
    FILTER_NEAREST_MIP_LINEAR = 4,
    FILTER_LINEAR_MIP_LINEAR = 5
};

class PBR_SHARED TexSampler {
public:
    TexSampler();
    TexSampler(TexWrapMode sWrap, TexWrapMode tWrap, TexFilterMode min,
               TexFilterMode mag);
    TexSampler(TexWrapMode sWrap, TexWrapMode tWrap, TexWrapMode rWrap, TexFilterMode min,
               TexFilterMode mag);

    TexWrapMode sWrap() const;
    TexWrapMode tWrap() const;
    TexWrapMode rWrap() const;

    TexFilterMode minFilter() const;
    TexFilterMode magFilter() const;

    void setWrapMode(TexWrapMode sWrap, TexWrapMode tWrap);
    void setWrapMode(TexWrapMode sWrap, TexWrapMode tWrap, TexWrapMode rWrap);
    void setFilterMode(TexFilterMode min, TexFilterMode mag);

    void setNumSamples(uint32 numSamples);
    uint32 numSamples() const;

private:
    TexWrapMode _wrapS;
    TexWrapMode _wrapT;
    TexWrapMode _wrapR;

    TexFilterMode _min;
    TexFilterMode _mag;

    uint32 _numSamples = 0;
};

enum PixelType {
    TYPE_UBYTE = 0,
    TYPE_BYTE = 1,
    TYPE_USHORT = 2,
    TYPE_SHORT = 3,
    TYPE_UINT = 4,
    TYPE_INT = 5,
    TYPE_FLOAT = 6
};

struct TexFormat {
    ImageType imgType;
    ImageFormat imgFmt;
    ImageComponent pType;
    uint32 levels;
};

class PBR_SHARED Texture {
public:
    Texture(int32 w, int32 h, int32 d, const TexSampler& sampler, const TexFormat& fmt);
    Texture(RRID resId, int32 w, int32 h, int32 d, const TexSampler& sampler,
            const TexFormat& fmt);

    void setFormat(const TexFormat& format);

    int32 width() const;
    int32 height() const;
    int32 depth() const;

    const TexFormat& format() const;
    const TexSampler& sampler() const;

    RRID rrid() const { return _id; }

private:
    TexFormat _format;
    TexSampler _sampler;
    int32 _width;
    int32 _height;
    int32 _depth;
    RRID _id = -1;
};

} // namespace pbr

#endif