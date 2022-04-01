/* SPDX-License-Identifier: MIT */
/*
 * Copyright (C) 2022 Kenji Gaillac
 */

#pragma once

#include <carto.h>
#include <netinet/in.h>

#include "utils.h"

/**
** \brief Convert the hexa representation of IPv4 (from /proc/net/<...>)
**
** \param hex Hexa representation
** \param ipv4 Struct to store the result into
*/
void hex_to_ipv4(char* hex, struct in_addr* ipv4);

/**
** \brief Convert the hexa representation of IPv6 (from /proc/net/<...>)
**
** \param hex Hexa representation
** \param ipv4 Struct to store the result into
*/
void hex_to_ipv6(char* hex, struct in6_addr* ipv6);

/**
** \brief Read connections from specified file and add them to the array
**
** \param file_path File to read connections from
** \param a Array to add to
** \param conn_t Connection type (UDP / TCP)
** \param sa_t Address family (IPv4 / IPv6)
** \return Array*
*/
Array* add_connections_from(char* file_path, Array* a, enum prot conn_t, sa_family_t sa_t);
