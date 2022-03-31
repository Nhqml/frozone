#include <stdio.h>
#include <string.h>
#include "lib/freezer.h"
#define UID_TEST    1001    // edit this with an existing user id

/**
 * A very simple snippet in C showing the usage of the static lib freezer.c
 * - UID_TEST must be an existing user id of a valid user
 * - the module 'freezer_module.ko' must be loaded first
 */


void print_usage()
{
    puts("Usage: ./freezer_demo <action> <resource>");
    puts("<action>: lock | unlock | whitelist");
    puts("<resource>: proc | con | files | users");
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        print_usage();
        return -1;
    }

    if (strcmp("lock", argv[1]) == 0)
    {
        if (strcmp("proc", argv[2]) == 0)
            freeze_processes_uid(UID_TEST);
        else if (strcmp("con", argv[2]) == 0)
        {
            freeze_connections_uid(UID_TEST);
            add_connection_whitelist(UID_TEST, "91.243.117.180");
            add_connection_whitelist(UID_TEST, "127.0.0.53");
            add_connection_whitelist(101, "4.2.2.1");      // google DNS ?
        }
        else if (strcmp("files", argv[2]) == 0)
        {
            freeze_files_uid(UID_TEST);
        }
        else if (strcmp("users", argv[2]) == 0)
            freeze_users_uid(UID_TEST);
        else
        {
            print_usage();
            return -1;
        }
    }
    else if (strcmp("unlock", argv[1]) == 0)
    {
        if (strcmp("proc", argv[2]) == 0)
            unfreeze_processes_uid(UID_TEST);
        else if (strcmp("con", argv[2]) == 0)
            unfreeze_connections_uid(UID_TEST);
        else if (strcmp("files", argv[2]) == 0)
            unfreeze_files_uid(UID_TEST);
        else if (strcmp("users", argv[2]) == 0)
            unfreeze_users_uid(UID_TEST);
        else
        {
            print_usage();
            return -1;
        }
    }
    else if (strcmp("whitelist", argv[1]) == 0)
    {
        if (argc != 4)
        {
            print_usage();
            return -1;
        }

        if (strcmp("proc", argv[2]) == 0)
            add_process_whitelist(UID_TEST, argv[3]);
        else if (strcmp("con", argv[2]) == 0)
            add_connection_whitelist(UID_TEST, argv[3]);
        else if (strcmp("files", argv[2]) == 0)
            add_file_whitelist(UID_TEST, argv[3]);
        else
        {
            print_usage();
            return -1;
        }
    }

    return 0;
}