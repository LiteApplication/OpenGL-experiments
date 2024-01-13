#include "testgl/logging.hpp"
#include <cstdarg>

namespace logging_utils
{

    void info(const char *file, int line, const char *fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        fprintf(stdout, "%s[I] %s:%d: ", green.c_str(), file, line);
        vfprintf(stdout, fmt, args);
        fprintf(stdout, "%s\n", reset.c_str());
        va_end(args);
    }

    void warn(const char *file, int line, const char *fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        fprintf(stdout, "%s[W] %s:%d: ", yellow.c_str(), file, line);
        vfprintf(stdout, fmt, args);
        fprintf(stdout, "%s\n", reset.c_str());
        va_end(args);
    }

    void error(const char *file, int line, const char *fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        fprintf(stderr, "%s[E] %s:%d: ", red.c_str(), file, line);
        vfprintf(stderr, fmt, args);
        fprintf(stderr, "%s\n", reset.c_str());
        va_end(args);
    }

    void fatal(const char *file, int line, const char *fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        fprintf(stderr, "%s[F] %s:%d: ", magenta.c_str(), file, line);
        vfprintf(stderr, fmt, args);
        fprintf(stderr, "%s\n", reset.c_str());
        va_end(args);
        exit(EXIT_FAILURE);
    }

    void debug(const char *file, int line, const char *fmt, ...)
    {
#ifdef NDEBUG
        return;
#endif
        va_list args;
        va_start(args, fmt);
        fprintf(stdout, "%s[D] %s:%d: ", cyan.c_str(), file, line);
        vfprintf(stdout, fmt, args);
        fprintf(stdout, "%s\n", reset.c_str());
        va_end(args);
    }

}