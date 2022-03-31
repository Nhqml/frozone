/* SPDX-License-Identifier: MIT */
/*
 * Copyright (C) 2022 Styvell Pidoux
 */

#ifndef FREEZER_LIB_USER_NETLINK_H_
#define FREEZER_LIB_USER_NETLINK_H_

int send_socket_msg(int resource, unsigned int uid, int action, char* resource_data);
int send_socket_msg_except_uid(int resource, unsigned int uid, int action, char* resource_data);

#endif  //  FREEZER_LIB_USER_NETLINK_H_