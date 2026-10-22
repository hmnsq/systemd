/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once


#ଅନ୍ତର୍ଭାଵ "forward.h"
#ଅନ୍ତର୍ଭାଵ "stdio-util.h"

ଅକ୍ଷର* sysctl_normalize(ଅକ୍ଷର *s);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା sysctl_read(ଅଚଳ ଅକ୍ଷର *property, ଅକ୍ଷର **ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା sysctl_write_full(ଅଚଳ ଅକ୍ଷର *property, ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ, Hashmap **shadow);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା sysctl_writef(ଅଚଳ ଅକ୍ଷର *property, ଅଚଳ ଅକ୍ଷର *format, ...) _printf_(2, 3);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା sysctl_write(ଅଚଳ ଅକ୍ଷର *property, ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ) {
        ଫେରନ୍ତୁ sysctl_write_full(property, ମୂଲ୍ୟ, ଖାଲି);
}
ପୂର୍ଣ୍ଣସଂଖ୍ୟା sysctl_write_verify(ଅଚଳ ଅକ୍ଷର *property, ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sysctl_read_ip_property(ପୂର୍ଣ୍ଣସଂଖ୍ୟା af, ଅଚଳ ଅକ୍ଷର *ifname, ଅଚଳ ଅକ୍ଷର *property, ଅକ୍ଷର **ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା sysctl_read_ip_property_int(ପୂର୍ଣ୍ଣସଂଖ୍ୟା af, ଅଚଳ ଅକ୍ଷର *ifname, ଅଚଳ ଅକ୍ଷର *property, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା sysctl_read_ip_property_uint32(ପୂର୍ଣ୍ଣସଂଖ୍ୟା af, ଅଚଳ ଅକ୍ଷର *ifname, ଅଚଳ ଅକ୍ଷର *property, uint32_t *ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା sysctl_write_ip_property(ପୂର୍ଣ୍ଣସଂଖ୍ୟା af, ଅଚଳ ଅକ୍ଷର *ifname, ଅଚଳ ଅକ୍ଷର *property, ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ, Hashmap **shadow);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା sysctl_write_ip_property_boolean(ପୂର୍ଣ୍ଣସଂଖ୍ୟା af, ଅଚଳ ଅକ୍ଷର *ifname, ଅଚଳ ଅକ୍ଷର *property, bool ମୂଲ୍ୟ, Hashmap **shadow);

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା sysctl_write_ip_property_tristate(ପୂର୍ଣ୍ଣସଂଖ୍ୟା af, ଅଚଳ ଅକ୍ଷର *ifname, ଅଚଳ ଅକ୍ଷର *property, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୂଲ୍ୟ, Hashmap **shadow) {
        ଯଦି (ମୂଲ୍ୟ < 0)
                ଫେରନ୍ତୁ 0;

        ଫେରନ୍ତୁ sysctl_write_ip_property_boolean(af, ifname, property, ମୂଲ୍ୟ, shadow);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sysctl_write_ip_neighbor_property(ପୂର୍ଣ୍ଣସଂଖ୍ୟା af, ଅଚଳ ଅକ୍ଷର *ifname, ଅଚଳ ଅକ୍ଷର *property, ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ, Hashmap **shadow);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା sysctl_write_ip_neighbor_property_uint32(ପୂର୍ଣ୍ଣସଂଖ୍ୟା af, ଅଚଳ ଅକ୍ଷର *ifname, ଅଚଳ ଅକ୍ଷର *property, uint32_t ମୂଲ୍ୟ, Hashmap **shadow);

#define DEFINE_SYSCTL_WRITE_IP_PROPERTY(name, type, format)           \
        ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା sysctl_write_ip_property_##name(ପୂର୍ଣ୍ଣସଂଖ୍ୟା af, ଅଚଳ ଅକ୍ଷର *ifname, ଅଚଳ ଅକ୍ଷର *property, type ମୂଲ୍ୟ, Hashmap **shadow) { \
                ଅକ୍ଷର buf[DECIMAL_STR_MAX(type)];                        \
                xsprintf(buf, format, ମୂଲ୍ୟ);                           \
                ଫେରନ୍ତୁ sysctl_write_ip_property(af, ifname, property, buf, shadow); \
        }

DEFINE_SYSCTL_WRITE_IP_PROPERTY(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, "%i");
DEFINE_SYSCTL_WRITE_IP_PROPERTY(uint32, uint32_t, "%" PRIu32);
