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
                } طالما (n);

        رستہ_simplify(s);

        /* Kill the leading slash, but keep the first character of the string in the same place. */
        إذا (s[0] == '/' && s[1] != 0)
                memmove(s, s+1, لڑی_لمبائی(s));

        أرجع s;
}

ساكن صحيح shadow_update(Hashmap **shadow, ثابت حرف *property, ثابت حرف *قدر) {
        _cleanup_free_ حرف *k = عدم, *v = عدم, *cur_k = عدم, *cur_v = عدم;
        صحيح r;

        أكد(property);
        أكد(قدر);

        إذا (!shadow)
                أرجع 0;

        k = strdup(property);
        إذا (!k)
                أرجع -ENOMEM;

        v = strdup(قدر);
        إذا (!v)
                أرجع -ENOMEM;

        cur_v = hashmap_remove2(*shadow, k, (فراغ**)&cur_k);

        r = hashmap_ensure_put(shadow, &رستہ_hash_ops_free_free, k, v);
        إذا (r < 0) {
                أكد(r != -EEXIST);

                أرجع r;
        }

        TAKE_PTR(k);
        TAKE_PTR(v);

        أرجع 0;
}

صحيح sysctl_write_بھریا(ثابت حرف *property, ثابت حرف *قدر, Hashmap **shadow) {
        حرف *p;
        صحيح r;

        أكد(property);
        أكد(قدر);

        p = strjoina("/proc/sys/", property);

        رستہ_simplify(p);
        إذا (!رستہ_is_normalized(p))
                أرجع -EINVAL;

        log_debug("Setting '%s' to '%s'", p, قدر);

        r = shadow_update(shadow, p, قدر);
        إذا (r < 0)
                أرجع r;

        أرجع write_string_file(p, قدر, WRITE_STRING_FILE_VERIFY_ON_FAILURE | WRITE_STRING_FILE_DISABLE_بفر | WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL);
}

صحيح sysctl_writef(ثابت حرف *property, ثابت حرف *format, ...) {
        _cleanup_free_ حرف *v = عدم;
        va_list ap;
        صحيح r;

        va_start(ap, format);
        r = vasprintf(&v, format, ap);
        va_end(ap);

        إذا (r < 0)
                أرجع -ENOMEM;

        أرجع sysctl_write(property, v);
}

صحيح sysctl_write_verify(ثابت حرف *property, ثابت حرف *قدر) {
        صحيح r;

        أكد(property);
        أكد(قدر);

        /* Some sysctl settings accept invalid values on write, but refuses on read. E.g. coredump pattern,
         * be1e0283021ec73c2eb92839db9a471a068709d9 (v6.17) and 7d7c1fb85cba5627bbe741fb7539c709435e3848
         * (v6.16.8), which is fixed by a779e27f24aeb679969ddd1fdd7f636e22ddbc1e (v6.18) and
         * 304aa560385720baf3660fe8500f6dd425b63ea9 (v6.17.5). Let's first save the original value, and
         * restore to the saved value if the new value is refused. */

        _cleanup_free_ حرف *original = عدم;
        r = sysctl_read(property, &original);
        إذا (r >= 0 && streq(original, قدر))
                أرجع 0; /* Already set. */

        r = sysctl_write(property, قدر);
        إذا (r >= 0) {
                _cleanup_free_ حرف *current = عدم;
                r = sysctl_read(property, &current);
                إذا (r >= 0) {
                        إذا (streq(current, قدر))
                                أرجع 0; /* Yay! */

                        /* At least for coredump pattern, this does not happen, but let's handle this as the
                         * same as we wrote something invalid. */
                        r = -EINVAL;
                }
        }

        إذا (original)
                (فراغ) sysctl_write(property, original);

        أرجع r;
}

ساكن ثابت حرف* af_منزل_sysctl_dir(صحيح af) {
        إذا (af == AF_MPLS)
                أرجع "mpls";

        أرجع af_منزل_ipv4_ipv6(af);
}

صحيح sysctl_write_ip_property(صحيح af, ثابت حرف *ifname, ثابت حرف *property, ثابت حرف *قدر, Hashmap **shadow) {
        ثابت حرف *p;

        أكد(property);
        أكد(قدر);

        إذا (!وچ_سیٹ(af, AF_INET, AF_INET6, AF_MPLS))
                أرجع -EAFNOSUPPORT;

        إذا (ifname) {
                إذا (!ifname_valid_بھریا(ifname, IFNAME_VALID_SPECIAL))
                        أرجع -EINVAL;

                p = strjoina("net/", af_منزل_sysctl_dir(af), "/conf/", ifname, "/", property);
        } وإلا
                p = strjoina("net/", af_منزل_sysctl_dir(af), "/", property);

        أرجع sysctl_write_بھریا(p, قدر, shadow);
}

صحيح sysctl_write_ip_property_boolean(صحيح af, ثابت حرف *ifname, ثابت حرف *property, bool قدر, Hashmap **shadow) {
        أرجع sysctl_write_ip_property(af, ifname, property, one_zero(قدر), shadow);
}

صحيح sysctl_write_ip_neighbor_property(صحيح af, ثابت حرف *ifname, ثابت حرف *property, ثابت حرف *قدر, Hashmap **shadow) {
        ثابت حرف *p;

        أكد(property);
        أكد(قدر);
        أكد(ifname);

        إذا (!وچ_سیٹ(af, AF_INET, AF_INET6))
                أرجع -EAFNOSUPPORT;

        إذا (ifname) {
                إذا (!ifname_valid_بھریا(ifname, IFNAME_VALID_SPECIAL))
                        أرجع -EINVAL;
                p = strjoina("net/", af_منزل_ipv4_ipv6(af), "/neigh/", ifname, "/", property);
        } وإلا
                p = strjoina("net/", af_منزل_ipv4_ipv6(af), "/neigh/default/", property);

        أرجع sysctl_write_بھریا(p, قدر, shadow);
}

صحيح sysctl_write_ip_neighbor_property_uint32(صحيح af, ثابت حرف *ifname, ثابت حرف *property, uint32_t قدر, Hashmap **shadow) {
        حرف buf[DECIMAL_STR_MAX(uint32_t)];
        xsprintf(buf, "%u", قدر);
        أرجع sysctl_write_ip_neighbor_property(af, ifname, property, buf, shadow);
}

صحيح sysctl_read(ثابت حرف *property, حرف **ret) {
        حرف *p;
        صحيح r;

        أكد(property);

        p = strjoina("/proc/sys/", property);

        رستہ_simplify(p);
        إذا (!رستہ_is_normalized(p)) /* Filter out attempts to write to /proc/sys/../../…, just in case */
                أرجع -EINVAL;

        r = read_بھریا_virtual_file(p, ret, عدم);
        إذا (r < 0)
                أرجع r;
        إذا (ret)
                delete_trailing_chars(*ret, NEWLINE);

        أرجع r;
}

صحيح sysctl_read_ip_property(صحيح af, ثابت حرف *ifname, ثابت حرف *property, حرف **ret) {
        ثابت حرف *p;

        أكد(property);

        إذا (!وچ_سیٹ(af, AF_INET, AF_INET6, AF_MPLS))
                أرجع -EAFNOSUPPORT;

        إذا (ifname) {
                إذا (!ifname_valid_بھریا(ifname, IFNAME_VALID_SPECIAL))
                        أرجع -EINVAL;

                p = strjoina("net/", af_منزل_sysctl_dir(af), "/conf/", ifname, "/", property);
        } وإلا
                p = strjoina("net/", af_منزل_sysctl_dir(af), "/", property);

        أرجع sysctl_read(p, ret);
}

صحيح sysctl_read_ip_property_int(صحيح af, ثابت حرف *ifname, ثابت حرف *property, صحيح *ret) {
        _cleanup_free_ حرف *s = عدم;
        صحيح r;

        أكد(ret);

        r = sysctl_read_ip_property(af, ifname, property, &s);
        إذا (r < 0)
                أرجع r;

        أرجع محفوظ_atoi(s, ret);
}

صحيح sysctl_read_ip_property_uint32(صحيح af, ثابت حرف *ifname, ثابت حرف *property, uint32_t *ret) {
        _cleanup_free_ حرف *s = عدم;
        صحيح r;

        أكد(ret);

        r = sysctl_read_ip_property(af, ifname, property, &s);
        إذا (r < 0)
                أرجع r;

        أرجع محفوظ_atou32(s, ret);
}
