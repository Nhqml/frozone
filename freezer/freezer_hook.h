#ifndef FREEZER_HOOK_H
#define FREEZER_HOOK_H

#include "resource_com.h"

/**
 * freezer_call_wrapper - Wrapper for calls made in userland to freeze a resource for a user
 * @data: netlink command data set in userland
 * @resource_data: resource data set in userland
 */
int freezer_call_wrapper(struct netlink_cmd *data, char* resource_data);

/**
 * init_freezer_syscalls - Initialize the syscall table with our hooked syscalls
 */
int init_freezer_syscalls(void);

/**
 * reset_freezer_syscalls - Reset the syscall table with the original syscalls
 */
void reset_freezer_syscalls(void);

#endif
