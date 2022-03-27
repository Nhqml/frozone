#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>

#include "test_freezer.h"

int main()
{
    if (CU_initialize_registry() != CUE_SUCCESS)
        return CU_get_error();

    /* "Freezer E2E" Tests Suite */
    CU_pSuite freezer_ts = CU_add_suite("Freezer", freezer_ts_init, freezer_ts_clean);
    if (NULL == freezer_ts)
    {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if (CU_add_test(freezer_ts, "Freezer::connections", test_freeze_connections) == NULL
        || CU_add_test(freezer_ts, "Freezer::processes", test_freeze_processes) == NULL
    )
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
