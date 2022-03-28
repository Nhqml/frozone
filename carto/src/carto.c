/* SPDX-License-Identifier: MIT */
/*
 * Copyright (C) 2022 Kenji Gaillac, Valentin Seux
 */

#include <carto.h>
#include <linux/limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

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

        char path_buf[PATH_MAX];
        if (snprintf(path_buf, PATH_MAX, "/proc/%d", process->pid) == -1)
            error(1, errno, "snprintf error");

        struct stat process_stat;
        if (stat(path_buf, &process_stat) == 0)
        {
            process->uid = process_stat.st_uid;
            process->gid = process_stat.st_gid;
            process->etime = time(NULL) - process_stat.st_ctime;
        }

        process->exe_path = proc_readlink(process->pid, "exe");
        process->cwd = proc_readlink(process->pid, "cwd");
        process->root = proc_readlink(process->pid, "root");

        if (snprintf(path_buf, PATH_MAX, "/proc/%d/cmdline", process->pid) == -1)
            error(1, errno, "snprintf error");
        FILE* f = fopen(path_buf, "r");
        if (f != NULL)
        {
            char proc_cmdline[ARG_MAX];
            size_t ret = fread(proc_cmdline, 1, ARG_MAX, f);
            if (ret > 0)
            {
                process->cmdline = xmalloc(ret + 1);
                memcpy(process->cmdline, proc_cmdline, ret);
                process->cmdline[ret] = '\0';
            }
            fclose(f);
        }

        array_push(processes, process);
    }

    array_push(processes, NULL);

    array_destroy(pids);

    return (process_t**)array_as_raw(processes);
}

void get_connections(void) {}

file_t** get_files(void)
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
        pid_t cur_pid = *(int*)pids->array[i];
        if (snprintf(proc_pid_path, PATH_MAX, "/proc/%d/fd", cur_pid) == -1)
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
            {
                file_t* file = xcalloc(1, sizeof(file_t));
                file->path = file_name;
                file->pid = cur_pid;

                struct stat file_stat;
                if (stat(proc_fd_path, &file_stat) == 0)
                    file->file_stat = file_stat;

                array_push(a, file);
            }
        }

        array_destroy(fds);
    }

    array_destroy(pids);

    array_push(a, NULL);

    return (file_t**)array_as_raw(a);
}
