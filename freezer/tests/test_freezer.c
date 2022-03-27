#include "test_freezer.h"

#include <CUnit/Basic.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../lib/freezer.h"

#define PATH_TO_MODULE "../module/freezer_module.ko"
#define TEST_NAME "test_user_tmp"
#define TEST_UID  1042
#define TEST_UID_STR "1042"
#define TEST_GID  100
#define TRUE_BOOLEAN    1
#define FALSE_BOOLEAN   0

/*
NOTICE:
The test executable must be ran from root account, since it must insmod / rmmod, and useradd/userdel
*/

/**
 * @brief Runs command as test user
 *
 * @param command command to execute
 * @return int : return 0 if success, else -1
 */
int run_command(char* command)
{
    int status;
    int res;
    int child = fork();
    if (child == 0)
    {
        setgid(TEST_GID);
        setuid(TEST_UID);

        execl("/bin/sh", "sh", "-c", command, (char *) NULL);
    }
    else if (child > 0)
    {
        waitpid(child, &status, WUNTRACED | WCONTINUED);
        res = WIFEXITED(status);
        if (res == TRUE_BOOLEAN) // command terminated normally
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
        return -1; // fork failed
    }
}

int freezer_ts_init(void)
{
    system("insmod " PATH_TO_MODULE);
    system("useradd " TEST_NAME " -u " TEST_UID_STR);
    return 0;
}

int freezer_ts_clean(void)
{
    system("rmmod freezer_module");
    system("userdel " TEST_NAME);
    return 0;
}

void test_freeze_connections(void)
{
    char* cmd = "curl gitlab.cri.epita.fr";
    //dummy test to check that new connections work normally
    CU_ASSERT_EQUAL(run_command(cmd), 0);

    // freeze the connections
    CU_ASSERT_EQUAL(freeze_connections_uid(TEST_UID), 0);
    CU_ASSERT_EQUAL(run_command(cmd), -1);

    // unfreeze the connections
    CU_ASSERT_EQUAL(unfreeze_connections_uid(TEST_UID), 0);
    CU_ASSERT_EQUAL(run_command(cmd), 0);
}

void test_freeze_processes(void)
{
    char* cmd = "whoami";
    // dummy test to check that creating new processes work normally
    CU_ASSERT_EQUAL(run_command(cmd), 0);

    CU_ASSERT_EQUAL(freeze_processes_uid(TEST_UID), 0);
    CU_ASSERT_EQUAL(run_command(cmd), -1);

    CU_ASSERT_EQUAL(unfreeze_processes_uid(TEST_UID), 0);
    CU_ASSERT_EQUAL(run_command(cmd), 0);
}