#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>

#include "test.h"

int main()
{
    CU_pSuite pSuite = NULL;

    if (CU_initialize_registry() != CUE_SUCCESS)
        return CU_get_error();

    pSuite = CU_add_suite("Tests suite", suite_init, suite_clean);
    if (NULL == pSuite)
    {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if (CU_add_test(pSuite, "test", test) == NULL || CU_add_test(pSuite, "Failing test", will_fail) == NULL)
    {
        CU_cleanup_registry();
        return CU_get_error();
    }

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_set_error_action(CUEA_IGNORE);

    CU_basic_run_tests();
    unsigned int failed_tests = CU_get_number_of_tests_failed();

    CU_cleanup_registry();

    CU_ErrorCode error_code = CU_get_error();
    if (error_code != CUE_SUCCESS)
        return error_code;

    if (failed_tests != 0)
        return -1;
}
