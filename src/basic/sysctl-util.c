/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ <stdio.h>

#அந்தர்பாவ "af-list.h"
#அந்தர்பாவ "alloc-util.h"
#அந்தர்பாவ "fileio.h"
#அந்தர்பாவ "hashmap.h"
#அந்தர்பாவ "log.h"
#அந்தர்பாவ "parse-util.h"
#அந்தர்பாவ "path-util.h"
#அந்தர்பாவ "socket-util.h"
#அந்தர்பாவ "string-util.h"
#அந்தர்பாவ "sysctl-util.h"

எழுத்து* sysctl_normalize(எழுத்து *s) {
        எழுத்து *n;

        ப்ரதிஜ்ஞா(s);

        n = strpbrk(s, "/.");

        /* If the first separator is a slash, the path is
         * assumed to be normalized and slashes remain slashes
         * and dots remains dots. */

        எனில் (n && *n == '.')
                /* Dots become slashes and slashes become dots. Fun. */
                செய் {
                        எனில் (*n == '.')
                                *n = '/';
                        இல்லையெனில்
                                *n = '.';

                        n = strpbrk(n + 1, "/.");
                } போது (n);

        path_simplify(s);

        /* Kill the leading slash, but keep the first character of the string in the same place. */
        எனில் (s[0] == '/' && s[1] != 0)
                memmove(s, s+1, strlen(s));

        திரும்பு s;
}

நிலையான முழு shadow_update(Hashmap **shadow, மாறிலி எழுத்து *property, மாறிலி எழுத்து *மதிப்பு) {
        _cleanup_free_ எழுத்து *k = வெற்று, *v = வெற்று, *cur_k = வெற்று, *cur_v = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(property);
        ப்ரதிஜ்ஞா(மதிப்பு);

        எனில் (!shadow)
                திரும்பு 0;

        k = strdup(property);
        எனில் (!k)
                திரும்பு -ENOMEM;

        v = strdup(மதிப்பு);
        எனில் (!v)
                திரும்பு -ENOMEM;

        cur_v = hashmap_remove2(*shadow, k, (வெறுமை**)&cur_k);

        r = hashmap_ensure_put(shadow, &path_hash_ops_free_free, k, v);
        எனில் (r < 0) {
                ப்ரதிஜ்ஞா(r != -EEXIST);

                திரும்பு r;
        }

        TAKE_PTR(k);
        TAKE_PTR(v);

        திரும்பு 0;
}

முழு sysctl_write_full(மாறிலி எழுத்து *property, மாறிலி எழுத்து *மதிப்பு, Hashmap **shadow) {
        எழுத்து *p;
        முழு r;

        ப்ரதிஜ்ஞா(property);
        ப்ரதிஜ்ஞா(மதிப்பு);

        p = strjoina("/proc/sys/", property);

        path_simplify(p);
        எனில் (!path_is_normalized(p))
                திரும்பு -EINVAL;

        log_debug("Setting '%s' to '%s'", p, மதிப்பு);

        r = shadow_update(shadow, p, மதிப்பு);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு write_string_file(p, மதிப்பு, WRITE_STRING_FILE_VERIFY_ON_FAILURE | WRITE_STRING_FILE_DISABLE_BUFFER | WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL);
}

முழு sysctl_writef(மாறிலி எழுத்து *property, மாறிலி எழுத்து *format, ...) {
        _cleanup_free_ எழுத்து *v = வெற்று;
        va_list ap;
        முழு r;

        va_start(ap, format);
        r = vasprintf(&v, format, ap);
        va_end(ap);

        எனில் (r < 0)
                திரும்பு -ENOMEM;

        திரும்பு sysctl_write(property, v);
}

முழு sysctl_write_verify(மாறிலி எழுத்து *property, மாறிலி எழுத்து *மதிப்பு) {
        முழு r;

        ப்ரதிஜ்ஞா(property);
        ப்ரதிஜ்ஞா(மதிப்பு);

        /* Some sysctl settings accept invalid values on write, but refuses on read. E.g. coredump pattern,
         * be1e0283021ec73c2eb92839db9a471a068709d9 (v6.17) and 7d7c1fb85cba5627bbe741fb7539c709435e3848
         * (v6.16.8), which is fixed by a779e27f24aeb679969ddd1fdd7f636e22ddbc1e (v6.18) and
         * 304aa560385720baf3660fe8500f6dd425b63ea9 (v6.17.5). Let's first save the original value, and
         * restore to the saved value if the new value is refused. */

        _cleanup_free_ எழுத்து *original = வெற்று;
        r = sysctl_read(property, &original);
        எனில் (r >= 0 && streq(original, மதிப்பு))
                திரும்பு 0; /* Already set. */

        r = sysctl_write(property, மதிப்பு);
        எனில் (r >= 0) {
                _cleanup_free_ எழுத்து *current = வெற்று;
                r = sysctl_read(property, &current);
                எனில் (r >= 0) {
                        எனில் (streq(current, மதிப்பு))
                                திரும்பு 0; /* Yay! */

                        /* At least for coredump pattern, this does not happen, but let's handle this as the
                         * same as we wrote something invalid. */
                        r = -EINVAL;
                }
        }

        எனில் (original)
                (வெறுமை) sysctl_write(property, original);

        திரும்பு r;
}

நிலையான மாறிலி எழுத்து* af_to_sysctl_dir(முழு af) {
        எனில் (af == AF_MPLS)
                திரும்பு "mpls";

        திரும்பு af_to_ipv4_ipv6(af);
}

முழு sysctl_write_ip_property(முழு af, மாறிலி எழுத்து *ifname, மாறிலி எழுத்து *property, மாறிலி எழுத்து *மதிப்பு, Hashmap **shadow) {
        மாறிலி எழுத்து *p;

        ப்ரதிஜ்ஞா(property);
        ப்ரதிஜ்ஞா(மதிப்பு);

        எனில் (!IN_SET(af, AF_INET, AF_INET6, AF_MPLS))
                திரும்பு -EAFNOSUPPORT;

        எனில் (ifname) {
                எனில் (!ifname_valid_full(ifname, IFNAME_VALID_SPECIAL))
                        திரும்பு -EINVAL;

                p = strjoina("net/", af_to_sysctl_dir(af), "/conf/", ifname, "/", property);
        } இல்லையெனில்
                p = strjoina("net/", af_to_sysctl_dir(af), "/", property);

        திரும்பு sysctl_write_full(p, மதிப்பு, shadow);
}

முழு sysctl_write_ip_property_boolean(முழு af, மாறிலி எழுத்து *ifname, மாறிலி எழுத்து *property, bool மதிப்பு, Hashmap **shadow) {
        திரும்பு sysctl_write_ip_property(af, ifname, property, one_zero(மதிப்பு), shadow);
}

முழு sysctl_write_ip_neighbor_property(முழு af, மாறிலி எழுத்து *ifname, மாறிலி எழுத்து *property, மாறிலி எழுத்து *மதிப்பு, Hashmap **shadow) {
        மாறிலி எழுத்து *p;

        ப்ரதிஜ்ஞா(property);
        ப்ரதிஜ்ஞா(மதிப்பு);
        ப்ரதிஜ்ஞா(ifname);

        எனில் (!IN_SET(af, AF_INET, AF_INET6))
                திரும்பு -EAFNOSUPPORT;

        எனில் (ifname) {
                எனில் (!ifname_valid_full(ifname, IFNAME_VALID_SPECIAL))
                        திரும்பு -EINVAL;
                p = strjoina("net/", af_to_ipv4_ipv6(af), "/neigh/", ifname, "/", property);
        } இல்லையெனில்
                p = strjoina("net/", af_to_ipv4_ipv6(af), "/neigh/default/", property);

        திரும்பு sysctl_write_full(p, மதிப்பு, shadow);
}

முழு sysctl_write_ip_neighbor_property_uint32(முழு af, மாறிலி எழுத்து *ifname, மாறிலி எழுத்து *property, uint32_t மதிப்பு, Hashmap **shadow) {
        எழுத்து buf[DECIMAL_STR_MAX(uint32_t)];
        xsprintf(buf, "%u", மதிப்பு);
        திரும்பு sysctl_write_ip_neighbor_property(af, ifname, property, buf, shadow);
}

முழு sysctl_read(மாறிலி எழுத்து *property, எழுத்து **ret) {
        எழுத்து *p;
        முழு r;

        ப்ரதிஜ்ஞா(property);

        p = strjoina("/proc/sys/", property);

        path_simplify(p);
        எனில் (!path_is_normalized(p)) /* Filter out attempts to write to /proc/sys/../../…, just in case */
                திரும்பு -EINVAL;

        r = read_full_virtual_file(p, ret, வெற்று);
        எனில் (r < 0)
                திரும்பு r;
        எனில் (ret)
                delete_trailing_chars(*ret, NEWLINE);

        திரும்பு r;
}

முழு sysctl_read_ip_property(முழு af, மாறிலி எழுத்து *ifname, மாறிலி எழுத்து *property, எழுத்து **ret) {
        மாறிலி எழுத்து *p;

        ப்ரதிஜ்ஞா(property);

        எனில் (!IN_SET(af, AF_INET, AF_INET6, AF_MPLS))
                திரும்பு -EAFNOSUPPORT;

        எனில் (ifname) {
                எனில் (!ifname_valid_full(ifname, IFNAME_VALID_SPECIAL))
                        திரும்பு -EINVAL;

                p = strjoina("net/", af_to_sysctl_dir(af), "/conf/", ifname, "/", property);
        } இல்லையெனில்
                p = strjoina("net/", af_to_sysctl_dir(af), "/", property);

        திரும்பு sysctl_read(p, ret);
}

முழு sysctl_read_ip_property_int(முழு af, மாறிலி எழுத்து *ifname, மாறிலி எழுத்து *property, முழு *ret) {
        _cleanup_free_ எழுத்து *s = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(ret);

        r = sysctl_read_ip_property(af, ifname, property, &s);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு safe_atoi(s, ret);
}

முழு sysctl_read_ip_property_uint32(முழு af, மாறிலி எழுத்து *ifname, மாறிலி எழுத்து *property, uint32_t *ret) {
        _cleanup_free_ எழுத்து *s = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(ret);

        r = sysctl_read_ip_property(af, ifname, property, &s);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு safe_atou32(s, ret);
}
