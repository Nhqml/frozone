#pragma once

#define __UNUSED__        __attribute__((unused))
#define __VISIBILITY__(V) __attribute__((visibility(V)))

#define _GNU_SOURCE

#include <utmpx.h>

typedef struct utmpx utmp_t;

/**
 * Return a NULL-terminated array of currently logged-in users
 *
 * The caller is responsible for freeing the memory
 */
__VISIBILITY__("default")
utmp_t** get_users(void);
// TODO: add parameter to filter user types

__VISIBILITY__("default")
pid_t** get_processes(void);

__VISIBILITY__("default")
void get_connections(void);

__VISIBILITY__("default")
void get_files(void);
