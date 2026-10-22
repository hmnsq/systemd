/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ <stdio.h>

#അന്തര്ഭാവ "af-list.h"
#അന്തര്ഭാവ "alloc-util.h"
#അന്തര്ഭാവ "fileio.h"
#അന്തര്ഭാവ "hashmap.h"
#അന്തര്ഭാവ "log.h"
#അന്തര്ഭാവ "parse-util.h"
#അന്തര്ഭാവ "path-util.h"
#അന്തര്ഭാവ "socket-util.h"
#അന്തര്ഭാവ "string-util.h"
#അന്തര്ഭാവ "sysctl-util.h"

അക്ഷരം* sysctl_normalize(അക്ഷരം *s) {
        അക്ഷരം *n;

        പ്രതിജ്ഞാ(s);

        n = strpbrk(s, "/.");

        /* If the first separator is a slash, the path is
         * assumed to be normalized and slashes remain slashes
         * and dots remains dots. */

        എങ്കിൽ (n && *n == '.')
                /* Dots become slashes and slashes become dots. Fun. */
                ചെയ്യുക {
                        എങ്കിൽ (*n == '.')
                                *n = '/';
                        അല്ലെങ്കിൽ
                                *n = '.';

                        n = strpbrk(n + 1, "/.");
                } ആകുമ്പോൾ (n);

        path_simplify(s);

        /* Kill the leading slash, but keep the first character of the string in the same place. */
        എങ്കിൽ (s[0] == '/' && s[1] != 0)
                memmove(s, s+1, strlen(s));

        തിരികെ s;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ shadow_update(Hashmap **shadow, സ്ഥിരം അക്ഷരം *property, സ്ഥിരം അക്ഷരം *മൂല്യം) {
        _cleanup_free_ അക്ഷരം *k = ശൂന്യ, *v = ശൂന്യ, *cur_k = ശൂന്യ, *cur_v = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(property);
        പ്രതിജ്ഞാ(മൂല്യം);

        എങ്കിൽ (!shadow)
                തിരികെ 0;

        k = strdup(property);
        എങ്കിൽ (!k)
                തിരികെ -ENOMEM;

        v = strdup(മൂല്യം);
        എങ്കിൽ (!v)
                തിരികെ -ENOMEM;

        cur_v = hashmap_remove2(*shadow, k, (ശൂന്യം**)&cur_k);

        r = hashmap_ensure_put(shadow, &path_hash_ops_free_free, k, v);
        എങ്കിൽ (r < 0) {
                പ്രതിജ്ഞാ(r != -EEXIST);

                തിരികെ r;
        }

        TAKE_PTR(k);
        TAKE_PTR(v);

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ sysctl_write_full(സ്ഥിരം അക്ഷരം *property, സ്ഥിരം അക്ഷരം *മൂല്യം, Hashmap **shadow) {
        അക്ഷരം *p;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(property);
        പ്രതിജ്ഞാ(മൂല്യം);

        p = strjoina("/proc/sys/", property);

        path_simplify(p);
        എങ്കിൽ (!path_is_normalized(p))
                തിരികെ -EINVAL;

        log_debug("Setting '%s' to '%s'", p, മൂല്യം);

        r = shadow_update(shadow, p, മൂല്യം);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ write_string_file(p, മൂല്യം, WRITE_STRING_FILE_VERIFY_ON_FAILURE | WRITE_STRING_FILE_DISABLE_BUFFER | WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL);
}

പൂർണ്ണസംഖ്യ sysctl_writef(സ്ഥിരം അക്ഷരം *property, സ്ഥിരം അക്ഷരം *format, ...) {
        _cleanup_free_ അക്ഷരം *v = ശൂന്യ;
        va_list ap;
        പൂർണ്ണസംഖ്യ r;

        va_start(ap, format);
        r = vasprintf(&v, format, ap);
        va_end(ap);

        എങ്കിൽ (r < 0)
                തിരികെ -ENOMEM;

        തിരികെ sysctl_write(property, v);
}

പൂർണ്ണസംഖ്യ sysctl_write_verify(സ്ഥിരം അക്ഷരം *property, സ്ഥിരം അക്ഷരം *മൂല്യം) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(property);
        പ്രതിജ്ഞാ(മൂല്യം);

        /* Some sysctl settings accept invalid values on write, but refuses on read. E.g. coredump pattern,
         * be1e0283021ec73c2eb92839db9a471a068709d9 (v6.17) and 7d7c1fb85cba5627bbe741fb7539c709435e3848
         * (v6.16.8), which is fixed by a779e27f24aeb679969ddd1fdd7f636e22ddbc1e (v6.18) and
         * 304aa560385720baf3660fe8500f6dd425b63ea9 (v6.17.5). Let's first save the original value, and
         * restore to the saved value if the new value is refused. */

        _cleanup_free_ അക്ഷരം *original = ശൂന്യ;
        r = sysctl_read(property, &original);
        എങ്കിൽ (r >= 0 && streq(original, മൂല്യം))
                തിരികെ 0; /* Already set. */

        r = sysctl_write(property, മൂല്യം);
        എങ്കിൽ (r >= 0) {
                _cleanup_free_ അക്ഷരം *current = ശൂന്യ;
                r = sysctl_read(property, &current);
                എങ്കിൽ (r >= 0) {
                        എങ്കിൽ (streq(current, മൂല്യം))
                                തിരികെ 0; /* Yay! */

                        /* At least for coredump pattern, this does not happen, but let's handle this as the
                         * same as we wrote something invalid. */
                        r = -EINVAL;
                }
        }

        എങ്കിൽ (original)
                (ശൂന്യം) sysctl_write(property, original);

        തിരികെ r;
}

സ്ഥിതം സ്ഥിരം അക്ഷരം* af_to_sysctl_dir(പൂർണ്ണസംഖ്യ af) {
        എങ്കിൽ (af == AF_MPLS)
                തിരികെ "mpls";

        തിരികെ af_to_ipv4_ipv6(af);
}

പൂർണ്ണസംഖ്യ sysctl_write_ip_property(പൂർണ്ണസംഖ്യ af, സ്ഥിരം അക്ഷരം *ifname, സ്ഥിരം അക്ഷരം *property, സ്ഥിരം അക്ഷരം *മൂല്യം, Hashmap **shadow) {
        സ്ഥിരം അക്ഷരം *p;

        പ്രതിജ്ഞാ(property);
        പ്രതിജ്ഞാ(മൂല്യം);

        എങ്കിൽ (!IN_SET(af, AF_INET, AF_INET6, AF_MPLS))
                തിരികെ -EAFNOSUPPORT;

        എങ്കിൽ (ifname) {
                എങ്കിൽ (!ifname_valid_full(ifname, IFNAME_VALID_SPECIAL))
                        തിരികെ -EINVAL;

                p = strjoina("net/", af_to_sysctl_dir(af), "/conf/", ifname, "/", property);
        } അല്ലെങ്കിൽ
                p = strjoina("net/", af_to_sysctl_dir(af), "/", property);

        തിരികെ sysctl_write_full(p, മൂല്യം, shadow);
}

പൂർണ്ണസംഖ്യ sysctl_write_ip_property_boolean(പൂർണ്ണസംഖ്യ af, സ്ഥിരം അക്ഷരം *ifname, സ്ഥിരം അക്ഷരം *property, bool മൂല്യം, Hashmap **shadow) {
        തിരികെ sysctl_write_ip_property(af, ifname, property, one_zero(മൂല്യം), shadow);
}

പൂർണ്ണസംഖ്യ sysctl_write_ip_neighbor_property(പൂർണ്ണസംഖ്യ af, സ്ഥിരം അക്ഷരം *ifname, സ്ഥിരം അക്ഷരം *property, സ്ഥിരം അക്ഷരം *മൂല്യം, Hashmap **shadow) {
        സ്ഥിരം അക്ഷരം *p;

        പ്രതിജ്ഞാ(property);
        പ്രതിജ്ഞാ(മൂല്യം);
        പ്രതിജ്ഞാ(ifname);

        എങ്കിൽ (!IN_SET(af, AF_INET, AF_INET6))
                തിരികെ -EAFNOSUPPORT;

        എങ്കിൽ (ifname) {
                എങ്കിൽ (!ifname_valid_full(ifname, IFNAME_VALID_SPECIAL))
                        തിരികെ -EINVAL;
                p = strjoina("net/", af_to_ipv4_ipv6(af), "/neigh/", ifname, "/", property);
        } അല്ലെങ്കിൽ
                p = strjoina("net/", af_to_ipv4_ipv6(af), "/neigh/default/", property);

        തിരികെ sysctl_write_full(p, മൂല്യം, shadow);
}

പൂർണ്ണസംഖ്യ sysctl_write_ip_neighbor_property_uint32(പൂർണ്ണസംഖ്യ af, സ്ഥിരം അക്ഷരം *ifname, സ്ഥിരം അക്ഷരം *property, uint32_t മൂല്യം, Hashmap **shadow) {
        അക്ഷരം buf[DECIMAL_STR_MAX(uint32_t)];
        xsprintf(buf, "%u", മൂല്യം);
        തിരികെ sysctl_write_ip_neighbor_property(af, ifname, property, buf, shadow);
}

പൂർണ്ണസംഖ്യ sysctl_read(സ്ഥിരം അക്ഷരം *property, അക്ഷരം **ret) {
        അക്ഷരം *p;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(property);

        p = strjoina("/proc/sys/", property);

        path_simplify(p);
        എങ്കിൽ (!path_is_normalized(p)) /* Filter out attempts to write to /proc/sys/../../…, just in case */
                തിരികെ -EINVAL;

        r = read_full_virtual_file(p, ret, ശൂന്യ);
        എങ്കിൽ (r < 0)
                തിരികെ r;
        എങ്കിൽ (ret)
                delete_trailing_chars(*ret, NEWLINE);

        തിരികെ r;
}

പൂർണ്ണസംഖ്യ sysctl_read_ip_property(പൂർണ്ണസംഖ്യ af, സ്ഥിരം അക്ഷരം *ifname, സ്ഥിരം അക്ഷരം *property, അക്ഷരം **ret) {
        സ്ഥിരം അക്ഷരം *p;

        പ്രതിജ്ഞാ(property);

        എങ്കിൽ (!IN_SET(af, AF_INET, AF_INET6, AF_MPLS))
                തിരികെ -EAFNOSUPPORT;

        എങ്കിൽ (ifname) {
                എങ്കിൽ (!ifname_valid_full(ifname, IFNAME_VALID_SPECIAL))
                        തിരികെ -EINVAL;

                p = strjoina("net/", af_to_sysctl_dir(af), "/conf/", ifname, "/", property);
        } അല്ലെങ്കിൽ
                p = strjoina("net/", af_to_sysctl_dir(af), "/", property);

        തിരികെ sysctl_read(p, ret);
}

പൂർണ്ണസംഖ്യ sysctl_read_ip_property_int(പൂർണ്ണസംഖ്യ af, സ്ഥിരം അക്ഷരം *ifname, സ്ഥിരം അക്ഷരം *property, പൂർണ്ണസംഖ്യ *ret) {
        _cleanup_free_ അക്ഷരം *s = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(ret);

        r = sysctl_read_ip_property(af, ifname, property, &s);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ safe_atoi(s, ret);
}

പൂർണ്ണസംഖ്യ sysctl_read_ip_property_uint32(പൂർണ്ണസംഖ്യ af, സ്ഥിരം അക്ഷരം *ifname, സ്ഥിരം അക്ഷരം *property, uint32_t *ret) {
        _cleanup_free_ അക്ഷരം *s = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(ret);

        r = sysctl_read_ip_property(af, ifname, property, &s);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ safe_atou32(s, ret);
}
