/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once


#అన్తర్భావ "forward.h"
#అన్తర్భావ "stdio-util.h"

అక్షరం* sysctl_normalize(అక్షరం *s);
పూర్ణాంకం sysctl_read(స్థిరరాశి అక్షరం *property, అక్షరం **ret);
పూర్ణాంకం sysctl_write_నిండిన(స్థిరరాశి అక్షరం *property, స్థిరరాశి అక్షరం *విలువ, Hashmap **shadow);
పూర్ణాంకం sysctl_writef(స్థిరరాశి అక్షరం *property, స్థిరరాశి అక్షరం *format, ...) _printf_(2, 3);
స్థిర అంతఃవరుస పూర్ణాంకం sysctl_write(స్థిరరాశి అక్షరం *property, స్థిరరాశి అక్షరం *విలువ) {
        తిరిగి sysctl_write_నిండిన(property, విలువ, ఖాళీ);
}
పూర్ణాంకం sysctl_write_verify(స్థిరరాశి అక్షరం *property, స్థిరరాశి అక్షరం *విలువ);

పూర్ణాంకం sysctl_read_ip_property(పూర్ణాంకం af, స్థిరరాశి అక్షరం *ifname, స్థిరరాశి అక్షరం *property, అక్షరం **ret);
పూర్ణాంకం sysctl_read_ip_property_int(పూర్ణాంకం af, స్థిరరాశి అక్షరం *ifname, స్థిరరాశి అక్షరం *property, పూర్ణాంకం *ret);
పూర్ణాంకం sysctl_read_ip_property_uint32(పూర్ణాంకం af, స్థిరరాశి అక్షరం *ifname, స్థిరరాశి అక్షరం *property, uint32_t *ret);
పూర్ణాంకం sysctl_write_ip_property(పూర్ణాంకం af, స్థిరరాశి అక్షరం *ifname, స్థిరరాశి అక్షరం *property, స్థిరరాశి అక్షరం *విలువ, Hashmap **shadow);
పూర్ణాంకం sysctl_write_ip_property_boolean(పూర్ణాంకం af, స్థిరరాశి అక్షరం *ifname, స్థిరరాశి అక్షరం *property, bool విలువ, Hashmap **shadow);

స్థిర అంతఃవరుస పూర్ణాంకం sysctl_write_ip_property_tristate(పూర్ణాంకం af, స్థిరరాశి అక్షరం *ifname, స్థిరరాశి అక్షరం *property, పూర్ణాంకం విలువ, Hashmap **shadow) {
        ఒకవేళ (విలువ < 0)
                తిరిగి 0;

        తిరిగి sysctl_write_ip_property_boolean(af, ifname, property, విలువ, shadow);
}

పూర్ణాంకం sysctl_write_ip_neighbor_property(పూర్ణాంకం af, స్థిరరాశి అక్షరం *ifname, స్థిరరాశి అక్షరం *property, స్థిరరాశి అక్షరం *విలువ, Hashmap **shadow);
పూర్ణాంకం sysctl_write_ip_neighbor_property_uint32(పూర్ణాంకం af, స్థిరరాశి అక్షరం *ifname, స్థిరరాశి అక్షరం *property, uint32_t విలువ, Hashmap **shadow);

#నిర్వచించు DEFINE_SYSCTL_WRITE_IP_PROPERTY(name, type, format)           \
        స్థిర అంతఃవరుస పూర్ణాంకం sysctl_write_ip_property_##name(పూర్ణాంకం af, స్థిరరాశి అక్షరం *ifname, స్థిరరాశి అక్షరం *property, type విలువ, Hashmap **shadow) { \
                అక్షరం buf[DECIMAL_STR_MAX(type)];                        \
                xsprintf(buf, format, విలువ);                           \
                తిరిగి sysctl_write_ip_property(af, ifname, property, buf, shadow); \
        }

DEFINE_SYSCTL_WRITE_IP_PROPERTY(పూర్ణాంకం, పూర్ణాంకం, "%i");
DEFINE_SYSCTL_WRITE_IP_PROPERTY(uint32, uint32_t, "%" PRIu32);
