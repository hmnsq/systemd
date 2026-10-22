/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once


#அந்தர்பாவ "forward.h"
#அந்தர்பாவ "stdio-util.h"

எழுத்து* sysctl_normalize(எழுத்து *s);
முழு sysctl_read(மாறிலி எழுத்து *property, எழுத்து **ret);
முழு sysctl_write_full(மாறிலி எழுத்து *property, மாறிலி எழுத்து *மதிப்பு, Hashmap **shadow);
முழு sysctl_writef(மாறிலி எழுத்து *property, மாறிலி எழுத்து *format, ...) _printf_(2, 3);
நிலையான உள்வரி முழு sysctl_write(மாறிலி எழுத்து *property, மாறிலி எழுத்து *மதிப்பு) {
        திரும்பு sysctl_write_full(property, மதிப்பு, வெற்று);
}
முழு sysctl_write_verify(மாறிலி எழுத்து *property, மாறிலி எழுத்து *மதிப்பு);

முழு sysctl_read_ip_property(முழு af, மாறிலி எழுத்து *ifname, மாறிலி எழுத்து *property, எழுத்து **ret);
முழு sysctl_read_ip_property_int(முழு af, மாறிலி எழுத்து *ifname, மாறிலி எழுத்து *property, முழு *ret);
முழு sysctl_read_ip_property_uint32(முழு af, மாறிலி எழுத்து *ifname, மாறிலி எழுத்து *property, uint32_t *ret);
முழு sysctl_write_ip_property(முழு af, மாறிலி எழுத்து *ifname, மாறிலி எழுத்து *property, மாறிலி எழுத்து *மதிப்பு, Hashmap **shadow);
முழு sysctl_write_ip_property_boolean(முழு af, மாறிலி எழுத்து *ifname, மாறிலி எழுத்து *property, bool மதிப்பு, Hashmap **shadow);

நிலையான உள்வரி முழு sysctl_write_ip_property_tristate(முழு af, மாறிலி எழுத்து *ifname, மாறிலி எழுத்து *property, முழு மதிப்பு, Hashmap **shadow) {
        எனில் (மதிப்பு < 0)
                திரும்பு 0;

        திரும்பு sysctl_write_ip_property_boolean(af, ifname, property, மதிப்பு, shadow);
}

முழு sysctl_write_ip_neighbor_property(முழு af, மாறிலி எழுத்து *ifname, மாறிலி எழுத்து *property, மாறிலி எழுத்து *மதிப்பு, Hashmap **shadow);
முழு sysctl_write_ip_neighbor_property_uint32(முழு af, மாறிலி எழுத்து *ifname, மாறிலி எழுத்து *property, uint32_t மதிப்பு, Hashmap **shadow);

#define DEFINE_SYSCTL_WRITE_IP_PROPERTY(name, type, format)           \
        நிலையான உள்வரி முழு sysctl_write_ip_property_##name(முழு af, மாறிலி எழுத்து *ifname, மாறிலி எழுத்து *property, type மதிப்பு, Hashmap **shadow) { \
                எழுத்து buf[DECIMAL_STR_MAX(type)];                        \
                xsprintf(buf, format, மதிப்பு);                           \
                திரும்பு sysctl_write_ip_property(af, ifname, property, buf, shadow); \
        }

DEFINE_SYSCTL_WRITE_IP_PROPERTY(முழு, முழு, "%i");
DEFINE_SYSCTL_WRITE_IP_PROPERTY(uint32, uint32_t, "%" PRIu32);
