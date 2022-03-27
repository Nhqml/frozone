#pragma once

#define __UNUSED__        __attribute__((unused))
#define __VISIBILITY__(V) __attribute__((visibility(V)))

#define _GNU_SOURCE

#include <utmpx.h>

typedef struct utmpx utmp_t;

/**
** \brief Return a NULL-terminated array of currently logged-in users
**
** The caller is responsible for freeing the memory
*/
__VISIBILITY__("default")
utmp_t** get_users(void);

/**
** \brief Return a NULL-terminated array of PIDs (running processes)
*/
__VISIBILITY__("default")
pid_t** get_processes(void);
// TODO(Valentin, Kenji): select what's interesting about processed that we want to provide, not only PIDs

__VISIBILITY__("default")
void get_connections(void);

/**
** \brief Return a NULL-terminated array of paths (active file descriptors)
**
** The caller is responsible for freeing the memory
*/
__VISIBILITY__("default")
char** get_files(void);
