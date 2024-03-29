#ifndef PBR_LOG_H
#define PBR_LOG_H

#define SPDLOG_ACTIVE_LEVEL 0
#include <spdlog/spdlog.h>

#define LOG_LEVEL_TRACE 0
#define LOG_LEVEL_DEBUG 1
#define LOG_LEVEL_INFO 2
#define LOG_LEVEL_WARN 3
#define LOG_LEVEL_ERROR 4
#define LOG_LEVEL_FATAL 5
#define LOG_LEVEL_OFF 6

#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_LEVEL_TRACE
#endif

#if LOG_LEVEL <= LOG_LEVEL_TRACE
#define LOG_TRACE(...) SPDLOG_TRACE(__VA_ARGS__)
#define LOG_TRACE_L(logger, ...) SPDLOG_LOGGER_TRACE(logger, __VA_ARGS__)
#else
#define LOG_TRACE(...) (void)0
#define LOG_TRACE_L(logger, ...) (void)0
#endif

#if LOG_LEVEL <= LOG_LEVEL_DEBUG
#define LOG_DEBUG(...) SPDLOG_DEBUG(__VA_ARGS__)
#define LOG_DEBUG_L(logger, ...) SPDLOG_LOGGER_DEBUG(logger, __VA_ARGS__)
#else
#define LOG_DEBUG(...) (void)0
#define LOG_DEBUG_L(logger, ...) (void)0
#endif

#if LOG_LEVEL <= LOG_LEVEL_INFO
#define LOG_INFO(...) SPDLOG_INFO(__VA_ARGS__)
#define LOG_INFO_L(logger, ...) SPDLOG_LOGGER_INFO(logger, __VA_ARGS__)
#else
#define LOG_INFO(...) (void)0
#define LOG_INFO_L(logger, ...) (void)0
#endif

#if LOG_LEVEL <= LOG_LEVEL_WARN
#define LOG_WARN(...) SPDLOG_WARN(__VA_ARGS__)
#define LOG_WARN_L(logger, ...) SPDLOG_LOGGER_WARN(logger, __VA_ARGS__)
#else
#define LOG_WARN(...) (void)0
#define LOG_WARN_L(logger, ...) (void)0
#endif

#if LOG_LEVEL <= LOG_LEVEL_ERROR
#define LOG_ERROR(...) SPDLOG_ERROR(__VA_ARGS__)
#define LOG_ERROR_L(logger, ...) SPDLOG_LOGGER_ERROR(logger, __VA_ARGS__)
#else
#define LOG_ERROR(...) (void)0
#define LOG_ERROR_L(logger, ...) (void)0
#endif

#if LOG_LEVEL <= LOG_LEVEL_FATAL
#define LOG_FATAL(...) SPDLOG_CRITICAL(__VA_ARGS__)
#define LOG_FATAL_L(logger, ...) SPDLOG_LOGGER_CRITICAL(logger, __VA_ARGS__)
#else
#define LOG_FATAL(...) (void)0
#define LOG_FATAL_L(logger, ...) (void)0
#endif

#define LOGT(...) LOG_TRACE(__VA_ARGS__)
#define LOGD(...) LOG_DEBUG(__VA_ARGS__)
#define LOGI(...) LOG_INFO(__VA_ARGS__)
#define LOGW(...) LOG_WARN(__VA_ARGS__)

namespace pbr {

using Logger = spdlog::logger;

void InitLogger();
void PrintStackTrace();

std::shared_ptr<Logger> GetDefaultLogger();
std::shared_ptr<Logger> GetLogger(const std::string& name);

} // namespace pbr

#endif