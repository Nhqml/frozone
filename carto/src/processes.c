#include "processes.h"

#include <errno.h>
#include <err.h>
#include <limits.h>
#include <stdio.h>

#include "utils.h"

char* proc_readlink(pid_t pid, const char* attribute)
{
    char buffer[PATH_MAX];

    if (snprintf(buffer, PATH_MAX, "/proc/%d/%s", pid, attribute) == -1)
        errc(1, errno, "sprintf error");

    return readlink_str(buffer);
}
