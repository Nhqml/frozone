/* SPDX-License-Identifier: MIT */
/*
 * Copyright (C) 2022 Styvell Pidoux
 */

#include <stdbool.h>
#include <stdio.h>

#include "freezer.h"
#include "resource_com.h"
#include "user_netlink.h"

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

int add_connection_whitelist(unsigned int uid, char *ipaddr)
{
    return send_socket_msg(NETWORK, uid, WHITELIST, ipaddr);
}

int add_connection_whitelist_except_uid(unsigned int uid, char *ipaddr)
{
    return send_socket_msg_except_uid(NETWORK, uid, WHITELIST, ipaddr);
}

int add_file_whitelist(unsigned int uid, char *file_path)
{
    return send_socket_msg(FILE, uid, WHITELIST, file_path);
}

int add_file_whitelist_except_uid(unsigned int uid, char *file_path)
{
    return send_socket_msg_except_uid(FILE, uid, WHITELIST, file_path);
}

int add_process_whitelist(unsigned int uid, char *process_name)
{
    return send_socket_msg(PROCESS, uid, WHITELIST, process_name);
}

int add_process_whitelist_except_uid(unsigned int uid, char *process_name)
{
    return send_socket_msg_except_uid(PROCESS, uid, WHITELIST, process_name);
}
