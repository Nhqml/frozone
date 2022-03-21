#ifndef FREEZER_HOOK_H
#define FREEZER_HOOK_H

/**
 * enable_page_rw - Enable read-write rights on a memory page
 * @ptr: ptr to memory page
 *
 * Used to allow write on the syscall table
 */
int enable_page_rw(void* ptr);

/**
 * disable_page_rw - Disable read-write rights on a memory page
 * @ptr: ptr to memory page
 *
 * Used to disable write on the syscall table
 */
int disable_page_rw(void* ptr);

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
int is_hooked_user(int uid);

/**
 * hooked_excve - Hooked function of the syscall excve
 * @regs: struct used by the kernel syscall wrapper, mandatory, DO NOT REMOVE
 *
 * This function is the hooked version of the syscall execve.
 * It blocks the execution for a specific user.
 * It sends the execution flow back to the original execve.
 */
int hooked_execve(struct pt_regs* regs);

/**
 * init_freezer_syscalls - Initialize the syscall table with our hooked syscalls
 */
int init_freezer_syscalls(void);

/**
 * reset_freezer_syscalls - Reset the syscall table with the original syscalls
 */
void reset_freezer_syscalls(void);

#endif
