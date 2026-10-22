/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <stdio.h>

#अन्तर्भाव "af-list.h"
#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "fileio.h"
#अन्तर्भाव "hashmap.h"
#अन्तर्भाव "log.h"
#अन्तर्भाव "parse-util.h"
#अन्तर्भाव "path-util.h"
#अन्तर्भाव "socket-util.h"
#अन्तर्भाव "string-util.h"
#अन्तर्भाव "sysctl-util.h"

वर्ण* sysctl_normalize(वर्ण *s) {
        वर्ण *n;

        प्रतिज्ञा(s);

        n = strpbrk(s, "/.");

        /* If the first separator is a slash, the path is
         * assumed to be normalized and slashes remain slashes
         * and dots remains dots. */

        यदि (n && *n == '.')
                /* Dots become slashes and slashes become dots. Fun. */
                कुरु {
                        यदि (*n == '.')
                                *n = '/';
                        अन्यथा
                                *n = '.';

                        n = strpbrk(n + 1, "/.");
                } यावत् (n);

        path_simplify(s);

        /* Kill the leading slash, but keep the first character of the string in the same place. */
        यदि (s[0] == '/' && s[1] != 0)
                memmove(s, s+1, strlen(s));

        प्रत्यागम s;
}

स्थिर पूर्णाङ्क shadow_update(Hashmap **shadow, ध्रुव वर्ण *property, ध्रुव वर्ण *मूल्य) {
        _cleanup_free_ वर्ण *k = रिक्त, *v = रिक्त, *cur_k = रिक्त, *cur_v = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(property);
        प्रतिज्ञा(मूल्य);

        यदि (!shadow)
                प्रत्यागम 0;

        k = strdup(property);
        यदि (!k)
                प्रत्यागम -ENOMEM;

        v = strdup(मूल्य);
        यदि (!v)
                प्रत्यागम -ENOMEM;

        cur_v = hashmap_remove2(*shadow, k, (शून्य**)&cur_k);

        r = hashmap_ensure_put(shadow, &path_hash_ops_free_free, k, v);
        यदि (r < 0) {
                प्रतिज्ञा(r != -EEXIST);

                प्रत्यागम r;
        }

        TAKE_PTR(k);
        TAKE_PTR(v);

        प्रत्यागम 0;
}

पूर्णाङ्क sysctl_write_full(ध्रुव वर्ण *property, ध्रुव वर्ण *मूल्य, Hashmap **shadow) {
        वर्ण *p;
        पूर्णाङ्क r;

        प्रतिज्ञा(property);
        प्रतिज्ञा(मूल्य);

        p = strjoina("/proc/sys/", property);

        path_simplify(p);
        यदि (!path_is_normalized(p))
                प्रत्यागम -EINVAL;

        log_debug("Setting '%s' to '%s'", p, मूल्य);

        r = shadow_update(shadow, p, मूल्य);
        यदि (r < 0)
                प्रत्यागम r;

        प्रत्यागम write_string_file(p, मूल्य, WRITE_STRING_FILE_VERIFY_ON_FAILURE | WRITE_STRING_FILE_DISABLE_BUFFER | WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL);
}

पूर्णाङ्क sysctl_writef(ध्रुव वर्ण *property, ध्रुव वर्ण *format, ...) {
        _cleanup_free_ वर्ण *v = रिक्त;
        va_list ap;
        पूर्णाङ्क r;

        va_start(ap, format);
        r = vasprintf(&v, format, ap);
        va_end(ap);

        यदि (r < 0)
                प्रत्यागम -ENOMEM;

        प्रत्यागम sysctl_write(property, v);
}

पूर्णाङ्क sysctl_write_verify(ध्रुव वर्ण *property, ध्रुव वर्ण *मूल्य) {
        पूर्णाङ्क r;

        प्रतिज्ञा(property);
        प्रतिज्ञा(मूल्य);

        /* Some sysctl settings accept invalid values on write, but refuses on read. E.g. coredump pattern,
         * be1e0283021ec73c2eb92839db9a471a068709d9 (v6.17) and 7d7c1fb85cba5627bbe741fb7539c709435e3848
         * (v6.16.8), which is fixed by a779e27f24aeb679969ddd1fdd7f636e22ddbc1e (v6.18) and
         * 304aa560385720baf3660fe8500f6dd425b63ea9 (v6.17.5). Let's first save the original value, and
         * restore to the saved value if the new value is refused. */

        _cleanup_free_ वर्ण *original = रिक्त;
        r = sysctl_read(property, &original);
        यदि (r >= 0 && streq(original, मूल्य))
                प्रत्यागम 0; /* Already set. */

        r = sysctl_write(property, मूल्य);
        यदि (r >= 0) {
                _cleanup_free_ वर्ण *current = रिक्त;
                r = sysctl_read(property, &current);
                यदि (r >= 0) {
                        यदि (streq(current, मूल्य))
                                प्रत्यागम 0; /* Yay! */

                        /* At least for coredump pattern, this does not happen, but let's handle this as the
                         * same as we wrote something invalid. */
                        r = -EINVAL;
                }
        }

        यदि (original)
                (शून्य) sysctl_write(property, original);

        प्रत्यागम r;
}

स्थिर ध्रुव वर्ण* af_to_sysctl_dir(पूर्णाङ्क af) {
        यदि (af == AF_MPLS)
                प्रत्यागम "mpls";

        प्रत्यागम af_to_ipv4_ipv6(af);
}

पूर्णाङ्क sysctl_write_ip_property(पूर्णाङ्क af, ध्रुव वर्ण *ifname, ध्रुव वर्ण *property, ध्रुव वर्ण *मूल्य, Hashmap **shadow) {
        ध्रुव वर्ण *p;

        प्रतिज्ञा(property);
        प्रतिज्ञा(मूल्य);

        यदि (!IN_SET(af, AF_INET, AF_INET6, AF_MPLS))
                प्रत्यागम -EAFNOSUPPORT;

        यदि (ifname) {
                यदि (!ifname_valid_full(ifname, IFNAME_VALID_SPECIAL))
                        प्रत्यागम -EINVAL;

                p = strjoina("net/", af_to_sysctl_dir(af), "/conf/", ifname, "/", property);
        } अन्यथा
                p = strjoina("net/", af_to_sysctl_dir(af), "/", property);

        प्रत्यागम sysctl_write_full(p, मूल्य, shadow);
}

पूर्णाङ्क sysctl_write_ip_property_boolean(पूर्णाङ्क af, ध्रुव वर्ण *ifname, ध्रुव वर्ण *property, bool मूल्य, Hashmap **shadow) {
        प्रत्यागम sysctl_write_ip_property(af, ifname, property, one_zero(मूल्य), shadow);
}

पूर्णाङ्क sysctl_write_ip_neighbor_property(पूर्णाङ्क af, ध्रुव वर्ण *ifname, ध्रुव वर्ण *property, ध्रुव वर्ण *मूल्य, Hashmap **shadow) {
        ध्रुव वर्ण *p;

        प्रतिज्ञा(property);
        प्रतिज्ञा(मूल्य);
        प्रतिज्ञा(ifname);

        यदि (!IN_SET(af, AF_INET, AF_INET6))
                प्रत्यागम -EAFNOSUPPORT;

        यदि (ifname) {
                यदि (!ifname_valid_full(ifname, IFNAME_VALID_SPECIAL))
                        प्रत्यागम -EINVAL;
                p = strjoina("net/", af_to_ipv4_ipv6(af), "/neigh/", ifname, "/", property);
        } अन्यथा
                p = strjoina("net/", af_to_ipv4_ipv6(af), "/neigh/default/", property);

        प्रत्यागम sysctl_write_full(p, मूल्य, shadow);
}

पूर्णाङ्क sysctl_write_ip_neighbor_property_uint32(पूर्णाङ्क af, ध्रुव वर्ण *ifname, ध्रुव वर्ण *property, uint32_t मूल्य, Hashmap **shadow) {
        वर्ण buf[DECIMAL_STR_MAX(uint32_t)];
        xsprintf(buf, "%u", मूल्य);
        प्रत्यागम sysctl_write_ip_neighbor_property(af, ifname, property, buf, shadow);
}

पूर्णाङ्क sysctl_read(ध्रुव वर्ण *property, वर्ण **ret) {
        वर्ण *p;
        पूर्णाङ्क r;

        प्रतिज्ञा(property);

        p = strjoina("/proc/sys/", property);

        path_simplify(p);
        यदि (!path_is_normalized(p)) /* Filter out attempts to write to /proc/sys/../../…, just in case */
                प्रत्यागम -EINVAL;

        r = read_full_virtual_file(p, ret, रिक्त);
        यदि (r < 0)
                प्रत्यागम r;
        यदि (ret)
                delete_trailing_chars(*ret, NEWLINE);

        प्रत्यागम r;
}

पूर्णाङ्क sysctl_read_ip_property(पूर्णाङ्क af, ध्रुव वर्ण *ifname, ध्रुव वर्ण *property, वर्ण **ret) {
        ध्रुव वर्ण *p;

        प्रतिज्ञा(property);

        यदि (!IN_SET(af, AF_INET, AF_INET6, AF_MPLS))
                प्रत्यागम -EAFNOSUPPORT;

        यदि (ifname) {
                यदि (!ifname_valid_full(ifname, IFNAME_VALID_SPECIAL))
                        प्रत्यागम -EINVAL;

                p = strjoina("net/", af_to_sysctl_dir(af), "/conf/", ifname, "/", property);
        } अन्यथा
                p = strjoina("net/", af_to_sysctl_dir(af), "/", property);

        प्रत्यागम sysctl_read(p, ret);
}

पूर्णाङ्क sysctl_read_ip_property_int(पूर्णाङ्क af, ध्रुव वर्ण *ifname, ध्रुव वर्ण *property, पूर्णाङ्क *ret) {
        _cleanup_free_ वर्ण *s = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        r = sysctl_read_ip_property(af, ifname, property, &s);
        यदि (r < 0)
                प्रत्यागम r;

        प्रत्यागम safe_atoi(s, ret);
}

पूर्णाङ्क sysctl_read_ip_property_uint32(पूर्णाङ्क af, ध्रुव वर्ण *ifname, ध्रुव वर्ण *property, uint32_t *ret) {
        _cleanup_free_ वर्ण *s = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        r = sysctl_read_ip_property(af, ifname, property, &s);
        यदि (r < 0)
                प्रत्यागम r;

        प्रत्यागम safe_atou32(s, ret);
}
