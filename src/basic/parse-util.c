/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <linux/ipv6.h>
#अन्तर्भाव <linux/netfilter/nf_tables.h>
#अन्तर्भाव <stdio.h>
#अन्तर्भाव <stdlib.h>
#अन्तर्भाव <sys/socket.h>

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "capability-list.h"
#अन्तर्भाव "capability-util.h"
#अन्तर्भाव "errno-list.h"
#अन्तर्भाव "extract-word.h"
#अन्तर्भाव "locale-util.h"
#अन्तर्भाव "log.h"
#अन्तर्भाव "missing-network.h"
#अन्तर्भाव "parse-util.h"
#अन्तर्भाव "path-util.h"
#अन्तर्भाव "process-util.h"
#अन्तर्भाव "string-util.h"
#अन्तर्भाव "strv.h"

पूर्णाङ्क parse_boolean(ध्रुव वर्ण *v) {
        यदि (!v)
                प्रत्यागम -EINVAL;

        यदि (STRCASE_IN_SET(v,
                           "1",
                           "yes",
                           "y",
                           "true",
                           "t",
                           "on"))
                प्रत्यागम 1;

        यदि (STRCASE_IN_SET(v,
                           "0",
                           "no",
                           "n",
                           "false",
                           "f",
                           "off"))
                प्रत्यागम 0;

        प्रत्यागम -EINVAL;
}

पूर्णाङ्क parse_tristate_full(ध्रुव वर्ण *v, ध्रुव वर्ण *third, पूर्णाङ्क *ret) {
        पूर्णाङ्क r;

        यदि (isempty(v) || streq_ptr(v, third)) { /* Empty string is always taken as the third/invalid/auto state */
                यदि (ret)
                        *ret = -1;
        } अन्यथा {
                r = parse_boolean(v);
                यदि (r < 0)
                        प्रत्यागम r;

                यदि (ret)
                        *ret = r;
        }

        प्रत्यागम 0;
}

पूर्णाङ्क parse_pid(ध्रुव वर्ण *s, pid_t *ret) {
        unsigned long ul = 0;
        pid_t pid;
        पूर्णाङ्क r;

        प्रतिज्ञा(s);

        r = safe_atolu(s, &ul);
        यदि (r < 0)
                प्रत्यागम r;

        pid = (pid_t) ul;

        यदि ((unsigned long) pid != ul)
                प्रत्यागम -ERANGE;

        यदि (!pid_is_valid(pid))
                प्रत्यागम -ERANGE;

        यदि (ret)
                *ret = pid;
        प्रत्यागम 0;
}

पूर्णाङ्क parse_mode(ध्रुव वर्ण *s, mode_t *ret) {
        unsigned m;
        पूर्णाङ्क r;

        प्रतिज्ञा(s);

        r = safe_atou_full(s, 8 |
                           SAFE_ATO_REFUSE_PLUS_MINUS, /* Leading '+' or even '-' char? that's just weird,
                                                        * refuse. User might have wanted to add mode flags or
                                                        * so, but this parser doesn't allow that, so let's
                                                        * better be safe. */
                           &m);
        यदि (r < 0)
                प्रत्यागम r;
        यदि (m > 07777)
                प्रत्यागम -ERANGE;

        यदि (ret)
                *ret = m;
        प्रत्यागम 0;
}

पूर्णाङ्क parse_ifindex(ध्रुव वर्ण *s) {
        पूर्णाङ्क ifi, r;

        प्रतिज्ञा(s);

        r = safe_atoi(s, &ifi);
        यदि (r < 0)
                प्रत्यागम r;
        यदि (ifi <= 0)
                प्रत्यागम -EINVAL;

        प्रत्यागम ifi;
}

पूर्णाङ्क parse_mtu(पूर्णाङ्क family, ध्रुव वर्ण *s, uint32_t *ret) {
        uint64_t u, m;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        r = parse_size(s, 1024, &u);
        यदि (r < 0)
                प्रत्यागम r;

        यदि (u > UINT32_MAX)
                प्रत्यागम -ERANGE;

        विकल्प (family) {
        पक्ष AF_INET:
                m = IPV4_MIN_MTU; /* This is 68 */
                भङ्ग;
        पक्ष AF_INET6:
                m = IPV6_MIN_MTU; /* This is 1280 */
                भङ्ग;
        स्वभाव:
                m = 0;
        }

        यदि (u < m)
                प्रत्यागम -ERANGE;

        *ret = (uint32_t) u;
        प्रत्यागम 0;
}

पूर्णाङ्क parse_size(ध्रुव वर्ण *t, uint64_t base, uint64_t *size) {

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

        संरचना table {
                ध्रुव वर्ण *suffix;
                unsigned long long factor;
        };

        स्थिर ध्रुव संरचना table iec[] = {
                { "E", 1024ULL*1024ULL*1024ULL*1024ULL*1024ULL*1024ULL },
                { "P", 1024ULL*1024ULL*1024ULL*1024ULL*1024ULL },
                { "T", 1024ULL*1024ULL*1024ULL*1024ULL },
                { "G", 1024ULL*1024ULL*1024ULL },
                { "M", 1024ULL*1024ULL },
                { "K", 1024ULL },
                { "B", 1ULL },
                { "",  1ULL },
        };

        स्थिर ध्रुव संरचना table si[] = {
                { "E", 1000ULL*1000ULL*1000ULL*1000ULL*1000ULL*1000ULL },
                { "P", 1000ULL*1000ULL*1000ULL*1000ULL*1000ULL },
                { "T", 1000ULL*1000ULL*1000ULL*1000ULL },
                { "G", 1000ULL*1000ULL*1000ULL },
                { "M", 1000ULL*1000ULL },
                { "K", 1000ULL },
                { "B", 1ULL },
                { "",  1ULL },
        };

        ध्रुव संरचना table *table;
        ध्रुव वर्ण *p;
        unsigned long long r = 0;
        unsigned n_entries, start_pos = 0;

        प्रतिज्ञा(t);
        प्रतिज्ञा(IN_SET(base, 1000, 1024));
        प्रतिज्ञा(size);

        यदि (base == 1000) {
                table = si;
                n_entries = ELEMENTSOF(si);
        } अन्यथा {
                table = iec;
                n_entries = ELEMENTSOF(iec);
        }

        p = t;
        कुरु {
                unsigned long long l, tmp;
                द्विगुण frac = 0;
                वर्ण *e;
                unsigned i;

                p += strspn(p, WHITESPACE);

                errno = 0;
                l = strtoull(p, &e, 10);
                यदि (errno > 0)
                        प्रत्यागम -errno;
                यदि (e == p)
                        प्रत्यागम -EINVAL;
                यदि (*p == '-')
                        प्रत्यागम -ERANGE;

                यदि (*e == '.') {
                        e++;

                        /* strtoull() itself would accept space/+/- */
                        यदि (ascii_isdigit(*e)) {
                                unsigned long long l2;
                                वर्ण *e2;

                                l2 = strtoull(e, &e2, 10);
                                यदि (errno > 0)
                                        प्रत्यागम -errno;

                                /* Ignore failure. E.g. 10.M is valid */
                                frac = l2;
                                प्रति (; e < e2; e++)
                                        frac /= 10;
                        }
                }

                e += strspn(e, WHITESPACE);

                प्रति (i = start_pos; i < n_entries; i++)
                        यदि (startswith(e, table[i].suffix))
                                भङ्ग;

                यदि (i >= n_entries)
                        प्रत्यागम -EINVAL;

                यदि (l + (frac > 0) > ULLONG_MAX / table[i].factor)
                        प्रत्यागम -ERANGE;

                tmp = l * table[i].factor + (unsigned long long) (frac * table[i].factor);
                यदि (tmp > ULLONG_MAX - r)
                        प्रत्यागम -ERANGE;

                r += tmp;
                यदि ((unsigned long long) (uint64_t) r != r)
                        प्रत्यागम -ERANGE;

                p = e + strlen(table[i].suffix);

                start_pos = i + 1;

        } यावत् (*p);

        *size = r;

        प्रत्यागम 0;
}

पूर्णाङ्क parse_sector_size(ध्रुव वर्ण *t, uint64_t *ret) {
        पूर्णाङ्क r;

        प्रतिज्ञा(t);
        प्रतिज्ञा(ret);

        uint64_t ss;

        r = safe_atou64(t, &ss);
        यदि (r < 0)
                प्रत्यागम log_error_errno(r, "Failed to parse sector size parameter %s", t);
        यदि (ss < 512 || ss > 4096) /* Allow up to 4K due to dm-crypt support and 4K alignment by the homed LUKS backend */
                प्रत्यागम log_error_errno(SYNTHETIC_ERRNO(ERANGE), "Sector size not between 512 and 4096: %s", t);
        यदि (!ISPOWEROF2(ss))
                प्रत्यागम log_error_errno(SYNTHETIC_ERRNO(EINVAL), "Sector size not power of 2: %s", t);

        *ret = ss;
        प्रत्यागम 0;
}

पूर्णाङ्क parse_range(ध्रुव वर्ण *t, unsigned *lower, unsigned *upper) {
        _cleanup_free_ वर्ण *word = रिक्त;
        unsigned l, u;
        पूर्णाङ्क r;

        प्रतिज्ञा(lower);
        प्रतिज्ञा(upper);

        /* Extract the lower bound. */
        r = extract_first_word(&t, &word, "-", EXTRACT_DONT_COALESCE_SEPARATORS);
        यदि (r < 0)
                प्रत्यागम r;
        यदि (r == 0)
                प्रत्यागम -EINVAL;

        r = safe_atou(word, &l);
        यदि (r < 0)
                प्रत्यागम r;

        /* Check for the upper bound and extract it if needed */
        यदि (!t)
                /* Single number with no dashes. */
                u = l;
        अन्यथा यदि (!*t)
                /* Trailing dash is an error. */
                प्रत्यागम -EINVAL;
        अन्यथा {
                r = safe_atou(t, &u);
                यदि (r < 0)
                        प्रत्यागम r;
        }

        *lower = l;
        *upper = u;
        प्रत्यागम 0;
}

पूर्णाङ्क parse_errno(ध्रुव वर्ण *t) {
        पूर्णाङ्क r, e;

        प्रतिज्ञा(t);

        r = errno_from_name(t);
        यदि (r > 0)
                प्रत्यागम r;

        r = safe_atoi(t, &e);
        यदि (r < 0)
                प्रत्यागम r;

        /* 0 is also allowed here */
        यदि (!errno_is_valid(e) && e != 0)
                प्रत्यागम -ERANGE;

        प्रत्यागम e;
}

पूर्णाङ्क parse_fd(ध्रुव वर्ण *t) {
        पूर्णाङ्क r, fd;

        प्रतिज्ञा(t);

        r = safe_atoi(t, &fd);
        यदि (r < 0)
                प्रत्यागम r;

        यदि (fd < 0)
                प्रत्यागम -EBADF;

        प्रत्यागम fd;
}

पूर्णाङ्क parse_user_shell(ध्रुव वर्ण *s, वर्ण **ret_sh, bool *ret_copy) {
        वर्ण *sh;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret_sh);
        प्रतिज्ञा(ret_copy);

        यदि (path_is_absolute(s) && path_is_normalized(s)) {
                sh = strdup(s);
                यदि (!sh)
                        प्रत्यागम -ENOMEM;

                *ret_sh = sh;
                *ret_copy = false;
        } अन्यथा {
                r = parse_boolean(s);
                यदि (r < 0)
                        प्रत्यागम r;

                *ret_sh = रिक्त;
                *ret_copy = r;
        }

        प्रत्यागम 0;
}

स्थिर ध्रुव वर्ण *mangle_base(ध्रुव वर्ण *s, unsigned *base) {
        ध्रुव वर्ण *k;

        प्रतिज्ञा(s);
        प्रतिज्ञा(base);

        /* Base already explicitly specified, then don't do anything. */
        यदि (SAFE_ATO_MASK_FLAGS(*base) != 0)
                प्रत्यागम s;

        /* Support Python 3 style "0b" and 0x" prefixes, because they truly make sense, much more than C's "0" prefix for octal. */
        k = STARTSWITH_SET(s, "0b", "0B");
        यदि (k) {
                *base = 2 | (*base & SAFE_ATO_ALL_FLAGS);
                प्रत्यागम k;
        }

        k = STARTSWITH_SET(s, "0o", "0O");
        यदि (k) {
                *base = 8 | (*base & SAFE_ATO_ALL_FLAGS);
                प्रत्यागम k;
        }

        प्रत्यागम s;
}

पूर्णाङ्क safe_atou_full(ध्रुव वर्ण *s, unsigned base, unsigned *ret_u) {
        वर्ण *x = रिक्त;
        unsigned long l;

        प्रतिज्ञा(s);
        प्रतिज्ञा(SAFE_ATO_MASK_FLAGS(base) <= 16);

        /* strtoul() is happy to parse negative values, and silently converts them to unsigned values without
         * generating an error. We want a clean error, hence let's look for the "-" prefix on our own, and
         * generate an error. But let's do so only after strtoul() validated that the string is clean
         * otherwise, so that we return EINVAL preferably over ERANGE. */

        यदि (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_WHITESPACE) &&
            strchr(WHITESPACE, s[0]))
                प्रत्यागम -EINVAL;

        s += strspn(s, WHITESPACE);

        यदि (FLAGS_SET(base, SAFE_ATO_REFUSE_PLUS_MINUS) &&
            IN_SET(s[0], '+', '-'))
                प्रत्यागम -EINVAL; /* Note that we check the "-" prefix again a second time below, but return a
                                 * different error. I.e. if the SAFE_ATO_REFUSE_PLUS_MINUS flag is set we
                                 * blanket refuse +/- prefixed integers, while if it is missing we'll just
                                 * return ERANGE, because the string actually parses correctly, but doesn't
                                 * fit in the return type. */

        यदि (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_ZERO) &&
            s[0] == '0' && !streq(s, "0"))
                प्रत्यागम -EINVAL; /* This is particularly useful to avoid ambiguities between C's octal
                                 * notation and assumed-to-be-decimal integers with a leading zero. */

        s = mangle_base(s, &base);

        errno = 0;
        l = strtoul(s, &x, SAFE_ATO_MASK_FLAGS(base) /* Let's mask off the flags bits so that only the actual
                                                      * base is left */);
        यदि (errno > 0)
                प्रत्यागम -errno;
        यदि (!x || x == s || *x != 0)
                प्रत्यागम -EINVAL;
        यदि (l != 0 && s[0] == '-')
                प्रत्यागम -ERANGE;
        यदि ((unsigned long) (unsigned) l != l)
                प्रत्यागम -ERANGE;

        यदि (ret_u)
                *ret_u = (unsigned) l;

        प्रत्यागम 0;
}

पूर्णाङ्क safe_atou_bounded(ध्रुव वर्ण *s, unsigned min, unsigned max, unsigned *ret) {
        unsigned v;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        r = safe_atou(s, &v);
        यदि (r < 0)
                प्रत्यागम r;

        यदि (v < min || v > max)
                प्रत्यागम -ERANGE;

        *ret = v;
        प्रत्यागम 0;
}

पूर्णाङ्क safe_atoi(ध्रुव वर्ण *s, पूर्णाङ्क *ret_i) {
        unsigned base = 0;
        वर्ण *x = रिक्त;
        long l;

        प्रतिज्ञा(s);

        s += strspn(s, WHITESPACE);
        s = mangle_base(s, &base);

        errno = 0;
        l = strtol(s, &x, base);
        यदि (errno > 0)
                प्रत्यागम -errno;
        यदि (!x || x == s || *x != 0)
                प्रत्यागम -EINVAL;
        यदि ((long) (पूर्णाङ्क) l != l)
                प्रत्यागम -ERANGE;

        यदि (ret_i)
                *ret_i = (पूर्णाङ्क) l;

        प्रत्यागम 0;
}

पूर्णाङ्क safe_atollu_full(ध्रुव वर्ण *s, unsigned base, unsigned long long *ret_llu) {
        वर्ण *x = रिक्त;
        unsigned long long l;

        प्रतिज्ञा(s);
        प्रतिज्ञा(SAFE_ATO_MASK_FLAGS(base) <= 16);

        यदि (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_WHITESPACE) &&
            strchr(WHITESPACE, s[0]))
                प्रत्यागम -EINVAL;

        s += strspn(s, WHITESPACE);

        यदि (FLAGS_SET(base, SAFE_ATO_REFUSE_PLUS_MINUS) &&
            IN_SET(s[0], '+', '-'))
                प्रत्यागम -EINVAL;

        यदि (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_ZERO) &&
            s[0] == '0' && s[1] != 0)
                प्रत्यागम -EINVAL;

        s = mangle_base(s, &base);

        errno = 0;
        l = strtoull(s, &x, SAFE_ATO_MASK_FLAGS(base));
        यदि (errno > 0)
                प्रत्यागम -errno;
        यदि (!x || x == s || *x != 0)
                प्रत्यागम -EINVAL;
        यदि (l != 0 && s[0] == '-')
                प्रत्यागम -ERANGE;

        यदि (ret_llu)
                *ret_llu = l;

        प्रत्यागम 0;
}

पूर्णाङ्क safe_atolli(ध्रुव वर्ण *s, long long *ret_lli) {
        unsigned base = 0;
        वर्ण *x = रिक्त;
        long long l;

        प्रतिज्ञा(s);

        s += strspn(s, WHITESPACE);
        s = mangle_base(s, &base);

        errno = 0;
        l = strtoll(s, &x, base);
        यदि (errno > 0)
                प्रत्यागम -errno;
        यदि (!x || x == s || *x != 0)
                प्रत्यागम -EINVAL;

        यदि (ret_lli)
                *ret_lli = l;

        प्रत्यागम 0;
}

पूर्णाङ्क safe_atou8_full(ध्रुव वर्ण *s, unsigned base, uint8_t *ret) {
        unsigned u;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        r = safe_atou_full(s, base, &u);
        यदि (r < 0)
                प्रत्यागम r;
        यदि (u > UINT8_MAX)
                प्रत्यागम -ERANGE;

        *ret = (uint8_t) u;
        प्रत्यागम 0;
}

पूर्णाङ्क safe_atou16_full(ध्रुव वर्ण *s, unsigned base, uint16_t *ret) {
        unsigned u;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        r = safe_atou_full(s, base, &u);
        यदि (r < 0)
                प्रत्यागम r;
        यदि (u > UINT16_MAX)
                प्रत्यागम -ERANGE;

        *ret = (uint16_t) u;
        प्रत्यागम 0;
}

पूर्णाङ्क safe_atoi16(ध्रुव वर्ण *s, int16_t *ret) {
        unsigned base = 0;
        वर्ण *x = रिक्त;
        long l;

        प्रतिज्ञा(s);

        s += strspn(s, WHITESPACE);
        s = mangle_base(s, &base);

        errno = 0;
        l = strtol(s, &x, base);
        यदि (errno > 0)
                प्रत्यागम -errno;
        यदि (!x || x == s || *x != 0)
                प्रत्यागम -EINVAL;
        यदि ((long) (int16_t) l != l)
                प्रत्यागम -ERANGE;

        यदि (ret)
                *ret = (int16_t) l;

        प्रत्यागम 0;
}

पूर्णाङ्क safe_atod(ध्रुव वर्ण *s, द्विगुण *ret_d) {
        _cleanup_(freelocalep) locale_t loc = (locale_t) 0;
        वर्ण *x = रिक्त;
        द्विगुण d = 0;

        प्रतिज्ञा(s);

        loc = newlocale(LC_NUMERIC_MASK, "C", (locale_t) 0);
        यदि (loc == (locale_t) 0)
                प्रत्यागम -errno;

        errno = 0;
        d = strtod_l(s, &x, loc);
        यदि (errno > 0)
                प्रत्यागम -errno;
        यदि (!x || x == s || *x != 0)
                प्रत्यागम -EINVAL;

        यदि (ret_d)
                *ret_d = d;

        प्रत्यागम 0;
}

पूर्णाङ्क parse_fractional_part_u(ध्रुव वर्ण **p, size_t digits, unsigned *res) {
        unsigned val = 0;
        ध्रुव वर्ण *s;

        प्रतिज्ञा(p);
        प्रतिज्ञा(res);

        s = *p;

        /* accept any number of digits, strtoull is limited to 19 */
        प्रति (size_t i = 0; i < digits; i++, s++) {
                यदि (!ascii_isdigit(*s)) {
                        यदि (i == 0)
                                प्रत्यागम -EINVAL;

                        /* too few digits, pad with 0 */
                        प्रति (; i < digits; i++)
                                val *= 10;

                        भङ्ग;
                }

                val *= 10;
                val += *s - '0';
        }

        /* maybe round up */
        यदि (*s >= '5' && *s <= '9')
                val++;

        s += strspn(s, DIGITS);

        *p = s;
        *res = val;

        प्रत्यागम 0;
}

पूर्णाङ्क parse_nice(ध्रुव वर्ण *s, पूर्णाङ्क *ret) {
        पूर्णाङ्क n, r;

        प्रतिज्ञा(ret);

        r = safe_atoi(s, &n);
        यदि (r < 0)
                प्रत्यागम r;

        यदि (!nice_is_valid(n))
                प्रत्यागम -ERANGE;

        *ret = n;
        प्रत्यागम 0;
}

पूर्णाङ्क parse_ip_port(ध्रुव वर्ण *s, uint16_t *ret) {
        uint16_t l;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        r = safe_atou16_full(s, SAFE_ATO_REFUSE_LEADING_WHITESPACE, &l);
        यदि (r < 0)
                प्रत्यागम r;

        यदि (l == 0)
                प्रत्यागम -EINVAL;

        *ret = l;

        प्रत्यागम 0;
}

पूर्णाङ्क parse_ip_port_range(ध्रुव वर्ण *s, uint16_t *low, uint16_t *high, bool allow_zero) {
        unsigned l, h;
        पूर्णाङ्क r;

        प्रतिज्ञा(low);
        प्रतिज्ञा(high);

        r = parse_range(s, &l, &h);
        यदि (r < 0)
                प्रत्यागम r;

        यदि (l > 65535 || h > 65535)
                प्रत्यागम -EINVAL;

        यदि (!allow_zero && (l == 0 || h == 0))
                प्रत्यागम -EINVAL;

        यदि (h < l)
                प्रत्यागम -EINVAL;

        *low = l;
        *high = h;

        प्रत्यागम 0;
}

पूर्णाङ्क parse_oom_score_adjust(ध्रुव वर्ण *s, पूर्णाङ्क *ret) {
        पूर्णाङ्क r, v;

        प्रतिज्ञा(s);
        प्रतिज्ञा(ret);

        r = safe_atoi(s, &v);
        यदि (r < 0)
                प्रत्यागम r;

        यदि (!oom_score_adjust_is_valid(v))
                प्रत्यागम -ERANGE;

        *ret = v;
        प्रत्यागम 0;
}

पूर्णाङ्क store_loadavg_fixed_point(unsigned long i, unsigned long f, loadavg_t *ret) {
        प्रतिज्ञा(ret);

        /* Validate both parts *before* shifting them, so that the shifts themselves cannot overflow.
         * Rejecting f >= 100 is equivalent to the old check of the shifted value against
         * LOADAVG_FIXED_POINT_1_0, since DIV_ROUND_UP(99 << 11, 100) == 2028 and
         * DIV_ROUND_UP(100 << 11, 100) == 2048. */
        यदि (i > (~0UL >> LOADAVG_PRECISION_BITS))
                प्रत्यागम -ERANGE;

        यदि (f >= 100)
                प्रत्यागम -ERANGE;

        *ret = (i << LOADAVG_PRECISION_BITS) | DIV_ROUND_UP(f << LOADAVG_PRECISION_BITS, 100);
        प्रत्यागम 0;
}

पूर्णाङ्क parse_loadavg_fixed_point(ध्रुव वर्ण *s, loadavg_t *ret) {
        ध्रुव वर्ण *d, *f_str, *i_str;
        unsigned long i, f;
        पूर्णाङ्क r;

        प्रतिज्ञा(s);
        प्रतिज्ञा(ret);

        d = strchr(s, '.');
        यदि (!d)
                प्रत्यागम -EINVAL;

        i_str = strndupa_safe(s, d - s);
        f_str = d + 1;

        r = safe_atolu_full(i_str, 10, &i);
        यदि (r < 0)
                प्रत्यागम r;

        r = safe_atolu_full(f_str, 10, &f);
        यदि (r < 0)
                प्रत्यागम r;

        प्रत्यागम store_loadavg_fixed_point(i, f, ret);
}

/* Limitations are described in https://www.netfilter.org/projects/nftables/manpage.html and
 * https://bugzilla.netfilter.org/show_bug.cgi?id=1175 */
bool nft_identifier_valid(ध्रुव वर्ण *id) {
        यदि (isempty(id))
                प्रत्यागम false;

        यदि (strlen(id) >= NFT_NAME_MAXLEN)
                प्रत्यागम false;

        यदि (!ascii_isalpha(id[0]))
                प्रत्यागम false;

        प्रत्यागम in_charset(id + 1, ALPHANUMERICAL "/\\_.");
}

पूर्णाङ्क parse_capability_set(ध्रुव वर्ण *s, uint64_t initial, uint64_t *current) {
        पूर्णाङ्क r;

        प्रतिज्ञा(s);
        प्रतिज्ञा(current);

        यदि (isempty(s)) {
                *current = CAP_MASK_UNSET;
                प्रत्यागम 1;
        }

        bool invert = false;
        यदि (s[0] == '~') {
                invert = true;
                s++;
        }

        uint64_t parsed;
        r = capability_set_from_string(s, &parsed);
        यदि (r < 0)
                प्रत्यागम r;

        यदि (parsed == 0 || *current == initial)
                /* "~" or uninitialized data -> replace */
                *current = invert ? all_capabilities() & ~parsed : parsed;
        अन्यथा {
                /* previous data -> merge */
                यदि (invert)
                        *current &= ~parsed;
                अन्यथा
                        *current |= parsed;
        }

        प्रत्यागम r;
}
