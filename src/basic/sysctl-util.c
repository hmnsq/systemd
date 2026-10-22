/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <stdio.h>

#include "af-list.h"
#include "alloc-util.h"
#include "fileio.h"
#include "hashmap.h"
#include "log.h"
#include "parse-util.h"
#include "path-util.h"
#include "socket-util.h"
#include "string-util.h"
#include "sysctl-util.h"

char* sysctl_normalize(char *s) {
        char *n;

        assert(s);

        n = strpbrk(s, "/.");

        /* If the first separator is a slash, the path is
         * assumed to be normalized and slashes remain slashes
         * and dots remains dots. */

        nếu (n && *n == '.')
                /* Dots become slashes and slashes become dots. Fun. */
                do {
                        nếu (*n == '.')
                                *n = '/';
                        không_thì
                                *n = '.';

                        n = strpbrk(n + 1, "/.");
                } trong_khi (n);

        path_simplify(s);

        /* Kill the leading slash, but keep the first character of the string in the same place. */
        nếu (s[0] == '/' && s[1] != 0)
                memmove(s, s+1, strlen(s));

        trả_về s;
}

static số_nguyên shadow_update(Hashmap **shadow, const char *property, const char *value) {
        _cleanup_free_ char *k = NULL, *v = NULL, *cur_k = NULL, *cur_v = NULL;
        số_nguyên r;

        assert(property);
        assert(value);

        nếu (!shadow)
                trả_về 0;

        k = strdup(property);
        nếu (!k)
                trả_về -ENOMEM;

        v = strdup(value);
        nếu (!v)
                trả_về -ENOMEM;

        cur_v = hashmap_remove2(*shadow, k, (void**)&cur_k);

        r = hashmap_ensure_put(shadow, &path_hash_ops_free_free, k, v);
        nếu (r < 0) {
                assert(r != -EEXIST);

                trả_về r;
        }

        TAKE_PTR(k);
        TAKE_PTR(v);

        trả_về 0;
}

số_nguyên sysctl_write_full(const char *property, const char *value, Hashmap **shadow) {
        char *p;
        số_nguyên r;

        assert(property);
        assert(value);

        p = strjoina("/proc/sys/", property);

        path_simplify(p);
        nếu (!path_is_normalized(p))
                trả_về -EINVAL;

        log_debug("Setting '%s' to '%s'", p, value);

        r = shadow_update(shadow, p, value);
        nếu (r < 0)
                trả_về r;

        trả_về write_string_file(p, value, WRITE_STRING_FILE_VERIFY_ON_FAILURE | WRITE_STRING_FILE_DISABLE_BUFFER | WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL);
}

số_nguyên sysctl_writef(const char *property, const char *format, ...) {
        _cleanup_free_ char *v = NULL;
        va_list ap;
        số_nguyên r;

        va_start(ap, format);
        r = vasprintf(&v, format, ap);
        va_end(ap);

        nếu (r < 0)
                trả_về -ENOMEM;

        trả_về sysctl_write(property, v);
}

số_nguyên sysctl_write_verify(const char *property, const char *value) {
        số_nguyên r;

        assert(property);
        assert(value);

        /* Some sysctl settings accept invalid values on write, but refuses on read. E.g. coredump pattern,
         * be1e0283021ec73c2eb92839db9a471a068709d9 (v6.17) and 7d7c1fb85cba5627bbe741fb7539c709435e3848
         * (v6.16.8), which is fixed by a779e27f24aeb679969ddd1fdd7f636e22ddbc1e (v6.18) and
         * 304aa560385720baf3660fe8500f6dd425b63ea9 (v6.17.5). Let's first save the original value, and
         * restore to the saved value if the new value is refused. */

        _cleanup_free_ char *original = NULL;
        r = sysctl_read(property, &original);
        nếu (r >= 0 && streq(original, value))
                trả_về 0; /* Already set. */

        r = sysctl_write(property, value);
        nếu (r >= 0) {
                _cleanup_free_ char *current = NULL;
                r = sysctl_read(property, &current);
                nếu (r >= 0) {
                        nếu (streq(current, value))
                                trả_về 0; /* Yay! */

                        /* At least for coredump pattern, this does not happen, but let's handle this as the
                         * same as we wrote something invalid. */
                        r = -EINVAL;
                }
        }

        nếu (original)
                (void) sysctl_write(property, original);

        trả_về r;
}

static const char* af_to_sysctl_dir(số_nguyên af) {
        nếu (af == AF_MPLS)
                trả_về "mpls";

        trả_về af_to_ipv4_ipv6(af);
}

số_nguyên sysctl_write_ip_property(số_nguyên af, const char *ifname, const char *property, const char *value, Hashmap **shadow) {
        const char *p;

        assert(property);
        assert(value);

        nếu (!IN_SET(af, AF_INET, AF_INET6, AF_MPLS))
                trả_về -EAFNOSUPPORT;

        nếu (ifname) {
                nếu (!ifname_valid_full(ifname, IFNAME_VALID_SPECIAL))
                        trả_về -EINVAL;

                p = strjoina("net/", af_to_sysctl_dir(af), "/conf/", ifname, "/", property);
        } không_thì
                p = strjoina("net/", af_to_sysctl_dir(af), "/", property);

        trả_về sysctl_write_full(p, value, shadow);
}

số_nguyên sysctl_write_ip_property_boolean(số_nguyên af, const char *ifname, const char *property, bool value, Hashmap **shadow) {
        trả_về sysctl_write_ip_property(af, ifname, property, one_zero(value), shadow);
}

số_nguyên sysctl_write_ip_neighbor_property(số_nguyên af, const char *ifname, const char *property, const char *value, Hashmap **shadow) {
        const char *p;

        assert(property);
        assert(value);
        assert(ifname);

        nếu (!IN_SET(af, AF_INET, AF_INET6))
                trả_về -EAFNOSUPPORT;

        nếu (ifname) {
                nếu (!ifname_valid_full(ifname, IFNAME_VALID_SPECIAL))
                        trả_về -EINVAL;
                p = strjoina("net/", af_to_ipv4_ipv6(af), "/neigh/", ifname, "/", property);
        } không_thì
                p = strjoina("net/", af_to_ipv4_ipv6(af), "/neigh/default/", property);

        trả_về sysctl_write_full(p, value, shadow);
}

số_nguyên sysctl_write_ip_neighbor_property_uint32(số_nguyên af, const char *ifname, const char *property, uint32_t value, Hashmap **shadow) {
        char buf[DECIMAL_STR_MAX(uint32_t)];
        xsprintf(buf, "%u", value);
        trả_về sysctl_write_ip_neighbor_property(af, ifname, property, buf, shadow);
}

số_nguyên sysctl_read(const char *property, char **ret) {
        char *p;
        số_nguyên r;

        assert(property);

        p = strjoina("/proc/sys/", property);

        path_simplify(p);
        nếu (!path_is_normalized(p)) /* Filter out attempts to write to /proc/sys/../../…, just in case */
                trả_về -EINVAL;

        r = read_full_virtual_file(p, ret, NULL);
        nếu (r < 0)
                trả_về r;
        nếu (ret)
                delete_trailing_chars(*ret, NEWLINE);

        trả_về r;
}

số_nguyên sysctl_read_ip_property(số_nguyên af, const char *ifname, const char *property, char **ret) {
        const char *p;

        assert(property);

        nếu (!IN_SET(af, AF_INET, AF_INET6, AF_MPLS))
                trả_về -EAFNOSUPPORT;

        nếu (ifname) {
                nếu (!ifname_valid_full(ifname, IFNAME_VALID_SPECIAL))
                        trả_về -EINVAL;

                p = strjoina("net/", af_to_sysctl_dir(af), "/conf/", ifname, "/", property);
        } không_thì
                p = strjoina("net/", af_to_sysctl_dir(af), "/", property);

        trả_về sysctl_read(p, ret);
}

số_nguyên sysctl_read_ip_property_int(số_nguyên af, const char *ifname, const char *property, số_nguyên *ret) {
        _cleanup_free_ char *s = NULL;
        số_nguyên r;

        assert(ret);

        r = sysctl_read_ip_property(af, ifname, property, &s);
        nếu (r < 0)
                trả_về r;

        trả_về safe_atoi(s, ret);
}

số_nguyên sysctl_read_ip_property_uint32(số_nguyên af, const char *ifname, const char *property, uint32_t *ret) {
        _cleanup_free_ char *s = NULL;
        số_nguyên r;

        assert(ret);

        r = sysctl_read_ip_property(af, ifname, property, &s);
        nếu (r < 0)
                trả_về r;

        trả_về safe_atou32(s, ret);
}
