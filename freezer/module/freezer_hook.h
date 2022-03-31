/* SPDX-License-Identifier: MIT */
/*
 * Copyright (C) 2022 Michel San, Styvell Pidoux
 */

#ifndef FREEZER_MODULE_FREEZER_HOOK_H_
#define FREEZER_MODULE_FREEZER_HOOK_H_

#include "../lib/resource_com.h"

/**
** \brief Wrapper for calls made in userland to act on a resource for a user
**
** \param data Dta from netlink set in userland
** \param resource_data Resource data to put in the whitelist
** \return bool
*/
int freezer_call_wrapper(struct netlink_cmd *data, char* resource_data);

/**
** \brief Initialize the syscall table with our hooked syscalls
**
** \return bool
*/
int init_freezer_syscalls(void);

/**
** \brief Reset the syscall table with the original syscalls
**
** \return bool
*/
void reset_freezer_syscalls(void);

#endif  //  FREEZER_MODULE_FREEZER_HOOK_H_
