/* SPDX-License-Identifier: MIT */
/*
 * Copyright (C) 2022 Kenji Gaillac
 */

#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>

#include "test_utils.h"

int main()
{
    if (CU_initialize_registry() != CUE_SUCCESS)
        return CU_get_error();

    /* "utils" Tests Suite */
    CU_pSuite utils_ts = CU_add_suite("Utils", utils_ts_init, utils_ts_clean);
    if (NULL == utils_ts)
    {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if (CU_add_test(utils_ts, "Array::new", test_array_new) == NULL
        || CU_add_test(utils_ts, "Array::with_capacity", test_array_with_capacity) == NULL
        || CU_add_test(utils_ts, "Array::push", test_array_push) == NULL)
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
