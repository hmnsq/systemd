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

        якщо (n && *n == '.')
                /* Dots become slashes and slashes become dots. Fun. */
                do {
                        якщо (*n == '.')
                                *n = '/';
                        інакше
                                *n = '.';

                        n = strpbrk(n + 1, "/.");
                } поки (n);

        path_simplify(s);

        /* Kill the leading slash, but keep the first character of the string in the same place. */
        якщо (s[0] == '/' && s[1] != 0)
                memmove(s, s+1, strlen(s));

        повернути s;
}

static ціле shadow_update(Hashmap **shadow, const char *property, const char *value) {
        _cleanup_free_ char *k = NULL, *v = NULL, *cur_k = NULL, *cur_v = NULL;
        ціле r;

        assert(property);
        assert(value);

        якщо (!shadow)
                повернути 0;

        k = strdup(property);
        якщо (!k)
                повернути -ENOMEM;

        v = strdup(value);
        якщо (!v)
                повернути -ENOMEM;

        cur_v = hashmap_remove2(*shadow, k, (void**)&cur_k);

        r = hashmap_ensure_put(shadow, &path_hash_ops_free_free, k, v);
        якщо (r < 0) {
                assert(r != -EEXIST);

                повернути r;
        }

        TAKE_PTR(k);
        TAKE_PTR(v);

        повернути 0;
}

ціле sysctl_write_full(const char *property, const char *value, Hashmap **shadow) {
        char *p;
        ціле r;

        assert(property);
        assert(value);

        p = strjoina("/proc/sys/", property);

        path_simplify(p);
        якщо (!path_is_normalized(p))
                повернути -EINVAL;

        log_debug("Setting '%s' to '%s'", p, value);

        r = shadow_update(shadow, p, value);
        якщо (r < 0)
                повернути r;

        повернути write_string_file(p, value, WRITE_STRING_FILE_VERIFY_ON_FAILURE | WRITE_STRING_FILE_DISABLE_BUFFER | WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL);
}

ціле sysctl_writef(const char *property, const char *format, ...) {
        _cleanup_free_ char *v = NULL;
        va_list ap;
        ціле r;

        va_start(ap, format);
        r = vasprintf(&v, format, ap);
        va_end(ap);

        якщо (r < 0)
                повернути -ENOMEM;

        повернути sysctl_write(property, v);
}

ціле sysctl_write_verify(const char *property, const char *value) {
        ціле r;

        assert(property);
        assert(value);

        /* Some sysctl settings accept invalid values on write, but refuses on read. E.g. coredump pattern,
         * be1e0283021ec73c2eb92839db9a471a068709d9 (v6.17) and 7d7c1fb85cba5627bbe741fb7539c709435e3848
         * (v6.16.8), which is fixed by a779e27f24aeb679969ddd1fdd7f636e22ddbc1e (v6.18) and
         * 304aa560385720baf3660fe8500f6dd425b63ea9 (v6.17.5). Let's first save the original value, and
         * restore to the saved value if the new value is refused. */

        _cleanup_free_ char *original = NULL;
        r = sysctl_read(property, &original);
        якщо (r >= 0 && streq(original, value))
                повернути 0; /* Already set. */

        r = sysctl_write(property, value);
        якщо (r >= 0) {
                _cleanup_free_ char *current = NULL;
                r = sysctl_read(property, &current);
                якщо (r >= 0) {
                        якщо (streq(current, value))
                                повернути 0; /* Yay! */

                        /* At least for coredump pattern, this does not happen, but let's handle this as the
                         * same as we wrote something invalid. */
                        r = -EINVAL;
                }
        }

        якщо (original)
                (void) sysctl_write(property, original);

        повернути r;
}

static const char* af_to_sysctl_dir(ціле af) {
        якщо (af == AF_MPLS)
                повернути "mpls";

        повернути af_to_ipv4_ipv6(af);
}

ціле sysctl_write_ip_property(ціле af, const char *ifname, const char *property, const char *value, Hashmap **shadow) {
        const char *p;

        assert(property);
        assert(value);

        якщо (!IN_SET(af, AF_INET, AF_INET6, AF_MPLS))
                повернути -EAFNOSUPPORT;

        якщо (ifname) {
                якщо (!ifname_valid_full(ifname, IFNAME_VALID_SPECIAL))
                        повернути -EINVAL;

                p = strjoina("net/", af_to_sysctl_dir(af), "/conf/", ifname, "/", property);
        } інакше
                p = strjoina("net/", af_to_sysctl_dir(af), "/", property);

        повернути sysctl_write_full(p, value, shadow);
}

ціле sysctl_write_ip_property_boolean(ціле af, const char *ifname, const char *property, bool value, Hashmap **shadow) {
        повернути sysctl_write_ip_property(af, ifname, property, one_zero(value), shadow);
}

ціле sysctl_write_ip_neighbor_property(ціле af, const char *ifname, const char *property, const char *value, Hashmap **shadow) {
        const char *p;

        assert(property);
        assert(value);
        assert(ifname);

        якщо (!IN_SET(af, AF_INET, AF_INET6))
                повернути -EAFNOSUPPORT;

        якщо (ifname) {
                якщо (!ifname_valid_full(ifname, IFNAME_VALID_SPECIAL))
                        повернути -EINVAL;
                p = strjoina("net/", af_to_ipv4_ipv6(af), "/neigh/", ifname, "/", property);
        } інакше
                p = strjoina("net/", af_to_ipv4_ipv6(af), "/neigh/default/", property);

        повернути sysctl_write_full(p, value, shadow);
}

ціле sysctl_write_ip_neighbor_property_uint32(ціле af, const char *ifname, const char *property, uint32_t value, Hashmap **shadow) {
        char buf[DECIMAL_STR_MAX(uint32_t)];
        xsprintf(buf, "%u", value);
        повернути sysctl_write_ip_neighbor_property(af, ifname, property, buf, shadow);
}

ціле sysctl_read(const char *property, char **ret) {
        char *p;
        ціле r;

        assert(property);

        p = strjoina("/proc/sys/", property);

        path_simplify(p);
        якщо (!path_is_normalized(p)) /* Filter out attempts to write to /proc/sys/../../…, just in case */
                повернути -EINVAL;

        r = read_full_virtual_file(p, ret, NULL);
        якщо (r < 0)
                повернути r;
        якщо (ret)
                delete_trailing_chars(*ret, NEWLINE);

        повернути r;
}

ціле sysctl_read_ip_property(ціле af, const char *ifname, const char *property, char **ret) {
        const char *p;

        assert(property);

        якщо (!IN_SET(af, AF_INET, AF_INET6, AF_MPLS))
                повернути -EAFNOSUPPORT;

        якщо (ifname) {
                якщо (!ifname_valid_full(ifname, IFNAME_VALID_SPECIAL))
                        повернути -EINVAL;

                p = strjoina("net/", af_to_sysctl_dir(af), "/conf/", ifname, "/", property);
        } інакше
                p = strjoina("net/", af_to_sysctl_dir(af), "/", property);

        повернути sysctl_read(p, ret);
}

ціле sysctl_read_ip_property_int(ціле af, const char *ifname, const char *property, ціле *ret) {
        _cleanup_free_ char *s = NULL;
        ціле r;

        assert(ret);

        r = sysctl_read_ip_property(af, ifname, property, &s);
        якщо (r < 0)
                повернути r;

        повернути safe_atoi(s, ret);
}

ціле sysctl_read_ip_property_uint32(ціле af, const char *ifname, const char *property, uint32_t *ret) {
        _cleanup_free_ char *s = NULL;
        ціле r;

        assert(ret);

        r = sysctl_read_ip_property(af, ifname, property, &s);
        якщо (r < 0)
                повернути r;

        повернути safe_atou32(s, ret);
}
