/* SPDX-License-Identifier: MIT */
/*
 * Copyright (C) 2022 Styvell Pidoux
 */

#ifndef FREEZER_USER_NETLINK_H
#define FREEZER_USER_NETLINK_H

int send_socket_msg(int resource, unsigned int uid, int is_lock);
int send_socket_msg_except_uid(int resource, unsigned int uid, int is_lock);

#endif