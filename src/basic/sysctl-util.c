/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ <stdio.h>

#અન્તર્ભાવ "af-list.h"
#અન્તર્ભાવ "alloc-util.h"
#અન્તર્ભાવ "fileio.h"
#અન્તર્ભાવ "hashmap.h"
#અન્તર્ભાવ "log.h"
#અન્તર્ભાવ "parse-util.h"
#અન્તર્ભાવ "path-util.h"
#અન્તર્ભાવ "socket-util.h"
#અન્તર્ભાવ "string-util.h"
#અન્તર્ભાવ "sysctl-util.h"

અક્ષર* sysctl_normalize(અક્ષર *s) {
        અક્ષર *n;

        પ્રતિજ્ઞા(s);

        n = strpbrk(s, "/.");

        /* If the first separator is a slash, the path is
         * assumed to be normalized and slashes remain slashes
         * and dots remains dots. */

        જો (n && *n == '.')
                /* Dots become slashes and slashes become dots. Fun. */
                કરો {
                        જો (*n == '.')
                                *n = '/';
                        નહીંતર
                                *n = '.';

                        n = strpbrk(n + 1, "/.");
                } જ્યારે (n);

        path_simplify(s);

        /* Kill the leading slash, but keep the first character of the string in the same place. */
        જો (s[0] == '/' && s[1] != 0)
                memmove(s, s+1, strlen(s));

        પાછા s;
}

સ્થિર પૂર્ણાંક shadow_update(Hashmap **shadow, અચલ અક્ષર *property, અચલ અક્ષર *કિંમત) {
        _cleanup_free_ અક્ષર *k = ખાલી, *v = ખાલી, *cur_k = ખાલી, *cur_v = ખાલી;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(property);
        પ્રતિજ્ઞા(કિંમત);

        જો (!shadow)
                પાછા 0;

        k = strdup(property);
        જો (!k)
                પાછા -ENOMEM;

        v = strdup(કિંમત);
        જો (!v)
                પાછા -ENOMEM;

        cur_v = hashmap_remove2(*shadow, k, (શૂન્ય**)&cur_k);

        r = hashmap_ensure_put(shadow, &path_hash_ops_free_free, k, v);
        જો (r < 0) {
                પ્રતિજ્ઞા(r != -EEXIST);

                પાછા r;
        }

        TAKE_PTR(k);
        TAKE_PTR(v);

        પાછા 0;
}

પૂર્ણાંક sysctl_write_full(અચલ અક્ષર *property, અચલ અક્ષર *કિંમત, Hashmap **shadow) {
        અક્ષર *p;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(property);
        પ્રતિજ્ઞા(કિંમત);

        p = strjoina("/proc/sys/", property);

        path_simplify(p);
        જો (!path_is_normalized(p))
                પાછા -EINVAL;

        log_debug("Setting '%s' to '%s'", p, કિંમત);

        r = shadow_update(shadow, p, કિંમત);
        જો (r < 0)
                પાછા r;

        પાછા write_string_file(p, કિંમત, WRITE_STRING_FILE_VERIFY_ON_FAILURE | WRITE_STRING_FILE_DISABLE_BUFFER | WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL);
}

પૂર્ણાંક sysctl_writef(અચલ અક્ષર *property, અચલ અક્ષર *format, ...) {
        _cleanup_free_ અક્ષર *v = ખાલી;
        va_list ap;
        પૂર્ણાંક r;

        va_start(ap, format);
        r = vasprintf(&v, format, ap);
        va_end(ap);

        જો (r < 0)
                પાછા -ENOMEM;

        પાછા sysctl_write(property, v);
}

પૂર્ણાંક sysctl_write_verify(અચલ અક્ષર *property, અચલ અક્ષર *કિંમત) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(property);
        પ્રતિજ્ઞા(કિંમત);

        /* Some sysctl settings accept invalid values on write, but refuses on read. E.g. coredump pattern,
         * be1e0283021ec73c2eb92839db9a471a068709d9 (v6.17) and 7d7c1fb85cba5627bbe741fb7539c709435e3848
         * (v6.16.8), which is fixed by a779e27f24aeb679969ddd1fdd7f636e22ddbc1e (v6.18) and
         * 304aa560385720baf3660fe8500f6dd425b63ea9 (v6.17.5). Let's first save the original value, and
         * restore to the saved value if the new value is refused. */

        _cleanup_free_ અક્ષર *original = ખાલી;
        r = sysctl_read(property, &original);
        જો (r >= 0 && streq(original, કિંમત))
                પાછા 0; /* Already set. */

        r = sysctl_write(property, કિંમત);
        જો (r >= 0) {
                _cleanup_free_ અક્ષર *current = ખાલી;
                r = sysctl_read(property, &current);
                જો (r >= 0) {
                        જો (streq(current, કિંમત))
                                પાછા 0; /* Yay! */

                        /* At least for coredump pattern, this does not happen, but let's handle this as the
                         * same as we wrote something invalid. */
                        r = -EINVAL;
                }
        }

        જો (original)
                (શૂન્ય) sysctl_write(property, original);

        પાછા r;
}

સ્થિર અચલ અક્ષર* af_to_sysctl_dir(પૂર્ણાંક af) {
        જો (af == AF_MPLS)
                પાછા "mpls";

        પાછા af_to_ipv4_ipv6(af);
}

પૂર્ણાંક sysctl_write_ip_property(પૂર્ણાંક af, અચલ અક્ષર *ifname, અચલ અક્ષર *property, અચલ અક્ષર *કિંમત, Hashmap **shadow) {
        અચલ અક્ષર *p;

        પ્રતિજ્ઞા(property);
        પ્રતિજ્ઞા(કિંમત);

        જો (!IN_SET(af, AF_INET, AF_INET6, AF_MPLS))
                પાછા -EAFNOSUPPORT;

        જો (ifname) {
                જો (!ifname_valid_full(ifname, IFNAME_VALID_SPECIAL))
                        પાછા -EINVAL;

                p = strjoina("net/", af_to_sysctl_dir(af), "/conf/", ifname, "/", property);
        } નહીંતર
                p = strjoina("net/", af_to_sysctl_dir(af), "/", property);

        પાછા sysctl_write_full(p, કિંમત, shadow);
}

પૂર્ણાંક sysctl_write_ip_property_boolean(પૂર્ણાંક af, અચલ અક્ષર *ifname, અચલ અક્ષર *property, bool કિંમત, Hashmap **shadow) {
        પાછા sysctl_write_ip_property(af, ifname, property, one_zero(કિંમત), shadow);
}

પૂર્ણાંક sysctl_write_ip_neighbor_property(પૂર્ણાંક af, અચલ અક્ષર *ifname, અચલ અક્ષર *property, અચલ અક્ષર *કિંમત, Hashmap **shadow) {
        અચલ અક્ષર *p;

        પ્રતિજ્ઞા(property);
        પ્રતિજ્ઞા(કિંમત);
        પ્રતિજ્ઞા(ifname);

        જો (!IN_SET(af, AF_INET, AF_INET6))
                પાછા -EAFNOSUPPORT;

        જો (ifname) {
                જો (!ifname_valid_full(ifname, IFNAME_VALID_SPECIAL))
                        પાછા -EINVAL;
                p = strjoina("net/", af_to_ipv4_ipv6(af), "/neigh/", ifname, "/", property);
        } નહીંતર
                p = strjoina("net/", af_to_ipv4_ipv6(af), "/neigh/default/", property);

        પાછા sysctl_write_full(p, કિંમત, shadow);
}

પૂર્ણાંક sysctl_write_ip_neighbor_property_uint32(પૂર્ણાંક af, અચલ અક્ષર *ifname, અચલ અક્ષર *property, uint32_t કિંમત, Hashmap **shadow) {
        અક્ષર buf[DECIMAL_STR_MAX(uint32_t)];
        xsprintf(buf, "%u", કિંમત);
        પાછા sysctl_write_ip_neighbor_property(af, ifname, property, buf, shadow);
}

પૂર્ણાંક sysctl_read(અચલ અક્ષર *property, અક્ષર **ret) {
        અક્ષર *p;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(property);

        p = strjoina("/proc/sys/", property);

        path_simplify(p);
        જો (!path_is_normalized(p)) /* Filter out attempts to write to /proc/sys/../../…, just in case */
                પાછા -EINVAL;

        r = read_full_virtual_file(p, ret, ખાલી);
        જો (r < 0)
                પાછા r;
        જો (ret)
                delete_trailing_chars(*ret, NEWLINE);

        પાછા r;
}

પૂર્ણાંક sysctl_read_ip_property(પૂર્ણાંક af, અચલ અક્ષર *ifname, અચલ અક્ષર *property, અક્ષર **ret) {
        અચલ અક્ષર *p;

        પ્રતિજ્ઞા(property);

        જો (!IN_SET(af, AF_INET, AF_INET6, AF_MPLS))
                પાછા -EAFNOSUPPORT;

        જો (ifname) {
                જો (!ifname_valid_full(ifname, IFNAME_VALID_SPECIAL))
                        પાછા -EINVAL;

                p = strjoina("net/", af_to_sysctl_dir(af), "/conf/", ifname, "/", property);
        } નહીંતર
                p = strjoina("net/", af_to_sysctl_dir(af), "/", property);

        પાછા sysctl_read(p, ret);
}

પૂર્ણાંક sysctl_read_ip_property_int(પૂર્ણાંક af, અચલ અક્ષર *ifname, અચલ અક્ષર *property, પૂર્ણાંક *ret) {
        _cleanup_free_ અક્ષર *s = ખાલી;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(ret);

        r = sysctl_read_ip_property(af, ifname, property, &s);
        જો (r < 0)
                પાછા r;

        પાછા safe_atoi(s, ret);
}

પૂર્ણાંક sysctl_read_ip_property_uint32(પૂર્ણાંક af, અચલ અક્ષર *ifname, અચલ અક્ષર *property, uint32_t *ret) {
        _cleanup_free_ અક્ષર *s = ખાલી;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(ret);

        r = sysctl_read_ip_property(af, ifname, property, &s);
        જો (r < 0)
                પાછા r;

        પાછા safe_atou32(s, ret);
}
