#include <linux/module.h>
#include <linux/netlink.h>
#include <linux/skbuff.h>
#include <net/sock.h>

#include "freezer_hook.h"

#define NETLINK_USER 31
#define NETLINK_LOG  "[freezer][netlink] "

static int __init freezer_init(void)
{
    printk(KERN_INFO NETLINK_LOG "loading module\n");
    return init_freezer_syscalls();
}

static void __exit freezer_exit(void)
{
    printk(KERN_INFO NETLINK_LOG "exiting module\n");
    reset_freezer_syscalls();
}

module_init(freezer_init);
module_exit(freezer_exit);

MODULE_LICENSE("GPL");
