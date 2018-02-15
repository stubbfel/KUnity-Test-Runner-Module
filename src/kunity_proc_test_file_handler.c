#include "kunity_proc_test_file_handler.h"
//{ global include region

#include <kunity_t.h>
#include <linux/fs.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/seq_file.h>
#include <linux/slab.h>
#include <linux/string.h>

//}

//{ local function prototypes region

static int proc_test_file_open(/* in */ struct inode* inode, /* in */ struct file* file);

static int proc_test_file_show(/* in */ struct seq_file* m, /* in */ void* v);

static int proc_test_module_open(/* in */ struct inode* inode, /* in */ struct file* file);

static int proc_test_module_show(/* in */ struct seq_file* m, /* in */ void* v);

static void put_seq_char(/* in */ char letter);

//}

//{ local variables region

static const struct file_operations proc_test_file_ops = {
    .owner = THIS_MODULE,
    .open = proc_test_file_open,
    .read = seq_read,
    .llseek = seq_lseek,
    .release = single_release,
};

static const struct file_operations proc_test_module_ops = {
    .owner = THIS_MODULE,
    .open = proc_test_module_open,
    .read = seq_read,
    .llseek = seq_lseek,
    .release = single_release,
};

static struct seq_file* test_proc_file;

//}

//{ global function implements region

result_code_e create_proc_test_structure(/* in */ struct proc_dir_entry* parent, /* in */ const ptr_test_list_s test_list)
{
    ptr_test_module_list_item_s module = NULL;
    result_code_e result = OK;
    if (test_list == NULL) {
        return ERROR_NULL_ARGUMENT;
    }

    list_for_each_entry(module, &test_list->test_module_list, test_module_head)
    {
        ptr_test_list_item_s test_item = NULL;
        const char* module_name = module == NULL ? "__KERNEL__" : module->test_module->name;
        struct proc_dir_entry* module_folder = proc_mkdir(module_name, parent);
        struct proc_dir_entry* single_test_folder = proc_mkdir("single_tests", module_folder);
        struct proc_dir_entry* tmp_entry = proc_create_data(module_name, 0, module_folder, &proc_test_module_ops, module);
        if (tmp_entry == NULL) {
            return ERROR_INVALID_OPERATION;
        }

        list_for_each_entry(test_item, &module->test_list, test_head)
        {
            tmp_entry = proc_create_data(test_item->test->name, 0, single_test_folder, &proc_test_file_ops, test_item->test);
            if (tmp_entry == NULL) {
                return ERROR_INVALID_OPERATION + 1;
            }
        }
    }

    return result;
}

//}

//{ local function implements region

static void put_seq_char(/* in */ char letter)
{
    seq_putc(test_proc_file, letter);
}

static int proc_test_file_open(/* in */ struct inode* inode, /* in */ struct file* file)
{
    return single_open(file, proc_test_file_show, NULL);
}

static int proc_test_module_open(/* in */ struct inode* inode, /* in */ struct file* file)
{
    return single_open(file, proc_test_module_show, NULL);
}

static int proc_test_file_show(/* in */ struct seq_file* seqfile, /* in */ void* data)
{
    test_session_control_block_s output = { put_seq_char, 0, 0 };
    ptr_test_s tmp_test = PDE_DATA(file_inode(seqfile->file));
    if (tmp_test == NULL) {
        return ERROR_INVALID_ARGUMENT;
    }

    test_proc_file = seqfile;
    return tmp_test->test_function(&output);
}

static int proc_test_module_show(/* in */ struct seq_file* seqfile, /* in */ void* data)
{
    test_session_control_block_s output = { put_seq_char, 0, 1 };
    ptr_test_list_item_s tmp_test = NULL;
    ptr_test_module_list_item_s tmp_module = PDE_DATA(file_inode(seqfile->file));
    if (tmp_module == NULL) {
        return ERROR_INVALID_ARGUMENT;
    }

    test_proc_file = seqfile;
    list_for_each_entry(tmp_test, &tmp_module->test_list, test_head)
    {
        result_code_e result = OK;
        if (list_is_last(&tmp_test->test_head, &tmp_module->test_list)) {
            output._skip_end = 0;
        }

        result = tmp_test->test->test_function(&output);
        if (result != OK) {
            return result;
        }

        output._skip_start = 1;
    }

    return OK;
}

//}
