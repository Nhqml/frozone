#include <carto.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

#include "users.h"
#include "utils.h"

utmp_t** get_users(void)
{
    // Rewind file ptr
    setutxent();

    Array* a = array_new();

    utmp_t* utmp = getutxent();
    while (utmp != NULL)
    {
        // As specified in the doc, returned pointer points to 'static' memory that we cannot "own". Therefore we
        // must copy the whole utmp
        utmp_t* cloned_utmp = clone_utmp(utmp);
        array_push(a, cloned_utmp);

        utmp = getutxent();
    }

    array_push(a, NULL);

    // Closes UTMP file
    endutxent();

    // Will probably leak some memory (Array `a` won't be reachable anymore)
    return (utmp_t**)array_as_raw(a);
}

pid_t** get_processes(void)
{
    Array* pids = get_num_dir_contents("/proc");
    array_push(pids, NULL);

    return (pid_t**)array_as_raw(pids);
}

void get_connections(void) {}

char** get_files(void)
{
    Array* a = array_new();

    Array* pids = get_num_dir_contents("/proc");

    if (pids == NULL)
    {
        array_destroy(a);
        array_destroy(pids);
        return NULL;
    }

    char proc_pid_path[PATH_MAX], proc_fd_path[PATH_MAX];
    for (size_t i = 0; i < pids->size; ++i)
    {
        if (sprintf(proc_pid_path, "/proc/%d/fd", *(int*)pids->array[i]) == -1)
            error(1, errno, "sprintf error");

        Array* fds = get_num_dir_contents(proc_pid_path);

        if (fds == NULL)
            continue;

        for (size_t j = 0; j < fds->size; ++j)
        {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-overflow"
            if (sprintf(proc_fd_path, "%s/%d", proc_pid_path, *(int*)fds->array[j]) == -1)
                error(1, errno, "sprintf error");
#pragma GCC diagnostic pop

            char* file_name = xmalloc(PATH_MAX);
            int ret = readlink(proc_fd_path, file_name, PATH_MAX);

            if (ret == -1)
            {
                free(file_name);
                continue;
            }

            file_name[ret] = '\0';
            array_push(a, file_name);
        }

        array_destroy(fds);
    }

    array_destroy(pids);

    array_push(a, NULL);

    return (char**)array_as_raw(a);
}
