/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once


#include "forward.h"
#include "stdio-util.h"

char* sysctl_normalize(char *s);
цео sysctl_read(const char *property, char **ret);
цео sysctl_write_full(const char *property, const char *value, Hashmap **shadow);
цео sysctl_writef(const char *property, const char *format, ...) _printf_(2, 3);
static inline цео sysctl_write(const char *property, const char *value) {
        врати sysctl_write_full(property, value, NULL);
}
цео sysctl_write_verify(const char *property, const char *value);

цео sysctl_read_ip_property(цео af, const char *ifname, const char *property, char **ret);
цео sysctl_read_ip_property_int(цео af, const char *ifname, const char *property, цео *ret);
цео sysctl_read_ip_property_uint32(цео af, const char *ifname, const char *property, uint32_t *ret);
цео sysctl_write_ip_property(цео af, const char *ifname, const char *property, const char *value, Hashmap **shadow);
цео sysctl_write_ip_property_boolean(цео af, const char *ifname, const char *property, bool value, Hashmap **shadow);

static inline цео sysctl_write_ip_property_tristate(цео af, const char *ifname, const char *property, цео value, Hashmap **shadow) {
        ако (value < 0)
                врати 0;

        врати sysctl_write_ip_property_boolean(af, ifname, property, value, shadow);
}

цео sysctl_write_ip_neighbor_property(цео af, const char *ifname, const char *property, const char *value, Hashmap **shadow);
цео sysctl_write_ip_neighbor_property_uint32(цео af, const char *ifname, const char *property, uint32_t value, Hashmap **shadow);

#define DEFINE_SYSCTL_WRITE_IP_PROPERTY(name, type, format)           \
        static inline цео sysctl_write_ip_property_##name(цео af, const char *ifname, const char *property, type value, Hashmap **shadow) { \
                char buf[DECIMAL_STR_MAX(type)];                        \
                xsprintf(buf, format, value);                           \
                врати sysctl_write_ip_property(af, ifname, property, buf, shadow); \
        }

DEFINE_SYSCTL_WRITE_IP_PROPERTY(цео, цео, "%i");
DEFINE_SYSCTL_WRITE_IP_PROPERTY(uint32, uint32_t, "%" PRIu32);
