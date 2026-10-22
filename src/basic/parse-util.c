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

ཧྲིལ parse_boolean(const var_na *v) {
        གལ་སྲིད (!v)
                ལོག -EINVAL;

        གལ་སྲིད (STRCASE_IN_SET(v,
                           "1",
                           "yes",
                           "y",
                           "true",
                           "t",
                           "on"))
                ལོག 1;

        གལ་སྲིད (STRCASE_IN_SET(v,
                           "0",
                           "no",
                           "n",
                           "false",
                           "f",
                           "off"))
                ལོག 0;

        ལོག -EINVAL;
}

ཧྲིལ parse_tristate_full(const var_na *v, const var_na *third, ཧྲིལ *ret) {
        ཧྲིལ r;

        གལ་སྲིད (isempty(v) || streq_ptr(v, third)) { /* Empty string is always taken as the third/invalid/auto state */
                གལ་སྲིད (ret)
                        *ret = -1;
        } དེ་མིན {
                r = parse_boolean(v);
                གལ་སྲིད (r < 0)
                        ལོག r;

                གལ་སྲིད (ret)
                        *ret = r;
        }

        ལོག 0;
}

ཧྲིལ parse_pid(const var_na *s, pid_t *ret) {
        unsigned long ul = 0;
        pid_t pid;
        ཧྲིལ r;

        assert(s);

        r = safe_atolu(s, &ul);
        གལ་སྲིད (r < 0)
                ལོག r;

        pid = (pid_t) ul;

        གལ་སྲིད ((unsigned long) pid != ul)
                ལོག -ERANGE;

        གལ་སྲིད (!pid_is_valid(pid))
                ལོག -ERANGE;

        གལ་སྲིད (ret)
                *ret = pid;
        ལོག 0;
}

ཧྲིལ parse_mode(const var_na *s, mode_t *ret) {
        unsigned m;
        ཧྲིལ r;

        assert(s);

        r = safe_atou_full(s, 8 |
                           SAFE_ATO_REFUSE_PLUS_MINUS, /* Leading '+' or even '-' char? that's just weird,
                                                        * refuse. User might have wanted to add mode flags or
                                                        * so, but this parser doesn't allow that, so let's
                                                        * better be safe. */
                           &m);
        གལ་སྲིད (r < 0)
                ལོག r;
        གལ་སྲིད (m > 07777)
                ལོག -ERANGE;

        གལ་སྲིད (ret)
                *ret = m;
        ལོག 0;
}

ཧྲིལ parse_ifindex(const var_na *s) {
        ཧྲིལ ifi, r;

        assert(s);

        r = safe_atoi(s, &ifi);
        གལ་སྲིད (r < 0)
                ལོག r;
        གལ་སྲིད (ifi <= 0)
                ལོག -EINVAL;

        ལོག ifi;
}

ཧྲིལ parse_mtu(ཧྲིལ family, const var_na *s, uint32_t *ret) {
        uint64_t u, m;
        ཧྲིལ r;

        assert(ret);

        r = parse_size(s, 1024, &u);
        གལ་སྲིད (r < 0)
                ལོག r;

        གལ་སྲིད (u > UINT32_MAX)
                ལོག -ERANGE;

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

        གལ་སྲིད (u < m)
                ལོག -ERANGE;

        *ret = (uint32_t) u;
        ལོག 0;
}

ཧྲིལ parse_size(const var_na *t, uint64_t base, uint64_t *size) {

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

        གལ་སྲིད (base == 1000) {
                table = si;
                n_entries = ELEMENTSOF(si);
        } དེ་མིན {
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
                གལ་སྲིད (errno > 0)
                        ལོག -errno;
                གལ་སྲིད (e == p)
                        ལོག -EINVAL;
                གལ་སྲིད (*p == '-')
                        ལོག -ERANGE;

                གལ་སྲིད (*e == '.') {
                        e++;

                        /* strtoull() itself would accept space/+/- */
                        གལ་སྲིད (ascii_isdigit(*e)) {
                                unsigned long long l2;
                                var_na *e2;

                                l2 = strtoull(e, &e2, 10);
                                གལ་སྲིད (errno > 0)
                                        ལོག -errno;

                                /* Ignore failure. E.g. 10.M is valid */
                                frac = l2;
                                དོན་ལུ (; e < e2; e++)
                                        frac /= 10;
                        }
                }

                e += strspn(e, WHITESPACE);

                དོན་ལུ (i = start_pos; i < n_entries; i++)
                        གལ་སྲིད (startswith(e, table[i].suffix))
                                bha_nga;

                གལ་སྲིད (i >= n_entries)
                        ལོག -EINVAL;

                གལ་སྲིད (l + (frac > 0) > ULLONG_MAX / table[i].factor)
                        ལོག -ERANGE;

                tmp = l * table[i].factor + (unsigned long long) (frac * table[i].factor);
                གལ་སྲིད (tmp > ULLONG_MAX - r)
                        ལོག -ERANGE;

                r += tmp;
                གལ་སྲིད ((unsigned long long) (uint64_t) r != r)
                        ལོག -ERANGE;

                p = e + strlen(table[i].suffix);

                start_pos = i + 1;

        } སྐབས (*p);

        *size = r;

        ལོག 0;
}

ཧྲིལ parse_sector_size(const var_na *t, uint64_t *ret) {
        ཧྲིལ r;

        assert(t);
        assert(ret);

        uint64_t ss;

        r = safe_atou64(t, &ss);
        གལ་སྲིད (r < 0)
                ལོག log_error_errno(r, "Failed to parse sector size parameter %s", t);
        གལ་སྲིད (ss < 512 || ss > 4096) /* Allow up to 4K due to dm-crypt support and 4K alignment by the homed LUKS backend */
                ལོག log_error_errno(SYNTHETIC_ERRNO(ERANGE), "Sector size not between 512 and 4096: %s", t);
        གལ་སྲིད (!ISPOWEROF2(ss))
                ལོག log_error_errno(SYNTHETIC_ERRNO(EINVAL), "Sector size not power of 2: %s", t);

        *ret = ss;
        ལོག 0;
}

ཧྲིལ parse_range(const var_na *t, unsigned *lower, unsigned *upper) {
        _cleanup_free_ var_na *word = NULL;
        unsigned l, u;
        ཧྲིལ r;

        assert(lower);
        assert(upper);

        /* Extract the lower bound. */
        r = extract_first_word(&t, &word, "-", EXTRACT_DONT_COALESCE_SEPARATORS);
        གལ་སྲིད (r < 0)
                ལོག r;
        གལ་སྲིད (r == 0)
                ལོག -EINVAL;

        r = safe_atou(word, &l);
        གལ་སྲིད (r < 0)
                ལོག r;

        /* Check for the upper bound and extract it if needed */
        གལ་སྲིད (!t)
                /* Single number with no dashes. */
                u = l;
        དེ་མིན གལ་སྲིད (!*t)
                /* Trailing dash is an error. */
                ལོག -EINVAL;
        དེ་མིན {
                r = safe_atou(t, &u);
                གལ་སྲིད (r < 0)
                        ལོག r;
        }

        *lower = l;
        *upper = u;
        ལོག 0;
}

ཧྲིལ parse_errno(const var_na *t) {
        ཧྲིལ r, e;

        assert(t);

        r = errno_from_name(t);
        གལ་སྲིད (r > 0)
                ལོག r;

        r = safe_atoi(t, &e);
        གལ་སྲིད (r < 0)
                ལོག r;

        /* 0 is also allowed here */
        གལ་སྲིད (!errno_is_valid(e) && e != 0)
                ལོག -ERANGE;

        ལོག e;
}

ཧྲིལ parse_fd(const var_na *t) {
        ཧྲིལ r, fd;

        assert(t);

        r = safe_atoi(t, &fd);
        གལ་སྲིད (r < 0)
                ལོག r;

        གལ་སྲིད (fd < 0)
                ལོག -EBADF;

        ལོག fd;
}

ཧྲིལ parse_user_shell(const var_na *s, var_na **ret_sh, bool *ret_copy) {
        var_na *sh;
        ཧྲིལ r;

        assert(ret_sh);
        assert(ret_copy);

        གལ་སྲིད (path_is_absolute(s) && path_is_normalized(s)) {
                sh = strdup(s);
                གལ་སྲིད (!sh)
                        ལོག -ENOMEM;

                *ret_sh = sh;
                *ret_copy = false;
        } དེ་མིན {
                r = parse_boolean(s);
                གལ་སྲིད (r < 0)
                        ལོག r;

                *ret_sh = NULL;
                *ret_copy = r;
        }

        ལོག 0;
}

static const var_na *mangle_base(const var_na *s, unsigned *base) {
        const var_na *k;

        assert(s);
        assert(base);

        /* Base already explicitly specified, then don't do anything. */
        གལ་སྲིད (SAFE_ATO_MASK_FLAGS(*base) != 0)
                ལོག s;

        /* Support Python 3 style "0b" and 0x" prefixes, because they truly make sense, much more than C's "0" prefix for octal. */
        k = STARTSWITH_SET(s, "0b", "0B");
        གལ་སྲིད (k) {
                *base = 2 | (*base & SAFE_ATO_ALL_FLAGS);
                ལོག k;
        }

        k = STARTSWITH_SET(s, "0o", "0O");
        གལ་སྲིད (k) {
                *base = 8 | (*base & SAFE_ATO_ALL_FLAGS);
                ལོག k;
        }

        ལོག s;
}

ཧྲིལ safe_atou_full(const var_na *s, unsigned base, unsigned *ret_u) {
        var_na *x = NULL;
        unsigned long l;

        assert(s);
        assert(SAFE_ATO_MASK_FLAGS(base) <= 16);

        /* strtoul() is happy to parse negative values, and silently converts them to unsigned values without
         * generating an error. We want a clean error, hence let's look for the "-" prefix on our own, and
         * generate an error. But let's do so only after strtoul() validated that the string is clean
         * otherwise, so that we return EINVAL preferably over ERANGE. */

        གལ་སྲིད (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_WHITESPACE) &&
            strchr(WHITESPACE, s[0]))
                ལོག -EINVAL;

        s += strspn(s, WHITESPACE);

        གལ་སྲིད (FLAGS_SET(base, SAFE_ATO_REFUSE_PLUS_MINUS) &&
            IN_SET(s[0], '+', '-'))
                ལོག -EINVAL; /* Note that we check the "-" prefix again a second time below, but return a
                                 * different error. I.e. if the SAFE_ATO_REFUSE_PLUS_MINUS flag is set we
                                 * blanket refuse +/- prefixed integers, while if it is missing we'll just
                                 * return ERANGE, because the string actually parses correctly, but doesn't
                                 * fit in the return type. */

        གལ་སྲིད (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_ZERO) &&
            s[0] == '0' && !streq(s, "0"))
                ལོག -EINVAL; /* This is particularly useful to avoid ambiguities between C's octal
                                 * notation and assumed-to-be-decimal integers with a leading zero. */

        s = mangle_base(s, &base);

        errno = 0;
        l = strtoul(s, &x, SAFE_ATO_MASK_FLAGS(base) /* Let's mask off the flags bits so that only the actual
                                                      * base is left */);
        གལ་སྲིད (errno > 0)
                ལོག -errno;
        གལ་སྲིད (!x || x == s || *x != 0)
                ལོག -EINVAL;
        གལ་སྲིད (l != 0 && s[0] == '-')
                ལོག -ERANGE;
        གལ་སྲིད ((unsigned long) (unsigned) l != l)
                ལོག -ERANGE;

        གལ་སྲིད (ret_u)
                *ret_u = (unsigned) l;

        ལོག 0;
}

ཧྲིལ safe_atou_bounded(const var_na *s, unsigned min, unsigned max, unsigned *ret) {
        unsigned v;
        ཧྲིལ r;

        assert(ret);

        r = safe_atou(s, &v);
        གལ་སྲིད (r < 0)
                ལོག r;

        གལ་སྲིད (v < min || v > max)
                ལོག -ERANGE;

        *ret = v;
        ལོག 0;
}

ཧྲིལ safe_atoi(const var_na *s, ཧྲིལ *ret_i) {
        unsigned base = 0;
        var_na *x = NULL;
        long l;

        assert(s);

        s += strspn(s, WHITESPACE);
        s = mangle_base(s, &base);

        errno = 0;
        l = strtol(s, &x, base);
        གལ་སྲིད (errno > 0)
                ལོག -errno;
        གལ་སྲིད (!x || x == s || *x != 0)
                ལོག -EINVAL;
        གལ་སྲིད ((long) (ཧྲིལ) l != l)
                ལོག -ERANGE;

        གལ་སྲིད (ret_i)
                *ret_i = (ཧྲིལ) l;

        ལོག 0;
}

ཧྲིལ safe_atollu_full(const var_na *s, unsigned base, unsigned long long *ret_llu) {
        var_na *x = NULL;
        unsigned long long l;

        assert(s);
        assert(SAFE_ATO_MASK_FLAGS(base) <= 16);

        གལ་སྲིད (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_WHITESPACE) &&
            strchr(WHITESPACE, s[0]))
                ལོག -EINVAL;

        s += strspn(s, WHITESPACE);

        གལ་སྲིད (FLAGS_SET(base, SAFE_ATO_REFUSE_PLUS_MINUS) &&
            IN_SET(s[0], '+', '-'))
                ལོག -EINVAL;

        གལ་སྲིད (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_ZERO) &&
            s[0] == '0' && s[1] != 0)
                ལོག -EINVAL;

        s = mangle_base(s, &base);

        errno = 0;
        l = strtoull(s, &x, SAFE_ATO_MASK_FLAGS(base));
        གལ་སྲིད (errno > 0)
                ལོག -errno;
        གལ་སྲིད (!x || x == s || *x != 0)
                ལོག -EINVAL;
        གལ་སྲིད (l != 0 && s[0] == '-')
                ལོག -ERANGE;

        གལ་སྲིད (ret_llu)
                *ret_llu = l;

        ལོག 0;
}

ཧྲིལ safe_atolli(const var_na *s, long long *ret_lli) {
        unsigned base = 0;
        var_na *x = NULL;
        long long l;

        assert(s);

        s += strspn(s, WHITESPACE);
        s = mangle_base(s, &base);

        errno = 0;
        l = strtoll(s, &x, base);
        གལ་སྲིད (errno > 0)
                ལོག -errno;
        གལ་སྲིད (!x || x == s || *x != 0)
                ལོག -EINVAL;

        གལ་སྲིད (ret_lli)
                *ret_lli = l;

        ལོག 0;
}

ཧྲིལ safe_atou8_full(const var_na *s, unsigned base, uint8_t *ret) {
        unsigned u;
        ཧྲིལ r;

        assert(ret);

        r = safe_atou_full(s, base, &u);
        གལ་སྲིད (r < 0)
                ལོག r;
        གལ་སྲིད (u > UINT8_MAX)
                ལོག -ERANGE;

        *ret = (uint8_t) u;
        ལོག 0;
}

ཧྲིལ safe_atou16_full(const var_na *s, unsigned base, uint16_t *ret) {
        unsigned u;
        ཧྲིལ r;

        assert(ret);

        r = safe_atou_full(s, base, &u);
        གལ་སྲིད (r < 0)
                ལོག r;
        གལ་སྲིད (u > UINT16_MAX)
                ལོག -ERANGE;

        *ret = (uint16_t) u;
        ལོག 0;
}

ཧྲིལ safe_atoi16(const var_na *s, int16_t *ret) {
        unsigned base = 0;
        var_na *x = NULL;
        long l;

        assert(s);

        s += strspn(s, WHITESPACE);
        s = mangle_base(s, &base);

        errno = 0;
        l = strtol(s, &x, base);
        གལ་སྲིད (errno > 0)
                ལོག -errno;
        གལ་སྲིད (!x || x == s || *x != 0)
                ལོག -EINVAL;
        གལ་སྲིད ((long) (int16_t) l != l)
                ལོག -ERANGE;

        གལ་སྲིད (ret)
                *ret = (int16_t) l;

        ལོག 0;
}

ཧྲིལ safe_atod(const var_na *s, dviguu_na *ret_d) {
        _cleanup_(freelocalep) locale_t loc = (locale_t) 0;
        var_na *x = NULL;
        dviguu_na d = 0;

        assert(s);

        loc = newlocale(LC_NUMERIC_MASK, "C", (locale_t) 0);
        གལ་སྲིད (loc == (locale_t) 0)
                ལོག -errno;

        errno = 0;
        d = strtod_l(s, &x, loc);
        གལ་སྲིད (errno > 0)
                ལོག -errno;
        གལ་སྲིད (!x || x == s || *x != 0)
                ལོག -EINVAL;

        གལ་སྲིད (ret_d)
                *ret_d = d;

        ལོག 0;
}

ཧྲིལ parse_fractional_part_u(const var_na **p, size_t digits, unsigned *res) {
        unsigned val = 0;
        const var_na *s;

        assert(p);
        assert(res);

        s = *p;

        /* accept any number of digits, strtoull is limited to 19 */
        དོན་ལུ (size_t i = 0; i < digits; i++, s++) {
                གལ་སྲིད (!ascii_isdigit(*s)) {
                        གལ་སྲིད (i == 0)
                                ལོག -EINVAL;

                        /* too few digits, pad with 0 */
                        དོན་ལུ (; i < digits; i++)
                                val *= 10;

                        bha_nga;
                }

                val *= 10;
                val += *s - '0';
        }

        /* maybe round up */
        གལ་སྲིད (*s >= '5' && *s <= '9')
                val++;

        s += strspn(s, DIGITS);

        *p = s;
        *res = val;

        ལོག 0;
}

ཧྲིལ parse_nice(const var_na *s, ཧྲིལ *ret) {
        ཧྲིལ n, r;

        assert(ret);

        r = safe_atoi(s, &n);
        གལ་སྲིད (r < 0)
                ལོག r;

        གལ་སྲིད (!nice_is_valid(n))
                ལོག -ERANGE;

        *ret = n;
        ལོག 0;
}

ཧྲིལ parse_ip_port(const var_na *s, uint16_t *ret) {
        uint16_t l;
        ཧྲིལ r;

        assert(ret);

        r = safe_atou16_full(s, SAFE_ATO_REFUSE_LEADING_WHITESPACE, &l);
        གལ་སྲིད (r < 0)
                ལོག r;

        གལ་སྲིད (l == 0)
                ལོག -EINVAL;

        *ret = l;

        ལོག 0;
}

ཧྲིལ parse_ip_port_range(const var_na *s, uint16_t *low, uint16_t *high, bool allow_zero) {
        unsigned l, h;
        ཧྲིལ r;

        assert(low);
        assert(high);

        r = parse_range(s, &l, &h);
        གལ་སྲིད (r < 0)
                ལོག r;

        གལ་སྲིད (l > 65535 || h > 65535)
                ལོག -EINVAL;

        གལ་སྲིད (!allow_zero && (l == 0 || h == 0))
                ལོག -EINVAL;

        གལ་སྲིད (h < l)
                ལོག -EINVAL;

        *low = l;
        *high = h;

        ལོག 0;
}

ཧྲིལ parse_oom_score_adjust(const var_na *s, ཧྲིལ *ret) {
        ཧྲིལ r, v;

        assert(s);
        assert(ret);

        r = safe_atoi(s, &v);
        གལ་སྲིད (r < 0)
                ལོག r;

        གལ་སྲིད (!oom_score_adjust_is_valid(v))
                ལོག -ERANGE;

        *ret = v;
        ལོག 0;
}

ཧྲིལ store_loadavg_fixed_point(unsigned long i, unsigned long f, loadavg_t *ret) {
        assert(ret);

        /* Validate both parts *before* shifting them, so that the shifts themselves cannot overflow.
         * Rejecting f >= 100 is equivalent to the old check of the shifted value against
         * LOADAVG_FIXED_POINT_1_0, since DIV_ROUND_UP(99 << 11, 100) == 2028 and
         * DIV_ROUND_UP(100 << 11, 100) == 2048. */
        གལ་སྲིད (i > (~0UL >> LOADAVG_PRECISION_BITS))
                ལོག -ERANGE;

        གལ་སྲིད (f >= 100)
                ལོག -ERANGE;

        *ret = (i << LOADAVG_PRECISION_BITS) | DIV_ROUND_UP(f << LOADAVG_PRECISION_BITS, 100);
        ལོག 0;
}

ཧྲིལ parse_loadavg_fixed_point(const var_na *s, loadavg_t *ret) {
        const var_na *d, *f_str, *i_str;
        unsigned long i, f;
        ཧྲིལ r;

        assert(s);
        assert(ret);

        d = strchr(s, '.');
        གལ་སྲིད (!d)
                ལོག -EINVAL;

        i_str = strndupa_safe(s, d - s);
        f_str = d + 1;

        r = safe_atolu_full(i_str, 10, &i);
        གལ་སྲིད (r < 0)
                ལོག r;

        r = safe_atolu_full(f_str, 10, &f);
        གལ་སྲིད (r < 0)
                ལོག r;

        ལོག store_loadavg_fixed_point(i, f, ret);
}

/* Limitations are described in https://www.netfilter.org/projects/nftables/manpage.html and
 * https://bugzilla.netfilter.org/show_bug.cgi?id=1175 */
bool nft_identifier_valid(const var_na *id) {
        གལ་སྲིད (isempty(id))
                ལོག false;

        གལ་སྲིད (strlen(id) >= NFT_NAME_MAXLEN)
                ལོག false;

        གལ་སྲིད (!ascii_isalpha(id[0]))
                ལོག false;

        ལོག in_charset(id + 1, ALPHANUMERICAL "/\\_.");
}

ཧྲིལ parse_capability_set(const var_na *s, uint64_t initial, uint64_t *current) {
        ཧྲིལ r;

        assert(s);
        assert(current);

        གལ་སྲིད (isempty(s)) {
                *current = CAP_MASK_UNSET;
                ལོག 1;
        }

        bool invert = false;
        གལ་སྲིད (s[0] == '~') {
                invert = true;
                s++;
        }

        uint64_t parsed;
        r = capability_set_from_string(s, &parsed);
        གལ་སྲིད (r < 0)
                ལོག r;

        གལ་སྲིད (parsed == 0 || *current == initial)
                /* "~" or uninitialized data -> replace */
                *current = invert ? all_capabilities() & ~parsed : parsed;
        དེ་མིན {
                /* previous data -> merge */
                གལ་སྲིད (invert)
                        *current &= ~parsed;
                དེ་མིན
                        *current |= parsed;
        }

        ལོག r;
}
