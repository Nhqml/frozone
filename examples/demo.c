#include <carto.h>
#include <stdio.h>
#include <stdlib.h>

void strmode(mode_t mode, char* buf)
{
    static const char chars[] = "rwxrwxrwx";

    for (size_t i = 0; i < 9; i++)
    {
        buf[i] = (mode & (1 << (8 - i))) ? chars[i] : '-';
    }
    buf[9] = '\0';
}

int main(int argc, char* argv[])
{
    FILE* f = NULL;
    if (argc == 2)
        f = fopen(argv[1], "w");

    if (f == NULL)
        f = stdout;

    char buf[30];

    fputs("============================ USERS ==============================\n", f);
    fprintf(f, "%-15s%-10s%-30s%-10s\n", "Name", "Device", "Host", "PID");

    utmp_t** users = get_users();
    for (utmp_t** user = users; *user != NULL; ++user)
    {
        fprintf(f, "%-15s", (*user)->ut_user);
        fprintf(f, "%-10s", (*user)->ut_line);
        fprintf(f, "%-30s", (*user)->ut_host);
        fprintf(f, "%-10d\n", (*user)->ut_pid);
        free(*user);
    }
    free(users);

    fputc('\n', f);

    fputs("============================ FILES ==============================\n", f);
    fprintf(f, "%-10s%-30s%-10s%-7s%-7s\n", "PID", "Path", "Mode", "UID", "GID");
    file_t** files = get_files();
    for (file_t** file = files; *file != NULL; ++file)
    {
        fprintf(f, "%-10d", (*file)->pid);
        snprintf(buf, 30, "%.29s", (*file)->path);
        fprintf(f, "%-30s ", buf);
        strmode((*file)->file_stat.st_mode, buf);
        fprintf(f, "%-10s", buf);
        fprintf(f, "%-7d", (*file)->file_stat.st_uid);
        fprintf(f, "%-7d\n", (*file)->file_stat.st_gid);

        free((*file)->path);
        free(*file);
    }
    free(files);

    fputc('\n', f);

    fputs("================================== PROCESSES ========================================\n", f);
    fprintf(f, "%-10s%-30s%-30s%-7s%-7s\n", "PID", "Path", "Directory", "UID", "GID");
    process_t** processes = get_processes();
    for (process_t** process = processes; *process != NULL; ++process)
    {
        fprintf(f, "%-10d", (*process)->pid);
        snprintf(buf, 30, "%.29s", (*process)->exe_path);
        fprintf(f, "%-30s ", buf);
        snprintf(buf, 30, "%.29s", (*process)->cwd);
        fprintf(f, "%-30s", buf);
        fprintf(f, "%-7d", (*process)->uid);
        fprintf(f, "%-7d\n", (*process)->gid);

        free((*process)->exe_path);
        free((*process)->cmdline);
        free((*process)->cwd);
        free((*process)->root);
        free(*process);
    }
    free(processes);

    return 0;
}
