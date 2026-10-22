/* SPDX-License-Identifier: LGPL-2.1-or-later */
#निर्देशः once


#अन्तर्भाव "forward.h"
#अन्तर्भाव "stdio-util.h"

वर्ण* sysctl_normalize(वर्ण *s);
पूर्णाङ्क sysctl_read(ध्रुव वर्ण *property, वर्ण **ret);
पूर्णाङ्क sysctl_write_पूर्णम्(ध्रुव वर्ण *property, ध्रुव वर्ण *मानम्, Hashmap **shadow);
पूर्णाङ्क sysctl_writef(ध्रुव वर्ण *property, ध्रुव वर्ण *format, ...) _printf_(2, 3);
स्थिर अन्तःस्थ पूर्णाङ्क sysctl_write(ध्रुव वर्ण *property, ध्रुव वर्ण *मानम्) {
        प्रत्यागम sysctl_write_पूर्णम्(property, मानम्, रिक्त);
}
पूर्णाङ्क sysctl_write_verify(ध्रुव वर्ण *property, ध्रुव वर्ण *मानम्);

पूर्णाङ्क sysctl_read_ip_property(पूर्णाङ्क af, ध्रुव वर्ण *ifname, ध्रुव वर्ण *property, वर्ण **ret);
पूर्णाङ्क sysctl_read_ip_property_int(पूर्णाङ्क af, ध्रुव वर्ण *ifname, ध्रुव वर्ण *property, पूर्णाङ्क *ret);
पूर्णाङ्क sysctl_read_ip_property_uint32(पूर्णाङ्क af, ध्रुव वर्ण *ifname, ध्रुव वर्ण *property, uint32_t *ret);
पूर्णाङ्क sysctl_write_ip_property(पूर्णाङ्क af, ध्रुव वर्ण *ifname, ध्रुव वर्ण *property, ध्रुव वर्ण *मानम्, Hashmap **shadow);
पूर्णाङ्क sysctl_write_ip_property_boolean(पूर्णाङ्क af, ध्रुव वर्ण *ifname, ध्रुव वर्ण *property, bool मानम्, Hashmap **shadow);

स्थिर अन्तःस्थ पूर्णाङ्क sysctl_write_ip_property_tristate(पूर्णाङ्क af, ध्रुव वर्ण *ifname, ध्रुव वर्ण *property, पूर्णाङ्क मानम्, Hashmap **shadow) {
        यदि (मानम् < 0)
                प्रत्यागम 0;

        प्रत्यागम sysctl_write_ip_property_boolean(af, ifname, property, मानम्, shadow);
}

पूर्णाङ्क sysctl_write_ip_neighbor_property(पूर्णाङ्क af, ध्रुव वर्ण *ifname, ध्रुव वर्ण *property, ध्रुव वर्ण *मानम्, Hashmap **shadow);
पूर्णाङ्क sysctl_write_ip_neighbor_property_uint32(पूर्णाङ्क af, ध्रुव वर्ण *ifname, ध्रुव वर्ण *property, uint32_t मानम्, Hashmap **shadow);

#परिभाषय DEFINE_SYSCTL_WRITE_IP_PROPERTY(name, type, format)           \
        स्थिर अन्तःस्थ पूर्णाङ्क sysctl_write_ip_property_##name(पूर्णाङ्क af, ध्रुव वर्ण *ifname, ध्रुव वर्ण *property, type मानम्, Hashmap **shadow) { \
                वर्ण buf[DECIMAL_STR_MAX(type)];                        \
                xsprintf(buf, format, मानम्);                           \
                प्रत्यागम sysctl_write_ip_property(af, ifname, property, buf, shadow); \
        }

DEFINE_SYSCTL_WRITE_IP_PROPERTY(पूर्णाङ्क, पूर्णाङ्क, "%i");
DEFINE_SYSCTL_WRITE_IP_PROPERTY(uint32, uint32_t, "%" PRIu32);
