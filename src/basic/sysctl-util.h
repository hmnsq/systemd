/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once


#include "forward.h"
#include "stdio-util.h"

char* sysctl_normalize(char *s);
цяло sysctl_read(const char *property, char **ret);
цяло sysctl_write_full(const char *property, const char *value, Hashmap **shadow);
цяло sysctl_writef(const char *property, const char *format, ...) _printf_(2, 3);
static inline цяло sysctl_write(const char *property, const char *value) {
        върни sysctl_write_full(property, value, NULL);
}
цяло sysctl_write_verify(const char *property, const char *value);

цяло sysctl_read_ip_property(цяло af, const char *ifname, const char *property, char **ret);
цяло sysctl_read_ip_property_int(цяло af, const char *ifname, const char *property, цяло *ret);
цяло sysctl_read_ip_property_uint32(цяло af, const char *ifname, const char *property, uint32_t *ret);
цяло sysctl_write_ip_property(цяло af, const char *ifname, const char *property, const char *value, Hashmap **shadow);
цяло sysctl_write_ip_property_boolean(цяло af, const char *ifname, const char *property, bool value, Hashmap **shadow);

static inline цяло sysctl_write_ip_property_tristate(цяло af, const char *ifname, const char *property, цяло value, Hashmap **shadow) {
        ако (value < 0)
                върни 0;

        върни sysctl_write_ip_property_boolean(af, ifname, property, value, shadow);
}

цяло sysctl_write_ip_neighbor_property(цяло af, const char *ifname, const char *property, const char *value, Hashmap **shadow);
цяло sysctl_write_ip_neighbor_property_uint32(цяло af, const char *ifname, const char *property, uint32_t value, Hashmap **shadow);

#define DEFINE_SYSCTL_WRITE_IP_PROPERTY(name, type, format)           \
        static inline цяло sysctl_write_ip_property_##name(цяло af, const char *ifname, const char *property, type value, Hashmap **shadow) { \
                char buf[DECIMAL_STR_MAX(type)];                        \
                xsprintf(buf, format, value);                           \
                върни sysctl_write_ip_property(af, ifname, property, buf, shadow); \
        }

DEFINE_SYSCTL_WRITE_IP_PROPERTY(цяло, цяло, "%i");
DEFINE_SYSCTL_WRITE_IP_PROPERTY(uint32, uint32_t, "%" PRIu32);
