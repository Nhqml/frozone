#ifndef FREEZER_USER_NETLINK_H
#define FREEZER_USER_NETLINK_H

int send_socket_msg(int resource, unsigned int uid, int action, char *resource_data);
int send_socket_msg_except_uid(int resource, unsigned int uid, int action, char *resource_data);

#endif