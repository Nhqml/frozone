/* SPDX-License-Identifier: MIT */
/*
 * Copyright (C) 2022 Michel San, Styvell Pidoux
 */

#ifndef FREEZER_MODULE_FREEZER_HOOK_UTILS_H_
#define FREEZER_MODULE_FREEZER_HOOK_UTILS_H_

#include "array.h"

/**
 * \brief Secure function to copy a string (absolute path) from userland to kernelland.
 *
 * \param src pointer to userland pathname string
 * \param max_size maximum number of bytes copied from userlland, ex: PATH_MAX or NAME_MAX
 * \return char* pointer to kernelland string, or NULL if error
 * \details The function copies a string into a pointer that has the exact size of the __user string, and is terminated
 */
char *safe_copy_from_user(const char __user *src, unsigned int max_size);


/*  ---------------------------------------------------- */
/*  ---------- WHITELIST FUNCTIONS ---------- */
/*  ---------------------------------------------------- */

/**
** \brief Add a resource data to a whitelist for one user
**
** \param array Array that refers to a whitelist
** \param index Current index of the whitelist
** \param resource_data Resource data to put in the whitelist
** \param uid User id
** \return bool
*/
int add_to_whitelist(struct array_uid **array, int *index, char *resource_data, unsigned int uid);

/**
** \brief Print a whitelist
**
** \param array Array that refers to a whitelist
** \param index Current index of the whitelist
*/
void print_whitelist(struct array_uid **array, int *index);

/**
** \brief Free the entire whitelist
**
** \param array Array that refers to a whitelist
** \param index Current index of the whitelist
*/
void whitelist_dispose(struct array_uid **array, int *index);

/**
** \brief Check if a resource data is in a whitelist
**
** \param array_uids Array that refers to a whitelist
** \param index Current index of the whitelist
** \param resource_data Resource data to check
** \param orig_uid User id
** \return bool
*/
int resource_data_is_in_whitelist(struct array_uid **array_uids, int *index, char *resource_data, int orig_uid);

/*  ---------------------------------------------------- */
/*  ---------- ARRAY FUNCTIONS ---------- */
/*  ---------------------------------------------------- */

/**
** \brief Check if an uid is in an array
**
** \param array Array of uids
** \param uid User id
** \return bool
*/
int uid_is_in_array(int *array, int uid);

/**
** \brief Check if an uid is in an array
**
** \param array Array of uids
** \param index Current index of the array
** \param uid User id
** \return bool
*/
int add_uid_to_array(int* array, int *index, unsigned int uid);

/**
** \brief Remove an id from the array
**
** \param array Array of uids
** \param index Current index of the array
** \param uid User id
** \return bool
*/
int remove_uid_from_array(int *array, int *index, unsigned int uid);

#endif  //  FREEZER_MODULE_FREEZER_HOOK_UTILS_H_
