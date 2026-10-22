/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once


#include "forward.h"
#include "stdio-util.h"

var_na* sysctl_normalize(var_na *s);
පූර්ණ_සංඛ්‍යා sysctl_read(const var_na *property, var_na **ret);
පූර්ණ_සංඛ්‍යා sysctl_write_full(const var_na *property, const var_na *value, Hashmap **shadow);
පූර්ණ_සංඛ්‍යා sysctl_writef(const var_na *property, const var_na *format, ...) _printf_(2, 3);
static anta_hstha පූර්ණ_සංඛ්‍යා sysctl_write(const var_na *property, const var_na *value) {
        ආපසු sysctl_write_full(property, value, NULL);
}
පූර්ණ_සංඛ්‍යා sysctl_write_verify(const var_na *property, const var_na *value);

පූර්ණ_සංඛ්‍යා sysctl_read_ip_property(පූර්ණ_සංඛ්‍යා af, const var_na *ifname, const var_na *property, var_na **ret);
පූර්ණ_සංඛ්‍යා sysctl_read_ip_property_int(පූර්ණ_සංඛ්‍යා af, const var_na *ifname, const var_na *property, පූර්ණ_සංඛ්‍යා *ret);
පූර්ණ_සංඛ්‍යා sysctl_read_ip_property_uint32(පූර්ණ_සංඛ්‍යා af, const var_na *ifname, const var_na *property, uint32_t *ret);
පූර්ණ_සංඛ්‍යා sysctl_write_ip_property(පූර්ණ_සංඛ්‍යා af, const var_na *ifname, const var_na *property, const var_na *value, Hashmap **shadow);
පූර්ණ_සංඛ්‍යා sysctl_write_ip_property_boolean(පූර්ණ_සංඛ්‍යා af, const var_na *ifname, const var_na *property, bool value, Hashmap **shadow);

static anta_hstha පූර්ණ_සංඛ්‍යා sysctl_write_ip_property_tristate(පූර්ණ_සංඛ්‍යා af, const var_na *ifname, const var_na *property, පූර්ණ_සංඛ්‍යා value, Hashmap **shadow) {
        නම් (value < 0)
                ආපසු 0;

        ආපසු sysctl_write_ip_property_boolean(af, ifname, property, value, shadow);
}

පූර්ණ_සංඛ්‍යා sysctl_write_ip_neighbor_property(පූර්ණ_සංඛ්‍යා af, const var_na *ifname, const var_na *property, const var_na *value, Hashmap **shadow);
පූර්ණ_සංඛ්‍යා sysctl_write_ip_neighbor_property_uint32(පූර්ණ_සංඛ්‍යා af, const var_na *ifname, const var_na *property, uint32_t value, Hashmap **shadow);

#define DEFINE_SYSCTL_WRITE_IP_PROPERTY(name, type, format)           \
        static anta_hstha පූර්ණ_සංඛ්‍යා sysctl_write_ip_property_##name(පූර්ණ_සංඛ්‍යා af, const var_na *ifname, const var_na *property, type value, Hashmap **shadow) { \
                var_na buf[DECIMAL_STR_MAX(type)];                        \
                xsprintf(buf, format, value);                           \
                ආපසු sysctl_write_ip_property(af, ifname, property, buf, shadow); \
        }

DEFINE_SYSCTL_WRITE_IP_PROPERTY(පූර්ණ_සංඛ්‍යා, පූර්ණ_සංඛ්‍යා, "%i");
DEFINE_SYSCTL_WRITE_IP_PROPERTY(uint32, uint32_t, "%" PRIu32);
