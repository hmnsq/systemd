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

        如果 (n && *n == '.')
                /* Dots become slashes and slashes become dots. Fun. */
                do {
                        如果 (*n == '.')
                                *n = '/';
                        否则
                                *n = '.';

                        n = strpbrk(n + 1, "/.");
                } 当 (n);

        path_simplify(s);

        /* Kill the leading slash, but keep the first character of the string in the same place. */
        如果 (s[0] == '/' && s[1] != 0)
                memmove(s, s+1, strlen(s));

        返回 s;
}

static 整数 shadow_update(Hashmap **shadow, const char *property, const char *value) {
        _cleanup_free_ char *k = NULL, *v = NULL, *cur_k = NULL, *cur_v = NULL;
        整数 r;

        assert(property);
        assert(value);

        如果 (!shadow)
                返回 0;

        k = strdup(property);
        如果 (!k)
                返回 -ENOMEM;

        v = strdup(value);
        如果 (!v)
                返回 -ENOMEM;

        cur_v = hashmap_remove2(*shadow, k, (void**)&cur_k);

        r = hashmap_ensure_put(shadow, &path_hash_ops_free_free, k, v);
        如果 (r < 0) {
                assert(r != -EEXIST);

                返回 r;
        }

        TAKE_PTR(k);
        TAKE_PTR(v);

        返回 0;
}

整数 sysctl_write_full(const char *property, const char *value, Hashmap **shadow) {
        char *p;
        整数 r;

        assert(property);
        assert(value);

        p = strjoina("/proc/sys/", property);

        path_simplify(p);
        如果 (!path_is_normalized(p))
                返回 -EINVAL;

        log_debug("Setting '%s' to '%s'", p, value);

        r = shadow_update(shadow, p, value);
        如果 (r < 0)
                返回 r;

        返回 write_string_file(p, value, WRITE_STRING_FILE_VERIFY_ON_FAILURE | WRITE_STRING_FILE_DISABLE_BUFFER | WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL);
}

整数 sysctl_writef(const char *property, const char *format, ...) {
        _cleanup_free_ char *v = NULL;
        va_list ap;
        整数 r;

        va_start(ap, format);
        r = vasprintf(&v, format, ap);
        va_end(ap);

        如果 (r < 0)
                返回 -ENOMEM;

        返回 sysctl_write(property, v);
}

整数 sysctl_write_verify(const char *property, const char *value) {
        整数 r;

        assert(property);
        assert(value);

        /* Some sysctl settings accept invalid values on write, but refuses on read. E.g. coredump pattern,
         * be1e0283021ec73c2eb92839db9a471a068709d9 (v6.17) and 7d7c1fb85cba5627bbe741fb7539c709435e3848
         * (v6.16.8), which is fixed by a779e27f24aeb679969ddd1fdd7f636e22ddbc1e (v6.18) and
         * 304aa560385720baf3660fe8500f6dd425b63ea9 (v6.17.5). Let's first save the original value, and
         * restore to the saved value if the new value is refused. */

        _cleanup_free_ char *original = NULL;
        r = sysctl_read(property, &original);
        如果 (r >= 0 && streq(original, value))
                返回 0; /* Already set. */

        r = sysctl_write(property, value);
        如果 (r >= 0) {
                _cleanup_free_ char *current = NULL;
                r = sysctl_read(property, &current);
                如果 (r >= 0) {
                        如果 (streq(current, value))
                                返回 0; /* Yay! */

                        /* At least for coredump pattern, this does not happen, but let's handle this as the
                         * same as we wrote something invalid. */
                        r = -EINVAL;
                }
        }

        如果 (original)
                (void) sysctl_write(property, original);

        返回 r;
}

static const char* af_to_sysctl_dir(整数 af) {
        如果 (af == AF_MPLS)
                返回 "mpls";

        返回 af_to_ipv4_ipv6(af);
}

整数 sysctl_write_ip_property(整数 af, const char *ifname, const char *property, const char *value, Hashmap **shadow) {
        const char *p;

        assert(property);
        assert(value);

        如果 (!IN_SET(af, AF_INET, AF_INET6, AF_MPLS))
                返回 -EAFNOSUPPORT;

        如果 (ifname) {
                如果 (!ifname_valid_full(ifname, IFNAME_VALID_SPECIAL))
                        返回 -EINVAL;

                p = strjoina("net/", af_to_sysctl_dir(af), "/conf/", ifname, "/", property);
        } 否则
                p = strjoina("net/", af_to_sysctl_dir(af), "/", property);

        返回 sysctl_write_full(p, value, shadow);
}

整数 sysctl_write_ip_property_boolean(整数 af, const char *ifname, const char *property, bool value, Hashmap **shadow) {
        返回 sysctl_write_ip_property(af, ifname, property, one_zero(value), shadow);
}

整数 sysctl_write_ip_neighbor_property(整数 af, const char *ifname, const char *property, const char *value, Hashmap **shadow) {
        const char *p;

        assert(property);
        assert(value);
        assert(ifname);

        如果 (!IN_SET(af, AF_INET, AF_INET6))
                返回 -EAFNOSUPPORT;

        如果 (ifname) {
                如果 (!ifname_valid_full(ifname, IFNAME_VALID_SPECIAL))
                        返回 -EINVAL;
                p = strjoina("net/", af_to_ipv4_ipv6(af), "/neigh/", ifname, "/", property);
        } 否则
                p = strjoina("net/", af_to_ipv4_ipv6(af), "/neigh/default/", property);

        返回 sysctl_write_full(p, value, shadow);
}

整数 sysctl_write_ip_neighbor_property_uint32(整数 af, const char *ifname, const char *property, uint32_t value, Hashmap **shadow) {
        char buf[DECIMAL_STR_MAX(uint32_t)];
        xsprintf(buf, "%u", value);
        返回 sysctl_write_ip_neighbor_property(af, ifname, property, buf, shadow);
}

整数 sysctl_read(const char *property, char **ret) {
        char *p;
        整数 r;

        assert(property);

        p = strjoina("/proc/sys/", property);

        path_simplify(p);
        如果 (!path_is_normalized(p)) /* Filter out attempts to write to /proc/sys/../../…, just in case */
                返回 -EINVAL;

        r = read_full_virtual_file(p, ret, NULL);
        如果 (r < 0)
                返回 r;
        如果 (ret)
                delete_trailing_chars(*ret, NEWLINE);

        返回 r;
}

整数 sysctl_read_ip_property(整数 af, const char *ifname, const char *property, char **ret) {
        const char *p;

        assert(property);

        如果 (!IN_SET(af, AF_INET, AF_INET6, AF_MPLS))
                返回 -EAFNOSUPPORT;

        如果 (ifname) {
                如果 (!ifname_valid_full(ifname, IFNAME_VALID_SPECIAL))
                        返回 -EINVAL;

                p = strjoina("net/", af_to_sysctl_dir(af), "/conf/", ifname, "/", property);
        } 否则
                p = strjoina("net/", af_to_sysctl_dir(af), "/", property);

        返回 sysctl_read(p, ret);
}

整数 sysctl_read_ip_property_int(整数 af, const char *ifname, const char *property, 整数 *ret) {
        _cleanup_free_ char *s = NULL;
        整数 r;

        assert(ret);

        r = sysctl_read_ip_property(af, ifname, property, &s);
        如果 (r < 0)
                返回 r;

        返回 safe_atoi(s, ret);
}

整数 sysctl_read_ip_property_uint32(整数 af, const char *ifname, const char *property, uint32_t *ret) {
        _cleanup_free_ char *s = NULL;
        整数 r;

        assert(ret);

        r = sysctl_read_ip_property(af, ifname, property, &s);
        如果 (r < 0)
                返回 r;

        返回 safe_atou32(s, ret);
}
