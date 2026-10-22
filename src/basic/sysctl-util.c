/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ <stdio.h>

#అన్తర్భావ "af-list.h"
#అన్తర్భావ "alloc-util.h"
#అన్తర్భావ "fileio.h"
#అన్తర్భావ "hashmap.h"
#అన్తర్భావ "log.h"
#అన్తర్భావ "parse-util.h"
#అన్తర్భావ "path-util.h"
#అన్తర్భావ "socket-util.h"
#అన్తర్భావ "string-util.h"
#అన్తర్భావ "sysctl-util.h"

అక్షరం* sysctl_normalize(అక్షరం *s) {
        అక్షరం *n;

        ప్రతిజ్ఞా(s);

        n = strpbrk(s, "/.");

        /* If the first separator is a slash, the path is
         * assumed to be normalized and slashes remain slashes
         * and dots remains dots. */

        ఒకవేళ (n && *n == '.')
                /* Dots become slashes and slashes become dots. Fun. */
                చేయి {
                        ఒకవేళ (*n == '.')
                                *n = '/';
                        లేకపోతే
                                *n = '.';

                        n = strpbrk(n + 1, "/.");
                } అయితే (n);

        మార్గం_simplify(s);

        /* Kill the leading slash, but keep the first character of the string in the same place. */
        ఒకవేళ (s[0] == '/' && s[1] != 0)
                memmove(s, s+1, అక్షరశ్రేణి_పొడవు(s));

        తిరిగి s;
}

స్థిర పూర్ణాంకం shadow_update(Hashmap **shadow, స్థిరరాశి అక్షరం *property, స్థిరరాశి అక్షరం *విలువ) {
        _cleanup_free_ అక్షరం *k = ఖాళీ, *v = ఖాళీ, *cur_k = ఖాళీ, *cur_v = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(property);
        ప్రతిజ్ఞా(విలువ);

        ఒకవేళ (!shadow)
                తిరిగి 0;

        k = strdup(property);
        ఒకవేళ (!k)
                తిరిగి -ENOMEM;

        v = strdup(విలువ);
        ఒకవేళ (!v)
                తిరిగి -ENOMEM;

        cur_v = hashmap_remove2(*shadow, k, (శూన్య**)&cur_k);

        r = hashmap_ensure_put(shadow, &మార్గం_hash_ops_free_free, k, v);
        ఒకవేళ (r < 0) {
                ప్రతిజ్ఞా(r != -EEXIST);

                తిరిగి r;
        }

        TAKE_PTR(k);
        TAKE_PTR(v);

        తిరిగి 0;
}

పూర్ణాంకం sysctl_write_నిండిన(స్థిరరాశి అక్షరం *property, స్థిరరాశి అక్షరం *విలువ, Hashmap **shadow) {
        అక్షరం *p;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(property);
        ప్రతిజ్ఞా(విలువ);

        p = strjoina("/proc/sys/", property);

        మార్గం_simplify(p);
        ఒకవేళ (!మార్గం_is_normalized(p))
                తిరిగి -EINVAL;

        log_debug("Setting '%s' to '%s'", p, విలువ);

        r = shadow_update(shadow, p, విలువ);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి write_string_file(p, విలువ, WRITE_STRING_FILE_VERIFY_ON_FAILURE | WRITE_STRING_FILE_DISABLE_బఫరు | WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL);
}

పూర్ణాంకం sysctl_writef(స్థిరరాశి అక్షరం *property, స్థిరరాశి అక్షరం *format, ...) {
        _cleanup_free_ అక్షరం *v = ఖాళీ;
        va_list ap;
        పూర్ణాంకం r;

        va_start(ap, format);
        r = vasprintf(&v, format, ap);
        va_end(ap);

        ఒకవేళ (r < 0)
                తిరిగి -ENOMEM;

        తిరిగి sysctl_write(property, v);
}

పూర్ణాంకం sysctl_write_verify(స్థిరరాశి అక్షరం *property, స్థిరరాశి అక్షరం *విలువ) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(property);
        ప్రతిజ్ఞా(విలువ);

        /* Some sysctl settings accept invalid values on write, but refuses on read. E.g. coredump pattern,
         * be1e0283021ec73c2eb92839db9a471a068709d9 (v6.17) and 7d7c1fb85cba5627bbe741fb7539c709435e3848
         * (v6.16.8), which is fixed by a779e27f24aeb679969ddd1fdd7f636e22ddbc1e (v6.18) and
         * 304aa560385720baf3660fe8500f6dd425b63ea9 (v6.17.5). Let's first save the original value, and
         * restore to the saved value if the new value is refused. */

        _cleanup_free_ అక్షరం *original = ఖాళీ;
        r = sysctl_read(property, &original);
        ఒకవేళ (r >= 0 && streq(original, విలువ))
                తిరిగి 0; /* Already set. */

        r = sysctl_write(property, విలువ);
        ఒకవేళ (r >= 0) {
                _cleanup_free_ అక్షరం *current = ఖాళీ;
                r = sysctl_read(property, &current);
                ఒకవేళ (r >= 0) {
                        ఒకవేళ (streq(current, విలువ))
                                తిరిగి 0; /* Yay! */

                        /* At least for coredump pattern, this does not happen, but let's handle this as the
                         * same as we wrote something invalid. */
                        r = -EINVAL;
                }
        }

        ఒకవేళ (original)
                (శూన్య) sysctl_write(property, original);

        తిరిగి r;
}

స్థిర స్థిరరాశి అక్షరం* af_గమ్యం_sysctl_dir(పూర్ణాంకం af) {
        ఒకవేళ (af == AF_MPLS)
                తిరిగి "mpls";

        తిరిగి af_గమ్యం_ipv4_ipv6(af);
}

పూర్ణాంకం sysctl_write_ip_property(పూర్ణాంకం af, స్థిరరాశి అక్షరం *ifname, స్థిరరాశి అక్షరం *property, స్థిరరాశి అక్షరం *విలువ, Hashmap **shadow) {
        స్థిరరాశి అక్షరం *p;

        ప్రతిజ్ఞా(property);
        ప్రతిజ్ఞా(విలువ);

        ఒకవేళ (!లోపల_సమితి(af, AF_INET, AF_INET6, AF_MPLS))
                తిరిగి -EAFNOSUPPORT;

        ఒకవేళ (ifname) {
                ఒకవేళ (!ifname_valid_నిండిన(ifname, IFNAME_VALID_SPECIAL))
                        తిరిగి -EINVAL;

                p = strjoina("net/", af_గమ్యం_sysctl_dir(af), "/conf/", ifname, "/", property);
        } లేకపోతే
                p = strjoina("net/", af_గమ్యం_sysctl_dir(af), "/", property);

        తిరిగి sysctl_write_నిండిన(p, విలువ, shadow);
}

పూర్ణాంకం sysctl_write_ip_property_boolean(పూర్ణాంకం af, స్థిరరాశి అక్షరం *ifname, స్థిరరాశి అక్షరం *property, bool విలువ, Hashmap **shadow) {
        తిరిగి sysctl_write_ip_property(af, ifname, property, one_zero(విలువ), shadow);
}

పూర్ణాంకం sysctl_write_ip_neighbor_property(పూర్ణాంకం af, స్థిరరాశి అక్షరం *ifname, స్థిరరాశి అక్షరం *property, స్థిరరాశి అక్షరం *విలువ, Hashmap **shadow) {
        స్థిరరాశి అక్షరం *p;

        ప్రతిజ్ఞా(property);
        ప్రతిజ్ఞా(విలువ);
        ప్రతిజ్ఞా(ifname);

        ఒకవేళ (!లోపల_సమితి(af, AF_INET, AF_INET6))
                తిరిగి -EAFNOSUPPORT;

        ఒకవేళ (ifname) {
                ఒకవేళ (!ifname_valid_నిండిన(ifname, IFNAME_VALID_SPECIAL))
                        తిరిగి -EINVAL;
                p = strjoina("net/", af_గమ్యం_ipv4_ipv6(af), "/neigh/", ifname, "/", property);
        } లేకపోతే
                p = strjoina("net/", af_గమ్యం_ipv4_ipv6(af), "/neigh/default/", property);

        తిరిగి sysctl_write_నిండిన(p, విలువ, shadow);
}

పూర్ణాంకం sysctl_write_ip_neighbor_property_uint32(పూర్ణాంకం af, స్థిరరాశి అక్షరం *ifname, స్థిరరాశి అక్షరం *property, uint32_t విలువ, Hashmap **shadow) {
        అక్షరం buf[DECIMAL_STR_MAX(uint32_t)];
        xsprintf(buf, "%u", విలువ);
        తిరిగి sysctl_write_ip_neighbor_property(af, ifname, property, buf, shadow);
}

పూర్ణాంకం sysctl_read(స్థిరరాశి అక్షరం *property, అక్షరం **ret) {
        అక్షరం *p;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(property);

        p = strjoina("/proc/sys/", property);

        మార్గం_simplify(p);
        ఒకవేళ (!మార్గం_is_normalized(p)) /* Filter out attempts to write to /proc/sys/../../…, just in case */
                తిరిగి -EINVAL;

        r = read_నిండిన_virtual_file(p, ret, ఖాళీ);
        ఒకవేళ (r < 0)
                తిరిగి r;
        ఒకవేళ (ret)
                delete_trailing_chars(*ret, NEWLINE);

        తిరిగి r;
}

పూర్ణాంకం sysctl_read_ip_property(పూర్ణాంకం af, స్థిరరాశి అక్షరం *ifname, స్థిరరాశి అక్షరం *property, అక్షరం **ret) {
        స్థిరరాశి అక్షరం *p;

        ప్రతిజ్ఞా(property);

        ఒకవేళ (!లోపల_సమితి(af, AF_INET, AF_INET6, AF_MPLS))
                తిరిగి -EAFNOSUPPORT;

        ఒకవేళ (ifname) {
                ఒకవేళ (!ifname_valid_నిండిన(ifname, IFNAME_VALID_SPECIAL))
                        తిరిగి -EINVAL;

                p = strjoina("net/", af_గమ్యం_sysctl_dir(af), "/conf/", ifname, "/", property);
        } లేకపోతే
                p = strjoina("net/", af_గమ్యం_sysctl_dir(af), "/", property);

        తిరిగి sysctl_read(p, ret);
}

పూర్ణాంకం sysctl_read_ip_property_int(పూర్ణాంకం af, స్థిరరాశి అక్షరం *ifname, స్థిరరాశి అక్షరం *property, పూర్ణాంకం *ret) {
        _cleanup_free_ అక్షరం *s = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(ret);

        r = sysctl_read_ip_property(af, ifname, property, &s);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి సురక్షిత_atoi(s, ret);
}

పూర్ణాంకం sysctl_read_ip_property_uint32(పూర్ణాంకం af, స్థిరరాశి అక్షరం *ifname, స్థిరరాశి అక్షరం *property, uint32_t *ret) {
        _cleanup_free_ అక్షరం *s = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(ret);

        r = sysctl_read_ip_property(af, ifname, property, &s);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి సురక్షిత_atou32(s, ret);
}
