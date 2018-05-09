#ifndef CAPTURER_H_INCLUDED
#define CAPTURER_H_INCLUDED
#include <string>
#include <semaphore.h>

namespace chessie  {

#define __CAPTURER_BUFFER_SIZE__ 4096

class OutputCapturer
{
    int tmp_fd, read_fd;
    int saved_stdout;
    int saved_stderr;
    bool isCapturing;
    std::string tmp_filename;
    std::string output;
    std::string log;
    char buffer[__CAPTURER_BUFFER_SIZE__];
    static OutputCapturer *capturer;

    static void destructCapturer();
    OutputCapturer();
    ~OutputCapturer();
    void sync();
    void start();
    void stop();
    void reset();
    std::string &content();

    friend class TestBase;
    friend class TestMethodBase;
    friend class TestMethodSetupBase;
    friend class TestMethodTearDownBase;
public:
    static OutputCapturer &getInstance();
    bool _matches(std::string s);
    bool _contains(std::string s);
    size_t _numberOf(std::string s);
    void _clear();
};

} // end of namespace

#define _get_output_capturer (chessie::OutputCapturer::getInstance())

//
// Define macros for console output text
//
#define OUTPUT_CONTAINS(pattern)    (_get_output_capturer._contains(pattern))
#define OUTPUT_MATCHES(pattern)     (_get_output_capturer._matches(pattern))
#define OUTPUT_NUMBER_OF(pattern)   (_get_output_capturer._numberOf(pattern))
#define OUTPUT_CLEAR()              (_get_output_capturer._clear())
// TODO: implement regexp matches

#endif
