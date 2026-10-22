/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <stdio.h>

#ضمن "af-list.h"
#ضمن "alloc-util.h"
#ضمن "fileio.h"
#ضمن "hashmap.h"
#ضمن "log.h"
#ضمن "parse-util.h"
#ضمن "path-util.h"
#ضمن "socket-util.h"
#ضمن "string-util.h"
#ضمن "sysctl-util.h"

حرف* sysctl_normalize(حرف *s) {
        حرف *n;

        أكد(s);

        n = strpbrk(s, "/.");

        /* If the first separator is a slash, the path is
         * assumed to be normalized and slashes remain slashes
         * and dots remains dots. */

        ܐܢ (n && *n == '.')
                /* Dots become slashes and slashes become dots. Fun. */
                ܥܒܕ {
                        ܐܢ (*n == '.')
                                *n = '/';
                        ܐܚܪܢܐ
                                *n = '.';

                        n = strpbrk(n + 1, "/.");
                } ܟܡܐ (n);

        path_simplify(s);

        /* Kill the leading slash, but keep the first character of the string in the same place. */
        ܐܢ (s[0] == '/' && s[1] != 0)
                memmove(s, s+1, strlen(s));

        ܦܢܐ s;
}

ساكن صحيح shadow_update(Hashmap **shadow, ثابت حرف *property, ثابت حرف *value) {
        _cleanup_free_ حرف *k = عدم, *v = عدم, *cur_k = عدم, *cur_v = عدم;
        صحيح r;

        أكد(property);
        أكد(value);

        ܐܢ (!shadow)
                ܦܢܐ 0;

        k = strdup(property);
        ܐܢ (!k)
                ܦܢܐ -ENOMEM;

        v = strdup(value);
        ܐܢ (!v)
                ܦܢܐ -ENOMEM;

        cur_v = hashmap_remove2(*shadow, k, (فراغ**)&cur_k);

        r = hashmap_ensure_put(shadow, &path_hash_ops_free_free, k, v);
        ܐܢ (r < 0) {
                أكد(r != -EEXIST);

                ܦܢܐ r;
        }

        TAKE_PTR(k);
        TAKE_PTR(v);

        ܦܢܐ 0;
}

صحيح sysctl_write_full(ثابت حرف *property, ثابت حرف *value, Hashmap **shadow) {
        حرف *p;
        صحيح r;

        أكد(property);
        أكد(value);

        p = strjoina("/proc/sys/", property);

        path_simplify(p);
        ܐܢ (!path_is_normalized(p))
                ܦܢܐ -EINVAL;

        log_debug("Setting '%s' to '%s'", p, value);

        r = shadow_update(shadow, p, value);
        ܐܢ (r < 0)
                ܦܢܐ r;

        ܦܢܐ write_string_file(p, value, WRITE_STRING_FILE_VERIFY_ON_FAILURE | WRITE_STRING_FILE_DISABLE_BUFFER | WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL);
}

صحيح sysctl_writef(ثابت حرف *property, ثابت حرف *format, ...) {
        _cleanup_free_ حرف *v = عدم;
        va_list ap;
        صحيح r;

        va_start(ap, format);
        r = vasprintf(&v, format, ap);
        va_end(ap);

        ܐܢ (r < 0)
                ܦܢܐ -ENOMEM;

        ܦܢܐ sysctl_write(property, v);
}

صحيح sysctl_write_verify(ثابت حرف *property, ثابت حرف *value) {
        صحيح r;

        أكد(property);
        أكد(value);

        /* Some sysctl settings accept invalid values on write, but refuses on read. E.g. coredump pattern,
         * be1e0283021ec73c2eb92839db9a471a068709d9 (v6.17) and 7d7c1fb85cba5627bbe741fb7539c709435e3848
         * (v6.16.8), which is fixed by a779e27f24aeb679969ddd1fdd7f636e22ddbc1e (v6.18) and
         * 304aa560385720baf3660fe8500f6dd425b63ea9 (v6.17.5). Let's first save the original value, and
         * restore to the saved value if the new value is refused. */

        _cleanup_free_ حرف *original = عدم;
        r = sysctl_read(property, &original);
        ܐܢ (r >= 0 && streq(original, value))
                ܦܢܐ 0; /* Already set. */

        r = sysctl_write(property, value);
        ܐܢ (r >= 0) {
                _cleanup_free_ حرف *current = عدم;
                r = sysctl_read(property, &current);
                ܐܢ (r >= 0) {
                        ܐܢ (streq(current, value))
                                ܦܢܐ 0; /* Yay! */

                        /* At least for coredump pattern, this does not happen, but let's handle this as the
                         * same as we wrote something invalid. */
                        r = -EINVAL;
                }
        }

        ܐܢ (original)
                (فراغ) sysctl_write(property, original);

        ܦܢܐ r;
}

ساكن ثابت حرف* af_to_sysctl_dir(صحيح af) {
        ܐܢ (af == AF_MPLS)
                ܦܢܐ "mpls";

        ܦܢܐ af_to_ipv4_ipv6(af);
}

صحيح sysctl_write_ip_property(صحيح af, ثابت حرف *ifname, ثابت حرف *property, ثابت حرف *value, Hashmap **shadow) {
        ثابت حرف *p;

        أكد(property);
        أكد(value);

        ܐܢ (!IN_SET(af, AF_INET, AF_INET6, AF_MPLS))
                ܦܢܐ -EAFNOSUPPORT;

        ܐܢ (ifname) {
                ܐܢ (!ifname_valid_full(ifname, IFNAME_VALID_SPECIAL))
                        ܦܢܐ -EINVAL;

                p = strjoina("net/", af_to_sysctl_dir(af), "/conf/", ifname, "/", property);
        } ܐܚܪܢܐ
                p = strjoina("net/", af_to_sysctl_dir(af), "/", property);

        ܦܢܐ sysctl_write_full(p, value, shadow);
}

صحيح sysctl_write_ip_property_boolean(صحيح af, ثابت حرف *ifname, ثابت حرف *property, bool value, Hashmap **shadow) {
        ܦܢܐ sysctl_write_ip_property(af, ifname, property, one_zero(value), shadow);
}

صحيح sysctl_write_ip_neighbor_property(صحيح af, ثابت حرف *ifname, ثابت حرف *property, ثابت حرف *value, Hashmap **shadow) {
        ثابت حرف *p;

        أكد(property);
        أكد(value);
        أكد(ifname);

        ܐܢ (!IN_SET(af, AF_INET, AF_INET6))
                ܦܢܐ -EAFNOSUPPORT;

        ܐܢ (ifname) {
                ܐܢ (!ifname_valid_full(ifname, IFNAME_VALID_SPECIAL))
                        ܦܢܐ -EINVAL;
                p = strjoina("net/", af_to_ipv4_ipv6(af), "/neigh/", ifname, "/", property);
        } ܐܚܪܢܐ
                p = strjoina("net/", af_to_ipv4_ipv6(af), "/neigh/default/", property);

        ܦܢܐ sysctl_write_full(p, value, shadow);
}

صحيح sysctl_write_ip_neighbor_property_uint32(صحيح af, ثابت حرف *ifname, ثابت حرف *property, uint32_t value, Hashmap **shadow) {
        حرف buf[DECIMAL_STR_MAX(uint32_t)];
        xsprintf(buf, "%u", value);
        ܦܢܐ sysctl_write_ip_neighbor_property(af, ifname, property, buf, shadow);
}

صحيح sysctl_read(ثابت حرف *property, حرف **ret) {
        حرف *p;
        صحيح r;

        أكد(property);

        p = strjoina("/proc/sys/", property);

        path_simplify(p);
        ܐܢ (!path_is_normalized(p)) /* Filter out attempts to write to /proc/sys/../../…, just in case */
                ܦܢܐ -EINVAL;

        r = read_full_virtual_file(p, ret, عدم);
        ܐܢ (r < 0)
                ܦܢܐ r;
        ܐܢ (ret)
                delete_trailing_chars(*ret, NEWLINE);

        ܦܢܐ r;
}

صحيح sysctl_read_ip_property(صحيح af, ثابت حرف *ifname, ثابت حرف *property, حرف **ret) {
        ثابت حرف *p;

        أكد(property);

        ܐܢ (!IN_SET(af, AF_INET, AF_INET6, AF_MPLS))
                ܦܢܐ -EAFNOSUPPORT;

        ܐܢ (ifname) {
                ܐܢ (!ifname_valid_full(ifname, IFNAME_VALID_SPECIAL))
                        ܦܢܐ -EINVAL;

                p = strjoina("net/", af_to_sysctl_dir(af), "/conf/", ifname, "/", property);
        } ܐܚܪܢܐ
                p = strjoina("net/", af_to_sysctl_dir(af), "/", property);

        ܦܢܐ sysctl_read(p, ret);
}

صحيح sysctl_read_ip_property_int(صحيح af, ثابت حرف *ifname, ثابت حرف *property, صحيح *ret) {
        _cleanup_free_ حرف *s = عدم;
        صحيح r;

        أكد(ret);

        r = sysctl_read_ip_property(af, ifname, property, &s);
        ܐܢ (r < 0)
                ܦܢܐ r;

        ܦܢܐ safe_atoi(s, ret);
}

صحيح sysctl_read_ip_property_uint32(صحيح af, ثابت حرف *ifname, ثابت حرف *property, uint32_t *ret) {
        _cleanup_free_ حرف *s = عدم;
        صحيح r;

        أكد(ret);

        r = sysctl_read_ip_property(af, ifname, property, &s);
        ܐܢ (r < 0)
                ܦܢܐ r;

        ܦܢܐ safe_atou32(s, ret);
}
