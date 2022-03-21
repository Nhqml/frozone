#include "test_utils.h"

#include <CUnit/Basic.h>

#include "../src/utils.h"

int utils_ts_init(void)
{
    return 0;
}

int utils_ts_clean(void)
{
    return 0;
}

void test_array_new(void)
{
    Array* a = array_new();

    CU_ASSERT_PTR_NOT_NULL(a);
    CU_ASSERT_EQUAL(a->size, 0);
    CU_ASSERT_EQUAL(a->capacity, ARRAY_DEFAULT_CAPACITY);

    array_destroy(a);
}

void test_array_with_capacity(void)
{
    Array* a = array_with_capacity(30);

    CU_ASSERT_PTR_NOT_NULL(a);
    CU_ASSERT_EQUAL(a->size, 0);
    CU_ASSERT_EQUAL(a->capacity, 30);

    array_destroy(a);

    a = array_with_capacity(100);

    CU_ASSERT_PTR_NOT_NULL(a);
    CU_ASSERT_EQUAL(a->size, 0);
    CU_ASSERT_EQUAL(a->capacity, 100);

    array_destroy(a);
}

void test_array_push(void)
{
    Array* a = array_new();

    for (size_t i = 0; i < 15; ++i)
        array_push(a, NULL);

    CU_ASSERT_PTR_NOT_NULL(a);
    CU_ASSERT_EQUAL(a->size, 15);
    CU_ASSERT_EQUAL(a->capacity, 20);

    array_destroy(a);
}