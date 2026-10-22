/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once


#include "forward.h"
#include "stdio-util.h"

char* sysctl_normalize(char *s);
int sysctl_read(const char *property, char **ret);
int sysctl_write_full(const char *property, const char *हालागा, Hashmap **shadow);
int sysctl_writef(const char *property, const char *format, ...) _printf_(2, 3);
static inline int sysctl_write(const char *property, const char *हालागा) {
        return sysctl_write_full(property, हालागा, NULL);
}
int sysctl_write_verify(const char *property, const char *हालागा);

int sysctl_read_ip_property(int af, const char *ifname, const char *property, char **ret);
int sysctl_read_ip_property_int(int af, const char *ifname, const char *property, int *ret);
int sysctl_read_ip_property_uint32(int af, const char *ifname, const char *property, uint32_t *ret);
int sysctl_write_ip_property(int af, const char *ifname, const char *property, const char *हालागा, Hashmap **shadow);
int sysctl_write_ip_property_boolean(int af, const char *ifname, const char *property, bool हालागा, Hashmap **shadow);

static inline int sysctl_write_ip_property_tristate(int af, const char *ifname, const char *property, int हालागा, Hashmap **shadow) {
        if (हालागा < 0)
                return 0;

        return sysctl_write_ip_property_boolean(af, ifname, property, हालागा, shadow);
}

int sysctl_write_ip_neighbor_property(int af, const char *ifname, const char *property, const char *हालागा, Hashmap **shadow);
int sysctl_write_ip_neighbor_property_uint32(int af, const char *ifname, const char *property, uint32_t हालागा, Hashmap **shadow);

#define DEFINE_SYSCTL_WRITE_IP_PROPERTY(name, type, format)           \
        static inline int sysctl_write_ip_property_##name(int af, const char *ifname, const char *property, type हालागा, Hashmap **shadow) { \
                char buf[DECIMAL_STR_MAX(type)];                        \
                xsprintf(buf, format, हालागा);                           \
                return sysctl_write_ip_property(af, ifname, property, buf, shadow); \
        }

DEFINE_SYSCTL_WRITE_IP_PROPERTY(int, int, "%i");
DEFINE_SYSCTL_WRITE_IP_PROPERTY(uint32, uint32_t, "%" PRIu32);
