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
#include <utmpx.h>

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
    pid_t pid;
    char* path;
    struct stat file_stat;
} file_t;

enum conn_type
{
    UDP,
    TCP,
};

typedef struct
{
    uid_t uid;
    enum conn_type type;
    sa_family_t addr_type; // See <sys/socket.h>
    union
    {
        struct in_addr addr;
        struct in6_addr addr6;
    } s_addr, d_addr;
    in_port_t s_port, d_port;
    uint8_t state; // See tcp_states.h
} connection_t;

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
