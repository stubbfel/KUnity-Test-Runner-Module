//{ global include region

#include <linux/module.h>
#include <kunity_t.h>

//}

//{ local include region

#include "kunity_test_finder.h"

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

static int kunity_init(void);
static void kunity_exit(void);

//}

//{ global variabel implements region

//}

//{ local variabel implements region

static test_query_s query;

//}


//{ global function implements region

//}


//{ local function implements region

static int kunity_init()
{
    pr_info("init kunity_test_runner_module");
    find_tests(&query);
    return 0;
}

static void kunity_exit()
{
    pr_info("init kunity_test_runner_module");
    destroy_test_list(&query.result_list);
}

//}


module_init(kunity_init)
module_exit(kunity_exit)

MODULE_AUTHOR("Felix Stubbe");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("run tests");
