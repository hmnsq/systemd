/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once


#ಅನ್ತರ್ಭಾವ "forward.h"
#ಅನ್ತರ್ಭಾವ "stdio-util.h"

ಅಕ್ಷರ* sysctl_normalize(ಅಕ್ಷರ *s);
ಪೂರ್ಣಾಂಕ sysctl_read(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *property, ಅಕ್ಷರ **ret);
ಪೂರ್ಣಾಂಕ sysctl_write_ತುಂಬಿದ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *property, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ, Hashmap **shadow);
ಪೂರ್ಣಾಂಕ sysctl_writef(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *property, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *format, ...) _printf_(2, 3);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ sysctl_write(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *property, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ) {
        ಹಿಂತಿರುಗಿ sysctl_write_ತುಂಬಿದ(property, ಮೌಲ್ಯ, ಖಾಲಿ);
}
ಪೂರ್ಣಾಂಕ sysctl_write_verify(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *property, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ);

ಪೂರ್ಣಾಂಕ sysctl_read_ip_property(ಪೂರ್ಣಾಂಕ af, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ifname, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *property, ಅಕ್ಷರ **ret);
ಪೂರ್ಣಾಂಕ sysctl_read_ip_property_int(ಪೂರ್ಣಾಂಕ af, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ifname, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *property, ಪೂರ್ಣಾಂಕ *ret);
ಪೂರ್ಣಾಂಕ sysctl_read_ip_property_uint32(ಪೂರ್ಣಾಂಕ af, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ifname, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *property, uint32_t *ret);
ಪೂರ್ಣಾಂಕ sysctl_write_ip_property(ಪೂರ್ಣಾಂಕ af, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ifname, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *property, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ, Hashmap **shadow);
ಪೂರ್ಣಾಂಕ sysctl_write_ip_property_boolean(ಪೂರ್ಣಾಂಕ af, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ifname, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *property, bool ಮೌಲ್ಯ, Hashmap **shadow);

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ sysctl_write_ip_property_tristate(ಪೂರ್ಣಾಂಕ af, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ifname, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *property, ಪೂರ್ಣಾಂಕ ಮೌಲ್ಯ, Hashmap **shadow) {
        ಆದರೆ (ಮೌಲ್ಯ < 0)
                ಹಿಂತಿರುಗಿ 0;

        ಹಿಂತಿರುಗಿ sysctl_write_ip_property_boolean(af, ifname, property, ಮೌಲ್ಯ, shadow);
}

ಪೂರ್ಣಾಂಕ sysctl_write_ip_neighbor_property(ಪೂರ್ಣಾಂಕ af, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ifname, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *property, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ, Hashmap **shadow);
ಪೂರ್ಣಾಂಕ sysctl_write_ip_neighbor_property_uint32(ಪೂರ್ಣಾಂಕ af, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ifname, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *property, uint32_t ಮೌಲ್ಯ, Hashmap **shadow);

#ವ್ಯಾಖ್ಯಾನಿಸು DEFINE_SYSCTL_WRITE_IP_PROPERTY(name, type, format)           \
        ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ sysctl_write_ip_property_##name(ಪೂರ್ಣಾಂಕ af, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ifname, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *property, type ಮೌಲ್ಯ, Hashmap **shadow) { \
                ಅಕ್ಷರ buf[DECIMAL_STR_MAX(type)];                        \
                xsprintf(buf, format, ಮೌಲ್ಯ);                           \
                ಹಿಂತಿರುಗಿ sysctl_write_ip_property(af, ifname, property, buf, shadow); \
        }

DEFINE_SYSCTL_WRITE_IP_PROPERTY(ಪೂರ್ಣಾಂಕ, ಪೂರ್ಣಾಂಕ, "%i");
DEFINE_SYSCTL_WRITE_IP_PROPERTY(uint32, uint32_t, "%" PRIu32);
