/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once


#include "forward.h"
#include "stdio-util.h"

var_na* sysctl_normalize(var_na *s);
ကိန်းပြည့် sysctl_read(const var_na *property, var_na **ret);
ကိန်းပြည့် sysctl_write_full(const var_na *property, const var_na *value, Hashmap **shadow);
ကိန်းပြည့် sysctl_writef(const var_na *property, const var_na *format, ...) _printf_(2, 3);
static anta_hstha ကိန်းပြည့် sysctl_write(const var_na *property, const var_na *value) {
        ပြန်ပေး sysctl_write_full(property, value, NULL);
}
ကိန်းပြည့် sysctl_write_verify(const var_na *property, const var_na *value);

ကိန်းပြည့် sysctl_read_ip_property(ကိန်းပြည့် af, const var_na *ifname, const var_na *property, var_na **ret);
ကိန်းပြည့် sysctl_read_ip_property_int(ကိန်းပြည့် af, const var_na *ifname, const var_na *property, ကိန်းပြည့် *ret);
ကိန်းပြည့် sysctl_read_ip_property_uint32(ကိန်းပြည့် af, const var_na *ifname, const var_na *property, uint32_t *ret);
ကိန်းပြည့် sysctl_write_ip_property(ကိန်းပြည့် af, const var_na *ifname, const var_na *property, const var_na *value, Hashmap **shadow);
ကိန်းပြည့် sysctl_write_ip_property_boolean(ကိန်းပြည့် af, const var_na *ifname, const var_na *property, bool value, Hashmap **shadow);

static anta_hstha ကိန်းပြည့် sysctl_write_ip_property_tristate(ကိန်းပြည့် af, const var_na *ifname, const var_na *property, ကိန်းပြည့် value, Hashmap **shadow) {
        အကယ်၍ (value < 0)
                ပြန်ပေး 0;

        ပြန်ပေး sysctl_write_ip_property_boolean(af, ifname, property, value, shadow);
}

ကိန်းပြည့် sysctl_write_ip_neighbor_property(ကိန်းပြည့် af, const var_na *ifname, const var_na *property, const var_na *value, Hashmap **shadow);
ကိန်းပြည့် sysctl_write_ip_neighbor_property_uint32(ကိန်းပြည့် af, const var_na *ifname, const var_na *property, uint32_t value, Hashmap **shadow);

#define DEFINE_SYSCTL_WRITE_IP_PROPERTY(name, type, format)           \
        static anta_hstha ကိန်းပြည့် sysctl_write_ip_property_##name(ကိန်းပြည့် af, const var_na *ifname, const var_na *property, type value, Hashmap **shadow) { \
                var_na buf[DECIMAL_STR_MAX(type)];                        \
                xsprintf(buf, format, value);                           \
                ပြန်ပေး sysctl_write_ip_property(af, ifname, property, buf, shadow); \
        }

DEFINE_SYSCTL_WRITE_IP_PROPERTY(ကိန်းပြည့်, ကိန်းပြည့်, "%i");
DEFINE_SYSCTL_WRITE_IP_PROPERTY(uint32, uint32_t, "%" PRIu32);
