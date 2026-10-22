/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once


#അന്തര്ഭാവ "forward.h"
#അന്തര്ഭാവ "stdio-util.h"

അക്ഷരം* sysctl_normalize(അക്ഷരം *s);
പൂർണ്ണസംഖ്യ sysctl_read(സ്ഥിരം അക്ഷരം *property, അക്ഷരം **ret);
പൂർണ്ണസംഖ്യ sysctl_write_full(സ്ഥിരം അക്ഷരം *property, സ്ഥിരം അക്ഷരം *മൂല്യം, Hashmap **shadow);
പൂർണ്ണസംഖ്യ sysctl_writef(സ്ഥിരം അക്ഷരം *property, സ്ഥിരം അക്ഷരം *format, ...) _printf_(2, 3);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ sysctl_write(സ്ഥിരം അക്ഷരം *property, സ്ഥിരം അക്ഷരം *മൂല്യം) {
        തിരികെ sysctl_write_full(property, മൂല്യം, ശൂന്യ);
}
പൂർണ്ണസംഖ്യ sysctl_write_verify(സ്ഥിരം അക്ഷരം *property, സ്ഥിരം അക്ഷരം *മൂല്യം);

പൂർണ്ണസംഖ്യ sysctl_read_ip_property(പൂർണ്ണസംഖ്യ af, സ്ഥിരം അക്ഷരം *ifname, സ്ഥിരം അക്ഷരം *property, അക്ഷരം **ret);
പൂർണ്ണസംഖ്യ sysctl_read_ip_property_int(പൂർണ്ണസംഖ്യ af, സ്ഥിരം അക്ഷരം *ifname, സ്ഥിരം അക്ഷരം *property, പൂർണ്ണസംഖ്യ *ret);
പൂർണ്ണസംഖ്യ sysctl_read_ip_property_uint32(പൂർണ്ണസംഖ്യ af, സ്ഥിരം അക്ഷരം *ifname, സ്ഥിരം അക്ഷരം *property, uint32_t *ret);
പൂർണ്ണസംഖ്യ sysctl_write_ip_property(പൂർണ്ണസംഖ്യ af, സ്ഥിരം അക്ഷരം *ifname, സ്ഥിരം അക്ഷരം *property, സ്ഥിരം അക്ഷരം *മൂല്യം, Hashmap **shadow);
പൂർണ്ണസംഖ്യ sysctl_write_ip_property_boolean(പൂർണ്ണസംഖ്യ af, സ്ഥിരം അക്ഷരം *ifname, സ്ഥിരം അക്ഷരം *property, bool മൂല്യം, Hashmap **shadow);

സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ sysctl_write_ip_property_tristate(പൂർണ്ണസംഖ്യ af, സ്ഥിരം അക്ഷരം *ifname, സ്ഥിരം അക്ഷരം *property, പൂർണ്ണസംഖ്യ മൂല്യം, Hashmap **shadow) {
        എങ്കിൽ (മൂല്യം < 0)
                തിരികെ 0;

        തിരികെ sysctl_write_ip_property_boolean(af, ifname, property, മൂല്യം, shadow);
}

പൂർണ്ണസംഖ്യ sysctl_write_ip_neighbor_property(പൂർണ്ണസംഖ്യ af, സ്ഥിരം അക്ഷരം *ifname, സ്ഥിരം അക്ഷരം *property, സ്ഥിരം അക്ഷരം *മൂല്യം, Hashmap **shadow);
പൂർണ്ണസംഖ്യ sysctl_write_ip_neighbor_property_uint32(പൂർണ്ണസംഖ്യ af, സ്ഥിരം അക്ഷരം *ifname, സ്ഥിരം അക്ഷരം *property, uint32_t മൂല്യം, Hashmap **shadow);

#define DEFINE_SYSCTL_WRITE_IP_PROPERTY(name, type, format)           \
        സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ sysctl_write_ip_property_##name(പൂർണ്ണസംഖ്യ af, സ്ഥിരം അക്ഷരം *ifname, സ്ഥിരം അക്ഷരം *property, type മൂല്യം, Hashmap **shadow) { \
                അക്ഷരം buf[DECIMAL_STR_MAX(type)];                        \
                xsprintf(buf, format, മൂല്യം);                           \
                തിരികെ sysctl_write_ip_property(af, ifname, property, buf, shadow); \
        }

DEFINE_SYSCTL_WRITE_IP_PROPERTY(പൂർണ്ണസംഖ്യ, പൂർണ്ണസംഖ്യ, "%i");
DEFINE_SYSCTL_WRITE_IP_PROPERTY(uint32, uint32_t, "%" PRIu32);
