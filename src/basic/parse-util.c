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

整数 parse_boolean(const char *v) {
        もし (!v)
                戻る -EINVAL;

        もし (STRCASE_IN_SET(v,
                           "1",
                           "yes",
                           "y",
                           "true",
                           "t",
                           "on"))
                戻る 1;

        もし (STRCASE_IN_SET(v,
                           "0",
                           "no",
                           "n",
                           "false",
                           "f",
                           "off"))
                戻る 0;

        戻る -EINVAL;
}

整数 parse_tristate_full(const char *v, const char *third, 整数 *ret) {
        整数 r;

        もし (isempty(v) || streq_ptr(v, third)) { /* Empty string is always taken as the third/invalid/auto state */
                もし (ret)
                        *ret = -1;
        } 否则 {
                r = parse_boolean(v);
                もし (r < 0)
                        戻る r;

                もし (ret)
                        *ret = r;
        }

        戻る 0;
}

整数 parse_pid(const char *s, pid_t *ret) {
        unsigned long ul = 0;
        pid_t pid;
        整数 r;

        assert(s);

        r = safe_atolu(s, &ul);
        もし (r < 0)
                戻る r;

        pid = (pid_t) ul;

        もし ((unsigned long) pid != ul)
                戻る -ERANGE;

        もし (!pid_is_valid(pid))
                戻る -ERANGE;

        もし (ret)
                *ret = pid;
        戻る 0;
}

整数 parse_mode(const char *s, mode_t *ret) {
        unsigned m;
        整数 r;

        assert(s);

        r = safe_atou_full(s, 8 |
                           SAFE_ATO_REFUSE_PLUS_MINUS, /* Leading '+' or even '-' char? that's just weird,
                                                        * refuse. User might have wanted to add mode flags or
                                                        * so, but this parser doesn't allow that, so let's
                                                        * better be safe. */
                           &m);
        もし (r < 0)
                戻る r;
        もし (m > 07777)
                戻る -ERANGE;

        もし (ret)
                *ret = m;
        戻る 0;
}

整数 parse_ifindex(const char *s) {
        整数 ifi, r;

        assert(s);

        r = safe_atoi(s, &ifi);
        もし (r < 0)
                戻る r;
        もし (ifi <= 0)
                戻る -EINVAL;

        戻る ifi;
}

整数 parse_mtu(整数 family, const char *s, uint32_t *ret) {
        uint64_t u, m;
        整数 r;

        assert(ret);

        r = parse_size(s, 1024, &u);
        もし (r < 0)
                戻る r;

        もし (u > UINT32_MAX)
                戻る -ERANGE;

        switch (family) {
        case AF_INET:
                m = IPV4_MIN_MTU; /* This is 68 */
                break;
        case AF_INET6:
                m = IPV6_MIN_MTU; /* This is 1280 */
                break;
        default:
                m = 0;
        }

        もし (u < m)
                戻る -ERANGE;

        *ret = (uint32_t) u;
        戻る 0;
}

整数 parse_size(const char *t, uint64_t base, uint64_t *size) {

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

        struct table {
                const char *suffix;
                unsigned long long factor;
        };

        static const struct table iec[] = {
                { "E", 1024ULL*1024ULL*1024ULL*1024ULL*1024ULL*1024ULL },
                { "P", 1024ULL*1024ULL*1024ULL*1024ULL*1024ULL },
                { "T", 1024ULL*1024ULL*1024ULL*1024ULL },
                { "G", 1024ULL*1024ULL*1024ULL },
                { "M", 1024ULL*1024ULL },
                { "K", 1024ULL },
                { "B", 1ULL },
                { "",  1ULL },
        };

        static const struct table si[] = {
                { "E", 1000ULL*1000ULL*1000ULL*1000ULL*1000ULL*1000ULL },
                { "P", 1000ULL*1000ULL*1000ULL*1000ULL*1000ULL },
                { "T", 1000ULL*1000ULL*1000ULL*1000ULL },
                { "G", 1000ULL*1000ULL*1000ULL },
                { "M", 1000ULL*1000ULL },
                { "K", 1000ULL },
                { "B", 1ULL },
                { "",  1ULL },
        };

        const struct table *table;
        const char *p;
        unsigned long long r = 0;
        unsigned n_entries, start_pos = 0;

        assert(t);
        assert(IN_SET(base, 1000, 1024));
        assert(size);

        もし (base == 1000) {
                table = si;
                n_entries = ELEMENTSOF(si);
        } 否则 {
                table = iec;
                n_entries = ELEMENTSOF(iec);
        }

        p = t;
        do {
                unsigned long long l, tmp;
                double frac = 0;
                char *e;
                unsigned i;

                p += strspn(p, WHITESPACE);

                errno = 0;
                l = strtoull(p, &e, 10);
                もし (errno > 0)
                        戻る -errno;
                もし (e == p)
                        戻る -EINVAL;
                もし (*p == '-')
                        戻る -ERANGE;

                もし (*e == '.') {
                        e++;

                        /* strtoull() itself would accept space/+/- */
                        もし (ascii_isdigit(*e)) {
                                unsigned long long l2;
                                char *e2;

                                l2 = strtoull(e, &e2, 10);
                                もし (errno > 0)
                                        戻る -errno;

                                /* Ignore failure. E.g. 10.M is valid */
                                frac = l2;
                                ために (; e < e2; e++)
                                        frac /= 10;
                        }
                }

                e += strspn(e, WHITESPACE);

                ために (i = start_pos; i < n_entries; i++)
                        もし (startswith(e, table[i].suffix))
                                break;

                もし (i >= n_entries)
                        戻る -EINVAL;

                もし (l + (frac > 0) > ULLONG_MAX / table[i].factor)
                        戻る -ERANGE;

                tmp = l * table[i].factor + (unsigned long long) (frac * table[i].factor);
                もし (tmp > ULLONG_MAX - r)
                        戻る -ERANGE;

                r += tmp;
                もし ((unsigned long long) (uint64_t) r != r)
                        戻る -ERANGE;

                p = e + strlen(table[i].suffix);

                start_pos = i + 1;

        } ながら (*p);

        *size = r;

        戻る 0;
}

整数 parse_sector_size(const char *t, uint64_t *ret) {
        整数 r;

        assert(t);
        assert(ret);

        uint64_t ss;

        r = safe_atou64(t, &ss);
        もし (r < 0)
                戻る log_error_errno(r, "Failed to parse sector size parameter %s", t);
        もし (ss < 512 || ss > 4096) /* Allow up to 4K due to dm-crypt support and 4K alignment by the homed LUKS backend */
                戻る log_error_errno(SYNTHETIC_ERRNO(ERANGE), "Sector size not between 512 and 4096: %s", t);
        もし (!ISPOWEROF2(ss))
                戻る log_error_errno(SYNTHETIC_ERRNO(EINVAL), "Sector size not power of 2: %s", t);

        *ret = ss;
        戻る 0;
}

整数 parse_range(const char *t, unsigned *lower, unsigned *upper) {
        _cleanup_free_ char *word = NULL;
        unsigned l, u;
        整数 r;

        assert(lower);
        assert(upper);

        /* Extract the lower bound. */
        r = extract_first_word(&t, &word, "-", EXTRACT_DONT_COALESCE_SEPARATORS);
        もし (r < 0)
                戻る r;
        もし (r == 0)
                戻る -EINVAL;

        r = safe_atou(word, &l);
        もし (r < 0)
                戻る r;

        /* Check for the upper bound and extract it if needed */
        もし (!t)
                /* Single number with no dashes. */
                u = l;
        否则 もし (!*t)
                /* Trailing dash is an error. */
                戻る -EINVAL;
        否则 {
                r = safe_atou(t, &u);
                もし (r < 0)
                        戻る r;
        }

        *lower = l;
        *upper = u;
        戻る 0;
}

整数 parse_errno(const char *t) {
        整数 r, e;

        assert(t);

        r = errno_from_name(t);
        もし (r > 0)
                戻る r;

        r = safe_atoi(t, &e);
        もし (r < 0)
                戻る r;

        /* 0 is also allowed here */
        もし (!errno_is_valid(e) && e != 0)
                戻る -ERANGE;

        戻る e;
}

整数 parse_fd(const char *t) {
        整数 r, fd;

        assert(t);

        r = safe_atoi(t, &fd);
        もし (r < 0)
                戻る r;

        もし (fd < 0)
                戻る -EBADF;

        戻る fd;
}

整数 parse_user_shell(const char *s, char **ret_sh, bool *ret_copy) {
        char *sh;
        整数 r;

        assert(ret_sh);
        assert(ret_copy);

        もし (path_is_absolute(s) && path_is_normalized(s)) {
                sh = strdup(s);
                もし (!sh)
                        戻る -ENOMEM;

                *ret_sh = sh;
                *ret_copy = false;
        } 否则 {
                r = parse_boolean(s);
                もし (r < 0)
                        戻る r;

                *ret_sh = NULL;
                *ret_copy = r;
        }

        戻る 0;
}

static const char *mangle_base(const char *s, unsigned *base) {
        const char *k;

        assert(s);
        assert(base);

        /* Base already explicitly specified, then don't do anything. */
        もし (SAFE_ATO_MASK_FLAGS(*base) != 0)
                戻る s;

        /* Support Python 3 style "0b" and 0x" prefixes, because they truly make sense, much more than C's "0" prefix for octal. */
        k = STARTSWITH_SET(s, "0b", "0B");
        もし (k) {
                *base = 2 | (*base & SAFE_ATO_ALL_FLAGS);
                戻る k;
        }

        k = STARTSWITH_SET(s, "0o", "0O");
        もし (k) {
                *base = 8 | (*base & SAFE_ATO_ALL_FLAGS);
                戻る k;
        }

        戻る s;
}

整数 safe_atou_full(const char *s, unsigned base, unsigned *ret_u) {
        char *x = NULL;
        unsigned long l;

        assert(s);
        assert(SAFE_ATO_MASK_FLAGS(base) <= 16);

        /* strtoul() is happy to parse negative values, and silently converts them to unsigned values without
         * generating an error. We want a clean error, hence let's look for the "-" prefix on our own, and
         * generate an error. But let's do so only after strtoul() validated that the string is clean
         * otherwise, so that we return EINVAL preferably over ERANGE. */

        もし (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_WHITESPACE) &&
            strchr(WHITESPACE, s[0]))
                戻る -EINVAL;

        s += strspn(s, WHITESPACE);

        もし (FLAGS_SET(base, SAFE_ATO_REFUSE_PLUS_MINUS) &&
            IN_SET(s[0], '+', '-'))
                戻る -EINVAL; /* Note that we check the "-" prefix again a second time below, but return a
                                 * different error. I.e. if the SAFE_ATO_REFUSE_PLUS_MINUS flag is set we
                                 * blanket refuse +/- prefixed integers, while if it is missing we'll just
                                 * return ERANGE, because the string actually parses correctly, but doesn't
                                 * fit in the return type. */

        もし (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_ZERO) &&
            s[0] == '0' && !streq(s, "0"))
                戻る -EINVAL; /* This is particularly useful to avoid ambiguities between C's octal
                                 * notation and assumed-to-be-decimal integers with a leading zero. */

        s = mangle_base(s, &base);

        errno = 0;
        l = strtoul(s, &x, SAFE_ATO_MASK_FLAGS(base) /* Let's mask off the flags bits so that only the actual
                                                      * base is left */);
        もし (errno > 0)
                戻る -errno;
        もし (!x || x == s || *x != 0)
                戻る -EINVAL;
        もし (l != 0 && s[0] == '-')
                戻る -ERANGE;
        もし ((unsigned long) (unsigned) l != l)
                戻る -ERANGE;

        もし (ret_u)
                *ret_u = (unsigned) l;

        戻る 0;
}

整数 safe_atou_bounded(const char *s, unsigned min, unsigned max, unsigned *ret) {
        unsigned v;
        整数 r;

        assert(ret);

        r = safe_atou(s, &v);
        もし (r < 0)
                戻る r;

        もし (v < min || v > max)
                戻る -ERANGE;

        *ret = v;
        戻る 0;
}

整数 safe_atoi(const char *s, 整数 *ret_i) {
        unsigned base = 0;
        char *x = NULL;
        long l;

        assert(s);

        s += strspn(s, WHITESPACE);
        s = mangle_base(s, &base);

        errno = 0;
        l = strtol(s, &x, base);
        もし (errno > 0)
                戻る -errno;
        もし (!x || x == s || *x != 0)
                戻る -EINVAL;
        もし ((long) (整数) l != l)
                戻る -ERANGE;

        もし (ret_i)
                *ret_i = (整数) l;

        戻る 0;
}

整数 safe_atollu_full(const char *s, unsigned base, unsigned long long *ret_llu) {
        char *x = NULL;
        unsigned long long l;

        assert(s);
        assert(SAFE_ATO_MASK_FLAGS(base) <= 16);

        もし (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_WHITESPACE) &&
            strchr(WHITESPACE, s[0]))
                戻る -EINVAL;

        s += strspn(s, WHITESPACE);

        もし (FLAGS_SET(base, SAFE_ATO_REFUSE_PLUS_MINUS) &&
            IN_SET(s[0], '+', '-'))
                戻る -EINVAL;

        もし (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_ZERO) &&
            s[0] == '0' && s[1] != 0)
                戻る -EINVAL;

        s = mangle_base(s, &base);

        errno = 0;
        l = strtoull(s, &x, SAFE_ATO_MASK_FLAGS(base));
        もし (errno > 0)
                戻る -errno;
        もし (!x || x == s || *x != 0)
                戻る -EINVAL;
        もし (l != 0 && s[0] == '-')
                戻る -ERANGE;

        もし (ret_llu)
                *ret_llu = l;

        戻る 0;
}

整数 safe_atolli(const char *s, long long *ret_lli) {
        unsigned base = 0;
        char *x = NULL;
        long long l;

        assert(s);

        s += strspn(s, WHITESPACE);
        s = mangle_base(s, &base);

        errno = 0;
        l = strtoll(s, &x, base);
        もし (errno > 0)
                戻る -errno;
        もし (!x || x == s || *x != 0)
                戻る -EINVAL;

        もし (ret_lli)
                *ret_lli = l;

        戻る 0;
}

整数 safe_atou8_full(const char *s, unsigned base, uint8_t *ret) {
        unsigned u;
        整数 r;

        assert(ret);

        r = safe_atou_full(s, base, &u);
        もし (r < 0)
                戻る r;
        もし (u > UINT8_MAX)
                戻る -ERANGE;

        *ret = (uint8_t) u;
        戻る 0;
}

整数 safe_atou16_full(const char *s, unsigned base, uint16_t *ret) {
        unsigned u;
        整数 r;

        assert(ret);

        r = safe_atou_full(s, base, &u);
        もし (r < 0)
                戻る r;
        もし (u > UINT16_MAX)
                戻る -ERANGE;

        *ret = (uint16_t) u;
        戻る 0;
}

整数 safe_atoi16(const char *s, int16_t *ret) {
        unsigned base = 0;
        char *x = NULL;
        long l;

        assert(s);

        s += strspn(s, WHITESPACE);
        s = mangle_base(s, &base);

        errno = 0;
        l = strtol(s, &x, base);
        もし (errno > 0)
                戻る -errno;
        もし (!x || x == s || *x != 0)
                戻る -EINVAL;
        もし ((long) (int16_t) l != l)
                戻る -ERANGE;

        もし (ret)
                *ret = (int16_t) l;

        戻る 0;
}

整数 safe_atod(const char *s, double *ret_d) {
        _cleanup_(freelocalep) locale_t loc = (locale_t) 0;
        char *x = NULL;
        double d = 0;

        assert(s);

        loc = newlocale(LC_NUMERIC_MASK, "C", (locale_t) 0);
        もし (loc == (locale_t) 0)
                戻る -errno;

        errno = 0;
        d = strtod_l(s, &x, loc);
        もし (errno > 0)
                戻る -errno;
        もし (!x || x == s || *x != 0)
                戻る -EINVAL;

        もし (ret_d)
                *ret_d = d;

        戻る 0;
}

整数 parse_fractional_part_u(const char **p, size_t digits, unsigned *res) {
        unsigned val = 0;
        const char *s;

        assert(p);
        assert(res);

        s = *p;

        /* accept any number of digits, strtoull is limited to 19 */
        ために (size_t i = 0; i < digits; i++, s++) {
                もし (!ascii_isdigit(*s)) {
                        もし (i == 0)
                                戻る -EINVAL;

                        /* too few digits, pad with 0 */
                        ために (; i < digits; i++)
                                val *= 10;

                        break;
                }

                val *= 10;
                val += *s - '0';
        }

        /* maybe round up */
        もし (*s >= '5' && *s <= '9')
                val++;

        s += strspn(s, DIGITS);

        *p = s;
        *res = val;

        戻る 0;
}

整数 parse_nice(const char *s, 整数 *ret) {
        整数 n, r;

        assert(ret);

        r = safe_atoi(s, &n);
        もし (r < 0)
                戻る r;

        もし (!nice_is_valid(n))
                戻る -ERANGE;

        *ret = n;
        戻る 0;
}

整数 parse_ip_port(const char *s, uint16_t *ret) {
        uint16_t l;
        整数 r;

        assert(ret);

        r = safe_atou16_full(s, SAFE_ATO_REFUSE_LEADING_WHITESPACE, &l);
        もし (r < 0)
                戻る r;

        もし (l == 0)
                戻る -EINVAL;

        *ret = l;

        戻る 0;
}

整数 parse_ip_port_range(const char *s, uint16_t *low, uint16_t *high, bool allow_zero) {
        unsigned l, h;
        整数 r;

        assert(low);
        assert(high);

        r = parse_range(s, &l, &h);
        もし (r < 0)
                戻る r;

        もし (l > 65535 || h > 65535)
                戻る -EINVAL;

        もし (!allow_zero && (l == 0 || h == 0))
                戻る -EINVAL;

        もし (h < l)
                戻る -EINVAL;

        *low = l;
        *high = h;

        戻る 0;
}

整数 parse_oom_score_adjust(const char *s, 整数 *ret) {
        整数 r, v;

        assert(s);
        assert(ret);

        r = safe_atoi(s, &v);
        もし (r < 0)
                戻る r;

        もし (!oom_score_adjust_is_valid(v))
                戻る -ERANGE;

        *ret = v;
        戻る 0;
}

整数 store_loadavg_fixed_point(unsigned long i, unsigned long f, loadavg_t *ret) {
        assert(ret);

        /* Validate both parts *before* shifting them, so that the shifts themselves cannot overflow.
         * Rejecting f >= 100 is equivalent to the old check of the shifted value against
         * LOADAVG_FIXED_POINT_1_0, since DIV_ROUND_UP(99 << 11, 100) == 2028 and
         * DIV_ROUND_UP(100 << 11, 100) == 2048. */
        もし (i > (~0UL >> LOADAVG_PRECISION_BITS))
                戻る -ERANGE;

        もし (f >= 100)
                戻る -ERANGE;

        *ret = (i << LOADAVG_PRECISION_BITS) | DIV_ROUND_UP(f << LOADAVG_PRECISION_BITS, 100);
        戻る 0;
}

整数 parse_loadavg_fixed_point(const char *s, loadavg_t *ret) {
        const char *d, *f_str, *i_str;
        unsigned long i, f;
        整数 r;

        assert(s);
        assert(ret);

        d = strchr(s, '.');
        もし (!d)
                戻る -EINVAL;

        i_str = strndupa_safe(s, d - s);
        f_str = d + 1;

        r = safe_atolu_full(i_str, 10, &i);
        もし (r < 0)
                戻る r;

        r = safe_atolu_full(f_str, 10, &f);
        もし (r < 0)
                戻る r;

        戻る store_loadavg_fixed_point(i, f, ret);
}

/* Limitations are described in https://www.netfilter.org/projects/nftables/manpage.html and
 * https://bugzilla.netfilter.org/show_bug.cgi?id=1175 */
bool nft_identifier_valid(const char *id) {
        もし (isempty(id))
                戻る false;

        もし (strlen(id) >= NFT_NAME_MAXLEN)
                戻る false;

        もし (!ascii_isalpha(id[0]))
                戻る false;

        戻る in_charset(id + 1, ALPHANUMERICAL "/\\_.");
}

整数 parse_capability_set(const char *s, uint64_t initial, uint64_t *current) {
        整数 r;

        assert(s);
        assert(current);

        もし (isempty(s)) {
                *current = CAP_MASK_UNSET;
                戻る 1;
        }

        bool invert = false;
        もし (s[0] == '~') {
                invert = true;
                s++;
        }

        uint64_t parsed;
        r = capability_set_from_string(s, &parsed);
        もし (r < 0)
                戻る r;

        もし (parsed == 0 || *current == initial)
                /* "~" or uninitialized data -> replace */
                *current = invert ? all_capabilities() & ~parsed : parsed;
        否则 {
                /* previous data -> merge */
                もし (invert)
                        *current &= ~parsed;
                否则
                        *current |= parsed;
        }

        戻る r;
}
