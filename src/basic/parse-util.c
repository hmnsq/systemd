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

ಪೂರ್ಣಾಂಕ parse_boolean(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *v) {
        ಆದರೆ (!v)
                ಹಿಂತಿರುಗಿ -EINVAL;

        ಆದರೆ (STRCASE_ಒಳ_ಗಣ(v,
                           "1",
                           "yes",
                           "y",
                           "true",
                           "t",
                           "on"))
                ಹಿಂತಿರುಗಿ 1;

        ಆದರೆ (STRCASE_ಒಳ_ಗಣ(v,
                           "0",
                           "no",
                           "n",
                           "false",
                           "f",
                           "off"))
                ಹಿಂತಿರುಗಿ 0;

        ಹಿಂತಿರುಗಿ -EINVAL;
}

ಪೂರ್ಣಾಂಕ parse_tristate_ತುಂಬಿದ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *v, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೂರನೇ, ಪೂರ್ಣಾಂಕ *ret) {
        ಪೂರ್ಣಾಂಕ r;

        ಆದರೆ (isempty(v) || streq_ptr(v, ಮೂರನೇ)) { /* Empty string is always taken as the third/invalid/auto state */
                ಆದರೆ (ret)
                        *ret = -1;
        } ಇಲ್ಲದಿದ್ದರೆ {
                r = parse_boolean(v);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                ಆದರೆ (ret)
                        *ret = r;
        }

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ parse_pid(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, pid_t *ret) {
        ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ul = 0;
        pid_t pid;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(s);

        r = ಸುರಕ್ಷಿತ_atolu(s, &ul);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        pid = (pid_t) ul;

        ಆದರೆ ((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ) pid != ul)
                ಹಿಂತಿರುಗಿ -ERANGE;

        ಆದರೆ (!pid_is_valid(pid))
                ಹಿಂತಿರುಗಿ -ERANGE;

        ಆದರೆ (ret)
                *ret = pid;
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ parse_ಬಹುಲಕ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, mode_t *ret) {
        ಚಿಹ್ನೆರಹಿತ m;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(s);

        r = ಸುರಕ್ಷಿತ_atou_ತುಂಬಿದ(s, 8 |
                           ಸುರಕ್ಷಿತ_ATO_REFUSE_PLUS_MINUS, /* Leading '+' or even '-' char? that's just weird,
                                                        * refuse. User might have wanted to add mode flags or
                                                        * so, but this parser doesn't allow that, so let's
                                                        * better be safe. */
                           &m);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;
        ಆದರೆ (m > 07777)
                ಹಿಂತಿರುಗಿ -ERANGE;

        ಆದರೆ (ret)
                *ret = m;
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ parse_ifindex(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s) {
        ಪೂರ್ಣಾಂಕ ifi, r;

        ಪ್ರತಿಜ್ಞಾ(s);

        r = ಸುರಕ್ಷಿತ_atoi(s, &ifi);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;
        ಆದರೆ (ifi <= 0)
                ಹಿಂತಿರುಗಿ -EINVAL;

        ಹಿಂತಿರುಗಿ ifi;
}

ಪೂರ್ಣಾಂಕ parse_mtu(ಪೂರ್ಣಾಂಕ family, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, uint32_t *ret) {
        uint64_t u, m;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        r = parse_ಗಾತ್ರ(s, 1024, &u);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (u > UINT32_MAX)
                ಹಿಂತಿರುಗಿ -ERANGE;

        ಆಯ್ಕೆ (family) {
        ಸ್ಥಿತಿ AF_INET:
                m = IPV4_MIN_MTU; /* This is 68 */
                ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ AF_INET6:
                m = IPV6_MIN_MTU; /* This is 1280 */
                ನಿಲ್ಲಿಸು;
        ಪೂರ್ವನಿಯೋಜಿತ:
                m = 0;
        }

        ಆದರೆ (u < m)
                ಹಿಂತಿರುಗಿ -ERANGE;

        *ret = (uint32_t) u;
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ parse_ಗಾತ್ರ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *t, uint64_t base, uint64_t *ಗಾತ್ರ_2) {

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

        ರಚನೆ ಕೋಷ್ಟಕ {
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *suffix;
                ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ದೀರ್ಘ ಗುಣಕ;
        };

        ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ ಕೋಷ್ಟಕ iec[] = {
                { "E", 1024ULL*1024ULL*1024ULL*1024ULL*1024ULL*1024ULL },
                { "P", 1024ULL*1024ULL*1024ULL*1024ULL*1024ULL },
                { "T", 1024ULL*1024ULL*1024ULL*1024ULL },
                { "G", 1024ULL*1024ULL*1024ULL },
                { "M", 1024ULL*1024ULL },
                { "K", 1024ULL },
                { "B", 1ULL },
                { "",  1ULL },
        };

        ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ ಕೋಷ್ಟಕ si[] = {
                { "E", 1000ULL*1000ULL*1000ULL*1000ULL*1000ULL*1000ULL },
                { "P", 1000ULL*1000ULL*1000ULL*1000ULL*1000ULL },
                { "T", 1000ULL*1000ULL*1000ULL*1000ULL },
                { "G", 1000ULL*1000ULL*1000ULL },
                { "M", 1000ULL*1000ULL },
                { "K", 1000ULL },
                { "B", 1ULL },
                { "",  1ULL },
        };

        ಸ್ಥಿರಾಂಕ ರಚನೆ ಕೋಷ್ಟಕ *ಕೋಷ್ಟಕ;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p;
        ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ದೀರ್ಘ r = 0;
        ಚಿಹ್ನೆರಹಿತ n_entries, ಆರಂಭ_pos = 0;

        ಪ್ರತಿಜ್ಞಾ(t);
        ಪ್ರತಿಜ್ಞಾ(ಒಳ_ಗಣ(base, 1000, 1024));
        ಪ್ರತಿಜ್ಞಾ(ಗಾತ್ರ_2);

        ಆದರೆ (base == 1000) {
                ಕೋಷ್ಟಕ = si;
                n_entries = ELEMENTSOF(si);
        } ಇಲ್ಲದಿದ್ದರೆ {
                ಕೋಷ್ಟಕ = iec;
                n_entries = ELEMENTSOF(iec);
        }

        p = t;
        ಮಾಡು {
                ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ದೀರ್ಘ l, tmp;
                ದ್ವಿಗುಣ frac = 0;
                ಅಕ್ಷರ *e;
                ಚಿಹ್ನೆರಹಿತ i;

                p += strspn(p, WHITESPACE);

                errno = 0;
                l = strtoull(p, &e, 10);
                ಆದರೆ (errno > 0)
                        ಹಿಂತಿರುಗಿ -errno;
                ಆದರೆ (e == p)
                        ಹಿಂತಿರುಗಿ -EINVAL;
                ಆದರೆ (*p == '-')
                        ಹಿಂತಿರುಗಿ -ERANGE;

                ಆದರೆ (*e == '.') {
                        e++;

                        /* strtoull() itself would accept space/+/- */
                        ಆದರೆ (ascii_isdigit(*e)) {
                                ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ದೀರ್ಘ l2;
                                ಅಕ್ಷರ *e2;

                                l2 = strtoull(e, &e2, 10);
                                ಆದರೆ (errno > 0)
                                        ಹಿಂತಿರುಗಿ -errno;

                                /* Ignore failure. E.g. 10.M is valid */
                                frac = l2;
                                ಗಾಗಿ (; e < e2; e++)
                                        frac /= 10;
                        }
                }

                e += strspn(e, WHITESPACE);

                ಗಾಗಿ (i = ಆರಂಭ_pos; i < n_entries; i++)
                        ಆದರೆ (startswith(e, ಕೋಷ್ಟಕ[i].suffix))
                                ನಿಲ್ಲಿಸು;

                ಆದರೆ (i >= n_entries)
                        ಹಿಂತಿರುಗಿ -EINVAL;

                ಆದರೆ (l + (frac > 0) > ULLONG_MAX / ಕೋಷ್ಟಕ[i].ಗುಣಕ)
                        ಹಿಂತಿರುಗಿ -ERANGE;

                tmp = l * ಕೋಷ್ಟಕ[i].ಗುಣಕ + (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ದೀರ್ಘ) (frac * ಕೋಷ್ಟಕ[i].ಗುಣಕ);
                ಆದರೆ (tmp > ULLONG_MAX - r)
                        ಹಿಂತಿರುಗಿ -ERANGE;

                r += tmp;
                ಆದರೆ ((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ದೀರ್ಘ) (uint64_t) r != r)
                        ಹಿಂತಿರುಗಿ -ERANGE;

                p = e + ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಕೋಷ್ಟಕ[i].suffix);

                ಆರಂಭ_pos = i + 1;

        } ಆಗ (*p);

        *ಗಾತ್ರ_2 = r;

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ parse_sector_ಗಾತ್ರ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *t, uint64_t *ret) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(t);
        ಪ್ರತಿಜ್ಞಾ(ret);

        uint64_t ss;

        r = ಸುರಕ್ಷಿತ_atou64(t, &ss);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ log_error_errno(r, "Failed to parse sector size parameter %s", t);
        ಆದರೆ (ss < 512 || ss > 4096) /* Allow up to 4K due to dm-crypt support and 4K alignment by the homed LUKS backend */
                ಹಿಂತಿರುಗಿ log_error_errno(SYNTHETIC_ERRNO(ERANGE), "Sector size not between 512 and 4096: %s", t);
        ಆದರೆ (!ISPOWEROF2(ss))
                ಹಿಂತಿರುಗಿ log_error_errno(SYNTHETIC_ERRNO(EINVAL), "Sector size not power of 2: %s", t);

        *ret = ss;
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ parse_range(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *t, ಚಿಹ್ನೆರಹಿತ *ಅಧೋ, ಚಿಹ್ನೆರಹಿತ *ಊರ್ಧ್ವ) {
        _cleanup_free_ ಅಕ್ಷರ *ಶಬ್ದ = ಖಾಲಿ;
        ಚಿಹ್ನೆರಹಿತ l, u;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ಅಧೋ);
        ಪ್ರತಿಜ್ಞಾ(ಊರ್ಧ್ವ);

        /* Extract the lower bound. */
        r = ಹೊರತೆಗೆ_ಮೊದಲ_ಶಬ್ದ(&t, &ಶಬ್ದ, "-", ಹೊರತೆಗೆ_DONT_COALESCE_SEPARATORS);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;
        ಆದರೆ (r == 0)
                ಹಿಂತಿರುಗಿ -EINVAL;

        r = ಸುರಕ್ಷಿತ_atou(ಶಬ್ದ, &l);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        /* Check for the upper bound and extract it if needed */
        ಆದರೆ (!t)
                /* Single number with no dashes. */
                u = l;
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!*t)
                /* Trailing dash is an error. */
                ಹಿಂತಿರುಗಿ -EINVAL;
        ಇಲ್ಲದಿದ್ದರೆ {
                r = ಸುರಕ್ಷಿತ_atou(t, &u);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
        }

        *ಅಧೋ = l;
        *ಊರ್ಧ್ವ = u;
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ parse_errno(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *t) {
        ಪೂರ್ಣಾಂಕ r, e;

        ಪ್ರತಿಜ್ಞಾ(t);

        r = errno_from_name(t);
        ಆದರೆ (r > 0)
                ಹಿಂತಿರುಗಿ r;

        r = ಸುರಕ್ಷಿತ_atoi(t, &e);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        /* 0 is also allowed here */
        ಆದರೆ (!errno_is_valid(e) && e != 0)
                ಹಿಂತಿರುಗಿ -ERANGE;

        ಹಿಂತಿರುಗಿ e;
}

ಪೂರ್ಣಾಂಕ parse_fd(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *t) {
        ಪೂರ್ಣಾಂಕ r, fd;

        ಪ್ರತಿಜ್ಞಾ(t);

        r = ಸುರಕ್ಷಿತ_atoi(t, &fd);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (fd < 0)
                ಹಿಂತಿರುಗಿ -EBADF;

        ಹಿಂತಿರುಗಿ fd;
}

ಪೂರ್ಣಾಂಕ parse_user_shell(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ಅಕ್ಷರ **ret_sh, bool *ret_copy) {
        ಅಕ್ಷರ *sh;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret_sh);
        ಪ್ರತಿಜ್ಞಾ(ret_copy);

        ಆದರೆ (ಮಾರ್ಗ_is_absolute(s) && ಮಾರ್ಗ_is_normalized(s)) {
                sh = strdup(s);
                ಆದರೆ (!sh)
                        ಹಿಂತಿರುಗಿ -ENOMEM;

                *ret_sh = sh;
                *ret_copy = false;
        } ಇಲ್ಲದಿದ್ದರೆ {
                r = parse_boolean(s);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                *ret_sh = ಖಾಲಿ;
                *ret_copy = r;
        }

        ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *mangle_ತಳ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ಚಿಹ್ನೆರಹಿತ *base) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *k;

        ಪ್ರತಿಜ್ಞಾ(s);
        ಪ್ರತಿಜ್ಞಾ(base);

        /* Base already explicitly specified, then don't do anything. */
        ಆದರೆ (ಸುರಕ್ಷಿತ_ATO_ಮುಖವಾಡ_FLAGS(*base) != 0)
                ಹಿಂತಿರುಗಿ s;

        /* Support Python 3 style "0b" and 0x" prefixes, because they truly make sense, much more than C's "0" prefix for octal. */
        k = STARTSWITH_ಗಣ(s, "0b", "0B");
        ಆದರೆ (k) {
                *base = 2 | (*base & ಸುರಕ್ಷಿತ_ATO_ALL_FLAGS);
                ಹಿಂತಿರುಗಿ k;
        }

        k = STARTSWITH_ಗಣ(s, "0o", "0O");
        ಆದರೆ (k) {
                *base = 8 | (*base & ಸುರಕ್ಷಿತ_ATO_ALL_FLAGS);
                ಹಿಂತಿರುಗಿ k;
        }

        ಹಿಂತಿರುಗಿ s;
}

ಪೂರ್ಣಾಂಕ ಸುರಕ್ಷಿತ_atou_ತುಂಬಿದ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ಚಿಹ್ನೆರಹಿತ base, ಚಿಹ್ನೆರಹಿತ *ret_u) {
        ಅಕ್ಷರ *x = ಖಾಲಿ;
        ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ l;

        ಪ್ರತಿಜ್ಞಾ(s);
        ಪ್ರತಿಜ್ಞಾ(ಸುರಕ್ಷಿತ_ATO_ಮುಖವಾಡ_FLAGS(base) <= 16);

        /* strtoul() is happy to parse negative values, and silently converts them to unsigned values without
         * generating an error. We want a clean error, hence let's look for the "-" prefix on our own, and
         * generate an error. But let's do so only after strtoul() validated that the string is clean
         * otherwise, so that we return EINVAL preferably over ERANGE. */

        ಆದರೆ (FLAGS_ಗಣ(base, ಸುರಕ್ಷಿತ_ATO_REFUSE_LEADING_WHITESPACE) &&
            strchr(WHITESPACE, s[0]))
                ಹಿಂತಿರುಗಿ -EINVAL;

        s += strspn(s, WHITESPACE);

        ಆದರೆ (FLAGS_ಗಣ(base, ಸುರಕ್ಷಿತ_ATO_REFUSE_PLUS_MINUS) &&
            ಒಳ_ಗಣ(s[0], '+', '-'))
                ಹಿಂತಿರುಗಿ -EINVAL; /* Note that we check the "-" prefix again a second time below, but return a
                                 * different error. I.e. if the SAFE_ATO_REFUSE_PLUS_MINUS flag is set we
                                 * blanket refuse +/- prefixed integers, while if it is missing we'll just
                                 * return ERANGE, because the string actually parses correctly, but doesn't
                                 * fit in the return type. */

        ಆದರೆ (FLAGS_ಗಣ(base, ಸುರಕ್ಷಿತ_ATO_REFUSE_LEADING_ZERO) &&
            s[0] == '0' && !streq(s, "0"))
                ಹಿಂತಿರುಗಿ -EINVAL; /* This is particularly useful to avoid ambiguities between C's octal
                                 * notation and assumed-to-be-decimal integers with a leading zero. */

        s = mangle_ತಳ(s, &base);

        errno = 0;
        l = strtoul(s, &x, ಸುರಕ್ಷಿತ_ATO_ಮುಖವಾಡ_FLAGS(base) /* Let's mask off the flags bits so that only the actual
                                                      * base is left */);
        ಆದರೆ (errno > 0)
                ಹಿಂತಿರುಗಿ -errno;
        ಆದರೆ (!x || x == s || *x != 0)
                ಹಿಂತಿರುಗಿ -EINVAL;
        ಆದರೆ (l != 0 && s[0] == '-')
                ಹಿಂತಿರುಗಿ -ERANGE;
        ಆದರೆ ((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ) (ಚಿಹ್ನೆರಹಿತ) l != l)
                ಹಿಂತಿರುಗಿ -ERANGE;

        ಆದರೆ (ret_u)
                *ret_u = (ಚಿಹ್ನೆರಹಿತ) l;

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ ಸುರಕ್ಷಿತ_atou_bounded(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ಚಿಹ್ನೆರಹಿತ min, ಚಿಹ್ನೆರಹಿತ max, ಚಿಹ್ನೆರಹಿತ *ret) {
        ಚಿಹ್ನೆರಹಿತ v;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        r = ಸುರಕ್ಷಿತ_atou(s, &v);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (v < min || v > max)
                ಹಿಂತಿರುಗಿ -ERANGE;

        *ret = v;
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ ಸುರಕ್ಷಿತ_atoi(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ಪೂರ್ಣಾಂಕ *ret_i) {
        ಚಿಹ್ನೆರಹಿತ base = 0;
        ಅಕ್ಷರ *x = ಖಾಲಿ;
        ದೀರ್ಘ l;

        ಪ್ರತಿಜ್ಞಾ(s);

        s += strspn(s, WHITESPACE);
        s = mangle_ತಳ(s, &base);

        errno = 0;
        l = strtol(s, &x, base);
        ಆದರೆ (errno > 0)
                ಹಿಂತಿರುಗಿ -errno;
        ಆದರೆ (!x || x == s || *x != 0)
                ಹಿಂತಿರುಗಿ -EINVAL;
        ಆದರೆ ((ದೀರ್ಘ) (ಪೂರ್ಣಾಂಕ) l != l)
                ಹಿಂತಿರುಗಿ -ERANGE;

        ಆದರೆ (ret_i)
                *ret_i = (ಪೂರ್ಣಾಂಕ) l;

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ ಸುರಕ್ಷಿತ_atollu_ತುಂಬಿದ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ಚಿಹ್ನೆರಹಿತ base, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ದೀರ್ಘ *ret_llu) {
        ಅಕ್ಷರ *x = ಖಾಲಿ;
        ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ದೀರ್ಘ l;

        ಪ್ರತಿಜ್ಞಾ(s);
        ಪ್ರತಿಜ್ಞಾ(ಸುರಕ್ಷಿತ_ATO_ಮುಖವಾಡ_FLAGS(base) <= 16);

        ಆದರೆ (FLAGS_ಗಣ(base, ಸುರಕ್ಷಿತ_ATO_REFUSE_LEADING_WHITESPACE) &&
            strchr(WHITESPACE, s[0]))
                ಹಿಂತಿರುಗಿ -EINVAL;

        s += strspn(s, WHITESPACE);

        ಆದರೆ (FLAGS_ಗಣ(base, ಸುರಕ್ಷಿತ_ATO_REFUSE_PLUS_MINUS) &&
            ಒಳ_ಗಣ(s[0], '+', '-'))
                ಹಿಂತಿರುಗಿ -EINVAL;

        ಆದರೆ (FLAGS_ಗಣ(base, ಸುರಕ್ಷಿತ_ATO_REFUSE_LEADING_ZERO) &&
            s[0] == '0' && s[1] != 0)
                ಹಿಂತಿರುಗಿ -EINVAL;

        s = mangle_ತಳ(s, &base);

        errno = 0;
        l = strtoull(s, &x, ಸುರಕ್ಷಿತ_ATO_ಮುಖವಾಡ_FLAGS(base));
        ಆದರೆ (errno > 0)
                ಹಿಂತಿರುಗಿ -errno;
        ಆದರೆ (!x || x == s || *x != 0)
                ಹಿಂತಿರುಗಿ -EINVAL;
        ಆದರೆ (l != 0 && s[0] == '-')
                ಹಿಂತಿರುಗಿ -ERANGE;

        ಆದರೆ (ret_llu)
                *ret_llu = l;

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ ಸುರಕ್ಷಿತ_atolli(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ದೀರ್ಘ ದೀರ್ಘ *ret_lli) {
        ಚಿಹ್ನೆರಹಿತ base = 0;
        ಅಕ್ಷರ *x = ಖಾಲಿ;
        ದೀರ್ಘ ದೀರ್ಘ l;

        ಪ್ರತಿಜ್ಞಾ(s);

        s += strspn(s, WHITESPACE);
        s = mangle_ತಳ(s, &base);

        errno = 0;
        l = strtoll(s, &x, base);
        ಆದರೆ (errno > 0)
                ಹಿಂತಿರುಗಿ -errno;
        ಆದರೆ (!x || x == s || *x != 0)
                ಹಿಂತಿರುಗಿ -EINVAL;

        ಆದರೆ (ret_lli)
                *ret_lli = l;

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ ಸುರಕ್ಷಿತ_atou8_ತುಂಬಿದ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ಚಿಹ್ನೆರಹಿತ base, uint8_t *ret) {
        ಚಿಹ್ನೆರಹಿತ u;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        r = ಸುರಕ್ಷಿತ_atou_ತುಂಬಿದ(s, base, &u);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;
        ಆದರೆ (u > UINT8_MAX)
                ಹಿಂತಿರುಗಿ -ERANGE;

        *ret = (uint8_t) u;
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ ಸುರಕ್ಷಿತ_atou16_ತುಂಬಿದ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ಚಿಹ್ನೆರಹಿತ base, uint16_t *ret) {
        ಚಿಹ್ನೆರಹಿತ u;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        r = ಸುರಕ್ಷಿತ_atou_ತುಂಬಿದ(s, base, &u);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;
        ಆದರೆ (u > UINT16_MAX)
                ಹಿಂತಿರುಗಿ -ERANGE;

        *ret = (uint16_t) u;
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ ಸುರಕ್ಷಿತ_atoi16(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, int16_t *ret) {
        ಚಿಹ್ನೆರಹಿತ base = 0;
        ಅಕ್ಷರ *x = ಖಾಲಿ;
        ದೀರ್ಘ l;

        ಪ್ರತಿಜ್ಞಾ(s);

        s += strspn(s, WHITESPACE);
        s = mangle_ತಳ(s, &base);

        errno = 0;
        l = strtol(s, &x, base);
        ಆದರೆ (errno > 0)
                ಹಿಂತಿರುಗಿ -errno;
        ಆದರೆ (!x || x == s || *x != 0)
                ಹಿಂತಿರುಗಿ -EINVAL;
        ಆದರೆ ((ದೀರ್ಘ) (int16_t) l != l)
                ಹಿಂತಿರುಗಿ -ERANGE;

        ಆದರೆ (ret)
                *ret = (int16_t) l;

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ ಸುರಕ್ಷಿತ_atod(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ದ್ವಿಗುಣ *ret_d) {
        _cleanup_(freelocalep) locale_t loc = (locale_t) 0;
        ಅಕ್ಷರ *x = ಖಾಲಿ;
        ದ್ವಿಗುಣ d = 0;

        ಪ್ರತಿಜ್ಞಾ(s);

        loc = newlocale(LC_NUMERIC_MASK, "C", (locale_t) 0);
        ಆದರೆ (loc == (locale_t) 0)
                ಹಿಂತಿರುಗಿ -errno;

        errno = 0;
        d = strtod_l(s, &x, loc);
        ಆದರೆ (errno > 0)
                ಹಿಂತಿರುಗಿ -errno;
        ಆದರೆ (!x || x == s || *x != 0)
                ಹಿಂತಿರುಗಿ -EINVAL;

        ಆದರೆ (ret_d)
                *ret_d = d;

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ parse_fractional_part_u(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **p, size_t digits, ಚಿಹ್ನೆರಹಿತ *res) {
        ಚಿಹ್ನೆರಹಿತ val = 0;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s;

        ಪ್ರತಿಜ್ಞಾ(p);
        ಪ್ರತಿಜ್ಞಾ(res);

        s = *p;

        /* accept any number of digits, strtoull is limited to 19 */
        ಗಾಗಿ (size_t i = 0; i < digits; i++, s++) {
                ಆದರೆ (!ascii_isdigit(*s)) {
                        ಆದರೆ (i == 0)
                                ಹಿಂತಿರುಗಿ -EINVAL;

                        /* too few digits, pad with 0 */
                        ಗಾಗಿ (; i < digits; i++)
                                val *= 10;

                        ನಿಲ್ಲಿಸು;
                }

                val *= 10;
                val += *s - '0';
        }

        /* maybe round up */
        ಆದರೆ (*s >= '5' && *s <= '9')
                val++;

        s += strspn(s, DIGITS);

        *p = s;
        *res = val;

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ parse_nice(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ಪೂರ್ಣಾಂಕ *ret) {
        ಪೂರ್ಣಾಂಕ n, r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        r = ಸುರಕ್ಷಿತ_atoi(s, &n);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (!nice_is_valid(n))
                ಹಿಂತಿರುಗಿ -ERANGE;

        *ret = n;
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ parse_ip_ಪೋರ್ಟ್(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, uint16_t *ret) {
        uint16_t l;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        r = ಸುರಕ್ಷಿತ_atou16_ತುಂಬಿದ(s, ಸುರಕ್ಷಿತ_ATO_REFUSE_LEADING_WHITESPACE, &l);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (l == 0)
                ಹಿಂತಿರುಗಿ -EINVAL;

        *ret = l;

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ parse_ip_ಪೋರ್ಟ್_range(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, uint16_t *low, uint16_t *high, bool allow_zero) {
        ಚಿಹ್ನೆರಹಿತ l, h;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(low);
        ಪ್ರತಿಜ್ಞಾ(high);

        r = parse_range(s, &l, &h);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (l > 65535 || h > 65535)
                ಹಿಂತಿರುಗಿ -EINVAL;

        ಆದರೆ (!allow_zero && (l == 0 || h == 0))
                ಹಿಂತಿರುಗಿ -EINVAL;

        ಆದರೆ (h < l)
                ಹಿಂತಿರುಗಿ -EINVAL;

        *low = l;
        *high = h;

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ parse_oom_score_adjust(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ಪೂರ್ಣಾಂಕ *ret) {
        ಪೂರ್ಣಾಂಕ r, v;

        ಪ್ರತಿಜ್ಞಾ(s);
        ಪ್ರತಿಜ್ಞಾ(ret);

        r = ಸುರಕ್ಷಿತ_atoi(s, &v);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (!oom_score_adjust_is_valid(v))
                ಹಿಂತಿರುಗಿ -ERANGE;

        *ret = v;
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ store_loadavg_fixed_ಬಿಂದು(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ i, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ f, loadavg_t *ret) {
        ಪ್ರತಿಜ್ಞಾ(ret);

        /* Validate both parts *before* shifting them, so that the shifts themselves cannot overflow.
         * Rejecting f >= 100 is equivalent to the old check of the shifted value against
         * LOADAVG_FIXED_POINT_1_0, since DIV_ROUND_UP(99 << 11, 100) == 2028 and
         * DIV_ROUND_UP(100 << 11, 100) == 2048. */
        ಆದರೆ (i > (~0UL >> LOADAVG_PRECISION_ಬಿಟ್ಟುಗಳು))
                ಹಿಂತಿರುಗಿ -ERANGE;

        ಆದರೆ (f >= 100)
                ಹಿಂತಿರುಗಿ -ERANGE;

        *ret = (i << LOADAVG_PRECISION_ಬಿಟ್ಟುಗಳು) | DIV_ಸುತ್ತು_ಮೇಲೆ(f << LOADAVG_PRECISION_ಬಿಟ್ಟುಗಳು, 100);
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ parse_loadavg_fixed_ಬಿಂದು(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, loadavg_t *ret) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *d, *f_str, *i_str;
        ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ i, f;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(s);
        ಪ್ರತಿಜ್ಞಾ(ret);

        d = strchr(s, '.');
        ಆದರೆ (!d)
                ಹಿಂತಿರುಗಿ -EINVAL;

        i_str = strndupa_ಸುರಕ್ಷಿತ(s, d - s);
        f_str = d + 1;

        r = ಸುರಕ್ಷಿತ_atolu_ತುಂಬಿದ(i_str, 10, &i);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        r = ಸುರಕ್ಷಿತ_atolu_ತುಂಬಿದ(f_str, 10, &f);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ store_loadavg_fixed_ಬಿಂದು(i, f, ret);
}

/* Limitations are described in https://www.netfilter.org/projects/nftables/manpage.html and
 * https://bugzilla.netfilter.org/show_bug.cgi?id=1175 */
bool nft_identifier_valid(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *id) {
        ಆದರೆ (isempty(id))
                ಹಿಂತಿರುಗಿ false;

        ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(id) >= NFT_NAME_MAXLEN)
                ಹಿಂತಿರುಗಿ false;

        ಆದರೆ (!ascii_isalpha(id[0]))
                ಹಿಂತಿರುಗಿ false;

        ಹಿಂತಿರುಗಿ ಒಳ_charset(id + 1, ALPHANUMERICAL "/\\_.");
}

ಪೂರ್ಣಾಂಕ parse_capability_ಗಣ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, uint64_t initial, uint64_t *current) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(s);
        ಪ್ರತಿಜ್ಞಾ(current);

        ಆದರೆ (isempty(s)) {
                *current = CAP_ಮುಖವಾಡ_UNSET;
                ಹಿಂತಿರುಗಿ 1;
        }

        bool invert = false;
        ಆದರೆ (s[0] == '~') {
                invert = true;
                s++;
        }

        uint64_t parsed;
        r = capability_ಗಣ_from_string(s, &parsed);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (parsed == 0 || *current == initial)
                /* "~" or uninitialized data -> replace */
                *current = invert ? all_capabilities() & ~parsed : parsed;
        ಇಲ್ಲದಿದ್ದರೆ {
                /* previous data -> merge */
                ಆದರೆ (invert)
                        *current &= ~parsed;
                ಇಲ್ಲದಿದ್ದರೆ
                        *current |= parsed;
        }

        ಹಿಂತಿರುಗಿ r;
}
