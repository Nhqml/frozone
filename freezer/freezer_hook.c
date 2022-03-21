#include "freezer_hook.h"

#include <asm/ptrace.h>
#include <linux/kallsyms.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/socket.h>
#include <linux/version.h>

#define SYSCALLSLOG "[freezer][syscalls] "


typedef int (* syscall_wrapper)(struct pt_regs *);
unsigned long sys_call_table_addr;


syscall_wrapper original_getuid;
syscall_wrapper original_execve;
syscall_wrapper original_write;


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

int hook_syscall(unsigned long syscall_cste, int (*hook)(struct pt_regs*))
{
  sys_call_table_addr = kallsyms_lookup_name("sys_call_table");

  enable_page_rw((void *)sys_call_table_addr);

  ((syscall_wrapper *)sys_call_table_addr)[syscall_cste] = *hook;

  disable_page_rw((void *)sys_call_table_addr);

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
int is_hooked_user(int uid)
{
    int res = original_getuid(NULL);
    printk(KERN_INFO SYSCALLSLOG "uid = %d", res);
    return res == uid;
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
    if (is_hooked_user(1001))
    {
        printk(KERN_INFO SYSCALLSLOG "excve interrupted for user 1001");
        return 0;
    }
    return (*original_execve)(regs);
}

int hooked_write(struct pt_regs *regs)
{
  return (*original_write)(regs);
}

int freeze_file(void)
{
  printk(KERN_INFO SYSCALLSLOG "Freeze file resource");

  hook_syscall(__NR_write, hooked_write);

  return 0;
}

int freeze_process(void)
{
  printk(KERN_INFO SYSCALLSLOG "Freeze process resource");

  hook_syscall(__NR_execve, hooked_execve);

  return 0;
}

int freeze_network(void)
{
  printk(KERN_INFO SYSCALLSLOG "Freeze network resource");
  return 0;
}

int freeze_sessions(void)
{
  printk(KERN_INFO SYSCALLSLOG "Freeze session resource");
  return 0;
}

int freezer_call_wrapper(struct netlink_cmd *data)
{	
  printk("freezer wrapper called");
  switch (data->resource)
  {
    case FILE:
      return freeze_file();

    case PROCESS:
      return freeze_process();

    case NETWORK:
      return freeze_network();

    case SESSIONS:
      return freeze_sessions();  
    
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
  //((syscall_wrapper *)sys_call_table_addr)[__NR_write] = log_write;
  //((syscall_wrapper *)sys_call_table_addr)[__NR_execve] = log_execve;

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
