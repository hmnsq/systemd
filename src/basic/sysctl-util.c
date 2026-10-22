/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <stdio.h>

#ಅನ್ತರ್ಭಾವ "af-list.h"
#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "fileio.h"
#ಅನ್ತರ್ಭಾವ "hashmap.h"
#ಅನ್ತರ್ಭಾವ "log.h"
#ಅನ್ತರ್ಭಾವ "parse-util.h"
#ಅನ್ತರ್ಭಾವ "path-util.h"
#ಅನ್ತರ್ಭಾವ "socket-util.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"
#ಅನ್ತರ್ಭಾವ "sysctl-util.h"

ವರ್ಣ* sysctl_normalize(ವರ್ಣ *s) {
        ವರ್ಣ *n;

        ಪ್ರತಿಜ್ಞಾ(s);

        n = strpbrk(s, "/.");

        /* If the first separator is a slash, the path is
         * assumed to be normalized and slashes remain slashes
         * and dots remains dots. */

        ಯದಿ (n && *n == '.')
                /* Dots become slashes and slashes become dots. Fun. */
                ಕುರು {
                        ಯದಿ (*n == '.')
                                *n = '/';
                        ಅನ್ಯಥಾ
                                *n = '.';

                        n = strpbrk(n + 1, "/.");
                } ಯಾವತ್ (n);

        path_simplify(s);

        /* Kill the leading slash, but keep the first character of the string in the same place. */
        ಯದಿ (s[0] == '/' && s[1] != 0)
                memmove(s, s+1, strlen(s));

        ಪ್ರತ್ಯಾಗಮ s;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ shadow_update(Hashmap **shadow, ಧ್ರುವ ವರ್ಣ *property, ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ) {
        _cleanup_free_ ವರ್ಣ *k = ರಿಕ್ತ, *v = ರಿಕ್ತ, *cur_k = ರಿಕ್ತ, *cur_v = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(property);
        ಪ್ರತಿಜ್ಞಾ(ಮೌಲ್ಯ);

        ಯದಿ (!shadow)
                ಪ್ರತ್ಯಾಗಮ 0;

        k = strdup(property);
        ಯದಿ (!k)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        v = strdup(ಮೌಲ್ಯ);
        ಯದಿ (!v)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        cur_v = hashmap_remove2(*shadow, k, (ಶೂನ್ಯ**)&cur_k);

        r = hashmap_ensure_put(shadow, &path_hash_ops_free_free, k, v);
        ಯದಿ (r < 0) {
                ಪ್ರತಿಜ್ಞಾ(r != -EEXIST);

                ಪ್ರತ್ಯಾಗಮ r;
        }

        TAKE_PTR(k);
        TAKE_PTR(v);

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ sysctl_write_full(ಧ್ರುವ ವರ್ಣ *property, ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ, Hashmap **shadow) {
        ವರ್ಣ *p;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(property);
        ಪ್ರತಿಜ್ಞಾ(ಮೌಲ್ಯ);

        p = strjoina("/proc/sys/", property);

        path_simplify(p);
        ಯದಿ (!path_is_normalized(p))
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        log_debug("Setting '%s' to '%s'", p, ಮೌಲ್ಯ);

        r = shadow_update(shadow, p, ಮೌಲ್ಯ);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ write_string_file(p, ಮೌಲ್ಯ, WRITE_STRING_FILE_VERIFY_ON_FAILURE | WRITE_STRING_FILE_DISABLE_BUFFER | WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL);
}

ಪೂರ್ಣಾಙ್ಕ sysctl_writef(ಧ್ರುವ ವರ್ಣ *property, ಧ್ರುವ ವರ್ಣ *format, ...) {
        _cleanup_free_ ವರ್ಣ *v = ರಿಕ್ತ;
        va_list ap;
        ಪೂರ್ಣಾಙ್ಕ r;

        va_start(ap, format);
        r = vasprintf(&v, format, ap);
        va_end(ap);

        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        ಪ್ರತ್ಯಾಗಮ sysctl_write(property, v);
}

ಪೂರ್ಣಾಙ್ಕ sysctl_write_verify(ಧ್ರುವ ವರ್ಣ *property, ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(property);
        ಪ್ರತಿಜ್ಞಾ(ಮೌಲ್ಯ);

        /* Some sysctl settings accept invalid values on write, but refuses on read. E.g. coredump pattern,
         * be1e0283021ec73c2eb92839db9a471a068709d9 (v6.17) and 7d7c1fb85cba5627bbe741fb7539c709435e3848
         * (v6.16.8), which is fixed by a779e27f24aeb679969ddd1fdd7f636e22ddbc1e (v6.18) and
         * 304aa560385720baf3660fe8500f6dd425b63ea9 (v6.17.5). Let's first save the original value, and
         * restore to the saved value if the new value is refused. */

        _cleanup_free_ ವರ್ಣ *original = ರಿಕ್ತ;
        r = sysctl_read(property, &original);
        ಯದಿ (r >= 0 && streq(original, ಮೌಲ್ಯ))
                ಪ್ರತ್ಯಾಗಮ 0; /* Already set. */

        r = sysctl_write(property, ಮೌಲ್ಯ);
        ಯದಿ (r >= 0) {
                _cleanup_free_ ವರ್ಣ *current = ರಿಕ್ತ;
                r = sysctl_read(property, &current);
                ಯದಿ (r >= 0) {
                        ಯದಿ (streq(current, ಮೌಲ್ಯ))
                                ಪ್ರತ್ಯಾಗಮ 0; /* Yay! */

                        /* At least for coredump pattern, this does not happen, but let's handle this as the
                         * same as we wrote something invalid. */
                        r = -EINVAL;
                }
        }

        ಯದಿ (original)
                (ಶೂನ್ಯ) sysctl_write(property, original);

        ಪ್ರತ್ಯಾಗಮ r;
}

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ* af_to_sysctl_dir(ಪೂರ್ಣಾಙ್ಕ af) {
        ಯದಿ (af == AF_MPLS)
                ಪ್ರತ್ಯಾಗಮ "mpls";

        ಪ್ರತ್ಯಾಗಮ af_to_ipv4_ipv6(af);
}

ಪೂರ್ಣಾಙ್ಕ sysctl_write_ip_property(ಪೂರ್ಣಾಙ್ಕ af, ಧ್ರುವ ವರ್ಣ *ifname, ಧ್ರುವ ವರ್ಣ *property, ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ, Hashmap **shadow) {
        ಧ್ರುವ ವರ್ಣ *p;

        ಪ್ರತಿಜ್ಞಾ(property);
        ಪ್ರತಿಜ್ಞಾ(ಮೌಲ್ಯ);

        ಯದಿ (!IN_SET(af, AF_INET, AF_INET6, AF_MPLS))
                ಪ್ರತ್ಯಾಗಮ -EAFNOSUPPORT;

        ಯದಿ (ifname) {
                ಯದಿ (!ifname_valid_full(ifname, IFNAME_VALID_SPECIAL))
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                p = strjoina("net/", af_to_sysctl_dir(af), "/conf/", ifname, "/", property);
        } ಅನ್ಯಥಾ
                p = strjoina("net/", af_to_sysctl_dir(af), "/", property);

        ಪ್ರತ್ಯಾಗಮ sysctl_write_full(p, ಮೌಲ್ಯ, shadow);
}

ಪೂರ್ಣಾಙ್ಕ sysctl_write_ip_property_boolean(ಪೂರ್ಣಾಙ್ಕ af, ಧ್ರುವ ವರ್ಣ *ifname, ಧ್ರುವ ವರ್ಣ *property, bool ಮೌಲ್ಯ, Hashmap **shadow) {
        ಪ್ರತ್ಯಾಗಮ sysctl_write_ip_property(af, ifname, property, one_zero(ಮೌಲ್ಯ), shadow);
}

ಪೂರ್ಣಾಙ್ಕ sysctl_write_ip_neighbor_property(ಪೂರ್ಣಾಙ್ಕ af, ಧ್ರುವ ವರ್ಣ *ifname, ಧ್ರುವ ವರ್ಣ *property, ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ, Hashmap **shadow) {
        ಧ್ರುವ ವರ್ಣ *p;

        ಪ್ರತಿಜ್ಞಾ(property);
        ಪ್ರತಿಜ್ಞಾ(ಮೌಲ್ಯ);
        ಪ್ರತಿಜ್ಞಾ(ifname);

        ಯದಿ (!IN_SET(af, AF_INET, AF_INET6))
                ಪ್ರತ್ಯಾಗಮ -EAFNOSUPPORT;

        ಯದಿ (ifname) {
                ಯದಿ (!ifname_valid_full(ifname, IFNAME_VALID_SPECIAL))
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;
                p = strjoina("net/", af_to_ipv4_ipv6(af), "/neigh/", ifname, "/", property);
        } ಅನ್ಯಥಾ
                p = strjoina("net/", af_to_ipv4_ipv6(af), "/neigh/default/", property);

        ಪ್ರತ್ಯಾಗಮ sysctl_write_full(p, ಮೌಲ್ಯ, shadow);
}

ಪೂರ್ಣಾಙ್ಕ sysctl_write_ip_neighbor_property_uint32(ಪೂರ್ಣಾಙ್ಕ af, ಧ್ರುವ ವರ್ಣ *ifname, ಧ್ರುವ ವರ್ಣ *property, uint32_t ಮೌಲ್ಯ, Hashmap **shadow) {
        ವರ್ಣ buf[DECIMAL_STR_MAX(uint32_t)];
        xsprintf(buf, "%u", ಮೌಲ್ಯ);
        ಪ್ರತ್ಯಾಗಮ sysctl_write_ip_neighbor_property(af, ifname, property, buf, shadow);
}

ಪೂರ್ಣಾಙ್ಕ sysctl_read(ಧ್ರುವ ವರ್ಣ *property, ವರ್ಣ **ret) {
        ವರ್ಣ *p;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(property);

        p = strjoina("/proc/sys/", property);

        path_simplify(p);
        ಯದಿ (!path_is_normalized(p)) /* Filter out attempts to write to /proc/sys/../../…, just in case */
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        r = read_full_virtual_file(p, ret, ರಿಕ್ತ);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;
        ಯದಿ (ret)
                delete_trailing_chars(*ret, NEWLINE);

        ಪ್ರತ್ಯಾಗಮ r;
}

ಪೂರ್ಣಾಙ್ಕ sysctl_read_ip_property(ಪೂರ್ಣಾಙ್ಕ af, ಧ್ರುವ ವರ್ಣ *ifname, ಧ್ರುವ ವರ್ಣ *property, ವರ್ಣ **ret) {
        ಧ್ರುವ ವರ್ಣ *p;

        ಪ್ರತಿಜ್ಞಾ(property);

        ಯದಿ (!IN_SET(af, AF_INET, AF_INET6, AF_MPLS))
                ಪ್ರತ್ಯಾಗಮ -EAFNOSUPPORT;

        ಯದಿ (ifname) {
                ಯದಿ (!ifname_valid_full(ifname, IFNAME_VALID_SPECIAL))
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                p = strjoina("net/", af_to_sysctl_dir(af), "/conf/", ifname, "/", property);
        } ಅನ್ಯಥಾ
                p = strjoina("net/", af_to_sysctl_dir(af), "/", property);

        ಪ್ರತ್ಯಾಗಮ sysctl_read(p, ret);
}

ಪೂರ್ಣಾಙ್ಕ sysctl_read_ip_property_int(ಪೂರ್ಣಾಙ್ಕ af, ಧ್ರುವ ವರ್ಣ *ifname, ಧ್ರುವ ವರ್ಣ *property, ಪೂರ್ಣಾಙ್ಕ *ret) {
        _cleanup_free_ ವರ್ಣ *s = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        r = sysctl_read_ip_property(af, ifname, property, &s);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ safe_atoi(s, ret);
}

ಪೂರ್ಣಾಙ್ಕ sysctl_read_ip_property_uint32(ಪೂರ್ಣಾಙ್ಕ af, ಧ್ರುವ ವರ್ಣ *ifname, ಧ್ರುವ ವರ್ಣ *property, uint32_t *ret) {
        _cleanup_free_ ವರ್ಣ *s = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        r = sysctl_read_ip_property(af, ifname, property, &s);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ safe_atou32(s, ret);
}
