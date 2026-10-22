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

نویسه* sysctl_normalize(نویسه *s) {
        نویسه *n;

        أكد(s);

        n = strpbrk(s, "/.");

        /* If the first separator is a slash, the path is
         * assumed to be normalized and slashes remain slashes
         * and dots remains dots. */

        اگر (n && *n == '.')
                /* Dots become slashes and slashes become dots. Fun. */
                انجامبده {
                        اگر (*n == '.')
                                *n = '/';
                        وگرنه
                                *n = '.';

                        n = strpbrk(n + 1, "/.");
                } وقتی (n);

        path_simplify(s);

        /* Kill the leading slash, but keep the first character of the string in the same place. */
        اگر (s[0] == '/' && s[1] != 0)
                memmove(s, s+1, strlen(s));

        بازگشت s;
}

ایستا صحیح shadow_update(Hashmap **shadow, ثابت نویسه *property, ثابت نویسه *مقدار) {
        _cleanup_free_ نویسه *k = پوچ, *v = پوچ, *cur_k = پوچ, *cur_v = پوچ;
        صحیح r;

        أكد(property);
        أكد(مقدار);

        اگر (!shadow)
                بازگشت 0;

        k = strdup(property);
        اگر (!k)
                بازگشت -ENOMEM;

        v = strdup(مقدار);
        اگر (!v)
                بازگشت -ENOMEM;

        cur_v = hashmap_remove2(*shadow, k, (تهی**)&cur_k);

        r = hashmap_ensure_put(shadow, &path_hash_ops_free_free, k, v);
        اگر (r < 0) {
                أكد(r != -EEXIST);

                بازگشت r;
        }

        TAKE_PTR(k);
        TAKE_PTR(v);

        بازگشت 0;
}

صحیح sysctl_write_full(ثابت نویسه *property, ثابت نویسه *مقدار, Hashmap **shadow) {
        نویسه *p;
        صحیح r;

        أكد(property);
        أكد(مقدار);

        p = strjoina("/proc/sys/", property);

        path_simplify(p);
        اگر (!path_is_normalized(p))
                بازگشت -EINVAL;

        log_debug("Setting '%s' to '%s'", p, مقدار);

        r = shadow_update(shadow, p, مقدار);
        اگر (r < 0)
                بازگشت r;

        بازگشت write_string_file(p, مقدار, WRITE_STRING_FILE_VERIFY_ON_FAILURE | WRITE_STRING_FILE_DISABLE_BUFFER | WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL);
}

صحیح sysctl_writef(ثابت نویسه *property, ثابت نویسه *format, ...) {
        _cleanup_free_ نویسه *v = پوچ;
        va_list ap;
        صحیح r;

        va_start(ap, format);
        r = vasprintf(&v, format, ap);
        va_end(ap);

        اگر (r < 0)
                بازگشت -ENOMEM;

        بازگشت sysctl_write(property, v);
}

صحیح sysctl_write_verify(ثابت نویسه *property, ثابت نویسه *مقدار) {
        صحیح r;

        أكد(property);
        أكد(مقدار);

        /* Some sysctl settings accept invalid values on write, but refuses on read. E.g. coredump pattern,
         * be1e0283021ec73c2eb92839db9a471a068709d9 (v6.17) and 7d7c1fb85cba5627bbe741fb7539c709435e3848
         * (v6.16.8), which is fixed by a779e27f24aeb679969ddd1fdd7f636e22ddbc1e (v6.18) and
         * 304aa560385720baf3660fe8500f6dd425b63ea9 (v6.17.5). Let's first save the original value, and
         * restore to the saved value if the new value is refused. */

        _cleanup_free_ نویسه *original = پوچ;
        r = sysctl_read(property, &original);
        اگر (r >= 0 && streq(original, مقدار))
                بازگشت 0; /* Already set. */

        r = sysctl_write(property, مقدار);
        اگر (r >= 0) {
                _cleanup_free_ نویسه *current = پوچ;
                r = sysctl_read(property, &current);
                اگر (r >= 0) {
                        اگر (streq(current, مقدار))
                                بازگشت 0; /* Yay! */

                        /* At least for coredump pattern, this does not happen, but let's handle this as the
                         * same as we wrote something invalid. */
                        r = -EINVAL;
                }
        }

        اگر (original)
                (تهی) sysctl_write(property, original);

        بازگشت r;
}

ایستا ثابت نویسه* af_to_sysctl_dir(صحیح af) {
        اگر (af == AF_MPLS)
                بازگشت "mpls";

        بازگشت af_to_ipv4_ipv6(af);
}

صحیح sysctl_write_ip_property(صحیح af, ثابت نویسه *ifname, ثابت نویسه *property, ثابت نویسه *مقدار, Hashmap **shadow) {
        ثابت نویسه *p;

        أكد(property);
        أكد(مقدار);

        اگر (!IN_SET(af, AF_INET, AF_INET6, AF_MPLS))
                بازگشت -EAFNOSUPPORT;

        اگر (ifname) {
                اگر (!ifname_valid_full(ifname, IFNAME_VALID_SPECIAL))
                        بازگشت -EINVAL;

                p = strjoina("net/", af_to_sysctl_dir(af), "/conf/", ifname, "/", property);
        } وگرنه
                p = strjoina("net/", af_to_sysctl_dir(af), "/", property);

        بازگشت sysctl_write_full(p, مقدار, shadow);
}

صحیح sysctl_write_ip_property_boolean(صحیح af, ثابت نویسه *ifname, ثابت نویسه *property, bool مقدار, Hashmap **shadow) {
        بازگشت sysctl_write_ip_property(af, ifname, property, one_zero(مقدار), shadow);
}

صحیح sysctl_write_ip_neighbor_property(صحیح af, ثابت نویسه *ifname, ثابت نویسه *property, ثابت نویسه *مقدار, Hashmap **shadow) {
        ثابت نویسه *p;

        أكد(property);
        أكد(مقدار);
        أكد(ifname);

        اگر (!IN_SET(af, AF_INET, AF_INET6))
                بازگشت -EAFNOSUPPORT;

        اگر (ifname) {
                اگر (!ifname_valid_full(ifname, IFNAME_VALID_SPECIAL))
                        بازگشت -EINVAL;
                p = strjoina("net/", af_to_ipv4_ipv6(af), "/neigh/", ifname, "/", property);
        } وگرنه
                p = strjoina("net/", af_to_ipv4_ipv6(af), "/neigh/default/", property);

        بازگشت sysctl_write_full(p, مقدار, shadow);
}

صحیح sysctl_write_ip_neighbor_property_uint32(صحیح af, ثابت نویسه *ifname, ثابت نویسه *property, uint32_t مقدار, Hashmap **shadow) {
        نویسه buf[DECIMAL_STR_MAX(uint32_t)];
        xsprintf(buf, "%u", مقدار);
        بازگشت sysctl_write_ip_neighbor_property(af, ifname, property, buf, shadow);
}

صحیح sysctl_read(ثابت نویسه *property, نویسه **ret) {
        نویسه *p;
        صحیح r;

        أكد(property);

        p = strjoina("/proc/sys/", property);

        path_simplify(p);
        اگر (!path_is_normalized(p)) /* Filter out attempts to write to /proc/sys/../../…, just in case */
                بازگشت -EINVAL;

        r = read_full_virtual_file(p, ret, پوچ);
        اگر (r < 0)
                بازگشت r;
        اگر (ret)
                delete_trailing_chars(*ret, NEWLINE);

        بازگشت r;
}

صحیح sysctl_read_ip_property(صحیح af, ثابت نویسه *ifname, ثابت نویسه *property, نویسه **ret) {
        ثابت نویسه *p;

        أكد(property);

        اگر (!IN_SET(af, AF_INET, AF_INET6, AF_MPLS))
                بازگشت -EAFNOSUPPORT;

        اگر (ifname) {
                اگر (!ifname_valid_full(ifname, IFNAME_VALID_SPECIAL))
                        بازگشت -EINVAL;

                p = strjoina("net/", af_to_sysctl_dir(af), "/conf/", ifname, "/", property);
        } وگرنه
                p = strjoina("net/", af_to_sysctl_dir(af), "/", property);

        بازگشت sysctl_read(p, ret);
}

صحیح sysctl_read_ip_property_int(صحیح af, ثابت نویسه *ifname, ثابت نویسه *property, صحیح *ret) {
        _cleanup_free_ نویسه *s = پوچ;
        صحیح r;

        أكد(ret);

        r = sysctl_read_ip_property(af, ifname, property, &s);
        اگر (r < 0)
                بازگشت r;

        بازگشت safe_atoi(s, ret);
}

صحیح sysctl_read_ip_property_uint32(صحیح af, ثابت نویسه *ifname, ثابت نویسه *property, uint32_t *ret) {
        _cleanup_free_ نویسه *s = پوچ;
        صحیح r;

        أكد(ret);

        r = sysctl_read_ip_property(af, ifname, property, &s);
        اگر (r < 0)
                بازگشت r;

        بازگشت safe_atou32(s, ret);
}
