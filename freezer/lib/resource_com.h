/* SPDX-License-Identifier: MIT */
/*
 * Copyright (C) 2022 Styvell Pidoux
 */

#ifndef FREEZER_LIB_RESOURCE_COM_H_
#define FREEZER_LIB_RESOURCE_COM_H_

// Define resources to block
#define FILE        1
#define PROCESS     2
#define NETWORK     3
#define SESSIONS    4

// Define action for resources
#define LOCK        1
#define UNLOCK      2
#define WHITELIST   3

struct netlink_cmd
{
    int resource;       // resource name to work in
    unsigned int uid;   // user id to work in
    int action;         // block or unblock or put in whitelist the resource
};

#endif  //  FREEZER_LIB_RESOURCE_COM_H_
