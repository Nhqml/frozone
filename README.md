# Frozone
*AppING 2022 - Final project*

## What

The goal of this project was to create 2 modules / libs in the C language that
would allow to retrieve live information on a system (running processes, logged
users, opened files, etc...), and freeze those resources on a Linux OS.

This project is divided in 2 libs:

- `carto`, which contains code to retrieve information on a system
- `freezer`, which consists of:
  - a linux kernel module (LKM)
  - a static lib to interact with the kernel module

The freezer_module supports the Linux Kernel from version 5.4.0, up to 5.11+.

It was tested on:
- Ubuntu 20.04, kernel version: 5.4.0-104
- Ubuntu 21.04, kernel version: 5.11.0-49

## Build

### Build (debug)

```bash
meson build && meson compile -C build
```

### Test

```bash
meson test -C build
```

#### Verbose mode

```bash
meson test -C build -v
```

#### With valgrind

```bash
meson test -C build -v --wrap='valgrind'
```

or, for a more detailed output:

```bash
meson test -C build -v --wrap='valgrind --track-origins=yes --leak-check=full'
```

### Build (release)

```bash
meson --buildtype=release build-release && meson compile -C build-release
```

## Installation

Retrieve the artifacts built from the latest pipeline manually (artifacts.zip).
```
artifacts.zip
 |--build-release
    |- carto
    |- examples
    |- freezer
    |- meson-info
    |-  ...
```

Load the module
```bash
sudo insmod build-release/freezer/module/freezer_module.ko
```

To unload the module
```bash
sudo rmmod freezer_module
```

## Manual testing

After loading the kernel module with previous commands, the `build-release/examples/demo_freezer` can be used to send commands directly to the module.

Print usage:
```bash
$ cd build-release/examples/
$ ./demo_freezer

Usage: ./freezer_demo <action> <resource>
<action>: lock | unlock | whitelist
<resource>: proc | con | files | users
```

All commands are applied to the user with uid *1001*, which can be edited directly in the file `demo_freezer.c`.

Either create a user with uid *1001* using `sudo useradd test_user -u 1001`, or change it in the code and rebuild.

### Locking the files

```bash
./demo_freezer lock files                   # enable locking of files
./demo_freezer whitelist files /etc/passwd  # add /etc/passwd to the whitelist, so it's not blocked
./demo_freezer unlock files                 # disable locking of files
```

Warning: locking the files will lock all files, even the shared libraries used by commands such as `cat` or `ls`, so they must be added first manually to the whitelist.


### Freezing connections

```bash
./demo_freezer lock con             # enable freeze of connections
./demo_freezer whitelist 1.1.1.1    # add 1.1.1.1 to the whitelist, so curl 1.1.1.1 should work
./demo_freezer unlock con           # disable freeze of connections
```

### Freeze users

This command will prevent users from connecting to other users account(local lateral / horizontal movement).

```bash
./demo_freezer lock users       # lock user 1001, so 1001 won't be able to connect to other users
su lambda_user                  # won't work
sudo su                         # won't work
./demo_freezer unlock users     # unlock freeze of users
