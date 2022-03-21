#include <CUnit/CUnit.h>
#include <carto.h>
#include <stdlib.h>

int carto_ts_init(void)
{
    return 0;
}

int carto_ts_clean(void)
{
    puts("\n\n");

    utmp_t** users = get_users();

    for (utmp_t** user = users; *user != NULL; ++user)
    {
        printf("User: '%s'\n", (*user)->ut_user);
        printf("Host: '%s'\n", (*user)->ut_host);
        printf("PID: %u\n", (*user)->ut_pid);
        printf("Device: %s\n", (*user)->ut_line);
        printf("Time: %d\n", (*user)->ut_tv.tv_sec);

        free(*user);
    }

    free(users);

    return 0;
}

void fail(void)
{
    utmp_t** users = get_users();

    // Should always return something
    CU_ASSERT_PTR_NOT_NULL(users);

    // Should never return an empty array (since at least one user should be logged in)
    CU_ASSERT_PTR_NOT_NULL(*users);

    for (utmp_t** user = users; *user != NULL; ++user)
        free(*user);

    free(users);
}
