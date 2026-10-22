/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <linux/ipv6.h>
#ಅನ್ತರ್ಭಾವ <linux/netfilter/nf_tables.h>
#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <stdlib.h>
#ಅನ್ತರ್ಭಾವ <sys/socket.h>

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "capability-list.h"
#ಅನ್ತರ್ಭಾವ "capability-util.h"
#ಅನ್ತರ್ಭಾವ "errno-list.h"
#ಅನ್ತರ್ಭಾವ "extract-word.h"
#ಅನ್ತರ್ಭಾವ "locale-util.h"
#ಅನ್ತರ್ಭಾವ "log.h"
#ಅನ್ತರ್ಭಾವ "missing-network.h"
#ಅನ್ತರ್ಭಾವ "parse-util.h"
#ಅನ್ತರ್ಭಾವ "path-util.h"
#ಅನ್ತರ್ಭಾವ "process-util.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"
#ಅನ್ತರ್ಭಾವ "strv.h"

ಪೂರ್ಣಾಙ್ಕ parse_boolean(ಧ್ರುವ ವರ್ಣ *v) {
        ಯದಿ (!v)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        ಯದಿ (STRCASE_IN_SET(v,
                           "1",
                           "yes",
                           "y",
                           "true",
                           "t",
                           "on"))
                ಪ್ರತ್ಯಾಗಮ 1;

        ಯದಿ (STRCASE_IN_SET(v,
                           "0",
                           "no",
                           "n",
                           "false",
                           "f",
                           "off"))
                ಪ್ರತ್ಯಾಗಮ 0;

        ಪ್ರತ್ಯಾಗಮ -EINVAL;
}

ಪೂರ್ಣಾಙ್ಕ parse_tristate_full(ಧ್ರುವ ವರ್ಣ *v, ಧ್ರುವ ವರ್ಣ *third, ಪೂರ್ಣಾಙ್ಕ *ret) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಯದಿ (isempty(v) || streq_ptr(v, third)) { /* Empty string is always taken as the third/invalid/auto state */
                ಯದಿ (ret)
                        *ret = -1;
        } ಅನ್ಯಥಾ {
                r = parse_boolean(v);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                ಯದಿ (ret)
                        *ret = r;
        }

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ parse_pid(ಧ್ರುವ ವರ್ಣ *s, pid_t *ret) {
        unsigned long ul = 0;
        pid_t pid;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(s);

        r = safe_atolu(s, &ul);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        pid = (pid_t) ul;

        ಯದಿ ((unsigned long) pid != ul)
                ಪ್ರತ್ಯಾಗಮ -ERANGE;

        ಯದಿ (!pid_is_valid(pid))
                ಪ್ರತ್ಯಾಗಮ -ERANGE;

        ಯದಿ (ret)
                *ret = pid;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ parse_mode(ಧ್ರುವ ವರ್ಣ *s, mode_t *ret) {
        unsigned m;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(s);

        r = safe_atou_full(s, 8 |
                           SAFE_ATO_REFUSE_PLUS_MINUS, /* Leading '+' or even '-' char? that's just weird,
                                                        * refuse. User might have wanted to add mode flags or
                                                        * so, but this parser doesn't allow that, so let's
                                                        * better be safe. */
                           &m);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;
        ಯದಿ (m > 07777)
                ಪ್ರತ್ಯಾಗಮ -ERANGE;

        ಯದಿ (ret)
                *ret = m;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ parse_ifindex(ಧ್ರುವ ವರ್ಣ *s) {
        ಪೂರ್ಣಾಙ್ಕ ifi, r;

        ಪ್ರತಿಜ್ಞಾ(s);

        r = safe_atoi(s, &ifi);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;
        ಯದಿ (ifi <= 0)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        ಪ್ರತ್ಯಾಗಮ ifi;
}

ಪೂರ್ಣಾಙ್ಕ parse_mtu(ಪೂರ್ಣಾಙ್ಕ family, ಧ್ರುವ ವರ್ಣ *s, uint32_t *ret) {
        uint64_t u, m;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        r = parse_size(s, 1024, &u);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (u > UINT32_MAX)
                ಪ್ರತ್ಯಾಗಮ -ERANGE;

        ವಿಕಲ್ಪ (family) {
        ಪಕ್ಷ AF_INET:
                m = IPV4_MIN_MTU; /* This is 68 */
                ಭಙ್ಗ;
        ಪಕ್ಷ AF_INET6:
                m = IPV6_MIN_MTU; /* This is 1280 */
                ಭಙ್ಗ;
        ಸ್ವಭಾವ:
                m = 0;
        }

        ಯದಿ (u < m)
                ಪ್ರತ್ಯಾಗಮ -ERANGE;

        *ret = (uint32_t) u;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ parse_size(ಧ್ರುವ ವರ್ಣ *t, uint64_t base, uint64_t *size) {

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

        ಸಂರಚನಾ table {
                ಧ್ರುವ ವರ್ಣ *suffix;
                unsigned long long factor;
        };

        ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ table iec[] = {
                { "E", 1024ULL*1024ULL*1024ULL*1024ULL*1024ULL*1024ULL },
                { "P", 1024ULL*1024ULL*1024ULL*1024ULL*1024ULL },
                { "T", 1024ULL*1024ULL*1024ULL*1024ULL },
                { "G", 1024ULL*1024ULL*1024ULL },
                { "M", 1024ULL*1024ULL },
                { "K", 1024ULL },
                { "B", 1ULL },
                { "",  1ULL },
        };

        ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ table si[] = {
                { "E", 1000ULL*1000ULL*1000ULL*1000ULL*1000ULL*1000ULL },
                { "P", 1000ULL*1000ULL*1000ULL*1000ULL*1000ULL },
                { "T", 1000ULL*1000ULL*1000ULL*1000ULL },
                { "G", 1000ULL*1000ULL*1000ULL },
                { "M", 1000ULL*1000ULL },
                { "K", 1000ULL },
                { "B", 1ULL },
                { "",  1ULL },
        };

        ಧ್ರುವ ಸಂರಚನಾ table *table;
        ಧ್ರುವ ವರ್ಣ *p;
        unsigned long long r = 0;
        unsigned n_entries, start_pos = 0;

        ಪ್ರತಿಜ್ಞಾ(t);
        ಪ್ರತಿಜ್ಞಾ(IN_SET(base, 1000, 1024));
        ಪ್ರತಿಜ್ಞಾ(size);

        ಯದಿ (base == 1000) {
                table = si;
                n_entries = ELEMENTSOF(si);
        } ಅನ್ಯಥಾ {
                table = iec;
                n_entries = ELEMENTSOF(iec);
        }

        p = t;
        ಕುರು {
                unsigned long long l, tmp;
                ದ್ವಿಗುಣ frac = 0;
                ವರ್ಣ *e;
                unsigned i;

                p += strspn(p, WHITESPACE);

                errno = 0;
                l = strtoull(p, &e, 10);
                ಯದಿ (errno > 0)
                        ಪ್ರತ್ಯಾಗಮ -errno;
                ಯದಿ (e == p)
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;
                ಯದಿ (*p == '-')
                        ಪ್ರತ್ಯಾಗಮ -ERANGE;

                ಯದಿ (*e == '.') {
                        e++;

                        /* strtoull() itself would accept space/+/- */
                        ಯದಿ (ascii_isdigit(*e)) {
                                unsigned long long l2;
                                ವರ್ಣ *e2;

                                l2 = strtoull(e, &e2, 10);
                                ಯದಿ (errno > 0)
                                        ಪ್ರತ್ಯಾಗಮ -errno;

                                /* Ignore failure. E.g. 10.M is valid */
                                frac = l2;
                                ಪ್ರತಿ (; e < e2; e++)
                                        frac /= 10;
                        }
                }

                e += strspn(e, WHITESPACE);

                ಪ್ರತಿ (i = start_pos; i < n_entries; i++)
                        ಯದಿ (startswith(e, table[i].suffix))
                                ಭಙ್ಗ;

                ಯದಿ (i >= n_entries)
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                ಯದಿ (l + (frac > 0) > ULLONG_MAX / table[i].factor)
                        ಪ್ರತ್ಯಾಗಮ -ERANGE;

                tmp = l * table[i].factor + (unsigned long long) (frac * table[i].factor);
                ಯದಿ (tmp > ULLONG_MAX - r)
                        ಪ್ರತ್ಯಾಗಮ -ERANGE;

                r += tmp;
                ಯದಿ ((unsigned long long) (uint64_t) r != r)
                        ಪ್ರತ್ಯಾಗಮ -ERANGE;

                p = e + strlen(table[i].suffix);

                start_pos = i + 1;

        } ಯಾವತ್ (*p);

        *size = r;

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ parse_sector_size(ಧ್ರುವ ವರ್ಣ *t, uint64_t *ret) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(t);
        ಪ್ರತಿಜ್ಞಾ(ret);

        uint64_t ss;

        r = safe_atou64(t, &ss);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ log_error_errno(r, "Failed to parse sector size parameter %s", t);
        ಯದಿ (ss < 512 || ss > 4096) /* Allow up to 4K due to dm-crypt support and 4K alignment by the homed LUKS backend */
                ಪ್ರತ್ಯಾಗಮ log_error_errno(SYNTHETIC_ERRNO(ERANGE), "Sector size not between 512 and 4096: %s", t);
        ಯದಿ (!ISPOWEROF2(ss))
                ಪ್ರತ್ಯಾಗಮ log_error_errno(SYNTHETIC_ERRNO(EINVAL), "Sector size not power of 2: %s", t);

        *ret = ss;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ parse_range(ಧ್ರುವ ವರ್ಣ *t, unsigned *lower, unsigned *upper) {
        _cleanup_free_ ವರ್ಣ *word = ರಿಕ್ತ;
        unsigned l, u;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(lower);
        ಪ್ರತಿಜ್ಞಾ(upper);

        /* Extract the lower bound. */
        r = extract_first_word(&t, &word, "-", EXTRACT_DONT_COALESCE_SEPARATORS);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;
        ಯದಿ (r == 0)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        r = safe_atou(word, &l);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        /* Check for the upper bound and extract it if needed */
        ಯದಿ (!t)
                /* Single number with no dashes. */
                u = l;
        ಅನ್ಯಥಾ ಯದಿ (!*t)
                /* Trailing dash is an error. */
                ಪ್ರತ್ಯಾಗಮ -EINVAL;
        ಅನ್ಯಥಾ {
                r = safe_atou(t, &u);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
        }

        *lower = l;
        *upper = u;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ parse_errno(ಧ್ರುವ ವರ್ಣ *t) {
        ಪೂರ್ಣಾಙ್ಕ r, e;

        ಪ್ರತಿಜ್ಞಾ(t);

        r = errno_from_name(t);
        ಯದಿ (r > 0)
                ಪ್ರತ್ಯಾಗಮ r;

        r = safe_atoi(t, &e);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        /* 0 is also allowed here */
        ಯದಿ (!errno_is_valid(e) && e != 0)
                ಪ್ರತ್ಯಾಗಮ -ERANGE;

        ಪ್ರತ್ಯಾಗಮ e;
}

ಪೂರ್ಣಾಙ್ಕ parse_fd(ಧ್ರುವ ವರ್ಣ *t) {
        ಪೂರ್ಣಾಙ್ಕ r, fd;

        ಪ್ರತಿಜ್ಞಾ(t);

        r = safe_atoi(t, &fd);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (fd < 0)
                ಪ್ರತ್ಯಾಗಮ -EBADF;

        ಪ್ರತ್ಯಾಗಮ fd;
}

ಪೂರ್ಣಾಙ್ಕ parse_user_shell(ಧ್ರುವ ವರ್ಣ *s, ವರ್ಣ **ret_sh, bool *ret_copy) {
        ವರ್ಣ *sh;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret_sh);
        ಪ್ರತಿಜ್ಞಾ(ret_copy);

        ಯದಿ (path_is_absolute(s) && path_is_normalized(s)) {
                sh = strdup(s);
                ಯದಿ (!sh)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                *ret_sh = sh;
                *ret_copy = false;
        } ಅನ್ಯಥಾ {
                r = parse_boolean(s);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                *ret_sh = ರಿಕ್ತ;
                *ret_copy = r;
        }

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ *mangle_base(ಧ್ರುವ ವರ್ಣ *s, unsigned *base) {
        ಧ್ರುವ ವರ್ಣ *k;

        ಪ್ರತಿಜ್ಞಾ(s);
        ಪ್ರತಿಜ್ಞಾ(base);

        /* Base already explicitly specified, then don't do anything. */
        ಯದಿ (SAFE_ATO_MASK_FLAGS(*base) != 0)
                ಪ್ರತ್ಯಾಗಮ s;

        /* Support Python 3 style "0b" and 0x" prefixes, because they truly make sense, much more than C's "0" prefix for octal. */
        k = STARTSWITH_SET(s, "0b", "0B");
        ಯದಿ (k) {
                *base = 2 | (*base & SAFE_ATO_ALL_FLAGS);
                ಪ್ರತ್ಯಾಗಮ k;
        }

        k = STARTSWITH_SET(s, "0o", "0O");
        ಯದಿ (k) {
                *base = 8 | (*base & SAFE_ATO_ALL_FLAGS);
                ಪ್ರತ್ಯಾಗಮ k;
        }

        ಪ್ರತ್ಯಾಗಮ s;
}

ಪೂರ್ಣಾಙ್ಕ safe_atou_full(ಧ್ರುವ ವರ್ಣ *s, unsigned base, unsigned *ret_u) {
        ವರ್ಣ *x = ರಿಕ್ತ;
        unsigned long l;

        ಪ್ರತಿಜ್ಞಾ(s);
        ಪ್ರತಿಜ್ಞಾ(SAFE_ATO_MASK_FLAGS(base) <= 16);

        /* strtoul() is happy to parse negative values, and silently converts them to unsigned values without
         * generating an error. We want a clean error, hence let's look for the "-" prefix on our own, and
         * generate an error. But let's do so only after strtoul() validated that the string is clean
         * otherwise, so that we return EINVAL preferably over ERANGE. */

        ಯದಿ (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_WHITESPACE) &&
            strchr(WHITESPACE, s[0]))
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        s += strspn(s, WHITESPACE);

        ಯದಿ (FLAGS_SET(base, SAFE_ATO_REFUSE_PLUS_MINUS) &&
            IN_SET(s[0], '+', '-'))
                ಪ್ರತ್ಯಾಗಮ -EINVAL; /* Note that we check the "-" prefix again a second time below, but return a
                                 * different error. I.e. if the SAFE_ATO_REFUSE_PLUS_MINUS flag is set we
                                 * blanket refuse +/- prefixed integers, while if it is missing we'll just
                                 * return ERANGE, because the string actually parses correctly, but doesn't
                                 * fit in the return type. */

        ಯದಿ (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_ZERO) &&
            s[0] == '0' && !streq(s, "0"))
                ಪ್ರತ್ಯಾಗಮ -EINVAL; /* This is particularly useful to avoid ambiguities between C's octal
                                 * notation and assumed-to-be-decimal integers with a leading zero. */

        s = mangle_base(s, &base);

        errno = 0;
        l = strtoul(s, &x, SAFE_ATO_MASK_FLAGS(base) /* Let's mask off the flags bits so that only the actual
                                                      * base is left */);
        ಯದಿ (errno > 0)
                ಪ್ರತ್ಯಾಗಮ -errno;
        ಯದಿ (!x || x == s || *x != 0)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;
        ಯದಿ (l != 0 && s[0] == '-')
                ಪ್ರತ್ಯಾಗಮ -ERANGE;
        ಯದಿ ((unsigned long) (unsigned) l != l)
                ಪ್ರತ್ಯಾಗಮ -ERANGE;

        ಯದಿ (ret_u)
                *ret_u = (unsigned) l;

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ safe_atou_bounded(ಧ್ರುವ ವರ್ಣ *s, unsigned min, unsigned max, unsigned *ret) {
        unsigned v;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        r = safe_atou(s, &v);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (v < min || v > max)
                ಪ್ರತ್ಯಾಗಮ -ERANGE;

        *ret = v;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ safe_atoi(ಧ್ರುವ ವರ್ಣ *s, ಪೂರ್ಣಾಙ್ಕ *ret_i) {
        unsigned base = 0;
        ವರ್ಣ *x = ರಿಕ್ತ;
        long l;

        ಪ್ರತಿಜ್ಞಾ(s);

        s += strspn(s, WHITESPACE);
        s = mangle_base(s, &base);

        errno = 0;
        l = strtol(s, &x, base);
        ಯದಿ (errno > 0)
                ಪ್ರತ್ಯಾಗಮ -errno;
        ಯದಿ (!x || x == s || *x != 0)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;
        ಯದಿ ((long) (ಪೂರ್ಣಾಙ್ಕ) l != l)
                ಪ್ರತ್ಯಾಗಮ -ERANGE;

        ಯದಿ (ret_i)
                *ret_i = (ಪೂರ್ಣಾಙ್ಕ) l;

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ safe_atollu_full(ಧ್ರುವ ವರ್ಣ *s, unsigned base, unsigned long long *ret_llu) {
        ವರ್ಣ *x = ರಿಕ್ತ;
        unsigned long long l;

        ಪ್ರತಿಜ್ಞಾ(s);
        ಪ್ರತಿಜ್ಞಾ(SAFE_ATO_MASK_FLAGS(base) <= 16);

        ಯದಿ (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_WHITESPACE) &&
            strchr(WHITESPACE, s[0]))
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        s += strspn(s, WHITESPACE);

        ಯದಿ (FLAGS_SET(base, SAFE_ATO_REFUSE_PLUS_MINUS) &&
            IN_SET(s[0], '+', '-'))
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        ಯದಿ (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_ZERO) &&
            s[0] == '0' && s[1] != 0)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        s = mangle_base(s, &base);

        errno = 0;
        l = strtoull(s, &x, SAFE_ATO_MASK_FLAGS(base));
        ಯದಿ (errno > 0)
                ಪ್ರತ್ಯಾಗಮ -errno;
        ಯದಿ (!x || x == s || *x != 0)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;
        ಯದಿ (l != 0 && s[0] == '-')
                ಪ್ರತ್ಯಾಗಮ -ERANGE;

        ಯದಿ (ret_llu)
                *ret_llu = l;

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ safe_atolli(ಧ್ರುವ ವರ್ಣ *s, long long *ret_lli) {
        unsigned base = 0;
        ವರ್ಣ *x = ರಿಕ್ತ;
        long long l;

        ಪ್ರತಿಜ್ಞಾ(s);

        s += strspn(s, WHITESPACE);
        s = mangle_base(s, &base);

        errno = 0;
        l = strtoll(s, &x, base);
        ಯದಿ (errno > 0)
                ಪ್ರತ್ಯಾಗಮ -errno;
        ಯದಿ (!x || x == s || *x != 0)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        ಯದಿ (ret_lli)
                *ret_lli = l;

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ safe_atou8_full(ಧ್ರುವ ವರ್ಣ *s, unsigned base, uint8_t *ret) {
        unsigned u;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        r = safe_atou_full(s, base, &u);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;
        ಯದಿ (u > UINT8_MAX)
                ಪ್ರತ್ಯಾಗಮ -ERANGE;

        *ret = (uint8_t) u;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ safe_atou16_full(ಧ್ರುವ ವರ್ಣ *s, unsigned base, uint16_t *ret) {
        unsigned u;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        r = safe_atou_full(s, base, &u);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;
        ಯದಿ (u > UINT16_MAX)
                ಪ್ರತ್ಯಾಗಮ -ERANGE;

        *ret = (uint16_t) u;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ safe_atoi16(ಧ್ರುವ ವರ್ಣ *s, int16_t *ret) {
        unsigned base = 0;
        ವರ್ಣ *x = ರಿಕ್ತ;
        long l;

        ಪ್ರತಿಜ್ಞಾ(s);

        s += strspn(s, WHITESPACE);
        s = mangle_base(s, &base);

        errno = 0;
        l = strtol(s, &x, base);
        ಯದಿ (errno > 0)
                ಪ್ರತ್ಯಾಗಮ -errno;
        ಯದಿ (!x || x == s || *x != 0)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;
        ಯದಿ ((long) (int16_t) l != l)
                ಪ್ರತ್ಯಾಗಮ -ERANGE;

        ಯದಿ (ret)
                *ret = (int16_t) l;

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ safe_atod(ಧ್ರುವ ವರ್ಣ *s, ದ್ವಿಗುಣ *ret_d) {
        _cleanup_(freelocalep) locale_t loc = (locale_t) 0;
        ವರ್ಣ *x = ರಿಕ್ತ;
        ದ್ವಿಗುಣ d = 0;

        ಪ್ರತಿಜ್ಞಾ(s);

        loc = newlocale(LC_NUMERIC_MASK, "C", (locale_t) 0);
        ಯದಿ (loc == (locale_t) 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        errno = 0;
        d = strtod_l(s, &x, loc);
        ಯದಿ (errno > 0)
                ಪ್ರತ್ಯಾಗಮ -errno;
        ಯದಿ (!x || x == s || *x != 0)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        ಯದಿ (ret_d)
                *ret_d = d;

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ parse_fractional_part_u(ಧ್ರುವ ವರ್ಣ **p, size_t digits, unsigned *res) {
        unsigned val = 0;
        ಧ್ರುವ ವರ್ಣ *s;

        ಪ್ರತಿಜ್ಞಾ(p);
        ಪ್ರತಿಜ್ಞಾ(res);

        s = *p;

        /* accept any number of digits, strtoull is limited to 19 */
        ಪ್ರತಿ (size_t i = 0; i < digits; i++, s++) {
                ಯದಿ (!ascii_isdigit(*s)) {
                        ಯದಿ (i == 0)
                                ಪ್ರತ್ಯಾಗಮ -EINVAL;

                        /* too few digits, pad with 0 */
                        ಪ್ರತಿ (; i < digits; i++)
                                val *= 10;

                        ಭಙ್ಗ;
                }

                val *= 10;
                val += *s - '0';
        }

        /* maybe round up */
        ಯದಿ (*s >= '5' && *s <= '9')
                val++;

        s += strspn(s, DIGITS);

        *p = s;
        *res = val;

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ parse_nice(ಧ್ರುವ ವರ್ಣ *s, ಪೂರ್ಣಾಙ್ಕ *ret) {
        ಪೂರ್ಣಾಙ್ಕ n, r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        r = safe_atoi(s, &n);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (!nice_is_valid(n))
                ಪ್ರತ್ಯಾಗಮ -ERANGE;

        *ret = n;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ parse_ip_port(ಧ್ರುವ ವರ್ಣ *s, uint16_t *ret) {
        uint16_t l;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        r = safe_atou16_full(s, SAFE_ATO_REFUSE_LEADING_WHITESPACE, &l);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (l == 0)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        *ret = l;

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ parse_ip_port_range(ಧ್ರುವ ವರ್ಣ *s, uint16_t *low, uint16_t *high, bool allow_zero) {
        unsigned l, h;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(low);
        ಪ್ರತಿಜ್ಞಾ(high);

        r = parse_range(s, &l, &h);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (l > 65535 || h > 65535)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        ಯದಿ (!allow_zero && (l == 0 || h == 0))
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        ಯದಿ (h < l)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        *low = l;
        *high = h;

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ parse_oom_score_adjust(ಧ್ರುವ ವರ್ಣ *s, ಪೂರ್ಣಾಙ್ಕ *ret) {
        ಪೂರ್ಣಾಙ್ಕ r, v;

        ಪ್ರತಿಜ್ಞಾ(s);
        ಪ್ರತಿಜ್ಞಾ(ret);

        r = safe_atoi(s, &v);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (!oom_score_adjust_is_valid(v))
                ಪ್ರತ್ಯಾಗಮ -ERANGE;

        *ret = v;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ store_loadavg_fixed_point(unsigned long i, unsigned long f, loadavg_t *ret) {
        ಪ್ರತಿಜ್ಞಾ(ret);

        /* Validate both parts *before* shifting them, so that the shifts themselves cannot overflow.
         * Rejecting f >= 100 is equivalent to the old check of the shifted value against
         * LOADAVG_FIXED_POINT_1_0, since DIV_ROUND_UP(99 << 11, 100) == 2028 and
         * DIV_ROUND_UP(100 << 11, 100) == 2048. */
        ಯದಿ (i > (~0UL >> LOADAVG_PRECISION_BITS))
                ಪ್ರತ್ಯಾಗಮ -ERANGE;

        ಯದಿ (f >= 100)
                ಪ್ರತ್ಯಾಗಮ -ERANGE;

        *ret = (i << LOADAVG_PRECISION_BITS) | DIV_ROUND_UP(f << LOADAVG_PRECISION_BITS, 100);
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ parse_loadavg_fixed_point(ಧ್ರುವ ವರ್ಣ *s, loadavg_t *ret) {
        ಧ್ರುವ ವರ್ಣ *d, *f_str, *i_str;
        unsigned long i, f;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(s);
        ಪ್ರತಿಜ್ಞಾ(ret);

        d = strchr(s, '.');
        ಯದಿ (!d)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        i_str = strndupa_safe(s, d - s);
        f_str = d + 1;

        r = safe_atolu_full(i_str, 10, &i);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        r = safe_atolu_full(f_str, 10, &f);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ store_loadavg_fixed_point(i, f, ret);
}

/* Limitations are described in https://www.netfilter.org/projects/nftables/manpage.html and
 * https://bugzilla.netfilter.org/show_bug.cgi?id=1175 */
bool nft_identifier_valid(ಧ್ರುವ ವರ್ಣ *id) {
        ಯದಿ (isempty(id))
                ಪ್ರತ್ಯಾಗಮ false;

        ಯದಿ (strlen(id) >= NFT_NAME_MAXLEN)
                ಪ್ರತ್ಯಾಗಮ false;

        ಯದಿ (!ascii_isalpha(id[0]))
                ಪ್ರತ್ಯಾಗಮ false;

        ಪ್ರತ್ಯಾಗಮ in_charset(id + 1, ALPHANUMERICAL "/\\_.");
}

ಪೂರ್ಣಾಙ್ಕ parse_capability_set(ಧ್ರುವ ವರ್ಣ *s, uint64_t initial, uint64_t *current) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(s);
        ಪ್ರತಿಜ್ಞಾ(current);

        ಯದಿ (isempty(s)) {
                *current = CAP_MASK_UNSET;
                ಪ್ರತ್ಯಾಗಮ 1;
        }

        bool invert = false;
        ಯದಿ (s[0] == '~') {
                invert = true;
                s++;
        }

        uint64_t parsed;
        r = capability_set_from_string(s, &parsed);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (parsed == 0 || *current == initial)
                /* "~" or uninitialized data -> replace */
                *current = invert ? all_capabilities() & ~parsed : parsed;
        ಅನ್ಯಥಾ {
                /* previous data -> merge */
                ಯದಿ (invert)
                        *current &= ~parsed;
                ಅನ್ಯಥಾ
                        *current |= parsed;
        }

        ಪ್ರತ್ಯಾಗಮ r;
}
