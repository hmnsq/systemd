/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once


#अन्तर्भाव "forward.h"
#अन्तर्भाव "stdio-util.h"

वर्ण* sysctl_normalize(वर्ण *s);
पूर्णाङ्क sysctl_read(ध्रुव वर्ण *property, वर्ण **ret);
पूर्णाङ्क sysctl_write_full(ध्रुव वर्ण *property, ध्रुव वर्ण *मूल्य, Hashmap **shadow);
पूर्णाङ्क sysctl_writef(ध्रुव वर्ण *property, ध्रुव वर्ण *format, ...) _printf_(2, 3);
स्थिर अन्तःस्थ पूर्णाङ्क sysctl_write(ध्रुव वर्ण *property, ध्रुव वर्ण *मूल्य) {
        प्रत्यागम sysctl_write_full(property, मूल्य, रिक्त);
}
पूर्णाङ्क sysctl_write_verify(ध्रुव वर्ण *property, ध्रुव वर्ण *मूल्य);

पूर्णाङ्क sysctl_read_ip_property(पूर्णाङ्क af, ध्रुव वर्ण *ifname, ध्रुव वर्ण *property, वर्ण **ret);
पूर्णाङ्क sysctl_read_ip_property_int(पूर्णाङ्क af, ध्रुव वर्ण *ifname, ध्रुव वर्ण *property, पूर्णाङ्क *ret);
पूर्णाङ्क sysctl_read_ip_property_uint32(पूर्णाङ्क af, ध्रुव वर्ण *ifname, ध्रुव वर्ण *property, uint32_t *ret);
पूर्णाङ्क sysctl_write_ip_property(पूर्णाङ्क af, ध्रुव वर्ण *ifname, ध्रुव वर्ण *property, ध्रुव वर्ण *मूल्य, Hashmap **shadow);
पूर्णाङ्क sysctl_write_ip_property_boolean(पूर्णाङ्क af, ध्रुव वर्ण *ifname, ध्रुव वर्ण *property, bool मूल्य, Hashmap **shadow);

स्थिर अन्तःस्थ पूर्णाङ्क sysctl_write_ip_property_tristate(पूर्णाङ्क af, ध्रुव वर्ण *ifname, ध्रुव वर्ण *property, पूर्णाङ्क मूल्य, Hashmap **shadow) {
        यदि (मूल्य < 0)
                प्रत्यागम 0;

        प्रत्यागम sysctl_write_ip_property_boolean(af, ifname, property, मूल्य, shadow);
}

पूर्णाङ्क sysctl_write_ip_neighbor_property(पूर्णाङ्क af, ध्रुव वर्ण *ifname, ध्रुव वर्ण *property, ध्रुव वर्ण *मूल्य, Hashmap **shadow);
पूर्णाङ्क sysctl_write_ip_neighbor_property_uint32(पूर्णाङ्क af, ध्रुव वर्ण *ifname, ध्रुव वर्ण *property, uint32_t मूल्य, Hashmap **shadow);

#define DEFINE_SYSCTL_WRITE_IP_PROPERTY(name, type, format)           \
        स्थिर अन्तःस्थ पूर्णाङ्क sysctl_write_ip_property_##name(पूर्णाङ्क af, ध्रुव वर्ण *ifname, ध्रुव वर्ण *property, type मूल्य, Hashmap **shadow) { \
                वर्ण buf[DECIMAL_STR_MAX(type)];                        \
                xsprintf(buf, format, मूल्य);                           \
                प्रत्यागम sysctl_write_ip_property(af, ifname, property, buf, shadow); \
        }

DEFINE_SYSCTL_WRITE_IP_PROPERTY(पूर्णाङ्क, पूर्णाङ्क, "%i");
DEFINE_SYSCTL_WRITE_IP_PROPERTY(uint32, uint32_t, "%" PRIu32);
