/* SPDX-License-Identifier: MIT */
/*
 * Copyright (C) 2022 Michel San, Styvell Pidoux
 */
#ifndef FREEZER_MODULE_FREEZER_HOOK_UTILS_H_
#define FREEZER_MODULE_FREEZER_HOOK_UTILS_H_


/**
 * @brief Secure function to copy a string (absolute path) from userland to kernelland.
 *
 * @param src pointer to userland pathname string
 * @param max_size maximum number of bytes copied from userlland, ex: PATH_MAX or NAME_MAX
 * @return char* pointer to kernelland string, or NULL if error
 * @details The function copies a string into a pointer that has the exact size of the __user string, and is terminated
 */
char *safe_copy_from_user(const char __user *src, unsigned int max_size);


#endif  //  FREEZER_MODULE_FREEZER_HOOK_UTILS_H_