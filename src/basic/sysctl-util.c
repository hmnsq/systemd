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

        만약 (n && *n == '.')
                /* Dots become slashes and slashes become dots. Fun. */
                do {
                        만약 (*n == '.')
                                *n = '/';
                        아니면
                                *n = '.';

                        n = strpbrk(n + 1, "/.");
                } 동안 (n);

        path_simplify(s);

        /* Kill the leading slash, but keep the first character of the string in the same place. */
        만약 (s[0] == '/' && s[1] != 0)
                memmove(s, s+1, strlen(s));

        반환 s;
}

static 정수 shadow_update(Hashmap **shadow, const char *property, const char *value) {
        _cleanup_free_ char *k = NULL, *v = NULL, *cur_k = NULL, *cur_v = NULL;
        정수 r;

        assert(property);
        assert(value);

        만약 (!shadow)
                반환 0;

        k = strdup(property);
        만약 (!k)
                반환 -ENOMEM;

        v = strdup(value);
        만약 (!v)
                반환 -ENOMEM;

        cur_v = hashmap_remove2(*shadow, k, (void**)&cur_k);

        r = hashmap_ensure_put(shadow, &path_hash_ops_free_free, k, v);
        만약 (r < 0) {
                assert(r != -EEXIST);

                반환 r;
        }

        TAKE_PTR(k);
        TAKE_PTR(v);

        반환 0;
}

정수 sysctl_write_full(const char *property, const char *value, Hashmap **shadow) {
        char *p;
        정수 r;

        assert(property);
        assert(value);

        p = strjoina("/proc/sys/", property);

        path_simplify(p);
        만약 (!path_is_normalized(p))
                반환 -EINVAL;

        log_debug("Setting '%s' to '%s'", p, value);

        r = shadow_update(shadow, p, value);
        만약 (r < 0)
                반환 r;

        반환 write_string_file(p, value, WRITE_STRING_FILE_VERIFY_ON_FAILURE | WRITE_STRING_FILE_DISABLE_BUFFER | WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL);
}

정수 sysctl_writef(const char *property, const char *format, ...) {
        _cleanup_free_ char *v = NULL;
        va_list ap;
        정수 r;

        va_start(ap, format);
        r = vasprintf(&v, format, ap);
        va_end(ap);

        만약 (r < 0)
                반환 -ENOMEM;

        반환 sysctl_write(property, v);
}

정수 sysctl_write_verify(const char *property, const char *value) {
        정수 r;

        assert(property);
        assert(value);

        /* Some sysctl settings accept invalid values on write, but refuses on read. E.g. coredump pattern,
         * be1e0283021ec73c2eb92839db9a471a068709d9 (v6.17) and 7d7c1fb85cba5627bbe741fb7539c709435e3848
         * (v6.16.8), which is fixed by a779e27f24aeb679969ddd1fdd7f636e22ddbc1e (v6.18) and
         * 304aa560385720baf3660fe8500f6dd425b63ea9 (v6.17.5). Let's first save the original value, and
         * restore to the saved value if the new value is refused. */

        _cleanup_free_ char *original = NULL;
        r = sysctl_read(property, &original);
        만약 (r >= 0 && streq(original, value))
                반환 0; /* Already set. */

        r = sysctl_write(property, value);
        만약 (r >= 0) {
                _cleanup_free_ char *current = NULL;
                r = sysctl_read(property, &current);
                만약 (r >= 0) {
                        만약 (streq(current, value))
                                반환 0; /* Yay! */

                        /* At least for coredump pattern, this does not happen, but let's handle this as the
                         * same as we wrote something invalid. */
                        r = -EINVAL;
                }
        }

        만약 (original)
                (void) sysctl_write(property, original);

        반환 r;
}

static const char* af_to_sysctl_dir(정수 af) {
        만약 (af == AF_MPLS)
                반환 "mpls";

        반환 af_to_ipv4_ipv6(af);
}

정수 sysctl_write_ip_property(정수 af, const char *ifname, const char *property, const char *value, Hashmap **shadow) {
        const char *p;

        assert(property);
        assert(value);

        만약 (!IN_SET(af, AF_INET, AF_INET6, AF_MPLS))
                반환 -EAFNOSUPPORT;

        만약 (ifname) {
                만약 (!ifname_valid_full(ifname, IFNAME_VALID_SPECIAL))
                        반환 -EINVAL;

                p = strjoina("net/", af_to_sysctl_dir(af), "/conf/", ifname, "/", property);
        } 아니면
                p = strjoina("net/", af_to_sysctl_dir(af), "/", property);

        반환 sysctl_write_full(p, value, shadow);
}

정수 sysctl_write_ip_property_boolean(정수 af, const char *ifname, const char *property, bool value, Hashmap **shadow) {
        반환 sysctl_write_ip_property(af, ifname, property, one_zero(value), shadow);
}

정수 sysctl_write_ip_neighbor_property(정수 af, const char *ifname, const char *property, const char *value, Hashmap **shadow) {
        const char *p;

        assert(property);
        assert(value);
        assert(ifname);

        만약 (!IN_SET(af, AF_INET, AF_INET6))
                반환 -EAFNOSUPPORT;

        만약 (ifname) {
                만약 (!ifname_valid_full(ifname, IFNAME_VALID_SPECIAL))
                        반환 -EINVAL;
                p = strjoina("net/", af_to_ipv4_ipv6(af), "/neigh/", ifname, "/", property);
        } 아니면
                p = strjoina("net/", af_to_ipv4_ipv6(af), "/neigh/default/", property);

        반환 sysctl_write_full(p, value, shadow);
}

정수 sysctl_write_ip_neighbor_property_uint32(정수 af, const char *ifname, const char *property, uint32_t value, Hashmap **shadow) {
        char buf[DECIMAL_STR_MAX(uint32_t)];
        xsprintf(buf, "%u", value);
        반환 sysctl_write_ip_neighbor_property(af, ifname, property, buf, shadow);
}

정수 sysctl_read(const char *property, char **ret) {
        char *p;
        정수 r;

        assert(property);

        p = strjoina("/proc/sys/", property);

        path_simplify(p);
        만약 (!path_is_normalized(p)) /* Filter out attempts to write to /proc/sys/../../…, just in case */
                반환 -EINVAL;

        r = read_full_virtual_file(p, ret, NULL);
        만약 (r < 0)
                반환 r;
        만약 (ret)
                delete_trailing_chars(*ret, NEWLINE);

        반환 r;
}

정수 sysctl_read_ip_property(정수 af, const char *ifname, const char *property, char **ret) {
        const char *p;

        assert(property);

        만약 (!IN_SET(af, AF_INET, AF_INET6, AF_MPLS))
                반환 -EAFNOSUPPORT;

        만약 (ifname) {
                만약 (!ifname_valid_full(ifname, IFNAME_VALID_SPECIAL))
                        반환 -EINVAL;

                p = strjoina("net/", af_to_sysctl_dir(af), "/conf/", ifname, "/", property);
        } 아니면
                p = strjoina("net/", af_to_sysctl_dir(af), "/", property);

        반환 sysctl_read(p, ret);
}

정수 sysctl_read_ip_property_int(정수 af, const char *ifname, const char *property, 정수 *ret) {
        _cleanup_free_ char *s = NULL;
        정수 r;

        assert(ret);

        r = sysctl_read_ip_property(af, ifname, property, &s);
        만약 (r < 0)
                반환 r;

        반환 safe_atoi(s, ret);
}

정수 sysctl_read_ip_property_uint32(정수 af, const char *ifname, const char *property, uint32_t *ret) {
        _cleanup_free_ char *s = NULL;
        정수 r;

        assert(ret);

        r = sysctl_read_ip_property(af, ifname, property, &s);
        만약 (r < 0)
                반환 r;

        반환 safe_atou32(s, ret);
}
