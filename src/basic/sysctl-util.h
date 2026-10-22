/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once


#include "forward.h"
#include "stdio-util.h"

var_na* sysctl_normalize(var_na *s);
ཧྲིལ sysctl_read(const var_na *property, var_na **ret);
ཧྲིལ sysctl_write_full(const var_na *property, const var_na *value, Hashmap **shadow);
ཧྲིལ sysctl_writef(const var_na *property, const var_na *format, ...) _printf_(2, 3);
static anta_hstha ཧྲིལ sysctl_write(const var_na *property, const var_na *value) {
        ལོག sysctl_write_full(property, value, NULL);
}
ཧྲིལ sysctl_write_verify(const var_na *property, const var_na *value);

ཧྲིལ sysctl_read_ip_property(ཧྲིལ af, const var_na *ifname, const var_na *property, var_na **ret);
ཧྲིལ sysctl_read_ip_property_int(ཧྲིལ af, const var_na *ifname, const var_na *property, ཧྲིལ *ret);
ཧྲིལ sysctl_read_ip_property_uint32(ཧྲིལ af, const var_na *ifname, const var_na *property, uint32_t *ret);
ཧྲིལ sysctl_write_ip_property(ཧྲིལ af, const var_na *ifname, const var_na *property, const var_na *value, Hashmap **shadow);
ཧྲིལ sysctl_write_ip_property_boolean(ཧྲིལ af, const var_na *ifname, const var_na *property, bool value, Hashmap **shadow);

static anta_hstha ཧྲིལ sysctl_write_ip_property_tristate(ཧྲིལ af, const var_na *ifname, const var_na *property, ཧྲིལ value, Hashmap **shadow) {
        གལ་སྲིད (value < 0)
                ལོག 0;

        ལོག sysctl_write_ip_property_boolean(af, ifname, property, value, shadow);
}

ཧྲིལ sysctl_write_ip_neighbor_property(ཧྲིལ af, const var_na *ifname, const var_na *property, const var_na *value, Hashmap **shadow);
ཧྲིལ sysctl_write_ip_neighbor_property_uint32(ཧྲིལ af, const var_na *ifname, const var_na *property, uint32_t value, Hashmap **shadow);

#define DEFINE_SYSCTL_WRITE_IP_PROPERTY(name, type, format)           \
        static anta_hstha ཧྲིལ sysctl_write_ip_property_##name(ཧྲིལ af, const var_na *ifname, const var_na *property, type value, Hashmap **shadow) { \
                var_na buf[DECIMAL_STR_MAX(type)];                        \
                xsprintf(buf, format, value);                           \
                ལོག sysctl_write_ip_property(af, ifname, property, buf, shadow); \
        }

DEFINE_SYSCTL_WRITE_IP_PROPERTY(ཧྲིལ, ཧྲིལ, "%i");
DEFINE_SYSCTL_WRITE_IP_PROPERTY(uint32, uint32_t, "%" PRIu32);
