#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdexcept>
#include <cstdlib>
#include "chessie/capturer.h"

namespace chessie  {
//************************************
//**   class OutputCapturer
//************************************

// The static class member
OutputCapturer *OutputCapturer::capturer = NULL;

OutputCapturer::OutputCapturer()
{
    isCapturing = false;
    // The temp file will be used to store the stdout/stderr of the tested
    // procedures
    char fn_template[] = "/tmp/chessieXXXXXX";
    tmp_fd = mkstemp(fn_template);
    if ( tmp_fd < 0 )
        throw std::runtime_error("failed to create temp file");
    tmp_filename = fn_template;
    read_fd = open(fn_template, O_RDONLY);
    if ( read_fd < 0 )
        throw std::runtime_error("failed to open temp file");
    // Preserve the original fd of stdout/stderr and set them as 
    // non-buffered io-streams
    saved_stdout = dup(STDOUT_FILENO);
    saved_stderr = dup(STDERR_FILENO);
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
}

OutputCapturer::~OutputCapturer()
{
    close(tmp_fd);
    close(read_fd);
    unlink(tmp_filename.c_str());
}

OutputCapturer &OutputCapturer::getInstance()
{
    if ( capturer == NULL )  {
        capturer = new OutputCapturer();
        std::atexit(OutputCapturer::destructCapturer);
    }
    return *capturer;
}

void OutputCapturer::destructCapturer()
{
    if ( capturer )  {
        delete capturer;
        capturer = NULL;
    }
}

std::string &OutputCapturer::content()
{
    sync();
    return log;
}

void OutputCapturer::start()
{
    if ( isCapturing )
        return;
    isCapturing = true;
    fflush(stdout);
    fflush(stderr);
    dup2(tmp_fd, STDOUT_FILENO);
    //dup2(tmp_fd, STDERR_FILENO);
}

void OutputCapturer::stop()
{
    if ( !isCapturing )
        return;
    isCapturing = false;
    // restore stdout & stderr to console
    dup2(saved_stdout, STDOUT_FILENO);
    dup2(saved_stderr, STDERR_FILENO);
    sync();
}

void OutputCapturer::sync()
{
    size_t size;
    while ( (size = read(read_fd, buffer, sizeof(buffer))) > 0 )  {
        output.append(buffer, size);
        log.append(buffer, size);
    }
}

void OutputCapturer::reset()
{
    sync();
    output.clear();
    log.clear();
}

void OutputCapturer::_clear()
{
    sync();
    output.clear();
}

bool OutputCapturer::_matches(std::string s)
{
    sync();
    return output == s;
}

bool OutputCapturer::_contains(std::string s)
{
    sync();
    return output.find(s) != std::string::npos;
}

size_t OutputCapturer::_numberOf(std::string s)
{
    size_t num = 0;
    sync();
    size_t pos = output.find(s);
    while ( pos != std::string::npos )  {
        num++;
        pos = output.find(s, pos+s.size());
    }
    return num;
}

}
