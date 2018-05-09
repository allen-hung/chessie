#ifndef CHESSIE_H_INCLUDED
#define CHESSIE_H_INCLUDED
#include <list>
#include <string>
#include "capturer.h"
#include "assert.h"
#include "logger.h"
#include "tester.h"

namespace chessie  {

class Chessie  {
    static Chessie *instance;
    std::list<TestClass *> testClasses;
    Logger logger;
    int repeat;
    bool stop_on_failure;
    Chessie();
    int parseArguments(int argc, char *argv[]);
    void filterTestCases(std::string &pattern_string);
public:
    static Chessie &getInstance();
    int testClassRunAll();
    int testClassRunAll(int argc, char *argv[]);
    int testClassRegister(TestClass *);
    int setLogger(FILE *fp);
    int setLogger(const char *filename);
    Logger &getLogger();
    void stop(const char *msg = NULL);
};

} // end of namespace

#define GetChessie (chessie::Chessie::getInstance())

#endif
