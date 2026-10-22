/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "af-list.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "alloc-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fileio.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hashmap.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "log.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "parse-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "path-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "socket-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sysctl-util.h"

ਅੱਖਰ* sysctl_normalize(ਅੱਖਰ *s) {
        ਅੱਖਰ *n;

        ਪ੍ਰਤਿਜ੍ਞਾ(s);

        n = strpbrk(s, "/.");

        /* If the first separator is a slash, the path is
         * assumed to be normalized and slashes remain slashes
         * and dots remains dots. */

        ਜੇ (n && *n == '.')
                /* Dots become slashes and slashes become dots. Fun. */
                ਕਰੋ {
                        ਜੇ (*n == '.')
                                *n = '/';
                        ਨਹੀਂਤਾਂ
                                *n = '.';

                        n = strpbrk(n + 1, "/.");
                } ਜਦੋਂ (n);

        ਰਸਤਾ_simplify(s);

        /* Kill the leading slash, but keep the first character of the string in the same place. */
        ਜੇ (s[0] == '/' && s[1] != 0)
                memmove(s, s+1, ਲੜੀ_ਲੰਬਾਈ(s));

        ਵਾਪਸ s;
}

ਸਥਿਰ ਪੂਰਨ shadow_update(Hashmap **shadow, ਅਚਲ ਅੱਖਰ *property, ਅਚਲ ਅੱਖਰ *ਮੁੱਲ) {
        _cleanup_free_ ਅੱਖਰ *k = ਨਿਰਾ, *v = ਨਿਰਾ, *cur_k = ਨਿਰਾ, *cur_v = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(property);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਮੁੱਲ);

        ਜੇ (!shadow)
                ਵਾਪਸ 0;

        k = strdup(property);
        ਜੇ (!k)
                ਵਾਪਸ -ENOMEM;

        v = strdup(ਮੁੱਲ);
        ਜੇ (!v)
                ਵਾਪਸ -ENOMEM;

        cur_v = hashmap_remove2(*shadow, k, (ਖਾਲੀ**)&cur_k);

        r = hashmap_ensure_put(shadow, &ਰਸਤਾ_hash_ops_free_free, k, v);
        ਜੇ (r < 0) {
                ਪ੍ਰਤਿਜ੍ਞਾ(r != -EEXIST);

                ਵਾਪਸ r;
        }

        TAKE_PTR(k);
        TAKE_PTR(v);

        ਵਾਪਸ 0;
}

ਪੂਰਨ sysctl_write_ਭਰਿਆ(ਅਚਲ ਅੱਖਰ *property, ਅਚਲ ਅੱਖਰ *ਮੁੱਲ, Hashmap **shadow) {
        ਅੱਖਰ *p;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(property);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਮੁੱਲ);

        p = strjoina("/proc/sys/", property);

        ਰਸਤਾ_simplify(p);
        ਜੇ (!ਰਸਤਾ_is_normalized(p))
                ਵਾਪਸ -EINVAL;

        log_debug("Setting '%s' to '%s'", p, ਮੁੱਲ);

        r = shadow_update(shadow, p, ਮੁੱਲ);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ write_string_file(p, ਮੁੱਲ, WRITE_STRING_FILE_VERIFY_ON_FAILURE | WRITE_STRING_FILE_DISABLE_ਬਫਰ | WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL);
}

ਪੂਰਨ sysctl_writef(ਅਚਲ ਅੱਖਰ *property, ਅਚਲ ਅੱਖਰ *format, ...) {
        _cleanup_free_ ਅੱਖਰ *v = ਨਿਰਾ;
        va_list ap;
        ਪੂਰਨ r;

        va_start(ap, format);
        r = vasprintf(&v, format, ap);
        va_end(ap);

        ਜੇ (r < 0)
                ਵਾਪਸ -ENOMEM;

        ਵਾਪਸ sysctl_write(property, v);
}

ਪੂਰਨ sysctl_write_verify(ਅਚਲ ਅੱਖਰ *property, ਅਚਲ ਅੱਖਰ *ਮੁੱਲ) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(property);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਮੁੱਲ);

        /* Some sysctl settings accept invalid values on write, but refuses on read. E.g. coredump pattern,
         * be1e0283021ec73c2eb92839db9a471a068709d9 (v6.17) and 7d7c1fb85cba5627bbe741fb7539c709435e3848
         * (v6.16.8), which is fixed by a779e27f24aeb679969ddd1fdd7f636e22ddbc1e (v6.18) and
         * 304aa560385720baf3660fe8500f6dd425b63ea9 (v6.17.5). Let's first save the original value, and
         * restore to the saved value if the new value is refused. */

        _cleanup_free_ ਅੱਖਰ *original = ਨਿਰਾ;
        r = sysctl_read(property, &original);
        ਜੇ (r >= 0 && streq(original, ਮੁੱਲ))
                ਵਾਪਸ 0; /* Already set. */

        r = sysctl_write(property, ਮੁੱਲ);
        ਜੇ (r >= 0) {
                _cleanup_free_ ਅੱਖਰ *current = ਨਿਰਾ;
                r = sysctl_read(property, &current);
                ਜੇ (r >= 0) {
                        ਜੇ (streq(current, ਮੁੱਲ))
                                ਵਾਪਸ 0; /* Yay! */

                        /* At least for coredump pattern, this does not happen, but let's handle this as the
                         * same as we wrote something invalid. */
                        r = -EINVAL;
                }
        }

        ਜੇ (original)
                (ਖਾਲੀ) sysctl_write(property, original);

        ਵਾਪਸ r;
}

ਸਥਿਰ ਅਚਲ ਅੱਖਰ* af_ਮੰਜ਼ਿਲ_sysctl_dir(ਪੂਰਨ af) {
        ਜੇ (af == AF_MPLS)
                ਵਾਪਸ "mpls";

        ਵਾਪਸ af_ਮੰਜ਼ਿਲ_ipv4_ipv6(af);
}

ਪੂਰਨ sysctl_write_ip_property(ਪੂਰਨ af, ਅਚਲ ਅੱਖਰ *ifname, ਅਚਲ ਅੱਖਰ *property, ਅਚਲ ਅੱਖਰ *ਮੁੱਲ, Hashmap **shadow) {
        ਅਚਲ ਅੱਖਰ *p;

        ਪ੍ਰਤਿਜ੍ਞਾ(property);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਮੁੱਲ);

        ਜੇ (!ਵਿੱਚ_ਸੈੱਟ(af, AF_INET, AF_INET6, AF_MPLS))
                ਵਾਪਸ -EAFNOSUPPORT;

        ਜੇ (ifname) {
                ਜੇ (!ifname_valid_ਭਰਿਆ(ifname, IFNAME_VALID_SPECIAL))
                        ਵਾਪਸ -EINVAL;

                p = strjoina("net/", af_ਮੰਜ਼ਿਲ_sysctl_dir(af), "/conf/", ifname, "/", property);
        } ਨਹੀਂਤਾਂ
                p = strjoina("net/", af_ਮੰਜ਼ਿਲ_sysctl_dir(af), "/", property);

        ਵਾਪਸ sysctl_write_ਭਰਿਆ(p, ਮੁੱਲ, shadow);
}

ਪੂਰਨ sysctl_write_ip_property_boolean(ਪੂਰਨ af, ਅਚਲ ਅੱਖਰ *ifname, ਅਚਲ ਅੱਖਰ *property, bool ਮੁੱਲ, Hashmap **shadow) {
        ਵਾਪਸ sysctl_write_ip_property(af, ifname, property, one_zero(ਮੁੱਲ), shadow);
}

ਪੂਰਨ sysctl_write_ip_neighbor_property(ਪੂਰਨ af, ਅਚਲ ਅੱਖਰ *ifname, ਅਚਲ ਅੱਖਰ *property, ਅਚਲ ਅੱਖਰ *ਮੁੱਲ, Hashmap **shadow) {
        ਅਚਲ ਅੱਖਰ *p;

        ਪ੍ਰਤਿਜ੍ਞਾ(property);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਮੁੱਲ);
        ਪ੍ਰਤਿਜ੍ਞਾ(ifname);

        ਜੇ (!ਵਿੱਚ_ਸੈੱਟ(af, AF_INET, AF_INET6))
                ਵਾਪਸ -EAFNOSUPPORT;

        ਜੇ (ifname) {
                ਜੇ (!ifname_valid_ਭਰਿਆ(ifname, IFNAME_VALID_SPECIAL))
                        ਵਾਪਸ -EINVAL;
                p = strjoina("net/", af_ਮੰਜ਼ਿਲ_ipv4_ipv6(af), "/neigh/", ifname, "/", property);
        } ਨਹੀਂਤਾਂ
                p = strjoina("net/", af_ਮੰਜ਼ਿਲ_ipv4_ipv6(af), "/neigh/default/", property);

        ਵਾਪਸ sysctl_write_ਭਰਿਆ(p, ਮੁੱਲ, shadow);
}

ਪੂਰਨ sysctl_write_ip_neighbor_property_uint32(ਪੂਰਨ af, ਅਚਲ ਅੱਖਰ *ifname, ਅਚਲ ਅੱਖਰ *property, uint32_t ਮੁੱਲ, Hashmap **shadow) {
        ਅੱਖਰ buf[DECIMAL_STR_MAX(uint32_t)];
        xsprintf(buf, "%u", ਮੁੱਲ);
        ਵਾਪਸ sysctl_write_ip_neighbor_property(af, ifname, property, buf, shadow);
}

ਪੂਰਨ sysctl_read(ਅਚਲ ਅੱਖਰ *property, ਅੱਖਰ **ret) {
        ਅੱਖਰ *p;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(property);

        p = strjoina("/proc/sys/", property);

        ਰਸਤਾ_simplify(p);
        ਜੇ (!ਰਸਤਾ_is_normalized(p)) /* Filter out attempts to write to /proc/sys/../../…, just in case */
                ਵਾਪਸ -EINVAL;

        r = read_ਭਰਿਆ_virtual_file(p, ret, ਨਿਰਾ);
        ਜੇ (r < 0)
                ਵਾਪਸ r;
        ਜੇ (ret)
                delete_trailing_chars(*ret, NEWLINE);

        ਵਾਪਸ r;
}

ਪੂਰਨ sysctl_read_ip_property(ਪੂਰਨ af, ਅਚਲ ਅੱਖਰ *ifname, ਅਚਲ ਅੱਖਰ *property, ਅੱਖਰ **ret) {
        ਅਚਲ ਅੱਖਰ *p;

        ਪ੍ਰਤਿਜ੍ਞਾ(property);

        ਜੇ (!ਵਿੱਚ_ਸੈੱਟ(af, AF_INET, AF_INET6, AF_MPLS))
                ਵਾਪਸ -EAFNOSUPPORT;

        ਜੇ (ifname) {
                ਜੇ (!ifname_valid_ਭਰਿਆ(ifname, IFNAME_VALID_SPECIAL))
                        ਵਾਪਸ -EINVAL;

                p = strjoina("net/", af_ਮੰਜ਼ਿਲ_sysctl_dir(af), "/conf/", ifname, "/", property);
        } ਨਹੀਂਤਾਂ
                p = strjoina("net/", af_ਮੰਜ਼ਿਲ_sysctl_dir(af), "/", property);

        ਵਾਪਸ sysctl_read(p, ret);
}

ਪੂਰਨ sysctl_read_ip_property_int(ਪੂਰਨ af, ਅਚਲ ਅੱਖਰ *ifname, ਅਚਲ ਅੱਖਰ *property, ਪੂਰਨ *ret) {
        _cleanup_free_ ਅੱਖਰ *s = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        r = sysctl_read_ip_property(af, ifname, property, &s);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ ਸੁਰੱਖਿਅਤ_atoi(s, ret);
}

ਪੂਰਨ sysctl_read_ip_property_uint32(ਪੂਰਨ af, ਅਚਲ ਅੱਖਰ *ifname, ਅਚਲ ਅੱਖਰ *property, uint32_t *ret) {
        _cleanup_free_ ਅੱਖਰ *s = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        r = sysctl_read_ip_property(af, ifname, property, &s);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ ਸੁਰੱਖਿਅਤ_atou32(s, ret);
}
