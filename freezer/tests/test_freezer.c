/* SPDX-License-Identifier: MIT */
/*
 * Copyright (C) 2022 Michel San
 */
#include "test_freezer.h"

#include <CUnit/Basic.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>

#include "../lib/freezer.h"

#define PATH_TO_MODULE           "../module/freezer_module.ko"
#define TEST_NAME                "test_user_tmp"
#define TEST_UID                 1042
#define TEST_UID_STR             "1042"
#define TEST_GID                 100
#define TRUE_BOOLEAN             1
#define FALSE_BOOLEAN            0
#define BLOCKED_USER_NAME        "blocked_user_tmp"
#define BLOCKED_USERNAME_UID_STR "1043"
#define BLOCKED_USERNAME_UID     1043

/*
NOTICE:
The test executable must be ran from root account, since it must insmod / rmmod, and useradd/userdel
*/

/**
 * @brief Runs command as a user
 *
 * @param command command to execute
 * @param uid impersonated uid
 * @return int : return 0 if success, else -1
 * @details Can set uid and gid to 0, to run as root
 */
int run_command(char* command, unsigned int uid, unsigned int gid)
{
    int status;
    int res;
    int child = fork();
    if (child == 0)
    {
        if (uid != 0)
        {
            if (setgid(gid) != 0)
                return -1;
            if (setuid(uid) != 0)
                return -1;
        }

        execl("/bin/sh", "sh", "-c", command, (char*)NULL);
    }
    else if (child > 0)
    {
        waitpid(child, &status, WUNTRACED | WCONTINUED);
        res = WIFEXITED(status);
        if (res == TRUE_BOOLEAN)  // command terminated normally
        {
            int exit_code = WEXITSTATUS(status);

            if (exit_code != 0)
            {
                return -1;
            }
            else
            {
                return exit_code;
            }
        }
        else
        {
            return -1;
        }
    }
    else
    {
        return -1;  // fork failed
    }
    return -1;  // to remove end of non-void function warning
}

/* ------------------------------------------------------ */
/* ----- TEST FIXTURES ----- */
/* ------------------------------------------------------ */

int freezer_ts_init(void)
{
    if (system("insmod " PATH_TO_MODULE) != 0)
        return -1;
    if (system("useradd " TEST_NAME " -u " TEST_UID_STR) != 0)
        return -1;
    if (system("echo '" TEST_NAME ":test' | chpasswd") != 0)     // set password for the test account to test
        return -1;
    if (system("useradd " BLOCKED_USER_NAME " -u " BLOCKED_USERNAME_UID_STR) != 0)
        return -1;
    if (system("echo '" BLOCKED_USER_NAME ":test' | chpasswd") != 0)     // set password for the blocked account to test
        return -1;

    return 0;
}

int freezer_ts_clean(void)
{
    if (system("rmmod freezer_module") != 0)
        return -1;
    if (system("userdel " TEST_NAME) != 0)
        return -1;
    if (system("userdel " BLOCKED_USER_NAME) != 0)
        return -1;

    return 0;
}

/* ------------------------------------------------------ */
/* ----- TEST FUNCTIONS ----- */
/* ------------------------------------------------------ */

void test_freeze_connections(void)
{
    char* cmd = "curl gitlab.cri.epita.fr";
    // dummy test to check that new connections work normally
    CU_ASSERT_EQUAL(run_command(cmd, TEST_UID, TEST_GID), 0);

    // freeze the connections
    CU_ASSERT_EQUAL(freeze_connections_uid(TEST_UID), 0);
    CU_ASSERT_EQUAL(run_command(cmd, TEST_UID, TEST_GID), -1);

    // unfreeze the connections
    CU_ASSERT_EQUAL(unfreeze_connections_uid(TEST_UID), 0);
    CU_ASSERT_EQUAL(run_command(cmd, TEST_UID, TEST_GID), 0);
}

void test_freeze_processes(void)
{
    char* cmd = "whoami";
    // dummy test to check that creating new processes work normally
    CU_ASSERT_EQUAL(run_command(cmd, TEST_UID, TEST_GID), 0);

    // freeze processes
    CU_ASSERT_EQUAL(freeze_processes_uid(TEST_UID), 0);
    CU_ASSERT_EQUAL(run_command(cmd, TEST_UID, TEST_GID), -1);

    // unfreeze processes
    CU_ASSERT_EQUAL(unfreeze_processes_uid(TEST_UID), 0);
    CU_ASSERT_EQUAL(run_command(cmd, TEST_UID, TEST_GID), 0);
}

// Tests that a blocked user cannot open files
void test_freeze_open_files(void)
{
    char* cmd = "cat /etc/passwd";

    CU_ASSERT_EQUAL(run_command(cmd, TEST_UID, TEST_GID), 0);

    CU_ASSERT_EQUAL(freeze_files_uid(TEST_UID), 0);
    CU_ASSERT_EQUAL(run_command(cmd, TEST_UID, TEST_GID), -1);

    CU_ASSERT_EQUAL(unfreeze_files_uid(TEST_UID), 0);
    CU_ASSERT_EQUAL(run_command(cmd, TEST_UID, TEST_GID), 0);
}

// Tests that a blocked user cannot login to another user (lateral movement)
void test_freeze_open_to_blocked_user(void)
{
    char* cmd = "echo test | su " TEST_NAME " -c echo true";

    // dummy test to check that we can connect from blocked user to the test account
    CU_ASSERT_EQUAL(run_command(cmd, BLOCKED_USERNAME_UID, TEST_GID), 0);

    // block the user
    CU_ASSERT_EQUAL(freeze_users_uid(BLOCKED_USERNAME_UID), 0);
    // test that we cannot connect from the blocked user to the test account
    CU_ASSERT_EQUAL(run_command(cmd, BLOCKED_USERNAME_UID, TEST_GID), -1);

    // // unblock the user
    CU_ASSERT_EQUAL(unfreeze_users_uid(BLOCKED_USERNAME_UID), 0);
    CU_ASSERT_EQUAL(run_command(cmd, BLOCKED_USERNAME_UID, TEST_GID), 0);
}

void test_freeze_whitelist_connections(void)
{
    char* cmd_ok = "curl 1.1.1.1";
    char* cmd_nok = "curl 1.0.0.1";
    // dummy test to check that new connections work normally
    CU_ASSERT_EQUAL(run_command(cmd_ok, TEST_UID, TEST_GID), 0);

    // whitelist IP
    CU_ASSERT_EQUAL(add_connection_whitelist(TEST_UID, "1.1.1.1"), 0);
    CU_ASSERT_EQUAL(add_connection_whitelist(TEST_UID, "127.0.0.53"), 0);    // systemd-resolve on ubuntu
    CU_ASSERT_EQUAL(add_connection_whitelist(101, "4.2.2.1"), 0);

    // freeze the connections
    CU_ASSERT_EQUAL(freeze_connections_uid(TEST_UID), 0);
    CU_ASSERT_EQUAL(run_command(cmd_ok, TEST_UID, TEST_GID), 0);        // 1.1.1.1 should work
    CU_ASSERT_EQUAL(run_command(cmd_nok, TEST_UID, TEST_GID), -1);      // 1.0.0.1 should not

    // unfreeze the connections
    CU_ASSERT_EQUAL(unfreeze_connections_uid(TEST_UID), 0);
    CU_ASSERT_EQUAL(run_command(cmd_ok, TEST_UID, TEST_GID), 0);        // 1.1.1.1 should work
    CU_ASSERT_EQUAL(run_command(cmd_nok, TEST_UID, TEST_GID), 0);       // 1.0.0.1 should work too
}

void test_freeze_whitelist_processes(void)
{
    char* cmd_ok = "/usr/bin/ls";

    // dummy tests to test that both commands work normally
    CU_ASSERT_EQUAL(run_command(cmd_ok, TEST_UID, TEST_GID), 0);

    // freeze processes
    CU_ASSERT_EQUAL(freeze_processes_uid(TEST_UID), 0);

    CU_ASSERT_EQUAL(run_command(cmd_ok, TEST_UID, TEST_GID), -1);       // ls should not work anymore

    // whitelist ls binary
    CU_ASSERT_EQUAL(add_process_whitelist(TEST_UID, cmd_ok), 0);

    CU_ASSERT_EQUAL(run_command(cmd_ok, TEST_UID, TEST_GID), 0);        // ls should again since it has been whitelisted

    // unfreeze the connections
    CU_ASSERT_EQUAL(unfreeze_processes_uid(TEST_UID), 0);
    CU_ASSERT_EQUAL(run_command(cmd_ok, TEST_UID, TEST_GID), 0);        // ls should work
}
