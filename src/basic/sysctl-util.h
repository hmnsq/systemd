/* SPDX-License-Identifier: LGPL-2.1-or-later */
#لارښوونه once


#ضمن "forward.h"
#ضمن "stdio-util.h"

حرف* sysctl_normalize(حرف *s);
صحيح sysctl_read(ثابت حرف *property, حرف **ret);
صحيح sysctl_write_ډک(ثابت حرف *property, ثابت حرف *ارزښت, Hashmap **shadow);
صحيح sysctl_writef(ثابت حرف *property, ثابت حرف *format, ...) _printf_(2, 3);
ساكن مضمن صحيح sysctl_write(ثابت حرف *property, ثابت حرف *ارزښت) {
        أرجع sysctl_write_ډک(property, ارزښت, عدم);
}
صحيح sysctl_write_verify(ثابت حرف *property, ثابت حرف *ارزښت);

صحيح sysctl_read_ip_property(صحيح af, ثابت حرف *ifname, ثابت حرف *property, حرف **ret);
صحيح sysctl_read_ip_property_int(صحيح af, ثابت حرف *ifname, ثابت حرف *property, صحيح *ret);
صحيح sysctl_read_ip_property_uint32(صحيح af, ثابت حرف *ifname, ثابت حرف *property, uint32_t *ret);
صحيح sysctl_write_ip_property(صحيح af, ثابت حرف *ifname, ثابت حرف *property, ثابت حرف *ارزښت, Hashmap **shadow);
صحيح sysctl_write_ip_property_boolean(صحيح af, ثابت حرف *ifname, ثابت حرف *property, bool ارزښت, Hashmap **shadow);

ساكن مضمن صحيح sysctl_write_ip_property_tristate(صحيح af, ثابت حرف *ifname, ثابت حرف *property, صحيح ارزښت, Hashmap **shadow) {
        إذا (ارزښت < 0)
                أرجع 0;

        أرجع sysctl_write_ip_property_boolean(af, ifname, property, ارزښت, shadow);
}

صحيح sysctl_write_ip_neighbor_property(صحيح af, ثابت حرف *ifname, ثابت حرف *property, ثابت حرف *ارزښت, Hashmap **shadow);
صحيح sysctl_write_ip_neighbor_property_uint32(صحيح af, ثابت حرف *ifname, ثابت حرف *property, uint32_t ارزښت, Hashmap **shadow);

#تعریف_کړه DEFINE_SYSCTL_WRITE_IP_PROPERTY(name, type, format)           \
        ساكن مضمن صحيح sysctl_write_ip_property_##name(صحيح af, ثابت حرف *ifname, ثابت حرف *property, type ارزښت, Hashmap **shadow) { \
                حرف buf[DECIMAL_STR_MAX(type)];                        \
                xsprintf(buf, format, ارزښت);                           \
                أرجع sysctl_write_ip_property(af, ifname, property, buf, shadow); \
        }

DEFINE_SYSCTL_WRITE_IP_PROPERTY(صحيح, صحيح, "%i");
DEFINE_SYSCTL_WRITE_IP_PROPERTY(uint32, uint32_t, "%" PRIu32);
