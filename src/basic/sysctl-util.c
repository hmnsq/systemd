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

        إذا (n && *n == '.')
                /* Dots become slashes and slashes become dots. Fun. */
                افعل {
                        إذا (*n == '.')
                                *n = '/';
                        وإلا
                                *n = '.';

                        n = strpbrk(n + 1, "/.");
                } بينما (n);

        path_simplify(s);

        /* Kill the leading slash, but keep the first character of the string in the same place. */
        إذا (s[0] == '/' && s[1] != 0)
                memmove(s, s+1, strlen(s));

        إرجاع s;
}

ساكن صحيح shadow_update(Hashmap **shadow, ثابت حرف *property, ثابت حرف *قيمة) {
        _cleanup_free_ حرف *k = عدم, *v = عدم, *cur_k = عدم, *cur_v = عدم;
        صحيح r;

        أكد(property);
        أكد(قيمة);

        إذا (!shadow)
                إرجاع 0;

        k = strdup(property);
        إذا (!k)
                إرجاع -ENOMEM;

        v = strdup(قيمة);
        إذا (!v)
                إرجاع -ENOMEM;

        cur_v = hashmap_remove2(*shadow, k, (فراغ**)&cur_k);

        r = hashmap_ensure_put(shadow, &path_hash_ops_free_free, k, v);
        إذا (r < 0) {
                أكد(r != -EEXIST);

                إرجاع r;
        }

        TAKE_PTR(k);
        TAKE_PTR(v);

        إرجاع 0;
}

صحيح sysctl_write_full(ثابت حرف *property, ثابت حرف *قيمة, Hashmap **shadow) {
        حرف *p;
        صحيح r;

        أكد(property);
        أكد(قيمة);

        p = strjoina("/proc/sys/", property);

        path_simplify(p);
        إذا (!path_is_normalized(p))
                إرجاع -EINVAL;

        log_debug("Setting '%s' to '%s'", p, قيمة);

        r = shadow_update(shadow, p, قيمة);
        إذا (r < 0)
                إرجاع r;

        إرجاع write_string_file(p, قيمة, WRITE_STRING_FILE_VERIFY_ON_FAILURE | WRITE_STRING_FILE_DISABLE_BUFFER | WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL);
}

صحيح sysctl_writef(ثابت حرف *property, ثابت حرف *format, ...) {
        _cleanup_free_ حرف *v = عدم;
        va_list ap;
        صحيح r;

        va_start(ap, format);
        r = vasprintf(&v, format, ap);
        va_end(ap);

        إذا (r < 0)
                إرجاع -ENOMEM;

        إرجاع sysctl_write(property, v);
}

صحيح sysctl_write_verify(ثابت حرف *property, ثابت حرف *قيمة) {
        صحيح r;

        أكد(property);
        أكد(قيمة);

        /* Some sysctl settings accept invalid values on write, but refuses on read. E.g. coredump pattern,
         * be1e0283021ec73c2eb92839db9a471a068709d9 (v6.17) and 7d7c1fb85cba5627bbe741fb7539c709435e3848
         * (v6.16.8), which is fixed by a779e27f24aeb679969ddd1fdd7f636e22ddbc1e (v6.18) and
         * 304aa560385720baf3660fe8500f6dd425b63ea9 (v6.17.5). Let's first save the original value, and
         * restore to the saved value if the new value is refused. */

        _cleanup_free_ حرف *original = عدم;
        r = sysctl_read(property, &original);
        إذا (r >= 0 && streq(original, قيمة))
                إرجاع 0; /* Already set. */

        r = sysctl_write(property, قيمة);
        إذا (r >= 0) {
                _cleanup_free_ حرف *current = عدم;
                r = sysctl_read(property, &current);
                إذا (r >= 0) {
                        إذا (streq(current, قيمة))
                                إرجاع 0; /* Yay! */

                        /* At least for coredump pattern, this does not happen, but let's handle this as the
                         * same as we wrote something invalid. */
                        r = -EINVAL;
                }
        }

        إذا (original)
                (فراغ) sysctl_write(property, original);

        إرجاع r;
}

ساكن ثابت حرف* af_to_sysctl_dir(صحيح af) {
        إذا (af == AF_MPLS)
                إرجاع "mpls";

        إرجاع af_to_ipv4_ipv6(af);
}

صحيح sysctl_write_ip_property(صحيح af, ثابت حرف *ifname, ثابت حرف *property, ثابت حرف *قيمة, Hashmap **shadow) {
        ثابت حرف *p;

        أكد(property);
        أكد(قيمة);

        إذا (!IN_SET(af, AF_INET, AF_INET6, AF_MPLS))
                إرجاع -EAFNOSUPPORT;

        إذا (ifname) {
                إذا (!ifname_valid_full(ifname, IFNAME_VALID_SPECIAL))
                        إرجاع -EINVAL;

                p = strjoina("net/", af_to_sysctl_dir(af), "/conf/", ifname, "/", property);
        } وإلا
                p = strjoina("net/", af_to_sysctl_dir(af), "/", property);

        إرجاع sysctl_write_full(p, قيمة, shadow);
}

صحيح sysctl_write_ip_property_boolean(صحيح af, ثابت حرف *ifname, ثابت حرف *property, bool قيمة, Hashmap **shadow) {
        إرجاع sysctl_write_ip_property(af, ifname, property, one_zero(قيمة), shadow);
}

صحيح sysctl_write_ip_neighbor_property(صحيح af, ثابت حرف *ifname, ثابت حرف *property, ثابت حرف *قيمة, Hashmap **shadow) {
        ثابت حرف *p;

        أكد(property);
        أكد(قيمة);
        أكد(ifname);

        إذا (!IN_SET(af, AF_INET, AF_INET6))
                إرجاع -EAFNOSUPPORT;

        إذا (ifname) {
                إذا (!ifname_valid_full(ifname, IFNAME_VALID_SPECIAL))
                        إرجاع -EINVAL;
                p = strjoina("net/", af_to_ipv4_ipv6(af), "/neigh/", ifname, "/", property);
        } وإلا
                p = strjoina("net/", af_to_ipv4_ipv6(af), "/neigh/default/", property);

        إرجاع sysctl_write_full(p, قيمة, shadow);
}

صحيح sysctl_write_ip_neighbor_property_uint32(صحيح af, ثابت حرف *ifname, ثابت حرف *property, uint32_t قيمة, Hashmap **shadow) {
        حرف buf[DECIMAL_STR_MAX(uint32_t)];
        xsprintf(buf, "%u", قيمة);
        إرجاع sysctl_write_ip_neighbor_property(af, ifname, property, buf, shadow);
}

صحيح sysctl_read(ثابت حرف *property, حرف **ret) {
        حرف *p;
        صحيح r;

        أكد(property);

        p = strjoina("/proc/sys/", property);

        path_simplify(p);
        إذا (!path_is_normalized(p)) /* Filter out attempts to write to /proc/sys/../../…, just in case */
                إرجاع -EINVAL;

        r = read_full_virtual_file(p, ret, عدم);
        إذا (r < 0)
                إرجاع r;
        إذا (ret)
                delete_trailing_chars(*ret, NEWLINE);

        إرجاع r;
}

صحيح sysctl_read_ip_property(صحيح af, ثابت حرف *ifname, ثابت حرف *property, حرف **ret) {
        ثابت حرف *p;

        أكد(property);

        إذا (!IN_SET(af, AF_INET, AF_INET6, AF_MPLS))
                إرجاع -EAFNOSUPPORT;

        إذا (ifname) {
                إذا (!ifname_valid_full(ifname, IFNAME_VALID_SPECIAL))
                        إرجاع -EINVAL;

                p = strjoina("net/", af_to_sysctl_dir(af), "/conf/", ifname, "/", property);
        } وإلا
                p = strjoina("net/", af_to_sysctl_dir(af), "/", property);

        إرجاع sysctl_read(p, ret);
}

صحيح sysctl_read_ip_property_int(صحيح af, ثابت حرف *ifname, ثابت حرف *property, صحيح *ret) {
        _cleanup_free_ حرف *s = عدم;
        صحيح r;

        أكد(ret);

        r = sysctl_read_ip_property(af, ifname, property, &s);
        إذا (r < 0)
                إرجاع r;

        إرجاع safe_atoi(s, ret);
}

صحيح sysctl_read_ip_property_uint32(صحيح af, ثابت حرف *ifname, ثابت حرف *property, uint32_t *ret) {
        _cleanup_free_ حرف *s = عدم;
        صحيح r;

        أكد(ret);

        r = sysctl_read_ip_property(af, ifname, property, &s);
        إذا (r < 0)
                إرجاع r;

        إرجاع safe_atou32(s, ret);
}
