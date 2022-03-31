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
    println("%p\n", kernel);
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

        strlcpy(process->cmdline, kinfo[i].p_comm, _POSIX2_LINE_MAX);

        array_push(processes, &kinfo[i]);
    }
    array_push(processes, NULL);
    return (process_t**)array_as_raw(processes);
}

void get_connections(void) {}

char** get_files(void)
{
    Array* a = array_new();

	struct kinfo_file *kf, *kflast;
	char *memf, *nlistf;
	char buf[_POSIX2_LINE_MAX];
	int cnt, flags;
	nlistf = memf = NULL;

	uid = getuid();

	flags = KVM_NO_FILES;

	if ((kd = kvm_openfiles(nlistf, memf, NULL, flags, buf)) == NULL)
		errx(1, "%s", buf);


	if ((kf = kvm_getfiles(kd, KERN_FILE_BYPID, -1, sizeof(*kf), &cnt)) == NULL)
		errx(1, "%s", kvm_geterr(kd));


	if (pledge("stdio rpath getpw", NULL) == -1)
		err(1, "pledge");

	find_splices(kf, cnt);
	fstat_header();

	for (kflast = &kf[cnt]; kf < kflast; ++kf) {
		fstat_dofile(kf);
	}


    array_push(a, NULL);

    return (char**)array_as_raw(a);
}
