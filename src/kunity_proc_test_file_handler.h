#ifndef KUNITY_PROC_TEST_FILE_HANDLER_H
#define KUNITY_PROC_TEST_FILE_HANDLER_H

//{ global include region

#include <linux/proc_fs.h>

//}
//{ local include region

#include "kunity_test_finder_t.h"

//}

//{ function region

extern result_code_e create_proc_test_structure(/* in */ struct proc_dir_entry* parent, /* in */ const ptr_test_list_s test_list);

//}

#endif // KUNITY_PROC_TEST_FILE_HANDLER_H
