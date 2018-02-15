#include "kunity_test_finder.h"
//{ global include region

#include <kunity_t.h>
#include <linux/glob.h>
#include <linux/kallsyms.h>
#include <linux/module.h>
#include <linux/slab.h>

//}

//{ local function prototypes region

static result_code_e create_test(/* in */ const char* name, /* in */ const char* module_name, /* in */ const kunity_test_function_ptr test_function, /* out */ ptr_test_s* test_location);

static result_code_e create_test_list_item(/* in */ const ptr_test_s test, /* out */ ptr_test_list_item_s* test_list_item_location);

static result_code_e create_test_list(/* out */ ptr_test_list_s* test_list_location);

static result_code_e create_test_module_list_item(/* in */ const struct module* module, /* out */ ptr_test_module_list_item_s* test_module_list_item_location);

static result_code_e destroy_test(/* in */ ptr_test_s* test_location);

static result_code_e destroy_test_list_item(/* in */ const ptr_test_list_item_s test_list_item);

static result_code_e destroy_test_module(/* in */ const ptr_test_module_list_item_s test_module_item);

static result_code_e add_test_list_item(/* in */ ptr_test_list_item_s test_item, /* in */ ptr_test_list_s test_list, /* in */ struct module* module);

static int find_test_for_each(/* in */ void* data, /* in */ const char* namebuf, /* in */ struct module* module, /* in */ unsigned long address);

static result_code_e _add_test_list_item(/* in */ ptr_test_list_item_s test_item, /* in */ ptr_test_module_list_item_s test_module);

//}

//{ local variables region

static const size_t sizeof_test = sizeof(test_s);

static const size_t sizeof_test_list_item = sizeof(test_list_item_s);

static const size_t sizeof_test_module_list_item = sizeof(test_module_list_item_s);

static const size_t sizeof_test_list = sizeof(test_list_s);

//}

//{ global function implements region

result_code_e find_tests(/* in */ const ptr_test_query_s query)
{
    if (query == NULL) {
        return ERROR_NULL_ARGUMENT;
    }

    if (create_test_list(&query->result_list) != OK) {
        return ERROR_INVALID_ARGUMENT;
    }

    if (kallsyms_on_each_symbol(find_test_for_each, query) != 0) {
        destroy_test_list(&query->result_list);
        return ERROR_INVALID_OPERATION;
    }

    return OK;
}

//}

//{ local function implements region

static result_code_e create_test(/* in */ const char* name, /* in */ const char* module_name, /* in */ const kunity_test_function_ptr test_function, /* out */ ptr_test_s* test_location)
{
    ptr_test_s tmp_test = NULL;
    if (name == NULL || module_name == NULL || test_function == NULL || test_location == NULL) {
        return ERROR_NULL_ARGUMENT;
    }

    if (*test_location != NULL) {
        return ERROR_INVALID_ARGUMENT;
    }

    tmp_test = kmalloc(sizeof_test, GFP_KERNEL);
    if (tmp_test == NULL) {
        return ERROR_INVALID_OPERATION;
    }

    tmp_test->name = name;
    tmp_test->test_function = test_function;
    *test_location = tmp_test;
    return OK;
}

static result_code_e create_test_list_item(/* in */ const ptr_test_s test, /* out */ ptr_test_list_item_s* test_list_item_location)
{
    ptr_test_list_item_s tmp_test_item = NULL;
    if (test == NULL || test_list_item_location == NULL) {
        return ERROR_NULL_ARGUMENT;
    }

    if (*test_list_item_location != NULL) {
        return ERROR_INVALID_ARGUMENT;
    }

    tmp_test_item = kmalloc(sizeof_test_list_item, GFP_KERNEL);
    if (tmp_test_item == NULL) {
        return ERROR_INVALID_OPERATION;
    }

    tmp_test_item->test = test;
    INIT_LIST_HEAD(&tmp_test_item->test_head);
    *test_list_item_location = tmp_test_item;
    return OK;
}

static result_code_e create_test_module_list_item(/* in */ const struct module* module, /* out */ ptr_test_module_list_item_s* test_module_list_item_location)
{
    ptr_test_module_list_item_s tmp_test_module_item = NULL;
    if (test_module_list_item_location == NULL) {
        return ERROR_NULL_ARGUMENT;
    }

    if (*test_module_list_item_location != NULL) {
        return ERROR_INVALID_ARGUMENT;
    }

    tmp_test_module_item = kmalloc(sizeof_test_module_list_item, GFP_KERNEL);
    if (tmp_test_module_item == NULL) {
        return ERROR_INVALID_OPERATION;
    }

    tmp_test_module_item->test_module = module;
    INIT_LIST_HEAD(&tmp_test_module_item->test_module_head);
    INIT_LIST_HEAD(&tmp_test_module_item->test_list);
    *test_module_list_item_location = tmp_test_module_item;
    return OK;
}

static result_code_e create_test_list(/* out */ ptr_test_list_s* test_list_location)
{
    ptr_test_list_s tmp_list = NULL;
    if (test_list_location == NULL) {
        return ERROR_NULL_ARGUMENT;
    }

    if (*test_list_location != NULL) {
        return ERROR_INVALID_ARGUMENT;
    }

    tmp_list = kmalloc(sizeof_test_list, GFP_KERNEL);
    if (tmp_list == NULL) {
        return ERROR_INVALID_OPERATION;
    }

    INIT_LIST_HEAD(&tmp_list->test_module_list);
    *test_list_location = tmp_list;
    return OK;
}

static result_code_e destroy_test(/* in */ ptr_test_s* test_location)
{
    if (test_location == NULL || *test_location == NULL) {
        return ERROR_NULL_ARGUMENT;
    }

    kzfree(*test_location);
    *test_location = NULL;
    return OK;
}

static result_code_e destroy_test_list_item(/* in */ const ptr_test_list_item_s test_list_item)
{
    if (test_list_item == NULL) {
        return ERROR_NULL_ARGUMENT;
    }

    destroy_test(&test_list_item->test);
    kzfree(test_list_item);
    return OK;
}

result_code_e destroy_test_list(/* in */ ptr_test_list_s* test_list_location)
{

    ptr_test_list_s tmp_list = NULL;
    if (test_list_location == NULL || *test_list_location == NULL) {
        return ERROR_NULL_ARGUMENT;
    }

    tmp_list = *test_list_location;
    while (!list_empty(&tmp_list->test_module_list)) {
        result_code_e result = OK;
        ptr_test_module_list_item_s module = list_entry(tmp_list->test_module_list.next, test_module_list_item_s, test_module_head);
        list_del(&module->test_module_head);
        result = destroy_test_module(module);
        if (result != OK) {
            return result;
        }
    }

    kzfree(tmp_list);
    *test_list_location = NULL;
    return OK;
}

static result_code_e _add_test_list_item(/* in */ ptr_test_list_item_s test_item, /* in */ ptr_test_module_list_item_s test_module)
{
    if (test_item == NULL || test_module == NULL) {
        return ERROR_NULL_ARGUMENT;
    }

    list_add(&test_item->test_head, &test_module->test_list);
    return OK;
}

static result_code_e add_test_list_item(/* in */ ptr_test_list_item_s test_item, /* in */ ptr_test_list_s test_list, /* in */ struct module* module)
{
    ptr_test_module_list_item_s it = NULL;
    if (test_item == NULL || test_list == NULL) {
        return ERROR_NULL_ARGUMENT;
    }

    list_for_each_entry(it, &test_list->test_module_list, test_module_head)
    {
        if (it->test_module != module) {
            continue;
        }

        return _add_test_list_item(test_item, it);
    }

    it = NULL;
    if (create_test_module_list_item(module, &it) != OK) {
        return ERROR_INVALID_OPERATION;
    }

    list_add(&it->test_module_head, &test_list->test_module_list);
    return _add_test_list_item(test_item, it);
}

static int find_test_for_each(/* in */ void* data, /* in */ const char* namebuf, /* in */ struct module* module, /* in */ unsigned long address)
{
    ptr_test_query_s query = NULL;
    ptr_test_s test = NULL;
    ptr_test_list_item_s test_item = NULL;
    const char* module_name = module == NULL ? "__KERNEL__" : module->name;
    if (address == 0) {
        return 0;
    }

    query = (ptr_test_query_s)data;
    if (glob_match(query->module_filter, module_name) == 0) {
        return 0;
    }

    if (glob_match(query->test_function_filter, namebuf) == 0) {
        return 0;
    }

    if (create_test(namebuf, module_name, (kunity_test_function_ptr)address, &test) != OK) {
        return 0;
    }

    if (create_test_list_item(test, &test_item) != OK) {
        destroy_test(&test);
        return 0;
    }

    if (add_test_list_item(test_item, query->result_list, module) != OK) {
        destroy_test_list_item(test_item);
    }

    return 0;
}

static result_code_e destroy_test_module(/* in */ const ptr_test_module_list_item_s test_module_item)
{

    if (test_module_item == NULL) {
        return ERROR_NULL_ARGUMENT;
    }

    while (!list_empty(&test_module_item->test_list)) {
        result_code_e result = OK;
        ptr_test_list_item_s test_item = list_entry(test_module_item->test_list.next, test_list_item_s, test_head);
        list_del(&test_item->test_head);
        result = destroy_test_list_item(test_item);
        if (result != OK) {
            return result;
        }
    }

    kzfree(test_module_item);
    return OK;
}
//}
