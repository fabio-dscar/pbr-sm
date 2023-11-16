#ifndef __PBR_IMAGE_H__
#define __PBR_IMAGE_H__

#include <PBR.h>
#include <PBRMath.h>

namespace pbr {

enum class ImageType : uint32 {
    IMGTYPE_1D = 0,
    IMGTYPE_2D = 1,
    IMGTYPE_3D = 2,
    IMGTYPE_CUBE = 3,
    IMGTYPE_UNKNOWN = 10
};

enum class ImageComponent : uint32 {
    UBYTE = 0,
    BYTE = 1,
    USHORT = 2,
    SHORT = 3,
    UINT = 4,
    INT = 5,
    FLOAT = 6,
    HALF = 7,

    UNKNOWN = 8
};

enum class ImageFormat : uint32 {
    IMGFMT_UNKNOWN = 0,

    // Unsigned IMGFMTs
    IMGFMT_R8 = 1,
    IMGFMT_RG8 = 2,
    IMGFMT_RGB8 = 3,
    IMGFMT_RGBA8 = 4,

    IMGFMT_R16 = 5,
    IMGFMT_RG16 = 6,
    IMGFMT_RGB16 = 7,
    IMGFMT_RGBA16 = 8,

    // Signed IMGFMTs
    IMGFMT_R8S = 9,
    IMGFMT_RG8S = 10,
    IMGFMT_RGB8S = 11,
    IMGFMT_RGBA8S = 12,

    IMGFMT_R16S = 13,
    IMGFMT_RG16S = 14,
    IMGFMT_RGB16S = 15,
    IMGFMT_RGBA16S = 16,

    // Float IMGFMTs
    IMGFMT_R16F = 17,
    IMGFMT_RG16F = 18,
    IMGFMT_RGB16F = 19,
    IMGFMT_RGBA16F = 20,

    IMGFMT_R32F = 21,
    IMGFMT_RG32F = 22,
    IMGFMT_RGB32F = 23,
    IMGFMT_RGBA32F = 24,

    // Signed integer IMGFMTs
    IMGFMT_R16I = 25,
    IMGFMT_RG16I = 26,
    IMGFMT_RGB16I = 27,
    IMGFMT_RGBA16I = 28,

    IMGFMT_R32I = 29,
    IMGFMT_RG32I = 30,
    IMGFMT_RGB32I = 31,
    IMGFMT_RGBA32I = 32,

    // Unsigned integer IMGFMTs
    IMGFMT_R16UI = 33,
    IMGFMT_RG16UI = 34,
    IMGFMT_RGB16UI = 35,
    IMGFMT_RGBA16UI = 36,

    IMGFMT_R32UI = 37,
    IMGFMT_RG32UI = 38,
    IMGFMT_RGB32UI = 39,
    IMGFMT_RGBA32UI = 40,

    // Packed IMGFMTs
    IMGFMT_RGBE8 = 41,
    IMGFMT_RGB9E5 = 42,
    IMGFMT_RG11B10F = 43,
    IMGFMT_RGB565 = 44,
    IMGFMT_RGBA4 = 45,
    IMGFMT_RGB10A2 = 46,

    // Depth IMGFMTs
    IMGFMT_D16 = 47,
    IMGFMT_D24 = 48,
    IMGFMT_D24S8 = 49,
    IMGFMT_D32F = 50,

    // Compressed IMGFMTs
    IMGFMT_DXT1 = 51,
    IMGFMT_DXT3 = 52,
    IMGFMT_DXT5 = 53,
    IMGFMT_ATI1N = 54,
    IMGFMT_ATI2N = 55,
    IMGFMT_BC7 = 56
};

uint32 formatToNumChannels(ImageFormat format);
uint32 formatToBytesPerChannel(ImageFormat format);
ImageComponent formatToImgComp(ImageFormat format);
uint32 mipDimension(uint32 baseDim, uint32 level);

class Image {
public:
    using enum pbr::ImageFormat;
    using enum pbr::ImageComponent;
    using enum pbr::ImageType;

    Image();
    ~Image();

    void init(ImageFormat format, uint32 width, uint32 height, uint32 depth,
              uint32 numLevels = 1);

    bool loadImage(const std::string& filePath);

    // Loads image from memory
    bool loadImage(ImageFormat format, uint32 width, uint32 height, uint32 depth,
                   const uint8* data, uint32 numLevels = 1);

    // Loads a mipmap level from memory
    bool loadImage(const uint8* data, uint32 lvl);

    bool saveImage(const std::string& filePath, uint32 lvl = 0) const;
    bool saveMipMap(const std::string& filePath) const;

    ImageType type() const;
    ImageFormat format() const;
    ImageComponent compType() const;

    bool hasMipMap() const;
    uint32 numLevels() const;

    int32 width() const;
    int32 height() const;
    int32 depth() const;

    uint8* data(uint32 lvl = 0) const;

    bool flipX();
    bool flipY();
    bool toGrayscale();
    bool toneMap(float exposure = 1.0f);

    uint32 size(uint32 lvl = 0) const;
    uint32 totalSize() const;
    uint32 numChannels() const;

private:
    bool loadPNG(const std::string& filePath);
    bool loadIMG(const std::string& filePath);

    bool saveIMG(const std::string& filePath) const;
    bool savePNG(const std::string& filePath, uint32 lvl = 0) const;

    ImageFormat _format;
    ImageType _type;
    int32 _width;
    int32 _height;
    int32 _depth;
    uint32 _numLevels;

    std::unique_ptr<uint8[]> _data;
};

enum CubemapFace : uint32 {
    CUBE_X_POS = 0,
    CUBE_X_NEG = 1,
    CUBE_Y_POS = 2,
    CUBE_Y_NEG = 3,
    CUBE_Z_POS = 4,
    CUBE_Z_NEG = 5
};

class Cubemap {
public:
    Cubemap();

    void init(ImageFormat format, uint32 width, uint32 height, uint32 numLevels = 1);

    bool loadCubemap(const std::string& filePath);
    bool loadCubemap(const std::string paths[6]);
    bool loadCubemap(ImageFormat format, uint32 width, uint32 height, const uint8* data,
                     uint32 numLevels = 1);
    bool loadFace(CubemapFace face, const uint8* data, uint32 lvl);

    bool saveCubemap(const std::string& filePath);

    uint8* data(CubemapFace face, uint32 lvl = 0) const;

    ImageType type() const;
    ImageFormat format() const;
    ImageComponent compType() const;

    int32 width() const;
    int32 height() const;

    const Image* faces() const;
    const Image* face(CubemapFace face) const;
    Image* face(CubemapFace face);

    uint32 size(CubemapFace face, uint32 lvl = 0) const;
    uint32 totalSize() const;
    uint32 numChannels() const;

    bool hasMipMap() const;
    uint32 numLevels() const;

private:
    bool loadCUBE(const std::string& filePath);
    bool saveCUBE(const std::string& filePath) const;

    Image _faces[6];
};

struct IMGHeader {
    char id[4];
    uint32 fmt;
    uint32 width;
    uint32 height;
    uint32 depth;
    uint32 compSize;
    uint32 totalSize;
    uint32 levels;
}; // 32 Bytes

struct CUBEHeader {
    char id[4];
    uint32 fmt;
    uint32 width;
    uint32 height;
    uint32 compSize;
    uint32 totalSize;
    uint32 levels;
}; // 28 Bytes

} // namespace pbr

#endif