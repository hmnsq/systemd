/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <linux/ipv6.h>
#include <linux/netfilter/nf_tables.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>

#include "alloc-util.h"
#include "capability-list.h"
#include "capability-util.h"
#include "errno-list.h"
#include "extract-word.h"
#include "locale-util.h"
#include "log.h"
#include "missing-network.h"
#include "parse-util.h"
#include "path-util.h"
#include "process-util.h"
#include "string-util.h"
#include "strv.h"

පූර්ණ_සංඛ්‍යා parse_boolean(const var_na *v) {
        නම් (!v)
                ආපසු -EINVAL;

        නම් (STRCASE_IN_SET(v,
                           "1",
                           "yes",
                           "y",
                           "true",
                           "t",
                           "on"))
                ආපසු 1;

        නම් (STRCASE_IN_SET(v,
                           "0",
                           "no",
                           "n",
                           "false",
                           "f",
                           "off"))
                ආපසු 0;

        ආපසු -EINVAL;
}

පූර්ණ_සංඛ්‍යා parse_tristate_full(const var_na *v, const var_na *third, පූර්ණ_සංඛ්‍යා *ret) {
        පූර්ණ_සංඛ්‍යා r;

        නම් (isempty(v) || streq_ptr(v, third)) { /* Empty string is always taken as the third/invalid/auto state */
                නම් (ret)
                        *ret = -1;
        } නැතහොත් {
                r = parse_boolean(v);
                නම් (r < 0)
                        ආපසු r;

                නම් (ret)
                        *ret = r;
        }

        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා parse_pid(const var_na *s, pid_t *ret) {
        unsigned long ul = 0;
        pid_t pid;
        පූර්ණ_සංඛ්‍යා r;

        assert(s);

        r = safe_atolu(s, &ul);
        නම් (r < 0)
                ආපසු r;

        pid = (pid_t) ul;

        නම් ((unsigned long) pid != ul)
                ආපසු -ERANGE;

        නම් (!pid_is_valid(pid))
                ආපසු -ERANGE;

        නම් (ret)
                *ret = pid;
        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා parse_mode(const var_na *s, mode_t *ret) {
        unsigned m;
        පූර්ණ_සංඛ්‍යා r;

        assert(s);

        r = safe_atou_full(s, 8 |
                           SAFE_ATO_REFUSE_PLUS_MINUS, /* Leading '+' or even '-' char? that's just weird,
                                                        * refuse. User might have wanted to add mode flags or
                                                        * so, but this parser doesn't allow that, so let's
                                                        * better be safe. */
                           &m);
        නම් (r < 0)
                ආපසු r;
        නම් (m > 07777)
                ආපසු -ERANGE;

        නම් (ret)
                *ret = m;
        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා parse_ifindex(const var_na *s) {
        පූර්ණ_සංඛ්‍යා ifi, r;

        assert(s);

        r = safe_atoi(s, &ifi);
        නම් (r < 0)
                ආපසු r;
        නම් (ifi <= 0)
                ආපසු -EINVAL;

        ආපසු ifi;
}

පූර්ණ_සංඛ්‍යා parse_mtu(පූර්ණ_සංඛ්‍යා family, const var_na *s, uint32_t *ret) {
        uint64_t u, m;
        පූර්ණ_සංඛ්‍යා r;

        assert(ret);

        r = parse_size(s, 1024, &u);
        නම් (r < 0)
                ආපසු r;

        නම් (u > UINT32_MAX)
                ආපසු -ERANGE;

        switch (family) {
        pak_sa AF_INET:
                m = IPV4_MIN_MTU; /* This is 68 */
                bha_nga;
        pak_sa AF_INET6:
                m = IPV6_MIN_MTU; /* This is 1280 */
                bha_nga;
        default:
                m = 0;
        }

        නම් (u < m)
                ආපසු -ERANGE;

        *ret = (uint32_t) u;
        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා parse_size(const var_na *t, uint64_t base, uint64_t *size) {

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

        sa_nracanaa table {
                const var_na *suffix;
                unsigned long long factor;
        };

        static const sa_nracanaa table iec[] = {
                { "E", 1024ULL*1024ULL*1024ULL*1024ULL*1024ULL*1024ULL },
                { "P", 1024ULL*1024ULL*1024ULL*1024ULL*1024ULL },
                { "T", 1024ULL*1024ULL*1024ULL*1024ULL },
                { "G", 1024ULL*1024ULL*1024ULL },
                { "M", 1024ULL*1024ULL },
                { "K", 1024ULL },
                { "B", 1ULL },
                { "",  1ULL },
        };

        static const sa_nracanaa table si[] = {
                { "E", 1000ULL*1000ULL*1000ULL*1000ULL*1000ULL*1000ULL },
                { "P", 1000ULL*1000ULL*1000ULL*1000ULL*1000ULL },
                { "T", 1000ULL*1000ULL*1000ULL*1000ULL },
                { "G", 1000ULL*1000ULL*1000ULL },
                { "M", 1000ULL*1000ULL },
                { "K", 1000ULL },
                { "B", 1ULL },
                { "",  1ULL },
        };

        const sa_nracanaa table *table;
        const var_na *p;
        unsigned long long r = 0;
        unsigned n_entries, start_pos = 0;

        assert(t);
        assert(IN_SET(base, 1000, 1024));
        assert(size);

        නම් (base == 1000) {
                table = si;
                n_entries = ELEMENTSOF(si);
        } නැතහොත් {
                table = iec;
                n_entries = ELEMENTSOF(iec);
        }

        p = t;
        do {
                unsigned long long l, tmp;
                dviguu_na frac = 0;
                var_na *e;
                unsigned i;

                p += strspn(p, WHITESPACE);

                errno = 0;
                l = strtoull(p, &e, 10);
                නම් (errno > 0)
                        ආපසු -errno;
                නම් (e == p)
                        ආපසු -EINVAL;
                නම් (*p == '-')
                        ආපසු -ERANGE;

                නම් (*e == '.') {
                        e++;

                        /* strtoull() itself would accept space/+/- */
                        නම් (ascii_isdigit(*e)) {
                                unsigned long long l2;
                                var_na *e2;

                                l2 = strtoull(e, &e2, 10);
                                නම් (errno > 0)
                                        ආපසු -errno;

                                /* Ignore failure. E.g. 10.M is valid */
                                frac = l2;
                                සඳහා (; e < e2; e++)
                                        frac /= 10;
                        }
                }

                e += strspn(e, WHITESPACE);

                සඳහා (i = start_pos; i < n_entries; i++)
                        නම් (startswith(e, table[i].suffix))
                                bha_nga;

                නම් (i >= n_entries)
                        ආපසු -EINVAL;

                නම් (l + (frac > 0) > ULLONG_MAX / table[i].factor)
                        ආපසු -ERANGE;

                tmp = l * table[i].factor + (unsigned long long) (frac * table[i].factor);
                නම් (tmp > ULLONG_MAX - r)
                        ආපසු -ERANGE;

                r += tmp;
                නම් ((unsigned long long) (uint64_t) r != r)
                        ආපසු -ERANGE;

                p = e + strlen(table[i].suffix);

                start_pos = i + 1;

        } අතරතුර (*p);

        *size = r;

        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා parse_sector_size(const var_na *t, uint64_t *ret) {
        පූර්ණ_සංඛ්‍යා r;

        assert(t);
        assert(ret);

        uint64_t ss;

        r = safe_atou64(t, &ss);
        නම් (r < 0)
                ආපසු log_error_errno(r, "Failed to parse sector size parameter %s", t);
        නම් (ss < 512 || ss > 4096) /* Allow up to 4K due to dm-crypt support and 4K alignment by the homed LUKS backend */
                ආපසු log_error_errno(SYNTHETIC_ERRNO(ERANGE), "Sector size not between 512 and 4096: %s", t);
        නම් (!ISPOWEROF2(ss))
                ආපසු log_error_errno(SYNTHETIC_ERRNO(EINVAL), "Sector size not power of 2: %s", t);

        *ret = ss;
        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා parse_range(const var_na *t, unsigned *lower, unsigned *upper) {
        _cleanup_free_ var_na *word = NULL;
        unsigned l, u;
        පූර්ණ_සංඛ්‍යා r;

        assert(lower);
        assert(upper);

        /* Extract the lower bound. */
        r = extract_first_word(&t, &word, "-", EXTRACT_DONT_COALESCE_SEPARATORS);
        නම් (r < 0)
                ආපසු r;
        නම් (r == 0)
                ආපසු -EINVAL;

        r = safe_atou(word, &l);
        නම් (r < 0)
                ආපසු r;

        /* Check for the upper bound and extract it if needed */
        නම් (!t)
                /* Single number with no dashes. */
                u = l;
        නැතහොත් නම් (!*t)
                /* Trailing dash is an error. */
                ආපසු -EINVAL;
        නැතහොත් {
                r = safe_atou(t, &u);
                නම් (r < 0)
                        ආපසු r;
        }

        *lower = l;
        *upper = u;
        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා parse_errno(const var_na *t) {
        පූර්ණ_සංඛ්‍යා r, e;

        assert(t);

        r = errno_from_name(t);
        නම් (r > 0)
                ආපසු r;

        r = safe_atoi(t, &e);
        නම් (r < 0)
                ආපසු r;

        /* 0 is also allowed here */
        නම් (!errno_is_valid(e) && e != 0)
                ආපසු -ERANGE;

        ආපසු e;
}

පූර්ණ_සංඛ්‍යා parse_fd(const var_na *t) {
        පූර්ණ_සංඛ්‍යා r, fd;

        assert(t);

        r = safe_atoi(t, &fd);
        නම් (r < 0)
                ආපසු r;

        නම් (fd < 0)
                ආපසු -EBADF;

        ආපසු fd;
}

පූර්ණ_සංඛ්‍යා parse_user_shell(const var_na *s, var_na **ret_sh, bool *ret_copy) {
        var_na *sh;
        පූර්ණ_සංඛ්‍යා r;

        assert(ret_sh);
        assert(ret_copy);

        නම් (path_is_absolute(s) && path_is_normalized(s)) {
                sh = strdup(s);
                නම් (!sh)
                        ආපසු -ENOMEM;

                *ret_sh = sh;
                *ret_copy = false;
        } නැතහොත් {
                r = parse_boolean(s);
                නම් (r < 0)
                        ආපසු r;

                *ret_sh = NULL;
                *ret_copy = r;
        }

        ආපසු 0;
}

static const var_na *mangle_base(const var_na *s, unsigned *base) {
        const var_na *k;

        assert(s);
        assert(base);

        /* Base already explicitly specified, then don't do anything. */
        නම් (SAFE_ATO_MASK_FLAGS(*base) != 0)
                ආපසු s;

        /* Support Python 3 style "0b" and 0x" prefixes, because they truly make sense, much more than C's "0" prefix for octal. */
        k = STARTSWITH_SET(s, "0b", "0B");
        නම් (k) {
                *base = 2 | (*base & SAFE_ATO_ALL_FLAGS);
                ආපසු k;
        }

        k = STARTSWITH_SET(s, "0o", "0O");
        නම් (k) {
                *base = 8 | (*base & SAFE_ATO_ALL_FLAGS);
                ආපසු k;
        }

        ආපසු s;
}

පූර්ණ_සංඛ්‍යා safe_atou_full(const var_na *s, unsigned base, unsigned *ret_u) {
        var_na *x = NULL;
        unsigned long l;

        assert(s);
        assert(SAFE_ATO_MASK_FLAGS(base) <= 16);

        /* strtoul() is happy to parse negative values, and silently converts them to unsigned values without
         * generating an error. We want a clean error, hence let's look for the "-" prefix on our own, and
         * generate an error. But let's do so only after strtoul() validated that the string is clean
         * otherwise, so that we return EINVAL preferably over ERANGE. */

        නම් (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_WHITESPACE) &&
            strchr(WHITESPACE, s[0]))
                ආපසු -EINVAL;

        s += strspn(s, WHITESPACE);

        නම් (FLAGS_SET(base, SAFE_ATO_REFUSE_PLUS_MINUS) &&
            IN_SET(s[0], '+', '-'))
                ආපසු -EINVAL; /* Note that we check the "-" prefix again a second time below, but return a
                                 * different error. I.e. if the SAFE_ATO_REFUSE_PLUS_MINUS flag is set we
                                 * blanket refuse +/- prefixed integers, while if it is missing we'll just
                                 * return ERANGE, because the string actually parses correctly, but doesn't
                                 * fit in the return type. */

        නම් (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_ZERO) &&
            s[0] == '0' && !streq(s, "0"))
                ආපසු -EINVAL; /* This is particularly useful to avoid ambiguities between C's octal
                                 * notation and assumed-to-be-decimal integers with a leading zero. */

        s = mangle_base(s, &base);

        errno = 0;
        l = strtoul(s, &x, SAFE_ATO_MASK_FLAGS(base) /* Let's mask off the flags bits so that only the actual
                                                      * base is left */);
        නම් (errno > 0)
                ආපසු -errno;
        නම් (!x || x == s || *x != 0)
                ආපසු -EINVAL;
        නම් (l != 0 && s[0] == '-')
                ආපසු -ERANGE;
        නම් ((unsigned long) (unsigned) l != l)
                ආපසු -ERANGE;

        නම් (ret_u)
                *ret_u = (unsigned) l;

        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා safe_atou_bounded(const var_na *s, unsigned min, unsigned max, unsigned *ret) {
        unsigned v;
        පූර්ණ_සංඛ්‍යා r;

        assert(ret);

        r = safe_atou(s, &v);
        නම් (r < 0)
                ආපසු r;

        නම් (v < min || v > max)
                ආපසු -ERANGE;

        *ret = v;
        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා safe_atoi(const var_na *s, පූර්ණ_සංඛ්‍යා *ret_i) {
        unsigned base = 0;
        var_na *x = NULL;
        long l;

        assert(s);

        s += strspn(s, WHITESPACE);
        s = mangle_base(s, &base);

        errno = 0;
        l = strtol(s, &x, base);
        නම් (errno > 0)
                ආපසු -errno;
        නම් (!x || x == s || *x != 0)
                ආපසු -EINVAL;
        නම් ((long) (පූර්ණ_සංඛ්‍යා) l != l)
                ආපසු -ERANGE;

        නම් (ret_i)
                *ret_i = (පූර්ණ_සංඛ්‍යා) l;

        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා safe_atollu_full(const var_na *s, unsigned base, unsigned long long *ret_llu) {
        var_na *x = NULL;
        unsigned long long l;

        assert(s);
        assert(SAFE_ATO_MASK_FLAGS(base) <= 16);

        නම් (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_WHITESPACE) &&
            strchr(WHITESPACE, s[0]))
                ආපසු -EINVAL;

        s += strspn(s, WHITESPACE);

        නම් (FLAGS_SET(base, SAFE_ATO_REFUSE_PLUS_MINUS) &&
            IN_SET(s[0], '+', '-'))
                ආපසු -EINVAL;

        නම් (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_ZERO) &&
            s[0] == '0' && s[1] != 0)
                ආපසු -EINVAL;

        s = mangle_base(s, &base);

        errno = 0;
        l = strtoull(s, &x, SAFE_ATO_MASK_FLAGS(base));
        නම් (errno > 0)
                ආපසු -errno;
        නම් (!x || x == s || *x != 0)
                ආපසු -EINVAL;
        නම් (l != 0 && s[0] == '-')
                ආපසු -ERANGE;

        නම් (ret_llu)
                *ret_llu = l;

        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා safe_atolli(const var_na *s, long long *ret_lli) {
        unsigned base = 0;
        var_na *x = NULL;
        long long l;

        assert(s);

        s += strspn(s, WHITESPACE);
        s = mangle_base(s, &base);

        errno = 0;
        l = strtoll(s, &x, base);
        නම් (errno > 0)
                ආපසු -errno;
        නම් (!x || x == s || *x != 0)
                ආපසු -EINVAL;

        නම් (ret_lli)
                *ret_lli = l;

        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා safe_atou8_full(const var_na *s, unsigned base, uint8_t *ret) {
        unsigned u;
        පූර්ණ_සංඛ්‍යා r;

        assert(ret);

        r = safe_atou_full(s, base, &u);
        නම් (r < 0)
                ආපසු r;
        නම් (u > UINT8_MAX)
                ආපසු -ERANGE;

        *ret = (uint8_t) u;
        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා safe_atou16_full(const var_na *s, unsigned base, uint16_t *ret) {
        unsigned u;
        පූර්ණ_සංඛ්‍යා r;

        assert(ret);

        r = safe_atou_full(s, base, &u);
        නම් (r < 0)
                ආපසු r;
        නම් (u > UINT16_MAX)
                ආපසු -ERANGE;

        *ret = (uint16_t) u;
        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා safe_atoi16(const var_na *s, int16_t *ret) {
        unsigned base = 0;
        var_na *x = NULL;
        long l;

        assert(s);

        s += strspn(s, WHITESPACE);
        s = mangle_base(s, &base);

        errno = 0;
        l = strtol(s, &x, base);
        නම් (errno > 0)
                ආපසු -errno;
        නම් (!x || x == s || *x != 0)
                ආපසු -EINVAL;
        නම් ((long) (int16_t) l != l)
                ආපසු -ERANGE;

        නම් (ret)
                *ret = (int16_t) l;

        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා safe_atod(const var_na *s, dviguu_na *ret_d) {
        _cleanup_(freelocalep) locale_t loc = (locale_t) 0;
        var_na *x = NULL;
        dviguu_na d = 0;

        assert(s);

        loc = newlocale(LC_NUMERIC_MASK, "C", (locale_t) 0);
        නම් (loc == (locale_t) 0)
                ආපසු -errno;

        errno = 0;
        d = strtod_l(s, &x, loc);
        නම් (errno > 0)
                ආපසු -errno;
        නම් (!x || x == s || *x != 0)
                ආපසු -EINVAL;

        නම් (ret_d)
                *ret_d = d;

        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා parse_fractional_part_u(const var_na **p, size_t digits, unsigned *res) {
        unsigned val = 0;
        const var_na *s;

        assert(p);
        assert(res);

        s = *p;

        /* accept any number of digits, strtoull is limited to 19 */
        සඳහා (size_t i = 0; i < digits; i++, s++) {
                නම් (!ascii_isdigit(*s)) {
                        නම් (i == 0)
                                ආපසු -EINVAL;

                        /* too few digits, pad with 0 */
                        සඳහා (; i < digits; i++)
                                val *= 10;

                        bha_nga;
                }

                val *= 10;
                val += *s - '0';
        }

        /* maybe round up */
        නම් (*s >= '5' && *s <= '9')
                val++;

        s += strspn(s, DIGITS);

        *p = s;
        *res = val;

        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා parse_nice(const var_na *s, පූර්ණ_සංඛ්‍යා *ret) {
        පූර්ණ_සංඛ්‍යා n, r;

        assert(ret);

        r = safe_atoi(s, &n);
        නම් (r < 0)
                ආපසු r;

        නම් (!nice_is_valid(n))
                ආපසු -ERANGE;

        *ret = n;
        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා parse_ip_port(const var_na *s, uint16_t *ret) {
        uint16_t l;
        පූර්ණ_සංඛ්‍යා r;

        assert(ret);

        r = safe_atou16_full(s, SAFE_ATO_REFUSE_LEADING_WHITESPACE, &l);
        නම් (r < 0)
                ආපසු r;

        නම් (l == 0)
                ආපසු -EINVAL;

        *ret = l;

        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා parse_ip_port_range(const var_na *s, uint16_t *low, uint16_t *high, bool allow_zero) {
        unsigned l, h;
        පූර්ණ_සංඛ්‍යා r;

        assert(low);
        assert(high);

        r = parse_range(s, &l, &h);
        නම් (r < 0)
                ආපසු r;

        නම් (l > 65535 || h > 65535)
                ආපසු -EINVAL;

        නම් (!allow_zero && (l == 0 || h == 0))
                ආපසු -EINVAL;

        නම් (h < l)
                ආපසු -EINVAL;

        *low = l;
        *high = h;

        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා parse_oom_score_adjust(const var_na *s, පූර්ණ_සංඛ්‍යා *ret) {
        පූර්ණ_සංඛ්‍යා r, v;

        assert(s);
        assert(ret);

        r = safe_atoi(s, &v);
        නම් (r < 0)
                ආපසු r;

        නම් (!oom_score_adjust_is_valid(v))
                ආපසු -ERANGE;

        *ret = v;
        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා store_loadavg_fixed_point(unsigned long i, unsigned long f, loadavg_t *ret) {
        assert(ret);

        /* Validate both parts *before* shifting them, so that the shifts themselves cannot overflow.
         * Rejecting f >= 100 is equivalent to the old check of the shifted value against
         * LOADAVG_FIXED_POINT_1_0, since DIV_ROUND_UP(99 << 11, 100) == 2028 and
         * DIV_ROUND_UP(100 << 11, 100) == 2048. */
        නම් (i > (~0UL >> LOADAVG_PRECISION_BITS))
                ආපසු -ERANGE;

        නම් (f >= 100)
                ආපසු -ERANGE;

        *ret = (i << LOADAVG_PRECISION_BITS) | DIV_ROUND_UP(f << LOADAVG_PRECISION_BITS, 100);
        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා parse_loadavg_fixed_point(const var_na *s, loadavg_t *ret) {
        const var_na *d, *f_str, *i_str;
        unsigned long i, f;
        පූර්ණ_සංඛ්‍යා r;

        assert(s);
        assert(ret);

        d = strchr(s, '.');
        නම් (!d)
                ආපසු -EINVAL;

        i_str = strndupa_safe(s, d - s);
        f_str = d + 1;

        r = safe_atolu_full(i_str, 10, &i);
        නම් (r < 0)
                ආපසු r;

        r = safe_atolu_full(f_str, 10, &f);
        නම් (r < 0)
                ආපසු r;

        ආපසු store_loadavg_fixed_point(i, f, ret);
}

/* Limitations are described in https://www.netfilter.org/projects/nftables/manpage.html and
 * https://bugzilla.netfilter.org/show_bug.cgi?id=1175 */
bool nft_identifier_valid(const var_na *id) {
        නම් (isempty(id))
                ආපසු false;

        නම් (strlen(id) >= NFT_NAME_MAXLEN)
                ආපසු false;

        නම් (!ascii_isalpha(id[0]))
                ආපසු false;

        ආපසු in_charset(id + 1, ALPHANUMERICAL "/\\_.");
}

පූර්ණ_සංඛ්‍යා parse_capability_set(const var_na *s, uint64_t initial, uint64_t *current) {
        පූර්ණ_සංඛ්‍යා r;

        assert(s);
        assert(current);

        නම් (isempty(s)) {
                *current = CAP_MASK_UNSET;
                ආපසු 1;
        }

        bool invert = false;
        නම් (s[0] == '~') {
                invert = true;
                s++;
        }

        uint64_t parsed;
        r = capability_set_from_string(s, &parsed);
        නම් (r < 0)
                ආපසු r;

        නම් (parsed == 0 || *current == initial)
                /* "~" or uninitialized data -> replace */
                *current = invert ? all_capabilities() & ~parsed : parsed;
        නැතහොත් {
                /* previous data -> merge */
                නම් (invert)
                        *current &= ~parsed;
                නැතහොත්
                        *current |= parsed;
        }

        ආපසු r;
}
