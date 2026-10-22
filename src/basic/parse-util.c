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

ціле parse_boolean(const char *v) {
        якщо (!v)
                повернути -EINVAL;

        якщо (STRCASE_IN_SET(v,
                           "1",
                           "yes",
                           "y",
                           "true",
                           "t",
                           "on"))
                повернути 1;

        якщо (STRCASE_IN_SET(v,
                           "0",
                           "no",
                           "n",
                           "false",
                           "f",
                           "off"))
                повернути 0;

        повернути -EINVAL;
}

ціле parse_tristate_full(const char *v, const char *third, ціле *ret) {
        ціле r;

        якщо (isempty(v) || streq_ptr(v, third)) { /* Empty string is always taken as the third/invalid/auto state */
                якщо (ret)
                        *ret = -1;
        } інакше {
                r = parse_boolean(v);
                якщо (r < 0)
                        повернути r;

                якщо (ret)
                        *ret = r;
        }

        повернути 0;
}

ціле parse_pid(const char *s, pid_t *ret) {
        unsigned long ul = 0;
        pid_t pid;
        ціле r;

        assert(s);

        r = safe_atolu(s, &ul);
        якщо (r < 0)
                повернути r;

        pid = (pid_t) ul;

        якщо ((unsigned long) pid != ul)
                повернути -ERANGE;

        якщо (!pid_is_valid(pid))
                повернути -ERANGE;

        якщо (ret)
                *ret = pid;
        повернути 0;
}

ціле parse_mode(const char *s, mode_t *ret) {
        unsigned m;
        ціле r;

        assert(s);

        r = safe_atou_full(s, 8 |
                           SAFE_ATO_REFUSE_PLUS_MINUS, /* Leading '+' or even '-' char? that's just weird,
                                                        * refuse. User might have wanted to add mode flags or
                                                        * so, but this parser doesn't allow that, so let's
                                                        * better be safe. */
                           &m);
        якщо (r < 0)
                повернути r;
        якщо (m > 07777)
                повернути -ERANGE;

        якщо (ret)
                *ret = m;
        повернути 0;
}

ціле parse_ifindex(const char *s) {
        ціле ifi, r;

        assert(s);

        r = safe_atoi(s, &ifi);
        якщо (r < 0)
                повернути r;
        якщо (ifi <= 0)
                повернути -EINVAL;

        повернути ifi;
}

ціле parse_mtu(ціле family, const char *s, uint32_t *ret) {
        uint64_t u, m;
        ціле r;

        assert(ret);

        r = parse_size(s, 1024, &u);
        якщо (r < 0)
                повернути r;

        якщо (u > UINT32_MAX)
                повернути -ERANGE;

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

        якщо (u < m)
                повернути -ERANGE;

        *ret = (uint32_t) u;
        повернути 0;
}

ціле parse_size(const char *t, uint64_t base, uint64_t *size) {

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

        якщо (base == 1000) {
                table = si;
                n_entries = ELEMENTSOF(si);
        } інакше {
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
                якщо (errno > 0)
                        повернути -errno;
                якщо (e == p)
                        повернути -EINVAL;
                якщо (*p == '-')
                        повернути -ERANGE;

                якщо (*e == '.') {
                        e++;

                        /* strtoull() itself would accept space/+/- */
                        якщо (ascii_isdigit(*e)) {
                                unsigned long long l2;
                                char *e2;

                                l2 = strtoull(e, &e2, 10);
                                якщо (errno > 0)
                                        повернути -errno;

                                /* Ignore failure. E.g. 10.M is valid */
                                frac = l2;
                                для (; e < e2; e++)
                                        frac /= 10;
                        }
                }

                e += strspn(e, WHITESPACE);

                для (i = start_pos; i < n_entries; i++)
                        якщо (startswith(e, table[i].suffix))
                                break;

                якщо (i >= n_entries)
                        повернути -EINVAL;

                якщо (l + (frac > 0) > ULLONG_MAX / table[i].factor)
                        повернути -ERANGE;

                tmp = l * table[i].factor + (unsigned long long) (frac * table[i].factor);
                якщо (tmp > ULLONG_MAX - r)
                        повернути -ERANGE;

                r += tmp;
                якщо ((unsigned long long) (uint64_t) r != r)
                        повернути -ERANGE;

                p = e + strlen(table[i].suffix);

                start_pos = i + 1;

        } поки (*p);

        *size = r;

        повернути 0;
}

ціле parse_sector_size(const char *t, uint64_t *ret) {
        ціле r;

        assert(t);
        assert(ret);

        uint64_t ss;

        r = safe_atou64(t, &ss);
        якщо (r < 0)
                повернути log_error_errno(r, "Failed to parse sector size parameter %s", t);
        якщо (ss < 512 || ss > 4096) /* Allow up to 4K due to dm-crypt support and 4K alignment by the homed LUKS backend */
                повернути log_error_errno(SYNTHETIC_ERRNO(ERANGE), "Sector size not between 512 and 4096: %s", t);
        якщо (!ISPOWEROF2(ss))
                повернути log_error_errno(SYNTHETIC_ERRNO(EINVAL), "Sector size not power of 2: %s", t);

        *ret = ss;
        повернути 0;
}

ціле parse_range(const char *t, unsigned *lower, unsigned *upper) {
        _cleanup_free_ char *word = NULL;
        unsigned l, u;
        ціле r;

        assert(lower);
        assert(upper);

        /* Extract the lower bound. */
        r = extract_first_word(&t, &word, "-", EXTRACT_DONT_COALESCE_SEPARATORS);
        якщо (r < 0)
                повернути r;
        якщо (r == 0)
                повернути -EINVAL;

        r = safe_atou(word, &l);
        якщо (r < 0)
                повернути r;

        /* Check for the upper bound and extract it if needed */
        якщо (!t)
                /* Single number with no dashes. */
                u = l;
        інакше якщо (!*t)
                /* Trailing dash is an error. */
                повернути -EINVAL;
        інакше {
                r = safe_atou(t, &u);
                якщо (r < 0)
                        повернути r;
        }

        *lower = l;
        *upper = u;
        повернути 0;
}

ціле parse_errno(const char *t) {
        ціле r, e;

        assert(t);

        r = errno_from_name(t);
        якщо (r > 0)
                повернути r;

        r = safe_atoi(t, &e);
        якщо (r < 0)
                повернути r;

        /* 0 is also allowed here */
        якщо (!errno_is_valid(e) && e != 0)
                повернути -ERANGE;

        повернути e;
}

ціле parse_fd(const char *t) {
        ціле r, fd;

        assert(t);

        r = safe_atoi(t, &fd);
        якщо (r < 0)
                повернути r;

        якщо (fd < 0)
                повернути -EBADF;

        повернути fd;
}

ціле parse_user_shell(const char *s, char **ret_sh, bool *ret_copy) {
        char *sh;
        ціле r;

        assert(ret_sh);
        assert(ret_copy);

        якщо (path_is_absolute(s) && path_is_normalized(s)) {
                sh = strdup(s);
                якщо (!sh)
                        повернути -ENOMEM;

                *ret_sh = sh;
                *ret_copy = false;
        } інакше {
                r = parse_boolean(s);
                якщо (r < 0)
                        повернути r;

                *ret_sh = NULL;
                *ret_copy = r;
        }

        повернути 0;
}

static const char *mangle_base(const char *s, unsigned *base) {
        const char *k;

        assert(s);
        assert(base);

        /* Base already explicitly specified, then don't do anything. */
        якщо (SAFE_ATO_MASK_FLAGS(*base) != 0)
                повернути s;

        /* Support Python 3 style "0b" and 0x" prefixes, because they truly make sense, much more than C's "0" prefix for octal. */
        k = STARTSWITH_SET(s, "0b", "0B");
        якщо (k) {
                *base = 2 | (*base & SAFE_ATO_ALL_FLAGS);
                повернути k;
        }

        k = STARTSWITH_SET(s, "0o", "0O");
        якщо (k) {
                *base = 8 | (*base & SAFE_ATO_ALL_FLAGS);
                повернути k;
        }

        повернути s;
}

ціле safe_atou_full(const char *s, unsigned base, unsigned *ret_u) {
        char *x = NULL;
        unsigned long l;

        assert(s);
        assert(SAFE_ATO_MASK_FLAGS(base) <= 16);

        /* strtoul() is happy to parse negative values, and silently converts them to unsigned values without
         * generating an error. We want a clean error, hence let's look for the "-" prefix on our own, and
         * generate an error. But let's do so only after strtoul() validated that the string is clean
         * otherwise, so that we return EINVAL preferably over ERANGE. */

        якщо (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_WHITESPACE) &&
            strchr(WHITESPACE, s[0]))
                повернути -EINVAL;

        s += strspn(s, WHITESPACE);

        якщо (FLAGS_SET(base, SAFE_ATO_REFUSE_PLUS_MINUS) &&
            IN_SET(s[0], '+', '-'))
                повернути -EINVAL; /* Note that we check the "-" prefix again a second time below, but return a
                                 * different error. I.e. if the SAFE_ATO_REFUSE_PLUS_MINUS flag is set we
                                 * blanket refuse +/- prefixed integers, while if it is missing we'll just
                                 * return ERANGE, because the string actually parses correctly, but doesn't
                                 * fit in the return type. */

        якщо (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_ZERO) &&
            s[0] == '0' && !streq(s, "0"))
                повернути -EINVAL; /* This is particularly useful to avoid ambiguities between C's octal
                                 * notation and assumed-to-be-decimal integers with a leading zero. */

        s = mangle_base(s, &base);

        errno = 0;
        l = strtoul(s, &x, SAFE_ATO_MASK_FLAGS(base) /* Let's mask off the flags bits so that only the actual
                                                      * base is left */);
        якщо (errno > 0)
                повернути -errno;
        якщо (!x || x == s || *x != 0)
                повернути -EINVAL;
        якщо (l != 0 && s[0] == '-')
                повернути -ERANGE;
        якщо ((unsigned long) (unsigned) l != l)
                повернути -ERANGE;

        якщо (ret_u)
                *ret_u = (unsigned) l;

        повернути 0;
}

ціле safe_atou_bounded(const char *s, unsigned min, unsigned max, unsigned *ret) {
        unsigned v;
        ціле r;

        assert(ret);

        r = safe_atou(s, &v);
        якщо (r < 0)
                повернути r;

        якщо (v < min || v > max)
                повернути -ERANGE;

        *ret = v;
        повернути 0;
}

ціле safe_atoi(const char *s, ціле *ret_i) {
        unsigned base = 0;
        char *x = NULL;
        long l;

        assert(s);

        s += strspn(s, WHITESPACE);
        s = mangle_base(s, &base);

        errno = 0;
        l = strtol(s, &x, base);
        якщо (errno > 0)
                повернути -errno;
        якщо (!x || x == s || *x != 0)
                повернути -EINVAL;
        якщо ((long) (ціле) l != l)
                повернути -ERANGE;

        якщо (ret_i)
                *ret_i = (ціле) l;

        повернути 0;
}

ціле safe_atollu_full(const char *s, unsigned base, unsigned long long *ret_llu) {
        char *x = NULL;
        unsigned long long l;

        assert(s);
        assert(SAFE_ATO_MASK_FLAGS(base) <= 16);

        якщо (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_WHITESPACE) &&
            strchr(WHITESPACE, s[0]))
                повернути -EINVAL;

        s += strspn(s, WHITESPACE);

        якщо (FLAGS_SET(base, SAFE_ATO_REFUSE_PLUS_MINUS) &&
            IN_SET(s[0], '+', '-'))
                повернути -EINVAL;

        якщо (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_ZERO) &&
            s[0] == '0' && s[1] != 0)
                повернути -EINVAL;

        s = mangle_base(s, &base);

        errno = 0;
        l = strtoull(s, &x, SAFE_ATO_MASK_FLAGS(base));
        якщо (errno > 0)
                повернути -errno;
        якщо (!x || x == s || *x != 0)
                повернути -EINVAL;
        якщо (l != 0 && s[0] == '-')
                повернути -ERANGE;

        якщо (ret_llu)
                *ret_llu = l;

        повернути 0;
}

ціле safe_atolli(const char *s, long long *ret_lli) {
        unsigned base = 0;
        char *x = NULL;
        long long l;

        assert(s);

        s += strspn(s, WHITESPACE);
        s = mangle_base(s, &base);

        errno = 0;
        l = strtoll(s, &x, base);
        якщо (errno > 0)
                повернути -errno;
        якщо (!x || x == s || *x != 0)
                повернути -EINVAL;

        якщо (ret_lli)
                *ret_lli = l;

        повернути 0;
}

ціле safe_atou8_full(const char *s, unsigned base, uint8_t *ret) {
        unsigned u;
        ціле r;

        assert(ret);

        r = safe_atou_full(s, base, &u);
        якщо (r < 0)
                повернути r;
        якщо (u > UINT8_MAX)
                повернути -ERANGE;

        *ret = (uint8_t) u;
        повернути 0;
}

ціле safe_atou16_full(const char *s, unsigned base, uint16_t *ret) {
        unsigned u;
        ціле r;

        assert(ret);

        r = safe_atou_full(s, base, &u);
        якщо (r < 0)
                повернути r;
        якщо (u > UINT16_MAX)
                повернути -ERANGE;

        *ret = (uint16_t) u;
        повернути 0;
}

ціле safe_atoi16(const char *s, int16_t *ret) {
        unsigned base = 0;
        char *x = NULL;
        long l;

        assert(s);

        s += strspn(s, WHITESPACE);
        s = mangle_base(s, &base);

        errno = 0;
        l = strtol(s, &x, base);
        якщо (errno > 0)
                повернути -errno;
        якщо (!x || x == s || *x != 0)
                повернути -EINVAL;
        якщо ((long) (int16_t) l != l)
                повернути -ERANGE;

        якщо (ret)
                *ret = (int16_t) l;

        повернути 0;
}

ціле safe_atod(const char *s, double *ret_d) {
        _cleanup_(freelocalep) locale_t loc = (locale_t) 0;
        char *x = NULL;
        double d = 0;

        assert(s);

        loc = newlocale(LC_NUMERIC_MASK, "C", (locale_t) 0);
        якщо (loc == (locale_t) 0)
                повернути -errno;

        errno = 0;
        d = strtod_l(s, &x, loc);
        якщо (errno > 0)
                повернути -errno;
        якщо (!x || x == s || *x != 0)
                повернути -EINVAL;

        якщо (ret_d)
                *ret_d = d;

        повернути 0;
}

ціле parse_fractional_part_u(const char **p, size_t digits, unsigned *res) {
        unsigned val = 0;
        const char *s;

        assert(p);
        assert(res);

        s = *p;

        /* accept any number of digits, strtoull is limited to 19 */
        для (size_t i = 0; i < digits; i++, s++) {
                якщо (!ascii_isdigit(*s)) {
                        якщо (i == 0)
                                повернути -EINVAL;

                        /* too few digits, pad with 0 */
                        для (; i < digits; i++)
                                val *= 10;

                        break;
                }

                val *= 10;
                val += *s - '0';
        }

        /* maybe round up */
        якщо (*s >= '5' && *s <= '9')
                val++;

        s += strspn(s, DIGITS);

        *p = s;
        *res = val;

        повернути 0;
}

ціле parse_nice(const char *s, ціле *ret) {
        ціле n, r;

        assert(ret);

        r = safe_atoi(s, &n);
        якщо (r < 0)
                повернути r;

        якщо (!nice_is_valid(n))
                повернути -ERANGE;

        *ret = n;
        повернути 0;
}

ціле parse_ip_port(const char *s, uint16_t *ret) {
        uint16_t l;
        ціле r;

        assert(ret);

        r = safe_atou16_full(s, SAFE_ATO_REFUSE_LEADING_WHITESPACE, &l);
        якщо (r < 0)
                повернути r;

        якщо (l == 0)
                повернути -EINVAL;

        *ret = l;

        повернути 0;
}

ціле parse_ip_port_range(const char *s, uint16_t *low, uint16_t *high, bool allow_zero) {
        unsigned l, h;
        ціле r;

        assert(low);
        assert(high);

        r = parse_range(s, &l, &h);
        якщо (r < 0)
                повернути r;

        якщо (l > 65535 || h > 65535)
                повернути -EINVAL;

        якщо (!allow_zero && (l == 0 || h == 0))
                повернути -EINVAL;

        якщо (h < l)
                повернути -EINVAL;

        *low = l;
        *high = h;

        повернути 0;
}

ціле parse_oom_score_adjust(const char *s, ціле *ret) {
        ціле r, v;

        assert(s);
        assert(ret);

        r = safe_atoi(s, &v);
        якщо (r < 0)
                повернути r;

        якщо (!oom_score_adjust_is_valid(v))
                повернути -ERANGE;

        *ret = v;
        повернути 0;
}

ціле store_loadavg_fixed_point(unsigned long i, unsigned long f, loadavg_t *ret) {
        assert(ret);

        /* Validate both parts *before* shifting them, so that the shifts themselves cannot overflow.
         * Rejecting f >= 100 is equivalent to the old check of the shifted value against
         * LOADAVG_FIXED_POINT_1_0, since DIV_ROUND_UP(99 << 11, 100) == 2028 and
         * DIV_ROUND_UP(100 << 11, 100) == 2048. */
        якщо (i > (~0UL >> LOADAVG_PRECISION_BITS))
                повернути -ERANGE;

        якщо (f >= 100)
                повернути -ERANGE;

        *ret = (i << LOADAVG_PRECISION_BITS) | DIV_ROUND_UP(f << LOADAVG_PRECISION_BITS, 100);
        повернути 0;
}

ціле parse_loadavg_fixed_point(const char *s, loadavg_t *ret) {
        const char *d, *f_str, *i_str;
        unsigned long i, f;
        ціле r;

        assert(s);
        assert(ret);

        d = strchr(s, '.');
        якщо (!d)
                повернути -EINVAL;

        i_str = strndupa_safe(s, d - s);
        f_str = d + 1;

        r = safe_atolu_full(i_str, 10, &i);
        якщо (r < 0)
                повернути r;

        r = safe_atolu_full(f_str, 10, &f);
        якщо (r < 0)
                повернути r;

        повернути store_loadavg_fixed_point(i, f, ret);
}

/* Limitations are described in https://www.netfilter.org/projects/nftables/manpage.html and
 * https://bugzilla.netfilter.org/show_bug.cgi?id=1175 */
bool nft_identifier_valid(const char *id) {
        якщо (isempty(id))
                повернути false;

        якщо (strlen(id) >= NFT_NAME_MAXLEN)
                повернути false;

        якщо (!ascii_isalpha(id[0]))
                повернути false;

        повернути in_charset(id + 1, ALPHANUMERICAL "/\\_.");
}

ціле parse_capability_set(const char *s, uint64_t initial, uint64_t *current) {
        ціле r;

        assert(s);
        assert(current);

        якщо (isempty(s)) {
                *current = CAP_MASK_UNSET;
                повернути 1;
        }

        bool invert = false;
        якщо (s[0] == '~') {
                invert = true;
                s++;
        }

        uint64_t parsed;
        r = capability_set_from_string(s, &parsed);
        якщо (r < 0)
                повернути r;

        якщо (parsed == 0 || *current == initial)
                /* "~" or uninitialized data -> replace */
                *current = invert ? all_capabilities() & ~parsed : parsed;
        інакше {
                /* previous data -> merge */
                якщо (invert)
                        *current &= ~parsed;
                інакше
                        *current |= parsed;
        }

        повернути r;
}
