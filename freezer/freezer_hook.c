#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <asm/ptrace.h>
#include <linux/socket.h>
#include <linux/kallsyms.h>
#include "freezer_hook.h"

#define SYSCALLSLOG "[freezer][syscalls] "


typedef int (* syscall_wrapper)(struct pt_regs *);

unsigned long sys_call_table_addr;

int enable_page_rw(void *ptr)
{
  unsigned int level;
  pte_t *pte = lookup_address((unsigned long) ptr, &level);

  if(pte->pte &~_PAGE_RW)
    pte->pte |=_PAGE_RW;

  return 0;
}

int disable_page_rw(void *ptr)
{
  unsigned int level;
  pte_t *pte = lookup_address((unsigned long) ptr, &level);
  pte->pte = pte->pte &~_PAGE_RW;
  return 0;
}

syscall_wrapper original_getuid;

syscall_wrapper original_execve;

int is_hooked_user(int uid)
{
	int res = original_getuid(NULL);
	printk(KERN_INFO SYSCALLSLOG "uid = %d", res);
	return res == uid;
}

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


int init_freezer_syscalls(void)
{
  printk(KERN_INFO SYSCALLSLOG "module has been loaded\n");

  sys_call_table_addr = kallsyms_lookup_name("sys_call_table");

  enable_page_rw((void *)sys_call_table_addr);

  // TODO: refactor this
  original_execve = ((syscall_wrapper *)sys_call_table_addr)[__NR_execve];
  original_getuid = ((syscall_wrapper *)sys_call_table_addr)[__NR_getuid];
  if (!original_execve) return -1;
  if (!original_getuid) return -1;

  // Replace current syscall with hooked syscall
  ((syscall_wrapper *)sys_call_table_addr)[__NR_execve] = hooked_execve;

  disable_page_rw((void *)sys_call_table_addr);

  printk(KERN_INFO SYSCALLSLOG "syscalls have been hooked\n");

  return 0;
}

void reset_freezer_syscalls(void)
{
  printk(KERN_INFO SYSCALLSLOG "module has been unloaded\n");

  enable_page_rw((void *)sys_call_table_addr);
  ((syscall_wrapper *)sys_call_table_addr)[__NR_execve] = original_execve;
  disable_page_rw((void *)sys_call_table_addr);
}