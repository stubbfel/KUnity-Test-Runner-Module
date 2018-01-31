#include <kunity.h>
#include <fff.h>
DEFINE_FFF_GLOBALS

FAKE_VOID_FUNC(IO_MEM_WR8, int, int);


KUNITY_TEST(bar_test)
{
    IO_MEM_WR8(1,0);
    TEST_ASSERT_EQUAL_INT(1, IO_MEM_WR8_fake.call_count);
    TEST_ASSERT_TRUE(1)
}


void bar_test_2(void)
{
   kunity_test_bar_test();
    pr_info ("huhu");
}
