/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <linux/ipv6.h>
#شامل <linux/netfilter/nf_tables.h>
#شامل <stdio.h>
#شامل <stdlib.h>
#شامل <sys/socket.h>

#شامل "alloc-util.h"
#شامل "capability-list.h"
#شامل "capability-util.h"
#شامل "errno-list.h"
#شامل "extract-word.h"
#شامل "locale-util.h"
#شامل "log.h"
#شامل "missing-network.h"
#شامل "parse-util.h"
#شامل "path-util.h"
#شامل "process-util.h"
#شامل "string-util.h"
#شامل "strv.h"

صحیح parse_boolean(ثابت نویسه *v) {
        اگر (!v)
                بازگشت -EINVAL;

        اگر (STRCASE_IN_SET(v,
                           "1",
                           "yes",
                           "y",
                           "true",
                           "t",
                           "on"))
                بازگشت 1;

        اگر (STRCASE_IN_SET(v,
                           "0",
                           "no",
                           "n",
                           "false",
                           "f",
                           "off"))
                بازگشت 0;

        بازگشت -EINVAL;
}

صحیح parse_tristate_full(ثابت نویسه *v, ثابت نویسه *third, صحیح *ret) {
        صحیح r;

        اگر (isempty(v) || streq_ptr(v, third)) { /* Empty string is always taken as the third/invalid/auto state */
                اگر (ret)
                        *ret = -1;
        } وگرنه {
                r = parse_boolean(v);
                اگر (r < 0)
                        بازگشت r;

                اگر (ret)
                        *ret = r;
        }

        بازگشت 0;
}

صحیح parse_pid(ثابت نویسه *s, pid_t *ret) {
        unsigned long ul = 0;
        pid_t pid;
        صحیح r;

        أكد(s);

        r = safe_atolu(s, &ul);
        اگر (r < 0)
                بازگشت r;

        pid = (pid_t) ul;

        اگر ((unsigned long) pid != ul)
                بازگشت -ERANGE;

        اگر (!pid_is_valid(pid))
                بازگشت -ERANGE;

        اگر (ret)
                *ret = pid;
        بازگشت 0;
}

صحیح parse_mode(ثابت نویسه *s, mode_t *ret) {
        unsigned m;
        صحیح r;

        أكد(s);

        r = safe_atou_full(s, 8 |
                           SAFE_ATO_REFUSE_PLUS_MINUS, /* Leading '+' or even '-' char? that's just weird,
                                                        * refuse. User might have wanted to add mode flags or
                                                        * so, but this parser doesn't allow that, so let's
                                                        * better be safe. */
                           &m);
        اگر (r < 0)
                بازگشت r;
        اگر (m > 07777)
                بازگشت -ERANGE;

        اگر (ret)
                *ret = m;
        بازگشت 0;
}

صحیح parse_ifindex(ثابت نویسه *s) {
        صحیح ifi, r;

        أكد(s);

        r = safe_atoi(s, &ifi);
        اگر (r < 0)
                بازگشت r;
        اگر (ifi <= 0)
                بازگشت -EINVAL;

        بازگشت ifi;
}

صحیح parse_mtu(صحیح family, ثابت نویسه *s, uint32_t *ret) {
        uint64_t u, m;
        صحیح r;

        أكد(ret);

        r = parse_size(s, 1024, &u);
        اگر (r < 0)
                بازگشت r;

        اگر (u > UINT32_MAX)
                بازگشت -ERANGE;

        انتخاب (family) {
        حالت AF_INET:
                m = IPV4_MIN_MTU; /* This is 68 */
                بشکن;
        حالت AF_INET6:
                m = IPV6_MIN_MTU; /* This is 1280 */
                بشکن;
        پیشفرض:
                m = 0;
        }

        اگر (u < m)
                بازگشت -ERANGE;

        *ret = (uint32_t) u;
        بازگشت 0;
}

صحیح parse_size(ثابت نویسه *t, uint64_t base, uint64_t *size) {

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

        ساختار table {
                ثابت نویسه *suffix;
                unsigned long long factor;
        };

        ایستا ثابت ساختار table iec[] = {
                { "E", 1024ULL*1024ULL*1024ULL*1024ULL*1024ULL*1024ULL },
                { "P", 1024ULL*1024ULL*1024ULL*1024ULL*1024ULL },
                { "T", 1024ULL*1024ULL*1024ULL*1024ULL },
                { "G", 1024ULL*1024ULL*1024ULL },
                { "M", 1024ULL*1024ULL },
                { "K", 1024ULL },
                { "B", 1ULL },
                { "",  1ULL },
        };

        ایستا ثابت ساختار table si[] = {
                { "E", 1000ULL*1000ULL*1000ULL*1000ULL*1000ULL*1000ULL },
                { "P", 1000ULL*1000ULL*1000ULL*1000ULL*1000ULL },
                { "T", 1000ULL*1000ULL*1000ULL*1000ULL },
                { "G", 1000ULL*1000ULL*1000ULL },
                { "M", 1000ULL*1000ULL },
                { "K", 1000ULL },
                { "B", 1ULL },
                { "",  1ULL },
        };

        ثابت ساختار table *table;
        ثابت نویسه *p;
        unsigned long long r = 0;
        unsigned n_entries, start_pos = 0;

        أكد(t);
        أكد(IN_SET(base, 1000, 1024));
        أكد(size);

        اگر (base == 1000) {
                table = si;
                n_entries = ELEMENTSOF(si);
        } وگرنه {
                table = iec;
                n_entries = ELEMENTSOF(iec);
        }

        p = t;
        انجامبده {
                unsigned long long l, tmp;
                دوگانه frac = 0;
                نویسه *e;
                unsigned i;

                p += strspn(p, WHITESPACE);

                errno = 0;
                l = strtoull(p, &e, 10);
                اگر (errno > 0)
                        بازگشت -errno;
                اگر (e == p)
                        بازگشت -EINVAL;
                اگر (*p == '-')
                        بازگشت -ERANGE;

                اگر (*e == '.') {
                        e++;

                        /* strtoull() itself would accept space/+/- */
                        اگر (ascii_isdigit(*e)) {
                                unsigned long long l2;
                                نویسه *e2;

                                l2 = strtoull(e, &e2, 10);
                                اگر (errno > 0)
                                        بازگشت -errno;

                                /* Ignore failure. E.g. 10.M is valid */
                                frac = l2;
                                برای (; e < e2; e++)
                                        frac /= 10;
                        }
                }

                e += strspn(e, WHITESPACE);

                برای (i = start_pos; i < n_entries; i++)
                        اگر (startswith(e, table[i].suffix))
                                بشکن;

                اگر (i >= n_entries)
                        بازگشت -EINVAL;

                اگر (l + (frac > 0) > ULLONG_MAX / table[i].factor)
                        بازگشت -ERANGE;

                tmp = l * table[i].factor + (unsigned long long) (frac * table[i].factor);
                اگر (tmp > ULLONG_MAX - r)
                        بازگشت -ERANGE;

                r += tmp;
                اگر ((unsigned long long) (uint64_t) r != r)
                        بازگشت -ERANGE;

                p = e + strlen(table[i].suffix);

                start_pos = i + 1;

        } وقتی (*p);

        *size = r;

        بازگشت 0;
}

صحیح parse_sector_size(ثابت نویسه *t, uint64_t *ret) {
        صحیح r;

        أكد(t);
        أكد(ret);

        uint64_t ss;

        r = safe_atou64(t, &ss);
        اگر (r < 0)
                بازگشت log_error_errno(r, "Failed to parse sector size parameter %s", t);
        اگر (ss < 512 || ss > 4096) /* Allow up to 4K due to dm-crypt support and 4K alignment by the homed LUKS backend */
                بازگشت log_error_errno(SYNTHETIC_ERRNO(ERANGE), "Sector size not between 512 and 4096: %s", t);
        اگر (!ISPOWEROF2(ss))
                بازگشت log_error_errno(SYNTHETIC_ERRNO(EINVAL), "Sector size not power of 2: %s", t);

        *ret = ss;
        بازگشت 0;
}

صحیح parse_range(ثابت نویسه *t, unsigned *lower, unsigned *upper) {
        _cleanup_free_ نویسه *word = پوچ;
        unsigned l, u;
        صحیح r;

        أكد(lower);
        أكد(upper);

        /* Extract the lower bound. */
        r = extract_first_word(&t, &word, "-", EXTRACT_DONT_COALESCE_SEPARATORS);
        اگر (r < 0)
                بازگشت r;
        اگر (r == 0)
                بازگشت -EINVAL;

        r = safe_atou(word, &l);
        اگر (r < 0)
                بازگشت r;

        /* Check for the upper bound and extract it if needed */
        اگر (!t)
                /* Single number with no dashes. */
                u = l;
        وگرنه اگر (!*t)
                /* Trailing dash is an error. */
                بازگشت -EINVAL;
        وگرنه {
                r = safe_atou(t, &u);
                اگر (r < 0)
                        بازگشت r;
        }

        *lower = l;
        *upper = u;
        بازگشت 0;
}

صحیح parse_errno(ثابت نویسه *t) {
        صحیح r, e;

        أكد(t);

        r = errno_from_name(t);
        اگر (r > 0)
                بازگشت r;

        r = safe_atoi(t, &e);
        اگر (r < 0)
                بازگشت r;

        /* 0 is also allowed here */
        اگر (!errno_is_valid(e) && e != 0)
                بازگشت -ERANGE;

        بازگشت e;
}

صحیح parse_fd(ثابت نویسه *t) {
        صحیح r, fd;

        أكد(t);

        r = safe_atoi(t, &fd);
        اگر (r < 0)
                بازگشت r;

        اگر (fd < 0)
                بازگشت -EBADF;

        بازگشت fd;
}

صحیح parse_user_shell(ثابت نویسه *s, نویسه **ret_sh, bool *ret_copy) {
        نویسه *sh;
        صحیح r;

        أكد(ret_sh);
        أكد(ret_copy);

        اگر (path_is_absolute(s) && path_is_normalized(s)) {
                sh = strdup(s);
                اگر (!sh)
                        بازگشت -ENOMEM;

                *ret_sh = sh;
                *ret_copy = false;
        } وگرنه {
                r = parse_boolean(s);
                اگر (r < 0)
                        بازگشت r;

                *ret_sh = پوچ;
                *ret_copy = r;
        }

        بازگشت 0;
}

ایستا ثابت نویسه *mangle_base(ثابت نویسه *s, unsigned *base) {
        ثابت نویسه *k;

        أكد(s);
        أكد(base);

        /* Base already explicitly specified, then don't do anything. */
        اگر (SAFE_ATO_MASK_FLAGS(*base) != 0)
                بازگشت s;

        /* Support Python 3 style "0b" and 0x" prefixes, because they truly make sense, much more than C's "0" prefix for octal. */
        k = STARTSWITH_SET(s, "0b", "0B");
        اگر (k) {
                *base = 2 | (*base & SAFE_ATO_ALL_FLAGS);
                بازگشت k;
        }

        k = STARTSWITH_SET(s, "0o", "0O");
        اگر (k) {
                *base = 8 | (*base & SAFE_ATO_ALL_FLAGS);
                بازگشت k;
        }

        بازگشت s;
}

صحیح safe_atou_full(ثابت نویسه *s, unsigned base, unsigned *ret_u) {
        نویسه *x = پوچ;
        unsigned long l;

        أكد(s);
        أكد(SAFE_ATO_MASK_FLAGS(base) <= 16);

        /* strtoul() is happy to parse negative values, and silently converts them to unsigned values without
         * generating an error. We want a clean error, hence let's look for the "-" prefix on our own, and
         * generate an error. But let's do so only after strtoul() validated that the string is clean
         * otherwise, so that we return EINVAL preferably over ERANGE. */

        اگر (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_WHITESPACE) &&
            strchr(WHITESPACE, s[0]))
                بازگشت -EINVAL;

        s += strspn(s, WHITESPACE);

        اگر (FLAGS_SET(base, SAFE_ATO_REFUSE_PLUS_MINUS) &&
            IN_SET(s[0], '+', '-'))
                بازگشت -EINVAL; /* Note that we check the "-" prefix again a second time below, but return a
                                 * different error. I.e. if the SAFE_ATO_REFUSE_PLUS_MINUS flag is set we
                                 * blanket refuse +/- prefixed integers, while if it is missing we'll just
                                 * return ERANGE, because the string actually parses correctly, but doesn't
                                 * fit in the return type. */

        اگر (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_ZERO) &&
            s[0] == '0' && !streq(s, "0"))
                بازگشت -EINVAL; /* This is particularly useful to avoid ambiguities between C's octal
                                 * notation and assumed-to-be-decimal integers with a leading zero. */

        s = mangle_base(s, &base);

        errno = 0;
        l = strtoul(s, &x, SAFE_ATO_MASK_FLAGS(base) /* Let's mask off the flags bits so that only the actual
                                                      * base is left */);
        اگر (errno > 0)
                بازگشت -errno;
        اگر (!x || x == s || *x != 0)
                بازگشت -EINVAL;
        اگر (l != 0 && s[0] == '-')
                بازگشت -ERANGE;
        اگر ((unsigned long) (unsigned) l != l)
                بازگشت -ERANGE;

        اگر (ret_u)
                *ret_u = (unsigned) l;

        بازگشت 0;
}

صحیح safe_atou_bounded(ثابت نویسه *s, unsigned min, unsigned max, unsigned *ret) {
        unsigned v;
        صحیح r;

        أكد(ret);

        r = safe_atou(s, &v);
        اگر (r < 0)
                بازگشت r;

        اگر (v < min || v > max)
                بازگشت -ERANGE;

        *ret = v;
        بازگشت 0;
}

صحیح safe_atoi(ثابت نویسه *s, صحیح *ret_i) {
        unsigned base = 0;
        نویسه *x = پوچ;
        long l;

        أكد(s);

        s += strspn(s, WHITESPACE);
        s = mangle_base(s, &base);

        errno = 0;
        l = strtol(s, &x, base);
        اگر (errno > 0)
                بازگشت -errno;
        اگر (!x || x == s || *x != 0)
                بازگشت -EINVAL;
        اگر ((long) (صحیح) l != l)
                بازگشت -ERANGE;

        اگر (ret_i)
                *ret_i = (صحیح) l;

        بازگشت 0;
}

صحیح safe_atollu_full(ثابت نویسه *s, unsigned base, unsigned long long *ret_llu) {
        نویسه *x = پوچ;
        unsigned long long l;

        أكد(s);
        أكد(SAFE_ATO_MASK_FLAGS(base) <= 16);

        اگر (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_WHITESPACE) &&
            strchr(WHITESPACE, s[0]))
                بازگشت -EINVAL;

        s += strspn(s, WHITESPACE);

        اگر (FLAGS_SET(base, SAFE_ATO_REFUSE_PLUS_MINUS) &&
            IN_SET(s[0], '+', '-'))
                بازگشت -EINVAL;

        اگر (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_ZERO) &&
            s[0] == '0' && s[1] != 0)
                بازگشت -EINVAL;

        s = mangle_base(s, &base);

        errno = 0;
        l = strtoull(s, &x, SAFE_ATO_MASK_FLAGS(base));
        اگر (errno > 0)
                بازگشت -errno;
        اگر (!x || x == s || *x != 0)
                بازگشت -EINVAL;
        اگر (l != 0 && s[0] == '-')
                بازگشت -ERANGE;

        اگر (ret_llu)
                *ret_llu = l;

        بازگشت 0;
}

صحیح safe_atolli(ثابت نویسه *s, long long *ret_lli) {
        unsigned base = 0;
        نویسه *x = پوچ;
        long long l;

        أكد(s);

        s += strspn(s, WHITESPACE);
        s = mangle_base(s, &base);

        errno = 0;
        l = strtoll(s, &x, base);
        اگر (errno > 0)
                بازگشت -errno;
        اگر (!x || x == s || *x != 0)
                بازگشت -EINVAL;

        اگر (ret_lli)
                *ret_lli = l;

        بازگشت 0;
}

صحیح safe_atou8_full(ثابت نویسه *s, unsigned base, uint8_t *ret) {
        unsigned u;
        صحیح r;

        أكد(ret);

        r = safe_atou_full(s, base, &u);
        اگر (r < 0)
                بازگشت r;
        اگر (u > UINT8_MAX)
                بازگشت -ERANGE;

        *ret = (uint8_t) u;
        بازگشت 0;
}

صحیح safe_atou16_full(ثابت نویسه *s, unsigned base, uint16_t *ret) {
        unsigned u;
        صحیح r;

        أكد(ret);

        r = safe_atou_full(s, base, &u);
        اگر (r < 0)
                بازگشت r;
        اگر (u > UINT16_MAX)
                بازگشت -ERANGE;

        *ret = (uint16_t) u;
        بازگشت 0;
}

صحیح safe_atoi16(ثابت نویسه *s, int16_t *ret) {
        unsigned base = 0;
        نویسه *x = پوچ;
        long l;

        أكد(s);

        s += strspn(s, WHITESPACE);
        s = mangle_base(s, &base);

        errno = 0;
        l = strtol(s, &x, base);
        اگر (errno > 0)
                بازگشت -errno;
        اگر (!x || x == s || *x != 0)
                بازگشت -EINVAL;
        اگر ((long) (int16_t) l != l)
                بازگشت -ERANGE;

        اگر (ret)
                *ret = (int16_t) l;

        بازگشت 0;
}

صحیح safe_atod(ثابت نویسه *s, دوگانه *ret_d) {
        _cleanup_(freelocalep) locale_t loc = (locale_t) 0;
        نویسه *x = پوچ;
        دوگانه d = 0;

        أكد(s);

        loc = newlocale(LC_NUMERIC_MASK, "C", (locale_t) 0);
        اگر (loc == (locale_t) 0)
                بازگشت -errno;

        errno = 0;
        d = strtod_l(s, &x, loc);
        اگر (errno > 0)
                بازگشت -errno;
        اگر (!x || x == s || *x != 0)
                بازگشت -EINVAL;

        اگر (ret_d)
                *ret_d = d;

        بازگشت 0;
}

صحیح parse_fractional_part_u(ثابت نویسه **p, size_t digits, unsigned *res) {
        unsigned val = 0;
        ثابت نویسه *s;

        أكد(p);
        أكد(res);

        s = *p;

        /* accept any number of digits, strtoull is limited to 19 */
        برای (size_t i = 0; i < digits; i++, s++) {
                اگر (!ascii_isdigit(*s)) {
                        اگر (i == 0)
                                بازگشت -EINVAL;

                        /* too few digits, pad with 0 */
                        برای (; i < digits; i++)
                                val *= 10;

                        بشکن;
                }

                val *= 10;
                val += *s - '0';
        }

        /* maybe round up */
        اگر (*s >= '5' && *s <= '9')
                val++;

        s += strspn(s, DIGITS);

        *p = s;
        *res = val;

        بازگشت 0;
}

صحیح parse_nice(ثابت نویسه *s, صحیح *ret) {
        صحیح n, r;

        أكد(ret);

        r = safe_atoi(s, &n);
        اگر (r < 0)
                بازگشت r;

        اگر (!nice_is_valid(n))
                بازگشت -ERANGE;

        *ret = n;
        بازگشت 0;
}

صحیح parse_ip_port(ثابت نویسه *s, uint16_t *ret) {
        uint16_t l;
        صحیح r;

        أكد(ret);

        r = safe_atou16_full(s, SAFE_ATO_REFUSE_LEADING_WHITESPACE, &l);
        اگر (r < 0)
                بازگشت r;

        اگر (l == 0)
                بازگشت -EINVAL;

        *ret = l;

        بازگشت 0;
}

صحیح parse_ip_port_range(ثابت نویسه *s, uint16_t *low, uint16_t *high, bool allow_zero) {
        unsigned l, h;
        صحیح r;

        أكد(low);
        أكد(high);

        r = parse_range(s, &l, &h);
        اگر (r < 0)
                بازگشت r;

        اگر (l > 65535 || h > 65535)
                بازگشت -EINVAL;

        اگر (!allow_zero && (l == 0 || h == 0))
                بازگشت -EINVAL;

        اگر (h < l)
                بازگشت -EINVAL;

        *low = l;
        *high = h;

        بازگشت 0;
}

صحیح parse_oom_score_adjust(ثابت نویسه *s, صحیح *ret) {
        صحیح r, v;

        أكد(s);
        أكد(ret);

        r = safe_atoi(s, &v);
        اگر (r < 0)
                بازگشت r;

        اگر (!oom_score_adjust_is_valid(v))
                بازگشت -ERANGE;

        *ret = v;
        بازگشت 0;
}

صحیح store_loadavg_fixed_point(unsigned long i, unsigned long f, loadavg_t *ret) {
        أكد(ret);

        /* Validate both parts *before* shifting them, so that the shifts themselves cannot overflow.
         * Rejecting f >= 100 is equivalent to the old check of the shifted value against
         * LOADAVG_FIXED_POINT_1_0, since DIV_ROUND_UP(99 << 11, 100) == 2028 and
         * DIV_ROUND_UP(100 << 11, 100) == 2048. */
        اگر (i > (~0UL >> LOADAVG_PRECISION_BITS))
                بازگشت -ERANGE;

        اگر (f >= 100)
                بازگشت -ERANGE;

        *ret = (i << LOADAVG_PRECISION_BITS) | DIV_ROUND_UP(f << LOADAVG_PRECISION_BITS, 100);
        بازگشت 0;
}

صحیح parse_loadavg_fixed_point(ثابت نویسه *s, loadavg_t *ret) {
        ثابت نویسه *d, *f_str, *i_str;
        unsigned long i, f;
        صحیح r;

        أكد(s);
        أكد(ret);

        d = strchr(s, '.');
        اگر (!d)
                بازگشت -EINVAL;

        i_str = strndupa_safe(s, d - s);
        f_str = d + 1;

        r = safe_atolu_full(i_str, 10, &i);
        اگر (r < 0)
                بازگشت r;

        r = safe_atolu_full(f_str, 10, &f);
        اگر (r < 0)
                بازگشت r;

        بازگشت store_loadavg_fixed_point(i, f, ret);
}

/* Limitations are described in https://www.netfilter.org/projects/nftables/manpage.html and
 * https://bugzilla.netfilter.org/show_bug.cgi?id=1175 */
bool nft_identifier_valid(ثابت نویسه *id) {
        اگر (isempty(id))
                بازگشت false;

        اگر (strlen(id) >= NFT_NAME_MAXLEN)
                بازگشت false;

        اگر (!ascii_isalpha(id[0]))
                بازگشت false;

        بازگشت in_charset(id + 1, ALPHANUMERICAL "/\\_.");
}

صحیح parse_capability_set(ثابت نویسه *s, uint64_t initial, uint64_t *current) {
        صحیح r;

        أكد(s);
        أكد(current);

        اگر (isempty(s)) {
                *current = CAP_MASK_UNSET;
                بازگشت 1;
        }

        bool invert = false;
        اگر (s[0] == '~') {
                invert = true;
                s++;
        }

        uint64_t parsed;
        r = capability_set_from_string(s, &parsed);
        اگر (r < 0)
                بازگشت r;

        اگر (parsed == 0 || *current == initial)
                /* "~" or uninitialized data -> replace */
                *current = invert ? all_capabilities() & ~parsed : parsed;
        وگرنه {
                /* previous data -> merge */
                اگر (invert)
                        *current &= ~parsed;
                وگرنه
                        *current |= parsed;
        }

        بازگشت r;
}
