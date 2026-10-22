/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once


#include "forward.h"
#include "stdio-util.h"

char* sysctl_normalize(char *s);
整数 sysctl_read(const char *property, char **ret);
整数 sysctl_write_full(const char *property, const char *value, Hashmap **shadow);
整数 sysctl_writef(const char *property, const char *format, ...) _printf_(2, 3);
static inline 整数 sysctl_write(const char *property, const char *value) {
        戻る sysctl_write_full(property, value, NULL);
}
整数 sysctl_write_verify(const char *property, const char *value);

整数 sysctl_read_ip_property(整数 af, const char *ifname, const char *property, char **ret);
整数 sysctl_read_ip_property_int(整数 af, const char *ifname, const char *property, 整数 *ret);
整数 sysctl_read_ip_property_uint32(整数 af, const char *ifname, const char *property, uint32_t *ret);
整数 sysctl_write_ip_property(整数 af, const char *ifname, const char *property, const char *value, Hashmap **shadow);
整数 sysctl_write_ip_property_boolean(整数 af, const char *ifname, const char *property, bool value, Hashmap **shadow);

static inline 整数 sysctl_write_ip_property_tristate(整数 af, const char *ifname, const char *property, 整数 value, Hashmap **shadow) {
        もし (value < 0)
                戻る 0;

        戻る sysctl_write_ip_property_boolean(af, ifname, property, value, shadow);
}

整数 sysctl_write_ip_neighbor_property(整数 af, const char *ifname, const char *property, const char *value, Hashmap **shadow);
整数 sysctl_write_ip_neighbor_property_uint32(整数 af, const char *ifname, const char *property, uint32_t value, Hashmap **shadow);

#define DEFINE_SYSCTL_WRITE_IP_PROPERTY(name, type, format)           \
        static inline 整数 sysctl_write_ip_property_##name(整数 af, const char *ifname, const char *property, type value, Hashmap **shadow) { \
                char buf[DECIMAL_STR_MAX(type)];                        \
                xsprintf(buf, format, value);                           \
                戻る sysctl_write_ip_property(af, ifname, property, buf, shadow); \
        }

DEFINE_SYSCTL_WRITE_IP_PROPERTY(整数, 整数, "%i");
DEFINE_SYSCTL_WRITE_IP_PROPERTY(uint32, uint32_t, "%" PRIu32);
