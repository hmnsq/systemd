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

अक्षर* sysctl_normalize(अक्षर *s) {
        अक्षर *n;

        प्रतिज्ञा(s);

        n = strpbrk(s, "/.");

        /* If the first separator is a slash, the path is
         * assumed to be normalized and slashes remain slashes
         * and dots remains dots. */

        जर (n && *n == '.')
                /* Dots become slashes and slashes become dots. Fun. */
                करा {
                        जर (*n == '.')
                                *n = '/';
                        अन्यथा
                                *n = '.';

                        n = strpbrk(n + 1, "/.");
                } पर्यंत (n);

        path_simplify(s);

        /* Kill the leading slash, but keep the first character of the string in the same place. */
        जर (s[0] == '/' && s[1] != 0)
                स_हटाओ(s, s+1, म_माप(s));

        परत s;
}

स्थिर पूर्णांक shadow_update(Hashmap **shadow, स्थिरांक अक्षर *property, स्थिरांक अक्षर *मूल्य) {
        _cleanup_free_ अक्षर *k = रिक्त, *v = रिक्त, *cur_k = रिक्त, *cur_v = रिक्त;
        पूर्णांक r;

        प्रतिज्ञा(property);
        प्रतिज्ञा(मूल्य);

        जर (!shadow)
                परत 0;

        k = strdup(property);
        जर (!k)
                परत -ENOMEM;

        v = strdup(मूल्य);
        जर (!v)
                परत -ENOMEM;

        cur_v = hashmap_remove2(*shadow, k, (शून्य**)&cur_k);

        r = hashmap_ensure_put(shadow, &path_hash_ops_free_free, k, v);
        जर (r < 0) {
                प्रतिज्ञा(r != -EEXIST);

                परत r;
        }

        TAKE_PTR(k);
        TAKE_PTR(v);

        परत 0;
}

पूर्णांक sysctl_write_full(स्थिरांक अक्षर *property, स्थिरांक अक्षर *मूल्य, Hashmap **shadow) {
        अक्षर *p;
        पूर्णांक r;

        प्रतिज्ञा(property);
        प्रतिज्ञा(मूल्य);

        p = strjoina("/proc/sys/", property);

        path_simplify(p);
        जर (!path_is_normalized(p))
                परत -EINVAL;

        log_debug("Setting '%s' to '%s'", p, मूल्य);

        r = shadow_update(shadow, p, मूल्य);
        जर (r < 0)
                परत r;

        परत write_string_file(p, मूल्य, WRITE_STRING_FILE_VERIFY_ON_FAILURE | WRITE_STRING_FILE_DISABLE_BUFFER | WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL);
}

पूर्णांक sysctl_writef(स्थिरांक अक्षर *property, स्थिरांक अक्षर *format, ...) {
        _cleanup_free_ अक्षर *v = रिक्त;
        बहु_सूची ap;
        पूर्णांक r;

        बहु_शुरू(ap, format);
        r = vasprintf(&v, format, ap);
        बहु_पूर्ण(ap);

        जर (r < 0)
                परत -ENOMEM;

        परत sysctl_write(property, v);
}

पूर्णांक sysctl_write_verify(स्थिरांक अक्षर *property, स्थिरांक अक्षर *मूल्य) {
        पूर्णांक r;

        प्रतिज्ञा(property);
        प्रतिज्ञा(मूल्य);

        /* Some sysctl settings accept invalid values on write, but refuses on read. E.g. coredump pattern,
         * be1e0283021ec73c2eb92839db9a471a068709d9 (v6.17) and 7d7c1fb85cba5627bbe741fb7539c709435e3848
         * (v6.16.8), which is fixed by a779e27f24aeb679969ddd1fdd7f636e22ddbc1e (v6.18) and
         * 304aa560385720baf3660fe8500f6dd425b63ea9 (v6.17.5). Let's first save the original value, and
         * restore to the saved value if the new value is refused. */

        _cleanup_free_ अक्षर *original = रिक्त;
        r = sysctl_read(property, &original);
        जर (r >= 0 && streq(original, मूल्य))
                परत 0; /* Already set. */

        r = sysctl_write(property, मूल्य);
        जर (r >= 0) {
                _cleanup_free_ अक्षर *current = रिक्त;
                r = sysctl_read(property, &current);
                जर (r >= 0) {
                        जर (streq(current, मूल्य))
                                परत 0; /* Yay! */

                        /* At least for coredump pattern, this does not happen, but let's handle this as the
                         * same as we wrote something invalid. */
                        r = -EINVAL;
                }
        }

        जर (original)
                (शून्य) sysctl_write(property, original);

        परत r;
}

स्थिर स्थिरांक अक्षर* af_to_sysctl_dir(पूर्णांक af) {
        जर (af == AF_MPLS)
                परत "mpls";

        परत af_to_ipv4_ipv6(af);
}

पूर्णांक sysctl_write_ip_property(पूर्णांक af, स्थिरांक अक्षर *ifname, स्थिरांक अक्षर *property, स्थिरांक अक्षर *मूल्य, Hashmap **shadow) {
        स्थिरांक अक्षर *p;

        प्रतिज्ञा(property);
        प्रतिज्ञा(मूल्य);

        जर (!IN_SET(af, AF_INET, AF_INET6, AF_MPLS))
                परत -EAFNOSUPPORT;

        जर (ifname) {
                जर (!ifname_valid_full(ifname, IFNAME_VALID_SPECIAL))
                        परत -EINVAL;

                p = strjoina("net/", af_to_sysctl_dir(af), "/conf/", ifname, "/", property);
        } अन्यथा
                p = strjoina("net/", af_to_sysctl_dir(af), "/", property);

        परत sysctl_write_full(p, मूल्य, shadow);
}

पूर्णांक sysctl_write_ip_property_boolean(पूर्णांक af, स्थिरांक अक्षर *ifname, स्थिरांक अक्षर *property, bool मूल्य, Hashmap **shadow) {
        परत sysctl_write_ip_property(af, ifname, property, one_zero(मूल्य), shadow);
}

पूर्णांक sysctl_write_ip_neighbor_property(पूर्णांक af, स्थिरांक अक्षर *ifname, स्थिरांक अक्षर *property, स्थिरांक अक्षर *मूल्य, Hashmap **shadow) {
        स्थिरांक अक्षर *p;

        प्रतिज्ञा(property);
        प्रतिज्ञा(मूल्य);
        प्रतिज्ञा(ifname);

        जर (!IN_SET(af, AF_INET, AF_INET6))
                परत -EAFNOSUPPORT;

        जर (ifname) {
                जर (!ifname_valid_full(ifname, IFNAME_VALID_SPECIAL))
                        परत -EINVAL;
                p = strjoina("net/", af_to_ipv4_ipv6(af), "/neigh/", ifname, "/", property);
        } अन्यथा
                p = strjoina("net/", af_to_ipv4_ipv6(af), "/neigh/default/", property);

        परत sysctl_write_full(p, मूल्य, shadow);
}

पूर्णांक sysctl_write_ip_neighbor_property_uint32(पूर्णांक af, स्थिरांक अक्षर *ifname, स्थिरांक अक्षर *property, uint32_t मूल्य, Hashmap **shadow) {
        अक्षर buf[DECIMAL_STR_MAX(uint32_t)];
        xsprintf(buf, "%u", मूल्य);
        परत sysctl_write_ip_neighbor_property(af, ifname, property, buf, shadow);
}

पूर्णांक sysctl_read(स्थिरांक अक्षर *property, अक्षर **ret) {
        अक्षर *p;
        पूर्णांक r;

        प्रतिज्ञा(property);

        p = strjoina("/proc/sys/", property);

        path_simplify(p);
        जर (!path_is_normalized(p)) /* Filter out attempts to write to /proc/sys/../../…, just in case */
                परत -EINVAL;

        r = read_full_virtual_file(p, ret, रिक्त);
        जर (r < 0)
                परत r;
        जर (ret)
                delete_trailing_chars(*ret, NEWLINE);

        परत r;
}

पूर्णांक sysctl_read_ip_property(पूर्णांक af, स्थिरांक अक्षर *ifname, स्थिरांक अक्षर *property, अक्षर **ret) {
        स्थिरांक अक्षर *p;

        प्रतिज्ञा(property);

        जर (!IN_SET(af, AF_INET, AF_INET6, AF_MPLS))
                परत -EAFNOSUPPORT;

        जर (ifname) {
                जर (!ifname_valid_full(ifname, IFNAME_VALID_SPECIAL))
                        परत -EINVAL;

                p = strjoina("net/", af_to_sysctl_dir(af), "/conf/", ifname, "/", property);
        } अन्यथा
                p = strjoina("net/", af_to_sysctl_dir(af), "/", property);

        परत sysctl_read(p, ret);
}

पूर्णांक sysctl_read_ip_property_int(पूर्णांक af, स्थिरांक अक्षर *ifname, स्थिरांक अक्षर *property, पूर्णांक *ret) {
        _cleanup_free_ अक्षर *s = रिक्त;
        पूर्णांक r;

        प्रतिज्ञा(ret);

        r = sysctl_read_ip_property(af, ifname, property, &s);
        जर (r < 0)
                परत r;

        परत safe_atoi(s, ret);
}

पूर्णांक sysctl_read_ip_property_uint32(पूर्णांक af, स्थिरांक अक्षर *ifname, स्थिरांक अक्षर *property, uint32_t *ret) {
        _cleanup_free_ अक्षर *s = रिक्त;
        पूर्णांक r;

        प्रतिज्ञा(ret);

        r = sysctl_read_ip_property(af, ifname, property, &s);
        जर (r < 0)
                परत r;

        परत safe_atou32(s, ret);
}
