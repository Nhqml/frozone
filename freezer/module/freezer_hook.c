/* SPDX-License-Identifier: MIT */
/*
 * Copyright (C) 2022 Michel San, Styvell Pidoux
 */

#include "freezer_hook.h"
#include "array.h"

#include <asm/ptrace.h>
#include <linux/errno.h>
#include <linux/in.h>
#include <linux/inet.h>
#include <linux/kallsyms.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/net.h>
#include <linux/socket.h>
#include <linux/string.h>
#include <linux/version.h>


/* The way we access "sys_call_table" varies as kernel internal changes.
 * Source: https://sysprog21.github.io/lkmpg/#sysfs-interacting-with-your-module
 * - Prior to v5.4 : manual symbol lookup
 * - v5.5 to v5.6  : use kallsyms_lookup_name()
 * - v5.7+         : Kprobes or specific kernel module parameter
 */

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 7, 0)
#define KPROBE_LOOKUP   1
#include <linux/kprobes.h>
#endif

#define SYSCALLSLOG    "[freezer][syscalls] "
#define MAX_SIZE_ARRAY 1024

typedef int (*syscall_wrapper)(struct pt_regs*);
unsigned long sys_call_table_addr;

syscall_wrapper original_getuid;
syscall_wrapper original_execve;
syscall_wrapper original_write;
syscall_wrapper original_openat;
syscall_wrapper original_connect;

int file_uid_array[MAX_SIZE_ARRAY];
int current_file_index = 0;
int process_uid_array[MAX_SIZE_ARRAY];
int current_process_index = 0;
int socket_uid_array[MAX_SIZE_ARRAY];
int current_socket_index = 0;
int sessions_uid_array[MAX_SIZE_ARRAY];
int current_sessions_index = 0;

struct array_uid *whitelist_network[MAX_SIZE_ARRAY];
int current_whitelist_network_index = 0;


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

int resource_data_is_in_array(struct array_uid **array_uids, int *index, char *resource_data)
{
    int orig_uid = original_getuid(NULL);
    int cur = 0;

    printk(KERN_INFO SYSCALLSLOG "uid = %d", orig_uid);

    while (cur < *index)
    {
        if (array_uids[cur]->uid == orig_uid)
        {
            unsigned int i = 0;

            for (; i < array_uids[cur]->array->size; ++i)
            {
                printk("dataInTest = %s\n", (char*)array_uids[cur]->array->array[i]);

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

void array_uid_dispose(struct array_uid **array, int *index)
{
    int cur = 0;
    while (cur < *index)
    {
        array_destroy(array[cur]->array);

        cur++;
    }
}


/**
 * enable_page_rw - Enable read-write rights on a memory page
 * @ptr: ptr to memory page
 *
 * Used to allow write on the syscall table
 */
int enable_page_rw(void* ptr)
{
    unsigned int level;
    pte_t* pte = lookup_address((unsigned long)ptr, &level);

    if (pte->pte & ~_PAGE_RW)
        pte->pte |= _PAGE_RW;

    return 0;
}

/**
 * disable_page_rw - Disable read-write rights on a memory page
 * @ptr: ptr to memory page
 *
 * Used to disable write on the syscall table
 */
int disable_page_rw(void* ptr)
{
    unsigned int level;
    pte_t* pte = lookup_address((unsigned long)ptr, &level);
    pte->pte = pte->pte & ~_PAGE_RW;
    return 0;
}

/**
 * is_hooked_user - Hook a user based on its uid
 * @uid: uid of the user to hook
 *
 * Used to hook a syscall for a specific user.
 * It compares the caller's uid with the param uid
 *
 * Usage example to block the user 1000:
 * if (is_hooked_user(1000)) {return 0;}
 */
int is_hooked_user(int* array, int index)
{
    int orig_uid = original_getuid(NULL);
    int cur = 0;

    // printk(KERN_INFO SYSCALLSLOG "uid = %d", orig_uid);

    while (cur < index)
    {
        if (array[cur] == orig_uid)
        {
            return 1;
        }

        cur++;
    }
    return 0;
}

/**
 * hooked_execve - Hooked function of the syscall execve
 * @regs: struct used by the kernel syscall wrapper, mandatory, DO NOT REMOVE
 *
 * This function is the hooked version of the syscall execve.
 * It blocks the execution for a specific user.
 * It sends the execution flow back to the original execve.
 */
int hooked_execve(struct pt_regs* regs)
{
    //printk(KERN_INFO SYSCALLSLOG "execve was called");

    if (is_hooked_user(process_uid_array, current_process_index))
    {
        printk(KERN_INFO SYSCALLSLOG "excve interrupted");

        // TODO: replace this line with `return 0;` to effectively interrupt the syscall
        // return (*original_execve)(regs);
        return -1;
    }
    return (*original_execve)(regs);
}

/**
 * hooked_connect - Hooked function of the syscall connect
 * @regs: struct used by the kernel syscall wrapper, mandatory, DO NOT REMOVE
 *
 * This function is the hooked version of the syscall connect.
 * It blocks all internet connections attempts for blocked users.
 */
int hooked_connect(struct pt_regs* regs)
{
    //printk(KERN_INFO SYSCALLSLOG "connect was called\n");

    struct __user sockaddr_in* sk = (struct sockaddr_in*)regs->si;
    struct sockaddr_in* copied_sk = kzalloc(sizeof(sk), GFP_KERNEL);
    char ip_addr[16];

    if (!copied_sk)
        printk(KERN_INFO SYSCALLSLOG "error while trying to allocate memory of size %lu \n", sizeof(sk));
    else
    {
        unsigned long error = copy_from_user(copied_sk, sk, sizeof(sk));

        if (error == 0)
        {
            if (copied_sk->sin_family == AF_INET) //  interrupt only internet connections
            {
                printk(KERN_INFO SYSCALLSLOG "internet connection detected\n");

                snprintf(ip_addr, sizeof(ip_addr), "%pIS", copied_sk);

                printk(KERN_INFO SYSCALLSLOG "ip addr is %s\n", ip_addr);

                if (!resource_data_is_in_array(whitelist_network, &current_whitelist_network_index, ip_addr))
                {
                    if (is_hooked_user(socket_uid_array, current_socket_index))
                    {
                        printk(KERN_INFO SYSCALLSLOG "connect interrupted\n");
                        // TODO return -1;
                        return -1;
                        // return (*original_connect)(regs);
                    }
                }
            }
        }
        else
            printk(KERN_INFO SYSCALLSLOG "error while trying to copy_from_user\n");
    }

    kfree(copied_sk);

    return (*original_connect)(regs);
}

int hooked_write(struct pt_regs* regs)
{
    if (is_hooked_user(file_uid_array, current_file_index))
    {
        printk(KERN_INFO SYSCALLSLOG "write interrupted\n");

        // TODO: replace this line with `return 0;` to effectively interrupt the syscall
        return (*original_write)(regs);
    }
    return (*original_write)(regs);
}

int hooked_openat(struct pt_regs* regs)
{
    // block creation of sessions FROM blocked users
    if (is_hooked_user(sessions_uid_array, current_sessions_index))
    {
        const char *passwd_file = "/etc/passwd";
        char opened_file[NAME_MAX] = {0};
        char __user *op_file_user = (char *)regs->si;
        unsigned long error = strncpy_from_user(opened_file, op_file_user, NAME_MAX);
        if (error > 0)
        {
            if (strncmp(opened_file, passwd_file, NAME_MAX) == 0)
            {
                printk(KERN_INFO SYSCALLSLOG "openat interrupted\n");
                return EACCES;
            }
        }
    }

    // block opening of files for blocked users
    if (is_hooked_user(file_uid_array, current_file_index))
    {
        printk(KERN_INFO SYSCALLSLOG "openat interrupted\n");
        return EACCES;
    }

    return (*original_openat)(regs);
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
    // TODO: remove in production
    printk(KERN_INFO SYSCALLSLOG "Uid %d was added to array\n", uid);

    return 1;
}

int remove_uid_from_array(int *array, int *index, unsigned int uid)
{
    int cur = 0;
    int is_rm_index = 0; // false

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

    printk(KERN_INFO SYSCALLSLOG "Uid %d was removed from array\n", uid);
    // TODO: remove these printk in production
    // printk(KERN_INFO SYSCALLSLOG "array[0] = %d", array[0]);
    // printk(KERN_INFO SYSCALLSLOG "array[1] = %d", array[1]);
    // printk(KERN_INFO SYSCALLSLOG "index = %d", *index);

    return 1;
}

int add_to_whitelist(struct array_uid **array, int *index, char *resource_data, unsigned int uid)
{
    int cur = 0;

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

    struct array_uid *new_array_uid = kzalloc(sizeof(struct array_uid), GFP_KERNEL);
    if (!new_array_uid)
        return -1;

    new_array_uid->uid = uid;
    new_array_uid->array = array_new();

    array_push(new_array_uid->array, resource_data);

    array[*index] = new_array_uid;
    *index = *index + 1;

    return 1;
}

int freezer_call_wrapper(struct netlink_cmd *data, char *resource_data)
{
    printk("freezer wrapper called");
    if (data->action == LOCK)
    {
        switch (data->resource)
        {
        case FILE:
            add_uid_to_array(file_uid_array, &current_file_index, data->uid);
            break;

        case PROCESS:
            add_uid_to_array(process_uid_array, &current_process_index, data->uid);
            break;

        case NETWORK:
            add_uid_to_array(socket_uid_array, &current_socket_index, data->uid);
            break;

        case SESSIONS:
            add_uid_to_array(sessions_uid_array, &current_sessions_index, data->uid);
            break;

        default:
            return -1;
        }
    }
    else if (data->action == UNLOCK)
    {
        switch (data->resource)
        {
        case FILE:
            remove_uid_from_array(file_uid_array, &current_file_index, data->uid);
            break;

        case PROCESS:
            remove_uid_from_array(process_uid_array, &current_process_index, data->uid);
            break;

        case NETWORK:
            remove_uid_from_array(socket_uid_array, &current_socket_index, data->uid);
            break;

        case SESSIONS:
            remove_uid_from_array(sessions_uid_array, &current_sessions_index, data->uid);
            break;

        default:
            return -1;
        }
    }
    else if (data->action == WHITELIST)
    {
        switch (data->resource)
        {
        case FILE:
            //add_to_whitelist(file_uid_array, data->resource_data, data->uid);
            break;

        case PROCESS:
            //add_to_whitelist(process_uid_array, data->resource_data, data->uid);
            break;

        case NETWORK:
            add_to_whitelist(whitelist_network, &current_whitelist_network_index, resource_data, data->uid);
            
            printk("size = %d\n", whitelist_network[0]->array->size);

            int i = 0;
            for (; i < whitelist_network[0]->array->size; ++i)
            {                
                printk("data =%s\n", (char*)whitelist_network[0]->array->array[i]);
            }

            break;

        case SESSIONS:
            //add_to_whitelist(sessions_uid_array, data->resource_data, data->uid);
            break;

        default:
            return -1;
        }
    }
    else
    {
        return -1;
    }

    return 0;
}

unsigned long acquire_sys_call_table(void)
{
#ifdef KPROBE_LOOKUP
    unsigned long (*kallsyms_lookup_name)(const char *name);
    struct kprobe kp = {
        .symbol_name = "kallsyms_lookup_name",
    };

    if (register_kprobe(&kp) < 0)
    {
        return NULL;
    }

    kallsyms_lookup_name = (unsigned long (*)(const char *name))kp.addr;
    unregister_kprobe(&kp);
#endif
    return kallsyms_lookup_name("sys_call_table");
}

int init_freezer_syscalls(void)
{
    printk(KERN_INFO SYSCALLSLOG "module has been loaded\n");

    sys_call_table_addr = acquire_sys_call_table();

    enable_page_rw((void*)sys_call_table_addr);

    // Save the original syscalls pointer
    original_openat = ((syscall_wrapper*)sys_call_table_addr)[__NR_openat];
    original_write = ((syscall_wrapper*)sys_call_table_addr)[__NR_write];
    original_execve = ((syscall_wrapper*)sys_call_table_addr)[__NR_execve];
    original_getuid = ((syscall_wrapper*)sys_call_table_addr)[__NR_getuid];
    original_connect = ((syscall_wrapper*)sys_call_table_addr)[__NR_connect];

    // Check that saved original syscalls are valid
    if (!original_write || !original_execve || !original_getuid || !original_connect
        || !original_openat)
        return -1;

    // Replace the syscalls
    ((syscall_wrapper*)sys_call_table_addr)[__NR_openat] = hooked_openat;
    ((syscall_wrapper*)sys_call_table_addr)[__NR_write] = hooked_write;
    ((syscall_wrapper*)sys_call_table_addr)[__NR_execve] = hooked_execve;
    ((syscall_wrapper*)sys_call_table_addr)[__NR_connect] = hooked_connect;

    disable_page_rw((void*)sys_call_table_addr);

    printk(KERN_INFO SYSCALLSLOG "syscalls have been hooked\n");
    return 0;
}

void reset_freezer_syscalls(void)
{
    printk(KERN_INFO SYSCALLSLOG "module has been unloaded\n");

    enable_page_rw((void*)sys_call_table_addr);

    ((syscall_wrapper*)sys_call_table_addr)[__NR_openat] = original_openat;
    ((syscall_wrapper*)sys_call_table_addr)[__NR_write] = original_write;
    ((syscall_wrapper*)sys_call_table_addr)[__NR_execve] = original_execve;
    ((syscall_wrapper*)sys_call_table_addr)[__NR_connect] = original_connect;

    disable_page_rw((void*)sys_call_table_addr);

    array_uid_dispose(whitelist_network, &current_whitelist_network_index);
}