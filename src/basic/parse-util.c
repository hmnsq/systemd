/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ <linux/ipv6.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <linux/netfilter/nf_tables.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdlib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/socket.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "alloc-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "capability-list.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "capability-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "errno-list.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "extract-word.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "locale-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "log.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "missing-network.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "parse-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "path-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "process-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "strv.h"

ਪੂਰਨ parse_boolean(ਅਚਲ ਅੱਖਰ *v) {
        ਜੇ (!v)
                ਵਾਪਸ -EINVAL;

        ਜੇ (STRCASE_ਵਿੱਚ_ਸੈੱਟ(v,
                           "1",
                           "yes",
                           "y",
                           "true",
                           "t",
                           "on"))
                ਵਾਪਸ 1;

        ਜੇ (STRCASE_ਵਿੱਚ_ਸੈੱਟ(v,
                           "0",
                           "no",
                           "n",
                           "false",
                           "f",
                           "off"))
                ਵਾਪਸ 0;

        ਵਾਪਸ -EINVAL;
}

ਪੂਰਨ parse_tristate_ਭਰਿਆ(ਅਚਲ ਅੱਖਰ *v, ਅਚਲ ਅੱਖਰ *ਤੀਜਾ, ਪੂਰਨ *ret) {
        ਪੂਰਨ r;

        ਜੇ (isempty(v) || streq_ptr(v, ਤੀਜਾ)) { /* Empty string is always taken as the third/invalid/auto state */
                ਜੇ (ret)
                        *ret = -1;
        } ਨਹੀਂਤਾਂ {
                r = parse_boolean(v);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                ਜੇ (ret)
                        *ret = r;
        }

        ਵਾਪਸ 0;
}

ਪੂਰਨ parse_pid(ਅਚਲ ਅੱਖਰ *s, pid_t *ret) {
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ul = 0;
        pid_t pid;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(s);

        r = ਸੁਰੱਖਿਅਤ_atolu(s, &ul);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        pid = (pid_t) ul;

        ਜੇ ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ) pid != ul)
                ਵਾਪਸ -ERANGE;

        ਜੇ (!pid_is_valid(pid))
                ਵਾਪਸ -ERANGE;

        ਜੇ (ret)
                *ret = pid;
        ਵਾਪਸ 0;
}

ਪੂਰਨ parse_ਬਹੁਲਕ(ਅਚਲ ਅੱਖਰ *s, mode_t *ret) {
        ਬਿਨਾਨਿਸ਼ਾਨੀ m;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(s);

        r = ਸੁਰੱਖਿਅਤ_atou_ਭਰਿਆ(s, 8 |
                           ਸੁਰੱਖਿਅਤ_ATO_REFUSE_PLUS_MINUS, /* Leading '+' or even '-' char? that's just weird,
                                                        * refuse. User might have wanted to add mode flags or
                                                        * so, but this parser doesn't allow that, so let's
                                                        * better be safe. */
                           &m);
        ਜੇ (r < 0)
                ਵਾਪਸ r;
        ਜੇ (m > 07777)
                ਵਾਪਸ -ERANGE;

        ਜੇ (ret)
                *ret = m;
        ਵਾਪਸ 0;
}

ਪੂਰਨ parse_ifindex(ਅਚਲ ਅੱਖਰ *s) {
        ਪੂਰਨ ifi, r;

        ਪ੍ਰਤਿਜ੍ਞਾ(s);

        r = ਸੁਰੱਖਿਅਤ_atoi(s, &ifi);
        ਜੇ (r < 0)
                ਵਾਪਸ r;
        ਜੇ (ifi <= 0)
                ਵਾਪਸ -EINVAL;

        ਵਾਪਸ ifi;
}

ਪੂਰਨ parse_mtu(ਪੂਰਨ family, ਅਚਲ ਅੱਖਰ *s, uint32_t *ret) {
        uint64_t u, m;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        r = parse_ਮਾਪ(s, 1024, &u);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (u > UINT32_MAX)
                ਵਾਪਸ -ERANGE;

        ਚੋਣ (family) {
        ਹਾਲਤ AF_INET:
                m = IPV4_MIN_MTU; /* This is 68 */
                ਰੋਕੋ;
        ਹਾਲਤ AF_INET6:
                m = IPV6_MIN_MTU; /* This is 1280 */
                ਰੋਕੋ;
        ਮੂਲ:
                m = 0;
        }

        ਜੇ (u < m)
                ਵਾਪਸ -ERANGE;

        *ret = (uint32_t) u;
        ਵਾਪਸ 0;
}

ਪੂਰਨ parse_ਮਾਪ(ਅਚਲ ਅੱਖਰ *t, uint64_t base, uint64_t *ਮਾਪ) {

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

        ਬਣਤਰ ਸਾਰਣੀ {
                ਅਚਲ ਅੱਖਰ *suffix;
                ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਲੰਮਾ ਗੁਣਕ;
        };

        ਸਥਿਰ ਅਚਲ ਬਣਤਰ ਸਾਰਣੀ iec[] = {
                { "E", 1024ULL*1024ULL*1024ULL*1024ULL*1024ULL*1024ULL },
                { "P", 1024ULL*1024ULL*1024ULL*1024ULL*1024ULL },
                { "T", 1024ULL*1024ULL*1024ULL*1024ULL },
                { "G", 1024ULL*1024ULL*1024ULL },
                { "M", 1024ULL*1024ULL },
                { "K", 1024ULL },
                { "B", 1ULL },
                { "",  1ULL },
        };

        ਸਥਿਰ ਅਚਲ ਬਣਤਰ ਸਾਰਣੀ si[] = {
                { "E", 1000ULL*1000ULL*1000ULL*1000ULL*1000ULL*1000ULL },
                { "P", 1000ULL*1000ULL*1000ULL*1000ULL*1000ULL },
                { "T", 1000ULL*1000ULL*1000ULL*1000ULL },
                { "G", 1000ULL*1000ULL*1000ULL },
                { "M", 1000ULL*1000ULL },
                { "K", 1000ULL },
                { "B", 1ULL },
                { "",  1ULL },
        };

        ਅਚਲ ਬਣਤਰ ਸਾਰਣੀ *ਸਾਰਣੀ;
        ਅਚਲ ਅੱਖਰ *p;
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਲੰਮਾ r = 0;
        ਬਿਨਾਨਿਸ਼ਾਨੀ n_entries, ਸ਼ੁਰੂ_pos = 0;

        ਪ੍ਰਤਿਜ੍ਞਾ(t);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਵਿੱਚ_ਸੈੱਟ(base, 1000, 1024));
        ਪ੍ਰਤਿਜ੍ਞਾ(ਮਾਪ);

        ਜੇ (base == 1000) {
                ਸਾਰਣੀ = si;
                n_entries = ELEMENTSOF(si);
        } ਨਹੀਂਤਾਂ {
                ਸਾਰਣੀ = iec;
                n_entries = ELEMENTSOF(iec);
        }

        p = t;
        ਕਰੋ {
                ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਲੰਮਾ l, tmp;
                ਦੁੱਗਣਾ frac = 0;
                ਅੱਖਰ *e;
                ਬਿਨਾਨਿਸ਼ਾਨੀ i;

                p += strspn(p, WHITESPACE);

                errno = 0;
                l = strtoull(p, &e, 10);
                ਜੇ (errno > 0)
                        ਵਾਪਸ -errno;
                ਜੇ (e == p)
                        ਵਾਪਸ -EINVAL;
                ਜੇ (*p == '-')
                        ਵਾਪਸ -ERANGE;

                ਜੇ (*e == '.') {
                        e++;

                        /* strtoull() itself would accept space/+/- */
                        ਜੇ (ascii_isdigit(*e)) {
                                ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਲੰਮਾ l2;
                                ਅੱਖਰ *e2;

                                l2 = strtoull(e, &e2, 10);
                                ਜੇ (errno > 0)
                                        ਵਾਪਸ -errno;

                                /* Ignore failure. E.g. 10.M is valid */
                                frac = l2;
                                ਲਈ (; e < e2; e++)
                                        frac /= 10;
                        }
                }

                e += strspn(e, WHITESPACE);

                ਲਈ (i = ਸ਼ੁਰੂ_pos; i < n_entries; i++)
                        ਜੇ (startswith(e, ਸਾਰਣੀ[i].suffix))
                                ਰੋਕੋ;

                ਜੇ (i >= n_entries)
                        ਵਾਪਸ -EINVAL;

                ਜੇ (l + (frac > 0) > ULLONG_MAX / ਸਾਰਣੀ[i].ਗੁਣਕ)
                        ਵਾਪਸ -ERANGE;

                tmp = l * ਸਾਰਣੀ[i].ਗੁਣਕ + (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਲੰਮਾ) (frac * ਸਾਰਣੀ[i].ਗੁਣਕ);
                ਜੇ (tmp > ULLONG_MAX - r)
                        ਵਾਪਸ -ERANGE;

                r += tmp;
                ਜੇ ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਲੰਮਾ) (uint64_t) r != r)
                        ਵਾਪਸ -ERANGE;

                p = e + ਲੜੀ_ਲੰਬਾਈ(ਸਾਰਣੀ[i].suffix);

                ਸ਼ੁਰੂ_pos = i + 1;

        } ਜਦੋਂ (*p);

        *ਮਾਪ = r;

        ਵਾਪਸ 0;
}

ਪੂਰਨ parse_sector_ਮਾਪ(ਅਚਲ ਅੱਖਰ *t, uint64_t *ret) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(t);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        uint64_t ss;

        r = ਸੁਰੱਖਿਅਤ_atou64(t, &ss);
        ਜੇ (r < 0)
                ਵਾਪਸ log_error_errno(r, "Failed to parse sector size parameter %s", t);
        ਜੇ (ss < 512 || ss > 4096) /* Allow up to 4K due to dm-crypt support and 4K alignment by the homed LUKS backend */
                ਵਾਪਸ log_error_errno(SYNTHETIC_ERRNO(ERANGE), "Sector size not between 512 and 4096: %s", t);
        ਜੇ (!ISPOWEROF2(ss))
                ਵਾਪਸ log_error_errno(SYNTHETIC_ERRNO(EINVAL), "Sector size not power of 2: %s", t);

        *ret = ss;
        ਵਾਪਸ 0;
}

ਪੂਰਨ parse_range(ਅਚਲ ਅੱਖਰ *t, ਬਿਨਾਨਿਸ਼ਾਨੀ *ਹੇਠਲਾ, ਬਿਨਾਨਿਸ਼ਾਨੀ *ਉੱਤਲਾ) {
        _cleanup_free_ ਅੱਖਰ *ਸ਼ਬਦ = ਨਿਰਾ;
        ਬਿਨਾਨਿਸ਼ਾਨੀ l, u;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਹੇਠਲਾ);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਉੱਤਲਾ);

        /* Extract the lower bound. */
        r = ਕੱਢੋ_ਪਹਿਲਾ_ਸ਼ਬਦ(&t, &ਸ਼ਬਦ, "-", ਕੱਢੋ_DONT_COALESCE_SEPARATORS);
        ਜੇ (r < 0)
                ਵਾਪਸ r;
        ਜੇ (r == 0)
                ਵਾਪਸ -EINVAL;

        r = ਸੁਰੱਖਿਅਤ_atou(ਸ਼ਬਦ, &l);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        /* Check for the upper bound and extract it if needed */
        ਜੇ (!t)
                /* Single number with no dashes. */
                u = l;
        ਨਹੀਂਤਾਂ ਜੇ (!*t)
                /* Trailing dash is an error. */
                ਵਾਪਸ -EINVAL;
        ਨਹੀਂਤਾਂ {
                r = ਸੁਰੱਖਿਅਤ_atou(t, &u);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
        }

        *ਹੇਠਲਾ = l;
        *ਉੱਤਲਾ = u;
        ਵਾਪਸ 0;
}

ਪੂਰਨ parse_errno(ਅਚਲ ਅੱਖਰ *t) {
        ਪੂਰਨ r, e;

        ਪ੍ਰਤਿਜ੍ਞਾ(t);

        r = errno_from_name(t);
        ਜੇ (r > 0)
                ਵਾਪਸ r;

        r = ਸੁਰੱਖਿਅਤ_atoi(t, &e);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        /* 0 is also allowed here */
        ਜੇ (!errno_is_valid(e) && e != 0)
                ਵਾਪਸ -ERANGE;

        ਵਾਪਸ e;
}

ਪੂਰਨ parse_fd(ਅਚਲ ਅੱਖਰ *t) {
        ਪੂਰਨ r, fd;

        ਪ੍ਰਤਿਜ੍ਞਾ(t);

        r = ਸੁਰੱਖਿਅਤ_atoi(t, &fd);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (fd < 0)
                ਵਾਪਸ -EBADF;

        ਵਾਪਸ fd;
}

ਪੂਰਨ parse_user_shell(ਅਚਲ ਅੱਖਰ *s, ਅੱਖਰ **ret_sh, bool *ret_copy) {
        ਅੱਖਰ *sh;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret_sh);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret_copy);

        ਜੇ (ਰਸਤਾ_is_absolute(s) && ਰਸਤਾ_is_normalized(s)) {
                sh = strdup(s);
                ਜੇ (!sh)
                        ਵਾਪਸ -ENOMEM;

                *ret_sh = sh;
                *ret_copy = false;
        } ਨਹੀਂਤਾਂ {
                r = parse_boolean(s);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                *ret_sh = ਨਿਰਾ;
                *ret_copy = r;
        }

        ਵਾਪਸ 0;
}

ਸਥਿਰ ਅਚਲ ਅੱਖਰ *mangle_ਅਧਾਰ(ਅਚਲ ਅੱਖਰ *s, ਬਿਨਾਨਿਸ਼ਾਨੀ *base) {
        ਅਚਲ ਅੱਖਰ *k;

        ਪ੍ਰਤਿਜ੍ਞਾ(s);
        ਪ੍ਰਤਿਜ੍ਞਾ(base);

        /* Base already explicitly specified, then don't do anything. */
        ਜੇ (ਸੁਰੱਖਿਅਤ_ATO_ਨਕਾਬ_FLAGS(*base) != 0)
                ਵਾਪਸ s;

        /* Support Python 3 style "0b" and 0x" prefixes, because they truly make sense, much more than C's "0" prefix for octal. */
        k = STARTSWITH_ਸੈੱਟ(s, "0b", "0B");
        ਜੇ (k) {
                *base = 2 | (*base & ਸੁਰੱਖਿਅਤ_ATO_ALL_FLAGS);
                ਵਾਪਸ k;
        }

        k = STARTSWITH_ਸੈੱਟ(s, "0o", "0O");
        ਜੇ (k) {
                *base = 8 | (*base & ਸੁਰੱਖਿਅਤ_ATO_ALL_FLAGS);
                ਵਾਪਸ k;
        }

        ਵਾਪਸ s;
}

ਪੂਰਨ ਸੁਰੱਖਿਅਤ_atou_ਭਰਿਆ(ਅਚਲ ਅੱਖਰ *s, ਬਿਨਾਨਿਸ਼ਾਨੀ base, ਬਿਨਾਨਿਸ਼ਾਨੀ *ret_u) {
        ਅੱਖਰ *x = ਨਿਰਾ;
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ l;

        ਪ੍ਰਤਿਜ੍ਞਾ(s);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਸੁਰੱਖਿਅਤ_ATO_ਨਕਾਬ_FLAGS(base) <= 16);

        /* strtoul() is happy to parse negative values, and silently converts them to unsigned values without
         * generating an error. We want a clean error, hence let's look for the "-" prefix on our own, and
         * generate an error. But let's do so only after strtoul() validated that the string is clean
         * otherwise, so that we return EINVAL preferably over ERANGE. */

        ਜੇ (FLAGS_ਸੈੱਟ(base, ਸੁਰੱਖਿਅਤ_ATO_REFUSE_LEADING_WHITESPACE) &&
            strchr(WHITESPACE, s[0]))
                ਵਾਪਸ -EINVAL;

        s += strspn(s, WHITESPACE);

        ਜੇ (FLAGS_ਸੈੱਟ(base, ਸੁਰੱਖਿਅਤ_ATO_REFUSE_PLUS_MINUS) &&
            ਵਿੱਚ_ਸੈੱਟ(s[0], '+', '-'))
                ਵਾਪਸ -EINVAL; /* Note that we check the "-" prefix again a second time below, but return a
                                 * different error. I.e. if the SAFE_ATO_REFUSE_PLUS_MINUS flag is set we
                                 * blanket refuse +/- prefixed integers, while if it is missing we'll just
                                 * return ERANGE, because the string actually parses correctly, but doesn't
                                 * fit in the return type. */

        ਜੇ (FLAGS_ਸੈੱਟ(base, ਸੁਰੱਖਿਅਤ_ATO_REFUSE_LEADING_ZERO) &&
            s[0] == '0' && !streq(s, "0"))
                ਵਾਪਸ -EINVAL; /* This is particularly useful to avoid ambiguities between C's octal
                                 * notation and assumed-to-be-decimal integers with a leading zero. */

        s = mangle_ਅਧਾਰ(s, &base);

        errno = 0;
        l = strtoul(s, &x, ਸੁਰੱਖਿਅਤ_ATO_ਨਕਾਬ_FLAGS(base) /* Let's mask off the flags bits so that only the actual
                                                      * base is left */);
        ਜੇ (errno > 0)
                ਵਾਪਸ -errno;
        ਜੇ (!x || x == s || *x != 0)
                ਵਾਪਸ -EINVAL;
        ਜੇ (l != 0 && s[0] == '-')
                ਵਾਪਸ -ERANGE;
        ਜੇ ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ) (ਬਿਨਾਨਿਸ਼ਾਨੀ) l != l)
                ਵਾਪਸ -ERANGE;

        ਜੇ (ret_u)
                *ret_u = (ਬਿਨਾਨਿਸ਼ਾਨੀ) l;

        ਵਾਪਸ 0;
}

ਪੂਰਨ ਸੁਰੱਖਿਅਤ_atou_bounded(ਅਚਲ ਅੱਖਰ *s, ਬਿਨਾਨਿਸ਼ਾਨੀ min, ਬਿਨਾਨਿਸ਼ਾਨੀ max, ਬਿਨਾਨਿਸ਼ਾਨੀ *ret) {
        ਬਿਨਾਨਿਸ਼ਾਨੀ v;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        r = ਸੁਰੱਖਿਅਤ_atou(s, &v);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (v < min || v > max)
                ਵਾਪਸ -ERANGE;

        *ret = v;
        ਵਾਪਸ 0;
}

ਪੂਰਨ ਸੁਰੱਖਿਅਤ_atoi(ਅਚਲ ਅੱਖਰ *s, ਪੂਰਨ *ret_i) {
        ਬਿਨਾਨਿਸ਼ਾਨੀ base = 0;
        ਅੱਖਰ *x = ਨਿਰਾ;
        ਲੰਮਾ l;

        ਪ੍ਰਤਿਜ੍ਞਾ(s);

        s += strspn(s, WHITESPACE);
        s = mangle_ਅਧਾਰ(s, &base);

        errno = 0;
        l = strtol(s, &x, base);
        ਜੇ (errno > 0)
                ਵਾਪਸ -errno;
        ਜੇ (!x || x == s || *x != 0)
                ਵਾਪਸ -EINVAL;
        ਜੇ ((ਲੰਮਾ) (ਪੂਰਨ) l != l)
                ਵਾਪਸ -ERANGE;

        ਜੇ (ret_i)
                *ret_i = (ਪੂਰਨ) l;

        ਵਾਪਸ 0;
}

ਪੂਰਨ ਸੁਰੱਖਿਅਤ_atollu_ਭਰਿਆ(ਅਚਲ ਅੱਖਰ *s, ਬਿਨਾਨਿਸ਼ਾਨੀ base, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਲੰਮਾ *ret_llu) {
        ਅੱਖਰ *x = ਨਿਰਾ;
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਲੰਮਾ l;

        ਪ੍ਰਤਿਜ੍ਞਾ(s);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਸੁਰੱਖਿਅਤ_ATO_ਨਕਾਬ_FLAGS(base) <= 16);

        ਜੇ (FLAGS_ਸੈੱਟ(base, ਸੁਰੱਖਿਅਤ_ATO_REFUSE_LEADING_WHITESPACE) &&
            strchr(WHITESPACE, s[0]))
                ਵਾਪਸ -EINVAL;

        s += strspn(s, WHITESPACE);

        ਜੇ (FLAGS_ਸੈੱਟ(base, ਸੁਰੱਖਿਅਤ_ATO_REFUSE_PLUS_MINUS) &&
            ਵਿੱਚ_ਸੈੱਟ(s[0], '+', '-'))
                ਵਾਪਸ -EINVAL;

        ਜੇ (FLAGS_ਸੈੱਟ(base, ਸੁਰੱਖਿਅਤ_ATO_REFUSE_LEADING_ZERO) &&
            s[0] == '0' && s[1] != 0)
                ਵਾਪਸ -EINVAL;

        s = mangle_ਅਧਾਰ(s, &base);

        errno = 0;
        l = strtoull(s, &x, ਸੁਰੱਖਿਅਤ_ATO_ਨਕਾਬ_FLAGS(base));
        ਜੇ (errno > 0)
                ਵਾਪਸ -errno;
        ਜੇ (!x || x == s || *x != 0)
                ਵਾਪਸ -EINVAL;
        ਜੇ (l != 0 && s[0] == '-')
                ਵਾਪਸ -ERANGE;

        ਜੇ (ret_llu)
                *ret_llu = l;

        ਵਾਪਸ 0;
}

ਪੂਰਨ ਸੁਰੱਖਿਅਤ_atolli(ਅਚਲ ਅੱਖਰ *s, ਲੰਮਾ ਲੰਮਾ *ret_lli) {
        ਬਿਨਾਨਿਸ਼ਾਨੀ base = 0;
        ਅੱਖਰ *x = ਨਿਰਾ;
        ਲੰਮਾ ਲੰਮਾ l;

        ਪ੍ਰਤਿਜ੍ਞਾ(s);

        s += strspn(s, WHITESPACE);
        s = mangle_ਅਧਾਰ(s, &base);

        errno = 0;
        l = strtoll(s, &x, base);
        ਜੇ (errno > 0)
                ਵਾਪਸ -errno;
        ਜੇ (!x || x == s || *x != 0)
                ਵਾਪਸ -EINVAL;

        ਜੇ (ret_lli)
                *ret_lli = l;

        ਵਾਪਸ 0;
}

ਪੂਰਨ ਸੁਰੱਖਿਅਤ_atou8_ਭਰਿਆ(ਅਚਲ ਅੱਖਰ *s, ਬਿਨਾਨਿਸ਼ਾਨੀ base, uint8_t *ret) {
        ਬਿਨਾਨਿਸ਼ਾਨੀ u;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        r = ਸੁਰੱਖਿਅਤ_atou_ਭਰਿਆ(s, base, &u);
        ਜੇ (r < 0)
                ਵਾਪਸ r;
        ਜੇ (u > UINT8_MAX)
                ਵਾਪਸ -ERANGE;

        *ret = (uint8_t) u;
        ਵਾਪਸ 0;
}

ਪੂਰਨ ਸੁਰੱਖਿਅਤ_atou16_ਭਰਿਆ(ਅਚਲ ਅੱਖਰ *s, ਬਿਨਾਨਿਸ਼ਾਨੀ base, uint16_t *ret) {
        ਬਿਨਾਨਿਸ਼ਾਨੀ u;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        r = ਸੁਰੱਖਿਅਤ_atou_ਭਰਿਆ(s, base, &u);
        ਜੇ (r < 0)
                ਵਾਪਸ r;
        ਜੇ (u > UINT16_MAX)
                ਵਾਪਸ -ERANGE;

        *ret = (uint16_t) u;
        ਵਾਪਸ 0;
}

ਪੂਰਨ ਸੁਰੱਖਿਅਤ_atoi16(ਅਚਲ ਅੱਖਰ *s, int16_t *ret) {
        ਬਿਨਾਨਿਸ਼ਾਨੀ base = 0;
        ਅੱਖਰ *x = ਨਿਰਾ;
        ਲੰਮਾ l;

        ਪ੍ਰਤਿਜ੍ਞਾ(s);

        s += strspn(s, WHITESPACE);
        s = mangle_ਅਧਾਰ(s, &base);

        errno = 0;
        l = strtol(s, &x, base);
        ਜੇ (errno > 0)
                ਵਾਪਸ -errno;
        ਜੇ (!x || x == s || *x != 0)
                ਵਾਪਸ -EINVAL;
        ਜੇ ((ਲੰਮਾ) (int16_t) l != l)
                ਵਾਪਸ -ERANGE;

        ਜੇ (ret)
                *ret = (int16_t) l;

        ਵਾਪਸ 0;
}

ਪੂਰਨ ਸੁਰੱਖਿਅਤ_atod(ਅਚਲ ਅੱਖਰ *s, ਦੁੱਗਣਾ *ret_d) {
        _cleanup_(freelocalep) locale_t loc = (locale_t) 0;
        ਅੱਖਰ *x = ਨਿਰਾ;
        ਦੁੱਗਣਾ d = 0;

        ਪ੍ਰਤਿਜ੍ਞਾ(s);

        loc = newlocale(LC_NUMERIC_MASK, "C", (locale_t) 0);
        ਜੇ (loc == (locale_t) 0)
                ਵਾਪਸ -errno;

        errno = 0;
        d = strtod_l(s, &x, loc);
        ਜੇ (errno > 0)
                ਵਾਪਸ -errno;
        ਜੇ (!x || x == s || *x != 0)
                ਵਾਪਸ -EINVAL;

        ਜੇ (ret_d)
                *ret_d = d;

        ਵਾਪਸ 0;
}

ਪੂਰਨ parse_fractional_part_u(ਅਚਲ ਅੱਖਰ **p, size_t digits, ਬਿਨਾਨਿਸ਼ਾਨੀ *res) {
        ਬਿਨਾਨਿਸ਼ਾਨੀ val = 0;
        ਅਚਲ ਅੱਖਰ *s;

        ਪ੍ਰਤਿਜ੍ਞਾ(p);
        ਪ੍ਰਤਿਜ੍ਞਾ(res);

        s = *p;

        /* accept any number of digits, strtoull is limited to 19 */
        ਲਈ (size_t i = 0; i < digits; i++, s++) {
                ਜੇ (!ascii_isdigit(*s)) {
                        ਜੇ (i == 0)
                                ਵਾਪਸ -EINVAL;

                        /* too few digits, pad with 0 */
                        ਲਈ (; i < digits; i++)
                                val *= 10;

                        ਰੋਕੋ;
                }

                val *= 10;
                val += *s - '0';
        }

        /* maybe round up */
        ਜੇ (*s >= '5' && *s <= '9')
                val++;

        s += strspn(s, DIGITS);

        *p = s;
        *res = val;

        ਵਾਪਸ 0;
}

ਪੂਰਨ parse_nice(ਅਚਲ ਅੱਖਰ *s, ਪੂਰਨ *ret) {
        ਪੂਰਨ n, r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        r = ਸੁਰੱਖਿਅਤ_atoi(s, &n);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (!nice_is_valid(n))
                ਵਾਪਸ -ERANGE;

        *ret = n;
        ਵਾਪਸ 0;
}

ਪੂਰਨ parse_ip_ਪੋਰਟ(ਅਚਲ ਅੱਖਰ *s, uint16_t *ret) {
        uint16_t l;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        r = ਸੁਰੱਖਿਅਤ_atou16_ਭਰਿਆ(s, ਸੁਰੱਖਿਅਤ_ATO_REFUSE_LEADING_WHITESPACE, &l);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (l == 0)
                ਵਾਪਸ -EINVAL;

        *ret = l;

        ਵਾਪਸ 0;
}

ਪੂਰਨ parse_ip_ਪੋਰਟ_range(ਅਚਲ ਅੱਖਰ *s, uint16_t *low, uint16_t *high, bool allow_zero) {
        ਬਿਨਾਨਿਸ਼ਾਨੀ l, h;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(low);
        ਪ੍ਰਤਿਜ੍ਞਾ(high);

        r = parse_range(s, &l, &h);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (l > 65535 || h > 65535)
                ਵਾਪਸ -EINVAL;

        ਜੇ (!allow_zero && (l == 0 || h == 0))
                ਵਾਪਸ -EINVAL;

        ਜੇ (h < l)
                ਵਾਪਸ -EINVAL;

        *low = l;
        *high = h;

        ਵਾਪਸ 0;
}

ਪੂਰਨ parse_oom_score_adjust(ਅਚਲ ਅੱਖਰ *s, ਪੂਰਨ *ret) {
        ਪੂਰਨ r, v;

        ਪ੍ਰਤਿਜ੍ਞਾ(s);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        r = ਸੁਰੱਖਿਅਤ_atoi(s, &v);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (!oom_score_adjust_is_valid(v))
                ਵਾਪਸ -ERANGE;

        *ret = v;
        ਵਾਪਸ 0;
}

ਪੂਰਨ store_loadavg_fixed_ਬਿੰਦੂ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ i, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ f, loadavg_t *ret) {
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        /* Validate both parts *before* shifting them, so that the shifts themselves cannot overflow.
         * Rejecting f >= 100 is equivalent to the old check of the shifted value against
         * LOADAVG_FIXED_POINT_1_0, since DIV_ROUND_UP(99 << 11, 100) == 2028 and
         * DIV_ROUND_UP(100 << 11, 100) == 2048. */
        ਜੇ (i > (~0UL >> LOADAVG_PRECISION_ਬਿੱਟ))
                ਵਾਪਸ -ERANGE;

        ਜੇ (f >= 100)
                ਵਾਪਸ -ERANGE;

        *ret = (i << LOADAVG_PRECISION_ਬਿੱਟ) | DIV_ਗੇੜਾ_ਉੱਪਰ(f << LOADAVG_PRECISION_ਬਿੱਟ, 100);
        ਵਾਪਸ 0;
}

ਪੂਰਨ parse_loadavg_fixed_ਬਿੰਦੂ(ਅਚਲ ਅੱਖਰ *s, loadavg_t *ret) {
        ਅਚਲ ਅੱਖਰ *d, *f_str, *i_str;
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ i, f;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(s);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        d = strchr(s, '.');
        ਜੇ (!d)
                ਵਾਪਸ -EINVAL;

        i_str = strndupa_ਸੁਰੱਖਿਅਤ(s, d - s);
        f_str = d + 1;

        r = ਸੁਰੱਖਿਅਤ_atolu_ਭਰਿਆ(i_str, 10, &i);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        r = ਸੁਰੱਖਿਅਤ_atolu_ਭਰਿਆ(f_str, 10, &f);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ store_loadavg_fixed_ਬਿੰਦੂ(i, f, ret);
}

/* Limitations are described in https://www.netfilter.org/projects/nftables/manpage.html and
 * https://bugzilla.netfilter.org/show_bug.cgi?id=1175 */
bool nft_identifier_valid(ਅਚਲ ਅੱਖਰ *id) {
        ਜੇ (isempty(id))
                ਵਾਪਸ false;

        ਜੇ (ਲੜੀ_ਲੰਬਾਈ(id) >= NFT_NAME_MAXLEN)
                ਵਾਪਸ false;

        ਜੇ (!ascii_isalpha(id[0]))
                ਵਾਪਸ false;

        ਵਾਪਸ ਵਿੱਚ_charset(id + 1, ALPHANUMERICAL "/\\_.");
}

ਪੂਰਨ parse_capability_ਸੈੱਟ(ਅਚਲ ਅੱਖਰ *s, uint64_t initial, uint64_t *current) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(s);
        ਪ੍ਰਤਿਜ੍ਞਾ(current);

        ਜੇ (isempty(s)) {
                *current = CAP_ਨਕਾਬ_UNSET;
                ਵਾਪਸ 1;
        }

        bool invert = false;
        ਜੇ (s[0] == '~') {
                invert = true;
                s++;
        }

        uint64_t parsed;
        r = capability_ਸੈੱਟ_from_string(s, &parsed);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (parsed == 0 || *current == initial)
                /* "~" or uninitialized data -> replace */
                *current = invert ? all_capabilities() & ~parsed : parsed;
        ਨਹੀਂਤਾਂ {
                /* previous data -> merge */
                ਜੇ (invert)
                        *current &= ~parsed;
                ਨਹੀਂਤਾਂ
                        *current |= parsed;
        }

        ਵਾਪਸ r;
}
