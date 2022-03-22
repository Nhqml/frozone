#include <carto.h>
#include <stdbool.h>
#include <stdio.h>

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

void get_processes(void) {}

void get_connections(void) {}

void get_files(void) {}
