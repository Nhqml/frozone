#pragma once

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

__VISIBILITY__("default")
int freeze_users_uid(unsigned int uid);

__VISIBILITY__("default")
int freeze_processes_uid(unsigned int uid);

__VISIBILITY__("default")
int freeze_connections_uid(unsigned int uid);

__VISIBILITY__("default")
int freeze_files_uid(unsigned int uid);

__VISIBILITY__("default")
int unfreeze_users_uid(unsigned int uid);

__VISIBILITY__("default")
int unfreeze_processes_uid(unsigned int uid);

__VISIBILITY__("default")
int unfreeze_connections_uid(unsigned int uid);

__VISIBILITY__("default")
int unfreeze_files_uid(unsigned int uid);

__VISIBILITY__("default")
int freeze_users_except_uid(unsigned int uid);

__VISIBILITY__("default")
int freeze_processes_except_uid(unsigned int uid);

__VISIBILITY__("default")
int freeze_connections_except_uid(unsigned int uid);

__VISIBILITY__("default")
int freeze_files_except_uid(unsigned int uid);

__VISIBILITY__("default")
int unfreeze_users_except_uid(unsigned int uid);

__VISIBILITY__("default")
int unfreeze_processes_except_uid(unsigned int uid);

__VISIBILITY__("default")
int unfreeze_connections_except_uid(unsigned int uid);

__VISIBILITY__("default")
int unfreeze_files_except_uid(unsigned int uid);

__VISIBILITY__("default")
int add_connection_whitelist(unsigned int uid, char *ipaddr);

__VISIBILITY__("default")
int add_connection_whitelist_except_uid(unsigned int uid, char *ipaddr);