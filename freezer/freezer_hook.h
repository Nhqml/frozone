/* SPDX-License-Identifier: MIT */
/*
 * Copyright (C) 2022 Kenji Gaillac, Michel San, Styvell Pidoux
 */

#ifndef FREEZER_HOOK_H
#define FREEZER_HOOK_H

#include "resource_com.h"

/**
 * freezer_call_wrapper - Wrapper for calls made in userland to freeze a resource for a user
 * @data: netlink command data set in userland
 */
int freezer_call_wrapper(struct netlink_cmd *data);

/**
 * init_freezer_syscalls - Initialize the syscall table with our hooked syscalls
 */
int init_freezer_syscalls(void);

/**
 * reset_freezer_syscalls - Reset the syscall table with the original syscalls
 */
void reset_freezer_syscalls(void);

#endif
