/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once


#અન્તર્ભાવ "forward.h"
#અન્તર્ભાવ "stdio-util.h"

અક્ષર* sysctl_normalize(અક્ષર *s);
પૂર્ણાંક sysctl_read(અચલ અક્ષર *property, અક્ષર **ret);
પૂર્ણાંક sysctl_write_full(અચલ અક્ષર *property, અચલ અક્ષર *કિંમત, Hashmap **shadow);
પૂર્ણાંક sysctl_writef(અચલ અક્ષર *property, અચલ અક્ષર *format, ...) _printf_(2, 3);
સ્થિર અંતઃરેખ પૂર્ણાંક sysctl_write(અચલ અક્ષર *property, અચલ અક્ષર *કિંમત) {
        પાછા sysctl_write_full(property, કિંમત, ખાલી);
}
પૂર્ણાંક sysctl_write_verify(અચલ અક્ષર *property, અચલ અક્ષર *કિંમત);

પૂર્ણાંક sysctl_read_ip_property(પૂર્ણાંક af, અચલ અક્ષર *ifname, અચલ અક્ષર *property, અક્ષર **ret);
પૂર્ણાંક sysctl_read_ip_property_int(પૂર્ણાંક af, અચલ અક્ષર *ifname, અચલ અક્ષર *property, પૂર્ણાંક *ret);
પૂર્ણાંક sysctl_read_ip_property_uint32(પૂર્ણાંક af, અચલ અક્ષર *ifname, અચલ અક્ષર *property, uint32_t *ret);
પૂર્ણાંક sysctl_write_ip_property(પૂર્ણાંક af, અચલ અક્ષર *ifname, અચલ અક્ષર *property, અચલ અક્ષર *કિંમત, Hashmap **shadow);
પૂર્ણાંક sysctl_write_ip_property_boolean(પૂર્ણાંક af, અચલ અક્ષર *ifname, અચલ અક્ષર *property, bool કિંમત, Hashmap **shadow);

સ્થિર અંતઃરેખ પૂર્ણાંક sysctl_write_ip_property_tristate(પૂર્ણાંક af, અચલ અક્ષર *ifname, અચલ અક્ષર *property, પૂર્ણાંક કિંમત, Hashmap **shadow) {
        જો (કિંમત < 0)
                પાછા 0;

        પાછા sysctl_write_ip_property_boolean(af, ifname, property, કિંમત, shadow);
}

પૂર્ણાંક sysctl_write_ip_neighbor_property(પૂર્ણાંક af, અચલ અક્ષર *ifname, અચલ અક્ષર *property, અચલ અક્ષર *કિંમત, Hashmap **shadow);
પૂર્ણાંક sysctl_write_ip_neighbor_property_uint32(પૂર્ણાંક af, અચલ અક્ષર *ifname, અચલ અક્ષર *property, uint32_t કિંમત, Hashmap **shadow);

#define DEFINE_SYSCTL_WRITE_IP_PROPERTY(name, type, format)           \
        સ્થિર અંતઃરેખ પૂર્ણાંક sysctl_write_ip_property_##name(પૂર્ણાંક af, અચલ અક્ષર *ifname, અચલ અક્ષર *property, type કિંમત, Hashmap **shadow) { \
                અક્ષર buf[DECIMAL_STR_MAX(type)];                        \
                xsprintf(buf, format, કિંમત);                           \
                પાછા sysctl_write_ip_property(af, ifname, property, buf, shadow); \
        }

DEFINE_SYSCTL_WRITE_IP_PROPERTY(પૂર્ણાંક, પૂર્ણાંક, "%i");
DEFINE_SYSCTL_WRITE_IP_PROPERTY(uint32, uint32_t, "%" PRIu32);
