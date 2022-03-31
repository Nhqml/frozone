/* SPDX-License-Identifier: MIT */
/*
 * Copyright (C) 2022 Michel San
 */
#pragma once

int freezer_ts_init(void);
int freezer_ts_clean(void);


void test_freeze_connections(void);
void test_freeze_processes(void);
void test_freeze_open_files(void);
void test_freeze_open_to_blocked_user(void);

void test_freeze_whitelist_connections(void);
void test_freeze_whitelist_processes(void);
