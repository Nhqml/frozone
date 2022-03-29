/* SPDX-License-Identifier: MIT */
/*
 * Copyright (C) 2022 Kenji Gaillac
 */

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

    // Since PIDs are returned in ascending order, the first one should be PID 1 (init process), owned by root (UID =
    // GID = 0)
    CU_ASSERT_EQUAL(processes[0]->pid, 1);
    CU_ASSERT_EQUAL(processes[0]->uid, 0);
    CU_ASSERT_EQUAL(processes[0]->gid, 0);

    for (process_t** process = processes; *process != NULL; ++process)
    {
        // No PID should be equal to 0
        CU_ASSERT_NOT_EQUAL((*process)->pid, 0);

        // Elapsed time must be greater or equal to 0
        CU_ASSERT_TRUE((*process)->etime >= 0);

        free((*process)->exe_path);
        free((*process)->cmdline);
        free((*process)->cwd);
        free((*process)->root);
        free(*process);
    }

    free(processes);
}

void test_get_connections(void)
{
    CU_PASS('Not implemented');
}

void test_get_files(void)
{
    file_t** files = get_files();

    // Should always return something
    CU_ASSERT_PTR_NOT_NULL(files);

    // Should never return an empty array (since at least one file should be opened)
    CU_ASSERT_PTR_NOT_NULL(*files);

    for (file_t** file = files; *file != NULL; ++file)
    {
        // No missing or empty file path
        CU_ASSERT_PTR_NOT_NULL((*file)->path);
        CU_ASSERT_NOT_EQUAL(((*file)->path)[0], '\0');

        free((*file)->path);
        free(*file);
    }

    free(files);
}
