#include <linux/module.h>
#include <linux/string.h>
#include <kunity.h>
#include <kunity_test_finder.h>


result_code_e kunity_module_runner_fake_test_function_ok(const ptr_test_session_control_block_s output)
{
    return OK;
}
EXPORT_SYMBOL(kunity_module_runner_fake_test_function_ok);

static size_t tmp_list_counter;
static result_code_e assert_test_item(/* in */ const ptr_test_list_item_s test_list_item)
{
    TEST_ASSERT_NOT_EQUAL(NULL, test_list_item);
    TEST_ASSERT_NOT_EQUAL(NULL, test_list_item->test);
    TEST_ASSERT_NOT_EQUAL(NULL, test_list_item->test->name);
    TEST_ASSERT_NOT_EQUAL(NULL, test_list_item->test->test_function);
    tmp_list_counter++;
    return OK;
}


KUNITY_TEST(test_find_test_null_argument)
{
    TEST_ASSERT_EQUAL(ERROR_NULL_ARGUMENT, find_tests(NULL));
}

KUNITY_TEST(test_find_test_invalid_argument)
{
    test_list_s list;
    test_query_s query ={ KUNITY_DEFAULT_TEST_NAME_FITER_STR, "*", &list};

    TEST_ASSERT_EQUAL(ERROR_INVALID_ARGUMENT, find_tests(&query));
}

KUNITY_TEST(test_find_test_found)
{   
    ptr_test_module_list_item_s module = NULL;
    ptr_test_list_item_s test = NULL;
    test_query_s query ={ "kunity_module_runner_fake_test_function_ok",  THIS_MODULE->name, NULL};
    TEST_ASSERT_EQUAL(OK, find_tests(&query));
    TEST_ASSERT_NOT_NULL(query.result_list);

    module = list_first_entry(&query.result_list->test_module_list, test_module_list_item_s, test_module_head);
    TEST_ASSERT_NOT_NULL(module);
    test = list_first_entry(&module->test_list, test_list_item_s, test_head);
    TEST_ASSERT_NOT_NULL(test);
    TEST_ASSERT_EQUAL_STRING("kunity_module_runner_fake_test_function_ok", test->test->name);
    TEST_ASSERT_EQUAL_PTR(&kunity_module_runner_fake_test_function_ok, test->test->test_function);
}


KUNITY_TEST(test_find_tests_found)
{
    ptr_test_module_list_item_s module = NULL;
    ptr_test_list_item_s test = NULL;
    test_query_s query ={ KUNITY_DEFAULT_TEST_NAME_FITER_STR, "*", NULL};
    TEST_ASSERT_EQUAL(OK, find_tests(&query));
    TEST_ASSERT_NOT_NULL (query.result_list);
    TEST_ASSERT_FALSE( list_empty(&query.result_list->test_module_list))
    module = list_first_entry(&query.result_list->test_module_list, test_module_list_item_s, test_module_head);
    TEST_ASSERT_FALSE( list_empty(&module->test_list))
    tmp_list_counter = 0;
    list_for_each_entry(test, &module->test_list, test_head)
    {
        assert_test_item(test);
    }

    TEST_ASSERT_GREATER_OR_EQUAL(1, tmp_list_counter);
}


