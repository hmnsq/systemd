/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ <stdio.h>

#ଅନ୍ତର୍ଭାଵ "af-list.h"
#ଅନ୍ତର୍ଭାଵ "alloc-util.h"
#ଅନ୍ତର୍ଭାଵ "fileio.h"
#ଅନ୍ତର୍ଭାଵ "hashmap.h"
#ଅନ୍ତର୍ଭାଵ "log.h"
#ଅନ୍ତର୍ଭାଵ "parse-util.h"
#ଅନ୍ତର୍ଭାଵ "path-util.h"
#ଅନ୍ତର୍ଭାଵ "socket-util.h"
#ଅନ୍ତର୍ଭାଵ "string-util.h"
#ଅନ୍ତର୍ଭାଵ "sysctl-util.h"

ଅକ୍ଷର* sysctl_normalize(ଅକ୍ଷର *s) {
        ଅକ୍ଷର *n;

        ପ୍ରତିଜ୍ଞା(s);

        n = strpbrk(s, "/.");

        /* If the first separator is a slash, the path is
         * assumed to be normalized and slashes remain slashes
         * and dots remains dots. */

        ଯଦି (n && *n == '.')
                /* Dots become slashes and slashes become dots. Fun. */
                କର {
                        ଯଦି (*n == '.')
                                *n = '/';
                        ଅନ୍ୟଥା
                                *n = '.';

                        n = strpbrk(n + 1, "/.");
                } ଯେତେବେଳେ (n);

        path_simplify(s);

        /* Kill the leading slash, but keep the first character of the string in the same place. */
        ଯଦି (s[0] == '/' && s[1] != 0)
                memmove(s, s+1, strlen(s));

        ଫେରନ୍ତୁ s;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା shadow_update(Hashmap **shadow, ଅଚଳ ଅକ୍ଷର *property, ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ) {
        _cleanup_free_ ଅକ୍ଷର *k = ଖାଲି, *v = ଖାଲି, *cur_k = ଖାଲି, *cur_v = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(property);
        ପ୍ରତିଜ୍ଞା(ମୂଲ୍ୟ);

        ଯଦି (!shadow)
                ଫେରନ୍ତୁ 0;

        k = strdup(property);
        ଯଦି (!k)
                ଫେରନ୍ତୁ -ENOMEM;

        v = strdup(ମୂଲ୍ୟ);
        ଯଦି (!v)
                ଫେରନ୍ତୁ -ENOMEM;

        cur_v = hashmap_remove2(*shadow, k, (ଶୂନ୍ୟ**)&cur_k);

        r = hashmap_ensure_put(shadow, &path_hash_ops_free_free, k, v);
        ଯଦି (r < 0) {
                ପ୍ରତିଜ୍ଞା(r != -EEXIST);

                ଫେରନ୍ତୁ r;
        }

        TAKE_PTR(k);
        TAKE_PTR(v);

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sysctl_write_full(ଅଚଳ ଅକ୍ଷର *property, ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ, Hashmap **shadow) {
        ଅକ୍ଷର *p;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(property);
        ପ୍ରତିଜ୍ଞା(ମୂଲ୍ୟ);

        p = strjoina("/proc/sys/", property);

        path_simplify(p);
        ଯଦି (!path_is_normalized(p))
                ଫେରନ୍ତୁ -EINVAL;

        log_debug("Setting '%s' to '%s'", p, ମୂଲ୍ୟ);

        r = shadow_update(shadow, p, ମୂଲ୍ୟ);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ write_string_file(p, ମୂଲ୍ୟ, WRITE_STRING_FILE_VERIFY_ON_FAILURE | WRITE_STRING_FILE_DISABLE_BUFFER | WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sysctl_writef(ଅଚଳ ଅକ୍ଷର *property, ଅଚଳ ଅକ୍ଷର *format, ...) {
        _cleanup_free_ ଅକ୍ଷର *v = ଖାଲି;
        va_list ap;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        va_start(ap, format);
        r = vasprintf(&v, format, ap);
        va_end(ap);

        ଯଦି (r < 0)
                ଫେରନ୍ତୁ -ENOMEM;

        ଫେରନ୍ତୁ sysctl_write(property, v);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sysctl_write_verify(ଅଚଳ ଅକ୍ଷର *property, ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(property);
        ପ୍ରତିଜ୍ଞା(ମୂଲ୍ୟ);

        /* Some sysctl settings accept invalid values on write, but refuses on read. E.g. coredump pattern,
         * be1e0283021ec73c2eb92839db9a471a068709d9 (v6.17) and 7d7c1fb85cba5627bbe741fb7539c709435e3848
         * (v6.16.8), which is fixed by a779e27f24aeb679969ddd1fdd7f636e22ddbc1e (v6.18) and
         * 304aa560385720baf3660fe8500f6dd425b63ea9 (v6.17.5). Let's first save the original value, and
         * restore to the saved value if the new value is refused. */

        _cleanup_free_ ଅକ୍ଷର *original = ଖାଲି;
        r = sysctl_read(property, &original);
        ଯଦି (r >= 0 && streq(original, ମୂଲ୍ୟ))
                ଫେରନ୍ତୁ 0; /* Already set. */

        r = sysctl_write(property, ମୂଲ୍ୟ);
        ଯଦି (r >= 0) {
                _cleanup_free_ ଅକ୍ଷର *current = ଖାଲି;
                r = sysctl_read(property, &current);
                ଯଦି (r >= 0) {
                        ଯଦି (streq(current, ମୂଲ୍ୟ))
                                ଫେରନ୍ତୁ 0; /* Yay! */

                        /* At least for coredump pattern, this does not happen, but let's handle this as the
                         * same as we wrote something invalid. */
                        r = -EINVAL;
                }
        }

        ଯଦି (original)
                (ଶୂନ୍ୟ) sysctl_write(property, original);

        ଫେରନ୍ତୁ r;
}

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର* af_to_sysctl_dir(ପୂର୍ଣ୍ଣସଂଖ୍ୟା af) {
        ଯଦି (af == AF_MPLS)
                ଫେରନ୍ତୁ "mpls";

        ଫେରନ୍ତୁ af_to_ipv4_ipv6(af);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sysctl_write_ip_property(ପୂର୍ଣ୍ଣସଂଖ୍ୟା af, ଅଚଳ ଅକ୍ଷର *ifname, ଅଚଳ ଅକ୍ଷର *property, ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ, Hashmap **shadow) {
        ଅଚଳ ଅକ୍ଷର *p;

        ପ୍ରତିଜ୍ଞା(property);
        ପ୍ରତିଜ୍ଞା(ମୂଲ୍ୟ);

        ଯଦି (!IN_SET(af, AF_INET, AF_INET6, AF_MPLS))
                ଫେରନ୍ତୁ -EAFNOSUPPORT;

        ଯଦି (ifname) {
                ଯଦି (!ifname_valid_full(ifname, IFNAME_VALID_SPECIAL))
                        ଫେରନ୍ତୁ -EINVAL;

                p = strjoina("net/", af_to_sysctl_dir(af), "/conf/", ifname, "/", property);
        } ଅନ୍ୟଥା
                p = strjoina("net/", af_to_sysctl_dir(af), "/", property);

        ଫେରନ୍ତୁ sysctl_write_full(p, ମୂଲ୍ୟ, shadow);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sysctl_write_ip_property_boolean(ପୂର୍ଣ୍ଣସଂଖ୍ୟା af, ଅଚଳ ଅକ୍ଷର *ifname, ଅଚଳ ଅକ୍ଷର *property, bool ମୂଲ୍ୟ, Hashmap **shadow) {
        ଫେରନ୍ତୁ sysctl_write_ip_property(af, ifname, property, one_zero(ମୂଲ୍ୟ), shadow);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sysctl_write_ip_neighbor_property(ପୂର୍ଣ୍ଣସଂଖ୍ୟା af, ଅଚଳ ଅକ୍ଷର *ifname, ଅଚଳ ଅକ୍ଷର *property, ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ, Hashmap **shadow) {
        ଅଚଳ ଅକ୍ଷର *p;

        ପ୍ରତିଜ୍ଞା(property);
        ପ୍ରତିଜ୍ଞା(ମୂଲ୍ୟ);
        ପ୍ରତିଜ୍ଞା(ifname);

        ଯଦି (!IN_SET(af, AF_INET, AF_INET6))
                ଫେରନ୍ତୁ -EAFNOSUPPORT;

        ଯଦି (ifname) {
                ଯଦି (!ifname_valid_full(ifname, IFNAME_VALID_SPECIAL))
                        ଫେରନ୍ତୁ -EINVAL;
                p = strjoina("net/", af_to_ipv4_ipv6(af), "/neigh/", ifname, "/", property);
        } ଅନ୍ୟଥା
                p = strjoina("net/", af_to_ipv4_ipv6(af), "/neigh/default/", property);

        ଫେରନ୍ତୁ sysctl_write_full(p, ମୂଲ୍ୟ, shadow);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sysctl_write_ip_neighbor_property_uint32(ପୂର୍ଣ୍ଣସଂଖ୍ୟା af, ଅଚଳ ଅକ୍ଷର *ifname, ଅଚଳ ଅକ୍ଷର *property, uint32_t ମୂଲ୍ୟ, Hashmap **shadow) {
        ଅକ୍ଷର buf[DECIMAL_STR_MAX(uint32_t)];
        xsprintf(buf, "%u", ମୂଲ୍ୟ);
        ଫେରନ୍ତୁ sysctl_write_ip_neighbor_property(af, ifname, property, buf, shadow);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sysctl_read(ଅଚଳ ଅକ୍ଷର *property, ଅକ୍ଷର **ret) {
        ଅକ୍ଷର *p;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(property);

        p = strjoina("/proc/sys/", property);

        path_simplify(p);
        ଯଦି (!path_is_normalized(p)) /* Filter out attempts to write to /proc/sys/../../…, just in case */
                ଫେରନ୍ତୁ -EINVAL;

        r = read_full_virtual_file(p, ret, ଖାଲି);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;
        ଯଦି (ret)
                delete_trailing_chars(*ret, NEWLINE);

        ଫେରନ୍ତୁ r;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sysctl_read_ip_property(ପୂର୍ଣ୍ଣସଂଖ୍ୟା af, ଅଚଳ ଅକ୍ଷର *ifname, ଅଚଳ ଅକ୍ଷର *property, ଅକ୍ଷର **ret) {
        ଅଚଳ ଅକ୍ଷର *p;

        ପ୍ରତିଜ୍ଞା(property);

        ଯଦି (!IN_SET(af, AF_INET, AF_INET6, AF_MPLS))
                ଫେରନ୍ତୁ -EAFNOSUPPORT;

        ଯଦି (ifname) {
                ଯଦି (!ifname_valid_full(ifname, IFNAME_VALID_SPECIAL))
                        ଫେରନ୍ତୁ -EINVAL;

                p = strjoina("net/", af_to_sysctl_dir(af), "/conf/", ifname, "/", property);
        } ଅନ୍ୟଥା
                p = strjoina("net/", af_to_sysctl_dir(af), "/", property);

        ଫେରନ୍ତୁ sysctl_read(p, ret);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sysctl_read_ip_property_int(ପୂର୍ଣ୍ଣସଂଖ୍ୟା af, ଅଚଳ ଅକ୍ଷର *ifname, ଅଚଳ ଅକ୍ଷର *property, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ret) {
        _cleanup_free_ ଅକ୍ଷର *s = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(ret);

        r = sysctl_read_ip_property(af, ifname, property, &s);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ safe_atoi(s, ret);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sysctl_read_ip_property_uint32(ପୂର୍ଣ୍ଣସଂଖ୍ୟା af, ଅଚଳ ଅକ୍ଷର *ifname, ଅଚଳ ଅକ୍ଷର *property, uint32_t *ret) {
        _cleanup_free_ ଅକ୍ଷର *s = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(ret);

        r = sysctl_read_ip_property(af, ifname, property, &s);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ safe_atou32(s, ret);
}
