#ifndef KUNITY_TEST_FINDER_T_H
#define KUNITY_TEST_FINDER_T_H

//{ global include region

#include <kunity_t.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/types.h>

//}

//{ define region
#ifndef KUNITY_DEFAULT_TEST_NAME_FITER
#define KUNITY_DEFAULT_TEST_NAME_FITER KUNITY_DEFAULT_TEST_NAME_PREFIX*
#endif

#ifndef KUNITY_DEFAULT_TEST_NAME_FITER_STR
#define STR(s) #s
#define XSTR(s) STR(s)
#define KUNITY_DEFAULT_TEST_NAME_FITER_STR XSTR(KUNITY_DEFAULT_TEST_NAME_FITER)
#endif
//}

//{ struct region

#pragma pack(push, 1)

#pragma pack(pop)

typedef struct test_list_item_sTag {
    struct list_head test_head;
    ptr_test_s test;
} test_list_item_s, *ptr_test_list_item_s;

typedef struct test_module_list_item_sTag {
    struct list_head test_module_head;
    const struct module* test_module;
    struct list_head test_list;
} test_module_list_item_s, *ptr_test_module_list_item_s;

typedef struct test_list_sTag {
    struct list_head test_module_list;
} test_list_s, *ptr_test_list_s;

typedef struct test_query_sTag {
    const char* test_function_filter;
    const char* module_filter;
    ptr_test_list_s result_list;
} test_query_s, *ptr_test_query_s;

typedef result_code_e (*test_list_iterator)(/* in */ const ptr_test_list_item_s);

typedef result_code_e (*test_module_list_iterator)(/* in */ const ptr_test_module_list_item_s);

//}

#endif // KUNITY_TEST_FINDER_T_H
