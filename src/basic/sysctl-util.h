/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once


#شامل "forward.h"
#شامل "stdio-util.h"

نویسه* sysctl_normalize(نویسه *s);
صحیح sysctl_read(ثابت نویسه *property, نویسه **ret);
صحیح sysctl_write_full(ثابت نویسه *property, ثابت نویسه *مقدار, Hashmap **shadow);
صحیح sysctl_writef(ثابت نویسه *property, ثابت نویسه *format, ...) _printf_(2, 3);
ایستا درونخطی صحیح sysctl_write(ثابت نویسه *property, ثابت نویسه *مقدار) {
        بازگشت sysctl_write_full(property, مقدار, پوچ);
}
صحیح sysctl_write_verify(ثابت نویسه *property, ثابت نویسه *مقدار);

صحیح sysctl_read_ip_property(صحیح af, ثابت نویسه *ifname, ثابت نویسه *property, نویسه **ret);
صحیح sysctl_read_ip_property_int(صحیح af, ثابت نویسه *ifname, ثابت نویسه *property, صحیح *ret);
صحیح sysctl_read_ip_property_uint32(صحیح af, ثابت نویسه *ifname, ثابت نویسه *property, uint32_t *ret);
صحیح sysctl_write_ip_property(صحیح af, ثابت نویسه *ifname, ثابت نویسه *property, ثابت نویسه *مقدار, Hashmap **shadow);
صحیح sysctl_write_ip_property_boolean(صحیح af, ثابت نویسه *ifname, ثابت نویسه *property, bool مقدار, Hashmap **shadow);

ایستا درونخطی صحیح sysctl_write_ip_property_tristate(صحیح af, ثابت نویسه *ifname, ثابت نویسه *property, صحیح مقدار, Hashmap **shadow) {
        اگر (مقدار < 0)
                بازگشت 0;

        بازگشت sysctl_write_ip_property_boolean(af, ifname, property, مقدار, shadow);
}

صحیح sysctl_write_ip_neighbor_property(صحیح af, ثابت نویسه *ifname, ثابت نویسه *property, ثابت نویسه *مقدار, Hashmap **shadow);
صحیح sysctl_write_ip_neighbor_property_uint32(صحیح af, ثابت نویسه *ifname, ثابت نویسه *property, uint32_t مقدار, Hashmap **shadow);

#define DEFINE_SYSCTL_WRITE_IP_PROPERTY(name, type, format)           \
        ایستا درونخطی صحیح sysctl_write_ip_property_##name(صحیح af, ثابت نویسه *ifname, ثابت نویسه *property, type مقدار, Hashmap **shadow) { \
                نویسه buf[DECIMAL_STR_MAX(type)];                        \
                xsprintf(buf, format, مقدار);                           \
                بازگشت sysctl_write_ip_property(af, ifname, property, buf, shadow); \
        }

DEFINE_SYSCTL_WRITE_IP_PROPERTY(صحیح, صحیح, "%i");
DEFINE_SYSCTL_WRITE_IP_PROPERTY(uint32, uint32_t, "%" PRIu32);
