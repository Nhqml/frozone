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

#include <sys/types.h>
#include <sys/queue.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/vnode.h>
#include <sys/socket.h>
#include <sys/socketvar.h>
#include <sys/eventvar.h>
#include <sys/sysctl.h>
#include <sys/filedesc.h>
#define _KERNEL /* for DTYPE_* */
#include <sys/file.h>
#undef _KERNEL

#include <net/route.h>
#include <netinet/in.h>

#include <netdb.h>
#include <arpa/inet.h>

#include <sys/pipe.h>

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <kvm.h>
#include <limits.h>
#include <nlist.h>
#include <pwd.h>
#include <search.h>
#include <signal.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <err.h>

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

        strlcpy(process->cmdline, kinfo[i].p_comm, _POSIX2_LINE_MAX);

        array_push(processes, &kinfo[i]);
    }
    array_push(processes, NULL);
    return (process_t**)array_as_raw(processes);
}

void get_connections(void) {}

char** get_files(void)
{
    kvm_t *kd;
	struct kinfo_file *kf, *kflast;
	char *memf, *nlistf;
	char buf[_POSIX2_LINE_MAX];
	int cnt, flags;
	nlistf = memf = NULL;

    kf = NULL;
    cnt = 0;

    Array* a = array_new();
	flags = KVM_NO_FILES;
	if ((kd = kvm_openfiles(nlistf, memf, NULL, flags, buf)) == NULL)
    {
		errx(1, "%s", buf);
    }
    puts("1");
	if ((kf = kvm_getfiles(kd, KERN_FILE_BYFILE, 0, sizeof(*kf), &cnt)) == NULL)
    {
        puts("2");
		errx(1, "%s", kvm_geterr(kd));
    }
    puts("3");
	if (pledge("stdio rpath getpw", NULL) == -1)
    {
        puts("4");
		err(1, "pledge");
    }
    puts("5");
	for (kflast = &kf[cnt]; kf < kflast; ++kf) {
		array_push(a, kflast);
	}
    puts("6");

    array_push(a, NULL);

    return (char**)array_as_raw(a);
}
