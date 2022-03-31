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

void
find_splices(struct kinfo_file *kf, int cnt)
{
	int i, created;

	created = 0;
	for (i = 0; i < cnt; i++) {
		if (kf[i].f_type != DTYPE_SOCKET ||
		    (kf[i].so_splice == 0 && kf[i].so_splicelen != -1))
			continue;
		if (created++ == 0) {
			if (hcreate(1000) == 0)
				err(1, "hcreate");
		}
		splice_insert('>', kf[i].f_data, &kf[i]);
		if (kf[i].so_splice != 0)
			splice_insert('<', kf[i].so_splice, &kf[i]);
	}
}

void
splice_insert(char type, u_int64_t ptr, struct kinfo_file *data)
{
	ENTRY entry, *found;

	if (asprintf(&entry.key, "%c%llx", type, hideroot ? 0 : ptr) == -1)
		err(1, NULL);
	entry.data = data;
	if ((found = hsearch(entry, ENTER)) == NULL)
		err(1, "hsearch");
	/* if it's ambiguous, set the data to NULL */
	if (found->data != data)
		found->data = NULL;
}

void
fstat_header(void)
{
	if (nflg)
		printf("%s",
"USER     CMD          PID   FD  DEV      INUM        MODE   R/W    SZ|DV");
	else
		printf("%s",
"USER     CMD          PID   FD MOUNT        INUM  MODE         R/W    SZ|DV");
	if (oflg)
		printf("%s", ":OFFSET  ");
	if (checkfile && fsflg == 0)
		printf(" NAME");
	if (sflg)
		printf("    XFERS   KBYTES");
	putchar('\n');
}

void
fstat_dofile(struct kinfo_file *kf)
{
	int i;

	Uname = user_from_uid(kf->p_uid, 0);
	procuid = &kf->p_uid;
	Pid = kf->p_pid;
	Comm = kf->p_comm;

	for (i = 0; i < nfilter; i++) {
		if (filter[i].what == KERN_FILE_BYPID) {
			if (filter[i].arg == Pid)
				break;
		} else if (filter[i].arg == *procuid) {
			break;
		}
	}
	if (i == nfilter && nfilter != 0)
		return;

	switch (kf->f_type) {
	case DTYPE_VNODE:
		vtrans(kf);
		break;
	case DTYPE_SOCKET:
		socktrans(kf);
		break;
	case DTYPE_PIPE:
		if (checkfile == 0)
			pipetrans(kf);
		break;
	case DTYPE_KQUEUE:
		if (checkfile == 0)
			kqueuetrans(kf);
		break;
	default:
		if (vflg) {
			warnx("unknown file type %d for file %d of pid %ld",
			    kf->f_type, kf->fd_fd, (long)Pid);
		}
		break;
	}
}

char** get_files(void)
{
    uid_t uid;
    kvm_t *kd;

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
