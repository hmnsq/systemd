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
        सचे (!v)
                पटिनिवत्त -EINVAL;

        सचे (STRCASE_IN_SET(v,
                           "1",
                           "yes",
                           "y",
                           "true",
                           "t",
                           "on"))
                पटिनिवत्त 1;

        सचे (STRCASE_IN_SET(v,
                           "0",
                           "no",
                           "n",
                           "false",
                           "f",
                           "off"))
                पटिनिवत्त 0;

        पटिनिवत्त -EINVAL;
}

पूर्णाङ्क parse_tristate_full(ध्रुव वर्ण *v, ध्रुव वर्ण *third, पूर्णाङ्क *ret) {
        पूर्णाङ्क r;

        सचे (isempty(v) || streq_ptr(v, third)) { /* Empty string is always taken as the third/invalid/auto state */
                सचे (ret)
                        *ret = -1;
        } अञ्ञथा {
                r = parse_boolean(v);
                सचे (r < 0)
                        पटिनिवत्त r;

                सचे (ret)
                        *ret = r;
        }

        पटिनिवत्त 0;
}

पूर्णाङ्क parse_pid(ध्रुव वर्ण *s, pid_t *ret) {
        unsigned long ul = 0;
        pid_t pid;
        पूर्णाङ्क r;

        प्रतिज्ञा(s);

        r = safe_atolu(s, &ul);
        सचे (r < 0)
                पटिनिवत्त r;

        pid = (pid_t) ul;

        सचे ((unsigned long) pid != ul)
                पटिनिवत्त -ERANGE;

        सचे (!pid_is_valid(pid))
                पटिनिवत्त -ERANGE;

        सचे (ret)
                *ret = pid;
        पटिनिवत्त 0;
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
        सचे (r < 0)
                पटिनिवत्त r;
        सचे (m > 07777)
                पटिनिवत्त -ERANGE;

        सचे (ret)
                *ret = m;
        पटिनिवत्त 0;
}

पूर्णाङ्क parse_ifindex(ध्रुव वर्ण *s) {
        पूर्णाङ्क ifi, r;

        प्रतिज्ञा(s);

        r = safe_atoi(s, &ifi);
        सचे (r < 0)
                पटिनिवत्त r;
        सचे (ifi <= 0)
                पटिनिवत्त -EINVAL;

        पटिनिवत्त ifi;
}

पूर्णाङ्क parse_mtu(पूर्णाङ्क family, ध्रुव वर्ण *s, uint32_t *ret) {
        uint64_t u, m;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        r = parse_size(s, 1024, &u);
        सचे (r < 0)
                पटिनिवत्त r;

        सचे (u > UINT32_MAX)
                पटिनिवत्त -ERANGE;

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

        सचे (u < m)
                पटिनिवत्त -ERANGE;

        *ret = (uint32_t) u;
        पटिनिवत्त 0;
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

        सचे (base == 1000) {
                table = si;
                n_entries = ELEMENTSOF(si);
        } अञ्ञथा {
                table = iec;
                n_entries = ELEMENTSOF(iec);
        }

        p = t;
        करोहि {
                unsigned long long l, tmp;
                द्विगुण frac = 0;
                वर्ण *e;
                unsigned i;

                p += strspn(p, WHITESPACE);

                errno = 0;
                l = strtoull(p, &e, 10);
                सचे (errno > 0)
                        पटिनिवत्त -errno;
                सचे (e == p)
                        पटिनिवत्त -EINVAL;
                सचे (*p == '-')
                        पटिनिवत्त -ERANGE;

                सचे (*e == '.') {
                        e++;

                        /* strtoull() itself would accept space/+/- */
                        सचे (ascii_isdigit(*e)) {
                                unsigned long long l2;
                                वर्ण *e2;

                                l2 = strtoull(e, &e2, 10);
                                सचे (errno > 0)
                                        पटिनिवत्त -errno;

                                /* Ignore failure. E.g. 10.M is valid */
                                frac = l2;
                                अत्थाय (; e < e2; e++)
                                        frac /= 10;
                        }
                }

                e += strspn(e, WHITESPACE);

                अत्थाय (i = start_pos; i < n_entries; i++)
                        सचे (startswith(e, table[i].suffix))
                                भङ्ग;

                सचे (i >= n_entries)
                        पटिनिवत्त -EINVAL;

                सचे (l + (frac > 0) > ULLONG_MAX / table[i].factor)
                        पटिनिवत्त -ERANGE;

                tmp = l * table[i].factor + (unsigned long long) (frac * table[i].factor);
                सचे (tmp > ULLONG_MAX - r)
                        पटिनिवत्त -ERANGE;

                r += tmp;
                सचे ((unsigned long long) (uint64_t) r != r)
                        पटिनिवत्त -ERANGE;

                p = e + strlen(table[i].suffix);

                start_pos = i + 1;

        } याव (*p);

        *size = r;

        पटिनिवत्त 0;
}

पूर्णाङ्क parse_sector_size(ध्रुव वर्ण *t, uint64_t *ret) {
        पूर्णाङ्क r;

        प्रतिज्ञा(t);
        प्रतिज्ञा(ret);

        uint64_t ss;

        r = safe_atou64(t, &ss);
        सचे (r < 0)
                पटिनिवत्त log_error_errno(r, "Failed to parse sector size parameter %s", t);
        सचे (ss < 512 || ss > 4096) /* Allow up to 4K due to dm-crypt support and 4K alignment by the homed LUKS backend */
                पटिनिवत्त log_error_errno(SYNTHETIC_ERRNO(ERANGE), "Sector size not between 512 and 4096: %s", t);
        सचे (!ISPOWEROF2(ss))
                पटिनिवत्त log_error_errno(SYNTHETIC_ERRNO(EINVAL), "Sector size not power of 2: %s", t);

        *ret = ss;
        पटिनिवत्त 0;
}

पूर्णाङ्क parse_range(ध्रुव वर्ण *t, unsigned *lower, unsigned *upper) {
        _cleanup_free_ वर्ण *word = रिक्त;
        unsigned l, u;
        पूर्णाङ्क r;

        प्रतिज्ञा(lower);
        प्रतिज्ञा(upper);

        /* Extract the lower bound. */
        r = extract_first_word(&t, &word, "-", EXTRACT_DONT_COALESCE_SEPARATORS);
        सचे (r < 0)
                पटिनिवत्त r;
        सचे (r == 0)
                पटिनिवत्त -EINVAL;

        r = safe_atou(word, &l);
        सचे (r < 0)
                पटिनिवत्त r;

        /* Check for the upper bound and extract it if needed */
        सचे (!t)
                /* Single number with no dashes. */
                u = l;
        अञ्ञथा सचे (!*t)
                /* Trailing dash is an error. */
                पटिनिवत्त -EINVAL;
        अञ्ञथा {
                r = safe_atou(t, &u);
                सचे (r < 0)
                        पटिनिवत्त r;
        }

        *lower = l;
        *upper = u;
        पटिनिवत्त 0;
}

पूर्णाङ्क parse_errno(ध्रुव वर्ण *t) {
        पूर्णाङ्क r, e;

        प्रतिज्ञा(t);

        r = errno_from_name(t);
        सचे (r > 0)
                पटिनिवत्त r;

        r = safe_atoi(t, &e);
        सचे (r < 0)
                पटिनिवत्त r;

        /* 0 is also allowed here */
        सचे (!errno_is_valid(e) && e != 0)
                पटिनिवत्त -ERANGE;

        पटिनिवत्त e;
}

पूर्णाङ्क parse_fd(ध्रुव वर्ण *t) {
        पूर्णाङ्क r, fd;

        प्रतिज्ञा(t);

        r = safe_atoi(t, &fd);
        सचे (r < 0)
                पटिनिवत्त r;

        सचे (fd < 0)
                पटिनिवत्त -EBADF;

        पटिनिवत्त fd;
}

पूर्णाङ्क parse_user_shell(ध्रुव वर्ण *s, वर्ण **ret_sh, bool *ret_copy) {
        वर्ण *sh;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret_sh);
        प्रतिज्ञा(ret_copy);

        सचे (path_is_absolute(s) && path_is_normalized(s)) {
                sh = strdup(s);
                सचे (!sh)
                        पटिनिवत्त -ENOMEM;

                *ret_sh = sh;
                *ret_copy = false;
        } अञ्ञथा {
                r = parse_boolean(s);
                सचे (r < 0)
                        पटिनिवत्त r;

                *ret_sh = रिक्त;
                *ret_copy = r;
        }

        पटिनिवत्त 0;
}

स्थिर ध्रुव वर्ण *mangle_base(ध्रुव वर्ण *s, unsigned *base) {
        ध्रुव वर्ण *k;

        प्रतिज्ञा(s);
        प्रतिज्ञा(base);

        /* Base already explicitly specified, then don't do anything. */
        सचे (SAFE_ATO_MASK_FLAGS(*base) != 0)
                पटिनिवत्त s;

        /* Support Python 3 style "0b" and 0x" prefixes, because they truly make sense, much more than C's "0" prefix for octal. */
        k = STARTSWITH_SET(s, "0b", "0B");
        सचे (k) {
                *base = 2 | (*base & SAFE_ATO_ALL_FLAGS);
                पटिनिवत्त k;
        }

        k = STARTSWITH_SET(s, "0o", "0O");
        सचे (k) {
                *base = 8 | (*base & SAFE_ATO_ALL_FLAGS);
                पटिनिवत्त k;
        }

        पटिनिवत्त s;
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

        सचे (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_WHITESPACE) &&
            strchr(WHITESPACE, s[0]))
                पटिनिवत्त -EINVAL;

        s += strspn(s, WHITESPACE);

        सचे (FLAGS_SET(base, SAFE_ATO_REFUSE_PLUS_MINUS) &&
            IN_SET(s[0], '+', '-'))
                पटिनिवत्त -EINVAL; /* Note that we check the "-" prefix again a second time below, but return a
                                 * different error. I.e. if the SAFE_ATO_REFUSE_PLUS_MINUS flag is set we
                                 * blanket refuse +/- prefixed integers, while if it is missing we'll just
                                 * return ERANGE, because the string actually parses correctly, but doesn't
                                 * fit in the return type. */

        सचे (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_ZERO) &&
            s[0] == '0' && !streq(s, "0"))
                पटिनिवत्त -EINVAL; /* This is particularly useful to avoid ambiguities between C's octal
                                 * notation and assumed-to-be-decimal integers with a leading zero. */

        s = mangle_base(s, &base);

        errno = 0;
        l = strtoul(s, &x, SAFE_ATO_MASK_FLAGS(base) /* Let's mask off the flags bits so that only the actual
                                                      * base is left */);
        सचे (errno > 0)
                पटिनिवत्त -errno;
        सचे (!x || x == s || *x != 0)
                पटिनिवत्त -EINVAL;
        सचे (l != 0 && s[0] == '-')
                पटिनिवत्त -ERANGE;
        सचे ((unsigned long) (unsigned) l != l)
                पटिनिवत्त -ERANGE;

        सचे (ret_u)
                *ret_u = (unsigned) l;

        पटिनिवत्त 0;
}

पूर्णाङ्क safe_atou_bounded(ध्रुव वर्ण *s, unsigned min, unsigned max, unsigned *ret) {
        unsigned v;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        r = safe_atou(s, &v);
        सचे (r < 0)
                पटिनिवत्त r;

        सचे (v < min || v > max)
                पटिनिवत्त -ERANGE;

        *ret = v;
        पटिनिवत्त 0;
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
        सचे (errno > 0)
                पटिनिवत्त -errno;
        सचे (!x || x == s || *x != 0)
                पटिनिवत्त -EINVAL;
        सचे ((long) (पूर्णाङ्क) l != l)
                पटिनिवत्त -ERANGE;

        सचे (ret_i)
                *ret_i = (पूर्णाङ्क) l;

        पटिनिवत्त 0;
}

पूर्णाङ्क safe_atollu_full(ध्रुव वर्ण *s, unsigned base, unsigned long long *ret_llu) {
        वर्ण *x = रिक्त;
        unsigned long long l;

        प्रतिज्ञा(s);
        प्रतिज्ञा(SAFE_ATO_MASK_FLAGS(base) <= 16);

        सचे (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_WHITESPACE) &&
            strchr(WHITESPACE, s[0]))
                पटिनिवत्त -EINVAL;

        s += strspn(s, WHITESPACE);

        सचे (FLAGS_SET(base, SAFE_ATO_REFUSE_PLUS_MINUS) &&
            IN_SET(s[0], '+', '-'))
                पटिनिवत्त -EINVAL;

        सचे (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_ZERO) &&
            s[0] == '0' && s[1] != 0)
                पटिनिवत्त -EINVAL;

        s = mangle_base(s, &base);

        errno = 0;
        l = strtoull(s, &x, SAFE_ATO_MASK_FLAGS(base));
        सचे (errno > 0)
                पटिनिवत्त -errno;
        सचे (!x || x == s || *x != 0)
                पटिनिवत्त -EINVAL;
        सचे (l != 0 && s[0] == '-')
                पटिनिवत्त -ERANGE;

        सचे (ret_llu)
                *ret_llu = l;

        पटिनिवत्त 0;
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
        सचे (errno > 0)
                पटिनिवत्त -errno;
        सचे (!x || x == s || *x != 0)
                पटिनिवत्त -EINVAL;

        सचे (ret_lli)
                *ret_lli = l;

        पटिनिवत्त 0;
}

पूर्णाङ्क safe_atou8_full(ध्रुव वर्ण *s, unsigned base, uint8_t *ret) {
        unsigned u;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        r = safe_atou_full(s, base, &u);
        सचे (r < 0)
                पटिनिवत्त r;
        सचे (u > UINT8_MAX)
                पटिनिवत्त -ERANGE;

        *ret = (uint8_t) u;
        पटिनिवत्त 0;
}

पूर्णाङ्क safe_atou16_full(ध्रुव वर्ण *s, unsigned base, uint16_t *ret) {
        unsigned u;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        r = safe_atou_full(s, base, &u);
        सचे (r < 0)
                पटिनिवत्त r;
        सचे (u > UINT16_MAX)
                पटिनिवत्त -ERANGE;

        *ret = (uint16_t) u;
        पटिनिवत्त 0;
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
        सचे (errno > 0)
                पटिनिवत्त -errno;
        सचे (!x || x == s || *x != 0)
                पटिनिवत्त -EINVAL;
        सचे ((long) (int16_t) l != l)
                पटिनिवत्त -ERANGE;

        सचे (ret)
                *ret = (int16_t) l;

        पटिनिवत्त 0;
}

पूर्णाङ्क safe_atod(ध्रुव वर्ण *s, द्विगुण *ret_d) {
        _cleanup_(freelocalep) locale_t loc = (locale_t) 0;
        वर्ण *x = रिक्त;
        द्विगुण d = 0;

        प्रतिज्ञा(s);

        loc = newlocale(LC_NUMERIC_MASK, "C", (locale_t) 0);
        सचे (loc == (locale_t) 0)
                पटिनिवत्त -errno;

        errno = 0;
        d = strtod_l(s, &x, loc);
        सचे (errno > 0)
                पटिनिवत्त -errno;
        सचे (!x || x == s || *x != 0)
                पटिनिवत्त -EINVAL;

        सचे (ret_d)
                *ret_d = d;

        पटिनिवत्त 0;
}

पूर्णाङ्क parse_fractional_part_u(ध्रुव वर्ण **p, size_t digits, unsigned *res) {
        unsigned val = 0;
        ध्रुव वर्ण *s;

        प्रतिज्ञा(p);
        प्रतिज्ञा(res);

        s = *p;

        /* accept any number of digits, strtoull is limited to 19 */
        अत्थाय (size_t i = 0; i < digits; i++, s++) {
                सचे (!ascii_isdigit(*s)) {
                        सचे (i == 0)
                                पटिनिवत्त -EINVAL;

                        /* too few digits, pad with 0 */
                        अत्थाय (; i < digits; i++)
                                val *= 10;

                        भङ्ग;
                }

                val *= 10;
                val += *s - '0';
        }

        /* maybe round up */
        सचे (*s >= '5' && *s <= '9')
                val++;

        s += strspn(s, DIGITS);

        *p = s;
        *res = val;

        पटिनिवत्त 0;
}

पूर्णाङ्क parse_nice(ध्रुव वर्ण *s, पूर्णाङ्क *ret) {
        पूर्णाङ्क n, r;

        प्रतिज्ञा(ret);

        r = safe_atoi(s, &n);
        सचे (r < 0)
                पटिनिवत्त r;

        सचे (!nice_is_valid(n))
                पटिनिवत्त -ERANGE;

        *ret = n;
        पटिनिवत्त 0;
}

पूर्णाङ्क parse_ip_port(ध्रुव वर्ण *s, uint16_t *ret) {
        uint16_t l;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        r = safe_atou16_full(s, SAFE_ATO_REFUSE_LEADING_WHITESPACE, &l);
        सचे (r < 0)
                पटिनिवत्त r;

        सचे (l == 0)
                पटिनिवत्त -EINVAL;

        *ret = l;

        पटिनिवत्त 0;
}

पूर्णाङ्क parse_ip_port_range(ध्रुव वर्ण *s, uint16_t *low, uint16_t *high, bool allow_zero) {
        unsigned l, h;
        पूर्णाङ्क r;

        प्रतिज्ञा(low);
        प्रतिज्ञा(high);

        r = parse_range(s, &l, &h);
        सचे (r < 0)
                पटिनिवत्त r;

        सचे (l > 65535 || h > 65535)
                पटिनिवत्त -EINVAL;

        सचे (!allow_zero && (l == 0 || h == 0))
                पटिनिवत्त -EINVAL;

        सचे (h < l)
                पटिनिवत्त -EINVAL;

        *low = l;
        *high = h;

        पटिनिवत्त 0;
}

पूर्णाङ्क parse_oom_score_adjust(ध्रुव वर्ण *s, पूर्णाङ्क *ret) {
        पूर्णाङ्क r, v;

        प्रतिज्ञा(s);
        प्रतिज्ञा(ret);

        r = safe_atoi(s, &v);
        सचे (r < 0)
                पटिनिवत्त r;

        सचे (!oom_score_adjust_is_valid(v))
                पटिनिवत्त -ERANGE;

        *ret = v;
        पटिनिवत्त 0;
}

पूर्णाङ्क store_loadavg_fixed_point(unsigned long i, unsigned long f, loadavg_t *ret) {
        प्रतिज्ञा(ret);

        /* Validate both parts *before* shifting them, so that the shifts themselves cannot overflow.
         * Rejecting f >= 100 is equivalent to the old check of the shifted value against
         * LOADAVG_FIXED_POINT_1_0, since DIV_ROUND_UP(99 << 11, 100) == 2028 and
         * DIV_ROUND_UP(100 << 11, 100) == 2048. */
        सचे (i > (~0UL >> LOADAVG_PRECISION_BITS))
                पटिनिवत्त -ERANGE;

        सचे (f >= 100)
                पटिनिवत्त -ERANGE;

        *ret = (i << LOADAVG_PRECISION_BITS) | DIV_ROUND_UP(f << LOADAVG_PRECISION_BITS, 100);
        पटिनिवत्त 0;
}

पूर्णाङ्क parse_loadavg_fixed_point(ध्रुव वर्ण *s, loadavg_t *ret) {
        ध्रुव वर्ण *d, *f_str, *i_str;
        unsigned long i, f;
        पूर्णाङ्क r;

        प्रतिज्ञा(s);
        प्रतिज्ञा(ret);

        d = strchr(s, '.');
        सचे (!d)
                पटिनिवत्त -EINVAL;

        i_str = strndupa_safe(s, d - s);
        f_str = d + 1;

        r = safe_atolu_full(i_str, 10, &i);
        सचे (r < 0)
                पटिनिवत्त r;

        r = safe_atolu_full(f_str, 10, &f);
        सचे (r < 0)
                पटिनिवत्त r;

        पटिनिवत्त store_loadavg_fixed_point(i, f, ret);
}

/* Limitations are described in https://www.netfilter.org/projects/nftables/manpage.html and
 * https://bugzilla.netfilter.org/show_bug.cgi?id=1175 */
bool nft_identifier_valid(ध्रुव वर्ण *id) {
        सचे (isempty(id))
                पटिनिवत्त false;

        सचे (strlen(id) >= NFT_NAME_MAXLEN)
                पटिनिवत्त false;

        सचे (!ascii_isalpha(id[0]))
                पटिनिवत्त false;

        पटिनिवत्त in_charset(id + 1, ALPHANUMERICAL "/\\_.");
}

पूर्णाङ्क parse_capability_set(ध्रुव वर्ण *s, uint64_t initial, uint64_t *current) {
        पूर्णाङ्क r;

        प्रतिज्ञा(s);
        प्रतिज्ञा(current);

        सचे (isempty(s)) {
                *current = CAP_MASK_UNSET;
                पटिनिवत्त 1;
        }

        bool invert = false;
        सचे (s[0] == '~') {
                invert = true;
                s++;
        }

        uint64_t parsed;
        r = capability_set_from_string(s, &parsed);
        सचे (r < 0)
                पटिनिवत्त r;

        सचे (parsed == 0 || *current == initial)
                /* "~" or uninitialized data -> replace */
                *current = invert ? all_capabilities() & ~parsed : parsed;
        अञ्ञथा {
                /* previous data -> merge */
                सचे (invert)
                        *current &= ~parsed;
                अञ्ञथा
                        *current |= parsed;
        }

        पटिनिवत्त r;
}
