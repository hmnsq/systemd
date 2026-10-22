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

        सचे (n && *n == '.')
                /* Dots become slashes and slashes become dots. Fun. */
                करोहि {
                        सचे (*n == '.')
                                *n = '/';
                        अञ्ञथा
                                *n = '.';

                        n = strpbrk(n + 1, "/.");
                } याव (n);

        path_simplify(s);

        /* Kill the leading slash, but keep the first character of the string in the same place. */
        सचे (s[0] == '/' && s[1] != 0)
                memmove(s, s+1, strlen(s));

        पटिनिवत्त s;
}

स्थिर पूर्णाङ्क shadow_update(Hashmap **shadow, ध्रुव वर्ण *property, ध्रुव वर्ण *मानम्) {
        _cleanup_free_ वर्ण *k = रिक्त, *v = रिक्त, *cur_k = रिक्त, *cur_v = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(property);
        प्रतिज्ञा(मानम्);

        सचे (!shadow)
                पटिनिवत्त 0;

        k = strdup(property);
        सचे (!k)
                पटिनिवत्त -ENOMEM;

        v = strdup(मानम्);
        सचे (!v)
                पटिनिवत्त -ENOMEM;

        cur_v = hashmap_remove2(*shadow, k, (शून्य**)&cur_k);

        r = hashmap_ensure_put(shadow, &path_hash_ops_free_free, k, v);
        सचे (r < 0) {
                प्रतिज्ञा(r != -EEXIST);

                पटिनिवत्त r;
        }

        TAKE_PTR(k);
        TAKE_PTR(v);

        पटिनिवत्त 0;
}

पूर्णाङ्क sysctl_write_full(ध्रुव वर्ण *property, ध्रुव वर्ण *मानम्, Hashmap **shadow) {
        वर्ण *p;
        पूर्णाङ्क r;

        प्रतिज्ञा(property);
        प्रतिज्ञा(मानम्);

        p = strjoina("/proc/sys/", property);

        path_simplify(p);
        सचे (!path_is_normalized(p))
                पटिनिवत्त -EINVAL;

        log_debug("Setting '%s' to '%s'", p, मानम्);

        r = shadow_update(shadow, p, मानम्);
        सचे (r < 0)
                पटिनिवत्त r;

        पटिनिवत्त write_string_file(p, मानम्, WRITE_STRING_FILE_VERIFY_ON_FAILURE | WRITE_STRING_FILE_DISABLE_BUFFER | WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL);
}

पूर्णाङ्क sysctl_writef(ध्रुव वर्ण *property, ध्रुव वर्ण *format, ...) {
        _cleanup_free_ वर्ण *v = रिक्त;
        va_list ap;
        पूर्णाङ्क r;

        va_start(ap, format);
        r = vasprintf(&v, format, ap);
        va_end(ap);

        सचे (r < 0)
                पटिनिवत्त -ENOMEM;

        पटिनिवत्त sysctl_write(property, v);
}

पूर्णाङ्क sysctl_write_verify(ध्रुव वर्ण *property, ध्रुव वर्ण *मानम्) {
        पूर्णाङ्क r;

        प्रतिज्ञा(property);
        प्रतिज्ञा(मानम्);

        /* Some sysctl settings accept invalid values on write, but refuses on read. E.g. coredump pattern,
         * be1e0283021ec73c2eb92839db9a471a068709d9 (v6.17) and 7d7c1fb85cba5627bbe741fb7539c709435e3848
         * (v6.16.8), which is fixed by a779e27f24aeb679969ddd1fdd7f636e22ddbc1e (v6.18) and
         * 304aa560385720baf3660fe8500f6dd425b63ea9 (v6.17.5). Let's first save the original value, and
         * restore to the saved value if the new value is refused. */

        _cleanup_free_ वर्ण *original = रिक्त;
        r = sysctl_read(property, &original);
        सचे (r >= 0 && streq(original, मानम्))
                पटिनिवत्त 0; /* Already set. */

        r = sysctl_write(property, मानम्);
        सचे (r >= 0) {
                _cleanup_free_ वर्ण *current = रिक्त;
                r = sysctl_read(property, &current);
                सचे (r >= 0) {
                        सचे (streq(current, मानम्))
                                पटिनिवत्त 0; /* Yay! */

                        /* At least for coredump pattern, this does not happen, but let's handle this as the
                         * same as we wrote something invalid. */
                        r = -EINVAL;
                }
        }

        सचे (original)
                (शून्य) sysctl_write(property, original);

        पटिनिवत्त r;
}

स्थिर ध्रुव वर्ण* af_to_sysctl_dir(पूर्णाङ्क af) {
        सचे (af == AF_MPLS)
                पटिनिवत्त "mpls";

        पटिनिवत्त af_to_ipv4_ipv6(af);
}

पूर्णाङ्क sysctl_write_ip_property(पूर्णाङ्क af, ध्रुव वर्ण *ifname, ध्रुव वर्ण *property, ध्रुव वर्ण *मानम्, Hashmap **shadow) {
        ध्रुव वर्ण *p;

        प्रतिज्ञा(property);
        प्रतिज्ञा(मानम्);

        सचे (!IN_SET(af, AF_INET, AF_INET6, AF_MPLS))
                पटिनिवत्त -EAFNOSUPPORT;

        सचे (ifname) {
                सचे (!ifname_valid_full(ifname, IFNAME_VALID_SPECIAL))
                        पटिनिवत्त -EINVAL;

                p = strjoina("net/", af_to_sysctl_dir(af), "/conf/", ifname, "/", property);
        } अञ्ञथा
                p = strjoina("net/", af_to_sysctl_dir(af), "/", property);

        पटिनिवत्त sysctl_write_full(p, मानम्, shadow);
}

पूर्णाङ्क sysctl_write_ip_property_boolean(पूर्णाङ्क af, ध्रुव वर्ण *ifname, ध्रुव वर्ण *property, bool मानम्, Hashmap **shadow) {
        पटिनिवत्त sysctl_write_ip_property(af, ifname, property, one_zero(मानम्), shadow);
}

पूर्णाङ्क sysctl_write_ip_neighbor_property(पूर्णाङ्क af, ध्रुव वर्ण *ifname, ध्रुव वर्ण *property, ध्रुव वर्ण *मानम्, Hashmap **shadow) {
        ध्रुव वर्ण *p;

        प्रतिज्ञा(property);
        प्रतिज्ञा(मानम्);
        प्रतिज्ञा(ifname);

        सचे (!IN_SET(af, AF_INET, AF_INET6))
                पटिनिवत्त -EAFNOSUPPORT;

        सचे (ifname) {
                सचे (!ifname_valid_full(ifname, IFNAME_VALID_SPECIAL))
                        पटिनिवत्त -EINVAL;
                p = strjoina("net/", af_to_ipv4_ipv6(af), "/neigh/", ifname, "/", property);
        } अञ्ञथा
                p = strjoina("net/", af_to_ipv4_ipv6(af), "/neigh/default/", property);

        पटिनिवत्त sysctl_write_full(p, मानम्, shadow);
}

पूर्णाङ्क sysctl_write_ip_neighbor_property_uint32(पूर्णाङ्क af, ध्रुव वर्ण *ifname, ध्रुव वर्ण *property, uint32_t मानम्, Hashmap **shadow) {
        वर्ण buf[DECIMAL_STR_MAX(uint32_t)];
        xsprintf(buf, "%u", मानम्);
        पटिनिवत्त sysctl_write_ip_neighbor_property(af, ifname, property, buf, shadow);
}

पूर्णाङ्क sysctl_read(ध्रुव वर्ण *property, वर्ण **ret) {
        वर्ण *p;
        पूर्णाङ्क r;

        प्रतिज्ञा(property);

        p = strjoina("/proc/sys/", property);

        path_simplify(p);
        सचे (!path_is_normalized(p)) /* Filter out attempts to write to /proc/sys/../../…, just in case */
                पटिनिवत्त -EINVAL;

        r = read_full_virtual_file(p, ret, रिक्त);
        सचे (r < 0)
                पटिनिवत्त r;
        सचे (ret)
                delete_trailing_chars(*ret, NEWLINE);

        पटिनिवत्त r;
}

पूर्णाङ्क sysctl_read_ip_property(पूर्णाङ्क af, ध्रुव वर्ण *ifname, ध्रुव वर्ण *property, वर्ण **ret) {
        ध्रुव वर्ण *p;

        प्रतिज्ञा(property);

        सचे (!IN_SET(af, AF_INET, AF_INET6, AF_MPLS))
                पटिनिवत्त -EAFNOSUPPORT;

        सचे (ifname) {
                सचे (!ifname_valid_full(ifname, IFNAME_VALID_SPECIAL))
                        पटिनिवत्त -EINVAL;

                p = strjoina("net/", af_to_sysctl_dir(af), "/conf/", ifname, "/", property);
        } अञ्ञथा
                p = strjoina("net/", af_to_sysctl_dir(af), "/", property);

        पटिनिवत्त sysctl_read(p, ret);
}

पूर्णाङ्क sysctl_read_ip_property_int(पूर्णाङ्क af, ध्रुव वर्ण *ifname, ध्रुव वर्ण *property, पूर्णाङ्क *ret) {
        _cleanup_free_ वर्ण *s = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        r = sysctl_read_ip_property(af, ifname, property, &s);
        सचे (r < 0)
                पटिनिवत्त r;

        पटिनिवत्त safe_atoi(s, ret);
}

पूर्णाङ्क sysctl_read_ip_property_uint32(पूर्णाङ्क af, ध्रुव वर्ण *ifname, ध्रुव वर्ण *property, uint32_t *ret) {
        _cleanup_free_ वर्ण *s = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        r = sysctl_read_ip_property(af, ifname, property, &s);
        सचे (r < 0)
                पटिनिवत्त r;

        पटिनिवत्त safe_atou32(s, ret);
}
