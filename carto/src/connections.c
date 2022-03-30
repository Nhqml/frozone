/* SPDX-License-Identifier: MIT */
/*
 * Copyright (C) 2022 Kenji Gaillac
 */

#include "connections.h"

#include <carto.h>
#include <stdio.h>

void hex_to_ipv4(char* hex, struct in_addr* ipv4)
{
    ipv4->s_addr = 0;
    for (size_t i = 0; i < 4; ++i)
    {
        ipv4->s_addr <<= 8;

        char hex_byte[3];
        hex_byte[0] = hex[2 * i];
        hex_byte[1] = hex[2 * i + 1];
        hex_byte[2] = 0;

        ipv4->s_addr += strtol(hex_byte, NULL, 16);
    }
}

void hex_to_ipv6(char* hex, struct in6_addr* ipv6)
{
    uint32_t* addr_words = ipv6->s6_addr32;

    for (size_t i = 0; i < 4; ++i)
    {
        for (size_t j = 0; j < 4; ++j)
        {
            addr_words[i] <<= 8;

            char hex_byte[3];
            hex_byte[0] = hex[8 * i + 2 * j];
            hex_byte[1] = hex[8 * i + 2 * j + 1];
            hex_byte[2] = 0;

            addr_words[i] += strtol(hex_byte, NULL, 16);
        }
    }
}

Array* add_connections_from(char* file_path, Array* a, enum conn_type conn_type, sa_family_t sa)
{
    FILE* f = fopen(file_path, "r");
    if (f != NULL)
    {
        char buf[256];

        // Skip first line (header)
        fgets(buf, 256, f);

        // Large enough to handle IPv4 and IPv6
        char s_addr[48], d_addr[48];
        in_port_t s_port, d_port;
        uid_t uid;
        uint8_t state;

        // Plz don't ask questions, I hate parsing in C
        while (fscanf(f, "%*s %48[^:]:%hx %48[^:]:%hx %hhx %*s %*s %*s %u %*[^\n]\n", s_addr, &s_port, d_addr, &d_port,
                      &state, &uid)
               != EOF)
        {
            connection_t* conn = xmalloc(sizeof(connection_t));
            conn->uid = uid;
            conn->type = conn_type;
            conn->addr_type = sa;

            if (conn->addr_type == AF_INET)
            {
                hex_to_ipv4(s_addr, &(conn->s_addr.addr));
                hex_to_ipv4(d_addr, &(conn->d_addr.addr));
            } else
            {
                hex_to_ipv6(s_addr, &(conn->s_addr.addr6));
                hex_to_ipv6(d_addr, &(conn->d_addr.addr6));
            }

            conn->s_port = s_port;
            conn->d_port = d_port;
            conn->state = state;

            array_push(a, conn);
        }

        fclose(f);
    }

    return a;
}
