#ifndef ASSERT_H_INCLUDED
#define ASSERT_H_INCLUDED
#include <string>
#include <exception>

namespace chessie  {

class AssertException : public std::exception {
    std::string _what;
public:
    AssertException(const std::string &con, const std::string &filename, int line);
    ~AssertException() throw();
    const char *what() const noexcept;
};

} // end of namespace

// Define ASSERT macros
#define ASSERT(condition)  \
    if ( !(condition) )  \
        throw chessie::AssertException("ASSERT(" #condition ")", __FILE__, __LINE__);

#define ASSERT_TRUE(condition)  \
    if ( !((condition) == true) )  \
        throw chessie::AssertException("ASSERT_TRUE(" #condition ")", __FILE__, __LINE__);

#define ASSERT_FALSE(condition)  \
    if ( !((condition) == false) )  \
        throw chessie::AssertException("ASSERT_FALSE(" #condition ")", __FILE__, __LINE__);

#define ASSERT_EQUAL(a, b)  \
    if ( !((a) == (b)) )  \
        throw chessie::AssertException("ASSERT_EQUAL(" #a ", " #b ")", __FILE__, __LINE__);

#define ASSERT_NOT_EQUAL(a, b)  \
    if ( !((a) != (b)) )  \
        throw chessie::AssertException("ASSERT_NOT_EQUAL(" #a ", " #b ")", __FILE__, __LINE__);

#define ASSERT_ZERO(number)  \
    if ( !((number) == 0) )  \
        throw chessie::AssertException("ASSERT_ZERO(" #number ")", __FILE__, __LINE__);

#define ASSERT_NONZERO(number)  \
    if ( !((number) != 0) )  \
        throw chessie::AssertException("ASSERT_NONZERO(" #number ")", __FILE__, __LINE__);


#define ASSERT_FAIL(reason)  \
    throw chessie::AssertException("ASSERT_FAIL(" #reason ")", __FILE__, __LINE__);

#endif
