#pragma once

#include <sys/types.h>

typedef struct varent {
	struct varent *next;
	struct var *var;
} VARENT;

struct kinfo_proc;

typedef struct var {
	char	*name;
	char	*header;
	char	*alias;
#define	COMM	0x01
#define	LJUST	0x02
#define	USER	0x04
#define	INF127	0x08
#define	NLIST	0x10
	u_int	flag;
	void	(*oproc)(const struct kinfo_proc *, struct varent *);
	short	width;
	char	parsed;
	int	off;
	enum type type;
	char *fmt;
	char *time;
} VAR;

char* proc_readlink(pid_t pid, const char* attribute);
