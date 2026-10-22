/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ <linux/ipv6.h>
#અન્તર્ભાવ <linux/netfilter/nf_tables.h>
#અન્તર્ભાવ <stdio.h>
#અન્તર્ભાવ <stdlib.h>
#અન્તર્ભાવ <sys/socket.h>

#અન્તર્ભાવ "alloc-util.h"
#અન્તર્ભાવ "capability-list.h"
#અન્તર્ભાવ "capability-util.h"
#અન્તર્ભાવ "errno-list.h"
#અન્તર્ભાવ "extract-word.h"
#અન્તર્ભાવ "locale-util.h"
#અન્તર્ભાવ "log.h"
#અન્તર્ભાવ "missing-network.h"
#અન્તર્ભાવ "parse-util.h"
#અન્તર્ભાવ "path-util.h"
#અન્તર્ભાવ "process-util.h"
#અન્તર્ભાવ "string-util.h"
#અન્તર્ભાવ "strv.h"

પૂર્ણાંક parse_boolean(અચલ અક્ષર *v) {
        જો (!v)
                પાછા -EINVAL;

        જો (STRCASE_IN_SET(v,
                           "1",
                           "yes",
                           "y",
                           "true",
                           "t",
                           "on"))
                પાછા 1;

        જો (STRCASE_IN_SET(v,
                           "0",
                           "no",
                           "n",
                           "false",
                           "f",
                           "off"))
                પાછા 0;

        પાછા -EINVAL;
}

પૂર્ણાંક parse_tristate_full(અચલ અક્ષર *v, અચલ અક્ષર *third, પૂર્ણાંક *ret) {
        પૂર્ણાંક r;

        જો (isempty(v) || streq_ptr(v, third)) { /* Empty string is always taken as the third/invalid/auto state */
                જો (ret)
                        *ret = -1;
        } નહીંતર {
                r = parse_boolean(v);
                જો (r < 0)
                        પાછા r;

                જો (ret)
                        *ret = r;
        }

        પાછા 0;
}

પૂર્ણાંક parse_pid(અચલ અક્ષર *s, pid_t *ret) {
        unsigned long ul = 0;
        pid_t pid;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(s);

        r = safe_atolu(s, &ul);
        જો (r < 0)
                પાછા r;

        pid = (pid_t) ul;

        જો ((unsigned long) pid != ul)
                પાછા -ERANGE;

        જો (!pid_is_valid(pid))
                પાછા -ERANGE;

        જો (ret)
                *ret = pid;
        પાછા 0;
}

પૂર્ણાંક parse_mode(અચલ અક્ષર *s, mode_t *ret) {
        unsigned m;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(s);

        r = safe_atou_full(s, 8 |
                           SAFE_ATO_REFUSE_PLUS_MINUS, /* Leading '+' or even '-' char? that's just weird,
                                                        * refuse. User might have wanted to add mode flags or
                                                        * so, but this parser doesn't allow that, so let's
                                                        * better be safe. */
                           &m);
        જો (r < 0)
                પાછા r;
        જો (m > 07777)
                પાછા -ERANGE;

        જો (ret)
                *ret = m;
        પાછા 0;
}

પૂર્ણાંક parse_ifindex(અચલ અક્ષર *s) {
        પૂર્ણાંક ifi, r;

        પ્રતિજ્ઞા(s);

        r = safe_atoi(s, &ifi);
        જો (r < 0)
                પાછા r;
        જો (ifi <= 0)
                પાછા -EINVAL;

        પાછા ifi;
}

પૂર્ણાંક parse_mtu(પૂર્ણાંક family, અચલ અક્ષર *s, uint32_t *ret) {
        uint64_t u, m;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(ret);

        r = parse_size(s, 1024, &u);
        જો (r < 0)
                પાછા r;

        જો (u > UINT32_MAX)
                પાછા -ERANGE;

        પસંદગી (family) {
        સ્થિતિ AF_INET:
                m = IPV4_MIN_MTU; /* This is 68 */
                તોડો;
        સ્થિતિ AF_INET6:
                m = IPV6_MIN_MTU; /* This is 1280 */
                તોડો;
        મૂળભૂત:
                m = 0;
        }

        જો (u < m)
                પાછા -ERANGE;

        *ret = (uint32_t) u;
        પાછા 0;
}

પૂર્ણાંક parse_size(અચલ અક્ષર *t, uint64_t base, uint64_t *size) {

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

        સંરચના table {
                અચલ અક્ષર *suffix;
                unsigned long long factor;
        };

        સ્થિર અચલ સંરચના table iec[] = {
                { "E", 1024ULL*1024ULL*1024ULL*1024ULL*1024ULL*1024ULL },
                { "P", 1024ULL*1024ULL*1024ULL*1024ULL*1024ULL },
                { "T", 1024ULL*1024ULL*1024ULL*1024ULL },
                { "G", 1024ULL*1024ULL*1024ULL },
                { "M", 1024ULL*1024ULL },
                { "K", 1024ULL },
                { "B", 1ULL },
                { "",  1ULL },
        };

        સ્થિર અચલ સંરચના table si[] = {
                { "E", 1000ULL*1000ULL*1000ULL*1000ULL*1000ULL*1000ULL },
                { "P", 1000ULL*1000ULL*1000ULL*1000ULL*1000ULL },
                { "T", 1000ULL*1000ULL*1000ULL*1000ULL },
                { "G", 1000ULL*1000ULL*1000ULL },
                { "M", 1000ULL*1000ULL },
                { "K", 1000ULL },
                { "B", 1ULL },
                { "",  1ULL },
        };

        અચલ સંરચના table *table;
        અચલ અક્ષર *p;
        unsigned long long r = 0;
        unsigned n_entries, start_pos = 0;

        પ્રતિજ્ઞા(t);
        પ્રતિજ્ઞા(IN_SET(base, 1000, 1024));
        પ્રતિજ્ઞા(size);

        જો (base == 1000) {
                table = si;
                n_entries = ELEMENTSOF(si);
        } નહીંતર {
                table = iec;
                n_entries = ELEMENTSOF(iec);
        }

        p = t;
        કરો {
                unsigned long long l, tmp;
                દ્વિગુણ frac = 0;
                અક્ષર *e;
                unsigned i;

                p += strspn(p, WHITESPACE);

                errno = 0;
                l = strtoull(p, &e, 10);
                જો (errno > 0)
                        પાછા -errno;
                જો (e == p)
                        પાછા -EINVAL;
                જો (*p == '-')
                        પાછા -ERANGE;

                જો (*e == '.') {
                        e++;

                        /* strtoull() itself would accept space/+/- */
                        જો (ascii_isdigit(*e)) {
                                unsigned long long l2;
                                અક્ષર *e2;

                                l2 = strtoull(e, &e2, 10);
                                જો (errno > 0)
                                        પાછા -errno;

                                /* Ignore failure. E.g. 10.M is valid */
                                frac = l2;
                                માટે (; e < e2; e++)
                                        frac /= 10;
                        }
                }

                e += strspn(e, WHITESPACE);

                માટે (i = start_pos; i < n_entries; i++)
                        જો (startswith(e, table[i].suffix))
                                તોડો;

                જો (i >= n_entries)
                        પાછા -EINVAL;

                જો (l + (frac > 0) > ULLONG_MAX / table[i].factor)
                        પાછા -ERANGE;

                tmp = l * table[i].factor + (unsigned long long) (frac * table[i].factor);
                જો (tmp > ULLONG_MAX - r)
                        પાછા -ERANGE;

                r += tmp;
                જો ((unsigned long long) (uint64_t) r != r)
                        પાછા -ERANGE;

                p = e + strlen(table[i].suffix);

                start_pos = i + 1;

        } જ્યારે (*p);

        *size = r;

        પાછા 0;
}

પૂર્ણાંક parse_sector_size(અચલ અક્ષર *t, uint64_t *ret) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(t);
        પ્રતિજ્ઞા(ret);

        uint64_t ss;

        r = safe_atou64(t, &ss);
        જો (r < 0)
                પાછા log_error_errno(r, "Failed to parse sector size parameter %s", t);
        જો (ss < 512 || ss > 4096) /* Allow up to 4K due to dm-crypt support and 4K alignment by the homed LUKS backend */
                પાછા log_error_errno(SYNTHETIC_ERRNO(ERANGE), "Sector size not between 512 and 4096: %s", t);
        જો (!ISPOWEROF2(ss))
                પાછા log_error_errno(SYNTHETIC_ERRNO(EINVAL), "Sector size not power of 2: %s", t);

        *ret = ss;
        પાછા 0;
}

પૂર્ણાંક parse_range(અચલ અક્ષર *t, unsigned *lower, unsigned *upper) {
        _cleanup_free_ અક્ષર *word = ખાલી;
        unsigned l, u;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(lower);
        પ્રતિજ્ઞા(upper);

        /* Extract the lower bound. */
        r = extract_first_word(&t, &word, "-", EXTRACT_DONT_COALESCE_SEPARATORS);
        જો (r < 0)
                પાછા r;
        જો (r == 0)
                પાછા -EINVAL;

        r = safe_atou(word, &l);
        જો (r < 0)
                પાછા r;

        /* Check for the upper bound and extract it if needed */
        જો (!t)
                /* Single number with no dashes. */
                u = l;
        નહીંતર જો (!*t)
                /* Trailing dash is an error. */
                પાછા -EINVAL;
        નહીંતર {
                r = safe_atou(t, &u);
                જો (r < 0)
                        પાછા r;
        }

        *lower = l;
        *upper = u;
        પાછા 0;
}

પૂર્ણાંક parse_errno(અચલ અક્ષર *t) {
        પૂર્ણાંક r, e;

        પ્રતિજ્ઞા(t);

        r = errno_from_name(t);
        જો (r > 0)
                પાછા r;

        r = safe_atoi(t, &e);
        જો (r < 0)
                પાછા r;

        /* 0 is also allowed here */
        જો (!errno_is_valid(e) && e != 0)
                પાછા -ERANGE;

        પાછા e;
}

પૂર્ણાંક parse_fd(અચલ અક્ષર *t) {
        પૂર્ણાંક r, fd;

        પ્રતિજ્ઞા(t);

        r = safe_atoi(t, &fd);
        જો (r < 0)
                પાછા r;

        જો (fd < 0)
                પાછા -EBADF;

        પાછા fd;
}

પૂર્ણાંક parse_user_shell(અચલ અક્ષર *s, અક્ષર **ret_sh, bool *ret_copy) {
        અક્ષર *sh;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(ret_sh);
        પ્રતિજ્ઞા(ret_copy);

        જો (path_is_absolute(s) && path_is_normalized(s)) {
                sh = strdup(s);
                જો (!sh)
                        પાછા -ENOMEM;

                *ret_sh = sh;
                *ret_copy = false;
        } નહીંતર {
                r = parse_boolean(s);
                જો (r < 0)
                        પાછા r;

                *ret_sh = ખાલી;
                *ret_copy = r;
        }

        પાછા 0;
}

સ્થિર અચલ અક્ષર *mangle_base(અચલ અક્ષર *s, unsigned *base) {
        અચલ અક્ષર *k;

        પ્રતિજ્ઞા(s);
        પ્રતિજ્ઞા(base);

        /* Base already explicitly specified, then don't do anything. */
        જો (SAFE_ATO_MASK_FLAGS(*base) != 0)
                પાછા s;

        /* Support Python 3 style "0b" and 0x" prefixes, because they truly make sense, much more than C's "0" prefix for octal. */
        k = STARTSWITH_SET(s, "0b", "0B");
        જો (k) {
                *base = 2 | (*base & SAFE_ATO_ALL_FLAGS);
                પાછા k;
        }

        k = STARTSWITH_SET(s, "0o", "0O");
        જો (k) {
                *base = 8 | (*base & SAFE_ATO_ALL_FLAGS);
                પાછા k;
        }

        પાછા s;
}

પૂર્ણાંક safe_atou_full(અચલ અક્ષર *s, unsigned base, unsigned *ret_u) {
        અક્ષર *x = ખાલી;
        unsigned long l;

        પ્રતિજ્ઞા(s);
        પ્રતિજ્ઞા(SAFE_ATO_MASK_FLAGS(base) <= 16);

        /* strtoul() is happy to parse negative values, and silently converts them to unsigned values without
         * generating an error. We want a clean error, hence let's look for the "-" prefix on our own, and
         * generate an error. But let's do so only after strtoul() validated that the string is clean
         * otherwise, so that we return EINVAL preferably over ERANGE. */

        જો (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_WHITESPACE) &&
            strchr(WHITESPACE, s[0]))
                પાછા -EINVAL;

        s += strspn(s, WHITESPACE);

        જો (FLAGS_SET(base, SAFE_ATO_REFUSE_PLUS_MINUS) &&
            IN_SET(s[0], '+', '-'))
                પાછા -EINVAL; /* Note that we check the "-" prefix again a second time below, but return a
                                 * different error. I.e. if the SAFE_ATO_REFUSE_PLUS_MINUS flag is set we
                                 * blanket refuse +/- prefixed integers, while if it is missing we'll just
                                 * return ERANGE, because the string actually parses correctly, but doesn't
                                 * fit in the return type. */

        જો (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_ZERO) &&
            s[0] == '0' && !streq(s, "0"))
                પાછા -EINVAL; /* This is particularly useful to avoid ambiguities between C's octal
                                 * notation and assumed-to-be-decimal integers with a leading zero. */

        s = mangle_base(s, &base);

        errno = 0;
        l = strtoul(s, &x, SAFE_ATO_MASK_FLAGS(base) /* Let's mask off the flags bits so that only the actual
                                                      * base is left */);
        જો (errno > 0)
                પાછા -errno;
        જો (!x || x == s || *x != 0)
                પાછા -EINVAL;
        જો (l != 0 && s[0] == '-')
                પાછા -ERANGE;
        જો ((unsigned long) (unsigned) l != l)
                પાછા -ERANGE;

        જો (ret_u)
                *ret_u = (unsigned) l;

        પાછા 0;
}

પૂર્ણાંક safe_atou_bounded(અચલ અક્ષર *s, unsigned min, unsigned max, unsigned *ret) {
        unsigned v;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(ret);

        r = safe_atou(s, &v);
        જો (r < 0)
                પાછા r;

        જો (v < min || v > max)
                પાછા -ERANGE;

        *ret = v;
        પાછા 0;
}

પૂર્ણાંક safe_atoi(અચલ અક્ષર *s, પૂર્ણાંક *ret_i) {
        unsigned base = 0;
        અક્ષર *x = ખાલી;
        long l;

        પ્રતિજ્ઞા(s);

        s += strspn(s, WHITESPACE);
        s = mangle_base(s, &base);

        errno = 0;
        l = strtol(s, &x, base);
        જો (errno > 0)
                પાછા -errno;
        જો (!x || x == s || *x != 0)
                પાછા -EINVAL;
        જો ((long) (પૂર્ણાંક) l != l)
                પાછા -ERANGE;

        જો (ret_i)
                *ret_i = (પૂર્ણાંક) l;

        પાછા 0;
}

પૂર્ણાંક safe_atollu_full(અચલ અક્ષર *s, unsigned base, unsigned long long *ret_llu) {
        અક્ષર *x = ખાલી;
        unsigned long long l;

        પ્રતિજ્ઞા(s);
        પ્રતિજ્ઞા(SAFE_ATO_MASK_FLAGS(base) <= 16);

        જો (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_WHITESPACE) &&
            strchr(WHITESPACE, s[0]))
                પાછા -EINVAL;

        s += strspn(s, WHITESPACE);

        જો (FLAGS_SET(base, SAFE_ATO_REFUSE_PLUS_MINUS) &&
            IN_SET(s[0], '+', '-'))
                પાછા -EINVAL;

        જો (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_ZERO) &&
            s[0] == '0' && s[1] != 0)
                પાછા -EINVAL;

        s = mangle_base(s, &base);

        errno = 0;
        l = strtoull(s, &x, SAFE_ATO_MASK_FLAGS(base));
        જો (errno > 0)
                પાછા -errno;
        જો (!x || x == s || *x != 0)
                પાછા -EINVAL;
        જો (l != 0 && s[0] == '-')
                પાછા -ERANGE;

        જો (ret_llu)
                *ret_llu = l;

        પાછા 0;
}

પૂર્ણાંક safe_atolli(અચલ અક્ષર *s, long long *ret_lli) {
        unsigned base = 0;
        અક્ષર *x = ખાલી;
        long long l;

        પ્રતિજ્ઞા(s);

        s += strspn(s, WHITESPACE);
        s = mangle_base(s, &base);

        errno = 0;
        l = strtoll(s, &x, base);
        જો (errno > 0)
                પાછા -errno;
        જો (!x || x == s || *x != 0)
                પાછા -EINVAL;

        જો (ret_lli)
                *ret_lli = l;

        પાછા 0;
}

પૂર્ણાંક safe_atou8_full(અચલ અક્ષર *s, unsigned base, uint8_t *ret) {
        unsigned u;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(ret);

        r = safe_atou_full(s, base, &u);
        જો (r < 0)
                પાછા r;
        જો (u > UINT8_MAX)
                પાછા -ERANGE;

        *ret = (uint8_t) u;
        પાછા 0;
}

પૂર્ણાંક safe_atou16_full(અચલ અક્ષર *s, unsigned base, uint16_t *ret) {
        unsigned u;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(ret);

        r = safe_atou_full(s, base, &u);
        જો (r < 0)
                પાછા r;
        જો (u > UINT16_MAX)
                પાછા -ERANGE;

        *ret = (uint16_t) u;
        પાછા 0;
}

પૂર્ણાંક safe_atoi16(અચલ અક્ષર *s, int16_t *ret) {
        unsigned base = 0;
        અક્ષર *x = ખાલી;
        long l;

        પ્રતિજ્ઞા(s);

        s += strspn(s, WHITESPACE);
        s = mangle_base(s, &base);

        errno = 0;
        l = strtol(s, &x, base);
        જો (errno > 0)
                પાછા -errno;
        જો (!x || x == s || *x != 0)
                પાછા -EINVAL;
        જો ((long) (int16_t) l != l)
                પાછા -ERANGE;

        જો (ret)
                *ret = (int16_t) l;

        પાછા 0;
}

પૂર્ણાંક safe_atod(અચલ અક્ષર *s, દ્વિગુણ *ret_d) {
        _cleanup_(freelocalep) locale_t loc = (locale_t) 0;
        અક્ષર *x = ખાલી;
        દ્વિગુણ d = 0;

        પ્રતિજ્ઞા(s);

        loc = newlocale(LC_NUMERIC_MASK, "C", (locale_t) 0);
        જો (loc == (locale_t) 0)
                પાછા -errno;

        errno = 0;
        d = strtod_l(s, &x, loc);
        જો (errno > 0)
                પાછા -errno;
        જો (!x || x == s || *x != 0)
                પાછા -EINVAL;

        જો (ret_d)
                *ret_d = d;

        પાછા 0;
}

પૂર્ણાંક parse_fractional_part_u(અચલ અક્ષર **p, size_t digits, unsigned *res) {
        unsigned val = 0;
        અચલ અક્ષર *s;

        પ્રતિજ્ઞા(p);
        પ્રતિજ્ઞા(res);

        s = *p;

        /* accept any number of digits, strtoull is limited to 19 */
        માટે (size_t i = 0; i < digits; i++, s++) {
                જો (!ascii_isdigit(*s)) {
                        જો (i == 0)
                                પાછા -EINVAL;

                        /* too few digits, pad with 0 */
                        માટે (; i < digits; i++)
                                val *= 10;

                        તોડો;
                }

                val *= 10;
                val += *s - '0';
        }

        /* maybe round up */
        જો (*s >= '5' && *s <= '9')
                val++;

        s += strspn(s, DIGITS);

        *p = s;
        *res = val;

        પાછા 0;
}

પૂર્ણાંક parse_nice(અચલ અક્ષર *s, પૂર્ણાંક *ret) {
        પૂર્ણાંક n, r;

        પ્રતિજ્ઞા(ret);

        r = safe_atoi(s, &n);
        જો (r < 0)
                પાછા r;

        જો (!nice_is_valid(n))
                પાછા -ERANGE;

        *ret = n;
        પાછા 0;
}

પૂર્ણાંક parse_ip_port(અચલ અક્ષર *s, uint16_t *ret) {
        uint16_t l;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(ret);

        r = safe_atou16_full(s, SAFE_ATO_REFUSE_LEADING_WHITESPACE, &l);
        જો (r < 0)
                પાછા r;

        જો (l == 0)
                પાછા -EINVAL;

        *ret = l;

        પાછા 0;
}

પૂર્ણાંક parse_ip_port_range(અચલ અક્ષર *s, uint16_t *low, uint16_t *high, bool allow_zero) {
        unsigned l, h;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(low);
        પ્રતિજ્ઞા(high);

        r = parse_range(s, &l, &h);
        જો (r < 0)
                પાછા r;

        જો (l > 65535 || h > 65535)
                પાછા -EINVAL;

        જો (!allow_zero && (l == 0 || h == 0))
                પાછા -EINVAL;

        જો (h < l)
                પાછા -EINVAL;

        *low = l;
        *high = h;

        પાછા 0;
}

પૂર્ણાંક parse_oom_score_adjust(અચલ અક્ષર *s, પૂર્ણાંક *ret) {
        પૂર્ણાંક r, v;

        પ્રતિજ્ઞા(s);
        પ્રતિજ્ઞા(ret);

        r = safe_atoi(s, &v);
        જો (r < 0)
                પાછા r;

        જો (!oom_score_adjust_is_valid(v))
                પાછા -ERANGE;

        *ret = v;
        પાછા 0;
}

પૂર્ણાંક store_loadavg_fixed_point(unsigned long i, unsigned long f, loadavg_t *ret) {
        પ્રતિજ્ઞા(ret);

        /* Validate both parts *before* shifting them, so that the shifts themselves cannot overflow.
         * Rejecting f >= 100 is equivalent to the old check of the shifted value against
         * LOADAVG_FIXED_POINT_1_0, since DIV_ROUND_UP(99 << 11, 100) == 2028 and
         * DIV_ROUND_UP(100 << 11, 100) == 2048. */
        જો (i > (~0UL >> LOADAVG_PRECISION_BITS))
                પાછા -ERANGE;

        જો (f >= 100)
                પાછા -ERANGE;

        *ret = (i << LOADAVG_PRECISION_BITS) | DIV_ROUND_UP(f << LOADAVG_PRECISION_BITS, 100);
        પાછા 0;
}

પૂર્ણાંક parse_loadavg_fixed_point(અચલ અક્ષર *s, loadavg_t *ret) {
        અચલ અક્ષર *d, *f_str, *i_str;
        unsigned long i, f;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(s);
        પ્રતિજ્ઞા(ret);

        d = strchr(s, '.');
        જો (!d)
                પાછા -EINVAL;

        i_str = strndupa_safe(s, d - s);
        f_str = d + 1;

        r = safe_atolu_full(i_str, 10, &i);
        જો (r < 0)
                પાછા r;

        r = safe_atolu_full(f_str, 10, &f);
        જો (r < 0)
                પાછા r;

        પાછા store_loadavg_fixed_point(i, f, ret);
}

/* Limitations are described in https://www.netfilter.org/projects/nftables/manpage.html and
 * https://bugzilla.netfilter.org/show_bug.cgi?id=1175 */
bool nft_identifier_valid(અચલ અક્ષર *id) {
        જો (isempty(id))
                પાછા false;

        જો (strlen(id) >= NFT_NAME_MAXLEN)
                પાછા false;

        જો (!ascii_isalpha(id[0]))
                પાછા false;

        પાછા in_charset(id + 1, ALPHANUMERICAL "/\\_.");
}

પૂર્ણાંક parse_capability_set(અચલ અક્ષર *s, uint64_t initial, uint64_t *current) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(s);
        પ્રતિજ્ઞા(current);

        જો (isempty(s)) {
                *current = CAP_MASK_UNSET;
                પાછા 1;
        }

        bool invert = false;
        જો (s[0] == '~') {
                invert = true;
                s++;
        }

        uint64_t parsed;
        r = capability_set_from_string(s, &parsed);
        જો (r < 0)
                પાછા r;

        જો (parsed == 0 || *current == initial)
                /* "~" or uninitialized data -> replace */
                *current = invert ? all_capabilities() & ~parsed : parsed;
        નહીંતર {
                /* previous data -> merge */
                જો (invert)
                        *current &= ~parsed;
                નહીંતર
                        *current |= parsed;
        }

        પાછા r;
}
