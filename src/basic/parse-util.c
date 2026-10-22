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

puur_naa_nka parse_boolean(const var_na *v) {
        ᮚᮦᮔ᮪ (!v)
                ᮝᮜᮤ -EINVAL;

        ᮚᮦᮔ᮪ (STRCASE_IN_SET(v,
                           "1",
                           "yes",
                           "y",
                           "true",
                           "t",
                           "on"))
                ᮝᮜᮤ 1;

        ᮚᮦᮔ᮪ (STRCASE_IN_SET(v,
                           "0",
                           "no",
                           "n",
                           "false",
                           "f",
                           "off"))
                ᮝᮜᮤ 0;

        ᮝᮜᮤ -EINVAL;
}

puur_naa_nka parse_tristate_full(const var_na *v, const var_na *third, puur_naa_nka *ret) {
        puur_naa_nka r;

        ᮚᮦᮔ᮪ (isempty(v) || streq_ptr(v, third)) { /* Empty string is always taken as the third/invalid/auto state */
                ᮚᮦᮔ᮪ (ret)
                        *ret = -1;
        } ᮜᮤᮚ {
                r = parse_boolean(v);
                ᮚᮦᮔ᮪ (r < 0)
                        ᮝᮜᮤ r;

                ᮚᮦᮔ᮪ (ret)
                        *ret = r;
        }

        ᮝᮜᮤ 0;
}

puur_naa_nka parse_pid(const var_na *s, pid_t *ret) {
        unsigned long ul = 0;
        pid_t pid;
        puur_naa_nka r;

        assert(s);

        r = safe_atolu(s, &ul);
        ᮚᮦᮔ᮪ (r < 0)
                ᮝᮜᮤ r;

        pid = (pid_t) ul;

        ᮚᮦᮔ᮪ ((unsigned long) pid != ul)
                ᮝᮜᮤ -ERANGE;

        ᮚᮦᮔ᮪ (!pid_is_valid(pid))
                ᮝᮜᮤ -ERANGE;

        ᮚᮦᮔ᮪ (ret)
                *ret = pid;
        ᮝᮜᮤ 0;
}

puur_naa_nka parse_mode(const var_na *s, mode_t *ret) {
        unsigned m;
        puur_naa_nka r;

        assert(s);

        r = safe_atou_full(s, 8 |
                           SAFE_ATO_REFUSE_PLUS_MINUS, /* Leading '+' or even '-' char? that's just weird,
                                                        * refuse. User might have wanted to add mode flags or
                                                        * so, but this parser doesn't allow that, so let's
                                                        * better be safe. */
                           &m);
        ᮚᮦᮔ᮪ (r < 0)
                ᮝᮜᮤ r;
        ᮚᮦᮔ᮪ (m > 07777)
                ᮝᮜᮤ -ERANGE;

        ᮚᮦᮔ᮪ (ret)
                *ret = m;
        ᮝᮜᮤ 0;
}

puur_naa_nka parse_ifindex(const var_na *s) {
        puur_naa_nka ifi, r;

        assert(s);

        r = safe_atoi(s, &ifi);
        ᮚᮦᮔ᮪ (r < 0)
                ᮝᮜᮤ r;
        ᮚᮦᮔ᮪ (ifi <= 0)
                ᮝᮜᮤ -EINVAL;

        ᮝᮜᮤ ifi;
}

puur_naa_nka parse_mtu(puur_naa_nka family, const var_na *s, uint32_t *ret) {
        uint64_t u, m;
        puur_naa_nka r;

        assert(ret);

        r = parse_size(s, 1024, &u);
        ᮚᮦᮔ᮪ (r < 0)
                ᮝᮜᮤ r;

        ᮚᮦᮔ᮪ (u > UINT32_MAX)
                ᮝᮜᮤ -ERANGE;

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

        ᮚᮦᮔ᮪ (u < m)
                ᮝᮜᮤ -ERANGE;

        *ret = (uint32_t) u;
        ᮝᮜᮤ 0;
}

puur_naa_nka parse_size(const var_na *t, uint64_t base, uint64_t *size) {

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

        ᮚᮦᮔ᮪ (base == 1000) {
                table = si;
                n_entries = ELEMENTSOF(si);
        } ᮜᮤᮚ {
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
                ᮚᮦᮔ᮪ (errno > 0)
                        ᮝᮜᮤ -errno;
                ᮚᮦᮔ᮪ (e == p)
                        ᮝᮜᮤ -EINVAL;
                ᮚᮦᮔ᮪ (*p == '-')
                        ᮝᮜᮤ -ERANGE;

                ᮚᮦᮔ᮪ (*e == '.') {
                        e++;

                        /* strtoull() itself would accept space/+/- */
                        ᮚᮦᮔ᮪ (ascii_isdigit(*e)) {
                                unsigned long long l2;
                                var_na *e2;

                                l2 = strtoull(e, &e2, 10);
                                ᮚᮦᮔ᮪ (errno > 0)
                                        ᮝᮜᮤ -errno;

                                /* Ignore failure. E.g. 10.M is valid */
                                frac = l2;
                                for (; e < e2; e++)
                                        frac /= 10;
                        }
                }

                e += strspn(e, WHITESPACE);

                for (i = start_pos; i < n_entries; i++)
                        ᮚᮦᮔ᮪ (startswith(e, table[i].suffix))
                                bha_nga;

                ᮚᮦᮔ᮪ (i >= n_entries)
                        ᮝᮜᮤ -EINVAL;

                ᮚᮦᮔ᮪ (l + (frac > 0) > ULLONG_MAX / table[i].factor)
                        ᮝᮜᮤ -ERANGE;

                tmp = l * table[i].factor + (unsigned long long) (frac * table[i].factor);
                ᮚᮦᮔ᮪ (tmp > ULLONG_MAX - r)
                        ᮝᮜᮤ -ERANGE;

                r += tmp;
                ᮚᮦᮔ᮪ ((unsigned long long) (uint64_t) r != r)
                        ᮝᮜᮤ -ERANGE;

                p = e + strlen(table[i].suffix);

                start_pos = i + 1;

        } ᮞᮨᮓᮨᮜ (*p);

        *size = r;

        ᮝᮜᮤ 0;
}

puur_naa_nka parse_sector_size(const var_na *t, uint64_t *ret) {
        puur_naa_nka r;

        assert(t);
        assert(ret);

        uint64_t ss;

        r = safe_atou64(t, &ss);
        ᮚᮦᮔ᮪ (r < 0)
                ᮝᮜᮤ log_error_errno(r, "Failed to parse sector size parameter %s", t);
        ᮚᮦᮔ᮪ (ss < 512 || ss > 4096) /* Allow up to 4K due to dm-crypt support and 4K alignment by the homed LUKS backend */
                ᮝᮜᮤ log_error_errno(SYNTHETIC_ERRNO(ERANGE), "Sector size not between 512 and 4096: %s", t);
        ᮚᮦᮔ᮪ (!ISPOWEROF2(ss))
                ᮝᮜᮤ log_error_errno(SYNTHETIC_ERRNO(EINVAL), "Sector size not power of 2: %s", t);

        *ret = ss;
        ᮝᮜᮤ 0;
}

puur_naa_nka parse_range(const var_na *t, unsigned *lower, unsigned *upper) {
        _cleanup_free_ var_na *word = NULL;
        unsigned l, u;
        puur_naa_nka r;

        assert(lower);
        assert(upper);

        /* Extract the lower bound. */
        r = extract_first_word(&t, &word, "-", EXTRACT_DONT_COALESCE_SEPARATORS);
        ᮚᮦᮔ᮪ (r < 0)
                ᮝᮜᮤ r;
        ᮚᮦᮔ᮪ (r == 0)
                ᮝᮜᮤ -EINVAL;

        r = safe_atou(word, &l);
        ᮚᮦᮔ᮪ (r < 0)
                ᮝᮜᮤ r;

        /* Check for the upper bound and extract it if needed */
        ᮚᮦᮔ᮪ (!t)
                /* Single number with no dashes. */
                u = l;
        ᮜᮤᮚ ᮚᮦᮔ᮪ (!*t)
                /* Trailing dash is an error. */
                ᮝᮜᮤ -EINVAL;
        ᮜᮤᮚ {
                r = safe_atou(t, &u);
                ᮚᮦᮔ᮪ (r < 0)
                        ᮝᮜᮤ r;
        }

        *lower = l;
        *upper = u;
        ᮝᮜᮤ 0;
}

puur_naa_nka parse_errno(const var_na *t) {
        puur_naa_nka r, e;

        assert(t);

        r = errno_from_name(t);
        ᮚᮦᮔ᮪ (r > 0)
                ᮝᮜᮤ r;

        r = safe_atoi(t, &e);
        ᮚᮦᮔ᮪ (r < 0)
                ᮝᮜᮤ r;

        /* 0 is also allowed here */
        ᮚᮦᮔ᮪ (!errno_is_valid(e) && e != 0)
                ᮝᮜᮤ -ERANGE;

        ᮝᮜᮤ e;
}

puur_naa_nka parse_fd(const var_na *t) {
        puur_naa_nka r, fd;

        assert(t);

        r = safe_atoi(t, &fd);
        ᮚᮦᮔ᮪ (r < 0)
                ᮝᮜᮤ r;

        ᮚᮦᮔ᮪ (fd < 0)
                ᮝᮜᮤ -EBADF;

        ᮝᮜᮤ fd;
}

puur_naa_nka parse_user_shell(const var_na *s, var_na **ret_sh, bool *ret_copy) {
        var_na *sh;
        puur_naa_nka r;

        assert(ret_sh);
        assert(ret_copy);

        ᮚᮦᮔ᮪ (path_is_absolute(s) && path_is_normalized(s)) {
                sh = strdup(s);
                ᮚᮦᮔ᮪ (!sh)
                        ᮝᮜᮤ -ENOMEM;

                *ret_sh = sh;
                *ret_copy = false;
        } ᮜᮤᮚ {
                r = parse_boolean(s);
                ᮚᮦᮔ᮪ (r < 0)
                        ᮝᮜᮤ r;

                *ret_sh = NULL;
                *ret_copy = r;
        }

        ᮝᮜᮤ 0;
}

static const var_na *mangle_base(const var_na *s, unsigned *base) {
        const var_na *k;

        assert(s);
        assert(base);

        /* Base already explicitly specified, then don't do anything. */
        ᮚᮦᮔ᮪ (SAFE_ATO_MASK_FLAGS(*base) != 0)
                ᮝᮜᮤ s;

        /* Support Python 3 style "0b" and 0x" prefixes, because they truly make sense, much more than C's "0" prefix for octal. */
        k = STARTSWITH_SET(s, "0b", "0B");
        ᮚᮦᮔ᮪ (k) {
                *base = 2 | (*base & SAFE_ATO_ALL_FLAGS);
                ᮝᮜᮤ k;
        }

        k = STARTSWITH_SET(s, "0o", "0O");
        ᮚᮦᮔ᮪ (k) {
                *base = 8 | (*base & SAFE_ATO_ALL_FLAGS);
                ᮝᮜᮤ k;
        }

        ᮝᮜᮤ s;
}

puur_naa_nka safe_atou_full(const var_na *s, unsigned base, unsigned *ret_u) {
        var_na *x = NULL;
        unsigned long l;

        assert(s);
        assert(SAFE_ATO_MASK_FLAGS(base) <= 16);

        /* strtoul() is happy to parse negative values, and silently converts them to unsigned values without
         * generating an error. We want a clean error, hence let's look for the "-" prefix on our own, and
         * generate an error. But let's do so only after strtoul() validated that the string is clean
         * otherwise, so that we return EINVAL preferably over ERANGE. */

        ᮚᮦᮔ᮪ (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_WHITESPACE) &&
            strchr(WHITESPACE, s[0]))
                ᮝᮜᮤ -EINVAL;

        s += strspn(s, WHITESPACE);

        ᮚᮦᮔ᮪ (FLAGS_SET(base, SAFE_ATO_REFUSE_PLUS_MINUS) &&
            IN_SET(s[0], '+', '-'))
                ᮝᮜᮤ -EINVAL; /* Note that we check the "-" prefix again a second time below, but return a
                                 * different error. I.e. if the SAFE_ATO_REFUSE_PLUS_MINUS flag is set we
                                 * blanket refuse +/- prefixed integers, while if it is missing we'll just
                                 * return ERANGE, because the string actually parses correctly, but doesn't
                                 * fit in the return type. */

        ᮚᮦᮔ᮪ (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_ZERO) &&
            s[0] == '0' && !streq(s, "0"))
                ᮝᮜᮤ -EINVAL; /* This is particularly useful to avoid ambiguities between C's octal
                                 * notation and assumed-to-be-decimal integers with a leading zero. */

        s = mangle_base(s, &base);

        errno = 0;
        l = strtoul(s, &x, SAFE_ATO_MASK_FLAGS(base) /* Let's mask off the flags bits so that only the actual
                                                      * base is left */);
        ᮚᮦᮔ᮪ (errno > 0)
                ᮝᮜᮤ -errno;
        ᮚᮦᮔ᮪ (!x || x == s || *x != 0)
                ᮝᮜᮤ -EINVAL;
        ᮚᮦᮔ᮪ (l != 0 && s[0] == '-')
                ᮝᮜᮤ -ERANGE;
        ᮚᮦᮔ᮪ ((unsigned long) (unsigned) l != l)
                ᮝᮜᮤ -ERANGE;

        ᮚᮦᮔ᮪ (ret_u)
                *ret_u = (unsigned) l;

        ᮝᮜᮤ 0;
}

puur_naa_nka safe_atou_bounded(const var_na *s, unsigned min, unsigned max, unsigned *ret) {
        unsigned v;
        puur_naa_nka r;

        assert(ret);

        r = safe_atou(s, &v);
        ᮚᮦᮔ᮪ (r < 0)
                ᮝᮜᮤ r;

        ᮚᮦᮔ᮪ (v < min || v > max)
                ᮝᮜᮤ -ERANGE;

        *ret = v;
        ᮝᮜᮤ 0;
}

puur_naa_nka safe_atoi(const var_na *s, puur_naa_nka *ret_i) {
        unsigned base = 0;
        var_na *x = NULL;
        long l;

        assert(s);

        s += strspn(s, WHITESPACE);
        s = mangle_base(s, &base);

        errno = 0;
        l = strtol(s, &x, base);
        ᮚᮦᮔ᮪ (errno > 0)
                ᮝᮜᮤ -errno;
        ᮚᮦᮔ᮪ (!x || x == s || *x != 0)
                ᮝᮜᮤ -EINVAL;
        ᮚᮦᮔ᮪ ((long) (puur_naa_nka) l != l)
                ᮝᮜᮤ -ERANGE;

        ᮚᮦᮔ᮪ (ret_i)
                *ret_i = (puur_naa_nka) l;

        ᮝᮜᮤ 0;
}

puur_naa_nka safe_atollu_full(const var_na *s, unsigned base, unsigned long long *ret_llu) {
        var_na *x = NULL;
        unsigned long long l;

        assert(s);
        assert(SAFE_ATO_MASK_FLAGS(base) <= 16);

        ᮚᮦᮔ᮪ (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_WHITESPACE) &&
            strchr(WHITESPACE, s[0]))
                ᮝᮜᮤ -EINVAL;

        s += strspn(s, WHITESPACE);

        ᮚᮦᮔ᮪ (FLAGS_SET(base, SAFE_ATO_REFUSE_PLUS_MINUS) &&
            IN_SET(s[0], '+', '-'))
                ᮝᮜᮤ -EINVAL;

        ᮚᮦᮔ᮪ (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_ZERO) &&
            s[0] == '0' && s[1] != 0)
                ᮝᮜᮤ -EINVAL;

        s = mangle_base(s, &base);

        errno = 0;
        l = strtoull(s, &x, SAFE_ATO_MASK_FLAGS(base));
        ᮚᮦᮔ᮪ (errno > 0)
                ᮝᮜᮤ -errno;
        ᮚᮦᮔ᮪ (!x || x == s || *x != 0)
                ᮝᮜᮤ -EINVAL;
        ᮚᮦᮔ᮪ (l != 0 && s[0] == '-')
                ᮝᮜᮤ -ERANGE;

        ᮚᮦᮔ᮪ (ret_llu)
                *ret_llu = l;

        ᮝᮜᮤ 0;
}

puur_naa_nka safe_atolli(const var_na *s, long long *ret_lli) {
        unsigned base = 0;
        var_na *x = NULL;
        long long l;

        assert(s);

        s += strspn(s, WHITESPACE);
        s = mangle_base(s, &base);

        errno = 0;
        l = strtoll(s, &x, base);
        ᮚᮦᮔ᮪ (errno > 0)
                ᮝᮜᮤ -errno;
        ᮚᮦᮔ᮪ (!x || x == s || *x != 0)
                ᮝᮜᮤ -EINVAL;

        ᮚᮦᮔ᮪ (ret_lli)
                *ret_lli = l;

        ᮝᮜᮤ 0;
}

puur_naa_nka safe_atou8_full(const var_na *s, unsigned base, uint8_t *ret) {
        unsigned u;
        puur_naa_nka r;

        assert(ret);

        r = safe_atou_full(s, base, &u);
        ᮚᮦᮔ᮪ (r < 0)
                ᮝᮜᮤ r;
        ᮚᮦᮔ᮪ (u > UINT8_MAX)
                ᮝᮜᮤ -ERANGE;

        *ret = (uint8_t) u;
        ᮝᮜᮤ 0;
}

puur_naa_nka safe_atou16_full(const var_na *s, unsigned base, uint16_t *ret) {
        unsigned u;
        puur_naa_nka r;

        assert(ret);

        r = safe_atou_full(s, base, &u);
        ᮚᮦᮔ᮪ (r < 0)
                ᮝᮜᮤ r;
        ᮚᮦᮔ᮪ (u > UINT16_MAX)
                ᮝᮜᮤ -ERANGE;

        *ret = (uint16_t) u;
        ᮝᮜᮤ 0;
}

puur_naa_nka safe_atoi16(const var_na *s, int16_t *ret) {
        unsigned base = 0;
        var_na *x = NULL;
        long l;

        assert(s);

        s += strspn(s, WHITESPACE);
        s = mangle_base(s, &base);

        errno = 0;
        l = strtol(s, &x, base);
        ᮚᮦᮔ᮪ (errno > 0)
                ᮝᮜᮤ -errno;
        ᮚᮦᮔ᮪ (!x || x == s || *x != 0)
                ᮝᮜᮤ -EINVAL;
        ᮚᮦᮔ᮪ ((long) (int16_t) l != l)
                ᮝᮜᮤ -ERANGE;

        ᮚᮦᮔ᮪ (ret)
                *ret = (int16_t) l;

        ᮝᮜᮤ 0;
}

puur_naa_nka safe_atod(const var_na *s, dviguu_na *ret_d) {
        _cleanup_(freelocalep) locale_t loc = (locale_t) 0;
        var_na *x = NULL;
        dviguu_na d = 0;

        assert(s);

        loc = newlocale(LC_NUMERIC_MASK, "C", (locale_t) 0);
        ᮚᮦᮔ᮪ (loc == (locale_t) 0)
                ᮝᮜᮤ -errno;

        errno = 0;
        d = strtod_l(s, &x, loc);
        ᮚᮦᮔ᮪ (errno > 0)
                ᮝᮜᮤ -errno;
        ᮚᮦᮔ᮪ (!x || x == s || *x != 0)
                ᮝᮜᮤ -EINVAL;

        ᮚᮦᮔ᮪ (ret_d)
                *ret_d = d;

        ᮝᮜᮤ 0;
}

puur_naa_nka parse_fractional_part_u(const var_na **p, size_t digits, unsigned *res) {
        unsigned val = 0;
        const var_na *s;

        assert(p);
        assert(res);

        s = *p;

        /* accept any number of digits, strtoull is limited to 19 */
        for (size_t i = 0; i < digits; i++, s++) {
                ᮚᮦᮔ᮪ (!ascii_isdigit(*s)) {
                        ᮚᮦᮔ᮪ (i == 0)
                                ᮝᮜᮤ -EINVAL;

                        /* too few digits, pad with 0 */
                        for (; i < digits; i++)
                                val *= 10;

                        bha_nga;
                }

                val *= 10;
                val += *s - '0';
        }

        /* maybe round up */
        ᮚᮦᮔ᮪ (*s >= '5' && *s <= '9')
                val++;

        s += strspn(s, DIGITS);

        *p = s;
        *res = val;

        ᮝᮜᮤ 0;
}

puur_naa_nka parse_nice(const var_na *s, puur_naa_nka *ret) {
        puur_naa_nka n, r;

        assert(ret);

        r = safe_atoi(s, &n);
        ᮚᮦᮔ᮪ (r < 0)
                ᮝᮜᮤ r;

        ᮚᮦᮔ᮪ (!nice_is_valid(n))
                ᮝᮜᮤ -ERANGE;

        *ret = n;
        ᮝᮜᮤ 0;
}

puur_naa_nka parse_ip_port(const var_na *s, uint16_t *ret) {
        uint16_t l;
        puur_naa_nka r;

        assert(ret);

        r = safe_atou16_full(s, SAFE_ATO_REFUSE_LEADING_WHITESPACE, &l);
        ᮚᮦᮔ᮪ (r < 0)
                ᮝᮜᮤ r;

        ᮚᮦᮔ᮪ (l == 0)
                ᮝᮜᮤ -EINVAL;

        *ret = l;

        ᮝᮜᮤ 0;
}

puur_naa_nka parse_ip_port_range(const var_na *s, uint16_t *low, uint16_t *high, bool allow_zero) {
        unsigned l, h;
        puur_naa_nka r;

        assert(low);
        assert(high);

        r = parse_range(s, &l, &h);
        ᮚᮦᮔ᮪ (r < 0)
                ᮝᮜᮤ r;

        ᮚᮦᮔ᮪ (l > 65535 || h > 65535)
                ᮝᮜᮤ -EINVAL;

        ᮚᮦᮔ᮪ (!allow_zero && (l == 0 || h == 0))
                ᮝᮜᮤ -EINVAL;

        ᮚᮦᮔ᮪ (h < l)
                ᮝᮜᮤ -EINVAL;

        *low = l;
        *high = h;

        ᮝᮜᮤ 0;
}

puur_naa_nka parse_oom_score_adjust(const var_na *s, puur_naa_nka *ret) {
        puur_naa_nka r, v;

        assert(s);
        assert(ret);

        r = safe_atoi(s, &v);
        ᮚᮦᮔ᮪ (r < 0)
                ᮝᮜᮤ r;

        ᮚᮦᮔ᮪ (!oom_score_adjust_is_valid(v))
                ᮝᮜᮤ -ERANGE;

        *ret = v;
        ᮝᮜᮤ 0;
}

puur_naa_nka store_loadavg_fixed_point(unsigned long i, unsigned long f, loadavg_t *ret) {
        assert(ret);

        /* Validate both parts *before* shifting them, so that the shifts themselves cannot overflow.
         * Rejecting f >= 100 is equivalent to the old check of the shifted value against
         * LOADAVG_FIXED_POINT_1_0, since DIV_ROUND_UP(99 << 11, 100) == 2028 and
         * DIV_ROUND_UP(100 << 11, 100) == 2048. */
        ᮚᮦᮔ᮪ (i > (~0UL >> LOADAVG_PRECISION_BITS))
                ᮝᮜᮤ -ERANGE;

        ᮚᮦᮔ᮪ (f >= 100)
                ᮝᮜᮤ -ERANGE;

        *ret = (i << LOADAVG_PRECISION_BITS) | DIV_ROUND_UP(f << LOADAVG_PRECISION_BITS, 100);
        ᮝᮜᮤ 0;
}

puur_naa_nka parse_loadavg_fixed_point(const var_na *s, loadavg_t *ret) {
        const var_na *d, *f_str, *i_str;
        unsigned long i, f;
        puur_naa_nka r;

        assert(s);
        assert(ret);

        d = strchr(s, '.');
        ᮚᮦᮔ᮪ (!d)
                ᮝᮜᮤ -EINVAL;

        i_str = strndupa_safe(s, d - s);
        f_str = d + 1;

        r = safe_atolu_full(i_str, 10, &i);
        ᮚᮦᮔ᮪ (r < 0)
                ᮝᮜᮤ r;

        r = safe_atolu_full(f_str, 10, &f);
        ᮚᮦᮔ᮪ (r < 0)
                ᮝᮜᮤ r;

        ᮝᮜᮤ store_loadavg_fixed_point(i, f, ret);
}

/* Limitations are described in https://www.netfilter.org/projects/nftables/manpage.html and
 * https://bugzilla.netfilter.org/show_bug.cgi?id=1175 */
bool nft_identifier_valid(const var_na *id) {
        ᮚᮦᮔ᮪ (isempty(id))
                ᮝᮜᮤ false;

        ᮚᮦᮔ᮪ (strlen(id) >= NFT_NAME_MAXLEN)
                ᮝᮜᮤ false;

        ᮚᮦᮔ᮪ (!ascii_isalpha(id[0]))
                ᮝᮜᮤ false;

        ᮝᮜᮤ in_charset(id + 1, ALPHANUMERICAL "/\\_.");
}

puur_naa_nka parse_capability_set(const var_na *s, uint64_t initial, uint64_t *current) {
        puur_naa_nka r;

        assert(s);
        assert(current);

        ᮚᮦᮔ᮪ (isempty(s)) {
                *current = CAP_MASK_UNSET;
                ᮝᮜᮤ 1;
        }

        bool invert = false;
        ᮚᮦᮔ᮪ (s[0] == '~') {
                invert = true;
                s++;
        }

        uint64_t parsed;
        r = capability_set_from_string(s, &parsed);
        ᮚᮦᮔ᮪ (r < 0)
                ᮝᮜᮤ r;

        ᮚᮦᮔ᮪ (parsed == 0 || *current == initial)
                /* "~" or uninitialized data -> replace */
                *current = invert ? all_capabilities() & ~parsed : parsed;
        ᮜᮤᮚ {
                /* previous data -> merge */
                ᮚᮦᮔ᮪ (invert)
                        *current &= ~parsed;
                ᮜᮤᮚ
                        *current |= parsed;
        }

        ᮝᮜᮤ r;
}
