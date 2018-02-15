//{ global include region

#include <kunity_t.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/workqueue.h>

//}

//{ local include region

#include "kunity_proc_test_file_handler.h"
#include "kunity_test_finder.h"

//}

//{ local typedef region

typedef struct runner_control_block_sTag {
    const char* proc_folder_name;
    struct proc_dir_entry* proc_test_root;
    test_query_s query;
} runner_control_block_s, *ptr_runner_control_blocks;

//}

//{ local function prototypes region

static void init_tests(/* in */ struct work_struct* work);

static void clean_tests(void);

static int kunity_init(void);

static void kunity_exit(void);

//}

//{ local variables region

static struct workqueue_struct* queue;

DECLARE_WORK(work, init_tests);

static char* module_filter = "*";
module_param(module_filter, charp, 0);
MODULE_PARM_DESC(module_filter, "a filter option for modules, which the runner is looking for test functions. (Default is \"*\"");

static char* test_function_filter = KUNITY_DEFAULT_TEST_NAME_FITER_STR;
module_param(test_function_filter, charp, 0);
MODULE_PARM_DESC(test_function_filter, "a filter option for test function names, which the runner is looking for. (Default is \"kunity_test_*\"");

static char* proc_test_root_folder_name = "kunity_test";
module_param(proc_test_root_folder_name, charp, 0);
MODULE_PARM_DESC(proc_test_root_folder_name, "the name of the proc folder, which will contains the test proc files. (Default is \"kunity_test\"");

static const size_t sizeof_runner_control_block = sizeof(runner_control_block_s);

static runner_control_block_s* rcb = NULL;

//}

//{ local function implements region

static void clean_tests()
{
    destroy_test_list(&rcb->query.result_list);
    if (rcb->proc_test_root != NULL) {
        proc_remove(rcb->proc_test_root);
    }

    kzfree(rcb);
}

static void init_tests(/* in */ struct work_struct* work)
{
    result_code_e result = OK;
    if (rcb != NULL) {
        clean_tests();
    }

    rcb = kzalloc(sizeof(runner_control_block_s), GFP_KERNEL);
    rcb->query.test_function_filter = test_function_filter;
    rcb->query.module_filter = module_filter;
    rcb->proc_folder_name = proc_test_root_folder_name;
    rcb->proc_test_root = proc_mkdir(rcb->proc_folder_name, NULL);
    if (rcb->proc_test_root == NULL) {
        pr_warning("cant create  proc folder %s", rcb->proc_folder_name);
        return;
    }

    result = find_tests(&rcb->query);
    if (result == OK) {
        result = create_proc_test_structure(rcb->proc_test_root, rcb->query.result_list);
        if (result == OK) {
            pr_info("successful created tests with following options:");
        } else {
            pr_info("test creation failed with error code %u, following options was used:", result);
        }
    } else {
        pr_info("could not find any test with follwing options:");
    }

    pr_info("\tmodule_filter = \"%s\"", rcb->query.module_filter);
    pr_info("\ttest_function_filter = \"%s\"", rcb->query.test_function_filter);
    pr_info("\tproc_folder_name = \"%s\"", rcb->proc_folder_name);
}

static int kunity_init()
{
    pr_info("init %s", THIS_MODULE->name);
    queue = create_workqueue("test_runner_work_queue");
    if (queue == NULL) {
        return -ERROR_INVALID_OPERATION;
    }

    return !queue_work(queue, &work);
}

static void kunity_exit()
{
    pr_info("exit %s", THIS_MODULE->name);
    if (queue == NULL) {
        return;
    }

    destroy_workqueue(queue);
    if (rcb == NULL) {
        return;
    }

    clean_tests();
}

//}

module_init(kunity_init);
module_exit(kunity_exit);
MODULE_AUTHOR("Felix Stubbe");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("run kunity tests");
