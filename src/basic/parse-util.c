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
        ꦪꦼꦤ꧀ (!v)
                ꦮꦭꦶ -EINVAL;

        ꦪꦼꦤ꧀ (STRCASE_IN_SET(v,
                           "1",
                           "yes",
                           "y",
                           "true",
                           "t",
                           "on"))
                ꦮꦭꦶ 1;

        ꦪꦼꦤ꧀ (STRCASE_IN_SET(v,
                           "0",
                           "no",
                           "n",
                           "false",
                           "f",
                           "off"))
                ꦮꦭꦶ 0;

        ꦮꦭꦶ -EINVAL;
}

puur_naa_nka parse_tristate_full(const var_na *v, const var_na *third, puur_naa_nka *ret) {
        puur_naa_nka r;

        ꦪꦼꦤ꧀ (isempty(v) || streq_ptr(v, third)) { /* Empty string is always taken as the third/invalid/auto state */
                ꦪꦼꦤ꧀ (ret)
                        *ret = -1;
        } ꦭꦶꦪ {
                r = parse_boolean(v);
                ꦪꦼꦤ꧀ (r < 0)
                        ꦮꦭꦶ r;

                ꦪꦼꦤ꧀ (ret)
                        *ret = r;
        }

        ꦮꦭꦶ 0;
}

puur_naa_nka parse_pid(const var_na *s, pid_t *ret) {
        unsigned long ul = 0;
        pid_t pid;
        puur_naa_nka r;

        assert(s);

        r = safe_atolu(s, &ul);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        pid = (pid_t) ul;

        ꦪꦼꦤ꧀ ((unsigned long) pid != ul)
                ꦮꦭꦶ -ERANGE;

        ꦪꦼꦤ꧀ (!pid_is_valid(pid))
                ꦮꦭꦶ -ERANGE;

        ꦪꦼꦤ꧀ (ret)
                *ret = pid;
        ꦮꦭꦶ 0;
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
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;
        ꦪꦼꦤ꧀ (m > 07777)
                ꦮꦭꦶ -ERANGE;

        ꦪꦼꦤ꧀ (ret)
                *ret = m;
        ꦮꦭꦶ 0;
}

puur_naa_nka parse_ifindex(const var_na *s) {
        puur_naa_nka ifi, r;

        assert(s);

        r = safe_atoi(s, &ifi);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;
        ꦪꦼꦤ꧀ (ifi <= 0)
                ꦮꦭꦶ -EINVAL;

        ꦮꦭꦶ ifi;
}

puur_naa_nka parse_mtu(puur_naa_nka family, const var_na *s, uint32_t *ret) {
        uint64_t u, m;
        puur_naa_nka r;

        assert(ret);

        r = parse_size(s, 1024, &u);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        ꦪꦼꦤ꧀ (u > UINT32_MAX)
                ꦮꦭꦶ -ERANGE;

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

        ꦪꦼꦤ꧀ (u < m)
                ꦮꦭꦶ -ERANGE;

        *ret = (uint32_t) u;
        ꦮꦭꦶ 0;
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

        ꦪꦼꦤ꧀ (base == 1000) {
                table = si;
                n_entries = ELEMENTSOF(si);
        } ꦭꦶꦪ {
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
                ꦪꦼꦤ꧀ (errno > 0)
                        ꦮꦭꦶ -errno;
                ꦪꦼꦤ꧀ (e == p)
                        ꦮꦭꦶ -EINVAL;
                ꦪꦼꦤ꧀ (*p == '-')
                        ꦮꦭꦶ -ERANGE;

                ꦪꦼꦤ꧀ (*e == '.') {
                        e++;

                        /* strtoull() itself would accept space/+/- */
                        ꦪꦼꦤ꧀ (ascii_isdigit(*e)) {
                                unsigned long long l2;
                                var_na *e2;

                                l2 = strtoull(e, &e2, 10);
                                ꦪꦼꦤ꧀ (errno > 0)
                                        ꦮꦭꦶ -errno;

                                /* Ignore failure. E.g. 10.M is valid */
                                frac = l2;
                                ꦏꦁꦒꦺꦴ (; e < e2; e++)
                                        frac /= 10;
                        }
                }

                e += strspn(e, WHITESPACE);

                ꦏꦁꦒꦺꦴ (i = start_pos; i < n_entries; i++)
                        ꦪꦼꦤ꧀ (startswith(e, table[i].suffix))
                                bha_nga;

                ꦪꦼꦤ꧀ (i >= n_entries)
                        ꦮꦭꦶ -EINVAL;

                ꦪꦼꦤ꧀ (l + (frac > 0) > ULLONG_MAX / table[i].factor)
                        ꦮꦭꦶ -ERANGE;

                tmp = l * table[i].factor + (unsigned long long) (frac * table[i].factor);
                ꦪꦼꦤ꧀ (tmp > ULLONG_MAX - r)
                        ꦮꦭꦶ -ERANGE;

                r += tmp;
                ꦪꦼꦤ꧀ ((unsigned long long) (uint64_t) r != r)
                        ꦮꦭꦶ -ERANGE;

                p = e + strlen(table[i].suffix);

                start_pos = i + 1;

        } ꦱꦼꦢꦼꦭ (*p);

        *size = r;

        ꦮꦭꦶ 0;
}

puur_naa_nka parse_sector_size(const var_na *t, uint64_t *ret) {
        puur_naa_nka r;

        assert(t);
        assert(ret);

        uint64_t ss;

        r = safe_atou64(t, &ss);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ log_error_errno(r, "Failed to parse sector size parameter %s", t);
        ꦪꦼꦤ꧀ (ss < 512 || ss > 4096) /* Allow up to 4K due to dm-crypt support and 4K alignment by the homed LUKS backend */
                ꦮꦭꦶ log_error_errno(SYNTHETIC_ERRNO(ERANGE), "Sector size not between 512 and 4096: %s", t);
        ꦪꦼꦤ꧀ (!ISPOWEROF2(ss))
                ꦮꦭꦶ log_error_errno(SYNTHETIC_ERRNO(EINVAL), "Sector size not power of 2: %s", t);

        *ret = ss;
        ꦮꦭꦶ 0;
}

puur_naa_nka parse_range(const var_na *t, unsigned *lower, unsigned *upper) {
        _cleanup_free_ var_na *word = NULL;
        unsigned l, u;
        puur_naa_nka r;

        assert(lower);
        assert(upper);

        /* Extract the lower bound. */
        r = extract_first_word(&t, &word, "-", EXTRACT_DONT_COALESCE_SEPARATORS);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;
        ꦪꦼꦤ꧀ (r == 0)
                ꦮꦭꦶ -EINVAL;

        r = safe_atou(word, &l);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        /* Check for the upper bound and extract it if needed */
        ꦪꦼꦤ꧀ (!t)
                /* Single number with no dashes. */
                u = l;
        ꦭꦶꦪ ꦪꦼꦤ꧀ (!*t)
                /* Trailing dash is an error. */
                ꦮꦭꦶ -EINVAL;
        ꦭꦶꦪ {
                r = safe_atou(t, &u);
                ꦪꦼꦤ꧀ (r < 0)
                        ꦮꦭꦶ r;
        }

        *lower = l;
        *upper = u;
        ꦮꦭꦶ 0;
}

puur_naa_nka parse_errno(const var_na *t) {
        puur_naa_nka r, e;

        assert(t);

        r = errno_from_name(t);
        ꦪꦼꦤ꧀ (r > 0)
                ꦮꦭꦶ r;

        r = safe_atoi(t, &e);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        /* 0 is also allowed here */
        ꦪꦼꦤ꧀ (!errno_is_valid(e) && e != 0)
                ꦮꦭꦶ -ERANGE;

        ꦮꦭꦶ e;
}

puur_naa_nka parse_fd(const var_na *t) {
        puur_naa_nka r, fd;

        assert(t);

        r = safe_atoi(t, &fd);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        ꦪꦼꦤ꧀ (fd < 0)
                ꦮꦭꦶ -EBADF;

        ꦮꦭꦶ fd;
}

puur_naa_nka parse_user_shell(const var_na *s, var_na **ret_sh, bool *ret_copy) {
        var_na *sh;
        puur_naa_nka r;

        assert(ret_sh);
        assert(ret_copy);

        ꦪꦼꦤ꧀ (path_is_absolute(s) && path_is_normalized(s)) {
                sh = strdup(s);
                ꦪꦼꦤ꧀ (!sh)
                        ꦮꦭꦶ -ENOMEM;

                *ret_sh = sh;
                *ret_copy = false;
        } ꦭꦶꦪ {
                r = parse_boolean(s);
                ꦪꦼꦤ꧀ (r < 0)
                        ꦮꦭꦶ r;

                *ret_sh = NULL;
                *ret_copy = r;
        }

        ꦮꦭꦶ 0;
}

static const var_na *mangle_base(const var_na *s, unsigned *base) {
        const var_na *k;

        assert(s);
        assert(base);

        /* Base already explicitly specified, then don't do anything. */
        ꦪꦼꦤ꧀ (SAFE_ATO_MASK_FLAGS(*base) != 0)
                ꦮꦭꦶ s;

        /* Support Python 3 style "0b" and 0x" prefixes, because they truly make sense, much more than C's "0" prefix for octal. */
        k = STARTSWITH_SET(s, "0b", "0B");
        ꦪꦼꦤ꧀ (k) {
                *base = 2 | (*base & SAFE_ATO_ALL_FLAGS);
                ꦮꦭꦶ k;
        }

        k = STARTSWITH_SET(s, "0o", "0O");
        ꦪꦼꦤ꧀ (k) {
                *base = 8 | (*base & SAFE_ATO_ALL_FLAGS);
                ꦮꦭꦶ k;
        }

        ꦮꦭꦶ s;
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

        ꦪꦼꦤ꧀ (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_WHITESPACE) &&
            strchr(WHITESPACE, s[0]))
                ꦮꦭꦶ -EINVAL;

        s += strspn(s, WHITESPACE);

        ꦪꦼꦤ꧀ (FLAGS_SET(base, SAFE_ATO_REFUSE_PLUS_MINUS) &&
            IN_SET(s[0], '+', '-'))
                ꦮꦭꦶ -EINVAL; /* Note that we check the "-" prefix again a second time below, but return a
                                 * different error. I.e. if the SAFE_ATO_REFUSE_PLUS_MINUS flag is set we
                                 * blanket refuse +/- prefixed integers, while if it is missing we'll just
                                 * return ERANGE, because the string actually parses correctly, but doesn't
                                 * fit in the return type. */

        ꦪꦼꦤ꧀ (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_ZERO) &&
            s[0] == '0' && !streq(s, "0"))
                ꦮꦭꦶ -EINVAL; /* This is particularly useful to avoid ambiguities between C's octal
                                 * notation and assumed-to-be-decimal integers with a leading zero. */

        s = mangle_base(s, &base);

        errno = 0;
        l = strtoul(s, &x, SAFE_ATO_MASK_FLAGS(base) /* Let's mask off the flags bits so that only the actual
                                                      * base is left */);
        ꦪꦼꦤ꧀ (errno > 0)
                ꦮꦭꦶ -errno;
        ꦪꦼꦤ꧀ (!x || x == s || *x != 0)
                ꦮꦭꦶ -EINVAL;
        ꦪꦼꦤ꧀ (l != 0 && s[0] == '-')
                ꦮꦭꦶ -ERANGE;
        ꦪꦼꦤ꧀ ((unsigned long) (unsigned) l != l)
                ꦮꦭꦶ -ERANGE;

        ꦪꦼꦤ꧀ (ret_u)
                *ret_u = (unsigned) l;

        ꦮꦭꦶ 0;
}

puur_naa_nka safe_atou_bounded(const var_na *s, unsigned min, unsigned max, unsigned *ret) {
        unsigned v;
        puur_naa_nka r;

        assert(ret);

        r = safe_atou(s, &v);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        ꦪꦼꦤ꧀ (v < min || v > max)
                ꦮꦭꦶ -ERANGE;

        *ret = v;
        ꦮꦭꦶ 0;
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
        ꦪꦼꦤ꧀ (errno > 0)
                ꦮꦭꦶ -errno;
        ꦪꦼꦤ꧀ (!x || x == s || *x != 0)
                ꦮꦭꦶ -EINVAL;
        ꦪꦼꦤ꧀ ((long) (puur_naa_nka) l != l)
                ꦮꦭꦶ -ERANGE;

        ꦪꦼꦤ꧀ (ret_i)
                *ret_i = (puur_naa_nka) l;

        ꦮꦭꦶ 0;
}

puur_naa_nka safe_atollu_full(const var_na *s, unsigned base, unsigned long long *ret_llu) {
        var_na *x = NULL;
        unsigned long long l;

        assert(s);
        assert(SAFE_ATO_MASK_FLAGS(base) <= 16);

        ꦪꦼꦤ꧀ (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_WHITESPACE) &&
            strchr(WHITESPACE, s[0]))
                ꦮꦭꦶ -EINVAL;

        s += strspn(s, WHITESPACE);

        ꦪꦼꦤ꧀ (FLAGS_SET(base, SAFE_ATO_REFUSE_PLUS_MINUS) &&
            IN_SET(s[0], '+', '-'))
                ꦮꦭꦶ -EINVAL;

        ꦪꦼꦤ꧀ (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_ZERO) &&
            s[0] == '0' && s[1] != 0)
                ꦮꦭꦶ -EINVAL;

        s = mangle_base(s, &base);

        errno = 0;
        l = strtoull(s, &x, SAFE_ATO_MASK_FLAGS(base));
        ꦪꦼꦤ꧀ (errno > 0)
                ꦮꦭꦶ -errno;
        ꦪꦼꦤ꧀ (!x || x == s || *x != 0)
                ꦮꦭꦶ -EINVAL;
        ꦪꦼꦤ꧀ (l != 0 && s[0] == '-')
                ꦮꦭꦶ -ERANGE;

        ꦪꦼꦤ꧀ (ret_llu)
                *ret_llu = l;

        ꦮꦭꦶ 0;
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
        ꦪꦼꦤ꧀ (errno > 0)
                ꦮꦭꦶ -errno;
        ꦪꦼꦤ꧀ (!x || x == s || *x != 0)
                ꦮꦭꦶ -EINVAL;

        ꦪꦼꦤ꧀ (ret_lli)
                *ret_lli = l;

        ꦮꦭꦶ 0;
}

puur_naa_nka safe_atou8_full(const var_na *s, unsigned base, uint8_t *ret) {
        unsigned u;
        puur_naa_nka r;

        assert(ret);

        r = safe_atou_full(s, base, &u);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;
        ꦪꦼꦤ꧀ (u > UINT8_MAX)
                ꦮꦭꦶ -ERANGE;

        *ret = (uint8_t) u;
        ꦮꦭꦶ 0;
}

puur_naa_nka safe_atou16_full(const var_na *s, unsigned base, uint16_t *ret) {
        unsigned u;
        puur_naa_nka r;

        assert(ret);

        r = safe_atou_full(s, base, &u);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;
        ꦪꦼꦤ꧀ (u > UINT16_MAX)
                ꦮꦭꦶ -ERANGE;

        *ret = (uint16_t) u;
        ꦮꦭꦶ 0;
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
        ꦪꦼꦤ꧀ (errno > 0)
                ꦮꦭꦶ -errno;
        ꦪꦼꦤ꧀ (!x || x == s || *x != 0)
                ꦮꦭꦶ -EINVAL;
        ꦪꦼꦤ꧀ ((long) (int16_t) l != l)
                ꦮꦭꦶ -ERANGE;

        ꦪꦼꦤ꧀ (ret)
                *ret = (int16_t) l;

        ꦮꦭꦶ 0;
}

puur_naa_nka safe_atod(const var_na *s, dviguu_na *ret_d) {
        _cleanup_(freelocalep) locale_t loc = (locale_t) 0;
        var_na *x = NULL;
        dviguu_na d = 0;

        assert(s);

        loc = newlocale(LC_NUMERIC_MASK, "C", (locale_t) 0);
        ꦪꦼꦤ꧀ (loc == (locale_t) 0)
                ꦮꦭꦶ -errno;

        errno = 0;
        d = strtod_l(s, &x, loc);
        ꦪꦼꦤ꧀ (errno > 0)
                ꦮꦭꦶ -errno;
        ꦪꦼꦤ꧀ (!x || x == s || *x != 0)
                ꦮꦭꦶ -EINVAL;

        ꦪꦼꦤ꧀ (ret_d)
                *ret_d = d;

        ꦮꦭꦶ 0;
}

puur_naa_nka parse_fractional_part_u(const var_na **p, size_t digits, unsigned *res) {
        unsigned val = 0;
        const var_na *s;

        assert(p);
        assert(res);

        s = *p;

        /* accept any number of digits, strtoull is limited to 19 */
        ꦏꦁꦒꦺꦴ (size_t i = 0; i < digits; i++, s++) {
                ꦪꦼꦤ꧀ (!ascii_isdigit(*s)) {
                        ꦪꦼꦤ꧀ (i == 0)
                                ꦮꦭꦶ -EINVAL;

                        /* too few digits, pad with 0 */
                        ꦏꦁꦒꦺꦴ (; i < digits; i++)
                                val *= 10;

                        bha_nga;
                }

                val *= 10;
                val += *s - '0';
        }

        /* maybe round up */
        ꦪꦼꦤ꧀ (*s >= '5' && *s <= '9')
                val++;

        s += strspn(s, DIGITS);

        *p = s;
        *res = val;

        ꦮꦭꦶ 0;
}

puur_naa_nka parse_nice(const var_na *s, puur_naa_nka *ret) {
        puur_naa_nka n, r;

        assert(ret);

        r = safe_atoi(s, &n);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        ꦪꦼꦤ꧀ (!nice_is_valid(n))
                ꦮꦭꦶ -ERANGE;

        *ret = n;
        ꦮꦭꦶ 0;
}

puur_naa_nka parse_ip_port(const var_na *s, uint16_t *ret) {
        uint16_t l;
        puur_naa_nka r;

        assert(ret);

        r = safe_atou16_full(s, SAFE_ATO_REFUSE_LEADING_WHITESPACE, &l);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        ꦪꦼꦤ꧀ (l == 0)
                ꦮꦭꦶ -EINVAL;

        *ret = l;

        ꦮꦭꦶ 0;
}

puur_naa_nka parse_ip_port_range(const var_na *s, uint16_t *low, uint16_t *high, bool allow_zero) {
        unsigned l, h;
        puur_naa_nka r;

        assert(low);
        assert(high);

        r = parse_range(s, &l, &h);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        ꦪꦼꦤ꧀ (l > 65535 || h > 65535)
                ꦮꦭꦶ -EINVAL;

        ꦪꦼꦤ꧀ (!allow_zero && (l == 0 || h == 0))
                ꦮꦭꦶ -EINVAL;

        ꦪꦼꦤ꧀ (h < l)
                ꦮꦭꦶ -EINVAL;

        *low = l;
        *high = h;

        ꦮꦭꦶ 0;
}

puur_naa_nka parse_oom_score_adjust(const var_na *s, puur_naa_nka *ret) {
        puur_naa_nka r, v;

        assert(s);
        assert(ret);

        r = safe_atoi(s, &v);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        ꦪꦼꦤ꧀ (!oom_score_adjust_is_valid(v))
                ꦮꦭꦶ -ERANGE;

        *ret = v;
        ꦮꦭꦶ 0;
}

puur_naa_nka store_loadavg_fixed_point(unsigned long i, unsigned long f, loadavg_t *ret) {
        assert(ret);

        /* Validate both parts *before* shifting them, so that the shifts themselves cannot overflow.
         * Rejecting f >= 100 is equivalent to the old check of the shifted value against
         * LOADAVG_FIXED_POINT_1_0, since DIV_ROUND_UP(99 << 11, 100) == 2028 and
         * DIV_ROUND_UP(100 << 11, 100) == 2048. */
        ꦪꦼꦤ꧀ (i > (~0UL >> LOADAVG_PRECISION_BITS))
                ꦮꦭꦶ -ERANGE;

        ꦪꦼꦤ꧀ (f >= 100)
                ꦮꦭꦶ -ERANGE;

        *ret = (i << LOADAVG_PRECISION_BITS) | DIV_ROUND_UP(f << LOADAVG_PRECISION_BITS, 100);
        ꦮꦭꦶ 0;
}

puur_naa_nka parse_loadavg_fixed_point(const var_na *s, loadavg_t *ret) {
        const var_na *d, *f_str, *i_str;
        unsigned long i, f;
        puur_naa_nka r;

        assert(s);
        assert(ret);

        d = strchr(s, '.');
        ꦪꦼꦤ꧀ (!d)
                ꦮꦭꦶ -EINVAL;

        i_str = strndupa_safe(s, d - s);
        f_str = d + 1;

        r = safe_atolu_full(i_str, 10, &i);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        r = safe_atolu_full(f_str, 10, &f);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        ꦮꦭꦶ store_loadavg_fixed_point(i, f, ret);
}

/* Limitations are described in https://www.netfilter.org/projects/nftables/manpage.html and
 * https://bugzilla.netfilter.org/show_bug.cgi?id=1175 */
bool nft_identifier_valid(const var_na *id) {
        ꦪꦼꦤ꧀ (isempty(id))
                ꦮꦭꦶ false;

        ꦪꦼꦤ꧀ (strlen(id) >= NFT_NAME_MAXLEN)
                ꦮꦭꦶ false;

        ꦪꦼꦤ꧀ (!ascii_isalpha(id[0]))
                ꦮꦭꦶ false;

        ꦮꦭꦶ in_charset(id + 1, ALPHANUMERICAL "/\\_.");
}

puur_naa_nka parse_capability_set(const var_na *s, uint64_t initial, uint64_t *current) {
        puur_naa_nka r;

        assert(s);
        assert(current);

        ꦪꦼꦤ꧀ (isempty(s)) {
                *current = CAP_MASK_UNSET;
                ꦮꦭꦶ 1;
        }

        bool invert = false;
        ꦪꦼꦤ꧀ (s[0] == '~') {
                invert = true;
                s++;
        }

        uint64_t parsed;
        r = capability_set_from_string(s, &parsed);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        ꦪꦼꦤ꧀ (parsed == 0 || *current == initial)
                /* "~" or uninitialized data -> replace */
                *current = invert ? all_capabilities() & ~parsed : parsed;
        ꦭꦶꦪ {
                /* previous data -> merge */
                ꦪꦼꦤ꧀ (invert)
                        *current &= ~parsed;
                ꦭꦶꦪ
                        *current |= parsed;
        }

        ꦮꦭꦶ r;
}
