#include <netlink.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <utmpx.h>
#include <pwd.h>
#include "resource_com.h"

#define NETLINK_USER 31
#define MAX_PAYLOAD 1024 /* maximum payload size*/

struct sockaddr_nl src_addr, dest_addr;
struct nlmsghdr *nlh = NULL;
struct iovec iov;
struct msghdr msg;

// copy all the src len in the dest even if there is '\0'
char* my_exact_copy(char *dest, char*src, size_t len)
{
    for (size_t i = 0; i < len; ++i)
    {
        dest[i] = src[i];
    }

    return dest;
}

int send_message(int sock_fd, int resource, unsigned int uid, int is_lock)
{
    printf("Sending message to kernel\n");

    // allocate netlink message structure
    nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PAYLOAD));
    if (nlh == NULL)
    {
        return -1;
    }

    // set netlink message structure
    memset(nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));
    nlh->nlmsg_len = NLMSG_SPACE(MAX_PAYLOAD);
    nlh->nlmsg_pid = getpid();
    nlh->nlmsg_flags = 0;
    nlh->nlmsg_seq = 0;

    // set data struct
    struct netlink_cmd data =
    {
        .resource = resource,
        .uid = uid,
        .is_lock = is_lock
    };

    // add message to the netlink message strcuture
    char *buf = (char *) &data;
    char *dest = my_exact_copy(NLMSG_DATA(nlh), buf, sizeof(struct netlink_cmd));
    if (dest == NULL)
    {
        return -1;
    }

    // wrap nlh into iov struct to use sendmsg()
    iov.iov_base = (void *)nlh;
    iov.iov_len = nlh->nlmsg_len;
    msg.msg_name = (void *)&dest_addr;
    msg.msg_namelen = sizeof(dest_addr);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    // send the message
    int res_msg = sendmsg(sock_fd, &msg, 0);
    if (res_msg < 0)
    {
        return -1;
    }

    //free(nlh);
    return res_msg;
}

int init_socket()
{
    printf("Initializing socket\n");

    // create socket
    int sock_fd = socket(PF_NETLINK, SOCK_RAW, NETLINK_USER);
    if (sock_fd < 0)
        return -1;

    // set the socket
    memset(&src_addr, 0, sizeof(src_addr));
    src_addr.nl_family = AF_NETLINK;
    src_addr.nl_pid = getpid(); /* self pid */

    // bind the socket
    int res_bind = bind(sock_fd, (struct sockaddr *)&src_addr, sizeof(src_addr));
    if (res_bind < 0)
    {
        return -1;
    }

    // set the destination address of socket
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.nl_family = AF_NETLINK;
    dest_addr.nl_pid = 0; /* For Linux Kernel */
    dest_addr.nl_groups = 0; /* unicast */

    return sock_fd;
}

int receive_message(int sock_fd)
{
    printf("Receiving message from kernel\n");

    // receive the message
    ssize_t res_msg = recvmsg(sock_fd, &msg, 0);
    if (res_msg < 0)
    {
        return -1;
    }

    printf("Received message payload: %d\n", nlh->nlmsg_type);

    if (nlh->nlmsg_type == NLMSG_DONE)
    {
        return res_msg;
    }
    else
    {
        return -1;
    }
}

int exit_socket(int sock_fd)
{
    printf("Exiting socket\n");
    if (nlh != NULL)
    {
        free(nlh);
    }

    // close socket
    return close(sock_fd);
}

int send_socket_msg(int resource, unsigned int uid, int is_lock)
{
    int sock_fd = init_socket();
    if (sock_fd < 0)
    {
        return -1;
    }

    int bytes_send = send_message(sock_fd, resource, uid, is_lock);
    if (bytes_send < 0)
    {
        exit_socket(sock_fd);
        return -1;
    }

    int bytes_received = receive_message(sock_fd);
    if (bytes_received < 0)
    {
        exit_socket(sock_fd);
        return -1;
    }

    return exit_socket(sock_fd);
}

int send_socket_msg_except_uid(int resource, unsigned int uid, int is_lock)
{
    struct passwd *p;
    struct utmpx* entry;
    int res = 0;

    // Rewind file ptr
    setutxent();

    entry = getutxent();
    while (entry != NULL)
    {
        p = getpwnam(entry->ut_user);

        if (p != NULL)
        {
            printf("For the name %s, the uid is %d\n", entry->ut_user, p->pw_uid);

            if (p->pw_uid != uid)
            {
                res = send_socket_msg(resource, p->pw_uid, is_lock);
                if (res < 0)
                {
                    endutxent();
                    return -1;
                }
            }
        }

        entry = getutxent();
    }

    // Closes UTMP file
    endutxent();
    return 1;
}

