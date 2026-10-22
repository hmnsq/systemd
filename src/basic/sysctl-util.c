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

        ຖ້າ (n && *n == '.')
                /* Dots become slashes and slashes become dots. Fun. */
                do {
                        ຖ້າ (*n == '.')
                                *n = '/';
                        ບໍ່ດັ່ງນັ້ນ
                                *n = '.';

                        n = strpbrk(n + 1, "/.");
                } ໃນຂະນະທີ່ (n);

        path_simplify(s);

        /* Kill the leading slash, but keep the first character of the string in the same place. */
        ຖ້າ (s[0] == '/' && s[1] != 0)
                memmove(s, s+1, strlen(s));

        ສົ່ງຄືນ s;
}

static ຈຳນວນເຕັມ shadow_update(Hashmap **shadow, const var_na *property, const var_na *value) {
        _cleanup_free_ var_na *k = NULL, *v = NULL, *cur_k = NULL, *cur_v = NULL;
        ຈຳນວນເຕັມ r;

        assert(property);
        assert(value);

        ຖ້າ (!shadow)
                ສົ່ງຄືນ 0;

        k = strdup(property);
        ຖ້າ (!k)
                ສົ່ງຄືນ -ENOMEM;

        v = strdup(value);
        ຖ້າ (!v)
                ສົ່ງຄືນ -ENOMEM;

        cur_v = hashmap_remove2(*shadow, k, (void**)&cur_k);

        r = hashmap_ensure_put(shadow, &path_hash_ops_free_free, k, v);
        ຖ້າ (r < 0) {
                assert(r != -EEXIST);

                ສົ່ງຄືນ r;
        }

        TAKE_PTR(k);
        TAKE_PTR(v);

        ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ sysctl_write_full(const var_na *property, const var_na *value, Hashmap **shadow) {
        var_na *p;
        ຈຳນວນເຕັມ r;

        assert(property);
        assert(value);

        p = strjoina("/proc/sys/", property);

        path_simplify(p);
        ຖ້າ (!path_is_normalized(p))
                ສົ່ງຄືນ -EINVAL;

        log_debug("Setting '%s' to '%s'", p, value);

        r = shadow_update(shadow, p, value);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;

        ສົ່ງຄືນ write_string_file(p, value, WRITE_STRING_FILE_VERIFY_ON_FAILURE | WRITE_STRING_FILE_DISABLE_BUFFER | WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL);
}

ຈຳນວນເຕັມ sysctl_writef(const var_na *property, const var_na *format, ...) {
        _cleanup_free_ var_na *v = NULL;
        va_list ap;
        ຈຳນວນເຕັມ r;

        va_start(ap, format);
        r = vasprintf(&v, format, ap);
        va_end(ap);

        ຖ້າ (r < 0)
                ສົ່ງຄືນ -ENOMEM;

        ສົ່ງຄືນ sysctl_write(property, v);
}

ຈຳນວນເຕັມ sysctl_write_verify(const var_na *property, const var_na *value) {
        ຈຳນວນເຕັມ r;

        assert(property);
        assert(value);

        /* Some sysctl settings accept invalid values on write, but refuses on read. E.g. coredump pattern,
         * be1e0283021ec73c2eb92839db9a471a068709d9 (v6.17) and 7d7c1fb85cba5627bbe741fb7539c709435e3848
         * (v6.16.8), which is fixed by a779e27f24aeb679969ddd1fdd7f636e22ddbc1e (v6.18) and
         * 304aa560385720baf3660fe8500f6dd425b63ea9 (v6.17.5). Let's first save the original value, and
         * restore to the saved value if the new value is refused. */

        _cleanup_free_ var_na *original = NULL;
        r = sysctl_read(property, &original);
        ຖ້າ (r >= 0 && streq(original, value))
                ສົ່ງຄືນ 0; /* Already set. */

        r = sysctl_write(property, value);
        ຖ້າ (r >= 0) {
                _cleanup_free_ var_na *current = NULL;
                r = sysctl_read(property, &current);
                ຖ້າ (r >= 0) {
                        ຖ້າ (streq(current, value))
                                ສົ່ງຄືນ 0; /* Yay! */

                        /* At least for coredump pattern, this does not happen, but let's handle this as the
                         * same as we wrote something invalid. */
                        r = -EINVAL;
                }
        }

        ຖ້າ (original)
                (void) sysctl_write(property, original);

        ສົ່ງຄືນ r;
}

static const var_na* af_to_sysctl_dir(ຈຳນວນເຕັມ af) {
        ຖ້າ (af == AF_MPLS)
                ສົ່ງຄືນ "mpls";

        ສົ່ງຄືນ af_to_ipv4_ipv6(af);
}

ຈຳນວນເຕັມ sysctl_write_ip_property(ຈຳນວນເຕັມ af, const var_na *ifname, const var_na *property, const var_na *value, Hashmap **shadow) {
        const var_na *p;

        assert(property);
        assert(value);

        ຖ້າ (!IN_SET(af, AF_INET, AF_INET6, AF_MPLS))
                ສົ່ງຄືນ -EAFNOSUPPORT;

        ຖ້າ (ifname) {
                ຖ້າ (!ifname_valid_full(ifname, IFNAME_VALID_SPECIAL))
                        ສົ່ງຄືນ -EINVAL;

                p = strjoina("net/", af_to_sysctl_dir(af), "/conf/", ifname, "/", property);
        } ບໍ່ດັ່ງນັ້ນ
                p = strjoina("net/", af_to_sysctl_dir(af), "/", property);

        ສົ່ງຄືນ sysctl_write_full(p, value, shadow);
}

ຈຳນວນເຕັມ sysctl_write_ip_property_boolean(ຈຳນວນເຕັມ af, const var_na *ifname, const var_na *property, bool value, Hashmap **shadow) {
        ສົ່ງຄືນ sysctl_write_ip_property(af, ifname, property, one_zero(value), shadow);
}

ຈຳນວນເຕັມ sysctl_write_ip_neighbor_property(ຈຳນວນເຕັມ af, const var_na *ifname, const var_na *property, const var_na *value, Hashmap **shadow) {
        const var_na *p;

        assert(property);
        assert(value);
        assert(ifname);

        ຖ້າ (!IN_SET(af, AF_INET, AF_INET6))
                ສົ່ງຄືນ -EAFNOSUPPORT;

        ຖ້າ (ifname) {
                ຖ້າ (!ifname_valid_full(ifname, IFNAME_VALID_SPECIAL))
                        ສົ່ງຄືນ -EINVAL;
                p = strjoina("net/", af_to_ipv4_ipv6(af), "/neigh/", ifname, "/", property);
        } ບໍ່ດັ່ງນັ້ນ
                p = strjoina("net/", af_to_ipv4_ipv6(af), "/neigh/default/", property);

        ສົ່ງຄືນ sysctl_write_full(p, value, shadow);
}

ຈຳນວນເຕັມ sysctl_write_ip_neighbor_property_uint32(ຈຳນວນເຕັມ af, const var_na *ifname, const var_na *property, uint32_t value, Hashmap **shadow) {
        var_na buf[DECIMAL_STR_MAX(uint32_t)];
        xsprintf(buf, "%u", value);
        ສົ່ງຄືນ sysctl_write_ip_neighbor_property(af, ifname, property, buf, shadow);
}

ຈຳນວນເຕັມ sysctl_read(const var_na *property, var_na **ret) {
        var_na *p;
        ຈຳນວນເຕັມ r;

        assert(property);

        p = strjoina("/proc/sys/", property);

        path_simplify(p);
        ຖ້າ (!path_is_normalized(p)) /* Filter out attempts to write to /proc/sys/../../…, just in case */
                ສົ່ງຄືນ -EINVAL;

        r = read_full_virtual_file(p, ret, NULL);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;
        ຖ້າ (ret)
                delete_trailing_chars(*ret, NEWLINE);

        ສົ່ງຄືນ r;
}

ຈຳນວນເຕັມ sysctl_read_ip_property(ຈຳນວນເຕັມ af, const var_na *ifname, const var_na *property, var_na **ret) {
        const var_na *p;

        assert(property);

        ຖ້າ (!IN_SET(af, AF_INET, AF_INET6, AF_MPLS))
                ສົ່ງຄືນ -EAFNOSUPPORT;

        ຖ້າ (ifname) {
                ຖ້າ (!ifname_valid_full(ifname, IFNAME_VALID_SPECIAL))
                        ສົ່ງຄືນ -EINVAL;

                p = strjoina("net/", af_to_sysctl_dir(af), "/conf/", ifname, "/", property);
        } ບໍ່ດັ່ງນັ້ນ
                p = strjoina("net/", af_to_sysctl_dir(af), "/", property);

        ສົ່ງຄືນ sysctl_read(p, ret);
}

ຈຳນວນເຕັມ sysctl_read_ip_property_int(ຈຳນວນເຕັມ af, const var_na *ifname, const var_na *property, ຈຳນວນເຕັມ *ret) {
        _cleanup_free_ var_na *s = NULL;
        ຈຳນວນເຕັມ r;

        assert(ret);

        r = sysctl_read_ip_property(af, ifname, property, &s);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;

        ສົ່ງຄືນ safe_atoi(s, ret);
}

ຈຳນວນເຕັມ sysctl_read_ip_property_uint32(ຈຳນວນເຕັມ af, const var_na *ifname, const var_na *property, uint32_t *ret) {
        _cleanup_free_ var_na *s = NULL;
        ຈຳນວນເຕັມ r;

        assert(ret);

        r = sysctl_read_ip_property(af, ifname, property, &s);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;

        ສົ່ງຄືນ safe_atou32(s, ret);
}
