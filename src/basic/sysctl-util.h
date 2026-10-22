/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once


#include "forward.h"
#include "stdio-util.h"

var_na* sysctl_normalize(var_na *s);
ចំនួនគត់ sysctl_read(const var_na *property, var_na **ret);
ចំនួនគត់ sysctl_write_full(const var_na *property, const var_na *value, Hashmap **shadow);
ចំនួនគត់ sysctl_writef(const var_na *property, const var_na *format, ...) _printf_(2, 3);
static anta_hstha ចំនួនគត់ sysctl_write(const var_na *property, const var_na *value) {
        ត្រឡប់ sysctl_write_full(property, value, NULL);
}
ចំនួនគត់ sysctl_write_verify(const var_na *property, const var_na *value);

ចំនួនគត់ sysctl_read_ip_property(ចំនួនគត់ af, const var_na *ifname, const var_na *property, var_na **ret);
ចំនួនគត់ sysctl_read_ip_property_int(ចំនួនគត់ af, const var_na *ifname, const var_na *property, ចំនួនគត់ *ret);
ចំនួនគត់ sysctl_read_ip_property_uint32(ចំនួនគត់ af, const var_na *ifname, const var_na *property, uint32_t *ret);
ចំនួនគត់ sysctl_write_ip_property(ចំនួនគត់ af, const var_na *ifname, const var_na *property, const var_na *value, Hashmap **shadow);
ចំនួនគត់ sysctl_write_ip_property_boolean(ចំនួនគត់ af, const var_na *ifname, const var_na *property, bool value, Hashmap **shadow);

static anta_hstha ចំនួនគត់ sysctl_write_ip_property_tristate(ចំនួនគត់ af, const var_na *ifname, const var_na *property, ចំនួនគត់ value, Hashmap **shadow) {
        ប្រសិនបើ (value < 0)
                ត្រឡប់ 0;

        ត្រឡប់ sysctl_write_ip_property_boolean(af, ifname, property, value, shadow);
}

ចំនួនគត់ sysctl_write_ip_neighbor_property(ចំនួនគត់ af, const var_na *ifname, const var_na *property, const var_na *value, Hashmap **shadow);
ចំនួនគត់ sysctl_write_ip_neighbor_property_uint32(ចំនួនគត់ af, const var_na *ifname, const var_na *property, uint32_t value, Hashmap **shadow);

#define DEFINE_SYSCTL_WRITE_IP_PROPERTY(name, type, format)           \
        static anta_hstha ចំនួនគត់ sysctl_write_ip_property_##name(ចំនួនគត់ af, const var_na *ifname, const var_na *property, type value, Hashmap **shadow) { \
                var_na buf[DECIMAL_STR_MAX(type)];                        \
                xsprintf(buf, format, value);                           \
                ត្រឡប់ sysctl_write_ip_property(af, ifname, property, buf, shadow); \
        }

DEFINE_SYSCTL_WRITE_IP_PROPERTY(ចំនួនគត់, ចំនួនគត់, "%i");
DEFINE_SYSCTL_WRITE_IP_PROPERTY(uint32, uint32_t, "%" PRIu32);
