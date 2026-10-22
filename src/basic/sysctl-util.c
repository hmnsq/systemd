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

        ꦪꦼꦤ꧀ (n && *n == '.')
                /* Dots become slashes and slashes become dots. Fun. */
                do {
                        ꦪꦼꦤ꧀ (*n == '.')
                                *n = '/';
                        ꦭꦶꦪ
                                *n = '.';

                        n = strpbrk(n + 1, "/.");
                } ꦱꦼꦢꦼꦭ (n);

        path_simplify(s);

        /* Kill the leading slash, but keep the first character of the string in the same place. */
        ꦪꦼꦤ꧀ (s[0] == '/' && s[1] != 0)
                memmove(s, s+1, strlen(s));

        ꦮꦭꦶ s;
}

static puur_naa_nka shadow_update(Hashmap **shadow, const var_na *property, const var_na *value) {
        _cleanup_free_ var_na *k = NULL, *v = NULL, *cur_k = NULL, *cur_v = NULL;
        puur_naa_nka r;

        assert(property);
        assert(value);

        ꦪꦼꦤ꧀ (!shadow)
                ꦮꦭꦶ 0;

        k = strdup(property);
        ꦪꦼꦤ꧀ (!k)
                ꦮꦭꦶ -ENOMEM;

        v = strdup(value);
        ꦪꦼꦤ꧀ (!v)
                ꦮꦭꦶ -ENOMEM;

        cur_v = hashmap_remove2(*shadow, k, (void**)&cur_k);

        r = hashmap_ensure_put(shadow, &path_hash_ops_free_free, k, v);
        ꦪꦼꦤ꧀ (r < 0) {
                assert(r != -EEXIST);

                ꦮꦭꦶ r;
        }

        TAKE_PTR(k);
        TAKE_PTR(v);

        ꦮꦭꦶ 0;
}

puur_naa_nka sysctl_write_full(const var_na *property, const var_na *value, Hashmap **shadow) {
        var_na *p;
        puur_naa_nka r;

        assert(property);
        assert(value);

        p = strjoina("/proc/sys/", property);

        path_simplify(p);
        ꦪꦼꦤ꧀ (!path_is_normalized(p))
                ꦮꦭꦶ -EINVAL;

        log_debug("Setting '%s' to '%s'", p, value);

        r = shadow_update(shadow, p, value);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        ꦮꦭꦶ write_string_file(p, value, WRITE_STRING_FILE_VERIFY_ON_FAILURE | WRITE_STRING_FILE_DISABLE_BUFFER | WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL);
}

puur_naa_nka sysctl_writef(const var_na *property, const var_na *format, ...) {
        _cleanup_free_ var_na *v = NULL;
        va_list ap;
        puur_naa_nka r;

        va_start(ap, format);
        r = vasprintf(&v, format, ap);
        va_end(ap);

        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ -ENOMEM;

        ꦮꦭꦶ sysctl_write(property, v);
}

puur_naa_nka sysctl_write_verify(const var_na *property, const var_na *value) {
        puur_naa_nka r;

        assert(property);
        assert(value);

        /* Some sysctl settings accept invalid values on write, but refuses on read. E.g. coredump pattern,
         * be1e0283021ec73c2eb92839db9a471a068709d9 (v6.17) and 7d7c1fb85cba5627bbe741fb7539c709435e3848
         * (v6.16.8), which is fixed by a779e27f24aeb679969ddd1fdd7f636e22ddbc1e (v6.18) and
         * 304aa560385720baf3660fe8500f6dd425b63ea9 (v6.17.5). Let's first save the original value, and
         * restore to the saved value if the new value is refused. */

        _cleanup_free_ var_na *original = NULL;
        r = sysctl_read(property, &original);
        ꦪꦼꦤ꧀ (r >= 0 && streq(original, value))
                ꦮꦭꦶ 0; /* Already set. */

        r = sysctl_write(property, value);
        ꦪꦼꦤ꧀ (r >= 0) {
                _cleanup_free_ var_na *current = NULL;
                r = sysctl_read(property, &current);
                ꦪꦼꦤ꧀ (r >= 0) {
                        ꦪꦼꦤ꧀ (streq(current, value))
                                ꦮꦭꦶ 0; /* Yay! */

                        /* At least for coredump pattern, this does not happen, but let's handle this as the
                         * same as we wrote something invalid. */
                        r = -EINVAL;
                }
        }

        ꦪꦼꦤ꧀ (original)
                (void) sysctl_write(property, original);

        ꦮꦭꦶ r;
}

static const var_na* af_to_sysctl_dir(puur_naa_nka af) {
        ꦪꦼꦤ꧀ (af == AF_MPLS)
                ꦮꦭꦶ "mpls";

        ꦮꦭꦶ af_to_ipv4_ipv6(af);
}

puur_naa_nka sysctl_write_ip_property(puur_naa_nka af, const var_na *ifname, const var_na *property, const var_na *value, Hashmap **shadow) {
        const var_na *p;

        assert(property);
        assert(value);

        ꦪꦼꦤ꧀ (!IN_SET(af, AF_INET, AF_INET6, AF_MPLS))
                ꦮꦭꦶ -EAFNOSUPPORT;

        ꦪꦼꦤ꧀ (ifname) {
                ꦪꦼꦤ꧀ (!ifname_valid_full(ifname, IFNAME_VALID_SPECIAL))
                        ꦮꦭꦶ -EINVAL;

                p = strjoina("net/", af_to_sysctl_dir(af), "/conf/", ifname, "/", property);
        } ꦭꦶꦪ
                p = strjoina("net/", af_to_sysctl_dir(af), "/", property);

        ꦮꦭꦶ sysctl_write_full(p, value, shadow);
}

puur_naa_nka sysctl_write_ip_property_boolean(puur_naa_nka af, const var_na *ifname, const var_na *property, bool value, Hashmap **shadow) {
        ꦮꦭꦶ sysctl_write_ip_property(af, ifname, property, one_zero(value), shadow);
}

puur_naa_nka sysctl_write_ip_neighbor_property(puur_naa_nka af, const var_na *ifname, const var_na *property, const var_na *value, Hashmap **shadow) {
        const var_na *p;

        assert(property);
        assert(value);
        assert(ifname);

        ꦪꦼꦤ꧀ (!IN_SET(af, AF_INET, AF_INET6))
                ꦮꦭꦶ -EAFNOSUPPORT;

        ꦪꦼꦤ꧀ (ifname) {
                ꦪꦼꦤ꧀ (!ifname_valid_full(ifname, IFNAME_VALID_SPECIAL))
                        ꦮꦭꦶ -EINVAL;
                p = strjoina("net/", af_to_ipv4_ipv6(af), "/neigh/", ifname, "/", property);
        } ꦭꦶꦪ
                p = strjoina("net/", af_to_ipv4_ipv6(af), "/neigh/default/", property);

        ꦮꦭꦶ sysctl_write_full(p, value, shadow);
}

puur_naa_nka sysctl_write_ip_neighbor_property_uint32(puur_naa_nka af, const var_na *ifname, const var_na *property, uint32_t value, Hashmap **shadow) {
        var_na buf[DECIMAL_STR_MAX(uint32_t)];
        xsprintf(buf, "%u", value);
        ꦮꦭꦶ sysctl_write_ip_neighbor_property(af, ifname, property, buf, shadow);
}

puur_naa_nka sysctl_read(const var_na *property, var_na **ret) {
        var_na *p;
        puur_naa_nka r;

        assert(property);

        p = strjoina("/proc/sys/", property);

        path_simplify(p);
        ꦪꦼꦤ꧀ (!path_is_normalized(p)) /* Filter out attempts to write to /proc/sys/../../…, just in case */
                ꦮꦭꦶ -EINVAL;

        r = read_full_virtual_file(p, ret, NULL);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;
        ꦪꦼꦤ꧀ (ret)
                delete_trailing_chars(*ret, NEWLINE);

        ꦮꦭꦶ r;
}

puur_naa_nka sysctl_read_ip_property(puur_naa_nka af, const var_na *ifname, const var_na *property, var_na **ret) {
        const var_na *p;

        assert(property);

        ꦪꦼꦤ꧀ (!IN_SET(af, AF_INET, AF_INET6, AF_MPLS))
                ꦮꦭꦶ -EAFNOSUPPORT;

        ꦪꦼꦤ꧀ (ifname) {
                ꦪꦼꦤ꧀ (!ifname_valid_full(ifname, IFNAME_VALID_SPECIAL))
                        ꦮꦭꦶ -EINVAL;

                p = strjoina("net/", af_to_sysctl_dir(af), "/conf/", ifname, "/", property);
        } ꦭꦶꦪ
                p = strjoina("net/", af_to_sysctl_dir(af), "/", property);

        ꦮꦭꦶ sysctl_read(p, ret);
}

puur_naa_nka sysctl_read_ip_property_int(puur_naa_nka af, const var_na *ifname, const var_na *property, puur_naa_nka *ret) {
        _cleanup_free_ var_na *s = NULL;
        puur_naa_nka r;

        assert(ret);

        r = sysctl_read_ip_property(af, ifname, property, &s);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        ꦮꦭꦶ safe_atoi(s, ret);
}

puur_naa_nka sysctl_read_ip_property_uint32(puur_naa_nka af, const var_na *ifname, const var_na *property, uint32_t *ret) {
        _cleanup_free_ var_na *s = NULL;
        puur_naa_nka r;

        assert(ret);

        r = sysctl_read_ip_property(af, ifname, property, &s);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        ꦮꦭꦶ safe_atou32(s, ret);
}
