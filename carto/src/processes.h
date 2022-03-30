#pragma once

#include <sys/types.h>

char* proc_readlink(pid_t pid, const char* attribute);
