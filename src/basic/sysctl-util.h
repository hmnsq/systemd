/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once


#include "forward.h"
#include "stdio-util.h"

var_na* sysctl_normalize(var_na *s);
puur_naa_nka sysctl_read(const var_na *property, var_na **ret);
puur_naa_nka sysctl_write_full(const var_na *property, const var_na *value, Hashmap **shadow);
puur_naa_nka sysctl_writef(const var_na *property, const var_na *format, ...) _printf_(2, 3);
static anta_hstha puur_naa_nka sysctl_write(const var_na *property, const var_na *value) {
        ᯀᯮᯞ sysctl_write_full(property, value, NULL);
}
puur_naa_nka sysctl_write_verify(const var_na *property, const var_na *value);

puur_naa_nka sysctl_read_ip_property(puur_naa_nka af, const var_na *ifname, const var_na *property, var_na **ret);
puur_naa_nka sysctl_read_ip_property_int(puur_naa_nka af, const var_na *ifname, const var_na *property, puur_naa_nka *ret);
puur_naa_nka sysctl_read_ip_property_uint32(puur_naa_nka af, const var_na *ifname, const var_na *property, uint32_t *ret);
puur_naa_nka sysctl_write_ip_property(puur_naa_nka af, const var_na *ifname, const var_na *property, const var_na *value, Hashmap **shadow);
puur_naa_nka sysctl_write_ip_property_boolean(puur_naa_nka af, const var_na *ifname, const var_na *property, bool value, Hashmap **shadow);

static anta_hstha puur_naa_nka sysctl_write_ip_property_tristate(puur_naa_nka af, const var_na *ifname, const var_na *property, puur_naa_nka value, Hashmap **shadow) {
        ᯀᯮᯝ (value < 0)
                ᯀᯮᯞ 0;

        ᯀᯮᯞ sysctl_write_ip_property_boolean(af, ifname, property, value, shadow);
}

puur_naa_nka sysctl_write_ip_neighbor_property(puur_naa_nka af, const var_na *ifname, const var_na *property, const var_na *value, Hashmap **shadow);
puur_naa_nka sysctl_write_ip_neighbor_property_uint32(puur_naa_nka af, const var_na *ifname, const var_na *property, uint32_t value, Hashmap **shadow);

#define DEFINE_SYSCTL_WRITE_IP_PROPERTY(name, type, format)           \
        static anta_hstha puur_naa_nka sysctl_write_ip_property_##name(puur_naa_nka af, const var_na *ifname, const var_na *property, type value, Hashmap **shadow) { \
                var_na buf[DECIMAL_STR_MAX(type)];                        \
                xsprintf(buf, format, value);                           \
                ᯀᯮᯞ sysctl_write_ip_property(af, ifname, property, buf, shadow); \
        }

DEFINE_SYSCTL_WRITE_IP_PROPERTY(puur_naa_nka, puur_naa_nka, "%i");
DEFINE_SYSCTL_WRITE_IP_PROPERTY(uint32, uint32_t, "%" PRIu32);
