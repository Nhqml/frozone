#include <carto.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>

#include "processes.h"
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

process_t** get_processes(void)
{
    Array* pids = get_num_dir_contents("/proc");

    Array* processes = array_with_capacity(pids->size + 1);

    for (size_t i = 0; i < pids->size; ++i)
    {
        process_t* process = xcalloc(1, sizeof(process_t));
        process->pid = *(int*)pids->array[i];

        process->exe_path = proc_readlink(process->pid, "exe");
        process->cwd = proc_readlink(process->pid, "cwd");
        process->root = proc_readlink(process->pid, "root");

        process->cmdline = NULL;

        array_push(processes, process);
    }

    array_push(processes, NULL);

    array_destroy(pids);

    return (process_t**)array_as_raw(processes);
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
        if (snprintf(proc_pid_path, PATH_MAX, "/proc/%d/fd", *(int*)pids->array[i]) == -1)
            error(1, errno, "sprintf error");

        Array* fds = get_num_dir_contents(proc_pid_path);

        if (fds == NULL)
            continue;

        for (size_t j = 0; j < fds->size; ++j)
        {
            if (snprintf(proc_fd_path, PATH_MAX, "%s/%d", proc_pid_path, *(int*)fds->array[j]) == -1)
                error(1, errno, "sprintf error");

            char* file_name = readlink_str(proc_fd_path);

            if (file_name != NULL)
                array_push(a, file_name);
        }

        array_destroy(fds);
    }

    array_destroy(pids);

    array_push(a, NULL);

    return (char**)array_as_raw(a);
}
