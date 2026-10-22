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

var_na* sysctl_normalize(var_na *s) {
        var_na *n;

        assert(s);

        n = strpbrk(s, "/.");

        /* If the first separator is a slash, the path is
         * assumed to be normalized and slashes remain slashes
         * and dots remains dots. */

        གལ་སྲིད (n && *n == '.')
                /* Dots become slashes and slashes become dots. Fun. */
                do {
                        གལ་སྲིད (*n == '.')
                                *n = '/';
                        དེ་མིན
                                *n = '.';

                        n = strpbrk(n + 1, "/.");
                } སྐབས (n);

        path_simplify(s);

        /* Kill the leading slash, but keep the first character of the string in the same place. */
        གལ་སྲིད (s[0] == '/' && s[1] != 0)
                memmove(s, s+1, strlen(s));

        ལོག s;
}

static ཧྲིལ shadow_update(Hashmap **shadow, const var_na *property, const var_na *value) {
        _cleanup_free_ var_na *k = NULL, *v = NULL, *cur_k = NULL, *cur_v = NULL;
        ཧྲིལ r;

        assert(property);
        assert(value);

        གལ་སྲིད (!shadow)
                ལོག 0;

        k = strdup(property);
        གལ་སྲིད (!k)
                ལོག -ENOMEM;

        v = strdup(value);
        གལ་སྲིད (!v)
                ལོག -ENOMEM;

        cur_v = hashmap_remove2(*shadow, k, (void**)&cur_k);

        r = hashmap_ensure_put(shadow, &path_hash_ops_free_free, k, v);
        གལ་སྲིད (r < 0) {
                assert(r != -EEXIST);

                ལོག r;
        }

        TAKE_PTR(k);
        TAKE_PTR(v);

        ལོག 0;
}

ཧྲིལ sysctl_write_full(const var_na *property, const var_na *value, Hashmap **shadow) {
        var_na *p;
        ཧྲིལ r;

        assert(property);
        assert(value);

        p = strjoina("/proc/sys/", property);

        path_simplify(p);
        གལ་སྲིད (!path_is_normalized(p))
                ལོག -EINVAL;

        log_debug("Setting '%s' to '%s'", p, value);

        r = shadow_update(shadow, p, value);
        གལ་སྲིད (r < 0)
                ལོག r;

        ལོག write_string_file(p, value, WRITE_STRING_FILE_VERIFY_ON_FAILURE | WRITE_STRING_FILE_DISABLE_BUFFER | WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL);
}

ཧྲིལ sysctl_writef(const var_na *property, const var_na *format, ...) {
        _cleanup_free_ var_na *v = NULL;
        va_list ap;
        ཧྲིལ r;

        va_start(ap, format);
        r = vasprintf(&v, format, ap);
        va_end(ap);

        གལ་སྲིད (r < 0)
                ལོག -ENOMEM;

        ལོག sysctl_write(property, v);
}

ཧྲིལ sysctl_write_verify(const var_na *property, const var_na *value) {
        ཧྲིལ r;

        assert(property);
        assert(value);

        /* Some sysctl settings accept invalid values on write, but refuses on read. E.g. coredump pattern,
         * be1e0283021ec73c2eb92839db9a471a068709d9 (v6.17) and 7d7c1fb85cba5627bbe741fb7539c709435e3848
         * (v6.16.8), which is fixed by a779e27f24aeb679969ddd1fdd7f636e22ddbc1e (v6.18) and
         * 304aa560385720baf3660fe8500f6dd425b63ea9 (v6.17.5). Let's first save the original value, and
         * restore to the saved value if the new value is refused. */

        _cleanup_free_ var_na *original = NULL;
        r = sysctl_read(property, &original);
        གལ་སྲིད (r >= 0 && streq(original, value))
                ལོག 0; /* Already set. */

        r = sysctl_write(property, value);
        གལ་སྲིད (r >= 0) {
                _cleanup_free_ var_na *current = NULL;
                r = sysctl_read(property, &current);
                གལ་སྲིད (r >= 0) {
                        གལ་སྲིད (streq(current, value))
                                ལོག 0; /* Yay! */

                        /* At least for coredump pattern, this does not happen, but let's handle this as the
                         * same as we wrote something invalid. */
                        r = -EINVAL;
                }
        }

        གལ་སྲིད (original)
                (void) sysctl_write(property, original);

        ལོག r;
}

static const var_na* af_to_sysctl_dir(ཧྲིལ af) {
        གལ་སྲིད (af == AF_MPLS)
                ལོག "mpls";

        ལོག af_to_ipv4_ipv6(af);
}

ཧྲིལ sysctl_write_ip_property(ཧྲིལ af, const var_na *ifname, const var_na *property, const var_na *value, Hashmap **shadow) {
        const var_na *p;

        assert(property);
        assert(value);

        གལ་སྲིད (!IN_SET(af, AF_INET, AF_INET6, AF_MPLS))
                ལོག -EAFNOSUPPORT;

        གལ་སྲིད (ifname) {
                གལ་སྲིད (!ifname_valid_full(ifname, IFNAME_VALID_SPECIAL))
                        ལོག -EINVAL;

                p = strjoina("net/", af_to_sysctl_dir(af), "/conf/", ifname, "/", property);
        } དེ་མིན
                p = strjoina("net/", af_to_sysctl_dir(af), "/", property);

        ལོག sysctl_write_full(p, value, shadow);
}

ཧྲིལ sysctl_write_ip_property_boolean(ཧྲིལ af, const var_na *ifname, const var_na *property, bool value, Hashmap **shadow) {
        ལོག sysctl_write_ip_property(af, ifname, property, one_zero(value), shadow);
}

ཧྲིལ sysctl_write_ip_neighbor_property(ཧྲིལ af, const var_na *ifname, const var_na *property, const var_na *value, Hashmap **shadow) {
        const var_na *p;

        assert(property);
        assert(value);
        assert(ifname);

        གལ་སྲིད (!IN_SET(af, AF_INET, AF_INET6))
                ལོག -EAFNOSUPPORT;

        གལ་སྲིད (ifname) {
                གལ་སྲིད (!ifname_valid_full(ifname, IFNAME_VALID_SPECIAL))
                        ལོག -EINVAL;
                p = strjoina("net/", af_to_ipv4_ipv6(af), "/neigh/", ifname, "/", property);
        } དེ་མིན
                p = strjoina("net/", af_to_ipv4_ipv6(af), "/neigh/default/", property);

        ལོག sysctl_write_full(p, value, shadow);
}

ཧྲིལ sysctl_write_ip_neighbor_property_uint32(ཧྲིལ af, const var_na *ifname, const var_na *property, uint32_t value, Hashmap **shadow) {
        var_na buf[DECIMAL_STR_MAX(uint32_t)];
        xsprintf(buf, "%u", value);
        ལོག sysctl_write_ip_neighbor_property(af, ifname, property, buf, shadow);
}

ཧྲིལ sysctl_read(const var_na *property, var_na **ret) {
        var_na *p;
        ཧྲིལ r;

        assert(property);

        p = strjoina("/proc/sys/", property);

        path_simplify(p);
        གལ་སྲིད (!path_is_normalized(p)) /* Filter out attempts to write to /proc/sys/../../…, just in case */
                ལོག -EINVAL;

        r = read_full_virtual_file(p, ret, NULL);
        གལ་སྲིད (r < 0)
                ལོག r;
        གལ་སྲིད (ret)
                delete_trailing_chars(*ret, NEWLINE);

        ལོག r;
}

ཧྲིལ sysctl_read_ip_property(ཧྲིལ af, const var_na *ifname, const var_na *property, var_na **ret) {
        const var_na *p;

        assert(property);

        གལ་སྲིད (!IN_SET(af, AF_INET, AF_INET6, AF_MPLS))
                ལོག -EAFNOSUPPORT;

        གལ་སྲིད (ifname) {
                གལ་སྲིད (!ifname_valid_full(ifname, IFNAME_VALID_SPECIAL))
                        ལོག -EINVAL;

                p = strjoina("net/", af_to_sysctl_dir(af), "/conf/", ifname, "/", property);
        } དེ་མིན
                p = strjoina("net/", af_to_sysctl_dir(af), "/", property);

        ལོག sysctl_read(p, ret);
}

ཧྲིལ sysctl_read_ip_property_int(ཧྲིལ af, const var_na *ifname, const var_na *property, ཧྲིལ *ret) {
        _cleanup_free_ var_na *s = NULL;
        ཧྲིལ r;

        assert(ret);

        r = sysctl_read_ip_property(af, ifname, property, &s);
        གལ་སྲིད (r < 0)
                ལོག r;

        ལོག safe_atoi(s, ret);
}

ཧྲིལ sysctl_read_ip_property_uint32(ཧྲིལ af, const var_na *ifname, const var_na *property, uint32_t *ret) {
        _cleanup_free_ var_na *s = NULL;
        ཧྲིལ r;

        assert(ret);

        r = sysctl_read_ip_property(af, ifname, property, &s);
        གལ་སྲིད (r < 0)
                ལོག r;

        ལོག safe_atou32(s, ret);
}
