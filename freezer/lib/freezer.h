/* SPDX-License-Identifier: MIT */
/*
 * Copyright (C) 2022 Styvell Pidoux
 */

#ifndef FREEZER_LIB_FREEZER_H_
#define FREEZER_LIB_FREEZER_H_

#define __UNUSED__        __attribute__((unused))
#define __VISIBILITY__(V) __attribute__((visibility(V)))

// __VISIBILITY__("default")
// int freeze_users(void);

// __VISIBILITY__("default")
// int freeze_processes(void);

// __VISIBILITY__("default")
// int freeze_connections(void);

// __VISIBILITY__("default")
// int freeze_files(void);

/**
** \brief Freeze the users resources for the specific uid
**
** \param uid User uid to freeze the resource
** \return bool
*/
__VISIBILITY__("default")
int freeze_users_uid(unsigned int uid);

/**
** \brief Freeze the processes resources for the specific uid
**
** \param uid User uid to freeze the resource
** \return bool
*/
__VISIBILITY__("default")
int freeze_processes_uid(unsigned int uid);

/**
** \brief Freeze the connections resources for the specific uid
**
** \param uid User uid to freeze the resource
** \return bool
*/
__VISIBILITY__("default")
int freeze_connections_uid(unsigned int uid);

/**
** \brief Freeze the files resources for the specific uid
**
** \param uid User uid to freeze the resource
** \return bool
*/
__VISIBILITY__("default")
int freeze_files_uid(unsigned int uid);

/**
** \brief Unfreeze the users resources for the specific uid
**
** \param uid User uid to freeze the resource
** \return bool
*/
__VISIBILITY__("default")
int unfreeze_users_uid(unsigned int uid);

/**
** \brief Unfreeze the processes resources for the specific uid
**
** \param uid User uid to freeze the resource
** \return bool
*/
__VISIBILITY__("default")
int unfreeze_processes_uid(unsigned int uid);

/**
** \brief Unfreeze the connections resources for the specific uid
**
** \param uid User uid to freeze the resource
** \return bool
*/
__VISIBILITY__("default")
int unfreeze_connections_uid(unsigned int uid);

/**
** \brief Unfreeze the files resources for the specific uid
**
** \param uid User uid to freeze the resource
** \return bool
*/
__VISIBILITY__("default")
int unfreeze_files_uid(unsigned int uid);

/**
** \brief Freeze the users resources for all uid except the specific uid
**
** \param uid User uid to NOT freeze the resource
** \return bool
*/
__VISIBILITY__("default")
int freeze_users_except_uid(unsigned int uid);

/**
** \brief Freeze the processes resources for all uid except the specific uid
**
** \param uid User uid to NOT freeze the resource
** \return bool
*/
__VISIBILITY__("default")
int freeze_processes_except_uid(unsigned int uid);

/**
** \brief Freeze the connections resources for all uid except the specific uid
**
** \param uid User uid to NOT freeze the resource
** \return bool
*/
__VISIBILITY__("default")
int freeze_connections_except_uid(unsigned int uid);

/**
** \brief Freeze the files resources for all uid except the specific uid
**
** \param uid User uid to NOT freeze the resource
** \return bool
*/
__VISIBILITY__("default")
int freeze_files_except_uid(unsigned int uid);

/**
** \brief Unfreeze the users resources for all uid except the specific uid
**
** \param uid User uid to NOT freeze the resource
** \return bool
*/
__VISIBILITY__("default")
int unfreeze_users_except_uid(unsigned int uid);

/**
** \brief Unfreeze the processes resources for all uid except the specific uid
**
** \param uid User uid to NOT freeze the resource
** \return bool
*/
__VISIBILITY__("default")
int unfreeze_processes_except_uid(unsigned int uid);

/**
** \brief Unfreeze the connections resources for all uid except the specific uid
**
** \param uid User uid to NOT freeze the resource
** \return bool
*/
__VISIBILITY__("default")
int unfreeze_connections_except_uid(unsigned int uid);

/**
** \brief Unfreeze the files resources for all uid except the specific uid
**
** \param uid User uid to NOT freeze the resource
** \return bool
*/
__VISIBILITY__("default")
int unfreeze_files_except_uid(unsigned int uid);

__VISIBILITY__("default")
int add_connection_whitelist(unsigned int uid, char *ipaddr);

__VISIBILITY__("default")
int add_connection_whitelist_except_uid(unsigned int uid, char *ipaddr);

#endif  //  FREEZER_LIB_FREEZER_H_
