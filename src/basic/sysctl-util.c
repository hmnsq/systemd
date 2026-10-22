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

        အကယ်၍ (n && *n == '.')
                /* Dots become slashes and slashes become dots. Fun. */
                do {
                        အကယ်၍ (*n == '.')
                                *n = '/';
                        မဟုတ်ပါက
                                *n = '.';

                        n = strpbrk(n + 1, "/.");
                } နေစဉ် (n);

        path_simplify(s);

        /* Kill the leading slash, but keep the first character of the string in the same place. */
        အကယ်၍ (s[0] == '/' && s[1] != 0)
                memmove(s, s+1, strlen(s));

        ပြန်ပေး s;
}

static ကိန်းပြည့် shadow_update(Hashmap **shadow, const var_na *property, const var_na *value) {
        _cleanup_free_ var_na *k = NULL, *v = NULL, *cur_k = NULL, *cur_v = NULL;
        ကိန်းပြည့် r;

        assert(property);
        assert(value);

        အကယ်၍ (!shadow)
                ပြန်ပေး 0;

        k = strdup(property);
        အကယ်၍ (!k)
                ပြန်ပေး -ENOMEM;

        v = strdup(value);
        အကယ်၍ (!v)
                ပြန်ပေး -ENOMEM;

        cur_v = hashmap_remove2(*shadow, k, (void**)&cur_k);

        r = hashmap_ensure_put(shadow, &path_hash_ops_free_free, k, v);
        အကယ်၍ (r < 0) {
                assert(r != -EEXIST);

                ပြန်ပေး r;
        }

        TAKE_PTR(k);
        TAKE_PTR(v);

        ပြန်ပေး 0;
}

ကိန်းပြည့် sysctl_write_full(const var_na *property, const var_na *value, Hashmap **shadow) {
        var_na *p;
        ကိန်းပြည့် r;

        assert(property);
        assert(value);

        p = strjoina("/proc/sys/", property);

        path_simplify(p);
        အကယ်၍ (!path_is_normalized(p))
                ပြန်ပေး -EINVAL;

        log_debug("Setting '%s' to '%s'", p, value);

        r = shadow_update(shadow, p, value);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        ပြန်ပေး write_string_file(p, value, WRITE_STRING_FILE_VERIFY_ON_FAILURE | WRITE_STRING_FILE_DISABLE_BUFFER | WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL);
}

ကိန်းပြည့် sysctl_writef(const var_na *property, const var_na *format, ...) {
        _cleanup_free_ var_na *v = NULL;
        va_list ap;
        ကိန်းပြည့် r;

        va_start(ap, format);
        r = vasprintf(&v, format, ap);
        va_end(ap);

        အကယ်၍ (r < 0)
                ပြန်ပေး -ENOMEM;

        ပြန်ပေး sysctl_write(property, v);
}

ကိန်းပြည့် sysctl_write_verify(const var_na *property, const var_na *value) {
        ကိန်းပြည့် r;

        assert(property);
        assert(value);

        /* Some sysctl settings accept invalid values on write, but refuses on read. E.g. coredump pattern,
         * be1e0283021ec73c2eb92839db9a471a068709d9 (v6.17) and 7d7c1fb85cba5627bbe741fb7539c709435e3848
         * (v6.16.8), which is fixed by a779e27f24aeb679969ddd1fdd7f636e22ddbc1e (v6.18) and
         * 304aa560385720baf3660fe8500f6dd425b63ea9 (v6.17.5). Let's first save the original value, and
         * restore to the saved value if the new value is refused. */

        _cleanup_free_ var_na *original = NULL;
        r = sysctl_read(property, &original);
        အကယ်၍ (r >= 0 && streq(original, value))
                ပြန်ပေး 0; /* Already set. */

        r = sysctl_write(property, value);
        အကယ်၍ (r >= 0) {
                _cleanup_free_ var_na *current = NULL;
                r = sysctl_read(property, &current);
                အကယ်၍ (r >= 0) {
                        အကယ်၍ (streq(current, value))
                                ပြန်ပေး 0; /* Yay! */

                        /* At least for coredump pattern, this does not happen, but let's handle this as the
                         * same as we wrote something invalid. */
                        r = -EINVAL;
                }
        }

        အကယ်၍ (original)
                (void) sysctl_write(property, original);

        ပြန်ပေး r;
}

static const var_na* af_to_sysctl_dir(ကိန်းပြည့် af) {
        အကယ်၍ (af == AF_MPLS)
                ပြန်ပေး "mpls";

        ပြန်ပေး af_to_ipv4_ipv6(af);
}

ကိန်းပြည့် sysctl_write_ip_property(ကိန်းပြည့် af, const var_na *ifname, const var_na *property, const var_na *value, Hashmap **shadow) {
        const var_na *p;

        assert(property);
        assert(value);

        အကယ်၍ (!IN_SET(af, AF_INET, AF_INET6, AF_MPLS))
                ပြန်ပေး -EAFNOSUPPORT;

        အကယ်၍ (ifname) {
                အကယ်၍ (!ifname_valid_full(ifname, IFNAME_VALID_SPECIAL))
                        ပြန်ပေး -EINVAL;

                p = strjoina("net/", af_to_sysctl_dir(af), "/conf/", ifname, "/", property);
        } မဟုတ်ပါက
                p = strjoina("net/", af_to_sysctl_dir(af), "/", property);

        ပြန်ပေး sysctl_write_full(p, value, shadow);
}

ကိန်းပြည့် sysctl_write_ip_property_boolean(ကိန်းပြည့် af, const var_na *ifname, const var_na *property, bool value, Hashmap **shadow) {
        ပြန်ပေး sysctl_write_ip_property(af, ifname, property, one_zero(value), shadow);
}

ကိန်းပြည့် sysctl_write_ip_neighbor_property(ကိန်းပြည့် af, const var_na *ifname, const var_na *property, const var_na *value, Hashmap **shadow) {
        const var_na *p;

        assert(property);
        assert(value);
        assert(ifname);

        အကယ်၍ (!IN_SET(af, AF_INET, AF_INET6))
                ပြန်ပေး -EAFNOSUPPORT;

        အကယ်၍ (ifname) {
                အကယ်၍ (!ifname_valid_full(ifname, IFNAME_VALID_SPECIAL))
                        ပြန်ပေး -EINVAL;
                p = strjoina("net/", af_to_ipv4_ipv6(af), "/neigh/", ifname, "/", property);
        } မဟုတ်ပါက
                p = strjoina("net/", af_to_ipv4_ipv6(af), "/neigh/default/", property);

        ပြန်ပေး sysctl_write_full(p, value, shadow);
}

ကိန်းပြည့် sysctl_write_ip_neighbor_property_uint32(ကိန်းပြည့် af, const var_na *ifname, const var_na *property, uint32_t value, Hashmap **shadow) {
        var_na buf[DECIMAL_STR_MAX(uint32_t)];
        xsprintf(buf, "%u", value);
        ပြန်ပေး sysctl_write_ip_neighbor_property(af, ifname, property, buf, shadow);
}

ကိန်းပြည့် sysctl_read(const var_na *property, var_na **ret) {
        var_na *p;
        ကိန်းပြည့် r;

        assert(property);

        p = strjoina("/proc/sys/", property);

        path_simplify(p);
        အကယ်၍ (!path_is_normalized(p)) /* Filter out attempts to write to /proc/sys/../../…, just in case */
                ပြန်ပေး -EINVAL;

        r = read_full_virtual_file(p, ret, NULL);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;
        အကယ်၍ (ret)
                delete_trailing_chars(*ret, NEWLINE);

        ပြန်ပေး r;
}

ကိန်းပြည့် sysctl_read_ip_property(ကိန်းပြည့် af, const var_na *ifname, const var_na *property, var_na **ret) {
        const var_na *p;

        assert(property);

        အကယ်၍ (!IN_SET(af, AF_INET, AF_INET6, AF_MPLS))
                ပြန်ပေး -EAFNOSUPPORT;

        အကယ်၍ (ifname) {
                အကယ်၍ (!ifname_valid_full(ifname, IFNAME_VALID_SPECIAL))
                        ပြန်ပေး -EINVAL;

                p = strjoina("net/", af_to_sysctl_dir(af), "/conf/", ifname, "/", property);
        } မဟုတ်ပါက
                p = strjoina("net/", af_to_sysctl_dir(af), "/", property);

        ပြန်ပေး sysctl_read(p, ret);
}

ကိန်းပြည့် sysctl_read_ip_property_int(ကိန်းပြည့် af, const var_na *ifname, const var_na *property, ကိန်းပြည့် *ret) {
        _cleanup_free_ var_na *s = NULL;
        ကိန်းပြည့် r;

        assert(ret);

        r = sysctl_read_ip_property(af, ifname, property, &s);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        ပြန်ပေး safe_atoi(s, ret);
}

ကိန်းပြည့် sysctl_read_ip_property_uint32(ကိန်းပြည့် af, const var_na *ifname, const var_na *property, uint32_t *ret) {
        _cleanup_free_ var_na *s = NULL;
        ကိန်းပြည့် r;

        assert(ret);

        r = sysctl_read_ip_property(af, ifname, property, &s);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        ပြန်ပေး safe_atou32(s, ret);
}
