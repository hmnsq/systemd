/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once


#अन्तर्भाव "forward.h"
#अन्तर्भाव "stdio-util.h"

अक्षर* sysctl_normalize(अक्षर *s);
पूर्णांक sysctl_read(स्थिरांक अक्षर *property, अक्षर **ret);
पूर्णांक sysctl_write_full(स्थिरांक अक्षर *property, स्थिरांक अक्षर *मूल्य, Hashmap **shadow);
पूर्णांक sysctl_writef(स्थिरांक अक्षर *property, स्थिरांक अक्षर *format, ...) _printf_(2, 3);
स्थिर ओळीत पूर्णांक sysctl_write(स्थिरांक अक्षर *property, स्थिरांक अक्षर *मूल्य) {
        परत sysctl_write_full(property, मूल्य, रिक्त);
}
पूर्णांक sysctl_write_verify(स्थिरांक अक्षर *property, स्थिरांक अक्षर *मूल्य);

पूर्णांक sysctl_read_ip_property(पूर्णांक af, स्थिरांक अक्षर *ifname, स्थिरांक अक्षर *property, अक्षर **ret);
पूर्णांक sysctl_read_ip_property_int(पूर्णांक af, स्थिरांक अक्षर *ifname, स्थिरांक अक्षर *property, पूर्णांक *ret);
पूर्णांक sysctl_read_ip_property_uint32(पूर्णांक af, स्थिरांक अक्षर *ifname, स्थिरांक अक्षर *property, uint32_t *ret);
पूर्णांक sysctl_write_ip_property(पूर्णांक af, स्थिरांक अक्षर *ifname, स्थिरांक अक्षर *property, स्थिरांक अक्षर *मूल्य, Hashmap **shadow);
पूर्णांक sysctl_write_ip_property_boolean(पूर्णांक af, स्थिरांक अक्षर *ifname, स्थिरांक अक्षर *property, bool मूल्य, Hashmap **shadow);

स्थिर ओळीत पूर्णांक sysctl_write_ip_property_tristate(पूर्णांक af, स्थिरांक अक्षर *ifname, स्थिरांक अक्षर *property, पूर्णांक मूल्य, Hashmap **shadow) {
        जर (मूल्य < 0)
                परत 0;

        परत sysctl_write_ip_property_boolean(af, ifname, property, मूल्य, shadow);
}

पूर्णांक sysctl_write_ip_neighbor_property(पूर्णांक af, स्थिरांक अक्षर *ifname, स्थिरांक अक्षर *property, स्थिरांक अक्षर *मूल्य, Hashmap **shadow);
पूर्णांक sysctl_write_ip_neighbor_property_uint32(पूर्णांक af, स्थिरांक अक्षर *ifname, स्थिरांक अक्षर *property, uint32_t मूल्य, Hashmap **shadow);

#define DEFINE_SYSCTL_WRITE_IP_PROPERTY(name, type, format)           \
        स्थिर ओळीत पूर्णांक sysctl_write_ip_property_##name(पूर्णांक af, स्थिरांक अक्षर *ifname, स्थिरांक अक्षर *property, type मूल्य, Hashmap **shadow) { \
                अक्षर buf[DECIMAL_STR_MAX(type)];                        \
                xsprintf(buf, format, मूल्य);                           \
                परत sysctl_write_ip_property(af, ifname, property, buf, shadow); \
        }

DEFINE_SYSCTL_WRITE_IP_PROPERTY(पूर्णांक, पूर्णांक, "%i");
DEFINE_SYSCTL_WRITE_IP_PROPERTY(uint32, uint32_t, "%" PRIu32);
