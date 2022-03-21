#include "freezer_hook.h"

#include <asm/ptrace.h>
#include <linux/kallsyms.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/socket.h>
#include <linux/version.h>

#define SYSCALLSLOG "[freezer][syscalls] "
#define MAX_SIZE_ARRAY 1024


typedef int (* syscall_wrapper)(struct pt_regs *);
unsigned long sys_call_table_addr;


syscall_wrapper original_getuid;
syscall_wrapper original_execve;
syscall_wrapper original_write;

int file_uid_array[MAX_SIZE_ARRAY];
int current_file_index = 0;
int process_uid_array[MAX_SIZE_ARRAY];
int current_process_index = 0;
int socket_uid_array[MAX_SIZE_ARRAY];
int current_socket_index = 0;
int sessions_uid_array[MAX_SIZE_ARRAY];
int current_sessions_index = 0;


/**
 * enable_page_rw - Enable read-write rights on a memory page
 * @ptr: ptr to memory page
 *
 * Used to allow write on the syscall table
 */
int enable_page_rw(void *ptr)
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
int disable_page_rw(void *ptr) 
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

	printk(KERN_INFO SYSCALLSLOG "uid = %d", orig_uid);
  
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
 * hooked_excve - Hooked function of the syscall execve
 * @regs: struct used by the kernel syscall wrapper, mandatory, DO NOT REMOVE
 *
 * This function is the hooked version of the syscall execve.
 * It blocks the execution for a specific user.
 * It sends the execution flow back to the original execve.
 */
int hooked_execve(struct pt_regs *regs)
{
  printk(KERN_INFO SYSCALLSLOG "execve was called");

  if (is_hooked_user(process_uid_array, current_process_index))
  {
    printk(KERN_INFO SYSCALLSLOG "excve interrupted");

    return (*original_execve)(regs);
  }
  return (*original_execve)(regs);
}

int hooked_write(struct pt_regs *regs)
{
  printk(KERN_INFO SYSCALLSLOG "write was called");

  if (is_hooked_user(file_uid_array, current_file_index))
  {
    printk(KERN_INFO SYSCALLSLOG "write interrupted");
    
    return (*original_write)(regs);
  }
  return (*original_write)(regs);
}

int add_uid_to_array(int* array, int index, int uid)
{
  if (index >= MAX_SIZE_ARRAY)
  {
    return 0; // datalab
  }

  int cur = 0;
  while (cur < index)
  {
    if (array[cur] == uid)
    {
      return 0;
    }

    cur++;
  }

  // uid not aleready in array, so add uid
  array[index] = uid;
  printk(KERN_INFO SYSCALLSLOG "Uid %d was added to array", uid);

  return 1;
}

int freezer_call_wrapper(struct netlink_cmd *data)
{	
  printk("freezer wrapper called");
  switch (data->resource)
  {
    case FILE:
      current_file_index = current_file_index + add_uid_to_array(file_uid_array, current_file_index, data->uid);
      break;

    case PROCESS:
      current_process_index = current_process_index + add_uid_to_array(process_uid_array, current_process_index, data->uid);
      break;

    case NETWORK:
      current_socket_index = current_socket_index + add_uid_to_array(socket_uid_array, current_socket_index, data->uid);
      break;

    case SESSIONS:
      current_sessions_index = current_sessions_index + add_uid_to_array(sessions_uid_array, current_sessions_index, data->uid); 
      break;

    default:
      return -1;
  }

  return 0;
}	

int init_freezer_syscalls(void)
{
    printk(KERN_INFO SYSCALLSLOG "module has been loaded\n");

    sys_call_table_addr = kallsyms_lookup_name("sys_call_table");

    enable_page_rw((void*)sys_call_table_addr);

  // TODO: refactor this
  original_write = ((syscall_wrapper *)sys_call_table_addr)[__NR_write];
  original_execve = ((syscall_wrapper *)sys_call_table_addr)[__NR_execve];
  original_getuid = ((syscall_wrapper *)sys_call_table_addr)[__NR_getuid];
  if (!original_write) return -1;
  if (!original_execve) return -1;
  if (!original_getuid) return -1;
  ((syscall_wrapper *)sys_call_table_addr)[__NR_write] = hooked_write;
  ((syscall_wrapper *)sys_call_table_addr)[__NR_execve] = hooked_execve;

    disable_page_rw((void*)sys_call_table_addr);

  printk(KERN_INFO SYSCALLSLOG "syscalls have been hooked\n");
  return 0;
}

void reset_freezer_syscalls(void)
{
    printk(KERN_INFO SYSCALLSLOG "module has been unloaded\n");

  enable_page_rw((void *)sys_call_table_addr);
  // TODO: refactor this
  ((syscall_wrapper *)sys_call_table_addr)[__NR_write] = original_write;
  ((syscall_wrapper *)sys_call_table_addr)[__NR_execve] = original_execve;
  disable_page_rw((void *)sys_call_table_addr);
}
