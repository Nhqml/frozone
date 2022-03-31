/* SPDX-License-Identifier: MIT */
/*
 * Copyright (C) 2022 Michel San, Styvell Pidoux
 */
#ifndef FREEZER_MODULE_FREEZER_HOOK_UTILS_H_
#define FREEZER_MODULE_FREEZER_HOOK_UTILS_H_

#include "array.h"


/**
 * @brief Secure function to copy a string (absolute path) from userland to kernelland.
 *
 * @param src pointer to userland pathname string
 * @param max_size maximum number of bytes copied from userlland, ex: PATH_MAX or NAME_MAX
 * @return char* pointer to kernelland string, or NULL if error
 * @details The function copies a string into a pointer that has the exact size of the __user string, and is terminated
 */
char *safe_copy_from_user(const char __user *src, unsigned int max_size);


/*  ---------------------------------------------------- */
/*  ---------- WHITELIST FUNCTIONS ---------- */
/*  ---------------------------------------------------- */

int add_to_whitelist(struct array_uid **array, int *index, char *resource_data, unsigned int uid);

void print_whitelist(struct array_uid **array, int *index);

void array_uid_dispose(struct array_uid **array, int *index);

int uid_is_in_array(int *array, int uid);

/*  ---------------------------------------------------- */
/*  ---------- ARRAY FUNCTIONS ---------- */
/*  ---------------------------------------------------- */

int resource_data_is_in_array(struct array_uid **array_uids, int *index, char *resource_data, int orig_uid);

int add_uid_to_array(int* array, int *index, unsigned int uid);

int remove_uid_from_array(int *array, int *index, unsigned int uid);

#endif  //  FREEZER_MODULE_FREEZER_HOOK_UTILS_H_