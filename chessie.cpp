#include <stdexcept>
#include <regex>
#include "chessie/chessie.h"

namespace chessie  {
static const char *help_msg =
    "Usage: %s [OPTIONS]...\n"
    "\n"
    "Options:\n"
    "\t--help\tshow this help\n"
    "\n"
    "\t--list\tlist all available tests\n"
    "\n"
    "\t--repeat=NUMBER\n"
    "\t\trun the tests for NUMBER times\n"
    "\n"
    "\t--filter=PATTERN\n"
    "\t\trun only the tests which matches PATTERN\n"
    "\n"
    "\t--log=FILE\n"
    "\t\tlog file for recording test results\n"
    "\t\n"
    "\t--failure-to-stop\n"
    "\t\tstop running the tests when failure occurs\n"
    "\n";

//----------------------------
// Implementation of TestBase
//----------------------------

Chessie *Chessie::instance = NULL;

Chessie &Chessie::getInstance()
{
    if ( instance == NULL )
        instance = new Chessie();
    return *instance;
}

Chessie::Chessie() :
    logger("chessie_test.log"), repeat(1), stop_on_failure(false)
{
}

int Chessie::setLogger(FILE *fp)
{
    logger = Logger(fp);
    return 0;
}

int Chessie::setLogger(const char *filename)
{
    logger = Logger(filename);
    return 0;
}

Logger &Chessie::getLogger()
{
    return logger;
}

int Chessie::testClassRegister(TestClass *cls)
{
    testClasses.push_back(cls);
    return 0;
}

void Chessie::stop(const char *msg)
{
    if ( stop_on_failure )  {
        const char *msg = "Enforced to stop test as failure occured (--failure-to-stop)\n";
        fprintf(stderr, "%s", msg);
        logger.printf("%s", msg);
        exit(1);
    }
}

void Chessie::filterTestCases(std::string &pattern_string)
{
    // Convert pattern string into regex string
    std::string regex_str = "^(";
    std::vector<std::string> patterns;
    int start = 0;
    auto next = pattern_string.find(",", start);
    while ( next != std::string::npos )  {
        patterns.push_back(pattern_string.substr(start, next-start));
        start = next + 1;
        next = pattern_string.find(",", start);
    }
    patterns.push_back(pattern_string.substr(start));
    bool first = true;
    const std::string special_chars = ".^$+()[{\\|";
    for ( auto pattern : patterns )  {
        if ( pattern.empty() )
            continue;
        bool dot = false, wildcard = false;
        if ( !first )
            regex_str += "|";
        for ( char ch : pattern )  {
            if ( ch == '*' )
                regex_str += ".*", wildcard = true;
            else if ( ch == '?' )
                regex_str += ".", wildcard = true;
            else if ( special_chars.find(ch) != std::string::npos )
                regex_str += "\\", regex_str += ch;
            else
                regex_str += ch;
            if ( ch == '.')
                dot = true;
        }
        if ( !dot && !wildcard )
            regex_str += "\\..*";
        first = false;
    }
    regex_str += ")$";
    //printf("regex = %s\n", regex_str.c_str());
    std::regex regex(regex_str);


    std::list<TestClass *> filtered_classes;

    // Match regex to each test
    for ( auto test_class : testClasses ) {
        int match_count = 0;
        for ( auto test : test_class->getTestList() )  {
            auto name = std::get<0>(test);
            if ( std::regex_match(name, regex) )  {
                //printf("%s\n", name.c_str());
                match_count++;
            }
            else  {
                auto method = std::get<1>(test);
                test_class->removeTestMothod(method);
            }
        }
        if ( match_count > 0 )
            filtered_classes.push_back(test_class);
    }
    testClasses = filtered_classes;
}

int Chessie::parseArguments(int argc, char *argv[])
{
    bool flagHelp = false;
    bool flagList = false;
    bool flagRepeat = false;
    bool flagLog = false;
    bool flagStop = false;
    bool flagFilter = false;
    for ( int i = 1 ; i < argc ; i++ )  {
        auto arg = std::string(argv[i]);
        try {
            if ( arg == "--help" )
                flagHelp = true;
            else if ( arg == "--list" )
                flagList = true;
            else if ( arg.find("--repeat=") == 0 ) {
                auto value = arg.substr(9);
                repeat = std::stoi(value);
                if ( repeat <= 0 )
                    throw std::invalid_argument("number must be greater than zero");
                flagRepeat = true;
            }
            else if ( arg.find("--log=") == 0 ) {
                auto value = arg.substr(6);
                if ( value.empty() )
                    throw std::invalid_argument("filename is empty");
                setLogger(value.c_str());
                flagLog = true;
            }
            else if ( arg == "--failure-to-stop" ) {
                stop_on_failure = true;
                flagStop = true;
            }
            else if ( arg.find("--filter=" ) == 0 ) {
                auto value = arg.substr(9);
                if ( value.empty() )
                    throw std::invalid_argument("pattern is empty");
                filterTestCases(value);
                flagFilter = true;
            }
            else
                throw std::invalid_argument("not supported");
        }
        catch ( std::exception &e ) {
            fprintf(stderr, "Illegal option specified with \'%s\': %s\n",
                arg.c_str(), e.what());
            exit(1);
        }
    }

    if ( flagRepeat || flagLog || flagStop )
        ; // TBD

    if ( flagList ) {
        printf("All available tests%s:\n", flagFilter ? " (filtered)" : "");
        for ( auto test_class : testClasses )
            test_class->printTestList();
        printf("\n");
    }

    if ( flagHelp )
        printf(help_msg, argv[0]);

    if ( flagHelp || flagList )
        return 1;

    return 0;
}

int Chessie::testClassRunAll()
{
    Logger console(stdout);
    int ret = 0;
    for ( int i = 1 ; i <= repeat ; i++ )  {
        for ( auto test_class : testClasses )  {
            ret |= test_class->run();
            if ( ret )
                stop(); // left stop() to detemine whether to stop process
        }
        if ( repeat > 1 )  {
            console.printf("Total tested round: %d of %d\n\n", i, repeat);
            logger.printf("Total tested round: %d of %d\n\n", i, repeat);
        }
    }
    return ret;
}

int Chessie::testClassRunAll(int argc, char *argv[])
{
    if ( parseArguments(argc, argv) )
        return 1;

    return testClassRunAll();
}

} // end of namespace
