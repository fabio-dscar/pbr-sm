#ifndef PBR_PBR_H
#define PBR_PBR_H

#if defined(_WIN32) || defined(_WIN64)
#define PBR_WINDOWS
#elif defined(__linux__)
#define PBR_LINUX
#endif

#include <cstdint>
#include <type_traits>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <memory>
#include <string>
#include <cstring>
#include <iomanip>
#include <cassert>
#include <format>
#include <vector>
#include <array>
#include <unordered_map>

#include <Check.h>

#if defined(_MSC_VER)
#define PBR_MSVC
#include <float.h>
#include <intrin.h>
#pragma warning(disable : 4244) // int to float warning
#pragma warning(disable : 4843) // double to float warning
#pragma warning(disable : 4305) // double const to float
#endif

#if defined(DEBUG)
#define THROW_ERROR(...)                                                                 \
    throw std::runtime_error(std::format("{} ({}): {}", std::string(__FILE__),           \
                                         std::to_string(__LINE__),                       \
                                         std::format(__VA_ARGS__)))
#else
#define THROW_ERROR(...) throw std::runtime_error(std::format(__VA_ARGS__))
#endif

#define THROW(...) THROW_ERROR(__VA_ARGS__)

namespace pbr {

// Integer types
typedef std::uint8_t uint8;
typedef std::uint16_t uint16;
typedef std::uint32_t uint32;
typedef std::uint64_t uint64;

typedef std::int8_t int8;
typedef std::int16_t int16;
typedef std::int32_t int32;
typedef std::int64_t int64;

typedef uint32 RRID;

template<typename T>
using sref = std::shared_ptr<T>;

class ParameterMap;

} // namespace pbr

#endif