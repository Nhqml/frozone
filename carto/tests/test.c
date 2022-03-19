#include "test.h"

#include <CUnit/Basic.h>

#include "carto.h"

int suite_init(void)
{
    return 0;
}

int suite_clean(void)
{
    return 0;
}

void test()
{
    CU_ASSERT(0 + 2 == 2);
    CU_ASSERT(0 - 2 == -2);
    CU_ASSERT(2 + 2 == 4);
}

void will_fail()
{
    CU_ASSERT(1 == 2);
}
