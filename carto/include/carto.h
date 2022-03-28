#pragma once

#define __UNUSED__        __attribute__((unused))
#define __VISIBILITY__(V) __attribute__((visibility(V)))

#define _GNU_SOURCE

#include <sys/types.h>
#include <utmp.h>

typedef struct utmpx utmp_t;

typedef struct
{
    pid_t pid;      // PID
    char* exe_path; // Executable path
    char* cmdline;  // Command line
    char* cwd;      // Current Working Directory
    char* root;     // Filesystem root for this process
    uid_t uid;      // UID
    gid_t gid;      // GID
    time_t etime;   // Elasped time
} process_t;

/**
** \brief Return a NULL-terminated array of currently logged-in users
**
** The caller is responsible for freeing the memory
*/
__VISIBILITY__("default") utmp_t** get_users(void);

/**
** \brief Return a NULL-terminated array of PIDs (running processes)
*/
__VISIBILITY__("default")
process_t** get_processes(void);

__VISIBILITY__("default")
void get_connections(void);

/**
** \brief Return a NULL-terminated array of paths (active file descriptors)
**
** The caller is responsible for freeing the memory
*/
__VISIBILITY__("default")
char** get_files(void);
// TODO(Kenji): keep track of PID and files infos (UID/GID, mode, size, timestamps, link count)
