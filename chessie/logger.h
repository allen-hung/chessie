#ifndef LOGGER_H_INCLUDED
#define LOGGER_H_INCLUDED
#include <stdio.h>
#include <string>

namespace chessie  {

class Logger {
    FILE *fp;
    std::string fn;
public:
    Logger(FILE *_fp);
    Logger(const char *filename);
    void printf(const char *fmt, ...);
};

} // end of namesapce

#endif
