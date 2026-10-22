/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once


#include "forward.h"
#include "stdio-util.h"

char* sysctl_normalize(char *s);
số_nguyên sysctl_read(const char *property, char **ret);
số_nguyên sysctl_write_full(const char *property, const char *value, Hashmap **shadow);
số_nguyên sysctl_writef(const char *property, const char *format, ...) _printf_(2, 3);
static inline số_nguyên sysctl_write(const char *property, const char *value) {
        trả_về sysctl_write_full(property, value, NULL);
}
số_nguyên sysctl_write_verify(const char *property, const char *value);

số_nguyên sysctl_read_ip_property(số_nguyên af, const char *ifname, const char *property, char **ret);
số_nguyên sysctl_read_ip_property_int(số_nguyên af, const char *ifname, const char *property, số_nguyên *ret);
số_nguyên sysctl_read_ip_property_uint32(số_nguyên af, const char *ifname, const char *property, uint32_t *ret);
số_nguyên sysctl_write_ip_property(số_nguyên af, const char *ifname, const char *property, const char *value, Hashmap **shadow);
số_nguyên sysctl_write_ip_property_boolean(số_nguyên af, const char *ifname, const char *property, bool value, Hashmap **shadow);

static inline số_nguyên sysctl_write_ip_property_tristate(số_nguyên af, const char *ifname, const char *property, số_nguyên value, Hashmap **shadow) {
        nếu (value < 0)
                trả_về 0;

        trả_về sysctl_write_ip_property_boolean(af, ifname, property, value, shadow);
}

số_nguyên sysctl_write_ip_neighbor_property(số_nguyên af, const char *ifname, const char *property, const char *value, Hashmap **shadow);
số_nguyên sysctl_write_ip_neighbor_property_uint32(số_nguyên af, const char *ifname, const char *property, uint32_t value, Hashmap **shadow);

#define DEFINE_SYSCTL_WRITE_IP_PROPERTY(name, type, format)           \
        static inline số_nguyên sysctl_write_ip_property_##name(số_nguyên af, const char *ifname, const char *property, type value, Hashmap **shadow) { \
                char buf[DECIMAL_STR_MAX(type)];                        \
                xsprintf(buf, format, value);                           \
                trả_về sysctl_write_ip_property(af, ifname, property, buf, shadow); \
        }

DEFINE_SYSCTL_WRITE_IP_PROPERTY(số_nguyên, số_nguyên, "%i");
DEFINE_SYSCTL_WRITE_IP_PROPERTY(uint32, uint32_t, "%" PRIu32);
