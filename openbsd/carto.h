/* SPDX-License-Identifier: MIT */
/*
 * Copyright (C) 2022 Kenji Gaillac, Valentin Seux
 */

#pragma once

#define __UNUSED__        __attribute__((unused))
#define __VISIBILITY__(V) __attribute__((visibility(V)))

#define _GNU_SOURCE

#include <arpa/inet.h>
#include <sys/stat.h>
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

typedef struct
{
    pid_t pid;             // PID
    char* path;            // File Path
    struct stat file_stat; // File stat informations
} file_t;

enum prot
{
    UDP,
    TCP,
};

/**
** \brief Return a NULL-terminated array of currently logged-in users
**
** The caller is responsible for freeing the memory
*/
__VISIBILITY__("default")
utmp_t** get_users(void);

/**
** \brief Return a NULL-terminated array of PIDs (running processes)
**
** The caller is responsible for freeing the memory
*/
__VISIBILITY__("default")
process_t** get_processes(void);

/**
** \brief Return a NULL-terminated array of current TCP and UDP connections
**
** The caller is responsible for freeing the memory
*/
__VISIBILITY__("default")
connection_t** get_connections(void);

/**
** \brief Return a NULL-terminated array of paths (active file descriptors)
**
** The caller is responsible for freeing the memory
*/
__VISIBILITY__("default")
file_t** get_files(void);
