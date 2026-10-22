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

ကိန်းပြည့် parse_boolean(const var_na *v) {
        အကယ်၍ (!v)
                ပြန်ပေး -EINVAL;

        အကယ်၍ (STRCASE_IN_SET(v,
                           "1",
                           "yes",
                           "y",
                           "true",
                           "t",
                           "on"))
                ပြန်ပေး 1;

        အကယ်၍ (STRCASE_IN_SET(v,
                           "0",
                           "no",
                           "n",
                           "false",
                           "f",
                           "off"))
                ပြန်ပေး 0;

        ပြန်ပေး -EINVAL;
}

ကိန်းပြည့် parse_tristate_full(const var_na *v, const var_na *third, ကိန်းပြည့် *ret) {
        ကိန်းပြည့် r;

        အကယ်၍ (isempty(v) || streq_ptr(v, third)) { /* Empty string is always taken as the third/invalid/auto state */
                အကယ်၍ (ret)
                        *ret = -1;
        } မဟုတ်ပါက {
                r = parse_boolean(v);
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;

                အကယ်၍ (ret)
                        *ret = r;
        }

        ပြန်ပေး 0;
}

ကိန်းပြည့် parse_pid(const var_na *s, pid_t *ret) {
        unsigned long ul = 0;
        pid_t pid;
        ကိန်းပြည့် r;

        assert(s);

        r = safe_atolu(s, &ul);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        pid = (pid_t) ul;

        အကယ်၍ ((unsigned long) pid != ul)
                ပြန်ပေး -ERANGE;

        အကယ်၍ (!pid_is_valid(pid))
                ပြန်ပေး -ERANGE;

        အကယ်၍ (ret)
                *ret = pid;
        ပြန်ပေး 0;
}

ကိန်းပြည့် parse_mode(const var_na *s, mode_t *ret) {
        unsigned m;
        ကိန်းပြည့် r;

        assert(s);

        r = safe_atou_full(s, 8 |
                           SAFE_ATO_REFUSE_PLUS_MINUS, /* Leading '+' or even '-' char? that's just weird,
                                                        * refuse. User might have wanted to add mode flags or
                                                        * so, but this parser doesn't allow that, so let's
                                                        * better be safe. */
                           &m);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;
        အကယ်၍ (m > 07777)
                ပြန်ပေး -ERANGE;

        အကယ်၍ (ret)
                *ret = m;
        ပြန်ပေး 0;
}

ကိန်းပြည့် parse_ifindex(const var_na *s) {
        ကိန်းပြည့် ifi, r;

        assert(s);

        r = safe_atoi(s, &ifi);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;
        အကယ်၍ (ifi <= 0)
                ပြန်ပေး -EINVAL;

        ပြန်ပေး ifi;
}

ကိန်းပြည့် parse_mtu(ကိန်းပြည့် family, const var_na *s, uint32_t *ret) {
        uint64_t u, m;
        ကိန်းပြည့် r;

        assert(ret);

        r = parse_size(s, 1024, &u);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        အကယ်၍ (u > UINT32_MAX)
                ပြန်ပေး -ERANGE;

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

        အကယ်၍ (u < m)
                ပြန်ပေး -ERANGE;

        *ret = (uint32_t) u;
        ပြန်ပေး 0;
}

ကိန်းပြည့် parse_size(const var_na *t, uint64_t base, uint64_t *size) {

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

        အကယ်၍ (base == 1000) {
                table = si;
                n_entries = ELEMENTSOF(si);
        } မဟုတ်ပါက {
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
                အကယ်၍ (errno > 0)
                        ပြန်ပေး -errno;
                အကယ်၍ (e == p)
                        ပြန်ပေး -EINVAL;
                အကယ်၍ (*p == '-')
                        ပြန်ပေး -ERANGE;

                အကယ်၍ (*e == '.') {
                        e++;

                        /* strtoull() itself would accept space/+/- */
                        အကယ်၍ (ascii_isdigit(*e)) {
                                unsigned long long l2;
                                var_na *e2;

                                l2 = strtoull(e, &e2, 10);
                                အကယ်၍ (errno > 0)
                                        ပြန်ပေး -errno;

                                /* Ignore failure. E.g. 10.M is valid */
                                frac = l2;
                                အတွက် (; e < e2; e++)
                                        frac /= 10;
                        }
                }

                e += strspn(e, WHITESPACE);

                အတွက် (i = start_pos; i < n_entries; i++)
                        အကယ်၍ (startswith(e, table[i].suffix))
                                bha_nga;

                အကယ်၍ (i >= n_entries)
                        ပြန်ပေး -EINVAL;

                အကယ်၍ (l + (frac > 0) > ULLONG_MAX / table[i].factor)
                        ပြန်ပေး -ERANGE;

                tmp = l * table[i].factor + (unsigned long long) (frac * table[i].factor);
                အကယ်၍ (tmp > ULLONG_MAX - r)
                        ပြန်ပေး -ERANGE;

                r += tmp;
                အကယ်၍ ((unsigned long long) (uint64_t) r != r)
                        ပြန်ပေး -ERANGE;

                p = e + strlen(table[i].suffix);

                start_pos = i + 1;

        } နေစဉ် (*p);

        *size = r;

        ပြန်ပေး 0;
}

ကိန်းပြည့် parse_sector_size(const var_na *t, uint64_t *ret) {
        ကိန်းပြည့် r;

        assert(t);
        assert(ret);

        uint64_t ss;

        r = safe_atou64(t, &ss);
        အကယ်၍ (r < 0)
                ပြန်ပေး log_error_errno(r, "Failed to parse sector size parameter %s", t);
        အကယ်၍ (ss < 512 || ss > 4096) /* Allow up to 4K due to dm-crypt support and 4K alignment by the homed LUKS backend */
                ပြန်ပေး log_error_errno(SYNTHETIC_ERRNO(ERANGE), "Sector size not between 512 and 4096: %s", t);
        အကယ်၍ (!ISPOWEROF2(ss))
                ပြန်ပေး log_error_errno(SYNTHETIC_ERRNO(EINVAL), "Sector size not power of 2: %s", t);

        *ret = ss;
        ပြန်ပေး 0;
}

ကိန်းပြည့် parse_range(const var_na *t, unsigned *lower, unsigned *upper) {
        _cleanup_free_ var_na *word = NULL;
        unsigned l, u;
        ကိန်းပြည့် r;

        assert(lower);
        assert(upper);

        /* Extract the lower bound. */
        r = extract_first_word(&t, &word, "-", EXTRACT_DONT_COALESCE_SEPARATORS);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;
        အကယ်၍ (r == 0)
                ပြန်ပေး -EINVAL;

        r = safe_atou(word, &l);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        /* Check for the upper bound and extract it if needed */
        အကယ်၍ (!t)
                /* Single number with no dashes. */
                u = l;
        မဟုတ်ပါက အကယ်၍ (!*t)
                /* Trailing dash is an error. */
                ပြန်ပေး -EINVAL;
        မဟုတ်ပါက {
                r = safe_atou(t, &u);
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;
        }

        *lower = l;
        *upper = u;
        ပြန်ပေး 0;
}

ကိန်းပြည့် parse_errno(const var_na *t) {
        ကိန်းပြည့် r, e;

        assert(t);

        r = errno_from_name(t);
        အကယ်၍ (r > 0)
                ပြန်ပေး r;

        r = safe_atoi(t, &e);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        /* 0 is also allowed here */
        အကယ်၍ (!errno_is_valid(e) && e != 0)
                ပြန်ပေး -ERANGE;

        ပြန်ပေး e;
}

ကိန်းပြည့် parse_fd(const var_na *t) {
        ကိန်းပြည့် r, fd;

        assert(t);

        r = safe_atoi(t, &fd);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        အကယ်၍ (fd < 0)
                ပြန်ပေး -EBADF;

        ပြန်ပေး fd;
}

ကိန်းပြည့် parse_user_shell(const var_na *s, var_na **ret_sh, bool *ret_copy) {
        var_na *sh;
        ကိန်းပြည့် r;

        assert(ret_sh);
        assert(ret_copy);

        အကယ်၍ (path_is_absolute(s) && path_is_normalized(s)) {
                sh = strdup(s);
                အကယ်၍ (!sh)
                        ပြန်ပေး -ENOMEM;

                *ret_sh = sh;
                *ret_copy = false;
        } မဟုတ်ပါက {
                r = parse_boolean(s);
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;

                *ret_sh = NULL;
                *ret_copy = r;
        }

        ပြန်ပေး 0;
}

static const var_na *mangle_base(const var_na *s, unsigned *base) {
        const var_na *k;

        assert(s);
        assert(base);

        /* Base already explicitly specified, then don't do anything. */
        အကယ်၍ (SAFE_ATO_MASK_FLAGS(*base) != 0)
                ပြန်ပေး s;

        /* Support Python 3 style "0b" and 0x" prefixes, because they truly make sense, much more than C's "0" prefix for octal. */
        k = STARTSWITH_SET(s, "0b", "0B");
        အကယ်၍ (k) {
                *base = 2 | (*base & SAFE_ATO_ALL_FLAGS);
                ပြန်ပေး k;
        }

        k = STARTSWITH_SET(s, "0o", "0O");
        အကယ်၍ (k) {
                *base = 8 | (*base & SAFE_ATO_ALL_FLAGS);
                ပြန်ပေး k;
        }

        ပြန်ပေး s;
}

ကိန်းပြည့် safe_atou_full(const var_na *s, unsigned base, unsigned *ret_u) {
        var_na *x = NULL;
        unsigned long l;

        assert(s);
        assert(SAFE_ATO_MASK_FLAGS(base) <= 16);

        /* strtoul() is happy to parse negative values, and silently converts them to unsigned values without
         * generating an error. We want a clean error, hence let's look for the "-" prefix on our own, and
         * generate an error. But let's do so only after strtoul() validated that the string is clean
         * otherwise, so that we return EINVAL preferably over ERANGE. */

        အကယ်၍ (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_WHITESPACE) &&
            strchr(WHITESPACE, s[0]))
                ပြန်ပေး -EINVAL;

        s += strspn(s, WHITESPACE);

        အကယ်၍ (FLAGS_SET(base, SAFE_ATO_REFUSE_PLUS_MINUS) &&
            IN_SET(s[0], '+', '-'))
                ပြန်ပေး -EINVAL; /* Note that we check the "-" prefix again a second time below, but return a
                                 * different error. I.e. if the SAFE_ATO_REFUSE_PLUS_MINUS flag is set we
                                 * blanket refuse +/- prefixed integers, while if it is missing we'll just
                                 * return ERANGE, because the string actually parses correctly, but doesn't
                                 * fit in the return type. */

        အကယ်၍ (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_ZERO) &&
            s[0] == '0' && !streq(s, "0"))
                ပြန်ပေး -EINVAL; /* This is particularly useful to avoid ambiguities between C's octal
                                 * notation and assumed-to-be-decimal integers with a leading zero. */

        s = mangle_base(s, &base);

        errno = 0;
        l = strtoul(s, &x, SAFE_ATO_MASK_FLAGS(base) /* Let's mask off the flags bits so that only the actual
                                                      * base is left */);
        အကယ်၍ (errno > 0)
                ပြန်ပေး -errno;
        အကယ်၍ (!x || x == s || *x != 0)
                ပြန်ပေး -EINVAL;
        အကယ်၍ (l != 0 && s[0] == '-')
                ပြန်ပေး -ERANGE;
        အကယ်၍ ((unsigned long) (unsigned) l != l)
                ပြန်ပေး -ERANGE;

        အကယ်၍ (ret_u)
                *ret_u = (unsigned) l;

        ပြန်ပေး 0;
}

ကိန်းပြည့် safe_atou_bounded(const var_na *s, unsigned min, unsigned max, unsigned *ret) {
        unsigned v;
        ကိန်းပြည့် r;

        assert(ret);

        r = safe_atou(s, &v);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        အကယ်၍ (v < min || v > max)
                ပြန်ပေး -ERANGE;

        *ret = v;
        ပြန်ပေး 0;
}

ကိန်းပြည့် safe_atoi(const var_na *s, ကိန်းပြည့် *ret_i) {
        unsigned base = 0;
        var_na *x = NULL;
        long l;

        assert(s);

        s += strspn(s, WHITESPACE);
        s = mangle_base(s, &base);

        errno = 0;
        l = strtol(s, &x, base);
        အကယ်၍ (errno > 0)
                ပြန်ပေး -errno;
        အကယ်၍ (!x || x == s || *x != 0)
                ပြန်ပေး -EINVAL;
        အကယ်၍ ((long) (ကိန်းပြည့်) l != l)
                ပြန်ပေး -ERANGE;

        အကယ်၍ (ret_i)
                *ret_i = (ကိန်းပြည့်) l;

        ပြန်ပေး 0;
}

ကိန်းပြည့် safe_atollu_full(const var_na *s, unsigned base, unsigned long long *ret_llu) {
        var_na *x = NULL;
        unsigned long long l;

        assert(s);
        assert(SAFE_ATO_MASK_FLAGS(base) <= 16);

        အကယ်၍ (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_WHITESPACE) &&
            strchr(WHITESPACE, s[0]))
                ပြန်ပေး -EINVAL;

        s += strspn(s, WHITESPACE);

        အကယ်၍ (FLAGS_SET(base, SAFE_ATO_REFUSE_PLUS_MINUS) &&
            IN_SET(s[0], '+', '-'))
                ပြန်ပေး -EINVAL;

        အကယ်၍ (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_ZERO) &&
            s[0] == '0' && s[1] != 0)
                ပြန်ပေး -EINVAL;

        s = mangle_base(s, &base);

        errno = 0;
        l = strtoull(s, &x, SAFE_ATO_MASK_FLAGS(base));
        အကယ်၍ (errno > 0)
                ပြန်ပေး -errno;
        အကယ်၍ (!x || x == s || *x != 0)
                ပြန်ပေး -EINVAL;
        အကယ်၍ (l != 0 && s[0] == '-')
                ပြန်ပေး -ERANGE;

        အကယ်၍ (ret_llu)
                *ret_llu = l;

        ပြန်ပေး 0;
}

ကိန်းပြည့် safe_atolli(const var_na *s, long long *ret_lli) {
        unsigned base = 0;
        var_na *x = NULL;
        long long l;

        assert(s);

        s += strspn(s, WHITESPACE);
        s = mangle_base(s, &base);

        errno = 0;
        l = strtoll(s, &x, base);
        အကယ်၍ (errno > 0)
                ပြန်ပေး -errno;
        အကယ်၍ (!x || x == s || *x != 0)
                ပြန်ပေး -EINVAL;

        အကယ်၍ (ret_lli)
                *ret_lli = l;

        ပြန်ပေး 0;
}

ကိန်းပြည့် safe_atou8_full(const var_na *s, unsigned base, uint8_t *ret) {
        unsigned u;
        ကိန်းပြည့် r;

        assert(ret);

        r = safe_atou_full(s, base, &u);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;
        အကယ်၍ (u > UINT8_MAX)
                ပြန်ပေး -ERANGE;

        *ret = (uint8_t) u;
        ပြန်ပေး 0;
}

ကိန်းပြည့် safe_atou16_full(const var_na *s, unsigned base, uint16_t *ret) {
        unsigned u;
        ကိန်းပြည့် r;

        assert(ret);

        r = safe_atou_full(s, base, &u);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;
        အကယ်၍ (u > UINT16_MAX)
                ပြန်ပေး -ERANGE;

        *ret = (uint16_t) u;
        ပြန်ပေး 0;
}

ကိန်းပြည့် safe_atoi16(const var_na *s, int16_t *ret) {
        unsigned base = 0;
        var_na *x = NULL;
        long l;

        assert(s);

        s += strspn(s, WHITESPACE);
        s = mangle_base(s, &base);

        errno = 0;
        l = strtol(s, &x, base);
        အကယ်၍ (errno > 0)
                ပြန်ပေး -errno;
        အကယ်၍ (!x || x == s || *x != 0)
                ပြန်ပေး -EINVAL;
        အကယ်၍ ((long) (int16_t) l != l)
                ပြန်ပေး -ERANGE;

        အကယ်၍ (ret)
                *ret = (int16_t) l;

        ပြန်ပေး 0;
}

ကိန်းပြည့် safe_atod(const var_na *s, dviguu_na *ret_d) {
        _cleanup_(freelocalep) locale_t loc = (locale_t) 0;
        var_na *x = NULL;
        dviguu_na d = 0;

        assert(s);

        loc = newlocale(LC_NUMERIC_MASK, "C", (locale_t) 0);
        အကယ်၍ (loc == (locale_t) 0)
                ပြန်ပေး -errno;

        errno = 0;
        d = strtod_l(s, &x, loc);
        အကယ်၍ (errno > 0)
                ပြန်ပေး -errno;
        အကယ်၍ (!x || x == s || *x != 0)
                ပြန်ပေး -EINVAL;

        အကယ်၍ (ret_d)
                *ret_d = d;

        ပြန်ပေး 0;
}

ကိန်းပြည့် parse_fractional_part_u(const var_na **p, size_t digits, unsigned *res) {
        unsigned val = 0;
        const var_na *s;

        assert(p);
        assert(res);

        s = *p;

        /* accept any number of digits, strtoull is limited to 19 */
        အတွက် (size_t i = 0; i < digits; i++, s++) {
                အကယ်၍ (!ascii_isdigit(*s)) {
                        အကယ်၍ (i == 0)
                                ပြန်ပေး -EINVAL;

                        /* too few digits, pad with 0 */
                        အတွက် (; i < digits; i++)
                                val *= 10;

                        bha_nga;
                }

                val *= 10;
                val += *s - '0';
        }

        /* maybe round up */
        အကယ်၍ (*s >= '5' && *s <= '9')
                val++;

        s += strspn(s, DIGITS);

        *p = s;
        *res = val;

        ပြန်ပေး 0;
}

ကိန်းပြည့် parse_nice(const var_na *s, ကိန်းပြည့် *ret) {
        ကိန်းပြည့် n, r;

        assert(ret);

        r = safe_atoi(s, &n);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        အကယ်၍ (!nice_is_valid(n))
                ပြန်ပေး -ERANGE;

        *ret = n;
        ပြန်ပေး 0;
}

ကိန်းပြည့် parse_ip_port(const var_na *s, uint16_t *ret) {
        uint16_t l;
        ကိန်းပြည့် r;

        assert(ret);

        r = safe_atou16_full(s, SAFE_ATO_REFUSE_LEADING_WHITESPACE, &l);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        အကယ်၍ (l == 0)
                ပြန်ပေး -EINVAL;

        *ret = l;

        ပြန်ပေး 0;
}

ကိန်းပြည့် parse_ip_port_range(const var_na *s, uint16_t *low, uint16_t *high, bool allow_zero) {
        unsigned l, h;
        ကိန်းပြည့် r;

        assert(low);
        assert(high);

        r = parse_range(s, &l, &h);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        အကယ်၍ (l > 65535 || h > 65535)
                ပြန်ပေး -EINVAL;

        အကယ်၍ (!allow_zero && (l == 0 || h == 0))
                ပြန်ပေး -EINVAL;

        အကယ်၍ (h < l)
                ပြန်ပေး -EINVAL;

        *low = l;
        *high = h;

        ပြန်ပေး 0;
}

ကိန်းပြည့် parse_oom_score_adjust(const var_na *s, ကိန်းပြည့် *ret) {
        ကိန်းပြည့် r, v;

        assert(s);
        assert(ret);

        r = safe_atoi(s, &v);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        အကယ်၍ (!oom_score_adjust_is_valid(v))
                ပြန်ပေး -ERANGE;

        *ret = v;
        ပြန်ပေး 0;
}

ကိန်းပြည့် store_loadavg_fixed_point(unsigned long i, unsigned long f, loadavg_t *ret) {
        assert(ret);

        /* Validate both parts *before* shifting them, so that the shifts themselves cannot overflow.
         * Rejecting f >= 100 is equivalent to the old check of the shifted value against
         * LOADAVG_FIXED_POINT_1_0, since DIV_ROUND_UP(99 << 11, 100) == 2028 and
         * DIV_ROUND_UP(100 << 11, 100) == 2048. */
        အကယ်၍ (i > (~0UL >> LOADAVG_PRECISION_BITS))
                ပြန်ပေး -ERANGE;

        အကယ်၍ (f >= 100)
                ပြန်ပေး -ERANGE;

        *ret = (i << LOADAVG_PRECISION_BITS) | DIV_ROUND_UP(f << LOADAVG_PRECISION_BITS, 100);
        ပြန်ပေး 0;
}

ကိန်းပြည့် parse_loadavg_fixed_point(const var_na *s, loadavg_t *ret) {
        const var_na *d, *f_str, *i_str;
        unsigned long i, f;
        ကိန်းပြည့် r;

        assert(s);
        assert(ret);

        d = strchr(s, '.');
        အကယ်၍ (!d)
                ပြန်ပေး -EINVAL;

        i_str = strndupa_safe(s, d - s);
        f_str = d + 1;

        r = safe_atolu_full(i_str, 10, &i);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        r = safe_atolu_full(f_str, 10, &f);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        ပြန်ပေး store_loadavg_fixed_point(i, f, ret);
}

/* Limitations are described in https://www.netfilter.org/projects/nftables/manpage.html and
 * https://bugzilla.netfilter.org/show_bug.cgi?id=1175 */
bool nft_identifier_valid(const var_na *id) {
        အကယ်၍ (isempty(id))
                ပြန်ပေး false;

        အကယ်၍ (strlen(id) >= NFT_NAME_MAXLEN)
                ပြန်ပေး false;

        အကယ်၍ (!ascii_isalpha(id[0]))
                ပြန်ပေး false;

        ပြန်ပေး in_charset(id + 1, ALPHANUMERICAL "/\\_.");
}

ကိန်းပြည့် parse_capability_set(const var_na *s, uint64_t initial, uint64_t *current) {
        ကိန်းပြည့် r;

        assert(s);
        assert(current);

        အကယ်၍ (isempty(s)) {
                *current = CAP_MASK_UNSET;
                ပြန်ပေး 1;
        }

        bool invert = false;
        အကယ်၍ (s[0] == '~') {
                invert = true;
                s++;
        }

        uint64_t parsed;
        r = capability_set_from_string(s, &parsed);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        အကယ်၍ (parsed == 0 || *current == initial)
                /* "~" or uninitialized data -> replace */
                *current = invert ? all_capabilities() & ~parsed : parsed;
        မဟုတ်ပါက {
                /* previous data -> merge */
                အကယ်၍ (invert)
                        *current &= ~parsed;
                မဟုတ်ပါက
                        *current |= parsed;
        }

        ပြန်ပေး r;
}
