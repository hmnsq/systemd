/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <linux/ipv6.h>
#অন্তর্ভাব <linux/netfilter/nf_tables.h>
#অন্তর্ভাব <stdio.h>
#অন্তর্ভাব <stdlib.h>
#অন্তর্ভাব <sys/socket.h>

#অন্তর্ভাব "alloc-util.h"
#অন্তর্ভাব "capability-list.h"
#অন্তর্ভাব "capability-util.h"
#অন্তর্ভাব "errno-list.h"
#অন্তর্ভাব "extract-word.h"
#অন্তর্ভাব "locale-util.h"
#অন্তর্ভাব "log.h"
#অন্তর্ভাব "missing-network.h"
#অন্তর্ভাব "parse-util.h"
#অন্তর্ভাব "path-util.h"
#অন্তর্ভাব "process-util.h"
#অন্তর্ভাব "string-util.h"
#অন্তর্ভাব "strv.h"

পূর্ণসংখ্যা parse_boolean(ধ্রুবক অক্ষর *v) {
        যদি (!v)
                ফেরত -EINVAL;

        যদি (STRCASE_IN_SET(v,
                           "1",
                           "yes",
                           "y",
                           "true",
                           "t",
                           "on"))
                ফেরত 1;

        যদি (STRCASE_IN_SET(v,
                           "0",
                           "no",
                           "n",
                           "false",
                           "f",
                           "off"))
                ফেরত 0;

        ফেরত -EINVAL;
}

পূর্ণসংখ্যা parse_tristate_full(ধ্রুবক অক্ষর *v, ধ্রুবক অক্ষর *third, পূর্ণসংখ্যা *ret) {
        পূর্ণসংখ্যা r;

        যদি (isempty(v) || streq_ptr(v, third)) { /* Empty string is always taken as the third/invalid/auto state */
                যদি (ret)
                        *ret = -1;
        } অন্যথা {
                r = parse_boolean(v);
                যদি (r < 0)
                        ফেরত r;

                যদি (ret)
                        *ret = r;
        }

        ফেরত 0;
}

পূর্ণসংখ্যা parse_pid(ধ্রুবক অক্ষর *s, pid_t *ret) {
        unsigned long ul = 0;
        pid_t pid;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(s);

        r = safe_atolu(s, &ul);
        যদি (r < 0)
                ফেরত r;

        pid = (pid_t) ul;

        যদি ((unsigned long) pid != ul)
                ফেরত -ERANGE;

        যদি (!pid_is_valid(pid))
                ফেরত -ERANGE;

        যদি (ret)
                *ret = pid;
        ফেরত 0;
}

পূর্ণসংখ্যা parse_mode(ধ্রুবক অক্ষর *s, mode_t *ret) {
        unsigned m;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(s);

        r = safe_atou_full(s, 8 |
                           SAFE_ATO_REFUSE_PLUS_MINUS, /* Leading '+' or even '-' char? that's just weird,
                                                        * refuse. User might have wanted to add mode flags or
                                                        * so, but this parser doesn't allow that, so let's
                                                        * better be safe. */
                           &m);
        যদি (r < 0)
                ফেরত r;
        যদি (m > 07777)
                ফেরত -ERANGE;

        যদি (ret)
                *ret = m;
        ফেরত 0;
}

পূর্ণসংখ্যা parse_ifindex(ধ্রুবক অক্ষর *s) {
        পূর্ণসংখ্যা ifi, r;

        প্রতিজ্ঞা(s);

        r = safe_atoi(s, &ifi);
        যদি (r < 0)
                ফেরত r;
        যদি (ifi <= 0)
                ফেরত -EINVAL;

        ফেরত ifi;
}

পূর্ণসংখ্যা parse_mtu(পূর্ণসংখ্যা family, ধ্রুবক অক্ষর *s, uint32_t *ret) {
        uint64_t u, m;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(ret);

        r = parse_আকার(s, 1024, &u);
        যদি (r < 0)
                ফেরত r;

        যদি (u > UINT32_MAX)
                ফেরত -ERANGE;

        নির্বাচন (family) {
        ক্ষেত্র AF_INET:
                m = IPV4_MIN_MTU; /* This is 68 */
                বিরতি;
        ক্ষেত্র AF_INET6:
                m = IPV6_MIN_MTU; /* This is 1280 */
                বিরতি;
        পূর্বনির্ধারিত:
                m = 0;
        }

        যদি (u < m)
                ফেরত -ERANGE;

        *ret = (uint32_t) u;
        ফেরত 0;
}

পূর্ণসংখ্যা parse_আকার(ধ্রুবক অক্ষর *t, uint64_t base, uint64_t *আকার_2) {

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

        কাঠামো table {
                ধ্রুবক অক্ষর *suffix;
                unsigned long long factor;
        };

        স্থির ধ্রুবক কাঠামো table iec[] = {
                { "E", 1024ULL*1024ULL*1024ULL*1024ULL*1024ULL*1024ULL },
                { "P", 1024ULL*1024ULL*1024ULL*1024ULL*1024ULL },
                { "T", 1024ULL*1024ULL*1024ULL*1024ULL },
                { "G", 1024ULL*1024ULL*1024ULL },
                { "M", 1024ULL*1024ULL },
                { "K", 1024ULL },
                { "B", 1ULL },
                { "",  1ULL },
        };

        স্থির ধ্রুবক কাঠামো table si[] = {
                { "E", 1000ULL*1000ULL*1000ULL*1000ULL*1000ULL*1000ULL },
                { "P", 1000ULL*1000ULL*1000ULL*1000ULL*1000ULL },
                { "T", 1000ULL*1000ULL*1000ULL*1000ULL },
                { "G", 1000ULL*1000ULL*1000ULL },
                { "M", 1000ULL*1000ULL },
                { "K", 1000ULL },
                { "B", 1ULL },
                { "",  1ULL },
        };

        ধ্রুবক কাঠামো table *table;
        ধ্রুবক অক্ষর *p;
        unsigned long long r = 0;
        unsigned n_entries, start_pos = 0;

        প্রতিজ্ঞা(t);
        প্রতিজ্ঞা(IN_SET(base, 1000, 1024));
        প্রতিজ্ঞা(আকার_2);

        যদি (base == 1000) {
                table = si;
                n_entries = ELEMENTSOF(si);
        } অন্যথা {
                table = iec;
                n_entries = ELEMENTSOF(iec);
        }

        p = t;
        করো {
                unsigned long long l, tmp;
                দ্বিগুণ frac = 0;
                অক্ষর *e;
                unsigned i;

                p += strspn(p, WHITESPACE);

                errno = 0;
                l = strtoull(p, &e, 10);
                যদি (errno > 0)
                        ফেরত -errno;
                যদি (e == p)
                        ফেরত -EINVAL;
                যদি (*p == '-')
                        ফেরত -ERANGE;

                যদি (*e == '.') {
                        e++;

                        /* strtoull() itself would accept space/+/- */
                        যদি (ascii_isdigit(*e)) {
                                unsigned long long l2;
                                অক্ষর *e2;

                                l2 = strtoull(e, &e2, 10);
                                যদি (errno > 0)
                                        ফেরত -errno;

                                /* Ignore failure. E.g. 10.M is valid */
                                frac = l2;
                                জন্য (; e < e2; e++)
                                        frac /= 10;
                        }
                }

                e += strspn(e, WHITESPACE);

                জন্য (i = start_pos; i < n_entries; i++)
                        যদি (startswith(e, table[i].suffix))
                                বিরতি;

                যদি (i >= n_entries)
                        ফেরত -EINVAL;

                যদি (l + (frac > 0) > ULLONG_MAX / table[i].factor)
                        ফেরত -ERANGE;

                tmp = l * table[i].factor + (unsigned long long) (frac * table[i].factor);
                যদি (tmp > ULLONG_MAX - r)
                        ফেরত -ERANGE;

                r += tmp;
                যদি ((unsigned long long) (uint64_t) r != r)
                        ফেরত -ERANGE;

                p = e + strlen(table[i].suffix);

                start_pos = i + 1;

        } যখন (*p);

        *আকার_2 = r;

        ফেরত 0;
}

পূর্ণসংখ্যা parse_sector_আকার(ধ্রুবক অক্ষর *t, uint64_t *ret) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(t);
        প্রতিজ্ঞা(ret);

        uint64_t ss;

        r = safe_atou64(t, &ss);
        যদি (r < 0)
                ফেরত log_error_errno(r, "Failed to parse sector size parameter %s", t);
        যদি (ss < 512 || ss > 4096) /* Allow up to 4K due to dm-crypt support and 4K alignment by the homed LUKS backend */
                ফেরত log_error_errno(SYNTHETIC_ERRNO(ERANGE), "Sector size not between 512 and 4096: %s", t);
        যদি (!ISPOWEROF2(ss))
                ফেরত log_error_errno(SYNTHETIC_ERRNO(EINVAL), "Sector size not power of 2: %s", t);

        *ret = ss;
        ফেরত 0;
}

পূর্ণসংখ্যা parse_range(ধ্রুবক অক্ষর *t, unsigned *lower, unsigned *upper) {
        _cleanup_free_ অক্ষর *word = ফাঁকা;
        unsigned l, u;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(lower);
        প্রতিজ্ঞা(upper);

        /* Extract the lower bound. */
        r = extract_first_word(&t, &word, "-", EXTRACT_DONT_COALESCE_SEPARATORS);
        যদি (r < 0)
                ফেরত r;
        যদি (r == 0)
                ফেরত -EINVAL;

        r = safe_atou(word, &l);
        যদি (r < 0)
                ফেরত r;

        /* Check for the upper bound and extract it if needed */
        যদি (!t)
                /* Single number with no dashes. */
                u = l;
        অন্যথা যদি (!*t)
                /* Trailing dash is an error. */
                ফেরত -EINVAL;
        অন্যথা {
                r = safe_atou(t, &u);
                যদি (r < 0)
                        ফেরত r;
        }

        *lower = l;
        *upper = u;
        ফেরত 0;
}

পূর্ণসংখ্যা parse_errno(ধ্রুবক অক্ষর *t) {
        পূর্ণসংখ্যা r, e;

        প্রতিজ্ঞা(t);

        r = errno_from_name(t);
        যদি (r > 0)
                ফেরত r;

        r = safe_atoi(t, &e);
        যদি (r < 0)
                ফেরত r;

        /* 0 is also allowed here */
        যদি (!errno_is_valid(e) && e != 0)
                ফেরত -ERANGE;

        ফেরত e;
}

পূর্ণসংখ্যা parse_fd(ধ্রুবক অক্ষর *t) {
        পূর্ণসংখ্যা r, fd;

        প্রতিজ্ঞা(t);

        r = safe_atoi(t, &fd);
        যদি (r < 0)
                ফেরত r;

        যদি (fd < 0)
                ফেরত -EBADF;

        ফেরত fd;
}

পূর্ণসংখ্যা parse_user_shell(ধ্রুবক অক্ষর *s, অক্ষর **ret_sh, bool *ret_copy) {
        অক্ষর *sh;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(ret_sh);
        প্রতিজ্ঞা(ret_copy);

        যদি (path_is_absolute(s) && path_is_normalized(s)) {
                sh = strdup(s);
                যদি (!sh)
                        ফেরত -ENOMEM;

                *ret_sh = sh;
                *ret_copy = false;
        } অন্যথা {
                r = parse_boolean(s);
                যদি (r < 0)
                        ফেরত r;

                *ret_sh = ফাঁকা;
                *ret_copy = r;
        }

        ফেরত 0;
}

স্থির ধ্রুবক অক্ষর *mangle_base(ধ্রুবক অক্ষর *s, unsigned *base) {
        ধ্রুবক অক্ষর *k;

        প্রতিজ্ঞা(s);
        প্রতিজ্ঞা(base);

        /* Base already explicitly specified, then don't do anything. */
        যদি (SAFE_ATO_MASK_FLAGS(*base) != 0)
                ফেরত s;

        /* Support Python 3 style "0b" and 0x" prefixes, because they truly make sense, much more than C's "0" prefix for octal. */
        k = STARTSWITH_SET(s, "0b", "0B");
        যদি (k) {
                *base = 2 | (*base & SAFE_ATO_ALL_FLAGS);
                ফেরত k;
        }

        k = STARTSWITH_SET(s, "0o", "0O");
        যদি (k) {
                *base = 8 | (*base & SAFE_ATO_ALL_FLAGS);
                ফেরত k;
        }

        ফেরত s;
}

পূর্ণসংখ্যা safe_atou_full(ধ্রুবক অক্ষর *s, unsigned base, unsigned *ret_u) {
        অক্ষর *x = ফাঁকা;
        unsigned long l;

        প্রতিজ্ঞা(s);
        প্রতিজ্ঞা(SAFE_ATO_MASK_FLAGS(base) <= 16);

        /* strtoul() is happy to parse negative values, and silently converts them to unsigned values without
         * generating an error. We want a clean error, hence let's look for the "-" prefix on our own, and
         * generate an error. But let's do so only after strtoul() validated that the string is clean
         * otherwise, so that we return EINVAL preferably over ERANGE. */

        যদি (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_WHITESPACE) &&
            strchr(WHITESPACE, s[0]))
                ফেরত -EINVAL;

        s += strspn(s, WHITESPACE);

        যদি (FLAGS_SET(base, SAFE_ATO_REFUSE_PLUS_MINUS) &&
            IN_SET(s[0], '+', '-'))
                ফেরত -EINVAL; /* Note that we check the "-" prefix again a second time below, but return a
                                 * different error. I.e. if the SAFE_ATO_REFUSE_PLUS_MINUS flag is set we
                                 * blanket refuse +/- prefixed integers, while if it is missing we'll just
                                 * return ERANGE, because the string actually parses correctly, but doesn't
                                 * fit in the return type. */

        যদি (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_ZERO) &&
            s[0] == '0' && !streq(s, "0"))
                ফেরত -EINVAL; /* This is particularly useful to avoid ambiguities between C's octal
                                 * notation and assumed-to-be-decimal integers with a leading zero. */

        s = mangle_base(s, &base);

        errno = 0;
        l = strtoul(s, &x, SAFE_ATO_MASK_FLAGS(base) /* Let's mask off the flags bits so that only the actual
                                                      * base is left */);
        যদি (errno > 0)
                ফেরত -errno;
        যদি (!x || x == s || *x != 0)
                ফেরত -EINVAL;
        যদি (l != 0 && s[0] == '-')
                ফেরত -ERANGE;
        যদি ((unsigned long) (unsigned) l != l)
                ফেরত -ERANGE;

        যদি (ret_u)
                *ret_u = (unsigned) l;

        ফেরত 0;
}

পূর্ণসংখ্যা safe_atou_bounded(ধ্রুবক অক্ষর *s, unsigned min, unsigned max, unsigned *ret) {
        unsigned v;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(ret);

        r = safe_atou(s, &v);
        যদি (r < 0)
                ফেরত r;

        যদি (v < min || v > max)
                ফেরত -ERANGE;

        *ret = v;
        ফেরত 0;
}

পূর্ণসংখ্যা safe_atoi(ধ্রুবক অক্ষর *s, পূর্ণসংখ্যা *ret_i) {
        unsigned base = 0;
        অক্ষর *x = ফাঁকা;
        long l;

        প্রতিজ্ঞা(s);

        s += strspn(s, WHITESPACE);
        s = mangle_base(s, &base);

        errno = 0;
        l = strtol(s, &x, base);
        যদি (errno > 0)
                ফেরত -errno;
        যদি (!x || x == s || *x != 0)
                ফেরত -EINVAL;
        যদি ((long) (পূর্ণসংখ্যা) l != l)
                ফেরত -ERANGE;

        যদি (ret_i)
                *ret_i = (পূর্ণসংখ্যা) l;

        ফেরত 0;
}

পূর্ণসংখ্যা safe_atollu_full(ধ্রুবক অক্ষর *s, unsigned base, unsigned long long *ret_llu) {
        অক্ষর *x = ফাঁকা;
        unsigned long long l;

        প্রতিজ্ঞা(s);
        প্রতিজ্ঞা(SAFE_ATO_MASK_FLAGS(base) <= 16);

        যদি (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_WHITESPACE) &&
            strchr(WHITESPACE, s[0]))
                ফেরত -EINVAL;

        s += strspn(s, WHITESPACE);

        যদি (FLAGS_SET(base, SAFE_ATO_REFUSE_PLUS_MINUS) &&
            IN_SET(s[0], '+', '-'))
                ফেরত -EINVAL;

        যদি (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_ZERO) &&
            s[0] == '0' && s[1] != 0)
                ফেরত -EINVAL;

        s = mangle_base(s, &base);

        errno = 0;
        l = strtoull(s, &x, SAFE_ATO_MASK_FLAGS(base));
        যদি (errno > 0)
                ফেরত -errno;
        যদি (!x || x == s || *x != 0)
                ফেরত -EINVAL;
        যদি (l != 0 && s[0] == '-')
                ফেরত -ERANGE;

        যদি (ret_llu)
                *ret_llu = l;

        ফেরত 0;
}

পূর্ণসংখ্যা safe_atolli(ধ্রুবক অক্ষর *s, long long *ret_lli) {
        unsigned base = 0;
        অক্ষর *x = ফাঁকা;
        long long l;

        প্রতিজ্ঞা(s);

        s += strspn(s, WHITESPACE);
        s = mangle_base(s, &base);

        errno = 0;
        l = strtoll(s, &x, base);
        যদি (errno > 0)
                ফেরত -errno;
        যদি (!x || x == s || *x != 0)
                ফেরত -EINVAL;

        যদি (ret_lli)
                *ret_lli = l;

        ফেরত 0;
}

পূর্ণসংখ্যা safe_atou8_full(ধ্রুবক অক্ষর *s, unsigned base, uint8_t *ret) {
        unsigned u;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(ret);

        r = safe_atou_full(s, base, &u);
        যদি (r < 0)
                ফেরত r;
        যদি (u > UINT8_MAX)
                ফেরত -ERANGE;

        *ret = (uint8_t) u;
        ফেরত 0;
}

পূর্ণসংখ্যা safe_atou16_full(ধ্রুবক অক্ষর *s, unsigned base, uint16_t *ret) {
        unsigned u;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(ret);

        r = safe_atou_full(s, base, &u);
        যদি (r < 0)
                ফেরত r;
        যদি (u > UINT16_MAX)
                ফেরত -ERANGE;

        *ret = (uint16_t) u;
        ফেরত 0;
}

পূর্ণসংখ্যা safe_atoi16(ধ্রুবক অক্ষর *s, int16_t *ret) {
        unsigned base = 0;
        অক্ষর *x = ফাঁকা;
        long l;

        প্রতিজ্ঞা(s);

        s += strspn(s, WHITESPACE);
        s = mangle_base(s, &base);

        errno = 0;
        l = strtol(s, &x, base);
        যদি (errno > 0)
                ফেরত -errno;
        যদি (!x || x == s || *x != 0)
                ফেরত -EINVAL;
        যদি ((long) (int16_t) l != l)
                ফেরত -ERANGE;

        যদি (ret)
                *ret = (int16_t) l;

        ফেরত 0;
}

পূর্ণসংখ্যা safe_atod(ধ্রুবক অক্ষর *s, দ্বিগুণ *ret_d) {
        _cleanup_(freelocalep) locale_t loc = (locale_t) 0;
        অক্ষর *x = ফাঁকা;
        দ্বিগুণ d = 0;

        প্রতিজ্ঞা(s);

        loc = newlocale(LC_NUMERIC_MASK, "C", (locale_t) 0);
        যদি (loc == (locale_t) 0)
                ফেরত -errno;

        errno = 0;
        d = strtod_l(s, &x, loc);
        যদি (errno > 0)
                ফেরত -errno;
        যদি (!x || x == s || *x != 0)
                ফেরত -EINVAL;

        যদি (ret_d)
                *ret_d = d;

        ফেরত 0;
}

পূর্ণসংখ্যা parse_fractional_part_u(ধ্রুবক অক্ষর **p, size_t digits, unsigned *res) {
        unsigned val = 0;
        ধ্রুবক অক্ষর *s;

        প্রতিজ্ঞা(p);
        প্রতিজ্ঞা(res);

        s = *p;

        /* accept any number of digits, strtoull is limited to 19 */
        জন্য (size_t i = 0; i < digits; i++, s++) {
                যদি (!ascii_isdigit(*s)) {
                        যদি (i == 0)
                                ফেরত -EINVAL;

                        /* too few digits, pad with 0 */
                        জন্য (; i < digits; i++)
                                val *= 10;

                        বিরতি;
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

        ফেরত 0;
}

পূর্ণসংখ্যা parse_nice(ধ্রুবক অক্ষর *s, পূর্ণসংখ্যা *ret) {
        পূর্ণসংখ্যা n, r;

        প্রতিজ্ঞা(ret);

        r = safe_atoi(s, &n);
        যদি (r < 0)
                ফেরত r;

        যদি (!nice_is_valid(n))
                ফেরত -ERANGE;

        *ret = n;
        ফেরত 0;
}

পূর্ণসংখ্যা parse_ip_port(ধ্রুবক অক্ষর *s, uint16_t *ret) {
        uint16_t l;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(ret);

        r = safe_atou16_full(s, SAFE_ATO_REFUSE_LEADING_WHITESPACE, &l);
        যদি (r < 0)
                ফেরত r;

        যদি (l == 0)
                ফেরত -EINVAL;

        *ret = l;

        ফেরত 0;
}

পূর্ণসংখ্যা parse_ip_port_range(ধ্রুবক অক্ষর *s, uint16_t *low, uint16_t *high, bool allow_zero) {
        unsigned l, h;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(low);
        প্রতিজ্ঞা(high);

        r = parse_range(s, &l, &h);
        যদি (r < 0)
                ফেরত r;

        যদি (l > 65535 || h > 65535)
                ফেরত -EINVAL;

        যদি (!allow_zero && (l == 0 || h == 0))
                ফেরত -EINVAL;

        যদি (h < l)
                ফেরত -EINVAL;

        *low = l;
        *high = h;

        ফেরত 0;
}

পূর্ণসংখ্যা parse_oom_score_adjust(ধ্রুবক অক্ষর *s, পূর্ণসংখ্যা *ret) {
        পূর্ণসংখ্যা r, v;

        প্রতিজ্ঞা(s);
        প্রতিজ্ঞা(ret);

        r = safe_atoi(s, &v);
        যদি (r < 0)
                ফেরত r;

        যদি (!oom_score_adjust_is_valid(v))
                ফেরত -ERANGE;

        *ret = v;
        ফেরত 0;
}

পূর্ণসংখ্যা store_loadavg_fixed_point(unsigned long i, unsigned long f, loadavg_t *ret) {
        প্রতিজ্ঞা(ret);

        /* Validate both parts *before* shifting them, so that the shifts themselves cannot overflow.
         * Rejecting f >= 100 is equivalent to the old check of the shifted value against
         * LOADAVG_FIXED_POINT_1_0, since DIV_ROUND_UP(99 << 11, 100) == 2028 and
         * DIV_ROUND_UP(100 << 11, 100) == 2048. */
        যদি (i > (~0UL >> LOADAVG_PRECISION_BITS))
                ফেরত -ERANGE;

        যদি (f >= 100)
                ফেরত -ERANGE;

        *ret = (i << LOADAVG_PRECISION_BITS) | DIV_ROUND_UP(f << LOADAVG_PRECISION_BITS, 100);
        ফেরত 0;
}

পূর্ণসংখ্যা parse_loadavg_fixed_point(ধ্রুবক অক্ষর *s, loadavg_t *ret) {
        ধ্রুবক অক্ষর *d, *f_str, *i_str;
        unsigned long i, f;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(s);
        প্রতিজ্ঞা(ret);

        d = strchr(s, '.');
        যদি (!d)
                ফেরত -EINVAL;

        i_str = strndupa_safe(s, d - s);
        f_str = d + 1;

        r = safe_atolu_full(i_str, 10, &i);
        যদি (r < 0)
                ফেরত r;

        r = safe_atolu_full(f_str, 10, &f);
        যদি (r < 0)
                ফেরত r;

        ফেরত store_loadavg_fixed_point(i, f, ret);
}

/* Limitations are described in https://www.netfilter.org/projects/nftables/manpage.html and
 * https://bugzilla.netfilter.org/show_bug.cgi?id=1175 */
bool nft_identifier_valid(ধ্রুবক অক্ষর *id) {
        যদি (isempty(id))
                ফেরত false;

        যদি (strlen(id) >= NFT_NAME_MAXLEN)
                ফেরত false;

        যদি (!ascii_isalpha(id[0]))
                ফেরত false;

        ফেরত in_charset(id + 1, ALPHANUMERICAL "/\\_.");
}

পূর্ণসংখ্যা parse_capability_set(ধ্রুবক অক্ষর *s, uint64_t initial, uint64_t *current) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(s);
        প্রতিজ্ঞা(current);

        যদি (isempty(s)) {
                *current = CAP_MASK_UNSET;
                ফেরত 1;
        }

        bool invert = false;
        যদি (s[0] == '~') {
                invert = true;
                s++;
        }

        uint64_t parsed;
        r = capability_set_from_string(s, &parsed);
        যদি (r < 0)
                ফেরত r;

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

        ফেরত r;
}
