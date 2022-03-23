#include <stdbool.h>
#include <stdio.h>

#include "user_netlink.h"
#include "resource_com.h"

int freeze_users(void)
{
    return 0;
}

int freeze_processes(void)
{
    return 0;
}

int freeze_connections(void)
{
    return 0;
}

int freeze_files(void)
{
    return 0;
}

int freeze_users_uid(int uid)
{
    return send_socket_msg(SESSIONS, uid);
}

int freeze_processes_uid(int uid)
{
    return send_socket_msg(PROCESS, uid);
}

int freeze_connections_uid(int uid)
{
    return send_socket_msg(NETWORK, uid);
}

int freeze_files_uid(int uid)
{
    return send_socket_msg(FILE, uid);
}
