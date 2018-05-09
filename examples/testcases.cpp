#include <stdio.h>
#include <chessie/chessie.h>

#define VER 2

TEST_LOG("mytest_log.txt")

TEST_CLASS(functional)  {

    TEST_DESCRIPTION("some test cases are intended to fail");

    static int x = 0;

    CLASS_SETUP()
    {
        ASSERT_FAIL("fail entire class");
    }


    CLASS_TEAR_DOWN()
    {
        
    }

    SETUP()
    {
        if ( ++x == 2 )
            ASSERT(false);
    }

    TEAR_DOWN()
    {
        ASSERT(1 == 2);
    }

    TEST_METHOD(put)
    {
        int i, n;
        for ( i = 0; i < 5 ; i++ )
            printf("test case 1-1  \n");
        n = OUTPUT_NUMBER_OF("test case 1-1  \n");
        ASSERT_EQUAL(n, 5);
        OUTPUT_CLEAR();
        for ( i = 0; i < 3 ; i++ )
            printf("test case 1-1  \n");
        //fprintf(stderr, "num of instances = %d\n", n);
        n = OUTPUT_NUMBER_OF("test case 1-1  \n");
        //fprintf(stderr, "num of instances = %d\n", n);
        ASSERT_EQUAL(n, 3);
    }

    TEST_METHOD(assert_false)
    {
        ASSERT_FALSE(true);
    }

    TEST_METHOD(assert_true)
    {
        ASSERT_TRUE(false);
    }

    TEST_METHOD(assert_equal)
    {
        ASSERT_EQUAL(1, 2);
    }

    TEST_METHOD(assert_not_equal)
    {
        ASSERT_NOT_EQUAL(1, 1);
    }

    TEST_METHOD(assert_fail)
    {
        ASSERT_FAIL("no reason, just fail");
    }

    TEST_METHOD(get)
    {
        printf("test case 1-2\n");
    }
};

TEST_CLASS(integration)  {
    static int x = 0;

    SKIP_IF(VER == 3, "version 3 is not supported")
    SKIP_IF(VER == 2, "version 2 is not supported")
    TEST_METHOD(skip1)
    {
        printf("test case 2-1\n");
    }

    SKIP("skip anyway")
    TEST_METHOD(skip2)
    {
        printf("test case 2-2\n");
    }

    //SKIP("")
    TEST_METHOD(platform_restart)
    {
        printf("test case 2-2\n");
    }
};
