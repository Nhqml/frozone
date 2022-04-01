/* SPDX-License-Identifier: MIT */
/*
 * Copyright (C) 2022 Michel San, Styvell Pidoux
 */

#include <linux/netlink.h>
#include <pwd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <utmpx.h>

#include "resource_com.h"
#include "user_netlink.h"

#define NETLINK_USER 31
#define MAX_PAYLOAD 1024 /* maximum payload size*/
#define MAX_PASSWD 1024 /* maximum password struct size */

struct sockaddr_nl src_addr, dest_addr;
struct nlmsghdr *nlh = NULL;
struct iovec iov;
struct msghdr msg;

/**
** \brief Copy all data in the source even if the data is '\0'
**
** \param dest Destination memory
** \param src Source memory
** \param len Length of the source memory to copy in the destination memory
** \return The destination memory
*/
char* my_exact_copy(char *dest, char*src, size_t len)
{
    if (!dest || !src)
        return NULL;

    for (size_t i = 0; i < len; ++i)
    {
        dest[i] = src[i];
    }

    return dest;
}

/**
** \brief Concatainate all data in the source even if the data is '\0' to the destination
**
** \param dest Destination memory
** \param src Source memory
** \param len_dest Length of the source memory
** \param len_src Length of the destination memory
** \return The new destination memory (different from the one passed in parameters)
*/
char* my_exact_new_cat(char *dest, char*src, size_t len_dest, size_t len_src)
{
    char *new_dest;

    if (!dest || !src)
        return NULL;

    new_dest = malloc(sizeof(char) * (len_dest + len_src));
    if (!new_dest)
        return NULL;

    for (size_t i = 0; i < len_dest; ++i)
    {
        new_dest[i] = dest[i];
    }

    for (size_t i = 0; i < len_src; ++i)
    {
        new_dest[len_dest + i] = src[i];
    }

    return new_dest;
}

/**
** \brief Send message to the kernel through a netlink socket for one uid with a specific socket file descriptor
**
** \param sock_fd Socket file descriptor of the communication with the kernel
** \param resource Resource name to work in the kernel land
** \param uid User uid to work in the kernel land
** \param action Action to do in th kernel land
** \param resource_data Resource data to put to the whitelist in kernel land
** \return bool
*/
bool send_message(int sock_fd, int resource, unsigned int uid, int action, char* resource_data)
{
    int size_resource_data;
    char *buf;
    char *dest;
    char *new_buf;

    // printf("Sending message to kernel\n");

    // allocate netlink message structure
    nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PAYLOAD));
    if (!nlh)
        return false;

    // set netlink message structure
    memset(nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));
    nlh->nlmsg_len = NLMSG_SPACE(MAX_PAYLOAD);
    nlh->nlmsg_pid = getpid();
    nlh->nlmsg_flags = 0;
    nlh->nlmsg_seq = 0;

    size_resource_data = 0;

    // set data struct
    struct netlink_cmd data =
    {
        .resource = resource,
        .uid = uid,
        .action = action
    };

    // add message to the netlink message strcuture
    buf = (char *) &data;

    if (resource_data)
    {
        size_resource_data = strlen(resource_data);
        new_buf = my_exact_new_cat(buf, resource_data, sizeof(struct netlink_cmd), size_resource_data);
        dest = my_exact_copy(NLMSG_DATA(nlh), new_buf, sizeof(struct netlink_cmd) + size_resource_data);

        if (new_buf)
            free(new_buf);
    }
    else
    {
        dest = my_exact_copy(NLMSG_DATA(nlh), buf, sizeof(struct netlink_cmd));
    }

    if (!dest)
        return false;

    // wrap nlh into iov struct to use sendmsg()
    iov.iov_base = (void *)nlh;
    iov.iov_len = nlh->nlmsg_len;
    msg.msg_name = (void *)&dest_addr;
    msg.msg_namelen = sizeof(dest_addr);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    // send the message
    if (sendmsg(sock_fd, &msg, 0) < 0)
        return false;

    return true;
}

/**
** \brief Init the socket to communicate with the kernel land
**
** \return The socket file descriptor
*/
int init_socket()
{
    int sock_fd;

    // printf("Initializing socket\n");

    // create socket
    sock_fd = socket(PF_NETLINK, SOCK_RAW, NETLINK_USER);
    if (sock_fd < 0)
        return -1;

    // set the socket
    memset(&src_addr, 0, sizeof(src_addr));
    src_addr.nl_family = AF_NETLINK;
    src_addr.nl_pid = getpid(); /* self pid */

    // bind the socket
    if (bind(sock_fd, (struct sockaddr *)&src_addr, sizeof(src_addr)) < 0)
        return -1;

    // set the destination address of socket
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.nl_family = AF_NETLINK;
    dest_addr.nl_pid = 0; /* for Linux Kernel */
    dest_addr.nl_groups = 0; /* unicast */

    return sock_fd;
}

/**
** \brief Check the kernel response, if the kernel has received our message
**
** \param sock_fd Socket file descriptor of the communication with the kernel
** \return bool
*/
bool receive_message(int sock_fd)
{
    // printf("Receiving message from kernel\n");

    // receive the message
    if (recvmsg(sock_fd, &msg, 0) < 0)
        return false;

    // printf("Received message payload: %d\n", nlh->nlmsg_type);

    return nlh->nlmsg_type == NLMSG_DONE;
}

/**
** \brief Close the communication between the user land and the kernel alnd with a specify socket
**
** \param sock_fd Socket file descriptor of the communication with the kernel
** \return bool
*/
int exit_socket(int sock_fd)
{
    // printf("Exiting socket\n");

    if (nlh != NULL)
    {
        free(nlh);
    }

    // close socket
    return close(sock_fd);
}

bool send_socket_msg(int resource, unsigned int uid, int action, char* resource_data)
{
    int sock_fd = init_socket();
    if (sock_fd < 0)
    {
        return false;
    }

    if (!send_message(sock_fd, resource, uid, action, resource_data))
    {
        exit_socket(sock_fd);
        return false;
    }

    if (!receive_message(sock_fd))
    {
        exit_socket(sock_fd);
        return false;
    }

    if (!exit_socket(sock_fd))
        return false;

    return true;
}

bool send_socket_msg_except_uid(int resource, unsigned int uid, int action, char* resource_data)
{
    struct passwd password;
    struct passwd *p;
    char *buffer;
    int error;
    struct utmpx* entry;

    // Rewind file ptr
    setutxent();

    entry = getutxent();
    while (entry)
    {
        buffer = malloc(MAX_PASSWD);
        if (!buffer)
            return false;

        error = getpwnam_r(entry->ut_user, &password, buffer, MAX_PASSWD, &p);
        if (error != 0)
            return false;

        if (p != NULL)
        {
            if (p->pw_uid != uid)
            {
                if (!send_socket_msg(resource, p->pw_uid, action, resource_data))
                {
                    endutxent();
                    return false;
                }
            }
        }

        entry = getutxent();
    }

    // Closes UTMP file
    endutxent();

    return true;
}
