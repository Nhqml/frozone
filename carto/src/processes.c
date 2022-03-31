#include "processes.h"
#include <errno.h>
#include <stdio.h>

#ifdef __OpenBSD__
    #include <limits.h>
    #include <err.h>
#else
    #include <linux/limits.h>
    #include <error.h>
#endif

#include "utils.h"

char* proc_readlink(pid_t pid, const char* attribute)
{
    char buffer[PATH_MAX];

    if (snprintf(buffer, PATH_MAX, "/proc/%d/%s", pid, attribute) == -1)
    #ifdef __OpenBSD__
        errc(1, errno, "sprintf error");
    #else
        error(1, errno, "sprintf error");
    #endif

    return readlink_str(buffer);
}
