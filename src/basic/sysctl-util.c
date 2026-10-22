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
                गर {
                        यदि (*n == '.')
                                *n = '/';
                        अन्यथा
                                *n = '.';

                        n = strpbrk(n + 1, "/.");
                } जबसम्म (n);

        path_simplify(s);

        /* Kill the leading slash, but keep the first character of the string in the same place. */
        यदि (s[0] == '/' && s[1] != 0)
                स_हटाओ(s, s+1, म_माप(s));

        फर्काऊ s;
}

स्थिर पूर्णाङ्क shadow_update(Hashmap **shadow, अचर वर्ण *property, अचर वर्ण *मान) {
        _cleanup_free_ वर्ण *k = रिक्त, *v = रिक्त, *cur_k = रिक्त, *cur_v = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(property);
        प्रतिज्ञा(मान);

        यदि (!shadow)
                फर्काऊ 0;

        k = strdup(property);
        यदि (!k)
                फर्काऊ -ENOMEM;

        v = strdup(मान);
        यदि (!v)
                फर्काऊ -ENOMEM;

        cur_v = hashmap_remove2(*shadow, k, (शून्य**)&cur_k);

        r = hashmap_ensure_put(shadow, &path_hash_ops_free_free, k, v);
        यदि (r < 0) {
                प्रतिज्ञा(r != -EEXIST);

                फर्काऊ r;
        }

        TAKE_PTR(k);
        TAKE_PTR(v);

        फर्काऊ 0;
}

पूर्णाङ्क sysctl_write_full(अचर वर्ण *property, अचर वर्ण *मान, Hashmap **shadow) {
        वर्ण *p;
        पूर्णाङ्क r;

        प्रतिज्ञा(property);
        प्रतिज्ञा(मान);

        p = strjoina("/proc/sys/", property);

        path_simplify(p);
        यदि (!path_is_normalized(p))
                फर्काऊ -EINVAL;

        log_debug("Setting '%s' to '%s'", p, मान);

        r = shadow_update(shadow, p, मान);
        यदि (r < 0)
                फर्काऊ r;

        फर्काऊ write_string_file(p, मान, WRITE_STRING_FILE_VERIFY_ON_FAILURE | WRITE_STRING_FILE_DISABLE_BUFFER | WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL);
}

पूर्णाङ्क sysctl_writef(अचर वर्ण *property, अचर वर्ण *format, ...) {
        _cleanup_free_ वर्ण *v = रिक्त;
        बहु_सूची ap;
        पूर्णाङ्क r;

        बहु_शुरू(ap, format);
        r = vasprintf(&v, format, ap);
        बहु_पूर्ण(ap);

        यदि (r < 0)
                फर्काऊ -ENOMEM;

        फर्काऊ sysctl_write(property, v);
}

पूर्णाङ्क sysctl_write_verify(अचर वर्ण *property, अचर वर्ण *मान) {
        पूर्णाङ्क r;

        प्रतिज्ञा(property);
        प्रतिज्ञा(मान);

        /* Some sysctl settings accept invalid values on write, but refuses on read. E.g. coredump pattern,
         * be1e0283021ec73c2eb92839db9a471a068709d9 (v6.17) and 7d7c1fb85cba5627bbe741fb7539c709435e3848
         * (v6.16.8), which is fixed by a779e27f24aeb679969ddd1fdd7f636e22ddbc1e (v6.18) and
         * 304aa560385720baf3660fe8500f6dd425b63ea9 (v6.17.5). Let's first save the original value, and
         * restore to the saved value if the new value is refused. */

        _cleanup_free_ वर्ण *original = रिक्त;
        r = sysctl_read(property, &original);
        यदि (r >= 0 && streq(original, मान))
                फर्काऊ 0; /* Already set. */

        r = sysctl_write(property, मान);
        यदि (r >= 0) {
                _cleanup_free_ वर्ण *current = रिक्त;
                r = sysctl_read(property, &current);
                यदि (r >= 0) {
                        यदि (streq(current, मान))
                                फर्काऊ 0; /* Yay! */

                        /* At least for coredump pattern, this does not happen, but let's handle this as the
                         * same as we wrote something invalid. */
                        r = -EINVAL;
                }
        }

        यदि (original)
                (शून्य) sysctl_write(property, original);

        फर्काऊ r;
}

स्थिर अचर वर्ण* af_to_sysctl_dir(पूर्णाङ्क af) {
        यदि (af == AF_MPLS)
                फर्काऊ "mpls";

        फर्काऊ af_to_ipv4_ipv6(af);
}

पूर्णाङ्क sysctl_write_ip_property(पूर्णाङ्क af, अचर वर्ण *ifname, अचर वर्ण *property, अचर वर्ण *मान, Hashmap **shadow) {
        अचर वर्ण *p;

        प्रतिज्ञा(property);
        प्रतिज्ञा(मान);

        यदि (!IN_SET(af, AF_INET, AF_INET6, AF_MPLS))
                फर्काऊ -EAFNOSUPPORT;

        यदि (ifname) {
                यदि (!ifname_valid_full(ifname, IFNAME_VALID_SPECIAL))
                        फर्काऊ -EINVAL;

                p = strjoina("net/", af_to_sysctl_dir(af), "/conf/", ifname, "/", property);
        } अन्यथा
                p = strjoina("net/", af_to_sysctl_dir(af), "/", property);

        फर्काऊ sysctl_write_full(p, मान, shadow);
}

पूर्णाङ्क sysctl_write_ip_property_boolean(पूर्णाङ्क af, अचर वर्ण *ifname, अचर वर्ण *property, bool मान, Hashmap **shadow) {
        फर्काऊ sysctl_write_ip_property(af, ifname, property, one_zero(मान), shadow);
}

पूर्णाङ्क sysctl_write_ip_neighbor_property(पूर्णाङ्क af, अचर वर्ण *ifname, अचर वर्ण *property, अचर वर्ण *मान, Hashmap **shadow) {
        अचर वर्ण *p;

        प्रतिज्ञा(property);
        प्रतिज्ञा(मान);
        प्रतिज्ञा(ifname);

        यदि (!IN_SET(af, AF_INET, AF_INET6))
                फर्काऊ -EAFNOSUPPORT;

        यदि (ifname) {
                यदि (!ifname_valid_full(ifname, IFNAME_VALID_SPECIAL))
                        फर्काऊ -EINVAL;
                p = strjoina("net/", af_to_ipv4_ipv6(af), "/neigh/", ifname, "/", property);
        } अन्यथा
                p = strjoina("net/", af_to_ipv4_ipv6(af), "/neigh/default/", property);

        फर्काऊ sysctl_write_full(p, मान, shadow);
}

पूर्णाङ्क sysctl_write_ip_neighbor_property_uint32(पूर्णाङ्क af, अचर वर्ण *ifname, अचर वर्ण *property, uint32_t मान, Hashmap **shadow) {
        वर्ण buf[DECIMAL_STR_MAX(uint32_t)];
        xsprintf(buf, "%u", मान);
        फर्काऊ sysctl_write_ip_neighbor_property(af, ifname, property, buf, shadow);
}

पूर्णाङ्क sysctl_read(अचर वर्ण *property, वर्ण **ret) {
        वर्ण *p;
        पूर्णाङ्क r;

        प्रतिज्ञा(property);

        p = strjoina("/proc/sys/", property);

        path_simplify(p);
        यदि (!path_is_normalized(p)) /* Filter out attempts to write to /proc/sys/../../…, just in case */
                फर्काऊ -EINVAL;

        r = read_full_virtual_file(p, ret, रिक्त);
        यदि (r < 0)
                फर्काऊ r;
        यदि (ret)
                delete_trailing_chars(*ret, NEWLINE);

        फर्काऊ r;
}

पूर्णाङ्क sysctl_read_ip_property(पूर्णाङ्क af, अचर वर्ण *ifname, अचर वर्ण *property, वर्ण **ret) {
        अचर वर्ण *p;

        प्रतिज्ञा(property);

        यदि (!IN_SET(af, AF_INET, AF_INET6, AF_MPLS))
                फर्काऊ -EAFNOSUPPORT;

        यदि (ifname) {
                यदि (!ifname_valid_full(ifname, IFNAME_VALID_SPECIAL))
                        फर्काऊ -EINVAL;

                p = strjoina("net/", af_to_sysctl_dir(af), "/conf/", ifname, "/", property);
        } अन्यथा
                p = strjoina("net/", af_to_sysctl_dir(af), "/", property);

        फर्काऊ sysctl_read(p, ret);
}

पूर्णाङ्क sysctl_read_ip_property_int(पूर्णाङ्क af, अचर वर्ण *ifname, अचर वर्ण *property, पूर्णाङ्क *ret) {
        _cleanup_free_ वर्ण *s = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        r = sysctl_read_ip_property(af, ifname, property, &s);
        यदि (r < 0)
                फर्काऊ r;

        फर्काऊ safe_atoi(s, ret);
}

पूर्णाङ्क sysctl_read_ip_property_uint32(पूर्णाङ्क af, अचर वर्ण *ifname, अचर वर्ण *property, uint32_t *ret) {
        _cleanup_free_ वर्ण *s = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        r = sysctl_read_ip_property(af, ifname, property, &s);
        यदि (r < 0)
                फर्काऊ r;

        फर्काऊ safe_atou32(s, ret);
}
