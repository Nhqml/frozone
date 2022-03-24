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
    return send_socket_msg(SESSIONS, uid, LOCK);
}

int freeze_processes_uid(int uid)
{
    return send_socket_msg(PROCESS, uid, LOCK);
}

int freeze_connections_uid(int uid)
{
    return send_socket_msg(NETWORK, uid, LOCK);
}

int freeze_files_uid(int uid)
{
    return send_socket_msg(FILE, uid, LOCK);
}

int unfreeze_users_uid(int uid)
{
    return send_socket_msg(SESSIONS, uid, UNLOCK);
}

int unfreeze_processes_uid(int uid)
{
    return send_socket_msg(PROCESS, uid, UNLOCK);
}

int unfreeze_connections_uid(int uid)
{
    return send_socket_msg(NETWORK, uid, UNLOCK);
}

int unfreeze_files_uid(int uid)
{
    return send_socket_msg(FILE, uid, UNLOCK);
}
