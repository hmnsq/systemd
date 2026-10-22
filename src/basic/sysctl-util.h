/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once


#ಅನ್ತರ್ಭಾವ "forward.h"
#ಅನ್ತರ್ಭಾವ "stdio-util.h"

ವರ್ಣ* sysctl_normalize(ವರ್ಣ *s);
ಪೂರ್ಣಾಙ್ಕ sysctl_read(ಧ್ರುವ ವರ್ಣ *property, ವರ್ಣ **ret);
ಪೂರ್ಣಾಙ್ಕ sysctl_write_full(ಧ್ರುವ ವರ್ಣ *property, ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ, Hashmap **shadow);
ಪೂರ್ಣಾಙ್ಕ sysctl_writef(ಧ್ರುವ ವರ್ಣ *property, ಧ್ರುವ ವರ್ಣ *format, ...) _printf_(2, 3);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ sysctl_write(ಧ್ರುವ ವರ್ಣ *property, ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ) {
        ಪ್ರತ್ಯಾಗಮ sysctl_write_full(property, ಮೌಲ್ಯ, ರಿಕ್ತ);
}
ಪೂರ್ಣಾಙ್ಕ sysctl_write_verify(ಧ್ರುವ ವರ್ಣ *property, ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ);

ಪೂರ್ಣಾಙ್ಕ sysctl_read_ip_property(ಪೂರ್ಣಾಙ್ಕ af, ಧ್ರುವ ವರ್ಣ *ifname, ಧ್ರುವ ವರ್ಣ *property, ವರ್ಣ **ret);
ಪೂರ್ಣಾಙ್ಕ sysctl_read_ip_property_int(ಪೂರ್ಣಾಙ್ಕ af, ಧ್ರುವ ವರ್ಣ *ifname, ಧ್ರುವ ವರ್ಣ *property, ಪೂರ್ಣಾಙ್ಕ *ret);
ಪೂರ್ಣಾಙ್ಕ sysctl_read_ip_property_uint32(ಪೂರ್ಣಾಙ್ಕ af, ಧ್ರುವ ವರ್ಣ *ifname, ಧ್ರುವ ವರ್ಣ *property, uint32_t *ret);
ಪೂರ್ಣಾಙ್ಕ sysctl_write_ip_property(ಪೂರ್ಣಾಙ್ಕ af, ಧ್ರುವ ವರ್ಣ *ifname, ಧ್ರುವ ವರ್ಣ *property, ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ, Hashmap **shadow);
ಪೂರ್ಣಾಙ್ಕ sysctl_write_ip_property_boolean(ಪೂರ್ಣಾಙ್ಕ af, ಧ್ರುವ ವರ್ಣ *ifname, ಧ್ರುವ ವರ್ಣ *property, bool ಮೌಲ್ಯ, Hashmap **shadow);

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ sysctl_write_ip_property_tristate(ಪೂರ್ಣಾಙ್ಕ af, ಧ್ರುವ ವರ್ಣ *ifname, ಧ್ರುವ ವರ್ಣ *property, ಪೂರ್ಣಾಙ್ಕ ಮೌಲ್ಯ, Hashmap **shadow) {
        ಯದಿ (ಮೌಲ್ಯ < 0)
                ಪ್ರತ್ಯಾಗಮ 0;

        ಪ್ರತ್ಯಾಗಮ sysctl_write_ip_property_boolean(af, ifname, property, ಮೌಲ್ಯ, shadow);
}

ಪೂರ್ಣಾಙ್ಕ sysctl_write_ip_neighbor_property(ಪೂರ್ಣಾಙ್ಕ af, ಧ್ರುವ ವರ್ಣ *ifname, ಧ್ರುವ ವರ್ಣ *property, ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ, Hashmap **shadow);
ಪೂರ್ಣಾಙ್ಕ sysctl_write_ip_neighbor_property_uint32(ಪೂರ್ಣಾಙ್ಕ af, ಧ್ರುವ ವರ್ಣ *ifname, ಧ್ರುವ ವರ್ಣ *property, uint32_t ಮೌಲ್ಯ, Hashmap **shadow);

#define DEFINE_SYSCTL_WRITE_IP_PROPERTY(name, type, format)           \
        ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ sysctl_write_ip_property_##name(ಪೂರ್ಣಾಙ್ಕ af, ಧ್ರುವ ವರ್ಣ *ifname, ಧ್ರುವ ವರ್ಣ *property, type ಮೌಲ್ಯ, Hashmap **shadow) { \
                ವರ್ಣ buf[DECIMAL_STR_MAX(type)];                        \
                xsprintf(buf, format, ಮೌಲ್ಯ);                           \
                ಪ್ರತ್ಯಾಗಮ sysctl_write_ip_property(af, ifname, property, buf, shadow); \
        }

DEFINE_SYSCTL_WRITE_IP_PROPERTY(ಪೂರ್ಣಾಙ್ಕ, ಪೂರ್ಣಾಙ್ಕ, "%i");
DEFINE_SYSCTL_WRITE_IP_PROPERTY(uint32, uint32_t, "%" PRIu32);
