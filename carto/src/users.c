#include <stdio.h>
#include <utmpx.h>

#include "utils.h"

typedef struct utmpx utmp_t;

void print_users(void)
{
    // Rewind file ptr
    setutxent();

    utmp_t* utmp = getutxent();
    print("%s", utmp->ut_user);

    // Closes UTMP file
    endutxent();
}
