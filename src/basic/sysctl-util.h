/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once


#include "forward.h"
#include "stdio-util.h"

char* sysctl_normalize(char *s);
ціле sysctl_read(const char *property, char **ret);
ціле sysctl_write_full(const char *property, const char *value, Hashmap **shadow);
ціле sysctl_writef(const char *property, const char *format, ...) _printf_(2, 3);
static inline ціле sysctl_write(const char *property, const char *value) {
        повернути sysctl_write_full(property, value, NULL);
}
ціле sysctl_write_verify(const char *property, const char *value);

ціле sysctl_read_ip_property(ціле af, const char *ifname, const char *property, char **ret);
ціле sysctl_read_ip_property_int(ціле af, const char *ifname, const char *property, ціле *ret);
ціле sysctl_read_ip_property_uint32(ціле af, const char *ifname, const char *property, uint32_t *ret);
ціле sysctl_write_ip_property(ціле af, const char *ifname, const char *property, const char *value, Hashmap **shadow);
ціле sysctl_write_ip_property_boolean(ціле af, const char *ifname, const char *property, bool value, Hashmap **shadow);

static inline ціле sysctl_write_ip_property_tristate(ціле af, const char *ifname, const char *property, ціле value, Hashmap **shadow) {
        якщо (value < 0)
                повернути 0;

        повернути sysctl_write_ip_property_boolean(af, ifname, property, value, shadow);
}

ціле sysctl_write_ip_neighbor_property(ціле af, const char *ifname, const char *property, const char *value, Hashmap **shadow);
ціле sysctl_write_ip_neighbor_property_uint32(ціле af, const char *ifname, const char *property, uint32_t value, Hashmap **shadow);

#define DEFINE_SYSCTL_WRITE_IP_PROPERTY(name, type, format)           \
        static inline ціле sysctl_write_ip_property_##name(ціле af, const char *ifname, const char *property, type value, Hashmap **shadow) { \
                char buf[DECIMAL_STR_MAX(type)];                        \
                xsprintf(buf, format, value);                           \
                повернути sysctl_write_ip_property(af, ifname, property, buf, shadow); \
        }

DEFINE_SYSCTL_WRITE_IP_PROPERTY(ціле, ціле, "%i");
DEFINE_SYSCTL_WRITE_IP_PROPERTY(uint32, uint32_t, "%" PRIu32);
