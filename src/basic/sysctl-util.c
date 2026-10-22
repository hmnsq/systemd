/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश <stdio.h>

#समावेश "af-list.h"
#समावेश "alloc-util.h"
#समावेश "fileio.h"
#समावेश "hashmap.h"
#समावेश "log.h"
#समावेश "parse-util.h"
#समावेश "path-util.h"
#समावेश "socket-util.h"
#समावेश "string-util.h"
#समावेश "sysctl-util.h"

अक्षर* sysctl_normalize(अक्षर *s) {
        अक्षर *n;

        निश्चित(s);

        n = strpbrk(s, "/.");

        /* If the first separator is a slash, the path is
         * assumed to be normalized and slashes remain slashes
         * and dots remains dots. */

        अगर (n && *n == '.')
                /* Dots become slashes and slashes become dots. Fun. */
                करो {
                        अगर (*n == '.')
                                *n = '/';
                        अन्यथा
                                *n = '.';

                        n = strpbrk(n + 1, "/.");
                } जबतक (n);

        path_simplify(s);

        /* Kill the leading slash, but keep the first character of the string in the same place. */
        अगर (s[0] == '/' && s[1] != 0)
                स_हटाओ(s, s+1, म_माप(s));

        वापस s;
}

अटल पूर्णांक shadow_update(Hashmap **shadow, स्थिर अक्षर *property, स्थिर अक्षर *मूल्य) {
        _cleanup_free_ अक्षर *k = शून्य, *v = शून्य, *cur_k = शून्य, *cur_v = शून्य;
        पूर्णांक r;

        निश्चित(property);
        निश्चित(मूल्य);

        अगर (!shadow)
                वापस 0;

        k = strdup(property);
        अगर (!k)
                वापस -ENOMEM;

        v = strdup(मूल्य);
        अगर (!v)
                वापस -ENOMEM;

        cur_v = hashmap_remove2(*shadow, k, (व्योम**)&cur_k);

        r = hashmap_ensure_put(shadow, &path_hash_ops_free_free, k, v);
        अगर (r < 0) {
                निश्चित(r != -EEXIST);

                वापस r;
        }

        TAKE_PTR(k);
        TAKE_PTR(v);

        वापस 0;
}

पूर्णांक sysctl_write_full(स्थिर अक्षर *property, स्थिर अक्षर *मूल्य, Hashmap **shadow) {
        अक्षर *p;
        पूर्णांक r;

        निश्चित(property);
        निश्चित(मूल्य);

        p = strjoina("/proc/sys/", property);

        path_simplify(p);
        अगर (!path_is_normalized(p))
                वापस -EINVAL;

        log_debug("Setting '%s' to '%s'", p, मूल्य);

        r = shadow_update(shadow, p, मूल्य);
        अगर (r < 0)
                वापस r;

        वापस write_string_file(p, मूल्य, WRITE_STRING_FILE_VERIFY_ON_FAILURE | WRITE_STRING_FILE_DISABLE_बफर | WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL);
}

पूर्णांक sysctl_writef(स्थिर अक्षर *property, स्थिर अक्षर *format, ...) {
        _cleanup_free_ अक्षर *v = शून्य;
        बहु_सूची ap;
        पूर्णांक r;

        बहु_शुरू(ap, format);
        r = vasprintf(&v, format, ap);
        बहु_पूर्ण(ap);

        अगर (r < 0)
                वापस -ENOMEM;

        वापस sysctl_write(property, v);
}

पूर्णांक sysctl_write_verify(स्थिर अक्षर *property, स्थिर अक्षर *मूल्य) {
        पूर्णांक r;

        निश्चित(property);
        निश्चित(मूल्य);

        /* Some sysctl settings accept invalid values on write, but refuses on read. E.g. coredump pattern,
         * be1e0283021ec73c2eb92839db9a471a068709d9 (v6.17) and 7d7c1fb85cba5627bbe741fb7539c709435e3848
         * (v6.16.8), which is fixed by a779e27f24aeb679969ddd1fdd7f636e22ddbc1e (v6.18) and
         * 304aa560385720baf3660fe8500f6dd425b63ea9 (v6.17.5). Let's first save the original value, and
         * restore to the saved value if the new value is refused. */

        _cleanup_free_ अक्षर *original = शून्य;
        r = sysctl_read(property, &original);
        अगर (r >= 0 && streq(original, मूल्य))
                वापस 0; /* Already set. */

        r = sysctl_write(property, मूल्य);
        अगर (r >= 0) {
                _cleanup_free_ अक्षर *current = शून्य;
                r = sysctl_read(property, &current);
                अगर (r >= 0) {
                        अगर (streq(current, मूल्य))
                                वापस 0; /* Yay! */

                        /* At least for coredump pattern, this does not happen, but let's handle this as the
                         * same as we wrote something invalid. */
                        r = -EINVAL;
                }
        }

        अगर (original)
                (व्योम) sysctl_write(property, original);

        वापस r;
}

अटल स्थिर अक्षर* af_to_sysctl_dir(पूर्णांक af) {
        अगर (af == AF_MPLS)
                वापस "mpls";

        वापस af_to_ipv4_ipv6(af);
}

पूर्णांक sysctl_write_ip_property(पूर्णांक af, स्थिर अक्षर *ifname, स्थिर अक्षर *property, स्थिर अक्षर *मूल्य, Hashmap **shadow) {
        स्थिर अक्षर *p;

        निश्चित(property);
        निश्चित(मूल्य);

        अगर (!IN_SET(af, AF_INET, AF_INET6, AF_MPLS))
                वापस -EAFNOSUPPORT;

        अगर (ifname) {
                अगर (!ifname_valid_full(ifname, IFNAME_VALID_SPECIAL))
                        वापस -EINVAL;

                p = strjoina("net/", af_to_sysctl_dir(af), "/conf/", ifname, "/", property);
        } अन्यथा
                p = strjoina("net/", af_to_sysctl_dir(af), "/", property);

        वापस sysctl_write_full(p, मूल्य, shadow);
}

पूर्णांक sysctl_write_ip_property_boolean(पूर्णांक af, स्थिर अक्षर *ifname, स्थिर अक्षर *property, bool मूल्य, Hashmap **shadow) {
        वापस sysctl_write_ip_property(af, ifname, property, one_zero(मूल्य), shadow);
}

पूर्णांक sysctl_write_ip_neighbor_property(पूर्णांक af, स्थिर अक्षर *ifname, स्थिर अक्षर *property, स्थिर अक्षर *मूल्य, Hashmap **shadow) {
        स्थिर अक्षर *p;

        निश्चित(property);
        निश्चित(मूल्य);
        निश्चित(ifname);

        अगर (!IN_SET(af, AF_INET, AF_INET6))
                वापस -EAFNOSUPPORT;

        अगर (ifname) {
                अगर (!ifname_valid_full(ifname, IFNAME_VALID_SPECIAL))
                        वापस -EINVAL;
                p = strjoina("net/", af_to_ipv4_ipv6(af), "/neigh/", ifname, "/", property);
        } अन्यथा
                p = strjoina("net/", af_to_ipv4_ipv6(af), "/neigh/default/", property);

        वापस sysctl_write_full(p, मूल्य, shadow);
}

पूर्णांक sysctl_write_ip_neighbor_property_uint32(पूर्णांक af, स्थिर अक्षर *ifname, स्थिर अक्षर *property, uint32_t मूल्य, Hashmap **shadow) {
        अक्षर buf[DECIMAL_STR_MAX(uint32_t)];
        xsprintf(buf, "%u", मूल्य);
        वापस sysctl_write_ip_neighbor_property(af, ifname, property, buf, shadow);
}

पूर्णांक sysctl_read(स्थिर अक्षर *property, अक्षर **ret) {
        अक्षर *p;
        पूर्णांक r;

        निश्चित(property);

        p = strjoina("/proc/sys/", property);

        path_simplify(p);
        अगर (!path_is_normalized(p)) /* Filter out attempts to write to /proc/sys/../../…, just in case */
                वापस -EINVAL;

        r = read_full_virtual_file(p, ret, शून्य);
        अगर (r < 0)
                वापस r;
        अगर (ret)
                delete_trailing_chars(*ret, NEWLINE);

        वापस r;
}

पूर्णांक sysctl_read_ip_property(पूर्णांक af, स्थिर अक्षर *ifname, स्थिर अक्षर *property, अक्षर **ret) {
        स्थिर अक्षर *p;

        निश्चित(property);

        अगर (!IN_SET(af, AF_INET, AF_INET6, AF_MPLS))
                वापस -EAFNOSUPPORT;

        अगर (ifname) {
                अगर (!ifname_valid_full(ifname, IFNAME_VALID_SPECIAL))
                        वापस -EINVAL;

                p = strjoina("net/", af_to_sysctl_dir(af), "/conf/", ifname, "/", property);
        } अन्यथा
                p = strjoina("net/", af_to_sysctl_dir(af), "/", property);

        वापस sysctl_read(p, ret);
}

पूर्णांक sysctl_read_ip_property_int(पूर्णांक af, स्थिर अक्षर *ifname, स्थिर अक्षर *property, पूर्णांक *ret) {
        _cleanup_free_ अक्षर *s = शून्य;
        पूर्णांक r;

        निश्चित(ret);

        r = sysctl_read_ip_property(af, ifname, property, &s);
        अगर (r < 0)
                वापस r;

        वापस safe_atoi(s, ret);
}

पूर्णांक sysctl_read_ip_property_uint32(पूर्णांक af, स्थिर अक्षर *ifname, स्थिर अक्षर *property, uint32_t *ret) {
        _cleanup_free_ अक्षर *s = शून्य;
        पूर्णांक r;

        निश्चित(ret);

        r = sysctl_read_ip_property(af, ifname, property, &s);
        अगर (r < 0)
                वापस r;

        वापस safe_atou32(s, ret);
}
