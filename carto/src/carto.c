#include <carto.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#include <stdio.h>
#include <kvm.h>
#include <limits.h>
#include <sys/param.h>
#include <sys/sysctl.h>

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

process_t** get_processes(void)
{
    char errbuf[_POSIX2_LINE_MAX];
    kvm_t *kernel = kvm_openfiles(NULL, NULL, NULL, KVM_NO_FILES, errbuf);
    int nentries = 0;
    struct kinfo_proc *kinfo = kvm_getprocs(kernel, KERN_PROC_ALL, 0, sizeof(struct kinfo_proc), &nentries);
    Array* processes = array_with_capacity(nentries + 1);
    int i;
    for (i = 0; i < nentries; ++i) {
        process_t* process = xcalloc(1, sizeof(process_t));
        process->pid = kinfo[i].p_pid;
        process->uid = kinfo[i].p_uid;
        process->gid = kinfo[i].p_gid;
        static time_t now;
        if (!now)
            (void)time(&now);
        process->etime = now - kinfo[i].p_ustart_sec;
        process->exe_path = proc_readlink(process->pid, "exe");
        process->cwd = proc_readlink(process->pid, "cwd");
        process->root = proc_readlink(process->pid, "root");

        strlcpy(process->cmdline, kp->p_comm, _POSIX2_LINE_MAX);

        array_push(processes, &kinfo[i]);
    }
    array_push(processes, NULL);
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
