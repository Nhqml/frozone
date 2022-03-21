#pragma once

#include <carto.h>

utmp_t* clone_utmp(utmp_t* utmp);

void free_users(utmp_t** users);
