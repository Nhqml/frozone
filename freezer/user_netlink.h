#ifndef FREEZER_USER_NETLINK_H
#define FREEZER_USER_NETLINK_H

int send_socket_msg(int resource, int uid, int is_lock);

#endif