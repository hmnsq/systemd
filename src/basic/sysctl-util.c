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

        ប្រសិនបើ (n && *n == '.')
                /* Dots become slashes and slashes become dots. Fun. */
                do {
                        ប្រសិនបើ (*n == '.')
                                *n = '/';
                        បើមិនដូច្នេះ
                                *n = '.';

                        n = strpbrk(n + 1, "/.");
                } ខណៈពេល (n);

        path_simplify(s);

        /* Kill the leading slash, but keep the first character of the string in the same place. */
        ប្រសិនបើ (s[0] == '/' && s[1] != 0)
                memmove(s, s+1, strlen(s));

        ត្រឡប់ s;
}

static ចំនួនគត់ shadow_update(Hashmap **shadow, const var_na *property, const var_na *value) {
        _cleanup_free_ var_na *k = NULL, *v = NULL, *cur_k = NULL, *cur_v = NULL;
        ចំនួនគត់ r;

        assert(property);
        assert(value);

        ប្រសិនបើ (!shadow)
                ត្រឡប់ 0;

        k = strdup(property);
        ប្រសិនបើ (!k)
                ត្រឡប់ -ENOMEM;

        v = strdup(value);
        ប្រសិនបើ (!v)
                ត្រឡប់ -ENOMEM;

        cur_v = hashmap_remove2(*shadow, k, (void**)&cur_k);

        r = hashmap_ensure_put(shadow, &path_hash_ops_free_free, k, v);
        ប្រសិនបើ (r < 0) {
                assert(r != -EEXIST);

                ត្រឡប់ r;
        }

        TAKE_PTR(k);
        TAKE_PTR(v);

        ត្រឡប់ 0;
}

ចំនួនគត់ sysctl_write_full(const var_na *property, const var_na *value, Hashmap **shadow) {
        var_na *p;
        ចំនួនគត់ r;

        assert(property);
        assert(value);

        p = strjoina("/proc/sys/", property);

        path_simplify(p);
        ប្រសិនបើ (!path_is_normalized(p))
                ត្រឡប់ -EINVAL;

        log_debug("Setting '%s' to '%s'", p, value);

        r = shadow_update(shadow, p, value);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        ត្រឡប់ write_string_file(p, value, WRITE_STRING_FILE_VERIFY_ON_FAILURE | WRITE_STRING_FILE_DISABLE_BUFFER | WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL);
}

ចំនួនគត់ sysctl_writef(const var_na *property, const var_na *format, ...) {
        _cleanup_free_ var_na *v = NULL;
        va_list ap;
        ចំនួនគត់ r;

        va_start(ap, format);
        r = vasprintf(&v, format, ap);
        va_end(ap);

        ប្រសិនបើ (r < 0)
                ត្រឡប់ -ENOMEM;

        ត្រឡប់ sysctl_write(property, v);
}

ចំនួនគត់ sysctl_write_verify(const var_na *property, const var_na *value) {
        ចំនួនគត់ r;

        assert(property);
        assert(value);

        /* Some sysctl settings accept invalid values on write, but refuses on read. E.g. coredump pattern,
         * be1e0283021ec73c2eb92839db9a471a068709d9 (v6.17) and 7d7c1fb85cba5627bbe741fb7539c709435e3848
         * (v6.16.8), which is fixed by a779e27f24aeb679969ddd1fdd7f636e22ddbc1e (v6.18) and
         * 304aa560385720baf3660fe8500f6dd425b63ea9 (v6.17.5). Let's first save the original value, and
         * restore to the saved value if the new value is refused. */

        _cleanup_free_ var_na *original = NULL;
        r = sysctl_read(property, &original);
        ប្រសិនបើ (r >= 0 && streq(original, value))
                ត្រឡប់ 0; /* Already set. */

        r = sysctl_write(property, value);
        ប្រសិនបើ (r >= 0) {
                _cleanup_free_ var_na *current = NULL;
                r = sysctl_read(property, &current);
                ប្រសិនបើ (r >= 0) {
                        ប្រសិនបើ (streq(current, value))
                                ត្រឡប់ 0; /* Yay! */

                        /* At least for coredump pattern, this does not happen, but let's handle this as the
                         * same as we wrote something invalid. */
                        r = -EINVAL;
                }
        }

        ប្រសិនបើ (original)
                (void) sysctl_write(property, original);

        ត្រឡប់ r;
}

static const var_na* af_to_sysctl_dir(ចំនួនគត់ af) {
        ប្រសិនបើ (af == AF_MPLS)
                ត្រឡប់ "mpls";

        ត្រឡប់ af_to_ipv4_ipv6(af);
}

ចំនួនគត់ sysctl_write_ip_property(ចំនួនគត់ af, const var_na *ifname, const var_na *property, const var_na *value, Hashmap **shadow) {
        const var_na *p;

        assert(property);
        assert(value);

        ប្រសិនបើ (!IN_SET(af, AF_INET, AF_INET6, AF_MPLS))
                ត្រឡប់ -EAFNOSUPPORT;

        ប្រសិនបើ (ifname) {
                ប្រសិនបើ (!ifname_valid_full(ifname, IFNAME_VALID_SPECIAL))
                        ត្រឡប់ -EINVAL;

                p = strjoina("net/", af_to_sysctl_dir(af), "/conf/", ifname, "/", property);
        } បើមិនដូច្នេះ
                p = strjoina("net/", af_to_sysctl_dir(af), "/", property);

        ត្រឡប់ sysctl_write_full(p, value, shadow);
}

ចំនួនគត់ sysctl_write_ip_property_boolean(ចំនួនគត់ af, const var_na *ifname, const var_na *property, bool value, Hashmap **shadow) {
        ត្រឡប់ sysctl_write_ip_property(af, ifname, property, one_zero(value), shadow);
}

ចំនួនគត់ sysctl_write_ip_neighbor_property(ចំនួនគត់ af, const var_na *ifname, const var_na *property, const var_na *value, Hashmap **shadow) {
        const var_na *p;

        assert(property);
        assert(value);
        assert(ifname);

        ប្រសិនបើ (!IN_SET(af, AF_INET, AF_INET6))
                ត្រឡប់ -EAFNOSUPPORT;

        ប្រសិនបើ (ifname) {
                ប្រសិនបើ (!ifname_valid_full(ifname, IFNAME_VALID_SPECIAL))
                        ត្រឡប់ -EINVAL;
                p = strjoina("net/", af_to_ipv4_ipv6(af), "/neigh/", ifname, "/", property);
        } បើមិនដូច្នេះ
                p = strjoina("net/", af_to_ipv4_ipv6(af), "/neigh/default/", property);

        ត្រឡប់ sysctl_write_full(p, value, shadow);
}

ចំនួនគត់ sysctl_write_ip_neighbor_property_uint32(ចំនួនគត់ af, const var_na *ifname, const var_na *property, uint32_t value, Hashmap **shadow) {
        var_na buf[DECIMAL_STR_MAX(uint32_t)];
        xsprintf(buf, "%u", value);
        ត្រឡប់ sysctl_write_ip_neighbor_property(af, ifname, property, buf, shadow);
}

ចំនួនគត់ sysctl_read(const var_na *property, var_na **ret) {
        var_na *p;
        ចំនួនគត់ r;

        assert(property);

        p = strjoina("/proc/sys/", property);

        path_simplify(p);
        ប្រសិនបើ (!path_is_normalized(p)) /* Filter out attempts to write to /proc/sys/../../…, just in case */
                ត្រឡប់ -EINVAL;

        r = read_full_virtual_file(p, ret, NULL);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;
        ប្រសិនបើ (ret)
                delete_trailing_chars(*ret, NEWLINE);

        ត្រឡប់ r;
}

ចំនួនគត់ sysctl_read_ip_property(ចំនួនគត់ af, const var_na *ifname, const var_na *property, var_na **ret) {
        const var_na *p;

        assert(property);

        ប្រសិនបើ (!IN_SET(af, AF_INET, AF_INET6, AF_MPLS))
                ត្រឡប់ -EAFNOSUPPORT;

        ប្រសិនបើ (ifname) {
                ប្រសិនបើ (!ifname_valid_full(ifname, IFNAME_VALID_SPECIAL))
                        ត្រឡប់ -EINVAL;

                p = strjoina("net/", af_to_sysctl_dir(af), "/conf/", ifname, "/", property);
        } បើមិនដូច្នេះ
                p = strjoina("net/", af_to_sysctl_dir(af), "/", property);

        ត្រឡប់ sysctl_read(p, ret);
}

ចំនួនគត់ sysctl_read_ip_property_int(ចំនួនគត់ af, const var_na *ifname, const var_na *property, ចំនួនគត់ *ret) {
        _cleanup_free_ var_na *s = NULL;
        ចំនួនគត់ r;

        assert(ret);

        r = sysctl_read_ip_property(af, ifname, property, &s);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        ត្រឡប់ safe_atoi(s, ret);
}

ចំនួនគត់ sysctl_read_ip_property_uint32(ចំនួនគត់ af, const var_na *ifname, const var_na *property, uint32_t *ret) {
        _cleanup_free_ var_na *s = NULL;
        ចំនួនគត់ r;

        assert(ret);

        r = sysctl_read_ip_property(af, ifname, property, &s);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        ត្រឡប់ safe_atou32(s, ret);
}
