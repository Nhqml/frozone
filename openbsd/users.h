/* SPDX-License-Identifier: MIT */
/*
 * Copyright (C) 2022 Kenji Gaillac
 */

#pragma once

#include <carto.h>

utmp_t* clone_utmp(utmp_t* utmp);

void free_users(utmp_t** users);

