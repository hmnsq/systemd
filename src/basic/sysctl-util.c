/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <stdio.h>

#شامل "af-list.h"
#شامل "alloc-util.h"
#شامل "fileio.h"
#شامل "hashmap.h"
#شامل "log.h"
#شامل "parse-util.h"
#شامل "path-util.h"
#شامل "socket-util.h"
#شامل "string-util.h"
#شامل "sysctl-util.h"

حرف* sysctl_normalize(حرف *s) {
        حرف *n;

        أكد(s);

        n = strpbrk(s, "/.");

        /* If the first separator is a slash, the path is
         * assumed to be normalized and slashes remain slashes
         * and dots remains dots. */

        اگر (n && *n == '.')
                /* Dots become slashes and slashes become dots. Fun. */
                کرو {
                        اگر (*n == '.')
                                *n = '/';
                        ورنہ
                                *n = '.';

                        n = strpbrk(n + 1, "/.");
                } جب (n);

        راستہ_simplify(s);

        /* Kill the leading slash, but keep the first character of the string in the same place. */
        اگر (s[0] == '/' && s[1] != 0)
                memmove(s, s+1, لڑی_لمبائی(s));

        واپس s;
}

ساکن صحیح shadow_update(Hashmap **shadow, ثابت حرف *property, ثابت حرف *قدر) {
        _cleanup_free_ حرف *k = تہی, *v = تہی, *cur_k = تہی, *cur_v = تہی;
        صحیح r;

        أكد(property);
        أكد(قدر);

        اگر (!shadow)
                واپس 0;

        k = strdup(property);
        اگر (!k)
                واپس -ENOMEM;

        v = strdup(قدر);
        اگر (!v)
                واپس -ENOMEM;

        cur_v = hashmap_remove2(*shadow, k, (خالی**)&cur_k);

        r = hashmap_ensure_put(shadow, &راستہ_hash_ops_free_free, k, v);
        اگر (r < 0) {
                أكد(r != -EEXIST);

                واپس r;
        }

        TAKE_PTR(k);
        TAKE_PTR(v);

        واپس 0;
}

صحیح sysctl_write_بھرا(ثابت حرف *property, ثابت حرف *قدر, Hashmap **shadow) {
        حرف *p;
        صحیح r;

        أكد(property);
        أكد(قدر);

        p = strjoina("/proc/sys/", property);

        راستہ_simplify(p);
        اگر (!راستہ_is_normalized(p))
                واپس -EINVAL;

        log_debug("Setting '%s' to '%s'", p, قدر);

        r = shadow_update(shadow, p, قدر);
        اگر (r < 0)
                واپس r;

        واپس write_string_file(p, قدر, WRITE_STRING_FILE_VERIFY_ON_FAILURE | WRITE_STRING_FILE_DISABLE_بفر | WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL);
}

صحیح sysctl_writef(ثابت حرف *property, ثابت حرف *format, ...) {
        _cleanup_free_ حرف *v = تہی;
        va_list ap;
        صحیح r;

        va_start(ap, format);
        r = vasprintf(&v, format, ap);
        va_end(ap);

        اگر (r < 0)
                واپس -ENOMEM;

        واپس sysctl_write(property, v);
}

صحیح sysctl_write_verify(ثابت حرف *property, ثابت حرف *قدر) {
        صحیح r;

        أكد(property);
        أكد(قدر);

        /* Some sysctl settings accept invalid values on write, but refuses on read. E.g. coredump pattern,
         * be1e0283021ec73c2eb92839db9a471a068709d9 (v6.17) and 7d7c1fb85cba5627bbe741fb7539c709435e3848
         * (v6.16.8), which is fixed by a779e27f24aeb679969ddd1fdd7f636e22ddbc1e (v6.18) and
         * 304aa560385720baf3660fe8500f6dd425b63ea9 (v6.17.5). Let's first save the original value, and
         * restore to the saved value if the new value is refused. */

        _cleanup_free_ حرف *original = تہی;
        r = sysctl_read(property, &original);
        اگر (r >= 0 && streq(original, قدر))
                واپس 0; /* Already set. */

        r = sysctl_write(property, قدر);
        اگر (r >= 0) {
                _cleanup_free_ حرف *current = تہی;
                r = sysctl_read(property, &current);
                اگر (r >= 0) {
                        اگر (streq(current, قدر))
                                واپس 0; /* Yay! */

                        /* At least for coredump pattern, this does not happen, but let's handle this as the
                         * same as we wrote something invalid. */
                        r = -EINVAL;
                }
        }

        اگر (original)
                (خالی) sysctl_write(property, original);

        واپس r;
}

ساکن ثابت حرف* af_مقصد_sysctl_dir(صحیح af) {
        اگر (af == AF_MPLS)
                واپس "mpls";

        واپس af_مقصد_ipv4_ipv6(af);
}

صحیح sysctl_write_ip_property(صحیح af, ثابت حرف *ifname, ثابت حرف *property, ثابت حرف *قدر, Hashmap **shadow) {
        ثابت حرف *p;

        أكد(property);
        أكد(قدر);

        اگر (!اندر_سیٹ(af, AF_INET, AF_INET6, AF_MPLS))
                واپس -EAFNOSUPPORT;

        اگر (ifname) {
                اگر (!ifname_valid_بھرا(ifname, IFNAME_VALID_SPECIAL))
                        واپس -EINVAL;

                p = strjoina("net/", af_مقصد_sysctl_dir(af), "/conf/", ifname, "/", property);
        } ورنہ
                p = strjoina("net/", af_مقصد_sysctl_dir(af), "/", property);

        واپس sysctl_write_بھرا(p, قدر, shadow);
}

صحیح sysctl_write_ip_property_boolean(صحیح af, ثابت حرف *ifname, ثابت حرف *property, bool قدر, Hashmap **shadow) {
        واپس sysctl_write_ip_property(af, ifname, property, one_zero(قدر), shadow);
}

صحیح sysctl_write_ip_neighbor_property(صحیح af, ثابت حرف *ifname, ثابت حرف *property, ثابت حرف *قدر, Hashmap **shadow) {
        ثابت حرف *p;

        أكد(property);
        أكد(قدر);
        أكد(ifname);

        اگر (!اندر_سیٹ(af, AF_INET, AF_INET6))
                واپس -EAFNOSUPPORT;

        اگر (ifname) {
                اگر (!ifname_valid_بھرا(ifname, IFNAME_VALID_SPECIAL))
                        واپس -EINVAL;
                p = strjoina("net/", af_مقصد_ipv4_ipv6(af), "/neigh/", ifname, "/", property);
        } ورنہ
                p = strjoina("net/", af_مقصد_ipv4_ipv6(af), "/neigh/default/", property);

        واپس sysctl_write_بھرا(p, قدر, shadow);
}

صحیح sysctl_write_ip_neighbor_property_uint32(صحیح af, ثابت حرف *ifname, ثابت حرف *property, uint32_t قدر, Hashmap **shadow) {
        حرف buf[DECIMAL_STR_MAX(uint32_t)];
        xsprintf(buf, "%u", قدر);
        واپس sysctl_write_ip_neighbor_property(af, ifname, property, buf, shadow);
}

صحیح sysctl_read(ثابت حرف *property, حرف **ret) {
        حرف *p;
        صحیح r;

        أكد(property);

        p = strjoina("/proc/sys/", property);

        راستہ_simplify(p);
        اگر (!راستہ_is_normalized(p)) /* Filter out attempts to write to /proc/sys/../../…, just in case */
                واپس -EINVAL;

        r = read_بھرا_virtual_file(p, ret, تہی);
        اگر (r < 0)
                واپس r;
        اگر (ret)
                delete_trailing_chars(*ret, NEWLINE);

        واپس r;
}

صحیح sysctl_read_ip_property(صحیح af, ثابت حرف *ifname, ثابت حرف *property, حرف **ret) {
        ثابت حرف *p;

        أكد(property);

        اگر (!اندر_سیٹ(af, AF_INET, AF_INET6, AF_MPLS))
                واپس -EAFNOSUPPORT;

        اگر (ifname) {
                اگر (!ifname_valid_بھرا(ifname, IFNAME_VALID_SPECIAL))
                        واپس -EINVAL;

                p = strjoina("net/", af_مقصد_sysctl_dir(af), "/conf/", ifname, "/", property);
        } ورنہ
                p = strjoina("net/", af_مقصد_sysctl_dir(af), "/", property);

        واپس sysctl_read(p, ret);
}

صحیح sysctl_read_ip_property_int(صحیح af, ثابت حرف *ifname, ثابت حرف *property, صحیح *ret) {
        _cleanup_free_ حرف *s = تہی;
        صحیح r;

        أكد(ret);

        r = sysctl_read_ip_property(af, ifname, property, &s);
        اگر (r < 0)
                واپس r;

        واپس محفوظ_atoi(s, ret);
}

صحیح sysctl_read_ip_property_uint32(صحیح af, ثابت حرف *ifname, ثابت حرف *property, uint32_t *ret) {
        _cleanup_free_ حرف *s = تہی;
        صحیح r;

        أكد(ret);

        r = sysctl_read_ip_property(af, ifname, property, &s);
        اگر (r < 0)
                واپس r;

        واپس محفوظ_atou32(s, ret);
}
