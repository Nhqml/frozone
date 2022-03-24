#pragma once

#define __UNUSED__        __attribute__((unused))
#define __VISIBILITY__(V) __attribute__((visibility(V)))

__VISIBILITY__("default")
int freeze_users(void);

__VISIBILITY__("default")
int freeze_processes(void);

__VISIBILITY__("default")
int freeze_connections(void);

__VISIBILITY__("default")
int freeze_files(void);

__VISIBILITY__("default")
int freeze_users_uid(int uid);

__VISIBILITY__("default")
int freeze_processes_uid(int uid);

__VISIBILITY__("default")
int freeze_connections_uid(int uid);

__VISIBILITY__("default")
int freeze_files_uid(int uid);

__VISIBILITY__("default")
int unfreeze_users_uid(int uid);

__VISIBILITY__("default")
int unfreeze_processes_uid(int uid);

__VISIBILITY__("default")
int unfreeze_connections_uid(int uid);

__VISIBILITY__("default")
int unfreeze_files_uid(int uid);