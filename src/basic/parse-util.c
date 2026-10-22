/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাৱ <linux/ipv6.h>
#অন্তর্ভাৱ <linux/netfilter/nf_tables.h>
#অন্তর্ভাৱ <stdio.h>
#অন্তর্ভাৱ <stdlib.h>
#অন্তর্ভাৱ <sys/socket.h>

#অন্তর্ভাৱ "alloc-util.h"
#অন্তর্ভাৱ "capability-list.h"
#অন্তর্ভাৱ "capability-util.h"
#অন্তর্ভাৱ "errno-list.h"
#অন্তর্ভাৱ "extract-word.h"
#অন্তর্ভাৱ "locale-util.h"
#অন্তর্ভাৱ "log.h"
#অন্তর্ভাৱ "missing-network.h"
#অন্তর্ভাৱ "parse-util.h"
#অন্তর্ভাৱ "path-util.h"
#অন্তর্ভাৱ "process-util.h"
#অন্তর্ভাৱ "string-util.h"
#অন্তর্ভাৱ "strv.h"

পূর্ণাঙ্ক parse_boolean(ধ্রুৱ ৱর্ণ *v) {
        যদি (!v)
                ঘূৰাই -EINVAL;

        যদি (STRCASE_IN_SET(v,
                           "1",
                           "yes",
                           "y",
                           "true",
                           "t",
                           "on"))
                ঘূৰাই 1;

        যদি (STRCASE_IN_SET(v,
                           "0",
                           "no",
                           "n",
                           "false",
                           "f",
                           "off"))
                ঘূৰাই 0;

        ঘূৰাই -EINVAL;
}

পূর্ণাঙ্ক parse_tristate_full(ধ্রুৱ ৱর্ণ *v, ধ্রুৱ ৱর্ণ *third, পূর্ণাঙ্ক *ret) {
        পূর্ণাঙ্ক r;

        যদি (isempty(v) || streq_ptr(v, third)) { /* Empty string is always taken as the third/invalid/auto state */
                যদি (ret)
                        *ret = -1;
        } অন্যথা {
                r = parse_boolean(v);
                যদি (r < 0)
                        ঘূৰাই r;

                যদি (ret)
                        *ret = r;
        }

        ঘূৰাই 0;
}

পূর্ণাঙ্ক parse_pid(ধ্রুৱ ৱর্ণ *s, pid_t *ret) {
        unsigned long ul = 0;
        pid_t pid;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(s);

        r = safe_atolu(s, &ul);
        যদি (r < 0)
                ঘূৰাই r;

        pid = (pid_t) ul;

        যদি ((unsigned long) pid != ul)
                ঘূৰাই -ERANGE;

        যদি (!pid_is_valid(pid))
                ঘূৰাই -ERANGE;

        যদি (ret)
                *ret = pid;
        ঘূৰাই 0;
}

পূর্ণাঙ্ক parse_mode(ধ্রুৱ ৱর্ণ *s, mode_t *ret) {
        unsigned m;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(s);

        r = safe_atou_full(s, 8 |
                           SAFE_ATO_REFUSE_PLUS_MINUS, /* Leading '+' or even '-' char? that's just weird,
                                                        * refuse. User might have wanted to add mode flags or
                                                        * so, but this parser doesn't allow that, so let's
                                                        * better be safe. */
                           &m);
        যদি (r < 0)
                ঘূৰাই r;
        যদি (m > 07777)
                ঘূৰাই -ERANGE;

        যদি (ret)
                *ret = m;
        ঘূৰাই 0;
}

পূর্ণাঙ্ক parse_ifindex(ধ্রুৱ ৱর্ণ *s) {
        পূর্ণাঙ্ক ifi, r;

        প্রতিজ্ঞা(s);

        r = safe_atoi(s, &ifi);
        যদি (r < 0)
                ঘূৰাই r;
        যদি (ifi <= 0)
                ঘূৰাই -EINVAL;

        ঘূৰাই ifi;
}

পূর্ণাঙ্ক parse_mtu(পূর্ণাঙ্ক family, ধ্রুৱ ৱর্ণ *s, uint32_t *ret) {
        uint64_t u, m;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(ret);

        r = parse_size(s, 1024, &u);
        যদি (r < 0)
                ঘূৰাই r;

        যদি (u > UINT32_MAX)
                ঘূৰাই -ERANGE;

        ৱিকল্প (family) {
        পক্ষ AF_INET:
                m = IPV4_MIN_MTU; /* This is 68 */
                ভঙ্গ;
        পক্ষ AF_INET6:
                m = IPV6_MIN_MTU; /* This is 1280 */
                ভঙ্গ;
        স্ৱভাৱ:
                m = 0;
        }

        যদি (u < m)
                ঘূৰাই -ERANGE;

        *ret = (uint32_t) u;
        ঘূৰাই 0;
}

পূর্ণাঙ্ক parse_size(ধ্রুৱ ৱর্ণ *t, uint64_t base, uint64_t *size) {

        /* Soo, sometimes we want to parse IEC binary suffixes, and
         * sometimes SI decimal suffixes. This function can parse
         * both. Which one is the right way depends on the
         * context. Wikipedia suggests that SI is customary for
         * hardware metrics and network speeds, while IEC is
         * customary for most data sizes used by software and volatile
         * (RAM) memory. Hence be careful which one you pick!
         *
         * In either case we use just K, M, G as suffix, and not Ki,
         * Mi, Gi or so (as IEC would suggest). That's because that's
         * frickin' ugly. But this means you really need to make sure
         * to document which base you are parsing when you use this
         * call. */

        সংরচনা table {
                ধ্রুৱ ৱর্ণ *suffix;
                unsigned long long factor;
        };

        স্থির ধ্রুৱ সংরচনা table iec[] = {
                { "E", 1024ULL*1024ULL*1024ULL*1024ULL*1024ULL*1024ULL },
                { "P", 1024ULL*1024ULL*1024ULL*1024ULL*1024ULL },
                { "T", 1024ULL*1024ULL*1024ULL*1024ULL },
                { "G", 1024ULL*1024ULL*1024ULL },
                { "M", 1024ULL*1024ULL },
                { "K", 1024ULL },
                { "B", 1ULL },
                { "",  1ULL },
        };

        স্থির ধ্রুৱ সংরচনা table si[] = {
                { "E", 1000ULL*1000ULL*1000ULL*1000ULL*1000ULL*1000ULL },
                { "P", 1000ULL*1000ULL*1000ULL*1000ULL*1000ULL },
                { "T", 1000ULL*1000ULL*1000ULL*1000ULL },
                { "G", 1000ULL*1000ULL*1000ULL },
                { "M", 1000ULL*1000ULL },
                { "K", 1000ULL },
                { "B", 1ULL },
                { "",  1ULL },
        };

        ধ্রুৱ সংরচনা table *table;
        ধ্রুৱ ৱর্ণ *p;
        unsigned long long r = 0;
        unsigned n_entries, start_pos = 0;

        প্রতিজ্ঞা(t);
        প্রতিজ্ঞা(IN_SET(base, 1000, 1024));
        প্রতিজ্ঞা(size);

        যদি (base == 1000) {
                table = si;
                n_entries = ELEMENTSOF(si);
        } অন্যথা {
                table = iec;
                n_entries = ELEMENTSOF(iec);
        }

        p = t;
        কৰা {
                unsigned long long l, tmp;
                দ্ৱিগুণ frac = 0;
                ৱর্ণ *e;
                unsigned i;

                p += strspn(p, WHITESPACE);

                errno = 0;
                l = strtoull(p, &e, 10);
                যদি (errno > 0)
                        ঘূৰাই -errno;
                যদি (e == p)
                        ঘূৰাই -EINVAL;
                যদি (*p == '-')
                        ঘূৰাই -ERANGE;

                যদি (*e == '.') {
                        e++;

                        /* strtoull() itself would accept space/+/- */
                        যদি (ascii_isdigit(*e)) {
                                unsigned long long l2;
                                ৱর্ণ *e2;

                                l2 = strtoull(e, &e2, 10);
                                যদি (errno > 0)
                                        ঘূৰাই -errno;

                                /* Ignore failure. E.g. 10.M is valid */
                                frac = l2;
                                বাবে (; e < e2; e++)
                                        frac /= 10;
                        }
                }

                e += strspn(e, WHITESPACE);

                বাবে (i = start_pos; i < n_entries; i++)
                        যদি (startswith(e, table[i].suffix))
                                ভঙ্গ;

                যদি (i >= n_entries)
                        ঘূৰাই -EINVAL;

                যদি (l + (frac > 0) > ULLONG_MAX / table[i].factor)
                        ঘূৰাই -ERANGE;

                tmp = l * table[i].factor + (unsigned long long) (frac * table[i].factor);
                যদি (tmp > ULLONG_MAX - r)
                        ঘূৰাই -ERANGE;

                r += tmp;
                যদি ((unsigned long long) (uint64_t) r != r)
                        ঘূৰাই -ERANGE;

                p = e + strlen(table[i].suffix);

                start_pos = i + 1;

        } যেতিয়ালৈ (*p);

        *size = r;

        ঘূৰাই 0;
}

পূর্ণাঙ্ক parse_sector_size(ধ্রুৱ ৱর্ণ *t, uint64_t *ret) {
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(t);
        প্রতিজ্ঞা(ret);

        uint64_t ss;

        r = safe_atou64(t, &ss);
        যদি (r < 0)
                ঘূৰাই log_error_errno(r, "Failed to parse sector size parameter %s", t);
        যদি (ss < 512 || ss > 4096) /* Allow up to 4K due to dm-crypt support and 4K alignment by the homed LUKS backend */
                ঘূৰাই log_error_errno(SYNTHETIC_ERRNO(ERANGE), "Sector size not between 512 and 4096: %s", t);
        যদি (!ISPOWEROF2(ss))
                ঘূৰাই log_error_errno(SYNTHETIC_ERRNO(EINVAL), "Sector size not power of 2: %s", t);

        *ret = ss;
        ঘূৰাই 0;
}

পূর্ণাঙ্ক parse_range(ধ্রুৱ ৱর্ণ *t, unsigned *lower, unsigned *upper) {
        _cleanup_free_ ৱর্ণ *word = রিক্ত;
        unsigned l, u;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(lower);
        প্রতিজ্ঞা(upper);

        /* Extract the lower bound. */
        r = extract_first_word(&t, &word, "-", EXTRACT_DONT_COALESCE_SEPARATORS);
        যদি (r < 0)
                ঘূৰাই r;
        যদি (r == 0)
                ঘূৰাই -EINVAL;

        r = safe_atou(word, &l);
        যদি (r < 0)
                ঘূৰাই r;

        /* Check for the upper bound and extract it if needed */
        যদি (!t)
                /* Single number with no dashes. */
                u = l;
        অন্যথা যদি (!*t)
                /* Trailing dash is an error. */
                ঘূৰাই -EINVAL;
        অন্যথা {
                r = safe_atou(t, &u);
                যদি (r < 0)
                        ঘূৰাই r;
        }

        *lower = l;
        *upper = u;
        ঘূৰাই 0;
}

পূর্ণাঙ্ক parse_errno(ধ্রুৱ ৱর্ণ *t) {
        পূর্ণাঙ্ক r, e;

        প্রতিজ্ঞা(t);

        r = errno_from_name(t);
        যদি (r > 0)
                ঘূৰাই r;

        r = safe_atoi(t, &e);
        যদি (r < 0)
                ঘূৰাই r;

        /* 0 is also allowed here */
        যদি (!errno_is_valid(e) && e != 0)
                ঘূৰাই -ERANGE;

        ঘূৰাই e;
}

পূর্ণাঙ্ক parse_fd(ধ্রুৱ ৱর্ণ *t) {
        পূর্ণাঙ্ক r, fd;

        প্রতিজ্ঞা(t);

        r = safe_atoi(t, &fd);
        যদি (r < 0)
                ঘূৰাই r;

        যদি (fd < 0)
                ঘূৰাই -EBADF;

        ঘূৰাই fd;
}

পূর্ণাঙ্ক parse_user_shell(ধ্রুৱ ৱর্ণ *s, ৱর্ণ **ret_sh, bool *ret_copy) {
        ৱর্ণ *sh;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(ret_sh);
        প্রতিজ্ঞা(ret_copy);

        যদি (path_is_absolute(s) && path_is_normalized(s)) {
                sh = strdup(s);
                যদি (!sh)
                        ঘূৰাই -ENOMEM;

                *ret_sh = sh;
                *ret_copy = false;
        } অন্যথা {
                r = parse_boolean(s);
                যদি (r < 0)
                        ঘূৰাই r;

                *ret_sh = রিক্ত;
                *ret_copy = r;
        }

        ঘূৰাই 0;
}

স্থির ধ্রুৱ ৱর্ণ *mangle_base(ধ্রুৱ ৱর্ণ *s, unsigned *base) {
        ধ্রুৱ ৱর্ণ *k;

        প্রতিজ্ঞা(s);
        প্রতিজ্ঞা(base);

        /* Base already explicitly specified, then don't do anything. */
        যদি (SAFE_ATO_MASK_FLAGS(*base) != 0)
                ঘূৰাই s;

        /* Support Python 3 style "0b" and 0x" prefixes, because they truly make sense, much more than C's "0" prefix for octal. */
        k = STARTSWITH_SET(s, "0b", "0B");
        যদি (k) {
                *base = 2 | (*base & SAFE_ATO_ALL_FLAGS);
                ঘূৰাই k;
        }

        k = STARTSWITH_SET(s, "0o", "0O");
        যদি (k) {
                *base = 8 | (*base & SAFE_ATO_ALL_FLAGS);
                ঘূৰাই k;
        }

        ঘূৰাই s;
}

পূর্ণাঙ্ক safe_atou_full(ধ্রুৱ ৱর্ণ *s, unsigned base, unsigned *ret_u) {
        ৱর্ণ *x = রিক্ত;
        unsigned long l;

        প্রতিজ্ঞা(s);
        প্রতিজ্ঞা(SAFE_ATO_MASK_FLAGS(base) <= 16);

        /* strtoul() is happy to parse negative values, and silently converts them to unsigned values without
         * generating an error. We want a clean error, hence let's look for the "-" prefix on our own, and
         * generate an error. But let's do so only after strtoul() validated that the string is clean
         * otherwise, so that we return EINVAL preferably over ERANGE. */

        যদি (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_WHITESPACE) &&
            strchr(WHITESPACE, s[0]))
                ঘূৰাই -EINVAL;

        s += strspn(s, WHITESPACE);

        যদি (FLAGS_SET(base, SAFE_ATO_REFUSE_PLUS_MINUS) &&
            IN_SET(s[0], '+', '-'))
                ঘূৰাই -EINVAL; /* Note that we check the "-" prefix again a second time below, but return a
                                 * different error. I.e. if the SAFE_ATO_REFUSE_PLUS_MINUS flag is set we
                                 * blanket refuse +/- prefixed integers, while if it is missing we'll just
                                 * return ERANGE, because the string actually parses correctly, but doesn't
                                 * fit in the return type. */

        যদি (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_ZERO) &&
            s[0] == '0' && !streq(s, "0"))
                ঘূৰাই -EINVAL; /* This is particularly useful to avoid ambiguities between C's octal
                                 * notation and assumed-to-be-decimal integers with a leading zero. */

        s = mangle_base(s, &base);

        errno = 0;
        l = strtoul(s, &x, SAFE_ATO_MASK_FLAGS(base) /* Let's mask off the flags bits so that only the actual
                                                      * base is left */);
        যদি (errno > 0)
                ঘূৰাই -errno;
        যদি (!x || x == s || *x != 0)
                ঘূৰাই -EINVAL;
        যদি (l != 0 && s[0] == '-')
                ঘূৰাই -ERANGE;
        যদি ((unsigned long) (unsigned) l != l)
                ঘূৰাই -ERANGE;

        যদি (ret_u)
                *ret_u = (unsigned) l;

        ঘূৰাই 0;
}

পূর্ণাঙ্ক safe_atou_bounded(ধ্রুৱ ৱর্ণ *s, unsigned min, unsigned max, unsigned *ret) {
        unsigned v;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(ret);

        r = safe_atou(s, &v);
        যদি (r < 0)
                ঘূৰাই r;

        যদি (v < min || v > max)
                ঘূৰাই -ERANGE;

        *ret = v;
        ঘূৰাই 0;
}

পূর্ণাঙ্ক safe_atoi(ধ্রুৱ ৱর্ণ *s, পূর্ণাঙ্ক *ret_i) {
        unsigned base = 0;
        ৱর্ণ *x = রিক্ত;
        long l;

        প্রতিজ্ঞা(s);

        s += strspn(s, WHITESPACE);
        s = mangle_base(s, &base);

        errno = 0;
        l = strtol(s, &x, base);
        যদি (errno > 0)
                ঘূৰাই -errno;
        যদি (!x || x == s || *x != 0)
                ঘূৰাই -EINVAL;
        যদি ((long) (পূর্ণাঙ্ক) l != l)
                ঘূৰাই -ERANGE;

        যদি (ret_i)
                *ret_i = (পূর্ণাঙ্ক) l;

        ঘূৰাই 0;
}

পূর্ণাঙ্ক safe_atollu_full(ধ্রুৱ ৱর্ণ *s, unsigned base, unsigned long long *ret_llu) {
        ৱর্ণ *x = রিক্ত;
        unsigned long long l;

        প্রতিজ্ঞা(s);
        প্রতিজ্ঞা(SAFE_ATO_MASK_FLAGS(base) <= 16);

        যদি (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_WHITESPACE) &&
            strchr(WHITESPACE, s[0]))
                ঘূৰাই -EINVAL;

        s += strspn(s, WHITESPACE);

        যদি (FLAGS_SET(base, SAFE_ATO_REFUSE_PLUS_MINUS) &&
            IN_SET(s[0], '+', '-'))
                ঘূৰাই -EINVAL;

        যদি (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_ZERO) &&
            s[0] == '0' && s[1] != 0)
                ঘূৰাই -EINVAL;

        s = mangle_base(s, &base);

        errno = 0;
        l = strtoull(s, &x, SAFE_ATO_MASK_FLAGS(base));
        যদি (errno > 0)
                ঘূৰাই -errno;
        যদি (!x || x == s || *x != 0)
                ঘূৰাই -EINVAL;
        যদি (l != 0 && s[0] == '-')
                ঘূৰাই -ERANGE;

        যদি (ret_llu)
                *ret_llu = l;

        ঘূৰাই 0;
}

পূর্ণাঙ্ক safe_atolli(ধ্রুৱ ৱর্ণ *s, long long *ret_lli) {
        unsigned base = 0;
        ৱর্ণ *x = রিক্ত;
        long long l;

        প্রতিজ্ঞা(s);

        s += strspn(s, WHITESPACE);
        s = mangle_base(s, &base);

        errno = 0;
        l = strtoll(s, &x, base);
        যদি (errno > 0)
                ঘূৰাই -errno;
        যদি (!x || x == s || *x != 0)
                ঘূৰাই -EINVAL;

        যদি (ret_lli)
                *ret_lli = l;

        ঘূৰাই 0;
}

পূর্ণাঙ্ক safe_atou8_full(ধ্রুৱ ৱর্ণ *s, unsigned base, uint8_t *ret) {
        unsigned u;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(ret);

        r = safe_atou_full(s, base, &u);
        যদি (r < 0)
                ঘূৰাই r;
        যদি (u > UINT8_MAX)
                ঘূৰাই -ERANGE;

        *ret = (uint8_t) u;
        ঘূৰাই 0;
}

পূর্ণাঙ্ক safe_atou16_full(ধ্রুৱ ৱর্ণ *s, unsigned base, uint16_t *ret) {
        unsigned u;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(ret);

        r = safe_atou_full(s, base, &u);
        যদি (r < 0)
                ঘূৰাই r;
        যদি (u > UINT16_MAX)
                ঘূৰাই -ERANGE;

        *ret = (uint16_t) u;
        ঘূৰাই 0;
}

পূর্ণাঙ্ক safe_atoi16(ধ্রুৱ ৱর্ণ *s, int16_t *ret) {
        unsigned base = 0;
        ৱর্ণ *x = রিক্ত;
        long l;

        প্রতিজ্ঞা(s);

        s += strspn(s, WHITESPACE);
        s = mangle_base(s, &base);

        errno = 0;
        l = strtol(s, &x, base);
        যদি (errno > 0)
                ঘূৰাই -errno;
        যদি (!x || x == s || *x != 0)
                ঘূৰাই -EINVAL;
        যদি ((long) (int16_t) l != l)
                ঘূৰাই -ERANGE;

        যদি (ret)
                *ret = (int16_t) l;

        ঘূৰাই 0;
}

পূর্ণাঙ্ক safe_atod(ধ্রুৱ ৱর্ণ *s, দ্ৱিগুণ *ret_d) {
        _cleanup_(freelocalep) locale_t loc = (locale_t) 0;
        ৱর্ণ *x = রিক্ত;
        দ্ৱিগুণ d = 0;

        প্রতিজ্ঞা(s);

        loc = newlocale(LC_NUMERIC_MASK, "C", (locale_t) 0);
        যদি (loc == (locale_t) 0)
                ঘূৰাই -errno;

        errno = 0;
        d = strtod_l(s, &x, loc);
        যদি (errno > 0)
                ঘূৰাই -errno;
        যদি (!x || x == s || *x != 0)
                ঘূৰাই -EINVAL;

        যদি (ret_d)
                *ret_d = d;

        ঘূৰাই 0;
}

পূর্ণাঙ্ক parse_fractional_part_u(ধ্রুৱ ৱর্ণ **p, size_t digits, unsigned *res) {
        unsigned val = 0;
        ধ্রুৱ ৱর্ণ *s;

        প্রতিজ্ঞা(p);
        প্রতিজ্ঞা(res);

        s = *p;

        /* accept any number of digits, strtoull is limited to 19 */
        বাবে (size_t i = 0; i < digits; i++, s++) {
                যদি (!ascii_isdigit(*s)) {
                        যদি (i == 0)
                                ঘূৰাই -EINVAL;

                        /* too few digits, pad with 0 */
                        বাবে (; i < digits; i++)
                                val *= 10;

                        ভঙ্গ;
                }

                val *= 10;
                val += *s - '0';
        }

        /* maybe round up */
        যদি (*s >= '5' && *s <= '9')
                val++;

        s += strspn(s, DIGITS);

        *p = s;
        *res = val;

        ঘূৰাই 0;
}

পূর্ণাঙ্ক parse_nice(ধ্রুৱ ৱর্ণ *s, পূর্ণাঙ্ক *ret) {
        পূর্ণাঙ্ক n, r;

        প্রতিজ্ঞা(ret);

        r = safe_atoi(s, &n);
        যদি (r < 0)
                ঘূৰাই r;

        যদি (!nice_is_valid(n))
                ঘূৰাই -ERANGE;

        *ret = n;
        ঘূৰাই 0;
}

পূর্ণাঙ্ক parse_ip_port(ধ্রুৱ ৱর্ণ *s, uint16_t *ret) {
        uint16_t l;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(ret);

        r = safe_atou16_full(s, SAFE_ATO_REFUSE_LEADING_WHITESPACE, &l);
        যদি (r < 0)
                ঘূৰাই r;

        যদি (l == 0)
                ঘূৰাই -EINVAL;

        *ret = l;

        ঘূৰাই 0;
}

পূর্ণাঙ্ক parse_ip_port_range(ধ্রুৱ ৱর্ণ *s, uint16_t *low, uint16_t *high, bool allow_zero) {
        unsigned l, h;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(low);
        প্রতিজ্ঞা(high);

        r = parse_range(s, &l, &h);
        যদি (r < 0)
                ঘূৰাই r;

        যদি (l > 65535 || h > 65535)
                ঘূৰাই -EINVAL;

        যদি (!allow_zero && (l == 0 || h == 0))
                ঘূৰাই -EINVAL;

        যদি (h < l)
                ঘূৰাই -EINVAL;

        *low = l;
        *high = h;

        ঘূৰাই 0;
}

পূর্ণাঙ্ক parse_oom_score_adjust(ধ্রুৱ ৱর্ণ *s, পূর্ণাঙ্ক *ret) {
        পূর্ণাঙ্ক r, v;

        প্রতিজ্ঞা(s);
        প্রতিজ্ঞা(ret);

        r = safe_atoi(s, &v);
        যদি (r < 0)
                ঘূৰাই r;

        যদি (!oom_score_adjust_is_valid(v))
                ঘূৰাই -ERANGE;

        *ret = v;
        ঘূৰাই 0;
}

পূর্ণাঙ্ক store_loadavg_fixed_point(unsigned long i, unsigned long f, loadavg_t *ret) {
        প্রতিজ্ঞা(ret);

        /* Validate both parts *before* shifting them, so that the shifts themselves cannot overflow.
         * Rejecting f >= 100 is equivalent to the old check of the shifted value against
         * LOADAVG_FIXED_POINT_1_0, since DIV_ROUND_UP(99 << 11, 100) == 2028 and
         * DIV_ROUND_UP(100 << 11, 100) == 2048. */
        যদি (i > (~0UL >> LOADAVG_PRECISION_BITS))
                ঘূৰাই -ERANGE;

        যদি (f >= 100)
                ঘূৰাই -ERANGE;

        *ret = (i << LOADAVG_PRECISION_BITS) | DIV_ROUND_UP(f << LOADAVG_PRECISION_BITS, 100);
        ঘূৰাই 0;
}

পূর্ণাঙ্ক parse_loadavg_fixed_point(ধ্রুৱ ৱর্ণ *s, loadavg_t *ret) {
        ধ্রুৱ ৱর্ণ *d, *f_str, *i_str;
        unsigned long i, f;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(s);
        প্রতিজ্ঞা(ret);

        d = strchr(s, '.');
        যদি (!d)
                ঘূৰাই -EINVAL;

        i_str = strndupa_safe(s, d - s);
        f_str = d + 1;

        r = safe_atolu_full(i_str, 10, &i);
        যদি (r < 0)
                ঘূৰাই r;

        r = safe_atolu_full(f_str, 10, &f);
        যদি (r < 0)
                ঘূৰাই r;

        ঘূৰাই store_loadavg_fixed_point(i, f, ret);
}

/* Limitations are described in https://www.netfilter.org/projects/nftables/manpage.html and
 * https://bugzilla.netfilter.org/show_bug.cgi?id=1175 */
bool nft_identifier_valid(ধ্রুৱ ৱর্ণ *id) {
        যদি (isempty(id))
                ঘূৰাই false;

        যদি (strlen(id) >= NFT_NAME_MAXLEN)
                ঘূৰাই false;

        যদি (!ascii_isalpha(id[0]))
                ঘূৰাই false;

        ঘূৰাই in_charset(id + 1, ALPHANUMERICAL "/\\_.");
}

পূর্ণাঙ্ক parse_capability_set(ধ্রুৱ ৱর্ণ *s, uint64_t initial, uint64_t *current) {
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(s);
        প্রতিজ্ঞা(current);

        যদি (isempty(s)) {
                *current = CAP_MASK_UNSET;
                ঘূৰাই 1;
        }

        bool invert = false;
        যদি (s[0] == '~') {
                invert = true;
                s++;
        }

        uint64_t parsed;
        r = capability_set_from_string(s, &parsed);
        যদি (r < 0)
                ঘূৰাই r;

        যদি (parsed == 0 || *current == initial)
                /* "~" or uninitialized data -> replace */
                *current = invert ? all_capabilities() & ~parsed : parsed;
        অন্যথা {
                /* previous data -> merge */
                যদি (invert)
                        *current &= ~parsed;
                অন্যথা
                        *current |= parsed;
        }

        ঘূৰাই r;
}
