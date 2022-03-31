#include <carto.h>

#ifdef __OpenBSD__
    #include <limits.h>
#else
    #include <linux/limits.h>
#endif

#include <stdbool.h>
#include <stdio.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#include "processes.h"
#include "users.h"
#include "utils.h"

#ifdef __OpenBSD__
    #include <kvm.h>
    #include <limits.h>
    #include <sys/param.h>
    #include <sys/sysctl.h>
    #include <err.h>
#endif

utmp_t** get_users(void)
{
    // Rewind file ptr
    setutxent();

    Array* a = array_new();

    utmp_t* utmp = getutxent();
    while (utmp != NULL)
    {
        // As specified in the doc, returned pointer points to 'static' memory that we cannot "own". Therefore we
        // must copy the whole utmp
        utmp_t* cloned_utmp = clone_utmp(utmp);
        array_push(a, cloned_utmp);

        utmp = getutxent();
    }

    array_push(a, NULL);

    // Closes UTMP file
    endutxent();

    // Will probably leak some memory (Array `a` won't be reachable anymore)
    return (utmp_t**)array_as_raw(a);
}

process_t** get_processes(void)
{
    Array* pids = get_num_dir_contents("/proc");

    Array* processes = array_with_capacity(pids->size + 1);

    for (size_t i = 0; i < pids->size; ++i)
    {
        process_t* process = xcalloc(1, sizeof(process_t));
        process->pid = *(int*)pids->array[i];

        char path_buf[PATH_MAX];
        if (snprintf(path_buf, PATH_MAX, "/proc/%d", process->pid) == -1)
            error(1, errno, "snprintf error");

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
            error(1, errno, "snprintf error");
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

connection_t** get_connections(void)
{
    Array* connections = array_new();

    add_connections_from("/proc/net/tcp", connections, TCP, AF_INET);
    add_connections_from("/proc/net/tcp6", connections, TCP, AF_INET6);
    add_connections_from("/proc/net/udp", connections, UDP, AF_INET);
    add_connections_from("/proc/net/udp6", connections, UDP, AF_INET6);

    array_push(connections, NULL);

    return (connection_t**)array_as_raw(connections);
}

file_t** get_files(void)
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
        pid_t cur_pid = *(int*)pids->array[i];
        if (snprintf(proc_pid_path, PATH_MAX, "/proc/%d/fd", cur_pid) == -1)
            error(1, errno, "sprintf error");

        Array* fds = get_num_dir_contents(proc_pid_path);

        if (fds == NULL)
            continue;

        for (size_t j = 0; j < fds->size; ++j)
        {
            if (snprintf(proc_fd_path, PATH_MAX, "%s/%d", proc_pid_path, *(int*)fds->array[j]) == -1)
                error(1, errno, "sprintf error");

            char* file_name = readlink_str(proc_fd_path);
            if (file_name != NULL)
            {
                file_t* file = xcalloc(1, sizeof(file_t));
                file->path = file_name;
                file->pid = cur_pid;

                struct stat file_stat;
                if (stat(proc_fd_path, &file_stat) == 0)
                    file->file_stat = file_stat;

                array_push(a, file);
            }
        }

        array_destroy(fds);
    }

    array_destroy(pids);

    array_push(a, NULL);

    return (file_t**)array_as_raw(a);
}

utmp_t** get_users_openBSD(void)
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

process_t** get_processes_openBSD(void)
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

char** get_files_openBSD(void)
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
	if ((kf = kvm_getfiles(kd, KERN_FILE_BYFILE, 0, sizeof(*kf), &cnt)) == NULL)
    {
		errx(1, "%s", kvm_geterr(kd));
    }
	if (pledge("stdio rpath getpw", NULL) == -1)
    {
		err(1, "pledge");
    }
	for (kflast = &kf[cnt]; kf < kflast; ++kf) {
		array_push(a, kflast);
	}

    array_push(a, NULL);

    return (char**)array_as_raw(a);
}
