/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <linux/ipv6.h>
#ضمن <linux/netfilter/nf_tables.h>
#ضمن <stdio.h>
#ضمن <stdlib.h>
#ضمن <sys/socket.h>

#ضمن "alloc-util.h"
#ضمن "capability-list.h"
#ضمن "capability-util.h"
#ضمن "errno-list.h"
#ضمن "extract-word.h"
#ضمن "locale-util.h"
#ضمن "log.h"
#ضمن "missing-network.h"
#ضمن "parse-util.h"
#ضمن "path-util.h"
#ضمن "process-util.h"
#ضمن "string-util.h"
#ضمن "strv.h"

صحيح parse_boolean(ثابت حرف *v) {
        إذا (!v)
                أرجع -EINVAL;

        إذا (STRCASE_IN_SET(v,
                           "1",
                           "yes",
                           "y",
                           "true",
                           "t",
                           "on"))
                أرجع 1;

        إذا (STRCASE_IN_SET(v,
                           "0",
                           "no",
                           "n",
                           "false",
                           "f",
                           "off"))
                أرجع 0;

        أرجع -EINVAL;
}

صحيح parse_tristate_full(ثابت حرف *v, ثابت حرف *third, صحيح *ret) {
        صحيح r;

        إذا (isempty(v) || streq_ptr(v, third)) { /* Empty string is always taken as the third/invalid/auto state */
                إذا (ret)
                        *ret = -1;
        } وإلا {
                r = parse_boolean(v);
                إذا (r < 0)
                        أرجع r;

                إذا (ret)
                        *ret = r;
        }

        أرجع 0;
}

صحيح parse_pid(ثابت حرف *s, pid_t *ret) {
        unsigned long ul = 0;
        pid_t pid;
        صحيح r;

        أكد(s);

        r = safe_atolu(s, &ul);
        إذا (r < 0)
                أرجع r;

        pid = (pid_t) ul;

        إذا ((unsigned long) pid != ul)
                أرجع -ERANGE;

        إذا (!pid_is_valid(pid))
                أرجع -ERANGE;

        إذا (ret)
                *ret = pid;
        أرجع 0;
}

صحيح parse_mode(ثابت حرف *s, mode_t *ret) {
        unsigned m;
        صحيح r;

        أكد(s);

        r = safe_atou_full(s, 8 |
                           SAFE_ATO_REFUSE_PLUS_MINUS, /* Leading '+' or even '-' char? that's just weird,
                                                        * refuse. User might have wanted to add mode flags or
                                                        * so, but this parser doesn't allow that, so let's
                                                        * better be safe. */
                           &m);
        إذا (r < 0)
                أرجع r;
        إذا (m > 07777)
                أرجع -ERANGE;

        إذا (ret)
                *ret = m;
        أرجع 0;
}

صحيح parse_ifindex(ثابت حرف *s) {
        صحيح ifi, r;

        أكد(s);

        r = safe_atoi(s, &ifi);
        إذا (r < 0)
                أرجع r;
        إذا (ifi <= 0)
                أرجع -EINVAL;

        أرجع ifi;
}

صحيح parse_mtu(صحيح family, ثابت حرف *s, uint32_t *ret) {
        uint64_t u, m;
        صحيح r;

        أكد(ret);

        r = parse_size(s, 1024, &u);
        إذا (r < 0)
                أرجع r;

        إذا (u > UINT32_MAX)
                أرجع -ERANGE;

        بدل (family) {
        حالة AF_INET:
                m = IPV4_MIN_MTU; /* This is 68 */
                اكسر;
        حالة AF_INET6:
                m = IPV6_MIN_MTU; /* This is 1280 */
                اكسر;
        افتراضي:
                m = 0;
        }

        إذا (u < m)
                أرجع -ERANGE;

        *ret = (uint32_t) u;
        أرجع 0;
}

صحيح parse_size(ثابت حرف *t, uint64_t base, uint64_t *size) {

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

        بنية table {
                ثابت حرف *suffix;
                unsigned long long factor;
        };

        ساكن ثابت بنية table iec[] = {
                { "E", 1024ULL*1024ULL*1024ULL*1024ULL*1024ULL*1024ULL },
                { "P", 1024ULL*1024ULL*1024ULL*1024ULL*1024ULL },
                { "T", 1024ULL*1024ULL*1024ULL*1024ULL },
                { "G", 1024ULL*1024ULL*1024ULL },
                { "M", 1024ULL*1024ULL },
                { "K", 1024ULL },
                { "B", 1ULL },
                { "",  1ULL },
        };

        ساكن ثابت بنية table si[] = {
                { "E", 1000ULL*1000ULL*1000ULL*1000ULL*1000ULL*1000ULL },
                { "P", 1000ULL*1000ULL*1000ULL*1000ULL*1000ULL },
                { "T", 1000ULL*1000ULL*1000ULL*1000ULL },
                { "G", 1000ULL*1000ULL*1000ULL },
                { "M", 1000ULL*1000ULL },
                { "K", 1000ULL },
                { "B", 1ULL },
                { "",  1ULL },
        };

        ثابت بنية table *table;
        ثابت حرف *p;
        unsigned long long r = 0;
        unsigned n_entries, start_pos = 0;

        أكد(t);
        أكد(IN_SET(base, 1000, 1024));
        أكد(size);

        إذا (base == 1000) {
                table = si;
                n_entries = ELEMENTSOF(si);
        } وإلا {
                table = iec;
                n_entries = ELEMENTSOF(iec);
        }

        p = t;
        افعل {
                unsigned long long l, tmp;
                مزدوج frac = 0;
                حرف *e;
                unsigned i;

                p += strspn(p, WHITESPACE);

                errno = 0;
                l = strtoull(p, &e, 10);
                إذا (errno > 0)
                        أرجع -errno;
                إذا (e == p)
                        أرجع -EINVAL;
                إذا (*p == '-')
                        أرجع -ERANGE;

                إذا (*e == '.') {
                        e++;

                        /* strtoull() itself would accept space/+/- */
                        إذا (ascii_isdigit(*e)) {
                                unsigned long long l2;
                                حرف *e2;

                                l2 = strtoull(e, &e2, 10);
                                إذا (errno > 0)
                                        أرجع -errno;

                                /* Ignore failure. E.g. 10.M is valid */
                                frac = l2;
                                لكل (; e < e2; e++)
                                        frac /= 10;
                        }
                }

                e += strspn(e, WHITESPACE);

                لكل (i = start_pos; i < n_entries; i++)
                        إذا (startswith(e, table[i].suffix))
                                اكسر;

                إذا (i >= n_entries)
                        أرجع -EINVAL;

                إذا (l + (frac > 0) > ULLONG_MAX / table[i].factor)
                        أرجع -ERANGE;

                tmp = l * table[i].factor + (unsigned long long) (frac * table[i].factor);
                إذا (tmp > ULLONG_MAX - r)
                        أرجع -ERANGE;

                r += tmp;
                إذا ((unsigned long long) (uint64_t) r != r)
                        أرجع -ERANGE;

                p = e + strlen(table[i].suffix);

                start_pos = i + 1;

        } طالما (*p);

        *size = r;

        أرجع 0;
}

صحيح parse_sector_size(ثابت حرف *t, uint64_t *ret) {
        صحيح r;

        أكد(t);
        أكد(ret);

        uint64_t ss;

        r = safe_atou64(t, &ss);
        إذا (r < 0)
                أرجع log_error_errno(r, "Failed to parse sector size parameter %s", t);
        إذا (ss < 512 || ss > 4096) /* Allow up to 4K due to dm-crypt support and 4K alignment by the homed LUKS backend */
                أرجع log_error_errno(SYNTHETIC_ERRNO(ERANGE), "Sector size not between 512 and 4096: %s", t);
        إذا (!ISPOWEROF2(ss))
                أرجع log_error_errno(SYNTHETIC_ERRNO(EINVAL), "Sector size not power of 2: %s", t);

        *ret = ss;
        أرجع 0;
}

صحيح parse_range(ثابت حرف *t, unsigned *lower, unsigned *upper) {
        _cleanup_free_ حرف *word = عدم;
        unsigned l, u;
        صحيح r;

        أكد(lower);
        أكد(upper);

        /* Extract the lower bound. */
        r = extract_first_word(&t, &word, "-", EXTRACT_DONT_COALESCE_SEPARATORS);
        إذا (r < 0)
                أرجع r;
        إذا (r == 0)
                أرجع -EINVAL;

        r = safe_atou(word, &l);
        إذا (r < 0)
                أرجع r;

        /* Check for the upper bound and extract it if needed */
        إذا (!t)
                /* Single number with no dashes. */
                u = l;
        وإلا إذا (!*t)
                /* Trailing dash is an error. */
                أرجع -EINVAL;
        وإلا {
                r = safe_atou(t, &u);
                إذا (r < 0)
                        أرجع r;
        }

        *lower = l;
        *upper = u;
        أرجع 0;
}

صحيح parse_errno(ثابت حرف *t) {
        صحيح r, e;

        أكد(t);

        r = errno_from_name(t);
        إذا (r > 0)
                أرجع r;

        r = safe_atoi(t, &e);
        إذا (r < 0)
                أرجع r;

        /* 0 is also allowed here */
        إذا (!errno_is_valid(e) && e != 0)
                أرجع -ERANGE;

        أرجع e;
}

صحيح parse_fd(ثابت حرف *t) {
        صحيح r, fd;

        أكد(t);

        r = safe_atoi(t, &fd);
        إذا (r < 0)
                أرجع r;

        إذا (fd < 0)
                أرجع -EBADF;

        أرجع fd;
}

صحيح parse_user_shell(ثابت حرف *s, حرف **ret_sh, bool *ret_copy) {
        حرف *sh;
        صحيح r;

        أكد(ret_sh);
        أكد(ret_copy);

        إذا (path_is_absolute(s) && path_is_normalized(s)) {
                sh = strdup(s);
                إذا (!sh)
                        أرجع -ENOMEM;

                *ret_sh = sh;
                *ret_copy = false;
        } وإلا {
                r = parse_boolean(s);
                إذا (r < 0)
                        أرجع r;

                *ret_sh = عدم;
                *ret_copy = r;
        }

        أرجع 0;
}

ساكن ثابت حرف *mangle_base(ثابت حرف *s, unsigned *base) {
        ثابت حرف *k;

        أكد(s);
        أكد(base);

        /* Base already explicitly specified, then don't do anything. */
        إذا (SAFE_ATO_MASK_FLAGS(*base) != 0)
                أرجع s;

        /* Support Python 3 style "0b" and 0x" prefixes, because they truly make sense, much more than C's "0" prefix for octal. */
        k = STARTSWITH_SET(s, "0b", "0B");
        إذا (k) {
                *base = 2 | (*base & SAFE_ATO_ALL_FLAGS);
                أرجع k;
        }

        k = STARTSWITH_SET(s, "0o", "0O");
        إذا (k) {
                *base = 8 | (*base & SAFE_ATO_ALL_FLAGS);
                أرجع k;
        }

        أرجع s;
}

صحيح safe_atou_full(ثابت حرف *s, unsigned base, unsigned *ret_u) {
        حرف *x = عدم;
        unsigned long l;

        أكد(s);
        أكد(SAFE_ATO_MASK_FLAGS(base) <= 16);

        /* strtoul() is happy to parse negative values, and silently converts them to unsigned values without
         * generating an error. We want a clean error, hence let's look for the "-" prefix on our own, and
         * generate an error. But let's do so only after strtoul() validated that the string is clean
         * otherwise, so that we return EINVAL preferably over ERANGE. */

        إذا (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_WHITESPACE) &&
            strchr(WHITESPACE, s[0]))
                أرجع -EINVAL;

        s += strspn(s, WHITESPACE);

        إذا (FLAGS_SET(base, SAFE_ATO_REFUSE_PLUS_MINUS) &&
            IN_SET(s[0], '+', '-'))
                أرجع -EINVAL; /* Note that we check the "-" prefix again a second time below, but return a
                                 * different error. I.e. if the SAFE_ATO_REFUSE_PLUS_MINUS flag is set we
                                 * blanket refuse +/- prefixed integers, while if it is missing we'll just
                                 * return ERANGE, because the string actually parses correctly, but doesn't
                                 * fit in the return type. */

        إذا (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_ZERO) &&
            s[0] == '0' && !streq(s, "0"))
                أرجع -EINVAL; /* This is particularly useful to avoid ambiguities between C's octal
                                 * notation and assumed-to-be-decimal integers with a leading zero. */

        s = mangle_base(s, &base);

        errno = 0;
        l = strtoul(s, &x, SAFE_ATO_MASK_FLAGS(base) /* Let's mask off the flags bits so that only the actual
                                                      * base is left */);
        إذا (errno > 0)
                أرجع -errno;
        إذا (!x || x == s || *x != 0)
                أرجع -EINVAL;
        إذا (l != 0 && s[0] == '-')
                أرجع -ERANGE;
        إذا ((unsigned long) (unsigned) l != l)
                أرجع -ERANGE;

        إذا (ret_u)
                *ret_u = (unsigned) l;

        أرجع 0;
}

صحيح safe_atou_bounded(ثابت حرف *s, unsigned min, unsigned max, unsigned *ret) {
        unsigned v;
        صحيح r;

        أكد(ret);

        r = safe_atou(s, &v);
        إذا (r < 0)
                أرجع r;

        إذا (v < min || v > max)
                أرجع -ERANGE;

        *ret = v;
        أرجع 0;
}

صحيح safe_atoi(ثابت حرف *s, صحيح *ret_i) {
        unsigned base = 0;
        حرف *x = عدم;
        long l;

        أكد(s);

        s += strspn(s, WHITESPACE);
        s = mangle_base(s, &base);

        errno = 0;
        l = strtol(s, &x, base);
        إذا (errno > 0)
                أرجع -errno;
        إذا (!x || x == s || *x != 0)
                أرجع -EINVAL;
        إذا ((long) (صحيح) l != l)
                أرجع -ERANGE;

        إذا (ret_i)
                *ret_i = (صحيح) l;

        أرجع 0;
}

صحيح safe_atollu_full(ثابت حرف *s, unsigned base, unsigned long long *ret_llu) {
        حرف *x = عدم;
        unsigned long long l;

        أكد(s);
        أكد(SAFE_ATO_MASK_FLAGS(base) <= 16);

        إذا (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_WHITESPACE) &&
            strchr(WHITESPACE, s[0]))
                أرجع -EINVAL;

        s += strspn(s, WHITESPACE);

        إذا (FLAGS_SET(base, SAFE_ATO_REFUSE_PLUS_MINUS) &&
            IN_SET(s[0], '+', '-'))
                أرجع -EINVAL;

        إذا (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_ZERO) &&
            s[0] == '0' && s[1] != 0)
                أرجع -EINVAL;

        s = mangle_base(s, &base);

        errno = 0;
        l = strtoull(s, &x, SAFE_ATO_MASK_FLAGS(base));
        إذا (errno > 0)
                أرجع -errno;
        إذا (!x || x == s || *x != 0)
                أرجع -EINVAL;
        إذا (l != 0 && s[0] == '-')
                أرجع -ERANGE;

        إذا (ret_llu)
                *ret_llu = l;

        أرجع 0;
}

صحيح safe_atolli(ثابت حرف *s, long long *ret_lli) {
        unsigned base = 0;
        حرف *x = عدم;
        long long l;

        أكد(s);

        s += strspn(s, WHITESPACE);
        s = mangle_base(s, &base);

        errno = 0;
        l = strtoll(s, &x, base);
        إذا (errno > 0)
                أرجع -errno;
        إذا (!x || x == s || *x != 0)
                أرجع -EINVAL;

        إذا (ret_lli)
                *ret_lli = l;

        أرجع 0;
}

صحيح safe_atou8_full(ثابت حرف *s, unsigned base, uint8_t *ret) {
        unsigned u;
        صحيح r;

        أكد(ret);

        r = safe_atou_full(s, base, &u);
        إذا (r < 0)
                أرجع r;
        إذا (u > UINT8_MAX)
                أرجع -ERANGE;

        *ret = (uint8_t) u;
        أرجع 0;
}

صحيح safe_atou16_full(ثابت حرف *s, unsigned base, uint16_t *ret) {
        unsigned u;
        صحيح r;

        أكد(ret);

        r = safe_atou_full(s, base, &u);
        إذا (r < 0)
                أرجع r;
        إذا (u > UINT16_MAX)
                أرجع -ERANGE;

        *ret = (uint16_t) u;
        أرجع 0;
}

صحيح safe_atoi16(ثابت حرف *s, int16_t *ret) {
        unsigned base = 0;
        حرف *x = عدم;
        long l;

        أكد(s);

        s += strspn(s, WHITESPACE);
        s = mangle_base(s, &base);

        errno = 0;
        l = strtol(s, &x, base);
        إذا (errno > 0)
                أرجع -errno;
        إذا (!x || x == s || *x != 0)
                أرجع -EINVAL;
        إذا ((long) (int16_t) l != l)
                أرجع -ERANGE;

        إذا (ret)
                *ret = (int16_t) l;

        أرجع 0;
}

صحيح safe_atod(ثابت حرف *s, مزدوج *ret_d) {
        _cleanup_(freelocalep) locale_t loc = (locale_t) 0;
        حرف *x = عدم;
        مزدوج d = 0;

        أكد(s);

        loc = newlocale(LC_NUMERIC_MASK, "C", (locale_t) 0);
        إذا (loc == (locale_t) 0)
                أرجع -errno;

        errno = 0;
        d = strtod_l(s, &x, loc);
        إذا (errno > 0)
                أرجع -errno;
        إذا (!x || x == s || *x != 0)
                أرجع -EINVAL;

        إذا (ret_d)
                *ret_d = d;

        أرجع 0;
}

صحيح parse_fractional_part_u(ثابت حرف **p, size_t digits, unsigned *res) {
        unsigned val = 0;
        ثابت حرف *s;

        أكد(p);
        أكد(res);

        s = *p;

        /* accept any number of digits, strtoull is limited to 19 */
        لكل (size_t i = 0; i < digits; i++, s++) {
                إذا (!ascii_isdigit(*s)) {
                        إذا (i == 0)
                                أرجع -EINVAL;

                        /* too few digits, pad with 0 */
                        لكل (; i < digits; i++)
                                val *= 10;

                        اكسر;
                }

                val *= 10;
                val += *s - '0';
        }

        /* maybe round up */
        إذا (*s >= '5' && *s <= '9')
                val++;

        s += strspn(s, DIGITS);

        *p = s;
        *res = val;

        أرجع 0;
}

صحيح parse_nice(ثابت حرف *s, صحيح *ret) {
        صحيح n, r;

        أكد(ret);

        r = safe_atoi(s, &n);
        إذا (r < 0)
                أرجع r;

        إذا (!nice_is_valid(n))
                أرجع -ERANGE;

        *ret = n;
        أرجع 0;
}

صحيح parse_ip_port(ثابت حرف *s, uint16_t *ret) {
        uint16_t l;
        صحيح r;

        أكد(ret);

        r = safe_atou16_full(s, SAFE_ATO_REFUSE_LEADING_WHITESPACE, &l);
        إذا (r < 0)
                أرجع r;

        إذا (l == 0)
                أرجع -EINVAL;

        *ret = l;

        أرجع 0;
}

صحيح parse_ip_port_range(ثابت حرف *s, uint16_t *low, uint16_t *high, bool allow_zero) {
        unsigned l, h;
        صحيح r;

        أكد(low);
        أكد(high);

        r = parse_range(s, &l, &h);
        إذا (r < 0)
                أرجع r;

        إذا (l > 65535 || h > 65535)
                أرجع -EINVAL;

        إذا (!allow_zero && (l == 0 || h == 0))
                أرجع -EINVAL;

        إذا (h < l)
                أرجع -EINVAL;

        *low = l;
        *high = h;

        أرجع 0;
}

صحيح parse_oom_score_adjust(ثابت حرف *s, صحيح *ret) {
        صحيح r, v;

        أكد(s);
        أكد(ret);

        r = safe_atoi(s, &v);
        إذا (r < 0)
                أرجع r;

        إذا (!oom_score_adjust_is_valid(v))
                أرجع -ERANGE;

        *ret = v;
        أرجع 0;
}

صحيح store_loadavg_fixed_point(unsigned long i, unsigned long f, loadavg_t *ret) {
        أكد(ret);

        /* Validate both parts *before* shifting them, so that the shifts themselves cannot overflow.
         * Rejecting f >= 100 is equivalent to the old check of the shifted value against
         * LOADAVG_FIXED_POINT_1_0, since DIV_ROUND_UP(99 << 11, 100) == 2028 and
         * DIV_ROUND_UP(100 << 11, 100) == 2048. */
        إذا (i > (~0UL >> LOADAVG_PRECISION_BITS))
                أرجع -ERANGE;

        إذا (f >= 100)
                أرجع -ERANGE;

        *ret = (i << LOADAVG_PRECISION_BITS) | DIV_ROUND_UP(f << LOADAVG_PRECISION_BITS, 100);
        أرجع 0;
}

صحيح parse_loadavg_fixed_point(ثابت حرف *s, loadavg_t *ret) {
        ثابت حرف *d, *f_str, *i_str;
        unsigned long i, f;
        صحيح r;

        أكد(s);
        أكد(ret);

        d = strchr(s, '.');
        إذا (!d)
                أرجع -EINVAL;

        i_str = strndupa_safe(s, d - s);
        f_str = d + 1;

        r = safe_atolu_full(i_str, 10, &i);
        إذا (r < 0)
                أرجع r;

        r = safe_atolu_full(f_str, 10, &f);
        إذا (r < 0)
                أرجع r;

        أرجع store_loadavg_fixed_point(i, f, ret);
}

/* Limitations are described in https://www.netfilter.org/projects/nftables/manpage.html and
 * https://bugzilla.netfilter.org/show_bug.cgi?id=1175 */
bool nft_identifier_valid(ثابت حرف *id) {
        إذا (isempty(id))
                أرجع false;

        إذا (strlen(id) >= NFT_NAME_MAXLEN)
                أرجع false;

        إذا (!ascii_isalpha(id[0]))
                أرجع false;

        أرجع in_charset(id + 1, ALPHANUMERICAL "/\\_.");
}

صحيح parse_capability_set(ثابت حرف *s, uint64_t initial, uint64_t *current) {
        صحيح r;

        أكد(s);
        أكد(current);

        إذا (isempty(s)) {
                *current = CAP_MASK_UNSET;
                أرجع 1;
        }

        bool invert = false;
        إذا (s[0] == '~') {
                invert = true;
                s++;
        }

        uint64_t parsed;
        r = capability_set_from_string(s, &parsed);
        إذا (r < 0)
                أرجع r;

        إذا (parsed == 0 || *current == initial)
                /* "~" or uninitialized data -> replace */
                *current = invert ? all_capabilities() & ~parsed : parsed;
        وإلا {
                /* previous data -> merge */
                إذا (invert)
                        *current &= ~parsed;
                وإلا
                        *current |= parsed;
        }

        أرجع r;
}
