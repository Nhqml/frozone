#!/bin/bash

# Script used by meson.build to define the kernel version

# if the script is ran from a gitlab pipeline -> get the version from env var
# if ran locally, get the kernel version from `uname -r`

if [ -n "$(echo $IS_GITLAB_JOB)" ]
then
    K_V=$(echo $KERNEL_VERSION)
else
    K_V=$(uname -r)
fi

echo $K_V