#include <carto.h>
#include <dirent.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/types.h>
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
        // Keep only normal processes
        if (utmp->ut_type == USER_PROCESS)
        {
            // As specified in the doc, returned pointer points to 'static' memory that we cannot "own". Therefore we
            // must copy the whole utmp
            utmp_t* cloned_utmp = clone_utmp(utmp);
            array_push(a, cloned_utmp);
        }

        utmp = getutxent();
    }

    array_push(a, NULL);

    // Closes UTMP file
    endutxent();

    // Will probably leak some memory (Array `a` won't be reachable anymore)
    return (utmp_t**)array_as_raw(a);
}

int* get_num_dir_contents(char* dir_path)
{
    DIR* dir = NULL;
    struct dirent* dir_cur = NULL;

    dir = opendir(dir_path);
    
    // log instead of error
    if (dir == NULL)
        error(1, errno, "failed to open %s directory", dir_path);
    
    Array* a = array_new();

    while ((dir_cur = readdir(dir)) != NULL)
    {
        int id = atoi(dir_cur->d_name);

        // if pid is 0, atoi did not encounter a valid number
        if (id)
            array_push(a, id);
    }

    closedir(dir);

    array_push(a, NULL);

    return (int*)array_as_raw(a);
}

void get_connections(void) {}

void get_files(void)
{
    pid_t* pids = get_num_dir_contents("/proc");

    Array* a = array_new();

    for (size_t i = 0; pids[i]; i++)
    {
        char* proc_pid_path = NULL;

        if (asprintf(&proc_pid_path, "/proc/%d/fd", pids[i]) == -1)
            error(1, errno, "asprintf error");
        
        int* fds = get_num_dir_contents(proc_pid_path);

        for (size_t j = 0; fds[j]; j++)
        {
            char* proc_fd_path = NULL;

            if (asprintf(&proc_fd_path, "%s/%d", proc_pid_path, fds[i]) == -1)
                error(1, errno, "asprintf error");

            char file_name[PATH_MAX] = { 0 };
            int ret = readlink(proc_fd_path, file_name, PATH_MAX);
            file_name[ret] = '\0';

            array_push(a, file_name);
        }
    }

    array_push(a, NULL);
}
