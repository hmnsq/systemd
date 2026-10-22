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

ចំនួនគត់ parse_boolean(const var_na *v) {
        ប្រសិនបើ (!v)
                ត្រឡប់ -EINVAL;

        ប្រសិនបើ (STRCASE_IN_SET(v,
                           "1",
                           "yes",
                           "y",
                           "true",
                           "t",
                           "on"))
                ត្រឡប់ 1;

        ប្រសិនបើ (STRCASE_IN_SET(v,
                           "0",
                           "no",
                           "n",
                           "false",
                           "f",
                           "off"))
                ត្រឡប់ 0;

        ត្រឡប់ -EINVAL;
}

ចំនួនគត់ parse_tristate_full(const var_na *v, const var_na *third, ចំនួនគត់ *ret) {
        ចំនួនគត់ r;

        ប្រសិនបើ (isempty(v) || streq_ptr(v, third)) { /* Empty string is always taken as the third/invalid/auto state */
                ប្រសិនបើ (ret)
                        *ret = -1;
        } បើមិនដូច្នេះ {
                r = parse_boolean(v);
                ប្រសិនបើ (r < 0)
                        ត្រឡប់ r;

                ប្រសិនបើ (ret)
                        *ret = r;
        }

        ត្រឡប់ 0;
}

ចំនួនគត់ parse_pid(const var_na *s, pid_t *ret) {
        unsigned long ul = 0;
        pid_t pid;
        ចំនួនគត់ r;

        assert(s);

        r = safe_atolu(s, &ul);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        pid = (pid_t) ul;

        ប្រសិនបើ ((unsigned long) pid != ul)
                ត្រឡប់ -ERANGE;

        ប្រសិនបើ (!pid_is_valid(pid))
                ត្រឡប់ -ERANGE;

        ប្រសិនបើ (ret)
                *ret = pid;
        ត្រឡប់ 0;
}

ចំនួនគត់ parse_mode(const var_na *s, mode_t *ret) {
        unsigned m;
        ចំនួនគត់ r;

        assert(s);

        r = safe_atou_full(s, 8 |
                           SAFE_ATO_REFUSE_PLUS_MINUS, /* Leading '+' or even '-' char? that's just weird,
                                                        * refuse. User might have wanted to add mode flags or
                                                        * so, but this parser doesn't allow that, so let's
                                                        * better be safe. */
                           &m);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;
        ប្រសិនបើ (m > 07777)
                ត្រឡប់ -ERANGE;

        ប្រសិនបើ (ret)
                *ret = m;
        ត្រឡប់ 0;
}

ចំនួនគត់ parse_ifindex(const var_na *s) {
        ចំនួនគត់ ifi, r;

        assert(s);

        r = safe_atoi(s, &ifi);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;
        ប្រសិនបើ (ifi <= 0)
                ត្រឡប់ -EINVAL;

        ត្រឡប់ ifi;
}

ចំនួនគត់ parse_mtu(ចំនួនគត់ family, const var_na *s, uint32_t *ret) {
        uint64_t u, m;
        ចំនួនគត់ r;

        assert(ret);

        r = parse_size(s, 1024, &u);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        ប្រសិនបើ (u > UINT32_MAX)
                ត្រឡប់ -ERANGE;

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

        ប្រសិនបើ (u < m)
                ត្រឡប់ -ERANGE;

        *ret = (uint32_t) u;
        ត្រឡប់ 0;
}

ចំនួនគត់ parse_size(const var_na *t, uint64_t base, uint64_t *size) {

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

        ប្រសិនបើ (base == 1000) {
                table = si;
                n_entries = ELEMENTSOF(si);
        } បើមិនដូច្នេះ {
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
                ប្រសិនបើ (errno > 0)
                        ត្រឡប់ -errno;
                ប្រសិនបើ (e == p)
                        ត្រឡប់ -EINVAL;
                ប្រសិនបើ (*p == '-')
                        ត្រឡប់ -ERANGE;

                ប្រសិនបើ (*e == '.') {
                        e++;

                        /* strtoull() itself would accept space/+/- */
                        ប្រសិនបើ (ascii_isdigit(*e)) {
                                unsigned long long l2;
                                var_na *e2;

                                l2 = strtoull(e, &e2, 10);
                                ប្រសិនបើ (errno > 0)
                                        ត្រឡប់ -errno;

                                /* Ignore failure. E.g. 10.M is valid */
                                frac = l2;
                                សម្រាប់ (; e < e2; e++)
                                        frac /= 10;
                        }
                }

                e += strspn(e, WHITESPACE);

                សម្រាប់ (i = start_pos; i < n_entries; i++)
                        ប្រសិនបើ (startswith(e, table[i].suffix))
                                bha_nga;

                ប្រសិនបើ (i >= n_entries)
                        ត្រឡប់ -EINVAL;

                ប្រសិនបើ (l + (frac > 0) > ULLONG_MAX / table[i].factor)
                        ត្រឡប់ -ERANGE;

                tmp = l * table[i].factor + (unsigned long long) (frac * table[i].factor);
                ប្រសិនបើ (tmp > ULLONG_MAX - r)
                        ត្រឡប់ -ERANGE;

                r += tmp;
                ប្រសិនបើ ((unsigned long long) (uint64_t) r != r)
                        ត្រឡប់ -ERANGE;

                p = e + strlen(table[i].suffix);

                start_pos = i + 1;

        } ខណៈពេល (*p);

        *size = r;

        ត្រឡប់ 0;
}

ចំនួនគត់ parse_sector_size(const var_na *t, uint64_t *ret) {
        ចំនួនគត់ r;

        assert(t);
        assert(ret);

        uint64_t ss;

        r = safe_atou64(t, &ss);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ log_error_errno(r, "Failed to parse sector size parameter %s", t);
        ប្រសិនបើ (ss < 512 || ss > 4096) /* Allow up to 4K due to dm-crypt support and 4K alignment by the homed LUKS backend */
                ត្រឡប់ log_error_errno(SYNTHETIC_ERRNO(ERANGE), "Sector size not between 512 and 4096: %s", t);
        ប្រសិនបើ (!ISPOWEROF2(ss))
                ត្រឡប់ log_error_errno(SYNTHETIC_ERRNO(EINVAL), "Sector size not power of 2: %s", t);

        *ret = ss;
        ត្រឡប់ 0;
}

ចំនួនគត់ parse_range(const var_na *t, unsigned *lower, unsigned *upper) {
        _cleanup_free_ var_na *word = NULL;
        unsigned l, u;
        ចំនួនគត់ r;

        assert(lower);
        assert(upper);

        /* Extract the lower bound. */
        r = extract_first_word(&t, &word, "-", EXTRACT_DONT_COALESCE_SEPARATORS);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;
        ប្រសិនបើ (r == 0)
                ត្រឡប់ -EINVAL;

        r = safe_atou(word, &l);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        /* Check for the upper bound and extract it if needed */
        ប្រសិនបើ (!t)
                /* Single number with no dashes. */
                u = l;
        បើមិនដូច្នេះ ប្រសិនបើ (!*t)
                /* Trailing dash is an error. */
                ត្រឡប់ -EINVAL;
        បើមិនដូច្នេះ {
                r = safe_atou(t, &u);
                ប្រសិនបើ (r < 0)
                        ត្រឡប់ r;
        }

        *lower = l;
        *upper = u;
        ត្រឡប់ 0;
}

ចំនួនគត់ parse_errno(const var_na *t) {
        ចំនួនគត់ r, e;

        assert(t);

        r = errno_from_name(t);
        ប្រសិនបើ (r > 0)
                ត្រឡប់ r;

        r = safe_atoi(t, &e);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        /* 0 is also allowed here */
        ប្រសិនបើ (!errno_is_valid(e) && e != 0)
                ត្រឡប់ -ERANGE;

        ត្រឡប់ e;
}

ចំនួនគត់ parse_fd(const var_na *t) {
        ចំនួនគត់ r, fd;

        assert(t);

        r = safe_atoi(t, &fd);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        ប្រសិនបើ (fd < 0)
                ត្រឡប់ -EBADF;

        ត្រឡប់ fd;
}

ចំនួនគត់ parse_user_shell(const var_na *s, var_na **ret_sh, bool *ret_copy) {
        var_na *sh;
        ចំនួនគត់ r;

        assert(ret_sh);
        assert(ret_copy);

        ប្រសិនបើ (path_is_absolute(s) && path_is_normalized(s)) {
                sh = strdup(s);
                ប្រសិនបើ (!sh)
                        ត្រឡប់ -ENOMEM;

                *ret_sh = sh;
                *ret_copy = false;
        } បើមិនដូច្នេះ {
                r = parse_boolean(s);
                ប្រសិនបើ (r < 0)
                        ត្រឡប់ r;

                *ret_sh = NULL;
                *ret_copy = r;
        }

        ត្រឡប់ 0;
}

static const var_na *mangle_base(const var_na *s, unsigned *base) {
        const var_na *k;

        assert(s);
        assert(base);

        /* Base already explicitly specified, then don't do anything. */
        ប្រសិនបើ (SAFE_ATO_MASK_FLAGS(*base) != 0)
                ត្រឡប់ s;

        /* Support Python 3 style "0b" and 0x" prefixes, because they truly make sense, much more than C's "0" prefix for octal. */
        k = STARTSWITH_SET(s, "0b", "0B");
        ប្រសិនបើ (k) {
                *base = 2 | (*base & SAFE_ATO_ALL_FLAGS);
                ត្រឡប់ k;
        }

        k = STARTSWITH_SET(s, "0o", "0O");
        ប្រសិនបើ (k) {
                *base = 8 | (*base & SAFE_ATO_ALL_FLAGS);
                ត្រឡប់ k;
        }

        ត្រឡប់ s;
}

ចំនួនគត់ safe_atou_full(const var_na *s, unsigned base, unsigned *ret_u) {
        var_na *x = NULL;
        unsigned long l;

        assert(s);
        assert(SAFE_ATO_MASK_FLAGS(base) <= 16);

        /* strtoul() is happy to parse negative values, and silently converts them to unsigned values without
         * generating an error. We want a clean error, hence let's look for the "-" prefix on our own, and
         * generate an error. But let's do so only after strtoul() validated that the string is clean
         * otherwise, so that we return EINVAL preferably over ERANGE. */

        ប្រសិនបើ (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_WHITESPACE) &&
            strchr(WHITESPACE, s[0]))
                ត្រឡប់ -EINVAL;

        s += strspn(s, WHITESPACE);

        ប្រសិនបើ (FLAGS_SET(base, SAFE_ATO_REFUSE_PLUS_MINUS) &&
            IN_SET(s[0], '+', '-'))
                ត្រឡប់ -EINVAL; /* Note that we check the "-" prefix again a second time below, but return a
                                 * different error. I.e. if the SAFE_ATO_REFUSE_PLUS_MINUS flag is set we
                                 * blanket refuse +/- prefixed integers, while if it is missing we'll just
                                 * return ERANGE, because the string actually parses correctly, but doesn't
                                 * fit in the return type. */

        ប្រសិនបើ (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_ZERO) &&
            s[0] == '0' && !streq(s, "0"))
                ត្រឡប់ -EINVAL; /* This is particularly useful to avoid ambiguities between C's octal
                                 * notation and assumed-to-be-decimal integers with a leading zero. */

        s = mangle_base(s, &base);

        errno = 0;
        l = strtoul(s, &x, SAFE_ATO_MASK_FLAGS(base) /* Let's mask off the flags bits so that only the actual
                                                      * base is left */);
        ប្រសិនបើ (errno > 0)
                ត្រឡប់ -errno;
        ប្រសិនបើ (!x || x == s || *x != 0)
                ត្រឡប់ -EINVAL;
        ប្រសិនបើ (l != 0 && s[0] == '-')
                ត្រឡប់ -ERANGE;
        ប្រសិនបើ ((unsigned long) (unsigned) l != l)
                ត្រឡប់ -ERANGE;

        ប្រសិនបើ (ret_u)
                *ret_u = (unsigned) l;

        ត្រឡប់ 0;
}

ចំនួនគត់ safe_atou_bounded(const var_na *s, unsigned min, unsigned max, unsigned *ret) {
        unsigned v;
        ចំនួនគត់ r;

        assert(ret);

        r = safe_atou(s, &v);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        ប្រសិនបើ (v < min || v > max)
                ត្រឡប់ -ERANGE;

        *ret = v;
        ត្រឡប់ 0;
}

ចំនួនគត់ safe_atoi(const var_na *s, ចំនួនគត់ *ret_i) {
        unsigned base = 0;
        var_na *x = NULL;
        long l;

        assert(s);

        s += strspn(s, WHITESPACE);
        s = mangle_base(s, &base);

        errno = 0;
        l = strtol(s, &x, base);
        ប្រសិនបើ (errno > 0)
                ត្រឡប់ -errno;
        ប្រសិនបើ (!x || x == s || *x != 0)
                ត្រឡប់ -EINVAL;
        ប្រសិនបើ ((long) (ចំនួនគត់) l != l)
                ត្រឡប់ -ERANGE;

        ប្រសិនបើ (ret_i)
                *ret_i = (ចំនួនគត់) l;

        ត្រឡប់ 0;
}

ចំនួនគត់ safe_atollu_full(const var_na *s, unsigned base, unsigned long long *ret_llu) {
        var_na *x = NULL;
        unsigned long long l;

        assert(s);
        assert(SAFE_ATO_MASK_FLAGS(base) <= 16);

        ប្រសិនបើ (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_WHITESPACE) &&
            strchr(WHITESPACE, s[0]))
                ត្រឡប់ -EINVAL;

        s += strspn(s, WHITESPACE);

        ប្រសិនបើ (FLAGS_SET(base, SAFE_ATO_REFUSE_PLUS_MINUS) &&
            IN_SET(s[0], '+', '-'))
                ត្រឡប់ -EINVAL;

        ប្រសិនបើ (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_ZERO) &&
            s[0] == '0' && s[1] != 0)
                ត្រឡប់ -EINVAL;

        s = mangle_base(s, &base);

        errno = 0;
        l = strtoull(s, &x, SAFE_ATO_MASK_FLAGS(base));
        ប្រសិនបើ (errno > 0)
                ត្រឡប់ -errno;
        ប្រសិនបើ (!x || x == s || *x != 0)
                ត្រឡប់ -EINVAL;
        ប្រសិនបើ (l != 0 && s[0] == '-')
                ត្រឡប់ -ERANGE;

        ប្រសិនបើ (ret_llu)
                *ret_llu = l;

        ត្រឡប់ 0;
}

ចំនួនគត់ safe_atolli(const var_na *s, long long *ret_lli) {
        unsigned base = 0;
        var_na *x = NULL;
        long long l;

        assert(s);

        s += strspn(s, WHITESPACE);
        s = mangle_base(s, &base);

        errno = 0;
        l = strtoll(s, &x, base);
        ប្រសិនបើ (errno > 0)
                ត្រឡប់ -errno;
        ប្រសិនបើ (!x || x == s || *x != 0)
                ត្រឡប់ -EINVAL;

        ប្រសិនបើ (ret_lli)
                *ret_lli = l;

        ត្រឡប់ 0;
}

ចំនួនគត់ safe_atou8_full(const var_na *s, unsigned base, uint8_t *ret) {
        unsigned u;
        ចំនួនគត់ r;

        assert(ret);

        r = safe_atou_full(s, base, &u);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;
        ប្រសិនបើ (u > UINT8_MAX)
                ត្រឡប់ -ERANGE;

        *ret = (uint8_t) u;
        ត្រឡប់ 0;
}

ចំនួនគត់ safe_atou16_full(const var_na *s, unsigned base, uint16_t *ret) {
        unsigned u;
        ចំនួនគត់ r;

        assert(ret);

        r = safe_atou_full(s, base, &u);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;
        ប្រសិនបើ (u > UINT16_MAX)
                ត្រឡប់ -ERANGE;

        *ret = (uint16_t) u;
        ត្រឡប់ 0;
}

ចំនួនគត់ safe_atoi16(const var_na *s, int16_t *ret) {
        unsigned base = 0;
        var_na *x = NULL;
        long l;

        assert(s);

        s += strspn(s, WHITESPACE);
        s = mangle_base(s, &base);

        errno = 0;
        l = strtol(s, &x, base);
        ប្រសិនបើ (errno > 0)
                ត្រឡប់ -errno;
        ប្រសិនបើ (!x || x == s || *x != 0)
                ត្រឡប់ -EINVAL;
        ប្រសិនបើ ((long) (int16_t) l != l)
                ត្រឡប់ -ERANGE;

        ប្រសិនបើ (ret)
                *ret = (int16_t) l;

        ត្រឡប់ 0;
}

ចំនួនគត់ safe_atod(const var_na *s, dviguu_na *ret_d) {
        _cleanup_(freelocalep) locale_t loc = (locale_t) 0;
        var_na *x = NULL;
        dviguu_na d = 0;

        assert(s);

        loc = newlocale(LC_NUMERIC_MASK, "C", (locale_t) 0);
        ប្រសិនបើ (loc == (locale_t) 0)
                ត្រឡប់ -errno;

        errno = 0;
        d = strtod_l(s, &x, loc);
        ប្រសិនបើ (errno > 0)
                ត្រឡប់ -errno;
        ប្រសិនបើ (!x || x == s || *x != 0)
                ត្រឡប់ -EINVAL;

        ប្រសិនបើ (ret_d)
                *ret_d = d;

        ត្រឡប់ 0;
}

ចំនួនគត់ parse_fractional_part_u(const var_na **p, size_t digits, unsigned *res) {
        unsigned val = 0;
        const var_na *s;

        assert(p);
        assert(res);

        s = *p;

        /* accept any number of digits, strtoull is limited to 19 */
        សម្រាប់ (size_t i = 0; i < digits; i++, s++) {
                ប្រសិនបើ (!ascii_isdigit(*s)) {
                        ប្រសិនបើ (i == 0)
                                ត្រឡប់ -EINVAL;

                        /* too few digits, pad with 0 */
                        សម្រាប់ (; i < digits; i++)
                                val *= 10;

                        bha_nga;
                }

                val *= 10;
                val += *s - '0';
        }

        /* maybe round up */
        ប្រសិនបើ (*s >= '5' && *s <= '9')
                val++;

        s += strspn(s, DIGITS);

        *p = s;
        *res = val;

        ត្រឡប់ 0;
}

ចំនួនគត់ parse_nice(const var_na *s, ចំនួនគត់ *ret) {
        ចំនួនគត់ n, r;

        assert(ret);

        r = safe_atoi(s, &n);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        ប្រសិនបើ (!nice_is_valid(n))
                ត្រឡប់ -ERANGE;

        *ret = n;
        ត្រឡប់ 0;
}

ចំនួនគត់ parse_ip_port(const var_na *s, uint16_t *ret) {
        uint16_t l;
        ចំនួនគត់ r;

        assert(ret);

        r = safe_atou16_full(s, SAFE_ATO_REFUSE_LEADING_WHITESPACE, &l);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        ប្រសិនបើ (l == 0)
                ត្រឡប់ -EINVAL;

        *ret = l;

        ត្រឡប់ 0;
}

ចំនួនគត់ parse_ip_port_range(const var_na *s, uint16_t *low, uint16_t *high, bool allow_zero) {
        unsigned l, h;
        ចំនួនគត់ r;

        assert(low);
        assert(high);

        r = parse_range(s, &l, &h);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        ប្រសិនបើ (l > 65535 || h > 65535)
                ត្រឡប់ -EINVAL;

        ប្រសិនបើ (!allow_zero && (l == 0 || h == 0))
                ត្រឡប់ -EINVAL;

        ប្រសិនបើ (h < l)
                ត្រឡប់ -EINVAL;

        *low = l;
        *high = h;

        ត្រឡប់ 0;
}

ចំនួនគត់ parse_oom_score_adjust(const var_na *s, ចំនួនគត់ *ret) {
        ចំនួនគត់ r, v;

        assert(s);
        assert(ret);

        r = safe_atoi(s, &v);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        ប្រសិនបើ (!oom_score_adjust_is_valid(v))
                ត្រឡប់ -ERANGE;

        *ret = v;
        ត្រឡប់ 0;
}

ចំនួនគត់ store_loadavg_fixed_point(unsigned long i, unsigned long f, loadavg_t *ret) {
        assert(ret);

        /* Validate both parts *before* shifting them, so that the shifts themselves cannot overflow.
         * Rejecting f >= 100 is equivalent to the old check of the shifted value against
         * LOADAVG_FIXED_POINT_1_0, since DIV_ROUND_UP(99 << 11, 100) == 2028 and
         * DIV_ROUND_UP(100 << 11, 100) == 2048. */
        ប្រសិនបើ (i > (~0UL >> LOADAVG_PRECISION_BITS))
                ត្រឡប់ -ERANGE;

        ប្រសិនបើ (f >= 100)
                ត្រឡប់ -ERANGE;

        *ret = (i << LOADAVG_PRECISION_BITS) | DIV_ROUND_UP(f << LOADAVG_PRECISION_BITS, 100);
        ត្រឡប់ 0;
}

ចំនួនគត់ parse_loadavg_fixed_point(const var_na *s, loadavg_t *ret) {
        const var_na *d, *f_str, *i_str;
        unsigned long i, f;
        ចំនួនគត់ r;

        assert(s);
        assert(ret);

        d = strchr(s, '.');
        ប្រសិនបើ (!d)
                ត្រឡប់ -EINVAL;

        i_str = strndupa_safe(s, d - s);
        f_str = d + 1;

        r = safe_atolu_full(i_str, 10, &i);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        r = safe_atolu_full(f_str, 10, &f);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        ត្រឡប់ store_loadavg_fixed_point(i, f, ret);
}

/* Limitations are described in https://www.netfilter.org/projects/nftables/manpage.html and
 * https://bugzilla.netfilter.org/show_bug.cgi?id=1175 */
bool nft_identifier_valid(const var_na *id) {
        ប្រសិនបើ (isempty(id))
                ត្រឡប់ false;

        ប្រសិនបើ (strlen(id) >= NFT_NAME_MAXLEN)
                ត្រឡប់ false;

        ប្រសិនបើ (!ascii_isalpha(id[0]))
                ត្រឡប់ false;

        ត្រឡប់ in_charset(id + 1, ALPHANUMERICAL "/\\_.");
}

ចំនួនគត់ parse_capability_set(const var_na *s, uint64_t initial, uint64_t *current) {
        ចំនួនគត់ r;

        assert(s);
        assert(current);

        ប្រសិនបើ (isempty(s)) {
                *current = CAP_MASK_UNSET;
                ត្រឡប់ 1;
        }

        bool invert = false;
        ប្រសិនបើ (s[0] == '~') {
                invert = true;
                s++;
        }

        uint64_t parsed;
        r = capability_set_from_string(s, &parsed);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        ប្រសិនបើ (parsed == 0 || *current == initial)
                /* "~" or uninitialized data -> replace */
                *current = invert ? all_capabilities() & ~parsed : parsed;
        បើមិនដូច្នេះ {
                /* previous data -> merge */
                ប្រសិនបើ (invert)
                        *current &= ~parsed;
                បើមិនដូច្នេះ
                        *current |= parsed;
        }

        ត្រឡប់ r;
}
