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
    int final_len = 0;
    char *tmp_dest = NULL;
    char *dest = NULL;

    if (src == NULL)
    {
        return NULL;
    }

    tmp_dest = kzalloc(max_size * sizeof(char), GFP_KERNEL);
    if (tmp_dest == NULL)
    {
        return NULL;
    }

    if (strncpy_from_user(tmp_dest, src, sizeof(tmp_dest) * sizeof(char)) <= 0)
    {
        res = -1;
    }

    if (res != -1)
    {
        int string_len = strnlen(tmp_dest, sizeof(tmp_dest));
        if (string_len == sizeof(tmp_dest) && tmp_dest[string_len] != '\0')
        {
            // user string is not NUL terminated, so we have to do it
            final_len = string_len + 1;
            dest = krealloc(dest, final_len, GFP_KERNEL);
            if (dest != NULL)
            {
                dest[final_len] = '\0';  // NUL terminate the string
            }
            else
            {
                res = -1;
            }
        }
        else
        {
            // user string is already NUL terminated
            final_len = string_len;
        }
    }

    if (res != -1)
    {
        dest = kzalloc(final_len, GFP_KERNEL);
        if (dest != NULL)
        {
            if (strncpy(dest, tmp_dest, sizeof(dest) * sizeof(char)) == NULL)
            {
                res = -1;
            }
        }
        else
        {
            res = -1;
        }
    }

    // cleaning
    kfree(tmp_dest);

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