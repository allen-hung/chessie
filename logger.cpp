#include <stdarg.h>
#include <unistd.h>
#include "chessie/logger.h"

namespace chessie  {

Logger::Logger(FILE *_fp)
{
    fp = _fp;
}

Logger::Logger(const char *_filename)
{
    fn = _filename;
    fp = NULL;
}

void Logger::printf(const char *fmt, ...)
{
    if ( fp == NULL )  {
        fp = fopen(fn.c_str(), "w");
        if ( fp == NULL )  {
            fprintf(stderr, "Unable to open log file '%s'\n", fn.c_str());
            exit(1);
        }
    }
    va_list args;
    va_start(args, fmt);
    vfprintf(fp, fmt, args);
    va_end(args);
}

} // end of namesapce
