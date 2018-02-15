#include <kunity.h>
#include <kunity_proc_test_file_handler.h>
#include <linux/module.h>
#include <linux/string.h>


//extern result_code_e kunity_module_runner_fake_test_function_ok(const ptr_output_functions_s output);

KUNITY_TEST(create_proc_test_file_null_argument)
{
    TEST_ASSERT_EQUAL(ERROR_NULL_ARGUMENT, create_proc_test_structure(NULL,NULL));
}


KUNITY_TEST(create_proc_test_file_ok)
{
    test_s test;

    test_list_s list;
    test_module_list_item_s module_item;
    test_list_item_s test_item;
    struct proc_dir_entry* test_root = proc_mkdir("test_kunity_test_root", NULL);
    module_item.test_module = THIS_MODULE;
    INIT_LIST_HEAD(&test_item.test_head);
    INIT_LIST_HEAD(&list.test_module_list);
    INIT_LIST_HEAD(&module_item.test_module_head);
    INIT_LIST_HEAD(&module_item.test_list);
    list_add(&test_item.test_head, &module_item.test_list);
    list_add(&module_item.test_module_head, &list.test_module_list);
    test.name = "fake_test_function";
    test.test_function = NULL;
    test_item.test = &test;
    if (test_root == NULL) {
        TEST_FAIL_MESSAGE("could not create \"test_kunity_test_root proc\" folder");
    }

    TEST_ASSERT_EQUAL(OK, create_proc_test_structure(test_root, &list));
    proc_remove(test_root);
}
