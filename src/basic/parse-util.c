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

정수 parse_boolean(const char *v) {
        만약 (!v)
                반환 -EINVAL;

        만약 (STRCASE_IN_SET(v,
                           "1",
                           "yes",
                           "y",
                           "true",
                           "t",
                           "on"))
                반환 1;

        만약 (STRCASE_IN_SET(v,
                           "0",
                           "no",
                           "n",
                           "false",
                           "f",
                           "off"))
                반환 0;

        반환 -EINVAL;
}

정수 parse_tristate_full(const char *v, const char *third, 정수 *ret) {
        정수 r;

        만약 (isempty(v) || streq_ptr(v, third)) { /* Empty string is always taken as the third/invalid/auto state */
                만약 (ret)
                        *ret = -1;
        } 아니면 {
                r = parse_boolean(v);
                만약 (r < 0)
                        반환 r;

                만약 (ret)
                        *ret = r;
        }

        반환 0;
}

정수 parse_pid(const char *s, pid_t *ret) {
        unsigned long ul = 0;
        pid_t pid;
        정수 r;

        assert(s);

        r = safe_atolu(s, &ul);
        만약 (r < 0)
                반환 r;

        pid = (pid_t) ul;

        만약 ((unsigned long) pid != ul)
                반환 -ERANGE;

        만약 (!pid_is_valid(pid))
                반환 -ERANGE;

        만약 (ret)
                *ret = pid;
        반환 0;
}

정수 parse_mode(const char *s, mode_t *ret) {
        unsigned m;
        정수 r;

        assert(s);

        r = safe_atou_full(s, 8 |
                           SAFE_ATO_REFUSE_PLUS_MINUS, /* Leading '+' or even '-' char? that's just weird,
                                                        * refuse. User might have wanted to add mode flags or
                                                        * so, but this parser doesn't allow that, so let's
                                                        * better be safe. */
                           &m);
        만약 (r < 0)
                반환 r;
        만약 (m > 07777)
                반환 -ERANGE;

        만약 (ret)
                *ret = m;
        반환 0;
}

정수 parse_ifindex(const char *s) {
        정수 ifi, r;

        assert(s);

        r = safe_atoi(s, &ifi);
        만약 (r < 0)
                반환 r;
        만약 (ifi <= 0)
                반환 -EINVAL;

        반환 ifi;
}

정수 parse_mtu(정수 family, const char *s, uint32_t *ret) {
        uint64_t u, m;
        정수 r;

        assert(ret);

        r = parse_size(s, 1024, &u);
        만약 (r < 0)
                반환 r;

        만약 (u > UINT32_MAX)
                반환 -ERANGE;

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

        만약 (u < m)
                반환 -ERANGE;

        *ret = (uint32_t) u;
        반환 0;
}

정수 parse_size(const char *t, uint64_t base, uint64_t *size) {

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

        만약 (base == 1000) {
                table = si;
                n_entries = ELEMENTSOF(si);
        } 아니면 {
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
                만약 (errno > 0)
                        반환 -errno;
                만약 (e == p)
                        반환 -EINVAL;
                만약 (*p == '-')
                        반환 -ERANGE;

                만약 (*e == '.') {
                        e++;

                        /* strtoull() itself would accept space/+/- */
                        만약 (ascii_isdigit(*e)) {
                                unsigned long long l2;
                                char *e2;

                                l2 = strtoull(e, &e2, 10);
                                만약 (errno > 0)
                                        반환 -errno;

                                /* Ignore failure. E.g. 10.M is valid */
                                frac = l2;
                                위해 (; e < e2; e++)
                                        frac /= 10;
                        }
                }

                e += strspn(e, WHITESPACE);

                위해 (i = start_pos; i < n_entries; i++)
                        만약 (startswith(e, table[i].suffix))
                                break;

                만약 (i >= n_entries)
                        반환 -EINVAL;

                만약 (l + (frac > 0) > ULLONG_MAX / table[i].factor)
                        반환 -ERANGE;

                tmp = l * table[i].factor + (unsigned long long) (frac * table[i].factor);
                만약 (tmp > ULLONG_MAX - r)
                        반환 -ERANGE;

                r += tmp;
                만약 ((unsigned long long) (uint64_t) r != r)
                        반환 -ERANGE;

                p = e + strlen(table[i].suffix);

                start_pos = i + 1;

        } 동안 (*p);

        *size = r;

        반환 0;
}

정수 parse_sector_size(const char *t, uint64_t *ret) {
        정수 r;

        assert(t);
        assert(ret);

        uint64_t ss;

        r = safe_atou64(t, &ss);
        만약 (r < 0)
                반환 log_error_errno(r, "Failed to parse sector size parameter %s", t);
        만약 (ss < 512 || ss > 4096) /* Allow up to 4K due to dm-crypt support and 4K alignment by the homed LUKS backend */
                반환 log_error_errno(SYNTHETIC_ERRNO(ERANGE), "Sector size not between 512 and 4096: %s", t);
        만약 (!ISPOWEROF2(ss))
                반환 log_error_errno(SYNTHETIC_ERRNO(EINVAL), "Sector size not power of 2: %s", t);

        *ret = ss;
        반환 0;
}

정수 parse_range(const char *t, unsigned *lower, unsigned *upper) {
        _cleanup_free_ char *word = NULL;
        unsigned l, u;
        정수 r;

        assert(lower);
        assert(upper);

        /* Extract the lower bound. */
        r = extract_first_word(&t, &word, "-", EXTRACT_DONT_COALESCE_SEPARATORS);
        만약 (r < 0)
                반환 r;
        만약 (r == 0)
                반환 -EINVAL;

        r = safe_atou(word, &l);
        만약 (r < 0)
                반환 r;

        /* Check for the upper bound and extract it if needed */
        만약 (!t)
                /* Single number with no dashes. */
                u = l;
        아니면 만약 (!*t)
                /* Trailing dash is an error. */
                반환 -EINVAL;
        아니면 {
                r = safe_atou(t, &u);
                만약 (r < 0)
                        반환 r;
        }

        *lower = l;
        *upper = u;
        반환 0;
}

정수 parse_errno(const char *t) {
        정수 r, e;

        assert(t);

        r = errno_from_name(t);
        만약 (r > 0)
                반환 r;

        r = safe_atoi(t, &e);
        만약 (r < 0)
                반환 r;

        /* 0 is also allowed here */
        만약 (!errno_is_valid(e) && e != 0)
                반환 -ERANGE;

        반환 e;
}

정수 parse_fd(const char *t) {
        정수 r, fd;

        assert(t);

        r = safe_atoi(t, &fd);
        만약 (r < 0)
                반환 r;

        만약 (fd < 0)
                반환 -EBADF;

        반환 fd;
}

정수 parse_user_shell(const char *s, char **ret_sh, bool *ret_copy) {
        char *sh;
        정수 r;

        assert(ret_sh);
        assert(ret_copy);

        만약 (path_is_absolute(s) && path_is_normalized(s)) {
                sh = strdup(s);
                만약 (!sh)
                        반환 -ENOMEM;

                *ret_sh = sh;
                *ret_copy = false;
        } 아니면 {
                r = parse_boolean(s);
                만약 (r < 0)
                        반환 r;

                *ret_sh = NULL;
                *ret_copy = r;
        }

        반환 0;
}

static const char *mangle_base(const char *s, unsigned *base) {
        const char *k;

        assert(s);
        assert(base);

        /* Base already explicitly specified, then don't do anything. */
        만약 (SAFE_ATO_MASK_FLAGS(*base) != 0)
                반환 s;

        /* Support Python 3 style "0b" and 0x" prefixes, because they truly make sense, much more than C's "0" prefix for octal. */
        k = STARTSWITH_SET(s, "0b", "0B");
        만약 (k) {
                *base = 2 | (*base & SAFE_ATO_ALL_FLAGS);
                반환 k;
        }

        k = STARTSWITH_SET(s, "0o", "0O");
        만약 (k) {
                *base = 8 | (*base & SAFE_ATO_ALL_FLAGS);
                반환 k;
        }

        반환 s;
}

정수 safe_atou_full(const char *s, unsigned base, unsigned *ret_u) {
        char *x = NULL;
        unsigned long l;

        assert(s);
        assert(SAFE_ATO_MASK_FLAGS(base) <= 16);

        /* strtoul() is happy to parse negative values, and silently converts them to unsigned values without
         * generating an error. We want a clean error, hence let's look for the "-" prefix on our own, and
         * generate an error. But let's do so only after strtoul() validated that the string is clean
         * otherwise, so that we return EINVAL preferably over ERANGE. */

        만약 (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_WHITESPACE) &&
            strchr(WHITESPACE, s[0]))
                반환 -EINVAL;

        s += strspn(s, WHITESPACE);

        만약 (FLAGS_SET(base, SAFE_ATO_REFUSE_PLUS_MINUS) &&
            IN_SET(s[0], '+', '-'))
                반환 -EINVAL; /* Note that we check the "-" prefix again a second time below, but return a
                                 * different error. I.e. if the SAFE_ATO_REFUSE_PLUS_MINUS flag is set we
                                 * blanket refuse +/- prefixed integers, while if it is missing we'll just
                                 * return ERANGE, because the string actually parses correctly, but doesn't
                                 * fit in the return type. */

        만약 (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_ZERO) &&
            s[0] == '0' && !streq(s, "0"))
                반환 -EINVAL; /* This is particularly useful to avoid ambiguities between C's octal
                                 * notation and assumed-to-be-decimal integers with a leading zero. */

        s = mangle_base(s, &base);

        errno = 0;
        l = strtoul(s, &x, SAFE_ATO_MASK_FLAGS(base) /* Let's mask off the flags bits so that only the actual
                                                      * base is left */);
        만약 (errno > 0)
                반환 -errno;
        만약 (!x || x == s || *x != 0)
                반환 -EINVAL;
        만약 (l != 0 && s[0] == '-')
                반환 -ERANGE;
        만약 ((unsigned long) (unsigned) l != l)
                반환 -ERANGE;

        만약 (ret_u)
                *ret_u = (unsigned) l;

        반환 0;
}

정수 safe_atou_bounded(const char *s, unsigned min, unsigned max, unsigned *ret) {
        unsigned v;
        정수 r;

        assert(ret);

        r = safe_atou(s, &v);
        만약 (r < 0)
                반환 r;

        만약 (v < min || v > max)
                반환 -ERANGE;

        *ret = v;
        반환 0;
}

정수 safe_atoi(const char *s, 정수 *ret_i) {
        unsigned base = 0;
        char *x = NULL;
        long l;

        assert(s);

        s += strspn(s, WHITESPACE);
        s = mangle_base(s, &base);

        errno = 0;
        l = strtol(s, &x, base);
        만약 (errno > 0)
                반환 -errno;
        만약 (!x || x == s || *x != 0)
                반환 -EINVAL;
        만약 ((long) (정수) l != l)
                반환 -ERANGE;

        만약 (ret_i)
                *ret_i = (정수) l;

        반환 0;
}

정수 safe_atollu_full(const char *s, unsigned base, unsigned long long *ret_llu) {
        char *x = NULL;
        unsigned long long l;

        assert(s);
        assert(SAFE_ATO_MASK_FLAGS(base) <= 16);

        만약 (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_WHITESPACE) &&
            strchr(WHITESPACE, s[0]))
                반환 -EINVAL;

        s += strspn(s, WHITESPACE);

        만약 (FLAGS_SET(base, SAFE_ATO_REFUSE_PLUS_MINUS) &&
            IN_SET(s[0], '+', '-'))
                반환 -EINVAL;

        만약 (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_ZERO) &&
            s[0] == '0' && s[1] != 0)
                반환 -EINVAL;

        s = mangle_base(s, &base);

        errno = 0;
        l = strtoull(s, &x, SAFE_ATO_MASK_FLAGS(base));
        만약 (errno > 0)
                반환 -errno;
        만약 (!x || x == s || *x != 0)
                반환 -EINVAL;
        만약 (l != 0 && s[0] == '-')
                반환 -ERANGE;

        만약 (ret_llu)
                *ret_llu = l;

        반환 0;
}

정수 safe_atolli(const char *s, long long *ret_lli) {
        unsigned base = 0;
        char *x = NULL;
        long long l;

        assert(s);

        s += strspn(s, WHITESPACE);
        s = mangle_base(s, &base);

        errno = 0;
        l = strtoll(s, &x, base);
        만약 (errno > 0)
                반환 -errno;
        만약 (!x || x == s || *x != 0)
                반환 -EINVAL;

        만약 (ret_lli)
                *ret_lli = l;

        반환 0;
}

정수 safe_atou8_full(const char *s, unsigned base, uint8_t *ret) {
        unsigned u;
        정수 r;

        assert(ret);

        r = safe_atou_full(s, base, &u);
        만약 (r < 0)
                반환 r;
        만약 (u > UINT8_MAX)
                반환 -ERANGE;

        *ret = (uint8_t) u;
        반환 0;
}

정수 safe_atou16_full(const char *s, unsigned base, uint16_t *ret) {
        unsigned u;
        정수 r;

        assert(ret);

        r = safe_atou_full(s, base, &u);
        만약 (r < 0)
                반환 r;
        만약 (u > UINT16_MAX)
                반환 -ERANGE;

        *ret = (uint16_t) u;
        반환 0;
}

정수 safe_atoi16(const char *s, int16_t *ret) {
        unsigned base = 0;
        char *x = NULL;
        long l;

        assert(s);

        s += strspn(s, WHITESPACE);
        s = mangle_base(s, &base);

        errno = 0;
        l = strtol(s, &x, base);
        만약 (errno > 0)
                반환 -errno;
        만약 (!x || x == s || *x != 0)
                반환 -EINVAL;
        만약 ((long) (int16_t) l != l)
                반환 -ERANGE;

        만약 (ret)
                *ret = (int16_t) l;

        반환 0;
}

정수 safe_atod(const char *s, double *ret_d) {
        _cleanup_(freelocalep) locale_t loc = (locale_t) 0;
        char *x = NULL;
        double d = 0;

        assert(s);

        loc = newlocale(LC_NUMERIC_MASK, "C", (locale_t) 0);
        만약 (loc == (locale_t) 0)
                반환 -errno;

        errno = 0;
        d = strtod_l(s, &x, loc);
        만약 (errno > 0)
                반환 -errno;
        만약 (!x || x == s || *x != 0)
                반환 -EINVAL;

        만약 (ret_d)
                *ret_d = d;

        반환 0;
}

정수 parse_fractional_part_u(const char **p, size_t digits, unsigned *res) {
        unsigned val = 0;
        const char *s;

        assert(p);
        assert(res);

        s = *p;

        /* accept any number of digits, strtoull is limited to 19 */
        위해 (size_t i = 0; i < digits; i++, s++) {
                만약 (!ascii_isdigit(*s)) {
                        만약 (i == 0)
                                반환 -EINVAL;

                        /* too few digits, pad with 0 */
                        위해 (; i < digits; i++)
                                val *= 10;

                        break;
                }

                val *= 10;
                val += *s - '0';
        }

        /* maybe round up */
        만약 (*s >= '5' && *s <= '9')
                val++;

        s += strspn(s, DIGITS);

        *p = s;
        *res = val;

        반환 0;
}

정수 parse_nice(const char *s, 정수 *ret) {
        정수 n, r;

        assert(ret);

        r = safe_atoi(s, &n);
        만약 (r < 0)
                반환 r;

        만약 (!nice_is_valid(n))
                반환 -ERANGE;

        *ret = n;
        반환 0;
}

정수 parse_ip_port(const char *s, uint16_t *ret) {
        uint16_t l;
        정수 r;

        assert(ret);

        r = safe_atou16_full(s, SAFE_ATO_REFUSE_LEADING_WHITESPACE, &l);
        만약 (r < 0)
                반환 r;

        만약 (l == 0)
                반환 -EINVAL;

        *ret = l;

        반환 0;
}

정수 parse_ip_port_range(const char *s, uint16_t *low, uint16_t *high, bool allow_zero) {
        unsigned l, h;
        정수 r;

        assert(low);
        assert(high);

        r = parse_range(s, &l, &h);
        만약 (r < 0)
                반환 r;

        만약 (l > 65535 || h > 65535)
                반환 -EINVAL;

        만약 (!allow_zero && (l == 0 || h == 0))
                반환 -EINVAL;

        만약 (h < l)
                반환 -EINVAL;

        *low = l;
        *high = h;

        반환 0;
}

정수 parse_oom_score_adjust(const char *s, 정수 *ret) {
        정수 r, v;

        assert(s);
        assert(ret);

        r = safe_atoi(s, &v);
        만약 (r < 0)
                반환 r;

        만약 (!oom_score_adjust_is_valid(v))
                반환 -ERANGE;

        *ret = v;
        반환 0;
}

정수 store_loadavg_fixed_point(unsigned long i, unsigned long f, loadavg_t *ret) {
        assert(ret);

        /* Validate both parts *before* shifting them, so that the shifts themselves cannot overflow.
         * Rejecting f >= 100 is equivalent to the old check of the shifted value against
         * LOADAVG_FIXED_POINT_1_0, since DIV_ROUND_UP(99 << 11, 100) == 2028 and
         * DIV_ROUND_UP(100 << 11, 100) == 2048. */
        만약 (i > (~0UL >> LOADAVG_PRECISION_BITS))
                반환 -ERANGE;

        만약 (f >= 100)
                반환 -ERANGE;

        *ret = (i << LOADAVG_PRECISION_BITS) | DIV_ROUND_UP(f << LOADAVG_PRECISION_BITS, 100);
        반환 0;
}

정수 parse_loadavg_fixed_point(const char *s, loadavg_t *ret) {
        const char *d, *f_str, *i_str;
        unsigned long i, f;
        정수 r;

        assert(s);
        assert(ret);

        d = strchr(s, '.');
        만약 (!d)
                반환 -EINVAL;

        i_str = strndupa_safe(s, d - s);
        f_str = d + 1;

        r = safe_atolu_full(i_str, 10, &i);
        만약 (r < 0)
                반환 r;

        r = safe_atolu_full(f_str, 10, &f);
        만약 (r < 0)
                반환 r;

        반환 store_loadavg_fixed_point(i, f, ret);
}

/* Limitations are described in https://www.netfilter.org/projects/nftables/manpage.html and
 * https://bugzilla.netfilter.org/show_bug.cgi?id=1175 */
bool nft_identifier_valid(const char *id) {
        만약 (isempty(id))
                반환 false;

        만약 (strlen(id) >= NFT_NAME_MAXLEN)
                반환 false;

        만약 (!ascii_isalpha(id[0]))
                반환 false;

        반환 in_charset(id + 1, ALPHANUMERICAL "/\\_.");
}

정수 parse_capability_set(const char *s, uint64_t initial, uint64_t *current) {
        정수 r;

        assert(s);
        assert(current);

        만약 (isempty(s)) {
                *current = CAP_MASK_UNSET;
                반환 1;
        }

        bool invert = false;
        만약 (s[0] == '~') {
                invert = true;
                s++;
        }

        uint64_t parsed;
        r = capability_set_from_string(s, &parsed);
        만약 (r < 0)
                반환 r;

        만약 (parsed == 0 || *current == initial)
                /* "~" or uninitialized data -> replace */
                *current = invert ? all_capabilities() & ~parsed : parsed;
        아니면 {
                /* previous data -> merge */
                만약 (invert)
                        *current &= ~parsed;
                아니면
                        *current |= parsed;
        }

        반환 r;
}
