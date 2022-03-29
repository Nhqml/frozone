#include "users.h"

#include <stdio.h>
#include <string.h>

#include "utils.h"

utmp_t* clone_utmp(utmp_t* utmp)
{
    puts("Before");
    utmp_t* cloned_utmp = xmalloc(sizeof(utmp_t));

    memcpy(cloned_utmp->ut_line, utmp->ut_line, UT_LINESIZE);
    memcpy(cloned_utmp->ut_name, utmp->ut_name, UT_LINESIZE);
    memcpy(cloned_utmp->ut_host, utmp->ut_host, UT_HOSTSIZE);
    cloned_utmp->ut_time = utmp->ut_time;
    puts("After");
    return cloned_utmp;
}

void free_users(utmp_t** users)
{
    for (utmp_t** user = users; *user != NULL; ++user)
        free(*user);

    free(users);
}
