/* SPDX-License-Identifier: MIT */
/*
 * Copyright (C) 2022 Kenji Gaillac
 */

#pragma once

#include <sys/types.h>

char* proc_readlink(pid_t pid, const char* attribute);
