/* SPDX-License-Identifier: MIT */
/*
 * Copyright (C) 2022 Styvell Pidoux
 */

#ifndef FREEZER_H
#define FREEZER_H

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
    int resource;   // resource to block
    unsigned int uid;        // user id to block
    int action;    // block or unblock the resource
};

#endif
