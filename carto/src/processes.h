#pragma once

#include <sys/types.h>

typedef struct varent {
	struct varent *next;
	struct var *var;
} VARENT;

char* proc_readlink(pid_t pid, const char* attribute);
