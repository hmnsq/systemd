/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once


#अन्तर्भाव "forward.h"
#अन्तर्भाव "stdio-util.h"

वर्ण* sysctl_normalize(वर्ण *s);
पूर्णाङ्क sysctl_read(अचर वर्ण *property, वर्ण **ret);
पूर्णाङ्क sysctl_write_full(अचर वर्ण *property, अचर वर्ण *मान, Hashmap **shadow);
पूर्णाङ्क sysctl_writef(अचर वर्ण *property, अचर वर्ण *format, ...) _printf_(2, 3);
स्थिर अन्तःस्थ पूर्णाङ्क sysctl_write(अचर वर्ण *property, अचर वर्ण *मान) {
        फर्काऊ sysctl_write_full(property, मान, रिक्त);
}
पूर्णाङ्क sysctl_write_verify(अचर वर्ण *property, अचर वर्ण *मान);

पूर्णाङ्क sysctl_read_ip_property(पूर्णाङ्क af, अचर वर्ण *ifname, अचर वर्ण *property, वर्ण **ret);
पूर्णाङ्क sysctl_read_ip_property_int(पूर्णाङ्क af, अचर वर्ण *ifname, अचर वर्ण *property, पूर्णाङ्क *ret);
पूर्णाङ्क sysctl_read_ip_property_uint32(पूर्णाङ्क af, अचर वर्ण *ifname, अचर वर्ण *property, uint32_t *ret);
पूर्णाङ्क sysctl_write_ip_property(पूर्णाङ्क af, अचर वर्ण *ifname, अचर वर्ण *property, अचर वर्ण *मान, Hashmap **shadow);
पूर्णाङ्क sysctl_write_ip_property_boolean(पूर्णाङ्क af, अचर वर्ण *ifname, अचर वर्ण *property, bool मान, Hashmap **shadow);

स्थिर अन्तःस्थ पूर्णाङ्क sysctl_write_ip_property_tristate(पूर्णाङ्क af, अचर वर्ण *ifname, अचर वर्ण *property, पूर्णाङ्क मान, Hashmap **shadow) {
        यदि (मान < 0)
                फर्काऊ 0;

        फर्काऊ sysctl_write_ip_property_boolean(af, ifname, property, मान, shadow);
}

पूर्णाङ्क sysctl_write_ip_neighbor_property(पूर्णाङ्क af, अचर वर्ण *ifname, अचर वर्ण *property, अचर वर्ण *मान, Hashmap **shadow);
पूर्णाङ्क sysctl_write_ip_neighbor_property_uint32(पूर्णाङ्क af, अचर वर्ण *ifname, अचर वर्ण *property, uint32_t मान, Hashmap **shadow);

#define DEFINE_SYSCTL_WRITE_IP_PROPERTY(name, type, format)           \
        स्थिर अन्तःस्थ पूर्णाङ्क sysctl_write_ip_property_##name(पूर्णाङ्क af, अचर वर्ण *ifname, अचर वर्ण *property, type मान, Hashmap **shadow) { \
                वर्ण buf[DECIMAL_STR_MAX(type)];                        \
                xsprintf(buf, format, मान);                           \
                फर्काऊ sysctl_write_ip_property(af, ifname, property, buf, shadow); \
        }

DEFINE_SYSCTL_WRITE_IP_PROPERTY(पूर्णाङ्क, पूर्णाङ्क, "%i");
DEFINE_SYSCTL_WRITE_IP_PROPERTY(uint32, uint32_t, "%" PRIu32);
