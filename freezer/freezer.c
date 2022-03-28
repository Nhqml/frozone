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

int freeze_users_uid(unsigned int uid)
{
    return send_socket_msg(SESSIONS, uid, LOCK, NULL);
}

int freeze_processes_uid(unsigned int uid)
{
    return send_socket_msg(PROCESS, uid, LOCK, NULL);
}

int freeze_connections_uid(unsigned int uid)
{
    return send_socket_msg(NETWORK, uid, LOCK, NULL);
}

int freeze_files_uid(unsigned int uid)
{
    return send_socket_msg(FILE, uid, LOCK, NULL);
}

int unfreeze_users_uid(unsigned int uid)
{
    return send_socket_msg(SESSIONS, uid, UNLOCK, NULL);
}

int unfreeze_processes_uid(unsigned int uid)
{
    return send_socket_msg(PROCESS, uid, UNLOCK, NULL);
}

int unfreeze_connections_uid(unsigned int uid)
{
    return send_socket_msg(NETWORK, uid, UNLOCK, NULL);
}

int unfreeze_files_uid(unsigned int uid)
{
    return send_socket_msg(FILE, uid, UNLOCK, NULL);
}

int freeze_users_except_uid(unsigned int uid)
{
    return send_socket_msg_except_uid(SESSIONS, uid, LOCK, NULL);
}

int freeze_processes_except_uid(unsigned int uid)
{
    return send_socket_msg_except_uid(PROCESS, uid, LOCK, NULL);
}

int freeze_connections_except_uid(unsigned int uid)
{
    return send_socket_msg_except_uid(NETWORK, uid, LOCK, NULL);
}

int freeze_files_except_uid(unsigned int uid)
{
    return send_socket_msg_except_uid(FILE, uid, LOCK, NULL);
}

int unfreeze_users_except_uid(unsigned int uid)
{
    return send_socket_msg_except_uid(SESSIONS, uid, UNLOCK, NULL);
}

int unfreeze_processes_except_uid(unsigned int uid)
{
    return send_socket_msg_except_uid(PROCESS, uid, UNLOCK, NULL);
}

int unfreeze_connections_except_uid(unsigned int uid)
{
    return send_socket_msg_except_uid(NETWORK, uid, UNLOCK, NULL);
}

int unfreeze_files_except_uid(unsigned int uid)
{
    return send_socket_msg_except_uid(FILE, uid, UNLOCK, NULL);
}
