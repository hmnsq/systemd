/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once


#অন্তর্ভাব "forward.h"
#অন্তর্ভাব "stdio-util.h"

অক্ষর* sysctl_normalize(অক্ষর *s);
পূর্ণসংখ্যা sysctl_read(ধ্রুবক অক্ষর *property, অক্ষর **ret);
পূর্ণসংখ্যা sysctl_write_full(ধ্রুবক অক্ষর *property, ধ্রুবক অক্ষর *মান, Hashmap **shadow);
পূর্ণসংখ্যা sysctl_writef(ধ্রুবক অক্ষর *property, ধ্রুবক অক্ষর *format, ...) _printf_(2, 3);
স্থির অন্তঃরেখ পূর্ণসংখ্যা sysctl_write(ধ্রুবক অক্ষর *property, ধ্রুবক অক্ষর *মান) {
        ফেরত sysctl_write_full(property, মান, ফাঁকা);
}
পূর্ণসংখ্যা sysctl_write_verify(ধ্রুবক অক্ষর *property, ধ্রুবক অক্ষর *মান);

পূর্ণসংখ্যা sysctl_read_ip_property(পূর্ণসংখ্যা af, ধ্রুবক অক্ষর *ifname, ধ্রুবক অক্ষর *property, অক্ষর **ret);
পূর্ণসংখ্যা sysctl_read_ip_property_int(পূর্ণসংখ্যা af, ধ্রুবক অক্ষর *ifname, ধ্রুবক অক্ষর *property, পূর্ণসংখ্যা *ret);
পূর্ণসংখ্যা sysctl_read_ip_property_uint32(পূর্ণসংখ্যা af, ধ্রুবক অক্ষর *ifname, ধ্রুবক অক্ষর *property, uint32_t *ret);
পূর্ণসংখ্যা sysctl_write_ip_property(পূর্ণসংখ্যা af, ধ্রুবক অক্ষর *ifname, ধ্রুবক অক্ষর *property, ধ্রুবক অক্ষর *মান, Hashmap **shadow);
পূর্ণসংখ্যা sysctl_write_ip_property_boolean(পূর্ণসংখ্যা af, ধ্রুবক অক্ষর *ifname, ধ্রুবক অক্ষর *property, bool মান, Hashmap **shadow);

স্থির অন্তঃরেখ পূর্ণসংখ্যা sysctl_write_ip_property_tristate(পূর্ণসংখ্যা af, ধ্রুবক অক্ষর *ifname, ধ্রুবক অক্ষর *property, পূর্ণসংখ্যা মান, Hashmap **shadow) {
        যদি (মান < 0)
                ফেরত 0;

        ফেরত sysctl_write_ip_property_boolean(af, ifname, property, মান, shadow);
}

পূর্ণসংখ্যা sysctl_write_ip_neighbor_property(পূর্ণসংখ্যা af, ধ্রুবক অক্ষর *ifname, ধ্রুবক অক্ষর *property, ধ্রুবক অক্ষর *মান, Hashmap **shadow);
পূর্ণসংখ্যা sysctl_write_ip_neighbor_property_uint32(পূর্ণসংখ্যা af, ধ্রুবক অক্ষর *ifname, ধ্রুবক অক্ষর *property, uint32_t মান, Hashmap **shadow);

#define DEFINE_SYSCTL_WRITE_IP_PROPERTY(name, type, format)           \
        স্থির অন্তঃরেখ পূর্ণসংখ্যা sysctl_write_ip_property_##name(পূর্ণসংখ্যা af, ধ্রুবক অক্ষর *ifname, ধ্রুবক অক্ষর *property, type মান, Hashmap **shadow) { \
                অক্ষর buf[DECIMAL_STR_MAX(type)];                        \
                xsprintf(buf, format, মান);                           \
                ফেরত sysctl_write_ip_property(af, ifname, property, buf, shadow); \
        }

DEFINE_SYSCTL_WRITE_IP_PROPERTY(পূর্ণসংখ্যা, পূর্ণসংখ্যা, "%i");
DEFINE_SYSCTL_WRITE_IP_PROPERTY(uint32, uint32_t, "%" PRIu32);
