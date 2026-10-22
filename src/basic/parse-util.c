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

целое parse_boolean(const char *v) {
        если (!v)
                вернуть -EINVAL;

        если (STRCASE_IN_SET(v,
                           "1",
                           "yes",
                           "y",
                           "true",
                           "t",
                           "on"))
                вернуть 1;

        если (STRCASE_IN_SET(v,
                           "0",
                           "no",
                           "n",
                           "false",
                           "f",
                           "off"))
                вернуть 0;

        вернуть -EINVAL;
}

целое parse_tristate_full(const char *v, const char *third, целое *ret) {
        целое r;

        если (isempty(v) || streq_ptr(v, third)) { /* Empty string is always taken as the third/invalid/auto state */
                если (ret)
                        *ret = -1;
        } иначе {
                r = parse_boolean(v);
                если (r < 0)
                        вернуть r;

                если (ret)
                        *ret = r;
        }

        вернуть 0;
}

целое parse_pid(const char *s, pid_t *ret) {
        unsigned long ul = 0;
        pid_t pid;
        целое r;

        assert(s);

        r = safe_atolu(s, &ul);
        если (r < 0)
                вернуть r;

        pid = (pid_t) ul;

        если ((unsigned long) pid != ul)
                вернуть -ERANGE;

        если (!pid_is_valid(pid))
                вернуть -ERANGE;

        если (ret)
                *ret = pid;
        вернуть 0;
}

целое parse_mode(const char *s, mode_t *ret) {
        unsigned m;
        целое r;

        assert(s);

        r = safe_atou_full(s, 8 |
                           SAFE_ATO_REFUSE_PLUS_MINUS, /* Leading '+' or even '-' char? that's just weird,
                                                        * refuse. User might have wanted to add mode flags or
                                                        * so, but this parser doesn't allow that, so let's
                                                        * better be safe. */
                           &m);
        если (r < 0)
                вернуть r;
        если (m > 07777)
                вернуть -ERANGE;

        если (ret)
                *ret = m;
        вернуть 0;
}

целое parse_ifindex(const char *s) {
        целое ifi, r;

        assert(s);

        r = safe_atoi(s, &ifi);
        если (r < 0)
                вернуть r;
        если (ifi <= 0)
                вернуть -EINVAL;

        вернуть ifi;
}

целое parse_mtu(целое family, const char *s, uint32_t *ret) {
        uint64_t u, m;
        целое r;

        assert(ret);

        r = parse_size(s, 1024, &u);
        если (r < 0)
                вернуть r;

        если (u > UINT32_MAX)
                вернуть -ERANGE;

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

        если (u < m)
                вернуть -ERANGE;

        *ret = (uint32_t) u;
        вернуть 0;
}

целое parse_size(const char *t, uint64_t base, uint64_t *size) {

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

        если (base == 1000) {
                table = si;
                n_entries = ELEMENTSOF(si);
        } иначе {
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
                если (errno > 0)
                        вернуть -errno;
                если (e == p)
                        вернуть -EINVAL;
                если (*p == '-')
                        вернуть -ERANGE;

                если (*e == '.') {
                        e++;

                        /* strtoull() itself would accept space/+/- */
                        если (ascii_isdigit(*e)) {
                                unsigned long long l2;
                                char *e2;

                                l2 = strtoull(e, &e2, 10);
                                если (errno > 0)
                                        вернуть -errno;

                                /* Ignore failure. E.g. 10.M is valid */
                                frac = l2;
                                для (; e < e2; e++)
                                        frac /= 10;
                        }
                }

                e += strspn(e, WHITESPACE);

                для (i = start_pos; i < n_entries; i++)
                        если (startswith(e, table[i].suffix))
                                break;

                если (i >= n_entries)
                        вернуть -EINVAL;

                если (l + (frac > 0) > ULLONG_MAX / table[i].factor)
                        вернуть -ERANGE;

                tmp = l * table[i].factor + (unsigned long long) (frac * table[i].factor);
                если (tmp > ULLONG_MAX - r)
                        вернуть -ERANGE;

                r += tmp;
                если ((unsigned long long) (uint64_t) r != r)
                        вернуть -ERANGE;

                p = e + strlen(table[i].suffix);

                start_pos = i + 1;

        } пока (*p);

        *size = r;

        вернуть 0;
}

целое parse_sector_size(const char *t, uint64_t *ret) {
        целое r;

        assert(t);
        assert(ret);

        uint64_t ss;

        r = safe_atou64(t, &ss);
        если (r < 0)
                вернуть log_error_errno(r, "Failed to parse sector size parameter %s", t);
        если (ss < 512 || ss > 4096) /* Allow up to 4K due to dm-crypt support and 4K alignment by the homed LUKS backend */
                вернуть log_error_errno(SYNTHETIC_ERRNO(ERANGE), "Sector size not between 512 and 4096: %s", t);
        если (!ISPOWEROF2(ss))
                вернуть log_error_errno(SYNTHETIC_ERRNO(EINVAL), "Sector size not power of 2: %s", t);

        *ret = ss;
        вернуть 0;
}

целое parse_range(const char *t, unsigned *lower, unsigned *upper) {
        _cleanup_free_ char *word = NULL;
        unsigned l, u;
        целое r;

        assert(lower);
        assert(upper);

        /* Extract the lower bound. */
        r = extract_first_word(&t, &word, "-", EXTRACT_DONT_COALESCE_SEPARATORS);
        если (r < 0)
                вернуть r;
        если (r == 0)
                вернуть -EINVAL;

        r = safe_atou(word, &l);
        если (r < 0)
                вернуть r;

        /* Check for the upper bound and extract it if needed */
        если (!t)
                /* Single number with no dashes. */
                u = l;
        иначе если (!*t)
                /* Trailing dash is an error. */
                вернуть -EINVAL;
        иначе {
                r = safe_atou(t, &u);
                если (r < 0)
                        вернуть r;
        }

        *lower = l;
        *upper = u;
        вернуть 0;
}

целое parse_errno(const char *t) {
        целое r, e;

        assert(t);

        r = errno_from_name(t);
        если (r > 0)
                вернуть r;

        r = safe_atoi(t, &e);
        если (r < 0)
                вернуть r;

        /* 0 is also allowed here */
        если (!errno_is_valid(e) && e != 0)
                вернуть -ERANGE;

        вернуть e;
}

целое parse_fd(const char *t) {
        целое r, fd;

        assert(t);

        r = safe_atoi(t, &fd);
        если (r < 0)
                вернуть r;

        если (fd < 0)
                вернуть -EBADF;

        вернуть fd;
}

целое parse_user_shell(const char *s, char **ret_sh, bool *ret_copy) {
        char *sh;
        целое r;

        assert(ret_sh);
        assert(ret_copy);

        если (path_is_absolute(s) && path_is_normalized(s)) {
                sh = strdup(s);
                если (!sh)
                        вернуть -ENOMEM;

                *ret_sh = sh;
                *ret_copy = false;
        } иначе {
                r = parse_boolean(s);
                если (r < 0)
                        вернуть r;

                *ret_sh = NULL;
                *ret_copy = r;
        }

        вернуть 0;
}

static const char *mangle_base(const char *s, unsigned *base) {
        const char *k;

        assert(s);
        assert(base);

        /* Base already explicitly specified, then don't do anything. */
        если (SAFE_ATO_MASK_FLAGS(*base) != 0)
                вернуть s;

        /* Support Python 3 style "0b" and 0x" prefixes, because they truly make sense, much more than C's "0" prefix for octal. */
        k = STARTSWITH_SET(s, "0b", "0B");
        если (k) {
                *base = 2 | (*base & SAFE_ATO_ALL_FLAGS);
                вернуть k;
        }

        k = STARTSWITH_SET(s, "0o", "0O");
        если (k) {
                *base = 8 | (*base & SAFE_ATO_ALL_FLAGS);
                вернуть k;
        }

        вернуть s;
}

целое safe_atou_full(const char *s, unsigned base, unsigned *ret_u) {
        char *x = NULL;
        unsigned long l;

        assert(s);
        assert(SAFE_ATO_MASK_FLAGS(base) <= 16);

        /* strtoul() is happy to parse negative values, and silently converts them to unsigned values without
         * generating an error. We want a clean error, hence let's look for the "-" prefix on our own, and
         * generate an error. But let's do so only after strtoul() validated that the string is clean
         * otherwise, so that we return EINVAL preferably over ERANGE. */

        если (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_WHITESPACE) &&
            strchr(WHITESPACE, s[0]))
                вернуть -EINVAL;

        s += strspn(s, WHITESPACE);

        если (FLAGS_SET(base, SAFE_ATO_REFUSE_PLUS_MINUS) &&
            IN_SET(s[0], '+', '-'))
                вернуть -EINVAL; /* Note that we check the "-" prefix again a second time below, but return a
                                 * different error. I.e. if the SAFE_ATO_REFUSE_PLUS_MINUS flag is set we
                                 * blanket refuse +/- prefixed integers, while if it is missing we'll just
                                 * return ERANGE, because the string actually parses correctly, but doesn't
                                 * fit in the return type. */

        если (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_ZERO) &&
            s[0] == '0' && !streq(s, "0"))
                вернуть -EINVAL; /* This is particularly useful to avoid ambiguities between C's octal
                                 * notation and assumed-to-be-decimal integers with a leading zero. */

        s = mangle_base(s, &base);

        errno = 0;
        l = strtoul(s, &x, SAFE_ATO_MASK_FLAGS(base) /* Let's mask off the flags bits so that only the actual
                                                      * base is left */);
        если (errno > 0)
                вернуть -errno;
        если (!x || x == s || *x != 0)
                вернуть -EINVAL;
        если (l != 0 && s[0] == '-')
                вернуть -ERANGE;
        если ((unsigned long) (unsigned) l != l)
                вернуть -ERANGE;

        если (ret_u)
                *ret_u = (unsigned) l;

        вернуть 0;
}

целое safe_atou_bounded(const char *s, unsigned min, unsigned max, unsigned *ret) {
        unsigned v;
        целое r;

        assert(ret);

        r = safe_atou(s, &v);
        если (r < 0)
                вернуть r;

        если (v < min || v > max)
                вернуть -ERANGE;

        *ret = v;
        вернуть 0;
}

целое safe_atoi(const char *s, целое *ret_i) {
        unsigned base = 0;
        char *x = NULL;
        long l;

        assert(s);

        s += strspn(s, WHITESPACE);
        s = mangle_base(s, &base);

        errno = 0;
        l = strtol(s, &x, base);
        если (errno > 0)
                вернуть -errno;
        если (!x || x == s || *x != 0)
                вернуть -EINVAL;
        если ((long) (целое) l != l)
                вернуть -ERANGE;

        если (ret_i)
                *ret_i = (целое) l;

        вернуть 0;
}

целое safe_atollu_full(const char *s, unsigned base, unsigned long long *ret_llu) {
        char *x = NULL;
        unsigned long long l;

        assert(s);
        assert(SAFE_ATO_MASK_FLAGS(base) <= 16);

        если (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_WHITESPACE) &&
            strchr(WHITESPACE, s[0]))
                вернуть -EINVAL;

        s += strspn(s, WHITESPACE);

        если (FLAGS_SET(base, SAFE_ATO_REFUSE_PLUS_MINUS) &&
            IN_SET(s[0], '+', '-'))
                вернуть -EINVAL;

        если (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_ZERO) &&
            s[0] == '0' && s[1] != 0)
                вернуть -EINVAL;

        s = mangle_base(s, &base);

        errno = 0;
        l = strtoull(s, &x, SAFE_ATO_MASK_FLAGS(base));
        если (errno > 0)
                вернуть -errno;
        если (!x || x == s || *x != 0)
                вернуть -EINVAL;
        если (l != 0 && s[0] == '-')
                вернуть -ERANGE;

        если (ret_llu)
                *ret_llu = l;

        вернуть 0;
}

целое safe_atolli(const char *s, long long *ret_lli) {
        unsigned base = 0;
        char *x = NULL;
        long long l;

        assert(s);

        s += strspn(s, WHITESPACE);
        s = mangle_base(s, &base);

        errno = 0;
        l = strtoll(s, &x, base);
        если (errno > 0)
                вернуть -errno;
        если (!x || x == s || *x != 0)
                вернуть -EINVAL;

        если (ret_lli)
                *ret_lli = l;

        вернуть 0;
}

целое safe_atou8_full(const char *s, unsigned base, uint8_t *ret) {
        unsigned u;
        целое r;

        assert(ret);

        r = safe_atou_full(s, base, &u);
        если (r < 0)
                вернуть r;
        если (u > UINT8_MAX)
                вернуть -ERANGE;

        *ret = (uint8_t) u;
        вернуть 0;
}

целое safe_atou16_full(const char *s, unsigned base, uint16_t *ret) {
        unsigned u;
        целое r;

        assert(ret);

        r = safe_atou_full(s, base, &u);
        если (r < 0)
                вернуть r;
        если (u > UINT16_MAX)
                вернуть -ERANGE;

        *ret = (uint16_t) u;
        вернуть 0;
}

целое safe_atoi16(const char *s, int16_t *ret) {
        unsigned base = 0;
        char *x = NULL;
        long l;

        assert(s);

        s += strspn(s, WHITESPACE);
        s = mangle_base(s, &base);

        errno = 0;
        l = strtol(s, &x, base);
        если (errno > 0)
                вернуть -errno;
        если (!x || x == s || *x != 0)
                вернуть -EINVAL;
        если ((long) (int16_t) l != l)
                вернуть -ERANGE;

        если (ret)
                *ret = (int16_t) l;

        вернуть 0;
}

целое safe_atod(const char *s, double *ret_d) {
        _cleanup_(freelocalep) locale_t loc = (locale_t) 0;
        char *x = NULL;
        double d = 0;

        assert(s);

        loc = newlocale(LC_NUMERIC_MASK, "C", (locale_t) 0);
        если (loc == (locale_t) 0)
                вернуть -errno;

        errno = 0;
        d = strtod_l(s, &x, loc);
        если (errno > 0)
                вернуть -errno;
        если (!x || x == s || *x != 0)
                вернуть -EINVAL;

        если (ret_d)
                *ret_d = d;

        вернуть 0;
}

целое parse_fractional_part_u(const char **p, size_t digits, unsigned *res) {
        unsigned val = 0;
        const char *s;

        assert(p);
        assert(res);

        s = *p;

        /* accept any number of digits, strtoull is limited to 19 */
        для (size_t i = 0; i < digits; i++, s++) {
                если (!ascii_isdigit(*s)) {
                        если (i == 0)
                                вернуть -EINVAL;

                        /* too few digits, pad with 0 */
                        для (; i < digits; i++)
                                val *= 10;

                        break;
                }

                val *= 10;
                val += *s - '0';
        }

        /* maybe round up */
        если (*s >= '5' && *s <= '9')
                val++;

        s += strspn(s, DIGITS);

        *p = s;
        *res = val;

        вернуть 0;
}

целое parse_nice(const char *s, целое *ret) {
        целое n, r;

        assert(ret);

        r = safe_atoi(s, &n);
        если (r < 0)
                вернуть r;

        если (!nice_is_valid(n))
                вернуть -ERANGE;

        *ret = n;
        вернуть 0;
}

целое parse_ip_port(const char *s, uint16_t *ret) {
        uint16_t l;
        целое r;

        assert(ret);

        r = safe_atou16_full(s, SAFE_ATO_REFUSE_LEADING_WHITESPACE, &l);
        если (r < 0)
                вернуть r;

        если (l == 0)
                вернуть -EINVAL;

        *ret = l;

        вернуть 0;
}

целое parse_ip_port_range(const char *s, uint16_t *low, uint16_t *high, bool allow_zero) {
        unsigned l, h;
        целое r;

        assert(low);
        assert(high);

        r = parse_range(s, &l, &h);
        если (r < 0)
                вернуть r;

        если (l > 65535 || h > 65535)
                вернуть -EINVAL;

        если (!allow_zero && (l == 0 || h == 0))
                вернуть -EINVAL;

        если (h < l)
                вернуть -EINVAL;

        *low = l;
        *high = h;

        вернуть 0;
}

целое parse_oom_score_adjust(const char *s, целое *ret) {
        целое r, v;

        assert(s);
        assert(ret);

        r = safe_atoi(s, &v);
        если (r < 0)
                вернуть r;

        если (!oom_score_adjust_is_valid(v))
                вернуть -ERANGE;

        *ret = v;
        вернуть 0;
}

целое store_loadavg_fixed_point(unsigned long i, unsigned long f, loadavg_t *ret) {
        assert(ret);

        /* Validate both parts *before* shifting them, so that the shifts themselves cannot overflow.
         * Rejecting f >= 100 is equivalent to the old check of the shifted value against
         * LOADAVG_FIXED_POINT_1_0, since DIV_ROUND_UP(99 << 11, 100) == 2028 and
         * DIV_ROUND_UP(100 << 11, 100) == 2048. */
        если (i > (~0UL >> LOADAVG_PRECISION_BITS))
                вернуть -ERANGE;

        если (f >= 100)
                вернуть -ERANGE;

        *ret = (i << LOADAVG_PRECISION_BITS) | DIV_ROUND_UP(f << LOADAVG_PRECISION_BITS, 100);
        вернуть 0;
}

целое parse_loadavg_fixed_point(const char *s, loadavg_t *ret) {
        const char *d, *f_str, *i_str;
        unsigned long i, f;
        целое r;

        assert(s);
        assert(ret);

        d = strchr(s, '.');
        если (!d)
                вернуть -EINVAL;

        i_str = strndupa_safe(s, d - s);
        f_str = d + 1;

        r = safe_atolu_full(i_str, 10, &i);
        если (r < 0)
                вернуть r;

        r = safe_atolu_full(f_str, 10, &f);
        если (r < 0)
                вернуть r;

        вернуть store_loadavg_fixed_point(i, f, ret);
}

/* Limitations are described in https://www.netfilter.org/projects/nftables/manpage.html and
 * https://bugzilla.netfilter.org/show_bug.cgi?id=1175 */
bool nft_identifier_valid(const char *id) {
        если (isempty(id))
                вернуть false;

        если (strlen(id) >= NFT_NAME_MAXLEN)
                вернуть false;

        если (!ascii_isalpha(id[0]))
                вернуть false;

        вернуть in_charset(id + 1, ALPHANUMERICAL "/\\_.");
}

целое parse_capability_set(const char *s, uint64_t initial, uint64_t *current) {
        целое r;

        assert(s);
        assert(current);

        если (isempty(s)) {
                *current = CAP_MASK_UNSET;
                вернуть 1;
        }

        bool invert = false;
        если (s[0] == '~') {
                invert = true;
                s++;
        }

        uint64_t parsed;
        r = capability_set_from_string(s, &parsed);
        если (r < 0)
                вернуть r;

        если (parsed == 0 || *current == initial)
                /* "~" or uninitialized data -> replace */
                *current = invert ? all_capabilities() & ~parsed : parsed;
        иначе {
                /* previous data -> merge */
                если (invert)
                        *current &= ~parsed;
                иначе
                        *current |= parsed;
        }

        вернуть r;
}
