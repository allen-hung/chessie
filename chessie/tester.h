#ifndef TESTER_H_INCLUDED
#define TESTER_H_INCLUDED
#include <list>
#include <vector>
#include <string>
#include <tuple>

namespace chessie  {

class TestBase  {
protected:
    std::string name;
    virtual void procedure() {};
public:
    TestBase(const char *name = "");
    int run();
};

class TestMethodBase : public TestBase {
    bool skip;
    std::string skipReason;
public:
    TestMethodBase(const char *name) : TestBase(name), skip(false) {}
    friend class TestClass;
};

class TestClass {
    std::string name;
    std::string description;
    std::list<TestMethodBase *> testMothods;
    TestBase *setup;
    TestBase *tearDown;
    TestBase *classSetup;
    TestBase *classTearDown;
    bool methodSkip;
    std::string methodSkipReason;
    int numSkipped;
    int numTested;
    int numPassed;
    int numFailed;
public:
    TestClass(const char *cls_name);
    int setDescription(const char *msg);
    int setTestMethodSkip(bool s, const char *r);
    int addTestMothod(TestMethodBase *method);
    int removeTestMothod(TestMethodBase *method);
    int setSetup(TestBase *b);
    int setTearDown(TestBase *b);
    int setClassSetup(TestBase *b);
    int setClassTearDown(TestBase *b);
    int run();
    void printTestList();
    using tuple = std::tuple<std::string, TestMethodBase *>;
    std::vector<tuple> getTestList();
};

} // end of namespace

// Define internal used macros
#define __MERGE__(x, y) x##y
#define __MERGE2__(x, y)    __MERGE__(x, y)
#define __UNIQUE_VAR__  __MERGE2__(_stub_, __LINE__)
#define __TEST_METHOD_ID__(id)  __MERGE__(TestMothod_, id)
#define __TEST_CLASS_NAMESPACE__(id)   __MERGE__(Chessie_TestClass_, id)

//
// Define macros for user to define test class, test method, and run tests
//
#define TEST_CLASS(id) \
    namespace __TEST_CLASS_NAMESPACE__(id) { \
        chessie::TestClass cls(#id); \
        int __UNIQUE_VAR__ = GetChessie.testClassRegister(&cls); \
    } \
    \
    extern "C" void *__MERGE2__(get_chessie_test_class_, id)()  { \
        return &__TEST_CLASS_NAMESPACE__(id)::cls; \
    } \
    \
    namespace __TEST_CLASS_NAMESPACE__(id)

#define TEST_DESCRIPTION(msg) \
    int __UNIQUE_VAR__ = cls.setDescription(msg);

#define TEST_LOG(file) \
    int __UNIQUE_VAR__ = GetChessie.setLogger(file);

#define TEST_METHOD(id) \
    class __TEST_METHOD_ID__(id) : public chessie::TestMethodBase {  \
        void procedure(); \
    public: \
        __TEST_METHOD_ID__(id)(const char *name) : chessie::TestMethodBase(name) {} \
    }; \
    \
    int __UNIQUE_VAR__ = cls.addTestMothod(new __TEST_METHOD_ID__(id)(#id)); \
    \
    void __TEST_METHOD_ID__(id)::procedure()

// Exactly same as TEST_METHOD
#define TEST_CASE(id) \
    class __TEST_METHOD_ID__(id) : public chessie::TestMethodBase {  \
        void procedure(); \
    public: \
        __TEST_METHOD_ID__(id)(const char *name) : chessie::TestMethodBase(name) {} \
    }; \
    \
    int __UNIQUE_VAR__ = cls.addTestMothod(new __TEST_METHOD_ID__(id)(#id)); \
    \
    void __TEST_METHOD_ID__(id)::procedure()

#define SKIP_IF(expr, reason) \
    int __UNIQUE_VAR__ = cls.setTestMethodSkip(expr, reason);

#define SKIP_UNLESS(expr, reason) \
    int __UNIQUE_VAR__ = cls.setTestMethodSkip(!(expr), reason);

#define SKIP(reason) \
    int __UNIQUE_VAR__ = cls.setTestMethodSkip(true, reason);

#define SETUP() \
    class TestMethodSetup : public chessie::TestBase {  \
        void procedure() override; \
    }; \
    \
    int __UNIQUE_VAR__ = cls.setSetup(new TestMethodSetup); \
    \
    void TestMethodSetup::procedure()

#define TEAR_DOWN() \
    class TestMethodTearDown : public chessie::TestBase {  \
        void procedure() override; \
    }; \
    \
    int __UNIQUE_VAR__ = cls.setTearDown(new TestMethodTearDown); \
    \
    void TestMethodTearDown::procedure()

#define CLASS_SETUP() \
    class TestClassSetup : public chessie::TestBase {  \
        void procedure() override; \
    }; \
    \
    int __UNIQUE_VAR__ = cls.setClassSetup(new TestClassSetup); \
    \
    void TestClassSetup::procedure()

#define CLASS_TEAR_DOWN() \
    class TestClassTearDown : public chessie::TestBase {  \
        void procedure() override; \
    }; \
    \
    int __UNIQUE_VAR__ = cls.setClassTearDown(new TestClassTearDown); \
    \
    void TestClassTearDown::procedure()

#define EXTERN_TEST_CLASS(id) \
    namespace __TEST_CLASS_NAMESPACE__(id)  {  \
        extern chessie::TestClass cls;  \
    }

#define TEST_CLASS_RUN(id) \
    __TEST_CLASS_NAMESPACE__(id)::cls.run()

#define TEST_CLASS_RUN_ALL() \
    GetChessie.testClassRunAll()

#define TEST_MAIN() \
    int main(int argc, char *argv[]) { \
        return GetChessie.testClassRunAll(argc, argv); \
    }

#endif
