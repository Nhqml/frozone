#ifndef FREEZER_H
#define FREEZER_H

// Define resources to block
#define FILE        1
#define PROCESS     2
#define NETWORK     3
#define SESSIONS    4

struct netlink_cmd
{
    int resource;   // resource to block
    int uid;        // user id to block
};

#endif
