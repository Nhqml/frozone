/* SPDX-License-Identifier: MIT */
/*
 * Copyright (C) 2022 Michel San, Styvell Pidoux
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

/*  ---------------------------------------------------- */
/*  ---------- WHITELIST FUNCTIONS ---------- */
/*  ---------------------------------------------------- */

int add_to_whitelist(struct array_uid **array, int *index, char *resource_data, unsigned int uid)
{
    int cur = 0;
    struct array_uid *new_array_uid = NULL;

    if (*index >= MAX_SIZE_ARRAY)
        return 0;

    while (cur < *index)
    {
        if (array[cur]->uid == uid)
        {
            array_push(array[cur]->array, resource_data);
            return 1;
        }

        cur++;
    }

    new_array_uid = kzalloc(sizeof(struct array_uid), GFP_KERNEL);
    if (!new_array_uid)
        return -1;

    new_array_uid->uid = uid;
    new_array_uid->array = array_new();

    array_push(new_array_uid->array, resource_data);

    array[*index] = new_array_uid;
    *index = *index + 1;

    return 1;
}

void print_whitelist(struct array_uid **array, int *index)
{
    int i = 0;
    int j = 0;
    for (; i < *index; i++)
    {
        printk(KERN_INFO "[debug][print_whitelist][id=%d][uid=%d]\n", i, array[i]->uid);

        for (; j < array[i]->array->size; j++)
        {
            printk(KERN_INFO "  [debug][print_whitelist][id:%d] %s\n", j, (char *)array[i]->array->array[j]);
        }
    }
}

void whitelist_dispose(struct array_uid **array, int *index)
{
    int cur = 0;
    while (cur < *index)
    {
        array_destroy(array[cur]->array);

        cur++;
    }
}

int resource_data_is_in_whitelist(struct array_uid **array_uids, int *index, char *resource_data, int orig_uid)
{
    int cur = 0;

    while (cur < *index)
    {
        if (array_uids[cur]->uid == orig_uid)
        {
            unsigned int i = 0;

            for (; i < array_uids[cur]->array->size; ++i)
            {
                if (strncmp((char*)array_uids[cur]->array->array[i], resource_data, strlen(resource_data)) == 0)
                {
                    return 1;
                }
            }

            return 0;
        }

        cur++;
    }
    return 0;
}

/*  ---------------------------------------------------- */
/*  ---------- ARRAY FUNCTIONS ---------- */
/*  ---------------------------------------------------- */

int uid_is_in_array(int *array, int uid)
{
    int i = 0;
    for (; i < MAX_SIZE_ARRAY; i++)
    {
        if (uid == array[i])
            return 0;
    }
    return -1;
}

int add_uid_to_array(int* array, int *index, unsigned int uid)
{
    int cur = 0;
    if (*index >= MAX_SIZE_ARRAY)
        return 0;

    while (cur < *index)
    {
        if (array[cur] == uid)
            return 0;

        cur++;
    }

    // uid not already in array, so add uid
    array[*index] = uid;
    *index = *index + 1;

    return 1;
}

int remove_uid_from_array(int *array, int *index, unsigned int uid)
{
    int cur = 0;
    int is_rm_index = 0;  // false

    if (*index >= MAX_SIZE_ARRAY)
        return 0;

    while (cur < *index)
    {
        if (array[cur] == uid)
        {
            is_rm_index = 1;
            break;
        }

        cur++;
    }

    if (is_rm_index == 0)
        return 0;

    (*index)--;

    while (cur < *index)
    {
        array[cur] = array[cur + 1];
        cur++;
    }

    return 1;
}
