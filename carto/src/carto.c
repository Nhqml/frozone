#include <carto.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#include <sys/param.h>
#include <sys/types.h>
#include <sys/signal.h>
#include <sys/sysctl.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/proc.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <ctype.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <kvm.h>
#include <locale.h>
#include <nlist.h>
#include <paths.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

#include "processes.h"
#include "users.h"
#include "utils.h"

utmp_t** get_users(void)
{
	struct utmp utmp;

	if (unveil(_PATH_UTMP, "r") == -1)
    {
	    err(1, "unveil %s", _PATH_UTMP);
    }
	if (pledge("stdio rpath", NULL) == -1)
    {
		err(1, "pledge");
    }
	if (!freopen(_PATH_UTMP, "r", stdin))
    {
		err(1, "can't open %s", _PATH_UTMP);
	}

    Array* a = array_new();

    while (fread((char*)&utmp, sizeof(utmp), 1, stdin) == 1)
    {
        utmp_t* cloned_utmp = clone_utmp(&utmp);
        array_push(a, cloned_utmp);
    }

    array_push(a, NULL);

    return (utmp_t**)array_as_raw(a);
}

struct varent *vhead;
int eval;
kvm_t *kd;
int kvm_sysctl_only;

process_t** get_processes(void)
{
    struct varent *vent;
    struct kinfo_proc *kp, **kinfo;
    int what, flag, nentries, lineno, i;
    char errbuf[_POSIX2_LINE_MAX];
    kd = kvm_open(NULL, NULL, NULL, KVM_NO_FILES, errbuf);
    kvm_sysctl_only = 1;
	if (kd == NULL)
    {
		errx(1, "%s", errbuf);
    }
    what = KERN_PROC_ALL;
    flag = 0;
    kp = kvm_getprocs(kd, what, flag, sizeof(*kp), &nentries);
	if (kp == NULL)
    {
		errx(1, "%s", kvm_geterr(kd));
    }
    //printheader();
	if (nentries == 0)
    {
		exit(1);
    }
	if ((kinfo = reallocarray(NULL, nentries, sizeof(*kinfo))) == NULL)
    {
		err(1, "failed to allocate memory for proc pointers");
    }
	for (i = 0; i < nentries; i++)
    {
		kinfo[i] = &kp[i];
    }
	for (i = lineno = 0; i < nentries; i++)
    {
		if (((int)kinfo[i]->p_tdev == NODEV || (kinfo[i]->p_psflags & PS_CONTROLT ) == 0))
        {
			continue;
        }
		if (kinfo[i]->p_tid == -1)
        {
			continue;
        }
		for (vent = vhead; vent; vent = vent->next)
        {
			(vent->var->oproc)(kinfo[i], vent);
			if (vent->next != NULL)
				(void)putchar(' ');
		}
		(void)putchar('\n');
	}
	exit(eval);

    Array* pids = get_num_dir_contents("/proc");

    Array* processes = array_with_capacity(pids->size + 1);

    for (size_t i = 0; i < pids->size; ++i)
    {
        process_t* process = xcalloc(1, sizeof(process_t));
        process->pid = *(int*)pids->array[i];

        char path_buf[PATH_MAX];
        if (snprintf(path_buf, PATH_MAX, "/proc/%d", process->pid) == -1)
            errc(1, errno, "snprintf error");

        struct stat process_stat;
        if (stat(path_buf, &process_stat) == 0)
        {
            process->uid = process_stat.st_uid;
            process->gid = process_stat.st_gid;
            process->etime = time(NULL) - process_stat.st_ctime;
        }

        process->exe_path = proc_readlink(process->pid, "exe");
        process->cwd = proc_readlink(process->pid, "cwd");
        process->root = proc_readlink(process->pid, "root");

        if (snprintf(path_buf, PATH_MAX, "/proc/%d/cmdline", process->pid) == -1)
            errc(1, errno, "snprintf error");
        FILE* f = fopen(path_buf, "r");
        if (f != NULL)
        {
            char proc_cmdline[ARG_MAX];
            size_t ret = fread(proc_cmdline, 1, ARG_MAX, f);
            if (ret > 0)
            {
                process->cmdline = xmalloc(ret + 1);
                memcpy(process->cmdline, proc_cmdline, ret);
                process->cmdline[ret] = '\0';
            }
            fclose(f);
        }

        array_push(processes, process);
    }

    array_push(processes, NULL);

    array_destroy(pids);

    return (process_t**)array_as_raw(processes);
}

void get_connections(void) {}

char** get_files(void)
{
    Array* a = array_new();

    Array* pids = get_num_dir_contents("/proc");

    if (pids == NULL)
    {
        array_destroy(a);
        array_destroy(pids);
        return NULL;
    }

    char proc_pid_path[PATH_MAX], proc_fd_path[PATH_MAX];
    for (size_t i = 0; i < pids->size; ++i)
    {
        if (snprintf(proc_pid_path, PATH_MAX, "/proc/%d/fd", *(int*)pids->array[i]) == -1)
            errc(1, errno, "sprintf error");

        Array* fds = get_num_dir_contents(proc_pid_path);

        if (fds == NULL)
            continue;

        for (size_t j = 0; j < fds->size; ++j)
        {
            if (snprintf(proc_fd_path, PATH_MAX, "%s/%d", proc_pid_path, *(int*)fds->array[j]) == -1)
                errc(1, errno, "sprintf error");

            char* file_name = readlink_str(proc_fd_path);

            if (file_name != NULL)
                array_push(a, file_name);
        }

        array_destroy(fds);
    }

    array_destroy(pids);

    array_push(a, NULL);

    return (char**)array_as_raw(a);
}
