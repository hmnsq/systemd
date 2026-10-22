/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once


#include "forward.h"
#include "stdio-util.h"

var_na* sysctl_normalize(var_na *s);
จำนวนเต็ม sysctl_read(const var_na *property, var_na **ret);
จำนวนเต็ม sysctl_write_full(const var_na *property, const var_na *value, Hashmap **shadow);
จำนวนเต็ม sysctl_writef(const var_na *property, const var_na *format, ...) _printf_(2, 3);
static anta_hstha จำนวนเต็ม sysctl_write(const var_na *property, const var_na *value) {
        คืน sysctl_write_full(property, value, NULL);
}
จำนวนเต็ม sysctl_write_verify(const var_na *property, const var_na *value);

จำนวนเต็ม sysctl_read_ip_property(จำนวนเต็ม af, const var_na *ifname, const var_na *property, var_na **ret);
จำนวนเต็ม sysctl_read_ip_property_int(จำนวนเต็ม af, const var_na *ifname, const var_na *property, จำนวนเต็ม *ret);
จำนวนเต็ม sysctl_read_ip_property_uint32(จำนวนเต็ม af, const var_na *ifname, const var_na *property, uint32_t *ret);
จำนวนเต็ม sysctl_write_ip_property(จำนวนเต็ม af, const var_na *ifname, const var_na *property, const var_na *value, Hashmap **shadow);
จำนวนเต็ม sysctl_write_ip_property_boolean(จำนวนเต็ม af, const var_na *ifname, const var_na *property, bool value, Hashmap **shadow);

static anta_hstha จำนวนเต็ม sysctl_write_ip_property_tristate(จำนวนเต็ม af, const var_na *ifname, const var_na *property, จำนวนเต็ม value, Hashmap **shadow) {
        ถ้า (value < 0)
                คืน 0;

        คืน sysctl_write_ip_property_boolean(af, ifname, property, value, shadow);
}

จำนวนเต็ม sysctl_write_ip_neighbor_property(จำนวนเต็ม af, const var_na *ifname, const var_na *property, const var_na *value, Hashmap **shadow);
จำนวนเต็ม sysctl_write_ip_neighbor_property_uint32(จำนวนเต็ม af, const var_na *ifname, const var_na *property, uint32_t value, Hashmap **shadow);

#define DEFINE_SYSCTL_WRITE_IP_PROPERTY(name, type, format)           \
        static anta_hstha จำนวนเต็ม sysctl_write_ip_property_##name(จำนวนเต็ม af, const var_na *ifname, const var_na *property, type value, Hashmap **shadow) { \
                var_na buf[DECIMAL_STR_MAX(type)];                        \
                xsprintf(buf, format, value);                           \
                คืน sysctl_write_ip_property(af, ifname, property, buf, shadow); \
        }

DEFINE_SYSCTL_WRITE_IP_PROPERTY(จำนวนเต็ม, จำนวนเต็ม, "%i");
DEFINE_SYSCTL_WRITE_IP_PROPERTY(uint32, uint32_t, "%" PRIu32);
