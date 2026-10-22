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

số_nguyên parse_boolean(const char *v) {
        nếu (!v)
                trả_về -EINVAL;

        nếu (STRCASE_IN_SET(v,
                           "1",
                           "yes",
                           "y",
                           "true",
                           "t",
                           "on"))
                trả_về 1;

        nếu (STRCASE_IN_SET(v,
                           "0",
                           "no",
                           "n",
                           "false",
                           "f",
                           "off"))
                trả_về 0;

        trả_về -EINVAL;
}

số_nguyên parse_tristate_full(const char *v, const char *third, số_nguyên *ret) {
        số_nguyên r;

        nếu (isempty(v) || streq_ptr(v, third)) { /* Empty string is always taken as the third/invalid/auto state */
                nếu (ret)
                        *ret = -1;
        } không_thì {
                r = parse_boolean(v);
                nếu (r < 0)
                        trả_về r;

                nếu (ret)
                        *ret = r;
        }

        trả_về 0;
}

số_nguyên parse_pid(const char *s, pid_t *ret) {
        unsigned long ul = 0;
        pid_t pid;
        số_nguyên r;

        assert(s);

        r = safe_atolu(s, &ul);
        nếu (r < 0)
                trả_về r;

        pid = (pid_t) ul;

        nếu ((unsigned long) pid != ul)
                trả_về -ERANGE;

        nếu (!pid_is_valid(pid))
                trả_về -ERANGE;

        nếu (ret)
                *ret = pid;
        trả_về 0;
}

số_nguyên parse_mode(const char *s, mode_t *ret) {
        unsigned m;
        số_nguyên r;

        assert(s);

        r = safe_atou_full(s, 8 |
                           SAFE_ATO_REFUSE_PLUS_MINUS, /* Leading '+' or even '-' char? that's just weird,
                                                        * refuse. User might have wanted to add mode flags or
                                                        * so, but this parser doesn't allow that, so let's
                                                        * better be safe. */
                           &m);
        nếu (r < 0)
                trả_về r;
        nếu (m > 07777)
                trả_về -ERANGE;

        nếu (ret)
                *ret = m;
        trả_về 0;
}

số_nguyên parse_ifindex(const char *s) {
        số_nguyên ifi, r;

        assert(s);

        r = safe_atoi(s, &ifi);
        nếu (r < 0)
                trả_về r;
        nếu (ifi <= 0)
                trả_về -EINVAL;

        trả_về ifi;
}

số_nguyên parse_mtu(số_nguyên family, const char *s, uint32_t *ret) {
        uint64_t u, m;
        số_nguyên r;

        assert(ret);

        r = parse_size(s, 1024, &u);
        nếu (r < 0)
                trả_về r;

        nếu (u > UINT32_MAX)
                trả_về -ERANGE;

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

        nếu (u < m)
                trả_về -ERANGE;

        *ret = (uint32_t) u;
        trả_về 0;
}

số_nguyên parse_size(const char *t, uint64_t base, uint64_t *size) {

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

        nếu (base == 1000) {
                table = si;
                n_entries = ELEMENTSOF(si);
        } không_thì {
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
                nếu (errno > 0)
                        trả_về -errno;
                nếu (e == p)
                        trả_về -EINVAL;
                nếu (*p == '-')
                        trả_về -ERANGE;

                nếu (*e == '.') {
                        e++;

                        /* strtoull() itself would accept space/+/- */
                        nếu (ascii_isdigit(*e)) {
                                unsigned long long l2;
                                char *e2;

                                l2 = strtoull(e, &e2, 10);
                                nếu (errno > 0)
                                        trả_về -errno;

                                /* Ignore failure. E.g. 10.M is valid */
                                frac = l2;
                                cho (; e < e2; e++)
                                        frac /= 10;
                        }
                }

                e += strspn(e, WHITESPACE);

                cho (i = start_pos; i < n_entries; i++)
                        nếu (startswith(e, table[i].suffix))
                                break;

                nếu (i >= n_entries)
                        trả_về -EINVAL;

                nếu (l + (frac > 0) > ULLONG_MAX / table[i].factor)
                        trả_về -ERANGE;

                tmp = l * table[i].factor + (unsigned long long) (frac * table[i].factor);
                nếu (tmp > ULLONG_MAX - r)
                        trả_về -ERANGE;

                r += tmp;
                nếu ((unsigned long long) (uint64_t) r != r)
                        trả_về -ERANGE;

                p = e + strlen(table[i].suffix);

                start_pos = i + 1;

        } trong_khi (*p);

        *size = r;

        trả_về 0;
}

số_nguyên parse_sector_size(const char *t, uint64_t *ret) {
        số_nguyên r;

        assert(t);
        assert(ret);

        uint64_t ss;

        r = safe_atou64(t, &ss);
        nếu (r < 0)
                trả_về log_error_errno(r, "Failed to parse sector size parameter %s", t);
        nếu (ss < 512 || ss > 4096) /* Allow up to 4K due to dm-crypt support and 4K alignment by the homed LUKS backend */
                trả_về log_error_errno(SYNTHETIC_ERRNO(ERANGE), "Sector size not between 512 and 4096: %s", t);
        nếu (!ISPOWEROF2(ss))
                trả_về log_error_errno(SYNTHETIC_ERRNO(EINVAL), "Sector size not power of 2: %s", t);

        *ret = ss;
        trả_về 0;
}

số_nguyên parse_range(const char *t, unsigned *lower, unsigned *upper) {
        _cleanup_free_ char *word = NULL;
        unsigned l, u;
        số_nguyên r;

        assert(lower);
        assert(upper);

        /* Extract the lower bound. */
        r = extract_first_word(&t, &word, "-", EXTRACT_DONT_COALESCE_SEPARATORS);
        nếu (r < 0)
                trả_về r;
        nếu (r == 0)
                trả_về -EINVAL;

        r = safe_atou(word, &l);
        nếu (r < 0)
                trả_về r;

        /* Check for the upper bound and extract it if needed */
        nếu (!t)
                /* Single number with no dashes. */
                u = l;
        không_thì nếu (!*t)
                /* Trailing dash is an error. */
                trả_về -EINVAL;
        không_thì {
                r = safe_atou(t, &u);
                nếu (r < 0)
                        trả_về r;
        }

        *lower = l;
        *upper = u;
        trả_về 0;
}

số_nguyên parse_errno(const char *t) {
        số_nguyên r, e;

        assert(t);

        r = errno_from_name(t);
        nếu (r > 0)
                trả_về r;

        r = safe_atoi(t, &e);
        nếu (r < 0)
                trả_về r;

        /* 0 is also allowed here */
        nếu (!errno_is_valid(e) && e != 0)
                trả_về -ERANGE;

        trả_về e;
}

số_nguyên parse_fd(const char *t) {
        số_nguyên r, fd;

        assert(t);

        r = safe_atoi(t, &fd);
        nếu (r < 0)
                trả_về r;

        nếu (fd < 0)
                trả_về -EBADF;

        trả_về fd;
}

số_nguyên parse_user_shell(const char *s, char **ret_sh, bool *ret_copy) {
        char *sh;
        số_nguyên r;

        assert(ret_sh);
        assert(ret_copy);

        nếu (path_is_absolute(s) && path_is_normalized(s)) {
                sh = strdup(s);
                nếu (!sh)
                        trả_về -ENOMEM;

                *ret_sh = sh;
                *ret_copy = false;
        } không_thì {
                r = parse_boolean(s);
                nếu (r < 0)
                        trả_về r;

                *ret_sh = NULL;
                *ret_copy = r;
        }

        trả_về 0;
}

static const char *mangle_base(const char *s, unsigned *base) {
        const char *k;

        assert(s);
        assert(base);

        /* Base already explicitly specified, then don't do anything. */
        nếu (SAFE_ATO_MASK_FLAGS(*base) != 0)
                trả_về s;

        /* Support Python 3 style "0b" and 0x" prefixes, because they truly make sense, much more than C's "0" prefix for octal. */
        k = STARTSWITH_SET(s, "0b", "0B");
        nếu (k) {
                *base = 2 | (*base & SAFE_ATO_ALL_FLAGS);
                trả_về k;
        }

        k = STARTSWITH_SET(s, "0o", "0O");
        nếu (k) {
                *base = 8 | (*base & SAFE_ATO_ALL_FLAGS);
                trả_về k;
        }

        trả_về s;
}

số_nguyên safe_atou_full(const char *s, unsigned base, unsigned *ret_u) {
        char *x = NULL;
        unsigned long l;

        assert(s);
        assert(SAFE_ATO_MASK_FLAGS(base) <= 16);

        /* strtoul() is happy to parse negative values, and silently converts them to unsigned values without
         * generating an error. We want a clean error, hence let's look for the "-" prefix on our own, and
         * generate an error. But let's do so only after strtoul() validated that the string is clean
         * otherwise, so that we return EINVAL preferably over ERANGE. */

        nếu (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_WHITESPACE) &&
            strchr(WHITESPACE, s[0]))
                trả_về -EINVAL;

        s += strspn(s, WHITESPACE);

        nếu (FLAGS_SET(base, SAFE_ATO_REFUSE_PLUS_MINUS) &&
            IN_SET(s[0], '+', '-'))
                trả_về -EINVAL; /* Note that we check the "-" prefix again a second time below, but return a
                                 * different error. I.e. if the SAFE_ATO_REFUSE_PLUS_MINUS flag is set we
                                 * blanket refuse +/- prefixed integers, while if it is missing we'll just
                                 * return ERANGE, because the string actually parses correctly, but doesn't
                                 * fit in the return type. */

        nếu (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_ZERO) &&
            s[0] == '0' && !streq(s, "0"))
                trả_về -EINVAL; /* This is particularly useful to avoid ambiguities between C's octal
                                 * notation and assumed-to-be-decimal integers with a leading zero. */

        s = mangle_base(s, &base);

        errno = 0;
        l = strtoul(s, &x, SAFE_ATO_MASK_FLAGS(base) /* Let's mask off the flags bits so that only the actual
                                                      * base is left */);
        nếu (errno > 0)
                trả_về -errno;
        nếu (!x || x == s || *x != 0)
                trả_về -EINVAL;
        nếu (l != 0 && s[0] == '-')
                trả_về -ERANGE;
        nếu ((unsigned long) (unsigned) l != l)
                trả_về -ERANGE;

        nếu (ret_u)
                *ret_u = (unsigned) l;

        trả_về 0;
}

số_nguyên safe_atou_bounded(const char *s, unsigned min, unsigned max, unsigned *ret) {
        unsigned v;
        số_nguyên r;

        assert(ret);

        r = safe_atou(s, &v);
        nếu (r < 0)
                trả_về r;

        nếu (v < min || v > max)
                trả_về -ERANGE;

        *ret = v;
        trả_về 0;
}

số_nguyên safe_atoi(const char *s, số_nguyên *ret_i) {
        unsigned base = 0;
        char *x = NULL;
        long l;

        assert(s);

        s += strspn(s, WHITESPACE);
        s = mangle_base(s, &base);

        errno = 0;
        l = strtol(s, &x, base);
        nếu (errno > 0)
                trả_về -errno;
        nếu (!x || x == s || *x != 0)
                trả_về -EINVAL;
        nếu ((long) (số_nguyên) l != l)
                trả_về -ERANGE;

        nếu (ret_i)
                *ret_i = (số_nguyên) l;

        trả_về 0;
}

số_nguyên safe_atollu_full(const char *s, unsigned base, unsigned long long *ret_llu) {
        char *x = NULL;
        unsigned long long l;

        assert(s);
        assert(SAFE_ATO_MASK_FLAGS(base) <= 16);

        nếu (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_WHITESPACE) &&
            strchr(WHITESPACE, s[0]))
                trả_về -EINVAL;

        s += strspn(s, WHITESPACE);

        nếu (FLAGS_SET(base, SAFE_ATO_REFUSE_PLUS_MINUS) &&
            IN_SET(s[0], '+', '-'))
                trả_về -EINVAL;

        nếu (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_ZERO) &&
            s[0] == '0' && s[1] != 0)
                trả_về -EINVAL;

        s = mangle_base(s, &base);

        errno = 0;
        l = strtoull(s, &x, SAFE_ATO_MASK_FLAGS(base));
        nếu (errno > 0)
                trả_về -errno;
        nếu (!x || x == s || *x != 0)
                trả_về -EINVAL;
        nếu (l != 0 && s[0] == '-')
                trả_về -ERANGE;

        nếu (ret_llu)
                *ret_llu = l;

        trả_về 0;
}

số_nguyên safe_atolli(const char *s, long long *ret_lli) {
        unsigned base = 0;
        char *x = NULL;
        long long l;

        assert(s);

        s += strspn(s, WHITESPACE);
        s = mangle_base(s, &base);

        errno = 0;
        l = strtoll(s, &x, base);
        nếu (errno > 0)
                trả_về -errno;
        nếu (!x || x == s || *x != 0)
                trả_về -EINVAL;

        nếu (ret_lli)
                *ret_lli = l;

        trả_về 0;
}

số_nguyên safe_atou8_full(const char *s, unsigned base, uint8_t *ret) {
        unsigned u;
        số_nguyên r;

        assert(ret);

        r = safe_atou_full(s, base, &u);
        nếu (r < 0)
                trả_về r;
        nếu (u > UINT8_MAX)
                trả_về -ERANGE;

        *ret = (uint8_t) u;
        trả_về 0;
}

số_nguyên safe_atou16_full(const char *s, unsigned base, uint16_t *ret) {
        unsigned u;
        số_nguyên r;

        assert(ret);

        r = safe_atou_full(s, base, &u);
        nếu (r < 0)
                trả_về r;
        nếu (u > UINT16_MAX)
                trả_về -ERANGE;

        *ret = (uint16_t) u;
        trả_về 0;
}

số_nguyên safe_atoi16(const char *s, int16_t *ret) {
        unsigned base = 0;
        char *x = NULL;
        long l;

        assert(s);

        s += strspn(s, WHITESPACE);
        s = mangle_base(s, &base);

        errno = 0;
        l = strtol(s, &x, base);
        nếu (errno > 0)
                trả_về -errno;
        nếu (!x || x == s || *x != 0)
                trả_về -EINVAL;
        nếu ((long) (int16_t) l != l)
                trả_về -ERANGE;

        nếu (ret)
                *ret = (int16_t) l;

        trả_về 0;
}

số_nguyên safe_atod(const char *s, double *ret_d) {
        _cleanup_(freelocalep) locale_t loc = (locale_t) 0;
        char *x = NULL;
        double d = 0;

        assert(s);

        loc = newlocale(LC_NUMERIC_MASK, "C", (locale_t) 0);
        nếu (loc == (locale_t) 0)
                trả_về -errno;

        errno = 0;
        d = strtod_l(s, &x, loc);
        nếu (errno > 0)
                trả_về -errno;
        nếu (!x || x == s || *x != 0)
                trả_về -EINVAL;

        nếu (ret_d)
                *ret_d = d;

        trả_về 0;
}

số_nguyên parse_fractional_part_u(const char **p, size_t digits, unsigned *res) {
        unsigned val = 0;
        const char *s;

        assert(p);
        assert(res);

        s = *p;

        /* accept any number of digits, strtoull is limited to 19 */
        cho (size_t i = 0; i < digits; i++, s++) {
                nếu (!ascii_isdigit(*s)) {
                        nếu (i == 0)
                                trả_về -EINVAL;

                        /* too few digits, pad with 0 */
                        cho (; i < digits; i++)
                                val *= 10;

                        break;
                }

                val *= 10;
                val += *s - '0';
        }

        /* maybe round up */
        nếu (*s >= '5' && *s <= '9')
                val++;

        s += strspn(s, DIGITS);

        *p = s;
        *res = val;

        trả_về 0;
}

số_nguyên parse_nice(const char *s, số_nguyên *ret) {
        số_nguyên n, r;

        assert(ret);

        r = safe_atoi(s, &n);
        nếu (r < 0)
                trả_về r;

        nếu (!nice_is_valid(n))
                trả_về -ERANGE;

        *ret = n;
        trả_về 0;
}

số_nguyên parse_ip_port(const char *s, uint16_t *ret) {
        uint16_t l;
        số_nguyên r;

        assert(ret);

        r = safe_atou16_full(s, SAFE_ATO_REFUSE_LEADING_WHITESPACE, &l);
        nếu (r < 0)
                trả_về r;

        nếu (l == 0)
                trả_về -EINVAL;

        *ret = l;

        trả_về 0;
}

số_nguyên parse_ip_port_range(const char *s, uint16_t *low, uint16_t *high, bool allow_zero) {
        unsigned l, h;
        số_nguyên r;

        assert(low);
        assert(high);

        r = parse_range(s, &l, &h);
        nếu (r < 0)
                trả_về r;

        nếu (l > 65535 || h > 65535)
                trả_về -EINVAL;

        nếu (!allow_zero && (l == 0 || h == 0))
                trả_về -EINVAL;

        nếu (h < l)
                trả_về -EINVAL;

        *low = l;
        *high = h;

        trả_về 0;
}

số_nguyên parse_oom_score_adjust(const char *s, số_nguyên *ret) {
        số_nguyên r, v;

        assert(s);
        assert(ret);

        r = safe_atoi(s, &v);
        nếu (r < 0)
                trả_về r;

        nếu (!oom_score_adjust_is_valid(v))
                trả_về -ERANGE;

        *ret = v;
        trả_về 0;
}

số_nguyên store_loadavg_fixed_point(unsigned long i, unsigned long f, loadavg_t *ret) {
        assert(ret);

        /* Validate both parts *before* shifting them, so that the shifts themselves cannot overflow.
         * Rejecting f >= 100 is equivalent to the old check of the shifted value against
         * LOADAVG_FIXED_POINT_1_0, since DIV_ROUND_UP(99 << 11, 100) == 2028 and
         * DIV_ROUND_UP(100 << 11, 100) == 2048. */
        nếu (i > (~0UL >> LOADAVG_PRECISION_BITS))
                trả_về -ERANGE;

        nếu (f >= 100)
                trả_về -ERANGE;

        *ret = (i << LOADAVG_PRECISION_BITS) | DIV_ROUND_UP(f << LOADAVG_PRECISION_BITS, 100);
        trả_về 0;
}

số_nguyên parse_loadavg_fixed_point(const char *s, loadavg_t *ret) {
        const char *d, *f_str, *i_str;
        unsigned long i, f;
        số_nguyên r;

        assert(s);
        assert(ret);

        d = strchr(s, '.');
        nếu (!d)
                trả_về -EINVAL;

        i_str = strndupa_safe(s, d - s);
        f_str = d + 1;

        r = safe_atolu_full(i_str, 10, &i);
        nếu (r < 0)
                trả_về r;

        r = safe_atolu_full(f_str, 10, &f);
        nếu (r < 0)
                trả_về r;

        trả_về store_loadavg_fixed_point(i, f, ret);
}

/* Limitations are described in https://www.netfilter.org/projects/nftables/manpage.html and
 * https://bugzilla.netfilter.org/show_bug.cgi?id=1175 */
bool nft_identifier_valid(const char *id) {
        nếu (isempty(id))
                trả_về false;

        nếu (strlen(id) >= NFT_NAME_MAXLEN)
                trả_về false;

        nếu (!ascii_isalpha(id[0]))
                trả_về false;

        trả_về in_charset(id + 1, ALPHANUMERICAL "/\\_.");
}

số_nguyên parse_capability_set(const char *s, uint64_t initial, uint64_t *current) {
        số_nguyên r;

        assert(s);
        assert(current);

        nếu (isempty(s)) {
                *current = CAP_MASK_UNSET;
                trả_về 1;
        }

        bool invert = false;
        nếu (s[0] == '~') {
                invert = true;
                s++;
        }

        uint64_t parsed;
        r = capability_set_from_string(s, &parsed);
        nếu (r < 0)
                trả_về r;

        nếu (parsed == 0 || *current == initial)
                /* "~" or uninitialized data -> replace */
                *current = invert ? all_capabilities() & ~parsed : parsed;
        không_thì {
                /* previous data -> merge */
                nếu (invert)
                        *current &= ~parsed;
                không_thì
                        *current |= parsed;
        }

        trả_về r;
}
