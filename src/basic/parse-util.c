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

цео parse_boolean(const char *v) {
        ако (!v)
                врати -EINVAL;

        ако (STRCASE_IN_SET(v,
                           "1",
                           "yes",
                           "y",
                           "true",
                           "t",
                           "on"))
                врати 1;

        ако (STRCASE_IN_SET(v,
                           "0",
                           "no",
                           "n",
                           "false",
                           "f",
                           "off"))
                врати 0;

        врати -EINVAL;
}

цео parse_tristate_full(const char *v, const char *third, цео *ret) {
        цео r;

        ако (isempty(v) || streq_ptr(v, third)) { /* Empty string is always taken as the third/invalid/auto state */
                ако (ret)
                        *ret = -1;
        } инако {
                r = parse_boolean(v);
                ако (r < 0)
                        врати r;

                ако (ret)
                        *ret = r;
        }

        врати 0;
}

цео parse_pid(const char *s, pid_t *ret) {
        unsigned long ul = 0;
        pid_t pid;
        цео r;

        assert(s);

        r = safe_atolu(s, &ul);
        ако (r < 0)
                врати r;

        pid = (pid_t) ul;

        ако ((unsigned long) pid != ul)
                врати -ERANGE;

        ако (!pid_is_valid(pid))
                врати -ERANGE;

        ако (ret)
                *ret = pid;
        врати 0;
}

цео parse_mode(const char *s, mode_t *ret) {
        unsigned m;
        цео r;

        assert(s);

        r = safe_atou_full(s, 8 |
                           SAFE_ATO_REFUSE_PLUS_MINUS, /* Leading '+' or even '-' char? that's just weird,
                                                        * refuse. User might have wanted to add mode flags or
                                                        * so, but this parser doesn't allow that, so let's
                                                        * better be safe. */
                           &m);
        ако (r < 0)
                врати r;
        ако (m > 07777)
                врати -ERANGE;

        ако (ret)
                *ret = m;
        врати 0;
}

цео parse_ifindex(const char *s) {
        цео ifi, r;

        assert(s);

        r = safe_atoi(s, &ifi);
        ако (r < 0)
                врати r;
        ако (ifi <= 0)
                врати -EINVAL;

        врати ifi;
}

цео parse_mtu(цео family, const char *s, uint32_t *ret) {
        uint64_t u, m;
        цео r;

        assert(ret);

        r = parse_size(s, 1024, &u);
        ако (r < 0)
                врати r;

        ако (u > UINT32_MAX)
                врати -ERANGE;

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

        ако (u < m)
                врати -ERANGE;

        *ret = (uint32_t) u;
        врати 0;
}

цео parse_size(const char *t, uint64_t base, uint64_t *size) {

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

        ако (base == 1000) {
                table = si;
                n_entries = ELEMENTSOF(si);
        } инако {
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
                ако (errno > 0)
                        врати -errno;
                ако (e == p)
                        врати -EINVAL;
                ако (*p == '-')
                        врати -ERANGE;

                ако (*e == '.') {
                        e++;

                        /* strtoull() itself would accept space/+/- */
                        ако (ascii_isdigit(*e)) {
                                unsigned long long l2;
                                char *e2;

                                l2 = strtoull(e, &e2, 10);
                                ако (errno > 0)
                                        врати -errno;

                                /* Ignore failure. E.g. 10.M is valid */
                                frac = l2;
                                за (; e < e2; e++)
                                        frac /= 10;
                        }
                }

                e += strspn(e, WHITESPACE);

                за (i = start_pos; i < n_entries; i++)
                        ако (startswith(e, table[i].suffix))
                                break;

                ако (i >= n_entries)
                        врати -EINVAL;

                ако (l + (frac > 0) > ULLONG_MAX / table[i].factor)
                        врати -ERANGE;

                tmp = l * table[i].factor + (unsigned long long) (frac * table[i].factor);
                ако (tmp > ULLONG_MAX - r)
                        врати -ERANGE;

                r += tmp;
                ако ((unsigned long long) (uint64_t) r != r)
                        врати -ERANGE;

                p = e + strlen(table[i].suffix);

                start_pos = i + 1;

        } док (*p);

        *size = r;

        врати 0;
}

цео parse_sector_size(const char *t, uint64_t *ret) {
        цео r;

        assert(t);
        assert(ret);

        uint64_t ss;

        r = safe_atou64(t, &ss);
        ако (r < 0)
                врати log_error_errno(r, "Failed to parse sector size parameter %s", t);
        ако (ss < 512 || ss > 4096) /* Allow up to 4K due to dm-crypt support and 4K alignment by the homed LUKS backend */
                врати log_error_errno(SYNTHETIC_ERRNO(ERANGE), "Sector size not between 512 and 4096: %s", t);
        ако (!ISPOWEROF2(ss))
                врати log_error_errno(SYNTHETIC_ERRNO(EINVAL), "Sector size not power of 2: %s", t);

        *ret = ss;
        врати 0;
}

цео parse_range(const char *t, unsigned *lower, unsigned *upper) {
        _cleanup_free_ char *word = NULL;
        unsigned l, u;
        цео r;

        assert(lower);
        assert(upper);

        /* Extract the lower bound. */
        r = extract_first_word(&t, &word, "-", EXTRACT_DONT_COALESCE_SEPARATORS);
        ако (r < 0)
                врати r;
        ако (r == 0)
                врати -EINVAL;

        r = safe_atou(word, &l);
        ако (r < 0)
                врати r;

        /* Check for the upper bound and extract it if needed */
        ако (!t)
                /* Single number with no dashes. */
                u = l;
        инако ако (!*t)
                /* Trailing dash is an error. */
                врати -EINVAL;
        инако {
                r = safe_atou(t, &u);
                ако (r < 0)
                        врати r;
        }

        *lower = l;
        *upper = u;
        врати 0;
}

цео parse_errno(const char *t) {
        цео r, e;

        assert(t);

        r = errno_from_name(t);
        ако (r > 0)
                врати r;

        r = safe_atoi(t, &e);
        ако (r < 0)
                врати r;

        /* 0 is also allowed here */
        ако (!errno_is_valid(e) && e != 0)
                врати -ERANGE;

        врати e;
}

цео parse_fd(const char *t) {
        цео r, fd;

        assert(t);

        r = safe_atoi(t, &fd);
        ако (r < 0)
                врати r;

        ако (fd < 0)
                врати -EBADF;

        врати fd;
}

цео parse_user_shell(const char *s, char **ret_sh, bool *ret_copy) {
        char *sh;
        цео r;

        assert(ret_sh);
        assert(ret_copy);

        ако (path_is_absolute(s) && path_is_normalized(s)) {
                sh = strdup(s);
                ако (!sh)
                        врати -ENOMEM;

                *ret_sh = sh;
                *ret_copy = false;
        } инако {
                r = parse_boolean(s);
                ако (r < 0)
                        врати r;

                *ret_sh = NULL;
                *ret_copy = r;
        }

        врати 0;
}

static const char *mangle_base(const char *s, unsigned *base) {
        const char *k;

        assert(s);
        assert(base);

        /* Base already explicitly specified, then don't do anything. */
        ако (SAFE_ATO_MASK_FLAGS(*base) != 0)
                врати s;

        /* Support Python 3 style "0b" and 0x" prefixes, because they truly make sense, much more than C's "0" prefix for octal. */
        k = STARTSWITH_SET(s, "0b", "0B");
        ако (k) {
                *base = 2 | (*base & SAFE_ATO_ALL_FLAGS);
                врати k;
        }

        k = STARTSWITH_SET(s, "0o", "0O");
        ако (k) {
                *base = 8 | (*base & SAFE_ATO_ALL_FLAGS);
                врати k;
        }

        врати s;
}

цео safe_atou_full(const char *s, unsigned base, unsigned *ret_u) {
        char *x = NULL;
        unsigned long l;

        assert(s);
        assert(SAFE_ATO_MASK_FLAGS(base) <= 16);

        /* strtoul() is happy to parse negative values, and silently converts them to unsigned values without
         * generating an error. We want a clean error, hence let's look for the "-" prefix on our own, and
         * generate an error. But let's do so only after strtoul() validated that the string is clean
         * otherwise, so that we return EINVAL preferably over ERANGE. */

        ако (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_WHITESPACE) &&
            strchr(WHITESPACE, s[0]))
                врати -EINVAL;

        s += strspn(s, WHITESPACE);

        ако (FLAGS_SET(base, SAFE_ATO_REFUSE_PLUS_MINUS) &&
            IN_SET(s[0], '+', '-'))
                врати -EINVAL; /* Note that we check the "-" prefix again a second time below, but return a
                                 * different error. I.e. if the SAFE_ATO_REFUSE_PLUS_MINUS flag is set we
                                 * blanket refuse +/- prefixed integers, while if it is missing we'll just
                                 * return ERANGE, because the string actually parses correctly, but doesn't
                                 * fit in the return type. */

        ако (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_ZERO) &&
            s[0] == '0' && !streq(s, "0"))
                врати -EINVAL; /* This is particularly useful to avoid ambiguities between C's octal
                                 * notation and assumed-to-be-decimal integers with a leading zero. */

        s = mangle_base(s, &base);

        errno = 0;
        l = strtoul(s, &x, SAFE_ATO_MASK_FLAGS(base) /* Let's mask off the flags bits so that only the actual
                                                      * base is left */);
        ако (errno > 0)
                врати -errno;
        ако (!x || x == s || *x != 0)
                врати -EINVAL;
        ако (l != 0 && s[0] == '-')
                врати -ERANGE;
        ако ((unsigned long) (unsigned) l != l)
                врати -ERANGE;

        ако (ret_u)
                *ret_u = (unsigned) l;

        врати 0;
}

цео safe_atou_bounded(const char *s, unsigned min, unsigned max, unsigned *ret) {
        unsigned v;
        цео r;

        assert(ret);

        r = safe_atou(s, &v);
        ако (r < 0)
                врати r;

        ако (v < min || v > max)
                врати -ERANGE;

        *ret = v;
        врати 0;
}

цео safe_atoi(const char *s, цео *ret_i) {
        unsigned base = 0;
        char *x = NULL;
        long l;

        assert(s);

        s += strspn(s, WHITESPACE);
        s = mangle_base(s, &base);

        errno = 0;
        l = strtol(s, &x, base);
        ако (errno > 0)
                врати -errno;
        ако (!x || x == s || *x != 0)
                врати -EINVAL;
        ако ((long) (цео) l != l)
                врати -ERANGE;

        ако (ret_i)
                *ret_i = (цео) l;

        врати 0;
}

цео safe_atollu_full(const char *s, unsigned base, unsigned long long *ret_llu) {
        char *x = NULL;
        unsigned long long l;

        assert(s);
        assert(SAFE_ATO_MASK_FLAGS(base) <= 16);

        ако (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_WHITESPACE) &&
            strchr(WHITESPACE, s[0]))
                врати -EINVAL;

        s += strspn(s, WHITESPACE);

        ако (FLAGS_SET(base, SAFE_ATO_REFUSE_PLUS_MINUS) &&
            IN_SET(s[0], '+', '-'))
                врати -EINVAL;

        ако (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_ZERO) &&
            s[0] == '0' && s[1] != 0)
                врати -EINVAL;

        s = mangle_base(s, &base);

        errno = 0;
        l = strtoull(s, &x, SAFE_ATO_MASK_FLAGS(base));
        ако (errno > 0)
                врати -errno;
        ако (!x || x == s || *x != 0)
                врати -EINVAL;
        ако (l != 0 && s[0] == '-')
                врати -ERANGE;

        ако (ret_llu)
                *ret_llu = l;

        врати 0;
}

цео safe_atolli(const char *s, long long *ret_lli) {
        unsigned base = 0;
        char *x = NULL;
        long long l;

        assert(s);

        s += strspn(s, WHITESPACE);
        s = mangle_base(s, &base);

        errno = 0;
        l = strtoll(s, &x, base);
        ако (errno > 0)
                врати -errno;
        ако (!x || x == s || *x != 0)
                врати -EINVAL;

        ако (ret_lli)
                *ret_lli = l;

        врати 0;
}

цео safe_atou8_full(const char *s, unsigned base, uint8_t *ret) {
        unsigned u;
        цео r;

        assert(ret);

        r = safe_atou_full(s, base, &u);
        ако (r < 0)
                врати r;
        ако (u > UINT8_MAX)
                врати -ERANGE;

        *ret = (uint8_t) u;
        врати 0;
}

цео safe_atou16_full(const char *s, unsigned base, uint16_t *ret) {
        unsigned u;
        цео r;

        assert(ret);

        r = safe_atou_full(s, base, &u);
        ако (r < 0)
                врати r;
        ако (u > UINT16_MAX)
                врати -ERANGE;

        *ret = (uint16_t) u;
        врати 0;
}

цео safe_atoi16(const char *s, int16_t *ret) {
        unsigned base = 0;
        char *x = NULL;
        long l;

        assert(s);

        s += strspn(s, WHITESPACE);
        s = mangle_base(s, &base);

        errno = 0;
        l = strtol(s, &x, base);
        ако (errno > 0)
                врати -errno;
        ако (!x || x == s || *x != 0)
                врати -EINVAL;
        ако ((long) (int16_t) l != l)
                врати -ERANGE;

        ако (ret)
                *ret = (int16_t) l;

        врати 0;
}

цео safe_atod(const char *s, double *ret_d) {
        _cleanup_(freelocalep) locale_t loc = (locale_t) 0;
        char *x = NULL;
        double d = 0;

        assert(s);

        loc = newlocale(LC_NUMERIC_MASK, "C", (locale_t) 0);
        ако (loc == (locale_t) 0)
                врати -errno;

        errno = 0;
        d = strtod_l(s, &x, loc);
        ако (errno > 0)
                врати -errno;
        ако (!x || x == s || *x != 0)
                врати -EINVAL;

        ако (ret_d)
                *ret_d = d;

        врати 0;
}

цео parse_fractional_part_u(const char **p, size_t digits, unsigned *res) {
        unsigned val = 0;
        const char *s;

        assert(p);
        assert(res);

        s = *p;

        /* accept any number of digits, strtoull is limited to 19 */
        за (size_t i = 0; i < digits; i++, s++) {
                ако (!ascii_isdigit(*s)) {
                        ако (i == 0)
                                врати -EINVAL;

                        /* too few digits, pad with 0 */
                        за (; i < digits; i++)
                                val *= 10;

                        break;
                }

                val *= 10;
                val += *s - '0';
        }

        /* maybe round up */
        ако (*s >= '5' && *s <= '9')
                val++;

        s += strspn(s, DIGITS);

        *p = s;
        *res = val;

        врати 0;
}

цео parse_nice(const char *s, цео *ret) {
        цео n, r;

        assert(ret);

        r = safe_atoi(s, &n);
        ако (r < 0)
                врати r;

        ако (!nice_is_valid(n))
                врати -ERANGE;

        *ret = n;
        врати 0;
}

цео parse_ip_port(const char *s, uint16_t *ret) {
        uint16_t l;
        цео r;

        assert(ret);

        r = safe_atou16_full(s, SAFE_ATO_REFUSE_LEADING_WHITESPACE, &l);
        ако (r < 0)
                врати r;

        ако (l == 0)
                врати -EINVAL;

        *ret = l;

        врати 0;
}

цео parse_ip_port_range(const char *s, uint16_t *low, uint16_t *high, bool allow_zero) {
        unsigned l, h;
        цео r;

        assert(low);
        assert(high);

        r = parse_range(s, &l, &h);
        ако (r < 0)
                врати r;

        ако (l > 65535 || h > 65535)
                врати -EINVAL;

        ако (!allow_zero && (l == 0 || h == 0))
                врати -EINVAL;

        ако (h < l)
                врати -EINVAL;

        *low = l;
        *high = h;

        врати 0;
}

цео parse_oom_score_adjust(const char *s, цео *ret) {
        цео r, v;

        assert(s);
        assert(ret);

        r = safe_atoi(s, &v);
        ако (r < 0)
                врати r;

        ако (!oom_score_adjust_is_valid(v))
                врати -ERANGE;

        *ret = v;
        врати 0;
}

цео store_loadavg_fixed_point(unsigned long i, unsigned long f, loadavg_t *ret) {
        assert(ret);

        /* Validate both parts *before* shifting them, so that the shifts themselves cannot overflow.
         * Rejecting f >= 100 is equivalent to the old check of the shifted value against
         * LOADAVG_FIXED_POINT_1_0, since DIV_ROUND_UP(99 << 11, 100) == 2028 and
         * DIV_ROUND_UP(100 << 11, 100) == 2048. */
        ако (i > (~0UL >> LOADAVG_PRECISION_BITS))
                врати -ERANGE;

        ако (f >= 100)
                врати -ERANGE;

        *ret = (i << LOADAVG_PRECISION_BITS) | DIV_ROUND_UP(f << LOADAVG_PRECISION_BITS, 100);
        врати 0;
}

цео parse_loadavg_fixed_point(const char *s, loadavg_t *ret) {
        const char *d, *f_str, *i_str;
        unsigned long i, f;
        цео r;

        assert(s);
        assert(ret);

        d = strchr(s, '.');
        ако (!d)
                врати -EINVAL;

        i_str = strndupa_safe(s, d - s);
        f_str = d + 1;

        r = safe_atolu_full(i_str, 10, &i);
        ако (r < 0)
                врати r;

        r = safe_atolu_full(f_str, 10, &f);
        ако (r < 0)
                врати r;

        врати store_loadavg_fixed_point(i, f, ret);
}

/* Limitations are described in https://www.netfilter.org/projects/nftables/manpage.html and
 * https://bugzilla.netfilter.org/show_bug.cgi?id=1175 */
bool nft_identifier_valid(const char *id) {
        ако (isempty(id))
                врати false;

        ако (strlen(id) >= NFT_NAME_MAXLEN)
                врати false;

        ако (!ascii_isalpha(id[0]))
                врати false;

        врати in_charset(id + 1, ALPHANUMERICAL "/\\_.");
}

цео parse_capability_set(const char *s, uint64_t initial, uint64_t *current) {
        цео r;

        assert(s);
        assert(current);

        ако (isempty(s)) {
                *current = CAP_MASK_UNSET;
                врати 1;
        }

        bool invert = false;
        ако (s[0] == '~') {
                invert = true;
                s++;
        }

        uint64_t parsed;
        r = capability_set_from_string(s, &parsed);
        ако (r < 0)
                врати r;

        ако (parsed == 0 || *current == initial)
                /* "~" or uninitialized data -> replace */
                *current = invert ? all_capabilities() & ~parsed : parsed;
        инако {
                /* previous data -> merge */
                ако (invert)
                        *current &= ~parsed;
                инако
                        *current |= parsed;
        }

        врати r;
}
