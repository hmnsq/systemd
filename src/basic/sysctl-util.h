/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once


#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "stdio-util.h"

ਅੱਖਰ* sysctl_normalize(ਅੱਖਰ *s);
ਪੂਰਨ sysctl_read(ਅਚਲ ਅੱਖਰ *property, ਅੱਖਰ **ret);
ਪੂਰਨ sysctl_write_ਭਰਿਆ(ਅਚਲ ਅੱਖਰ *property, ਅਚਲ ਅੱਖਰ *ਮੁੱਲ, Hashmap **shadow);
ਪੂਰਨ sysctl_writef(ਅਚਲ ਅੱਖਰ *property, ਅਚਲ ਅੱਖਰ *format, ...) _printf_(2, 3);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ sysctl_write(ਅਚਲ ਅੱਖਰ *property, ਅਚਲ ਅੱਖਰ *ਮੁੱਲ) {
        ਵਾਪਸ sysctl_write_ਭਰਿਆ(property, ਮੁੱਲ, ਨਿਰਾ);
}
ਪੂਰਨ sysctl_write_verify(ਅਚਲ ਅੱਖਰ *property, ਅਚਲ ਅੱਖਰ *ਮੁੱਲ);

ਪੂਰਨ sysctl_read_ip_property(ਪੂਰਨ af, ਅਚਲ ਅੱਖਰ *ifname, ਅਚਲ ਅੱਖਰ *property, ਅੱਖਰ **ret);
ਪੂਰਨ sysctl_read_ip_property_int(ਪੂਰਨ af, ਅਚਲ ਅੱਖਰ *ifname, ਅਚਲ ਅੱਖਰ *property, ਪੂਰਨ *ret);
ਪੂਰਨ sysctl_read_ip_property_uint32(ਪੂਰਨ af, ਅਚਲ ਅੱਖਰ *ifname, ਅਚਲ ਅੱਖਰ *property, uint32_t *ret);
ਪੂਰਨ sysctl_write_ip_property(ਪੂਰਨ af, ਅਚਲ ਅੱਖਰ *ifname, ਅਚਲ ਅੱਖਰ *property, ਅਚਲ ਅੱਖਰ *ਮੁੱਲ, Hashmap **shadow);
ਪੂਰਨ sysctl_write_ip_property_boolean(ਪੂਰਨ af, ਅਚਲ ਅੱਖਰ *ifname, ਅਚਲ ਅੱਖਰ *property, bool ਮੁੱਲ, Hashmap **shadow);

ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ sysctl_write_ip_property_tristate(ਪੂਰਨ af, ਅਚਲ ਅੱਖਰ *ifname, ਅਚਲ ਅੱਖਰ *property, ਪੂਰਨ ਮੁੱਲ, Hashmap **shadow) {
        ਜੇ (ਮੁੱਲ < 0)
                ਵਾਪਸ 0;

        ਵਾਪਸ sysctl_write_ip_property_boolean(af, ifname, property, ਮੁੱਲ, shadow);
}

ਪੂਰਨ sysctl_write_ip_neighbor_property(ਪੂਰਨ af, ਅਚਲ ਅੱਖਰ *ifname, ਅਚਲ ਅੱਖਰ *property, ਅਚਲ ਅੱਖਰ *ਮੁੱਲ, Hashmap **shadow);
ਪੂਰਨ sysctl_write_ip_neighbor_property_uint32(ਪੂਰਨ af, ਅਚਲ ਅੱਖਰ *ifname, ਅਚਲ ਅੱਖਰ *property, uint32_t ਮੁੱਲ, Hashmap **shadow);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFINE_SYSCTL_WRITE_IP_PROPERTY(name, type, format)           \
        ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ sysctl_write_ip_property_##name(ਪੂਰਨ af, ਅਚਲ ਅੱਖਰ *ifname, ਅਚਲ ਅੱਖਰ *property, type ਮੁੱਲ, Hashmap **shadow) { \
                ਅੱਖਰ buf[DECIMAL_STR_MAX(type)];                        \
                xsprintf(buf, format, ਮੁੱਲ);                           \
                ਵਾਪਸ sysctl_write_ip_property(af, ifname, property, buf, shadow); \
        }

DEFINE_SYSCTL_WRITE_IP_PROPERTY(ਪੂਰਨ, ਪੂਰਨ, "%i");
DEFINE_SYSCTL_WRITE_IP_PROPERTY(uint32, uint32_t, "%" PRIu32);
