#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>

#include "test_carto.h"

int main()
{
    if (CU_initialize_registry() != CUE_SUCCESS)
        return CU_get_error();

    /* "carto" Tests Suite */
    CU_pSuite carto_ts = CU_add_suite("Carto", carto_ts_init, carto_ts_clean);
    if (NULL == carto_ts)
    {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if (CU_add_test(carto_ts, "Fail", fail) == NULL)
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
