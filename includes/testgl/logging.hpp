#pragma once

#include <iostream>

// ANSI escape codes for colored terminal output
const std::string red("\033[0;31m");
const std::string green("\033[1;32m");
const std::string yellow("\033[1;33m");
const std::string cyan("\033[0;36m");
const std::string magenta("\033[0;35m");
const std::string gray("\033[0;37m");
const std::string reset("\033[0m");

#ifndef SOURCE_PATH_SIZE
#pragma message("SOURCE_PATH_SIZE not defined, use CMakeLists.txt")
#define SOURCE_PATH_SIZE 0
#endif
#define __FILENAME__ (__FILE__ + SOURCE_PATH_SIZE) // Strip source path from __FILE__

#define log_info(...) logging_utils::info(__FILENAME__, __LINE__, __VA_ARGS__)
#define log_warn(...) logging_utils::warn(__FILENAME__, __LINE__, __VA_ARGS__)
#define log_error(...) logging_utils::error(__FILENAME__, __LINE__, __VA_ARGS__)
#define log_fatal(...) logging_utils::fatal(__FILENAME__, __LINE__, __VA_ARGS__)

#ifdef NDEBUG
#define log_debug(...) (void)0 // No-op
#else
#define log_debug(...) logging_utils::debug(__FILENAME__, __LINE__, __VA_ARGS__)
#endif

namespace logging_utils
{
    enum class level
    {
        info,
        warn,
        error,
        fatal,
        debug
    };

    void info(const char *file, int line, const char *fmt, ...);
    void warn(const char *file, int line, const char *fmt, ...);
    void error(const char *file, int line, const char *fmt, ...);
    void fatal(const char *file, int line, const char *fmt, ...);
    void debug(const char *file, int line, const char *fmt, ...);
} // namespace log