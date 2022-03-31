/* SPDX-License-Identifier: MIT */
/*
 * Copyright (C) 2022 Kenji Gaillac
 */

#include "users.h"

#include <stdio.h>
#include <string.h>

#include "utils.h"

utmp_t* clone_utmp(utmp_t* utmp)
{
    utmp_t* cloned_utmp = xmalloc(sizeof(utmp_t));

    cloned_utmp->ut_type = utmp->ut_type;
    cloned_utmp->ut_pid = utmp->ut_pid;
    memcpy(cloned_utmp->ut_line, utmp->ut_line, __UT_LINESIZE);
    memcpy(cloned_utmp->ut_id, utmp->ut_id, 4);
    memcpy(cloned_utmp->ut_user, utmp->ut_user, __UT_NAMESIZE);
    memcpy(cloned_utmp->ut_host, utmp->ut_host, __UT_HOSTSIZE);
    cloned_utmp->ut_exit = utmp->ut_exit;
    cloned_utmp->ut_session = utmp->ut_session;
    cloned_utmp->ut_tv = utmp->ut_tv;

    return cloned_utmp;
}

void free_users(utmp_t** users)
{
    for (utmp_t** user = users; *user != NULL; ++user)
        free(*user);

    free(users);
}
