#include <stdio.h>
#include <string>
#include "chessie/assert.h"

namespace chessie  {

AssertException::AssertException(const std::string &con, const std::string &filename, int line)
{
    char buffer[20];
    snprintf(buffer, sizeof(buffer), "%d", line);
    _what = filename + ":" + buffer + ": fails on " + con;
}

AssertException::~AssertException() throw()
{
}

const char *AssertException::what() const noexcept
{
    return _what.c_str();
}

} // end of namespace
