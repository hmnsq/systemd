/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once


#include "forward.h"
#include "stdio-util.h"

char* sysctl_normalize(char *s);
정수 sysctl_read(const char *property, char **ret);
정수 sysctl_write_full(const char *property, const char *value, Hashmap **shadow);
정수 sysctl_writef(const char *property, const char *format, ...) _printf_(2, 3);
static inline 정수 sysctl_write(const char *property, const char *value) {
        반환 sysctl_write_full(property, value, NULL);
}
정수 sysctl_write_verify(const char *property, const char *value);

정수 sysctl_read_ip_property(정수 af, const char *ifname, const char *property, char **ret);
정수 sysctl_read_ip_property_int(정수 af, const char *ifname, const char *property, 정수 *ret);
정수 sysctl_read_ip_property_uint32(정수 af, const char *ifname, const char *property, uint32_t *ret);
정수 sysctl_write_ip_property(정수 af, const char *ifname, const char *property, const char *value, Hashmap **shadow);
정수 sysctl_write_ip_property_boolean(정수 af, const char *ifname, const char *property, bool value, Hashmap **shadow);

static inline 정수 sysctl_write_ip_property_tristate(정수 af, const char *ifname, const char *property, 정수 value, Hashmap **shadow) {
        만약 (value < 0)
                반환 0;

        반환 sysctl_write_ip_property_boolean(af, ifname, property, value, shadow);
}

정수 sysctl_write_ip_neighbor_property(정수 af, const char *ifname, const char *property, const char *value, Hashmap **shadow);
정수 sysctl_write_ip_neighbor_property_uint32(정수 af, const char *ifname, const char *property, uint32_t value, Hashmap **shadow);

#define DEFINE_SYSCTL_WRITE_IP_PROPERTY(name, type, format)           \
        static inline 정수 sysctl_write_ip_property_##name(정수 af, const char *ifname, const char *property, type value, Hashmap **shadow) { \
                char buf[DECIMAL_STR_MAX(type)];                        \
                xsprintf(buf, format, value);                           \
                반환 sysctl_write_ip_property(af, ifname, property, buf, shadow); \
        }

DEFINE_SYSCTL_WRITE_IP_PROPERTY(정수, 정수, "%i");
DEFINE_SYSCTL_WRITE_IP_PROPERTY(uint32, uint32_t, "%" PRIu32);
