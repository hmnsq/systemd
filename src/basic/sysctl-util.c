/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <stdio.h>

#অন্তর্ভাব "af-list.h"
#অন্তর্ভাব "alloc-util.h"
#অন্তর্ভাব "fileio.h"
#অন্তর্ভাব "hashmap.h"
#অন্তর্ভাব "log.h"
#অন্তর্ভাব "parse-util.h"
#অন্তর্ভাব "path-util.h"
#অন্তর্ভাব "socket-util.h"
#অন্তর্ভাব "string-util.h"
#অন্তর্ভাব "sysctl-util.h"

বর্ণ* sysctl_normalize(বর্ণ *s) {
        বর্ণ *n;

        প্রতিজ্ঞা(s);

        n = strpbrk(s, "/.");

        /* If the first separator is a slash, the path is
         * assumed to be normalized and slashes remain slashes
         * and dots remains dots. */

        যদি (n && *n == '.')
                /* Dots become slashes and slashes become dots. Fun. */
                কুরু {
                        যদি (*n == '.')
                                *n = '/';
                        অন্যথা
                                *n = '.';

                        n = strpbrk(n + 1, "/.");
                } যাবত্ (n);

        path_simplify(s);

        /* Kill the leading slash, but keep the first character of the string in the same place. */
        যদি (s[0] == '/' && s[1] != 0)
                memmove(s, s+1, strlen(s));

        প্রত্যাগম s;
}

স্থির পূর্ণাঙ্ক shadow_update(Hashmap **shadow, ধ্রুব বর্ণ *property, ধ্রুব বর্ণ *মান) {
        _cleanup_free_ বর্ণ *k = রিক্ত, *v = রিক্ত, *cur_k = রিক্ত, *cur_v = রিক্ত;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(property);
        প্রতিজ্ঞা(মান);

        যদি (!shadow)
                প্রত্যাগম 0;

        k = strdup(property);
        যদি (!k)
                প্রত্যাগম -ENOMEM;

        v = strdup(মান);
        যদি (!v)
                প্রত্যাগম -ENOMEM;

        cur_v = hashmap_remove2(*shadow, k, (শূন্য**)&cur_k);

        r = hashmap_ensure_put(shadow, &path_hash_ops_free_free, k, v);
        যদি (r < 0) {
                প্রতিজ্ঞা(r != -EEXIST);

                প্রত্যাগম r;
        }

        TAKE_PTR(k);
        TAKE_PTR(v);

        প্রত্যাগম 0;
}

পূর্ণাঙ্ক sysctl_write_full(ধ্রুব বর্ণ *property, ধ্রুব বর্ণ *মান, Hashmap **shadow) {
        বর্ণ *p;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(property);
        প্রতিজ্ঞা(মান);

        p = strjoina("/proc/sys/", property);

        path_simplify(p);
        যদি (!path_is_normalized(p))
                প্রত্যাগম -EINVAL;

        log_debug("Setting '%s' to '%s'", p, মান);

        r = shadow_update(shadow, p, মান);
        যদি (r < 0)
                প্রত্যাগম r;

        প্রত্যাগম write_string_file(p, মান, WRITE_STRING_FILE_VERIFY_ON_FAILURE | WRITE_STRING_FILE_DISABLE_BUFFER | WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL);
}

পূর্ণাঙ্ক sysctl_writef(ধ্রুব বর্ণ *property, ধ্রুব বর্ণ *format, ...) {
        _cleanup_free_ বর্ণ *v = রিক্ত;
        va_list ap;
        পূর্ণাঙ্ক r;

        va_start(ap, format);
        r = vasprintf(&v, format, ap);
        va_end(ap);

        যদি (r < 0)
                প্রত্যাগম -ENOMEM;

        প্রত্যাগম sysctl_write(property, v);
}

পূর্ণাঙ্ক sysctl_write_verify(ধ্রুব বর্ণ *property, ধ্রুব বর্ণ *মান) {
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(property);
        প্রতিজ্ঞা(মান);

        /* Some sysctl settings accept invalid values on write, but refuses on read. E.g. coredump pattern,
         * be1e0283021ec73c2eb92839db9a471a068709d9 (v6.17) and 7d7c1fb85cba5627bbe741fb7539c709435e3848
         * (v6.16.8), which is fixed by a779e27f24aeb679969ddd1fdd7f636e22ddbc1e (v6.18) and
         * 304aa560385720baf3660fe8500f6dd425b63ea9 (v6.17.5). Let's first save the original value, and
         * restore to the saved value if the new value is refused. */

        _cleanup_free_ বর্ণ *original = রিক্ত;
        r = sysctl_read(property, &original);
        যদি (r >= 0 && streq(original, মান))
                প্রত্যাগম 0; /* Already set. */

        r = sysctl_write(property, মান);
        যদি (r >= 0) {
                _cleanup_free_ বর্ণ *current = রিক্ত;
                r = sysctl_read(property, &current);
                যদি (r >= 0) {
                        যদি (streq(current, মান))
                                প্রত্যাগম 0; /* Yay! */

                        /* At least for coredump pattern, this does not happen, but let's handle this as the
                         * same as we wrote something invalid. */
                        r = -EINVAL;
                }
        }

        যদি (original)
                (শূন্য) sysctl_write(property, original);

        প্রত্যাগম r;
}

স্থির ধ্রুব বর্ণ* af_to_sysctl_dir(পূর্ণাঙ্ক af) {
        যদি (af == AF_MPLS)
                প্রত্যাগম "mpls";

        প্রত্যাগম af_to_ipv4_ipv6(af);
}

পূর্ণাঙ্ক sysctl_write_ip_property(পূর্ণাঙ্ক af, ধ্রুব বর্ণ *ifname, ধ্রুব বর্ণ *property, ধ্রুব বর্ণ *মান, Hashmap **shadow) {
        ধ্রুব বর্ণ *p;

        প্রতিজ্ঞা(property);
        প্রতিজ্ঞা(মান);

        যদি (!IN_SET(af, AF_INET, AF_INET6, AF_MPLS))
                প্রত্যাগম -EAFNOSUPPORT;

        যদি (ifname) {
                যদি (!ifname_valid_full(ifname, IFNAME_VALID_SPECIAL))
                        প্রত্যাগম -EINVAL;

                p = strjoina("net/", af_to_sysctl_dir(af), "/conf/", ifname, "/", property);
        } অন্যথা
                p = strjoina("net/", af_to_sysctl_dir(af), "/", property);

        প্রত্যাগম sysctl_write_full(p, মান, shadow);
}

পূর্ণাঙ্ক sysctl_write_ip_property_boolean(পূর্ণাঙ্ক af, ধ্রুব বর্ণ *ifname, ধ্রুব বর্ণ *property, bool মান, Hashmap **shadow) {
        প্রত্যাগম sysctl_write_ip_property(af, ifname, property, one_zero(মান), shadow);
}

পূর্ণাঙ্ক sysctl_write_ip_neighbor_property(পূর্ণাঙ্ক af, ধ্রুব বর্ণ *ifname, ধ্রুব বর্ণ *property, ধ্রুব বর্ণ *মান, Hashmap **shadow) {
        ধ্রুব বর্ণ *p;

        প্রতিজ্ঞা(property);
        প্রতিজ্ঞা(মান);
        প্রতিজ্ঞা(ifname);

        যদি (!IN_SET(af, AF_INET, AF_INET6))
                প্রত্যাগম -EAFNOSUPPORT;

        যদি (ifname) {
                যদি (!ifname_valid_full(ifname, IFNAME_VALID_SPECIAL))
                        প্রত্যাগম -EINVAL;
                p = strjoina("net/", af_to_ipv4_ipv6(af), "/neigh/", ifname, "/", property);
        } অন্যথা
                p = strjoina("net/", af_to_ipv4_ipv6(af), "/neigh/default/", property);

        প্রত্যাগম sysctl_write_full(p, মান, shadow);
}

পূর্ণাঙ্ক sysctl_write_ip_neighbor_property_uint32(পূর্ণাঙ্ক af, ধ্রুব বর্ণ *ifname, ধ্রুব বর্ণ *property, uint32_t মান, Hashmap **shadow) {
        বর্ণ buf[DECIMAL_STR_MAX(uint32_t)];
        xsprintf(buf, "%u", মান);
        প্রত্যাগম sysctl_write_ip_neighbor_property(af, ifname, property, buf, shadow);
}

পূর্ণাঙ্ক sysctl_read(ধ্রুব বর্ণ *property, বর্ণ **ret) {
        বর্ণ *p;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(property);

        p = strjoina("/proc/sys/", property);

        path_simplify(p);
        যদি (!path_is_normalized(p)) /* Filter out attempts to write to /proc/sys/../../…, just in case */
                প্রত্যাগম -EINVAL;

        r = read_full_virtual_file(p, ret, রিক্ত);
        যদি (r < 0)
                প্রত্যাগম r;
        যদি (ret)
                delete_trailing_chars(*ret, NEWLINE);

        প্রত্যাগম r;
}

পূর্ণাঙ্ক sysctl_read_ip_property(পূর্ণাঙ্ক af, ধ্রুব বর্ণ *ifname, ধ্রুব বর্ণ *property, বর্ণ **ret) {
        ধ্রুব বর্ণ *p;

        প্রতিজ্ঞা(property);

        যদি (!IN_SET(af, AF_INET, AF_INET6, AF_MPLS))
                প্রত্যাগম -EAFNOSUPPORT;

        যদি (ifname) {
                যদি (!ifname_valid_full(ifname, IFNAME_VALID_SPECIAL))
                        প্রত্যাগম -EINVAL;

                p = strjoina("net/", af_to_sysctl_dir(af), "/conf/", ifname, "/", property);
        } অন্যথা
                p = strjoina("net/", af_to_sysctl_dir(af), "/", property);

        প্রত্যাগম sysctl_read(p, ret);
}

পূর্ণাঙ্ক sysctl_read_ip_property_int(পূর্ণাঙ্ক af, ধ্রুব বর্ণ *ifname, ধ্রুব বর্ণ *property, পূর্ণাঙ্ক *ret) {
        _cleanup_free_ বর্ণ *s = রিক্ত;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(ret);

        r = sysctl_read_ip_property(af, ifname, property, &s);
        যদি (r < 0)
                প্রত্যাগম r;

        প্রত্যাগম safe_atoi(s, ret);
}

পূর্ণাঙ্ক sysctl_read_ip_property_uint32(পূর্ণাঙ্ক af, ধ্রুব বর্ণ *ifname, ধ্রুব বর্ণ *property, uint32_t *ret) {
        _cleanup_free_ বর্ণ *s = রিক্ত;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(ret);

        r = sysctl_read_ip_property(af, ifname, property, &s);
        যদি (r < 0)
                প্রত্যাগম r;

        প্রত্যাগম safe_atou32(s, ret);
}
