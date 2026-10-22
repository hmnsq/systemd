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

цяло parse_boolean(const char *v) {
        ако (!v)
                върни -EINVAL;

        ако (STRCASE_IN_SET(v,
                           "1",
                           "yes",
                           "y",
                           "true",
                           "t",
                           "on"))
                върни 1;

        ако (STRCASE_IN_SET(v,
                           "0",
                           "no",
                           "n",
                           "false",
                           "f",
                           "off"))
                върни 0;

        върни -EINVAL;
}

цяло parse_tristate_full(const char *v, const char *third, цяло *ret) {
        цяло r;

        ако (isempty(v) || streq_ptr(v, third)) { /* Empty string is always taken as the third/invalid/auto state */
                ако (ret)
                        *ret = -1;
        } иначе {
                r = parse_boolean(v);
                ако (r < 0)
                        върни r;

                ако (ret)
                        *ret = r;
        }

        върни 0;
}

цяло parse_pid(const char *s, pid_t *ret) {
        unsigned long ul = 0;
        pid_t pid;
        цяло r;

        assert(s);

        r = safe_atolu(s, &ul);
        ако (r < 0)
                върни r;

        pid = (pid_t) ul;

        ако ((unsigned long) pid != ul)
                върни -ERANGE;

        ако (!pid_is_valid(pid))
                върни -ERANGE;

        ако (ret)
                *ret = pid;
        върни 0;
}

цяло parse_mode(const char *s, mode_t *ret) {
        unsigned m;
        цяло r;

        assert(s);

        r = safe_atou_full(s, 8 |
                           SAFE_ATO_REFUSE_PLUS_MINUS, /* Leading '+' or even '-' char? that's just weird,
                                                        * refuse. User might have wanted to add mode flags or
                                                        * so, but this parser doesn't allow that, so let's
                                                        * better be safe. */
                           &m);
        ако (r < 0)
                върни r;
        ако (m > 07777)
                върни -ERANGE;

        ако (ret)
                *ret = m;
        върни 0;
}

цяло parse_ifindex(const char *s) {
        цяло ifi, r;

        assert(s);

        r = safe_atoi(s, &ifi);
        ако (r < 0)
                върни r;
        ако (ifi <= 0)
                върни -EINVAL;

        върни ifi;
}

цяло parse_mtu(цяло family, const char *s, uint32_t *ret) {
        uint64_t u, m;
        цяло r;

        assert(ret);

        r = parse_size(s, 1024, &u);
        ако (r < 0)
                върни r;

        ако (u > UINT32_MAX)
                върни -ERANGE;

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
                върни -ERANGE;

        *ret = (uint32_t) u;
        върни 0;
}

цяло parse_size(const char *t, uint64_t base, uint64_t *size) {

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
                ако (errno > 0)
                        върни -errno;
                ако (e == p)
                        върни -EINVAL;
                ако (*p == '-')
                        върни -ERANGE;

                ако (*e == '.') {
                        e++;

                        /* strtoull() itself would accept space/+/- */
                        ако (ascii_isdigit(*e)) {
                                unsigned long long l2;
                                char *e2;

                                l2 = strtoull(e, &e2, 10);
                                ако (errno > 0)
                                        върни -errno;

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
                        върни -EINVAL;

                ако (l + (frac > 0) > ULLONG_MAX / table[i].factor)
                        върни -ERANGE;

                tmp = l * table[i].factor + (unsigned long long) (frac * table[i].factor);
                ако (tmp > ULLONG_MAX - r)
                        върни -ERANGE;

                r += tmp;
                ако ((unsigned long long) (uint64_t) r != r)
                        върни -ERANGE;

                p = e + strlen(table[i].suffix);

                start_pos = i + 1;

        } докато (*p);

        *size = r;

        върни 0;
}

цяло parse_sector_size(const char *t, uint64_t *ret) {
        цяло r;

        assert(t);
        assert(ret);

        uint64_t ss;

        r = safe_atou64(t, &ss);
        ако (r < 0)
                върни log_error_errno(r, "Failed to parse sector size parameter %s", t);
        ако (ss < 512 || ss > 4096) /* Allow up to 4K due to dm-crypt support and 4K alignment by the homed LUKS backend */
                върни log_error_errno(SYNTHETIC_ERRNO(ERANGE), "Sector size not between 512 and 4096: %s", t);
        ако (!ISPOWEROF2(ss))
                върни log_error_errno(SYNTHETIC_ERRNO(EINVAL), "Sector size not power of 2: %s", t);

        *ret = ss;
        върни 0;
}

цяло parse_range(const char *t, unsigned *lower, unsigned *upper) {
        _cleanup_free_ char *word = NULL;
        unsigned l, u;
        цяло r;

        assert(lower);
        assert(upper);

        /* Extract the lower bound. */
        r = extract_first_word(&t, &word, "-", EXTRACT_DONT_COALESCE_SEPARATORS);
        ако (r < 0)
                върни r;
        ако (r == 0)
                върни -EINVAL;

        r = safe_atou(word, &l);
        ако (r < 0)
                върни r;

        /* Check for the upper bound and extract it if needed */
        ако (!t)
                /* Single number with no dashes. */
                u = l;
        иначе ако (!*t)
                /* Trailing dash is an error. */
                върни -EINVAL;
        иначе {
                r = safe_atou(t, &u);
                ако (r < 0)
                        върни r;
        }

        *lower = l;
        *upper = u;
        върни 0;
}

цяло parse_errno(const char *t) {
        цяло r, e;

        assert(t);

        r = errno_from_name(t);
        ако (r > 0)
                върни r;

        r = safe_atoi(t, &e);
        ако (r < 0)
                върни r;

        /* 0 is also allowed here */
        ако (!errno_is_valid(e) && e != 0)
                върни -ERANGE;

        върни e;
}

цяло parse_fd(const char *t) {
        цяло r, fd;

        assert(t);

        r = safe_atoi(t, &fd);
        ако (r < 0)
                върни r;

        ако (fd < 0)
                върни -EBADF;

        върни fd;
}

цяло parse_user_shell(const char *s, char **ret_sh, bool *ret_copy) {
        char *sh;
        цяло r;

        assert(ret_sh);
        assert(ret_copy);

        ако (path_is_absolute(s) && path_is_normalized(s)) {
                sh = strdup(s);
                ако (!sh)
                        върни -ENOMEM;

                *ret_sh = sh;
                *ret_copy = false;
        } иначе {
                r = parse_boolean(s);
                ако (r < 0)
                        върни r;

                *ret_sh = NULL;
                *ret_copy = r;
        }

        върни 0;
}

static const char *mangle_base(const char *s, unsigned *base) {
        const char *k;

        assert(s);
        assert(base);

        /* Base already explicitly specified, then don't do anything. */
        ако (SAFE_ATO_MASK_FLAGS(*base) != 0)
                върни s;

        /* Support Python 3 style "0b" and 0x" prefixes, because they truly make sense, much more than C's "0" prefix for octal. */
        k = STARTSWITH_SET(s, "0b", "0B");
        ако (k) {
                *base = 2 | (*base & SAFE_ATO_ALL_FLAGS);
                върни k;
        }

        k = STARTSWITH_SET(s, "0o", "0O");
        ако (k) {
                *base = 8 | (*base & SAFE_ATO_ALL_FLAGS);
                върни k;
        }

        върни s;
}

цяло safe_atou_full(const char *s, unsigned base, unsigned *ret_u) {
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
                върни -EINVAL;

        s += strspn(s, WHITESPACE);

        ако (FLAGS_SET(base, SAFE_ATO_REFUSE_PLUS_MINUS) &&
            IN_SET(s[0], '+', '-'))
                върни -EINVAL; /* Note that we check the "-" prefix again a second time below, but return a
                                 * different error. I.e. if the SAFE_ATO_REFUSE_PLUS_MINUS flag is set we
                                 * blanket refuse +/- prefixed integers, while if it is missing we'll just
                                 * return ERANGE, because the string actually parses correctly, but doesn't
                                 * fit in the return type. */

        ако (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_ZERO) &&
            s[0] == '0' && !streq(s, "0"))
                върни -EINVAL; /* This is particularly useful to avoid ambiguities between C's octal
                                 * notation and assumed-to-be-decimal integers with a leading zero. */

        s = mangle_base(s, &base);

        errno = 0;
        l = strtoul(s, &x, SAFE_ATO_MASK_FLAGS(base) /* Let's mask off the flags bits so that only the actual
                                                      * base is left */);
        ако (errno > 0)
                върни -errno;
        ако (!x || x == s || *x != 0)
                върни -EINVAL;
        ако (l != 0 && s[0] == '-')
                върни -ERANGE;
        ако ((unsigned long) (unsigned) l != l)
                върни -ERANGE;

        ако (ret_u)
                *ret_u = (unsigned) l;

        върни 0;
}

цяло safe_atou_bounded(const char *s, unsigned min, unsigned max, unsigned *ret) {
        unsigned v;
        цяло r;

        assert(ret);

        r = safe_atou(s, &v);
        ако (r < 0)
                върни r;

        ако (v < min || v > max)
                върни -ERANGE;

        *ret = v;
        върни 0;
}

цяло safe_atoi(const char *s, цяло *ret_i) {
        unsigned base = 0;
        char *x = NULL;
        long l;

        assert(s);

        s += strspn(s, WHITESPACE);
        s = mangle_base(s, &base);

        errno = 0;
        l = strtol(s, &x, base);
        ако (errno > 0)
                върни -errno;
        ако (!x || x == s || *x != 0)
                върни -EINVAL;
        ако ((long) (цяло) l != l)
                върни -ERANGE;

        ако (ret_i)
                *ret_i = (цяло) l;

        върни 0;
}

цяло safe_atollu_full(const char *s, unsigned base, unsigned long long *ret_llu) {
        char *x = NULL;
        unsigned long long l;

        assert(s);
        assert(SAFE_ATO_MASK_FLAGS(base) <= 16);

        ако (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_WHITESPACE) &&
            strchr(WHITESPACE, s[0]))
                върни -EINVAL;

        s += strspn(s, WHITESPACE);

        ако (FLAGS_SET(base, SAFE_ATO_REFUSE_PLUS_MINUS) &&
            IN_SET(s[0], '+', '-'))
                върни -EINVAL;

        ако (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_ZERO) &&
            s[0] == '0' && s[1] != 0)
                върни -EINVAL;

        s = mangle_base(s, &base);

        errno = 0;
        l = strtoull(s, &x, SAFE_ATO_MASK_FLAGS(base));
        ако (errno > 0)
                върни -errno;
        ако (!x || x == s || *x != 0)
                върни -EINVAL;
        ако (l != 0 && s[0] == '-')
                върни -ERANGE;

        ако (ret_llu)
                *ret_llu = l;

        върни 0;
}

цяло safe_atolli(const char *s, long long *ret_lli) {
        unsigned base = 0;
        char *x = NULL;
        long long l;

        assert(s);

        s += strspn(s, WHITESPACE);
        s = mangle_base(s, &base);

        errno = 0;
        l = strtoll(s, &x, base);
        ако (errno > 0)
                върни -errno;
        ако (!x || x == s || *x != 0)
                върни -EINVAL;

        ако (ret_lli)
                *ret_lli = l;

        върни 0;
}

цяло safe_atou8_full(const char *s, unsigned base, uint8_t *ret) {
        unsigned u;
        цяло r;

        assert(ret);

        r = safe_atou_full(s, base, &u);
        ако (r < 0)
                върни r;
        ако (u > UINT8_MAX)
                върни -ERANGE;

        *ret = (uint8_t) u;
        върни 0;
}

цяло safe_atou16_full(const char *s, unsigned base, uint16_t *ret) {
        unsigned u;
        цяло r;

        assert(ret);

        r = safe_atou_full(s, base, &u);
        ако (r < 0)
                върни r;
        ако (u > UINT16_MAX)
                върни -ERANGE;

        *ret = (uint16_t) u;
        върни 0;
}

цяло safe_atoi16(const char *s, int16_t *ret) {
        unsigned base = 0;
        char *x = NULL;
        long l;

        assert(s);

        s += strspn(s, WHITESPACE);
        s = mangle_base(s, &base);

        errno = 0;
        l = strtol(s, &x, base);
        ако (errno > 0)
                върни -errno;
        ако (!x || x == s || *x != 0)
                върни -EINVAL;
        ако ((long) (int16_t) l != l)
                върни -ERANGE;

        ако (ret)
                *ret = (int16_t) l;

        върни 0;
}

цяло safe_atod(const char *s, double *ret_d) {
        _cleanup_(freelocalep) locale_t loc = (locale_t) 0;
        char *x = NULL;
        double d = 0;

        assert(s);

        loc = newlocale(LC_NUMERIC_MASK, "C", (locale_t) 0);
        ако (loc == (locale_t) 0)
                върни -errno;

        errno = 0;
        d = strtod_l(s, &x, loc);
        ако (errno > 0)
                върни -errno;
        ако (!x || x == s || *x != 0)
                върни -EINVAL;

        ако (ret_d)
                *ret_d = d;

        върни 0;
}

цяло parse_fractional_part_u(const char **p, size_t digits, unsigned *res) {
        unsigned val = 0;
        const char *s;

        assert(p);
        assert(res);

        s = *p;

        /* accept any number of digits, strtoull is limited to 19 */
        за (size_t i = 0; i < digits; i++, s++) {
                ако (!ascii_isdigit(*s)) {
                        ако (i == 0)
                                върни -EINVAL;

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

        върни 0;
}

цяло parse_nice(const char *s, цяло *ret) {
        цяло n, r;

        assert(ret);

        r = safe_atoi(s, &n);
        ако (r < 0)
                върни r;

        ако (!nice_is_valid(n))
                върни -ERANGE;

        *ret = n;
        върни 0;
}

цяло parse_ip_port(const char *s, uint16_t *ret) {
        uint16_t l;
        цяло r;

        assert(ret);

        r = safe_atou16_full(s, SAFE_ATO_REFUSE_LEADING_WHITESPACE, &l);
        ако (r < 0)
                върни r;

        ако (l == 0)
                върни -EINVAL;

        *ret = l;

        върни 0;
}

цяло parse_ip_port_range(const char *s, uint16_t *low, uint16_t *high, bool allow_zero) {
        unsigned l, h;
        цяло r;

        assert(low);
        assert(high);

        r = parse_range(s, &l, &h);
        ако (r < 0)
                върни r;

        ако (l > 65535 || h > 65535)
                върни -EINVAL;

        ако (!allow_zero && (l == 0 || h == 0))
                върни -EINVAL;

        ако (h < l)
                върни -EINVAL;

        *low = l;
        *high = h;

        върни 0;
}

цяло parse_oom_score_adjust(const char *s, цяло *ret) {
        цяло r, v;

        assert(s);
        assert(ret);

        r = safe_atoi(s, &v);
        ако (r < 0)
                върни r;

        ако (!oom_score_adjust_is_valid(v))
                върни -ERANGE;

        *ret = v;
        върни 0;
}

цяло store_loadavg_fixed_point(unsigned long i, unsigned long f, loadavg_t *ret) {
        assert(ret);

        /* Validate both parts *before* shifting them, so that the shifts themselves cannot overflow.
         * Rejecting f >= 100 is equivalent to the old check of the shifted value against
         * LOADAVG_FIXED_POINT_1_0, since DIV_ROUND_UP(99 << 11, 100) == 2028 and
         * DIV_ROUND_UP(100 << 11, 100) == 2048. */
        ако (i > (~0UL >> LOADAVG_PRECISION_BITS))
                върни -ERANGE;

        ако (f >= 100)
                върни -ERANGE;

        *ret = (i << LOADAVG_PRECISION_BITS) | DIV_ROUND_UP(f << LOADAVG_PRECISION_BITS, 100);
        върни 0;
}

цяло parse_loadavg_fixed_point(const char *s, loadavg_t *ret) {
        const char *d, *f_str, *i_str;
        unsigned long i, f;
        цяло r;

        assert(s);
        assert(ret);

        d = strchr(s, '.');
        ако (!d)
                върни -EINVAL;

        i_str = strndupa_safe(s, d - s);
        f_str = d + 1;

        r = safe_atolu_full(i_str, 10, &i);
        ако (r < 0)
                върни r;

        r = safe_atolu_full(f_str, 10, &f);
        ако (r < 0)
                върни r;

        върни store_loadavg_fixed_point(i, f, ret);
}

/* Limitations are described in https://www.netfilter.org/projects/nftables/manpage.html and
 * https://bugzilla.netfilter.org/show_bug.cgi?id=1175 */
bool nft_identifier_valid(const char *id) {
        ако (isempty(id))
                върни false;

        ако (strlen(id) >= NFT_NAME_MAXLEN)
                върни false;

        ако (!ascii_isalpha(id[0]))
                върни false;

        върни in_charset(id + 1, ALPHANUMERICAL "/\\_.");
}

цяло parse_capability_set(const char *s, uint64_t initial, uint64_t *current) {
        цяло r;

        assert(s);
        assert(current);

        ако (isempty(s)) {
                *current = CAP_MASK_UNSET;
                върни 1;
        }

        bool invert = false;
        ако (s[0] == '~') {
                invert = true;
                s++;
        }

        uint64_t parsed;
        r = capability_set_from_string(s, &parsed);
        ако (r < 0)
                върни r;

        ако (parsed == 0 || *current == initial)
                /* "~" or uninitialized data -> replace */
                *current = invert ? all_capabilities() & ~parsed : parsed;
        иначе {
                /* previous data -> merge */
                ако (invert)
                        *current &= ~parsed;
                иначе
                        *current |= parsed;
        }

        върни r;
}
