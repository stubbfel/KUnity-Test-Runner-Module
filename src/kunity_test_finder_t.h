#ifndef KUNITY_TEST_FINDER_T_H
#define KUNITY_TEST_FINDER_T_H

//{ global include region

#include <linux/types.h>

//}

//{ local include region

//}


//{ define region

//}


//{ enum region

typedef enum result_code_eTag
{
    OK,
    ERROR_NULL_ARGUMENT,
    ERROR_INVALID_ARGUMENT,
    ERROR_INVALID_OPERATION
} result_code_e, *ptr_result_code_e;

//}


//{ typedef region

typedef void (*test_function_ptr) (void);

//}

//{ struct region

#pragma pack(push, 1)

#pragma pack(pop)

typedef struct test_sTag
{
    const char * name;
    const char * modul_name;
    test_function_ptr test_function;
} test_s, *ptr_test_s;

typedef struct test_list_item_sTag
{
    ptr_test_s test;
    struct test_list_item_sTag * next;
} test_list_item_s, *ptr_test_list_item_s;

typedef struct test_list_sTag
{
    size_t count;
    ptr_test_list_item_s head;
} test_list_s, *ptr_test_list_s;

typedef struct test_query_sTag
{
    const char * test_function_filter;
    const char * module_filter;
    ptr_test_list_s result_list;
} test_query_s, *ptr_test_query_s;

//}

#endif // KUNITY_TEST_FINDER_T_H
