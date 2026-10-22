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

ಅಕ್ಷರ* sysctl_normalize(ಅಕ್ಷರ *s) {
        ಅಕ್ಷರ *n;

        ಪ್ರತಿಜ್ಞಾ(s);

        n = strpbrk(s, "/.");

        /* If the first separator is a slash, the path is
         * assumed to be normalized and slashes remain slashes
         * and dots remains dots. */

        ಆದರೆ (n && *n == '.')
                /* Dots become slashes and slashes become dots. Fun. */
                ಮಾಡು {
                        ಆದರೆ (*n == '.')
                                *n = '/';
                        ಇಲ್ಲದಿದ್ದರೆ
                                *n = '.';

                        n = strpbrk(n + 1, "/.");
                } ಆಗ (n);

        ಮಾರ್ಗ_simplify(s);

        /* Kill the leading slash, but keep the first character of the string in the same place. */
        ಆದರೆ (s[0] == '/' && s[1] != 0)
                memmove(s, s+1, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(s));

        ಹಿಂತಿರುಗಿ s;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ shadow_update(Hashmap **shadow, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *property, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ) {
        _cleanup_free_ ಅಕ್ಷರ *k = ಖಾಲಿ, *v = ಖಾಲಿ, *cur_k = ಖಾಲಿ, *cur_v = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(property);
        ಪ್ರತಿಜ್ಞಾ(ಮೌಲ್ಯ);

        ಆದರೆ (!shadow)
                ಹಿಂತಿರುಗಿ 0;

        k = strdup(property);
        ಆದರೆ (!k)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        v = strdup(ಮೌಲ್ಯ);
        ಆದರೆ (!v)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        cur_v = hashmap_remove2(*shadow, k, (ಶೂನ್ಯ**)&cur_k);

        r = hashmap_ensure_put(shadow, &ಮಾರ್ಗ_hash_ops_free_free, k, v);
        ಆದರೆ (r < 0) {
                ಪ್ರತಿಜ್ಞಾ(r != -EEXIST);

                ಹಿಂತಿರುಗಿ r;
        }

        TAKE_PTR(k);
        TAKE_PTR(v);

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ sysctl_write_ತುಂಬಿದ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *property, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ, Hashmap **shadow) {
        ಅಕ್ಷರ *p;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(property);
        ಪ್ರತಿಜ್ಞಾ(ಮೌಲ್ಯ);

        p = strjoina("/proc/sys/", property);

        ಮಾರ್ಗ_simplify(p);
        ಆದರೆ (!ಮಾರ್ಗ_is_normalized(p))
                ಹಿಂತಿರುಗಿ -EINVAL;

        log_debug("Setting '%s' to '%s'", p, ಮೌಲ್ಯ);

        r = shadow_update(shadow, p, ಮೌಲ್ಯ);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ write_string_file(p, ಮೌಲ್ಯ, WRITE_STRING_FILE_VERIFY_ON_FAILURE | WRITE_STRING_FILE_DISABLE_ಬಫರ್ | WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL);
}

ಪೂರ್ಣಾಂಕ sysctl_writef(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *property, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *format, ...) {
        _cleanup_free_ ಅಕ್ಷರ *v = ಖಾಲಿ;
        va_list ap;
        ಪೂರ್ಣಾಂಕ r;

        va_start(ap, format);
        r = vasprintf(&v, format, ap);
        va_end(ap);

        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        ಹಿಂತಿರುಗಿ sysctl_write(property, v);
}

ಪೂರ್ಣಾಂಕ sysctl_write_verify(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *property, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(property);
        ಪ್ರತಿಜ್ಞಾ(ಮೌಲ್ಯ);

        /* Some sysctl settings accept invalid values on write, but refuses on read. E.g. coredump pattern,
         * be1e0283021ec73c2eb92839db9a471a068709d9 (v6.17) and 7d7c1fb85cba5627bbe741fb7539c709435e3848
         * (v6.16.8), which is fixed by a779e27f24aeb679969ddd1fdd7f636e22ddbc1e (v6.18) and
         * 304aa560385720baf3660fe8500f6dd425b63ea9 (v6.17.5). Let's first save the original value, and
         * restore to the saved value if the new value is refused. */

        _cleanup_free_ ಅಕ್ಷರ *original = ಖಾಲಿ;
        r = sysctl_read(property, &original);
        ಆದರೆ (r >= 0 && streq(original, ಮೌಲ್ಯ))
                ಹಿಂತಿರುಗಿ 0; /* Already set. */

        r = sysctl_write(property, ಮೌಲ್ಯ);
        ಆದರೆ (r >= 0) {
                _cleanup_free_ ಅಕ್ಷರ *current = ಖಾಲಿ;
                r = sysctl_read(property, &current);
                ಆದರೆ (r >= 0) {
                        ಆದರೆ (streq(current, ಮೌಲ್ಯ))
                                ಹಿಂತಿರುಗಿ 0; /* Yay! */

                        /* At least for coredump pattern, this does not happen, but let's handle this as the
                         * same as we wrote something invalid. */
                        r = -EINVAL;
                }
        }

        ಆದರೆ (original)
                (ಶೂನ್ಯ) sysctl_write(property, original);

        ಹಿಂತಿರುಗಿ r;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* af_ಗಮ್ಯ_sysctl_dir(ಪೂರ್ಣಾಂಕ af) {
        ಆದರೆ (af == AF_MPLS)
                ಹಿಂತಿರುಗಿ "mpls";

        ಹಿಂತಿರುಗಿ af_ಗಮ್ಯ_ipv4_ipv6(af);
}

ಪೂರ್ಣಾಂಕ sysctl_write_ip_property(ಪೂರ್ಣಾಂಕ af, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ifname, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *property, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ, Hashmap **shadow) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p;

        ಪ್ರತಿಜ್ಞಾ(property);
        ಪ್ರತಿಜ್ಞಾ(ಮೌಲ್ಯ);

        ಆದರೆ (!ಒಳ_ಗಣ(af, AF_INET, AF_INET6, AF_MPLS))
                ಹಿಂತಿರುಗಿ -EAFNOSUPPORT;

        ಆದರೆ (ifname) {
                ಆದರೆ (!ifname_valid_ತುಂಬಿದ(ifname, IFNAME_VALID_SPECIAL))
                        ಹಿಂತಿರುಗಿ -EINVAL;

                p = strjoina("net/", af_ಗಮ್ಯ_sysctl_dir(af), "/conf/", ifname, "/", property);
        } ಇಲ್ಲದಿದ್ದರೆ
                p = strjoina("net/", af_ಗಮ್ಯ_sysctl_dir(af), "/", property);

        ಹಿಂತಿರುಗಿ sysctl_write_ತುಂಬಿದ(p, ಮೌಲ್ಯ, shadow);
}

ಪೂರ್ಣಾಂಕ sysctl_write_ip_property_boolean(ಪೂರ್ಣಾಂಕ af, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ifname, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *property, bool ಮೌಲ್ಯ, Hashmap **shadow) {
        ಹಿಂತಿರುಗಿ sysctl_write_ip_property(af, ifname, property, one_zero(ಮೌಲ್ಯ), shadow);
}

ಪೂರ್ಣಾಂಕ sysctl_write_ip_neighbor_property(ಪೂರ್ಣಾಂಕ af, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ifname, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *property, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ, Hashmap **shadow) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p;

        ಪ್ರತಿಜ್ಞಾ(property);
        ಪ್ರತಿಜ್ಞಾ(ಮೌಲ್ಯ);
        ಪ್ರತಿಜ್ಞಾ(ifname);

        ಆದರೆ (!ಒಳ_ಗಣ(af, AF_INET, AF_INET6))
                ಹಿಂತಿರುಗಿ -EAFNOSUPPORT;

        ಆದರೆ (ifname) {
                ಆದರೆ (!ifname_valid_ತುಂಬಿದ(ifname, IFNAME_VALID_SPECIAL))
                        ಹಿಂತಿರುಗಿ -EINVAL;
                p = strjoina("net/", af_ಗಮ್ಯ_ipv4_ipv6(af), "/neigh/", ifname, "/", property);
        } ಇಲ್ಲದಿದ್ದರೆ
                p = strjoina("net/", af_ಗಮ್ಯ_ipv4_ipv6(af), "/neigh/default/", property);

        ಹಿಂತಿರುಗಿ sysctl_write_ತುಂಬಿದ(p, ಮೌಲ್ಯ, shadow);
}

ಪೂರ್ಣಾಂಕ sysctl_write_ip_neighbor_property_uint32(ಪೂರ್ಣಾಂಕ af, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ifname, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *property, uint32_t ಮೌಲ್ಯ, Hashmap **shadow) {
        ಅಕ್ಷರ buf[DECIMAL_STR_MAX(uint32_t)];
        xsprintf(buf, "%u", ಮೌಲ್ಯ);
        ಹಿಂತಿರುಗಿ sysctl_write_ip_neighbor_property(af, ifname, property, buf, shadow);
}

ಪೂರ್ಣಾಂಕ sysctl_read(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *property, ಅಕ್ಷರ **ret) {
        ಅಕ್ಷರ *p;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(property);

        p = strjoina("/proc/sys/", property);

        ಮಾರ್ಗ_simplify(p);
        ಆದರೆ (!ಮಾರ್ಗ_is_normalized(p)) /* Filter out attempts to write to /proc/sys/../../…, just in case */
                ಹಿಂತಿರುಗಿ -EINVAL;

        r = read_ತುಂಬಿದ_virtual_file(p, ret, ಖಾಲಿ);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;
        ಆದರೆ (ret)
                delete_trailing_chars(*ret, NEWLINE);

        ಹಿಂತಿರುಗಿ r;
}

ಪೂರ್ಣಾಂಕ sysctl_read_ip_property(ಪೂರ್ಣಾಂಕ af, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ifname, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *property, ಅಕ್ಷರ **ret) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p;

        ಪ್ರತಿಜ್ಞಾ(property);

        ಆದರೆ (!ಒಳ_ಗಣ(af, AF_INET, AF_INET6, AF_MPLS))
                ಹಿಂತಿರುಗಿ -EAFNOSUPPORT;

        ಆದರೆ (ifname) {
                ಆದರೆ (!ifname_valid_ತುಂಬಿದ(ifname, IFNAME_VALID_SPECIAL))
                        ಹಿಂತಿರುಗಿ -EINVAL;

                p = strjoina("net/", af_ಗಮ್ಯ_sysctl_dir(af), "/conf/", ifname, "/", property);
        } ಇಲ್ಲದಿದ್ದರೆ
                p = strjoina("net/", af_ಗಮ್ಯ_sysctl_dir(af), "/", property);

        ಹಿಂತಿರುಗಿ sysctl_read(p, ret);
}

ಪೂರ್ಣಾಂಕ sysctl_read_ip_property_int(ಪೂರ್ಣಾಂಕ af, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ifname, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *property, ಪೂರ್ಣಾಂಕ *ret) {
        _cleanup_free_ ಅಕ್ಷರ *s = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        r = sysctl_read_ip_property(af, ifname, property, &s);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ ಸುರಕ್ಷಿತ_atoi(s, ret);
}

ಪೂರ್ಣಾಂಕ sysctl_read_ip_property_uint32(ಪೂರ್ಣಾಂಕ af, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ifname, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *property, uint32_t *ret) {
        _cleanup_free_ ಅಕ್ಷರ *s = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        r = sysctl_read_ip_property(af, ifname, property, &s);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ ಸುರಕ್ಷಿತ_atou32(s, ret);
}
