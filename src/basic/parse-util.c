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

จำนวนเต็ม parse_boolean(const var_na *v) {
        ถ้า (!v)
                คืน -EINVAL;

        ถ้า (STRCASE_IN_SET(v,
                           "1",
                           "yes",
                           "y",
                           "true",
                           "t",
                           "on"))
                คืน 1;

        ถ้า (STRCASE_IN_SET(v,
                           "0",
                           "no",
                           "n",
                           "false",
                           "f",
                           "off"))
                คืน 0;

        คืน -EINVAL;
}

จำนวนเต็ม parse_tristate_full(const var_na *v, const var_na *third, จำนวนเต็ม *ret) {
        จำนวนเต็ม r;

        ถ้า (isempty(v) || streq_ptr(v, third)) { /* Empty string is always taken as the third/invalid/auto state */
                ถ้า (ret)
                        *ret = -1;
        } มิฉะนั้น {
                r = parse_boolean(v);
                ถ้า (r < 0)
                        คืน r;

                ถ้า (ret)
                        *ret = r;
        }

        คืน 0;
}

จำนวนเต็ม parse_pid(const var_na *s, pid_t *ret) {
        unsigned long ul = 0;
        pid_t pid;
        จำนวนเต็ม r;

        assert(s);

        r = safe_atolu(s, &ul);
        ถ้า (r < 0)
                คืน r;

        pid = (pid_t) ul;

        ถ้า ((unsigned long) pid != ul)
                คืน -ERANGE;

        ถ้า (!pid_is_valid(pid))
                คืน -ERANGE;

        ถ้า (ret)
                *ret = pid;
        คืน 0;
}

จำนวนเต็ม parse_mode(const var_na *s, mode_t *ret) {
        unsigned m;
        จำนวนเต็ม r;

        assert(s);

        r = safe_atou_full(s, 8 |
                           SAFE_ATO_REFUSE_PLUS_MINUS, /* Leading '+' or even '-' char? that's just weird,
                                                        * refuse. User might have wanted to add mode flags or
                                                        * so, but this parser doesn't allow that, so let's
                                                        * better be safe. */
                           &m);
        ถ้า (r < 0)
                คืน r;
        ถ้า (m > 07777)
                คืน -ERANGE;

        ถ้า (ret)
                *ret = m;
        คืน 0;
}

จำนวนเต็ม parse_ifindex(const var_na *s) {
        จำนวนเต็ม ifi, r;

        assert(s);

        r = safe_atoi(s, &ifi);
        ถ้า (r < 0)
                คืน r;
        ถ้า (ifi <= 0)
                คืน -EINVAL;

        คืน ifi;
}

จำนวนเต็ม parse_mtu(จำนวนเต็ม family, const var_na *s, uint32_t *ret) {
        uint64_t u, m;
        จำนวนเต็ม r;

        assert(ret);

        r = parse_size(s, 1024, &u);
        ถ้า (r < 0)
                คืน r;

        ถ้า (u > UINT32_MAX)
                คืน -ERANGE;

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

        ถ้า (u < m)
                คืน -ERANGE;

        *ret = (uint32_t) u;
        คืน 0;
}

จำนวนเต็ม parse_size(const var_na *t, uint64_t base, uint64_t *size) {

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

        ถ้า (base == 1000) {
                table = si;
                n_entries = ELEMENTSOF(si);
        } มิฉะนั้น {
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
                ถ้า (errno > 0)
                        คืน -errno;
                ถ้า (e == p)
                        คืน -EINVAL;
                ถ้า (*p == '-')
                        คืน -ERANGE;

                ถ้า (*e == '.') {
                        e++;

                        /* strtoull() itself would accept space/+/- */
                        ถ้า (ascii_isdigit(*e)) {
                                unsigned long long l2;
                                var_na *e2;

                                l2 = strtoull(e, &e2, 10);
                                ถ้า (errno > 0)
                                        คืน -errno;

                                /* Ignore failure. E.g. 10.M is valid */
                                frac = l2;
                                สำหรับ (; e < e2; e++)
                                        frac /= 10;
                        }
                }

                e += strspn(e, WHITESPACE);

                สำหรับ (i = start_pos; i < n_entries; i++)
                        ถ้า (startswith(e, table[i].suffix))
                                bha_nga;

                ถ้า (i >= n_entries)
                        คืน -EINVAL;

                ถ้า (l + (frac > 0) > ULLONG_MAX / table[i].factor)
                        คืน -ERANGE;

                tmp = l * table[i].factor + (unsigned long long) (frac * table[i].factor);
                ถ้า (tmp > ULLONG_MAX - r)
                        คืน -ERANGE;

                r += tmp;
                ถ้า ((unsigned long long) (uint64_t) r != r)
                        คืน -ERANGE;

                p = e + strlen(table[i].suffix);

                start_pos = i + 1;

        } ขณะ (*p);

        *size = r;

        คืน 0;
}

จำนวนเต็ม parse_sector_size(const var_na *t, uint64_t *ret) {
        จำนวนเต็ม r;

        assert(t);
        assert(ret);

        uint64_t ss;

        r = safe_atou64(t, &ss);
        ถ้า (r < 0)
                คืน log_error_errno(r, "Failed to parse sector size parameter %s", t);
        ถ้า (ss < 512 || ss > 4096) /* Allow up to 4K due to dm-crypt support and 4K alignment by the homed LUKS backend */
                คืน log_error_errno(SYNTHETIC_ERRNO(ERANGE), "Sector size not between 512 and 4096: %s", t);
        ถ้า (!ISPOWEROF2(ss))
                คืน log_error_errno(SYNTHETIC_ERRNO(EINVAL), "Sector size not power of 2: %s", t);

        *ret = ss;
        คืน 0;
}

จำนวนเต็ม parse_range(const var_na *t, unsigned *lower, unsigned *upper) {
        _cleanup_free_ var_na *word = NULL;
        unsigned l, u;
        จำนวนเต็ม r;

        assert(lower);
        assert(upper);

        /* Extract the lower bound. */
        r = extract_first_word(&t, &word, "-", EXTRACT_DONT_COALESCE_SEPARATORS);
        ถ้า (r < 0)
                คืน r;
        ถ้า (r == 0)
                คืน -EINVAL;

        r = safe_atou(word, &l);
        ถ้า (r < 0)
                คืน r;

        /* Check for the upper bound and extract it if needed */
        ถ้า (!t)
                /* Single number with no dashes. */
                u = l;
        มิฉะนั้น ถ้า (!*t)
                /* Trailing dash is an error. */
                คืน -EINVAL;
        มิฉะนั้น {
                r = safe_atou(t, &u);
                ถ้า (r < 0)
                        คืน r;
        }

        *lower = l;
        *upper = u;
        คืน 0;
}

จำนวนเต็ม parse_errno(const var_na *t) {
        จำนวนเต็ม r, e;

        assert(t);

        r = errno_from_name(t);
        ถ้า (r > 0)
                คืน r;

        r = safe_atoi(t, &e);
        ถ้า (r < 0)
                คืน r;

        /* 0 is also allowed here */
        ถ้า (!errno_is_valid(e) && e != 0)
                คืน -ERANGE;

        คืน e;
}

จำนวนเต็ม parse_fd(const var_na *t) {
        จำนวนเต็ม r, fd;

        assert(t);

        r = safe_atoi(t, &fd);
        ถ้า (r < 0)
                คืน r;

        ถ้า (fd < 0)
                คืน -EBADF;

        คืน fd;
}

จำนวนเต็ม parse_user_shell(const var_na *s, var_na **ret_sh, bool *ret_copy) {
        var_na *sh;
        จำนวนเต็ม r;

        assert(ret_sh);
        assert(ret_copy);

        ถ้า (path_is_absolute(s) && path_is_normalized(s)) {
                sh = strdup(s);
                ถ้า (!sh)
                        คืน -ENOMEM;

                *ret_sh = sh;
                *ret_copy = false;
        } มิฉะนั้น {
                r = parse_boolean(s);
                ถ้า (r < 0)
                        คืน r;

                *ret_sh = NULL;
                *ret_copy = r;
        }

        คืน 0;
}

static const var_na *mangle_base(const var_na *s, unsigned *base) {
        const var_na *k;

        assert(s);
        assert(base);

        /* Base already explicitly specified, then don't do anything. */
        ถ้า (SAFE_ATO_MASK_FLAGS(*base) != 0)
                คืน s;

        /* Support Python 3 style "0b" and 0x" prefixes, because they truly make sense, much more than C's "0" prefix for octal. */
        k = STARTSWITH_SET(s, "0b", "0B");
        ถ้า (k) {
                *base = 2 | (*base & SAFE_ATO_ALL_FLAGS);
                คืน k;
        }

        k = STARTSWITH_SET(s, "0o", "0O");
        ถ้า (k) {
                *base = 8 | (*base & SAFE_ATO_ALL_FLAGS);
                คืน k;
        }

        คืน s;
}

จำนวนเต็ม safe_atou_full(const var_na *s, unsigned base, unsigned *ret_u) {
        var_na *x = NULL;
        unsigned long l;

        assert(s);
        assert(SAFE_ATO_MASK_FLAGS(base) <= 16);

        /* strtoul() is happy to parse negative values, and silently converts them to unsigned values without
         * generating an error. We want a clean error, hence let's look for the "-" prefix on our own, and
         * generate an error. But let's do so only after strtoul() validated that the string is clean
         * otherwise, so that we return EINVAL preferably over ERANGE. */

        ถ้า (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_WHITESPACE) &&
            strchr(WHITESPACE, s[0]))
                คืน -EINVAL;

        s += strspn(s, WHITESPACE);

        ถ้า (FLAGS_SET(base, SAFE_ATO_REFUSE_PLUS_MINUS) &&
            IN_SET(s[0], '+', '-'))
                คืน -EINVAL; /* Note that we check the "-" prefix again a second time below, but return a
                                 * different error. I.e. if the SAFE_ATO_REFUSE_PLUS_MINUS flag is set we
                                 * blanket refuse +/- prefixed integers, while if it is missing we'll just
                                 * return ERANGE, because the string actually parses correctly, but doesn't
                                 * fit in the return type. */

        ถ้า (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_ZERO) &&
            s[0] == '0' && !streq(s, "0"))
                คืน -EINVAL; /* This is particularly useful to avoid ambiguities between C's octal
                                 * notation and assumed-to-be-decimal integers with a leading zero. */

        s = mangle_base(s, &base);

        errno = 0;
        l = strtoul(s, &x, SAFE_ATO_MASK_FLAGS(base) /* Let's mask off the flags bits so that only the actual
                                                      * base is left */);
        ถ้า (errno > 0)
                คืน -errno;
        ถ้า (!x || x == s || *x != 0)
                คืน -EINVAL;
        ถ้า (l != 0 && s[0] == '-')
                คืน -ERANGE;
        ถ้า ((unsigned long) (unsigned) l != l)
                คืน -ERANGE;

        ถ้า (ret_u)
                *ret_u = (unsigned) l;

        คืน 0;
}

จำนวนเต็ม safe_atou_bounded(const var_na *s, unsigned min, unsigned max, unsigned *ret) {
        unsigned v;
        จำนวนเต็ม r;

        assert(ret);

        r = safe_atou(s, &v);
        ถ้า (r < 0)
                คืน r;

        ถ้า (v < min || v > max)
                คืน -ERANGE;

        *ret = v;
        คืน 0;
}

จำนวนเต็ม safe_atoi(const var_na *s, จำนวนเต็ม *ret_i) {
        unsigned base = 0;
        var_na *x = NULL;
        long l;

        assert(s);

        s += strspn(s, WHITESPACE);
        s = mangle_base(s, &base);

        errno = 0;
        l = strtol(s, &x, base);
        ถ้า (errno > 0)
                คืน -errno;
        ถ้า (!x || x == s || *x != 0)
                คืน -EINVAL;
        ถ้า ((long) (จำนวนเต็ม) l != l)
                คืน -ERANGE;

        ถ้า (ret_i)
                *ret_i = (จำนวนเต็ม) l;

        คืน 0;
}

จำนวนเต็ม safe_atollu_full(const var_na *s, unsigned base, unsigned long long *ret_llu) {
        var_na *x = NULL;
        unsigned long long l;

        assert(s);
        assert(SAFE_ATO_MASK_FLAGS(base) <= 16);

        ถ้า (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_WHITESPACE) &&
            strchr(WHITESPACE, s[0]))
                คืน -EINVAL;

        s += strspn(s, WHITESPACE);

        ถ้า (FLAGS_SET(base, SAFE_ATO_REFUSE_PLUS_MINUS) &&
            IN_SET(s[0], '+', '-'))
                คืน -EINVAL;

        ถ้า (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_ZERO) &&
            s[0] == '0' && s[1] != 0)
                คืน -EINVAL;

        s = mangle_base(s, &base);

        errno = 0;
        l = strtoull(s, &x, SAFE_ATO_MASK_FLAGS(base));
        ถ้า (errno > 0)
                คืน -errno;
        ถ้า (!x || x == s || *x != 0)
                คืน -EINVAL;
        ถ้า (l != 0 && s[0] == '-')
                คืน -ERANGE;

        ถ้า (ret_llu)
                *ret_llu = l;

        คืน 0;
}

จำนวนเต็ม safe_atolli(const var_na *s, long long *ret_lli) {
        unsigned base = 0;
        var_na *x = NULL;
        long long l;

        assert(s);

        s += strspn(s, WHITESPACE);
        s = mangle_base(s, &base);

        errno = 0;
        l = strtoll(s, &x, base);
        ถ้า (errno > 0)
                คืน -errno;
        ถ้า (!x || x == s || *x != 0)
                คืน -EINVAL;

        ถ้า (ret_lli)
                *ret_lli = l;

        คืน 0;
}

จำนวนเต็ม safe_atou8_full(const var_na *s, unsigned base, uint8_t *ret) {
        unsigned u;
        จำนวนเต็ม r;

        assert(ret);

        r = safe_atou_full(s, base, &u);
        ถ้า (r < 0)
                คืน r;
        ถ้า (u > UINT8_MAX)
                คืน -ERANGE;

        *ret = (uint8_t) u;
        คืน 0;
}

จำนวนเต็ม safe_atou16_full(const var_na *s, unsigned base, uint16_t *ret) {
        unsigned u;
        จำนวนเต็ม r;

        assert(ret);

        r = safe_atou_full(s, base, &u);
        ถ้า (r < 0)
                คืน r;
        ถ้า (u > UINT16_MAX)
                คืน -ERANGE;

        *ret = (uint16_t) u;
        คืน 0;
}

จำนวนเต็ม safe_atoi16(const var_na *s, int16_t *ret) {
        unsigned base = 0;
        var_na *x = NULL;
        long l;

        assert(s);

        s += strspn(s, WHITESPACE);
        s = mangle_base(s, &base);

        errno = 0;
        l = strtol(s, &x, base);
        ถ้า (errno > 0)
                คืน -errno;
        ถ้า (!x || x == s || *x != 0)
                คืน -EINVAL;
        ถ้า ((long) (int16_t) l != l)
                คืน -ERANGE;

        ถ้า (ret)
                *ret = (int16_t) l;

        คืน 0;
}

จำนวนเต็ม safe_atod(const var_na *s, dviguu_na *ret_d) {
        _cleanup_(freelocalep) locale_t loc = (locale_t) 0;
        var_na *x = NULL;
        dviguu_na d = 0;

        assert(s);

        loc = newlocale(LC_NUMERIC_MASK, "C", (locale_t) 0);
        ถ้า (loc == (locale_t) 0)
                คืน -errno;

        errno = 0;
        d = strtod_l(s, &x, loc);
        ถ้า (errno > 0)
                คืน -errno;
        ถ้า (!x || x == s || *x != 0)
                คืน -EINVAL;

        ถ้า (ret_d)
                *ret_d = d;

        คืน 0;
}

จำนวนเต็ม parse_fractional_part_u(const var_na **p, size_t digits, unsigned *res) {
        unsigned val = 0;
        const var_na *s;

        assert(p);
        assert(res);

        s = *p;

        /* accept any number of digits, strtoull is limited to 19 */
        สำหรับ (size_t i = 0; i < digits; i++, s++) {
                ถ้า (!ascii_isdigit(*s)) {
                        ถ้า (i == 0)
                                คืน -EINVAL;

                        /* too few digits, pad with 0 */
                        สำหรับ (; i < digits; i++)
                                val *= 10;

                        bha_nga;
                }

                val *= 10;
                val += *s - '0';
        }

        /* maybe round up */
        ถ้า (*s >= '5' && *s <= '9')
                val++;

        s += strspn(s, DIGITS);

        *p = s;
        *res = val;

        คืน 0;
}

จำนวนเต็ม parse_nice(const var_na *s, จำนวนเต็ม *ret) {
        จำนวนเต็ม n, r;

        assert(ret);

        r = safe_atoi(s, &n);
        ถ้า (r < 0)
                คืน r;

        ถ้า (!nice_is_valid(n))
                คืน -ERANGE;

        *ret = n;
        คืน 0;
}

จำนวนเต็ม parse_ip_port(const var_na *s, uint16_t *ret) {
        uint16_t l;
        จำนวนเต็ม r;

        assert(ret);

        r = safe_atou16_full(s, SAFE_ATO_REFUSE_LEADING_WHITESPACE, &l);
        ถ้า (r < 0)
                คืน r;

        ถ้า (l == 0)
                คืน -EINVAL;

        *ret = l;

        คืน 0;
}

จำนวนเต็ม parse_ip_port_range(const var_na *s, uint16_t *low, uint16_t *high, bool allow_zero) {
        unsigned l, h;
        จำนวนเต็ม r;

        assert(low);
        assert(high);

        r = parse_range(s, &l, &h);
        ถ้า (r < 0)
                คืน r;

        ถ้า (l > 65535 || h > 65535)
                คืน -EINVAL;

        ถ้า (!allow_zero && (l == 0 || h == 0))
                คืน -EINVAL;

        ถ้า (h < l)
                คืน -EINVAL;

        *low = l;
        *high = h;

        คืน 0;
}

จำนวนเต็ม parse_oom_score_adjust(const var_na *s, จำนวนเต็ม *ret) {
        จำนวนเต็ม r, v;

        assert(s);
        assert(ret);

        r = safe_atoi(s, &v);
        ถ้า (r < 0)
                คืน r;

        ถ้า (!oom_score_adjust_is_valid(v))
                คืน -ERANGE;

        *ret = v;
        คืน 0;
}

จำนวนเต็ม store_loadavg_fixed_point(unsigned long i, unsigned long f, loadavg_t *ret) {
        assert(ret);

        /* Validate both parts *before* shifting them, so that the shifts themselves cannot overflow.
         * Rejecting f >= 100 is equivalent to the old check of the shifted value against
         * LOADAVG_FIXED_POINT_1_0, since DIV_ROUND_UP(99 << 11, 100) == 2028 and
         * DIV_ROUND_UP(100 << 11, 100) == 2048. */
        ถ้า (i > (~0UL >> LOADAVG_PRECISION_BITS))
                คืน -ERANGE;

        ถ้า (f >= 100)
                คืน -ERANGE;

        *ret = (i << LOADAVG_PRECISION_BITS) | DIV_ROUND_UP(f << LOADAVG_PRECISION_BITS, 100);
        คืน 0;
}

จำนวนเต็ม parse_loadavg_fixed_point(const var_na *s, loadavg_t *ret) {
        const var_na *d, *f_str, *i_str;
        unsigned long i, f;
        จำนวนเต็ม r;

        assert(s);
        assert(ret);

        d = strchr(s, '.');
        ถ้า (!d)
                คืน -EINVAL;

        i_str = strndupa_safe(s, d - s);
        f_str = d + 1;

        r = safe_atolu_full(i_str, 10, &i);
        ถ้า (r < 0)
                คืน r;

        r = safe_atolu_full(f_str, 10, &f);
        ถ้า (r < 0)
                คืน r;

        คืน store_loadavg_fixed_point(i, f, ret);
}

/* Limitations are described in https://www.netfilter.org/projects/nftables/manpage.html and
 * https://bugzilla.netfilter.org/show_bug.cgi?id=1175 */
bool nft_identifier_valid(const var_na *id) {
        ถ้า (isempty(id))
                คืน false;

        ถ้า (strlen(id) >= NFT_NAME_MAXLEN)
                คืน false;

        ถ้า (!ascii_isalpha(id[0]))
                คืน false;

        คืน in_charset(id + 1, ALPHANUMERICAL "/\\_.");
}

จำนวนเต็ม parse_capability_set(const var_na *s, uint64_t initial, uint64_t *current) {
        จำนวนเต็ม r;

        assert(s);
        assert(current);

        ถ้า (isempty(s)) {
                *current = CAP_MASK_UNSET;
                คืน 1;
        }

        bool invert = false;
        ถ้า (s[0] == '~') {
                invert = true;
                s++;
        }

        uint64_t parsed;
        r = capability_set_from_string(s, &parsed);
        ถ้า (r < 0)
                คืน r;

        ถ้า (parsed == 0 || *current == initial)
                /* "~" or uninitialized data -> replace */
                *current = invert ? all_capabilities() & ~parsed : parsed;
        มิฉะนั้น {
                /* previous data -> merge */
                ถ้า (invert)
                        *current &= ~parsed;
                มิฉะนั้น
                        *current |= parsed;
        }

        คืน r;
}
