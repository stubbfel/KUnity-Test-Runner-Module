#include "kunity_test_finder.h"
//{ global include region

#include <kunity_t.h>
#include <linux/glob.h>
#include <linux/kallsyms.h>
#include <linux/module.h>
#include <linux/slab.h>

//}

//{ local include region

//}


//{ local define region

//}


//{ local enum region

//}


//{ local typedef region

//}


//{local struct region

#pragma pack(push, 1)

#pragma pack(pop)

//}


//{ local function prototype region

static result_code_e create_test(/* in */ const char * name, /* in */ const char * module_name, /* in */ const test_function_ptr test_function, /* out */ ptr_test_s * test_location);

static result_code_e create_test_list_item(/* in */ const ptr_test_s test, /* out */ ptr_test_list_item_s * test_list_item_location);

static result_code_e create_test_list(/* out */ ptr_test_list_s * test_list_location);

static result_code_e destroy_test(/* in */ ptr_test_s * test_location);

static result_code_e destroy_test_list_item(/* in */ ptr_test_list_item_s * test_list_item_location, /* out */ ptr_test_list_item_s * next_test_item_location);

static result_code_e add_test_list_item(/* in */ ptr_test_list_item_s test_item, /* in */ ptr_test_list_s test_list);

static int find_test_for_each(void *data, const char *namebuf, struct module *module, unsigned long address);

//}

//{ global variabel implements region

//}


//{ local variabel implements region

static const size_t sizeof_test = sizeof (test_s);

static const size_t sizeof_test_list_item = sizeof (test_list_item_s);

static const size_t sizeof_test_list = sizeof (test_list_s);

//}

//{ global function implements region

result_code_e find_tests(/* in */ const ptr_test_query_s query)
{
    if (query == NULL)
    {
         return ERROR_NULL_ARGUMENT;
    }

    if (create_test_list(&query->result_list) != OK)
    {
        return ERROR_INVALID_ARGUMENT;
    }

    if(kallsyms_on_each_symbol(find_test_for_each, query) != 0)
    {
        destroy_test_list(&query->result_list);
        return ERROR_INVALID_OPERATION;
    }

    return OK;
}
//}


//{ local function implements region

static result_code_e create_test(/* in */ const char * name, /* in */ const char * module_name, /* in */ const test_function_ptr test_function, /* out */ ptr_test_s * test_location)
{
    ptr_test_s tmp_test = NULL;
    if (name == NULL || module_name == NULL || test_function == NULL || test_location == NULL)
    {
         return ERROR_NULL_ARGUMENT;
    }

    if (*test_location != NULL)
    {
        return ERROR_INVALID_ARGUMENT;
    }

    tmp_test =  kmalloc(sizeof_test, GFP_KERNEL);
    if (tmp_test == NULL)
    {
        return ERROR_INVALID_OPERATION;
    }

    tmp_test->name = name;
    tmp_test->modul_name = name;
    tmp_test->test_function = test_function;
    *test_location = tmp_test;
    return OK;
}

static result_code_e create_test_list_item(/* in */ const ptr_test_s test, /* out */ ptr_test_list_item_s * test_list_item_location)
{
    ptr_test_list_item_s tmp_test_item = NULL;
    if (test == NULL || test_list_item_location == NULL)
    {
         return ERROR_NULL_ARGUMENT;
    }

    if (*test_list_item_location != NULL)
    {
        return ERROR_INVALID_ARGUMENT;
    }

    tmp_test_item = kmalloc(sizeof_test_list_item, GFP_KERNEL);
    if (tmp_test_item == NULL)
    {
        return ERROR_INVALID_OPERATION;
    }

    tmp_test_item->test = test;
    tmp_test_item->next = NULL;
    *test_list_item_location = tmp_test_item;
    return OK;
}

static result_code_e create_test_list(/* out */ ptr_test_list_s * test_list_location)
{
    ptr_test_list_s tmp_list = NULL;
    if (test_list_location == NULL)
    {
         return ERROR_NULL_ARGUMENT;
    }

    if (*test_list_location != NULL)
    {
        return ERROR_INVALID_ARGUMENT;
    }

    tmp_list = kmalloc(sizeof_test_list, GFP_KERNEL);
    if (tmp_list == NULL)
    {
        return ERROR_INVALID_OPERATION;
    }

    return OK;
}

static result_code_e destroy_test(/* in */ ptr_test_s * test_location)
{
    if (test_location == NULL || *test_location == NULL)
    {
        return ERROR_NULL_ARGUMENT;
    }

    kfree(*test_location);
    *test_location = NULL;
    return OK;
}

static result_code_e destroy_test_list_item(/* in */ ptr_test_list_item_s * test_list_item_location, /* out */ ptr_test_list_item_s * next_test_item_location)
{
    ptr_test_list_item_s tmp_list_item = NULL;
    if (test_list_item_location == NULL || *test_list_item_location == NULL)
    {
        return ERROR_NULL_ARGUMENT;
    }

    tmp_list_item = *test_list_item_location;
    if (next_test_item_location != NULL)
    {
        *next_test_item_location = tmp_list_item->next;
    }

    destroy_test(&tmp_list_item->test);
    kfree(tmp_list_item);
    *test_list_item_location = NULL;
    return OK;
}

result_code_e destroy_test_list(/* in */ ptr_test_list_s * test_list_location)
{
    ptr_test_list_s tmp_list = NULL;
    ptr_test_list_item_s tmp_test_item = NULL;
    if (test_list_location == NULL || *test_list_location == NULL)
    {
        return ERROR_NULL_ARGUMENT;
    }

    tmp_list = *test_list_location;
    tmp_test_item = tmp_list->head;
    while (tmp_test_item  != NULL)
    {
        ptr_test_list_item_s tmp_next_test_item = NULL;
        if (destroy_test_list_item(&tmp_test_item, &tmp_next_test_item) != OK)
        {
            break;
        }

        tmp_test_item = tmp_next_test_item;
    }

    kfree(tmp_list);
    *test_list_location = NULL;
    return OK;
}

static result_code_e add_test_list_item(/* in */ ptr_test_list_item_s test_item, /* in */ ptr_test_list_s test_list)
{
    if(test_item == NULL || test_list == NULL)
    {
        return  ERROR_NULL_ARGUMENT;
    }

    test_list->count++;
    test_item->next = test_list->head;
    test_list->head = test_item;
    return OK;
}

static int find_test_for_each(void *data, const char *namebuf, struct module *module, unsigned long address)
{
    ptr_test_query_s query = NULL;
    ptr_test_s test = NULL;
    ptr_test_list_item_s test_item = NULL;
    const char * module_name= module == NULL ? "__KERNEL__" : module->name;
    if (address ==0)
    {
        return 0;
    }

    query = (ptr_test_query_s)data;
    if (glob_match(query->module_filter, module_name) == 0)
    {
        return 0;
    }

    if (glob_match(query->test_function_filter, namebuf) == 0)
    {
        return 0;
    }

    if (create_test(namebuf, module_name, (test_function_ptr)address, &test) != OK)
    {
        return 0;
    }

    if (create_test_list_item(test, &test_item) != OK)
    {
        destroy_test(&test);
        return 0;
    }

    if (add_test_list_item(test_item, query->result_list) != OK)
    {
        destroy_test_list_item(&test_item, NULL);
    }

    return 0;
}

//}
