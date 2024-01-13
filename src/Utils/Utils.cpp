#include <Utils.h>

#include <Image.h>

#include <filesystem>
#include <fstream>
#include <iostream>

#include <Mesh.h>
#include <PBRMaterial.h>
#include <RenderInterface.h>
#include <Texture.h>

#include <Resources.h>

#include <format>

#include <lodepng/lodepng.h>
#include <lodepng/lodepng_util.h>

using namespace pbr;

namespace {
// clang-format off
std::unique_ptr<Image> LoadPNGImage(const fs::path& filePath) {
    std::vector<unsigned char> png;
    std::vector<unsigned char> image;

    unsigned error = lodepng::load_file(png, filePath);
    if (error)
        FATAL("Error loading png file {}. {}", filePath.string(),
              lodepng_error_text(error));

    auto pngInfo = lodepng::getPNGHeaderInfo(png);

    if (pngInfo.color.bitdepth == 16)
        FATAL("Error loading png file {}. 16 bit depth not supported.",
              filePath.string());

    lodepng::State state;
    state.info_raw = pngInfo.color;

    unsigned width, height;
    error = lodepng::decode(image, width, height, state, png);
    if (error)
        FATAL("Error decoding png file {}. {}", filePath.string(),
              lodepng_error_text(error));

    int numChannels = 0;
    switch (state.info_png.color.colortype) {
    case LCT_GREY: numChannels = 1; break;
    case LCT_GREY_ALPHA: numChannels = 2; break;
    case LCT_RGB: numChannels = 3; break;
    case LCT_RGBA: numChannels = 4; break;
    default:
        FATAL("Error decoding png. Unsupported pixel format.");
    }

    ImageFormat fmt{.pFmt = PixelFormat::U8,
                    .width = static_cast<int>(width),
                    .height = static_cast<int>(height),
                    .nChannels = numChannels};

    return std::make_unique<Image>(fmt, reinterpret_cast<std::byte*>(image.data()));
}
// clang-format on

const std::array LodeNumChanToType = {LCT_GREY, LCT_GREY_ALPHA, LCT_RGB, LCT_RGBA};

void SavePNGImage(const fs::path& filePath, const Image& image) {
    lodepng::State state;
    lodepng_state_init(&state);

    const ImageFormat fmt = image.format();
    state.info_raw.colortype = LodeNumChanToType[fmt.nChannels - 1];
    state.info_raw.bitdepth = ComponentSize(fmt.pFmt) * 8;

    std::vector<unsigned char> png;
    auto imgData = reinterpret_cast<const unsigned char*>(image.data());
    unsigned error = lodepng::encode(png, imgData, fmt.width, fmt.height, state);
    if (error)
        FATAL("Error encoding png file {}. {}", filePath.string(),
              lodepng_error_text(error));

    lodepng_state_cleanup(&state);

    error = lodepng::save_file(png, filePath);
    if (error)
        FATAL("Error saving png file {}. {}", filePath.string(),
              lodepng_error_text(error));
}

struct ImgHeader {
    std::uint8_t id[4] = {'I', 'M', 'G', ' '};
    std::uint32_t fmt;
    std::int32_t width;
    std::int32_t height;
    std::int32_t depth;
    std::uint32_t compSize;
    std::int32_t numChannels;
    std::uint32_t totalSize;
    std::uint32_t levels;
};

std::unique_ptr<Image> LoadImgFormatImage(const fs::path& filePath) {
    std::ifstream file(filePath, std::ios::in | std::ios::binary);
    if (file.fail()) {
        perror(filePath.c_str());
        return nullptr;
    }

    ImgHeader header;
    file.read(reinterpret_cast<char*>(&header), sizeof(ImgHeader));

    ImageFormat fmt{.pFmt = static_cast<PixelFormat>(header.fmt),
                    .width = header.width,
                    .height = header.height,
                    .nChannels = header.numChannels};

    auto ptr = std::make_unique<std::byte[]>(header.totalSize);
    file.read(reinterpret_cast<char*>(ptr.get()), header.totalSize);

    return std::make_unique<Image>(fmt, ptr.get(), header.levels);
}

void SaveImgFormatImage(const fs::path& filePath, const Image& image) {
    const auto imgFmt = image.format();

    ImgHeader header;
    header.fmt = static_cast<std::uint32_t>(imgFmt.pFmt);
    header.width = imgFmt.width;
    header.height = imgFmt.height;
    header.depth = 0;
    header.compSize = ComponentSize(imgFmt.pFmt);
    header.numChannels = imgFmt.nChannels;
    header.totalSize = image.size();
    header.levels = image.numLevels();

    std::ofstream file(filePath, std::ios_base::out | std::ios_base::binary);
    file.write((const char*)&header, sizeof(ImgHeader));
    file.write(reinterpret_cast<const char*>(image.data()), header.totalSize);
}

struct CubeHeader {
    std::uint8_t id[4] = {'C', 'U', 'B', 'E'};
    std::uint32_t fmt;
    std::int32_t width;
    std::int32_t height;
    std::uint32_t compSize;
    std::int32_t numChannels;
    std::uint32_t totalSize;
    std::int32_t levels;
};

std::unique_ptr<CubeImage> LoadCubeFormatCube(const fs::path& filePath) {
    std::ifstream file(filePath, std::ios::in | std::ios::binary | std::ios::ate);
    if (file.fail())
        FATAL("Failed to open cubemap file {}", filePath.string());

    auto fileSize = file.tellg();
    if (fileSize == -1)
        FATAL("Failed to compute file size for {}", filePath.string());
    file.seekg(0, std::ios::beg);

    CubeHeader header;
    file.read(reinterpret_cast<char*>(&header), sizeof(CubeHeader));

    const ImageFormat faceFmt{.pFmt = static_cast<PixelFormat>(header.fmt),
                              .width = header.width,
                              .height = header.height,
                              .nChannels = header.numChannels};

    CHECK_EQ(static_cast<std::size_t>(fileSize),
             sizeof(CubeHeader) + ImageSize(faceFmt, header.levels) * 6);

    const auto faceSize = header.totalSize / 6;

    auto cubePtr = std::make_unique<CubeImage>(faceFmt, header.levels);
    auto& cube = *cubePtr;
    for (int face = 0; face < 6; ++face)
        file.read(reinterpret_cast<char*>(cube[face].data()), faceSize);

    return cubePtr;
}

} // namespace

std::optional<std::string> util::ReadTextFile(const fs::path& filePath) {
    std::ifstream file(filePath, std::ios_base::in | std::ios_base::ate);
    if (file.fail()) {
        perror(filePath.c_str());
        return {};
    }

    std::string contents;
    contents.reserve(static_cast<std::size_t>(file.tellg()));
    file.seekg(0, std::ios::beg);

    contents.assign((std::istreambuf_iterator<char>(file)),
                    std::istreambuf_iterator<char>());

    return contents;
}

std::unique_ptr<Image> util::LoadImage(const fs::path& filePath) {
    auto ext = filePath.extension().string();
    if (ext == ".png")
        return LoadPNGImage(filePath);
    else if (ext == ".img") {
        return LoadImgFormatImage(filePath);
    }

    FATAL("Unsupported format {}", ext);
}

void util::SaveImage(const fs::path& filePath, const Image& image) {
    auto ext = filePath.extension().string();
    if (ext == ".png")
        SavePNGImage(filePath, image);
    else if (ext == ".img")
        SaveImgFormatImage(filePath, image);
    else
        FATAL("Unsupported format.");
}

std::unique_ptr<CubeImage> util::LoadCubemap(const fs::path& filePath) {
    auto ext = filePath.extension().string();
    if (ext == ".cube")
        return LoadCubeFormatCube(filePath);
    else
        FATAL("Unsupported format.");
}

RRID util::LoadTexture(const fs::path& path) {
    if (!fs::exists(path)) {
        LOG_ERROR("Couldn't find texture {}. Assigning 'unset' texture.", path.string());
        return Resource.get<Texture>("null")->id();
    }

    return CreateNamedTexture(path.string(), path)->id();
}

void GLAPIENTRY util::OpenGLErrorCallback(GLenum, GLenum type, GLuint, GLenum severity,
                                          GLsizei, const GLchar* message, const void*) {
    std::cerr << std::format("[OPENGL] Type = 0x{:x}, Severity = 0x{:x}, Message = {}\n",
                             type, severity, message);
}