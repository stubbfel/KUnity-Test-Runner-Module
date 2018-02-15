#ifndef KUNITY_TEST_FINDER_H
#define KUNITY_TEST_FINDER_H
#include "kunity_test_finder_t.h"

//{ function region

extern result_code_e find_tests(/* in */ const ptr_test_query_s query);

extern result_code_e destroy_test_list(/* in */ ptr_test_list_s* test_list_location);

//}

#endif // KUNITY_TEST_FINDER_H
