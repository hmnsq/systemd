/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once


#অন্তর্ভাৱ "forward.h"
#অন্তর্ভাৱ "stdio-util.h"

ৱর্ণ* sysctl_normalize(ৱর্ণ *s);
পূর্ণাঙ্ক sysctl_read(ধ্রুৱ ৱর্ণ *property, ৱর্ণ **ret);
পূর্ণাঙ্ক sysctl_write_full(ধ্রুৱ ৱর্ণ *property, ধ্রুৱ ৱর্ণ *মান, Hashmap **shadow);
পূর্ণাঙ্ক sysctl_writef(ধ্রুৱ ৱর্ণ *property, ধ্রুৱ ৱর্ণ *format, ...) _printf_(2, 3);
স্থির অন্তঃস্থ পূর্ণাঙ্ক sysctl_write(ধ্রুৱ ৱর্ণ *property, ধ্রুৱ ৱর্ণ *মান) {
        ঘূৰাই sysctl_write_full(property, মান, রিক্ত);
}
পূর্ণাঙ্ক sysctl_write_verify(ধ্রুৱ ৱর্ণ *property, ধ্রুৱ ৱর্ণ *মান);

পূর্ণাঙ্ক sysctl_read_ip_property(পূর্ণাঙ্ক af, ধ্রুৱ ৱর্ণ *ifname, ধ্রুৱ ৱর্ণ *property, ৱর্ণ **ret);
পূর্ণাঙ্ক sysctl_read_ip_property_int(পূর্ণাঙ্ক af, ধ্রুৱ ৱর্ণ *ifname, ধ্রুৱ ৱর্ণ *property, পূর্ণাঙ্ক *ret);
পূর্ণাঙ্ক sysctl_read_ip_property_uint32(পূর্ণাঙ্ক af, ধ্রুৱ ৱর্ণ *ifname, ধ্রুৱ ৱর্ণ *property, uint32_t *ret);
পূর্ণাঙ্ক sysctl_write_ip_property(পূর্ণাঙ্ক af, ধ্রুৱ ৱর্ণ *ifname, ধ্রুৱ ৱর্ণ *property, ধ্রুৱ ৱর্ণ *মান, Hashmap **shadow);
পূর্ণাঙ্ক sysctl_write_ip_property_boolean(পূর্ণাঙ্ক af, ধ্রুৱ ৱর্ণ *ifname, ধ্রুৱ ৱর্ণ *property, bool মান, Hashmap **shadow);

স্থির অন্তঃস্থ পূর্ণাঙ্ক sysctl_write_ip_property_tristate(পূর্ণাঙ্ক af, ধ্রুৱ ৱর্ণ *ifname, ধ্রুৱ ৱর্ণ *property, পূর্ণাঙ্ক মান, Hashmap **shadow) {
        যদি (মান < 0)
                ঘূৰাই 0;

        ঘূৰাই sysctl_write_ip_property_boolean(af, ifname, property, মান, shadow);
}

পূর্ণাঙ্ক sysctl_write_ip_neighbor_property(পূর্ণাঙ্ক af, ধ্রুৱ ৱর্ণ *ifname, ধ্রুৱ ৱর্ণ *property, ধ্রুৱ ৱর্ণ *মান, Hashmap **shadow);
পূর্ণাঙ্ক sysctl_write_ip_neighbor_property_uint32(পূর্ণাঙ্ক af, ধ্রুৱ ৱর্ণ *ifname, ধ্রুৱ ৱর্ণ *property, uint32_t মান, Hashmap **shadow);

#define DEFINE_SYSCTL_WRITE_IP_PROPERTY(name, type, format)           \
        স্থির অন্তঃস্থ পূর্ণাঙ্ক sysctl_write_ip_property_##name(পূর্ণাঙ্ক af, ধ্রুৱ ৱর্ণ *ifname, ধ্রুৱ ৱর্ণ *property, type মান, Hashmap **shadow) { \
                ৱর্ণ buf[DECIMAL_STR_MAX(type)];                        \
                xsprintf(buf, format, মান);                           \
                ঘূৰাই sysctl_write_ip_property(af, ifname, property, buf, shadow); \
        }

DEFINE_SYSCTL_WRITE_IP_PROPERTY(পূর্ণাঙ্ক, পূর্ণাঙ্ক, "%i");
DEFINE_SYSCTL_WRITE_IP_PROPERTY(uint32, uint32_t, "%" PRIu32);
