/* SPDX-License-Identifier: MIT */
/*
 * Copyright (C) 2022 Michel San
 */
#include "freezer_hook_utils.h"
#include <linux/uaccess.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/string.h>

char *safe_copy_from_user(const char __user *src, unsigned int max_size)
{
    int res = 0;
    int copied_len = 0;
    char *dest = NULL;

    if (src == NULL)
        return NULL;

    dest = kzalloc(max_size * sizeof(char), GFP_KERNEL);
    if (dest == NULL)
        return NULL;

    copied_len = strncpy_from_user(dest, src, max_size * sizeof(char));
    if (copied_len <= 0)
        res = -1;

    if (res != -1)
    {
        if (copied_len == max_size - 1)  // strncpy_from_user returns length EXCLUDING trailing NULL byte
        {
            // user string is not NULL terminated, so we have to do it
            dest[copied_len] = '\0';
        }
        else
        {
            // user string is already NULL terminated, shrinking allocated memory
            dest = krealloc(dest, copied_len + 1, GFP_KERNEL);
            if (!dest)
                res = -1;
        }
    }

    // cleaning
    if (res == 0)
    {
        return dest;
    }
    else
    {
        kfree(dest);
        return NULL;
    }
}