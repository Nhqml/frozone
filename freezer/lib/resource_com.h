#ifndef FREEZER_H
#define FREEZER_H

// Define resources to block
#define FILE        1
#define PROCESS     2
#define NETWORK     3
#define SESSIONS    4

// Define lock or unlock resources
#define LOCK        1
#define UNLOCK      2

struct netlink_cmd
{
    int resource;   // resource to block
    unsigned int uid;        // user id to block
    int is_lock;    // block or unblock the resource
};

#endif
