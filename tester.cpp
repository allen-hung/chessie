#include <stdio.h>
#include <string>
#include "chessie/chessie.h"

namespace chessie  {

static Logger &logfile = GetChessie.getLogger();
static Logger console(stdout);
static chessie::OutputCapturer &__capturer__ = chessie::OutputCapturer::getInstance();

//----------------------------
// Implementation of TestBase
//----------------------------
TestBase::TestBase(const char *_name)
{
    name = _name;
}

//----------------------------------------
// Implementation of TestBase classes  
//----------------------------------------
int TestBase::run()
{
    std::string exception;
    int ret_code = 0;

    // Start run the procedure while capturing the output in the same time
    __capturer__.start();
    try  {
        procedure();
    }
    catch ( AssertException &e )  {
        exception = e.what();
        ret_code = -1;
    }
    __capturer__.stop(); // stop the capturing

    // Log the output and exception message to file (or stream)
    const char *s, *line = __capturer__.content().c_str();
    while ( *line )  {
        for ( s = line ; *s && *s != '\n' ; s++ );
        logfile.printf("Out> %.*s\n", s - line, line);
        if ( *s == '\n' )  s++;
        line = s;
    }
    if ( exception != "" )
        logfile.printf("Err> %s\n", exception.c_str());

    __capturer__.reset();

    return ret_code;
}

//------------------------------
// Implementation of TestClass  
//------------------------------
TestClass::TestClass(const char *cls_name)
{
    static TestBase dummyTestBase;
    name = cls_name;
    setup = &dummyTestBase;
    tearDown = &dummyTestBase;
    classSetup = &dummyTestBase;
    classTearDown = &dummyTestBase;
    methodSkip = false;
}

int TestClass::setDescription(const char *msg)
{
    description = msg;
    return 0;
}

int TestClass::setTestMethodSkip(bool s, const char *r)
{
    // Don't care what parameters are given if methodSkip is already true
    if ( methodSkip )
        return 0;
    methodSkip = s;
    methodSkipReason = r;
    return 0;
}

int TestClass::addTestMothod(TestMethodBase *method)
{
    method->skip = methodSkip;
    method->skipReason = methodSkipReason;
    methodSkip = false;
    methodSkipReason.clear();
    testMothods.push_back(method);
    return 0;
}

int TestClass::removeTestMothod(TestMethodBase *method)
{
    testMothods.remove(method);
    return 0;
}

int TestClass::setSetup(TestBase *base)
{
    setup = base;
    return 0;
}

int TestClass::setTearDown(TestBase *base)
{
    tearDown = base;
    return 0;
}

int TestClass::setClassSetup(TestBase *base)
{
    classSetup = base;
    return 0;
}

int TestClass::setClassTearDown(TestBase *base)
{
    classTearDown = base;
    return 0;
}

void TestClass::printTestList()
{
    printf(description.empty() ? "%s\n" : "%s (%s)\n", name.c_str(), description.c_str());
    for ( auto method : testMothods )
        printf("%s.%s\n", name.c_str(), method->name.c_str());
}

std::vector<TestClass::tuple> TestClass::getTestList()
{
    std::vector<tuple> tests;
    for ( auto method : testMothods )
        tests.push_back(std::make_tuple(name + "." + method->name, method));
    return tests;
}

int TestClass::run()
{
    int retCode;

    console.printf("Test class: %s\n", name.c_str());
    logfile.printf("<<< Test class: %s >>>\n", name.c_str());
    console.printf("Description: %s\n", description.empty() ? "N/A" : description.c_str());
    logfile.printf("Description: %s\n", description.empty() ? "N/A" : description.c_str());
    logfile.printf("\n");

    // Setup the test class
    logfile.printf("Setup test class\n");
    retCode = classSetup->run();
    if ( retCode )  {
        console.printf("\033[91mTest class fails in setup\033[0m\n"); // red
        console.printf("\n");
        logfile.printf("Test class fails in setup\n");
        logfile.printf("\n");
        return 1;
    }
    logfile.printf("\n");

    numSkipped = 0;
    numTested = 0;
    numPassed = 0;
    numFailed = 0;
    size_t i = 0;
    for ( auto method : testMothods )  {
        // Begin with a test case
        i++;
        console.printf("Test case %lu/%lu \"%s\": ", i, testMothods.size(), method->name.c_str());
        logfile.printf("Test case %lu/%lu: %s\n", i, testMothods.size(), method->name.c_str());

        // Check the test case is whether to skip?
        if ( method->skip ) {
            console.printf("\033[93mskipped\033[0m\n"); // yellow
            logfile.printf("Test case is skipped: %s\n", method->skipReason.c_str());
            logfile.printf("\n");
            numSkipped++;
            continue;
        }
        numTested++;

        // Setup the test case
        logfile.printf("Setup test case\n");
        retCode = setup->run();
        if ( retCode )  {
            console.printf("\033[91mfailed\033[0m (due to setup error)\n"); // red
            logfile.printf("Test case is failed: setup error\n");
            logfile.printf("\n");
            numFailed++;
            GetChessie.stop(); // left 'chessie' to detemine whether to stop process
            continue;
        }

        // Execute the test case
        logfile.printf("Execute test case\n");
        retCode = method->run();
        if ( retCode )  {
            console.printf("\033[91mfailed\033[0m\n"); // red
            logfile.printf("Test case is failed\n");
            numFailed++;
            GetChessie.stop(); // left 'chessie' to detemine whether to stop process
        }
        else  {
            console.printf("\033[92mpassed\033[0m\n"); // green
            logfile.printf("Test case is passed\n");
            numPassed++;
        }

        // Tear down the test case
        logfile.printf("Tear down test case\n");
        retCode = tearDown->run();
        logfile.printf("\n");
    }

    // Tear down the test class
    logfile.printf("Tear down test class\n");
    retCode = tearDown->run();
    logfile.printf("\n");

    // Print and log the statistics of test results
    char percentage_buf[20];
    if ( numTested > 0 )
        snprintf(percentage_buf, sizeof(percentage_buf), "%d.%01d%% (%d/%d)", 
            numPassed*100/numTested, (numPassed*100%numTested)*10/ numTested, numPassed, numTested);
    else
        snprintf(percentage_buf, sizeof(percentage_buf), "N/A");

    console.printf("Number of all cases: %lu\n", testMothods.size());
    //if ( numSkipped > 0 )
    console.printf("Number of passed cases: %d\n", numPassed);
    int colorCode = numFailed > 0 ? 91 : 0; // red or normal
    console.printf("Number of failed cases: \033[%dm%d\033[0m\n", colorCode, numFailed);
    colorCode = numSkipped > 0 ? 93 : 0; // yellow or normal
    console.printf("Number of skipped cases: \033[%dm%d\033[0m\n", colorCode, numSkipped);
    console.printf("Number of tested cases: %d\n", numTested);
    colorCode = numFailed > 0 ? 91 : 0; // red or normal
    console.printf("Pass rate (passed/tested): \033[%dm%s\033[0m\n", colorCode, percentage_buf);
    console.printf("\n");

    logfile.printf("Number of all cases: %lu\n", testMothods.size());
    logfile.printf("Number of passed cases: %d\n", numPassed);
    logfile.printf("Number of failed cases: %d\n", numFailed);
    logfile.printf("Number of skipped cases: %d\n", numSkipped);
    logfile.printf("Number of tested cases: %d\n", numTested);
    logfile.printf("Pass rate (passed/tested): %s\n", percentage_buf);
    logfile.printf("\n");

    return numFailed > 0 ? 1 : 0;
}

} // end of namespace
