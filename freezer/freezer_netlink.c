#include <linux/module.h>
#include <linux/netlink.h>
#include <linux/skbuff.h>
#include <net/sock.h>

#include "freezer_hook.h"
#include "resource_com.h"

#define NETLINK_USER 31
#define NETLINK_LOG  "[freezer][netlink] "

struct sock* nl_sk = NULL;

static void freezer_recv_msg(struct sk_buff* skb)
{
    struct nlmsghdr* nlh;
    struct sk_buff* skb_out;
    struct netlink_cmd* data;
    int pid = 0;
    int res = 0;
    int freezer_wrapper_res = 0;

    printk(KERN_INFO NETLINK_LOG "entering: %s\n", __FUNCTION__);

    nlh = (struct nlmsghdr*)skb->data;
    data = (struct netlink_cmd*)nlmsg_data(nlh);
    printk(KERN_INFO NETLINK_LOG "resource: %d\n", data->resource);
    printk(KERN_INFO NETLINK_LOG "uid: %d\n", data->uid);
    printk(KERN_INFO NETLINK_LOG "action: %d\n", data->action);
    printk(KERN_INFO NETLINK_LOG "resource_data: %s\n", data->resource_data);
    
    pid = nlh->nlmsg_pid; /*pid of sending process */

    // process the payload
    freezer_wrapper_res = freezer_call_wrapper(data) == 0 ? NLMSG_DONE : NLMSG_ERROR;

    // send response back
    skb_out = nlmsg_new(sizeof(int), 0);
    if (!skb_out)
    {
        printk(KERN_ERR NETLINK_LOG "failed to allocate new skb\n");
        return;
    }

    // skb, portid, seq, type, payload, flags
    // put the empty message (with response in the msg.type) on socket buffer
    nlh = nlmsg_put(skb_out, 0, 0, freezer_wrapper_res, 0, 0);
    NETLINK_CB(skb_out).dst_group = 0; // not in mcast group

    // send the message
    res = nlmsg_unicast(nl_sk, skb_out, pid);

    if (res < 0)
        printk(KERN_INFO NETLINK_LOG "error while sending back to user\n");
}

static int __init freezer_init(void)
{
    struct netlink_kernel_cfg cfg = { .input = freezer_recv_msg };

    printk(KERN_INFO NETLINK_LOG "loading freezer module");

    nl_sk = netlink_kernel_create(&init_net, NETLINK_USER, &cfg);
    if (!nl_sk)
    {
        printk(KERN_ALERT "error creating socket\n");
        return -10;
    }

    return init_freezer_syscalls();
}

static void __exit freezer_exit(void)
{
    printk(KERN_INFO "exiting freezer module\n");
    reset_freezer_syscalls();
    netlink_kernel_release(nl_sk);
}

module_init(freezer_init);
module_exit(freezer_exit);

MODULE_LICENSE("GPL");
