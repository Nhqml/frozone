#include <CUnit/CUnit.h>
#include <carto.h>
#include <stdlib.h>

int carto_ts_init(void)
{
    return 0;
}

int carto_ts_clean(void)
{
    return 0;
}

void test_get_users(void)
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

void test_get_processes(void)
{
    process_t** processes = get_processes();

    // Should always return something
    CU_ASSERT_PTR_NOT_NULL(processes);

    // Should never return an empty array (since at least one process should be running)
    CU_ASSERT_PTR_NOT_NULL(*processes);

    // Since PIDs are returned in ascending order, the first one should be PID 1 (init process)
    CU_ASSERT_EQUAL(processes[0]->pid, 1);

    for (process_t** process = processes; *process != NULL; ++process)
    {
        // No PID should be equal to 0
        CU_ASSERT_NOT_EQUAL((*process)->pid, 0);
        printf("%d: %s (%s) | cwd: %s, root: %s\n", (*process)->pid, (*process)->exe_path, (*process)->cmdline,
               (*process)->cwd, (*process)->root);
        free(*process);
    }

    free(processes);
}

void test_get_connections(void)
{
    CU_FAIL('Not implemented');
}

void test_get_files(void)
{
    char** files = get_files();

    // Should always return something
    CU_ASSERT_PTR_NOT_NULL(files);

    // Should never return an empty array (since at least one file should be opened)
    CU_ASSERT_PTR_NOT_NULL(*files);

    for (char** file = files; *file != NULL; ++file)
    {
        CU_ASSERT_PTR_NOT_NULL(*file);
        // No empty file name
        CU_ASSERT_NOT_EQUAL(*file[0], '\0');

        free(*file);
    }

    free(files);
}
