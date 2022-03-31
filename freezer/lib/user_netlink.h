/* SPDX-License-Identifier: MIT */
/*
 * Copyright (C) 2022 Styvell Pidoux
 */

#ifndef FREEZER_LIB_USER_NETLINK_H_
#define FREEZER_LIB_USER_NETLINK_H_

/**
** \brief Send message to the kernel through a netlink socket for one uid
**
** \param resource Resource name to work in the kernel land
** \param uid User uid to work in the kernel land
** \param action Action to do in th kernel land
** \param resource_data Resource data to put to the whitelist in kernel land
** \return bool
*/
bool send_socket_msg(int resource, unsigned int uid, int action, char* resource_data);

/**
** \brief Send message to the kernel through a netlink socket for every uid except one
**
** \param resource Resource name to work in the kernel land
** \param uid User uid to NOT work in the kernel land
** \param action Action to do in th kernel land
** \param resource_data Resource data to put to the whitelist in kernel land
** \return bool
*/
bool send_socket_msg_except_uid(int resource, unsigned int uid, int action, char* resource_data);

#endif  //  FREEZER_LIB_USER_NETLINK_H_
