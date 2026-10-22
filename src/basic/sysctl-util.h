/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once


#شامل "forward.h"
#شامل "stdio-util.h"

حرف* sysctl_normalize(حرف *s);
صحیح sysctl_read(ثابت حرف *property, حرف **ret);
صحیح sysctl_write_بھرا(ثابت حرف *property, ثابت حرف *قدر, Hashmap **shadow);
صحیح sysctl_writef(ثابت حرف *property, ثابت حرف *format, ...) _printf_(2, 3);
ساکن اندرونی صحیح sysctl_write(ثابت حرف *property, ثابت حرف *قدر) {
        واپس sysctl_write_بھرا(property, قدر, تہی);
}
صحیح sysctl_write_verify(ثابت حرف *property, ثابت حرف *قدر);

صحیح sysctl_read_ip_property(صحیح af, ثابت حرف *ifname, ثابت حرف *property, حرف **ret);
صحیح sysctl_read_ip_property_int(صحیح af, ثابت حرف *ifname, ثابت حرف *property, صحیح *ret);
صحیح sysctl_read_ip_property_uint32(صحیح af, ثابت حرف *ifname, ثابت حرف *property, uint32_t *ret);
صحیح sysctl_write_ip_property(صحیح af, ثابت حرف *ifname, ثابت حرف *property, ثابت حرف *قدر, Hashmap **shadow);
صحیح sysctl_write_ip_property_boolean(صحیح af, ثابت حرف *ifname, ثابت حرف *property, bool قدر, Hashmap **shadow);

ساکن اندرونی صحیح sysctl_write_ip_property_tristate(صحیح af, ثابت حرف *ifname, ثابت حرف *property, صحیح قدر, Hashmap **shadow) {
        اگر (قدر < 0)
                واپس 0;

        واپس sysctl_write_ip_property_boolean(af, ifname, property, قدر, shadow);
}

صحیح sysctl_write_ip_neighbor_property(صحیح af, ثابت حرف *ifname, ثابت حرف *property, ثابت حرف *قدر, Hashmap **shadow);
صحیح sysctl_write_ip_neighbor_property_uint32(صحیح af, ثابت حرف *ifname, ثابت حرف *property, uint32_t قدر, Hashmap **shadow);

#تعریف_کرو DEFINE_SYSCTL_WRITE_IP_PROPERTY(name, type, format)           \
        ساکن اندرونی صحیح sysctl_write_ip_property_##name(صحیح af, ثابت حرف *ifname, ثابت حرف *property, type قدر, Hashmap **shadow) { \
                حرف buf[DECIMAL_STR_MAX(type)];                        \
                xsprintf(buf, format, قدر);                           \
                واپس sysctl_write_ip_property(af, ifname, property, buf, shadow); \
        }

DEFINE_SYSCTL_WRITE_IP_PROPERTY(صحیح, صحیح, "%i");
DEFINE_SYSCTL_WRITE_IP_PROPERTY(uint32, uint32_t, "%" PRIu32);
