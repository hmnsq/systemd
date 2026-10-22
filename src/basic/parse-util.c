/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ <linux/ipv6.h>
#അന്തര്ഭാവ <linux/netfilter/nf_tables.h>
#അന്തര്ഭാവ <stdio.h>
#അന്തര്ഭാവ <stdlib.h>
#അന്തര്ഭാവ <sys/socket.h>

#അന്തര്ഭാവ "alloc-util.h"
#അന്തര്ഭാവ "capability-list.h"
#അന്തര്ഭാവ "capability-util.h"
#അന്തര്ഭാവ "errno-list.h"
#അന്തര്ഭാവ "extract-word.h"
#അന്തര്ഭാവ "locale-util.h"
#അന്തര്ഭാവ "log.h"
#അന്തര്ഭാവ "missing-network.h"
#അന്തര്ഭാവ "parse-util.h"
#അന്തര്ഭാവ "path-util.h"
#അന്തര്ഭാവ "process-util.h"
#അന്തര്ഭാവ "string-util.h"
#അന്തര്ഭാവ "strv.h"

പൂർണ്ണസംഖ്യ parse_boolean(സ്ഥിരം അക്ഷരം *v) {
        എങ്കിൽ (!v)
                തിരികെ -EINVAL;

        എങ്കിൽ (STRCASE_IN_SET(v,
                           "1",
                           "yes",
                           "y",
                           "true",
                           "t",
                           "on"))
                തിരികെ 1;

        എങ്കിൽ (STRCASE_IN_SET(v,
                           "0",
                           "no",
                           "n",
                           "false",
                           "f",
                           "off"))
                തിരികെ 0;

        തിരികെ -EINVAL;
}

പൂർണ്ണസംഖ്യ parse_tristate_full(സ്ഥിരം അക്ഷരം *v, സ്ഥിരം അക്ഷരം *third, പൂർണ്ണസംഖ്യ *ret) {
        പൂർണ്ണസംഖ്യ r;

        എങ്കിൽ (isempty(v) || streq_ptr(v, third)) { /* Empty string is always taken as the third/invalid/auto state */
                എങ്കിൽ (ret)
                        *ret = -1;
        } അല്ലെങ്കിൽ {
                r = parse_boolean(v);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                എങ്കിൽ (ret)
                        *ret = r;
        }

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ parse_pid(സ്ഥിരം അക്ഷരം *s, pid_t *ret) {
        unsigned long ul = 0;
        pid_t pid;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(s);

        r = safe_atolu(s, &ul);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        pid = (pid_t) ul;

        എങ്കിൽ ((unsigned long) pid != ul)
                തിരികെ -ERANGE;

        എങ്കിൽ (!pid_is_valid(pid))
                തിരികെ -ERANGE;

        എങ്കിൽ (ret)
                *ret = pid;
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ parse_mode(സ്ഥിരം അക്ഷരം *s, mode_t *ret) {
        unsigned m;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(s);

        r = safe_atou_full(s, 8 |
                           SAFE_ATO_REFUSE_PLUS_MINUS, /* Leading '+' or even '-' char? that's just weird,
                                                        * refuse. User might have wanted to add mode flags or
                                                        * so, but this parser doesn't allow that, so let's
                                                        * better be safe. */
                           &m);
        എങ്കിൽ (r < 0)
                തിരികെ r;
        എങ്കിൽ (m > 07777)
                തിരികെ -ERANGE;

        എങ്കിൽ (ret)
                *ret = m;
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ parse_ifindex(സ്ഥിരം അക്ഷരം *s) {
        പൂർണ്ണസംഖ്യ ifi, r;

        പ്രതിജ്ഞാ(s);

        r = safe_atoi(s, &ifi);
        എങ്കിൽ (r < 0)
                തിരികെ r;
        എങ്കിൽ (ifi <= 0)
                തിരികെ -EINVAL;

        തിരികെ ifi;
}

പൂർണ്ണസംഖ്യ parse_mtu(പൂർണ്ണസംഖ്യ family, സ്ഥിരം അക്ഷരം *s, uint32_t *ret) {
        uint64_t u, m;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(ret);

        r = parse_size(s, 1024, &u);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (u > UINT32_MAX)
                തിരികെ -ERANGE;

        തിരഞ്ഞെടുപ്പ് (family) {
        അവസ്ഥ AF_INET:
                m = IPV4_MIN_MTU; /* This is 68 */
                നിർത്തുക;
        അവസ്ഥ AF_INET6:
                m = IPV6_MIN_MTU; /* This is 1280 */
                നിർത്തുക;
        സ്ഥിരസ്ഥിതി:
                m = 0;
        }

        എങ്കിൽ (u < m)
                തിരികെ -ERANGE;

        *ret = (uint32_t) u;
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ parse_size(സ്ഥിരം അക്ഷരം *t, uint64_t base, uint64_t *size) {

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

        ഘടന table {
                സ്ഥിരം അക്ഷരം *suffix;
                unsigned long long factor;
        };

        സ്ഥിതം സ്ഥിരം ഘടന table iec[] = {
                { "E", 1024ULL*1024ULL*1024ULL*1024ULL*1024ULL*1024ULL },
                { "P", 1024ULL*1024ULL*1024ULL*1024ULL*1024ULL },
                { "T", 1024ULL*1024ULL*1024ULL*1024ULL },
                { "G", 1024ULL*1024ULL*1024ULL },
                { "M", 1024ULL*1024ULL },
                { "K", 1024ULL },
                { "B", 1ULL },
                { "",  1ULL },
        };

        സ്ഥിതം സ്ഥിരം ഘടന table si[] = {
                { "E", 1000ULL*1000ULL*1000ULL*1000ULL*1000ULL*1000ULL },
                { "P", 1000ULL*1000ULL*1000ULL*1000ULL*1000ULL },
                { "T", 1000ULL*1000ULL*1000ULL*1000ULL },
                { "G", 1000ULL*1000ULL*1000ULL },
                { "M", 1000ULL*1000ULL },
                { "K", 1000ULL },
                { "B", 1ULL },
                { "",  1ULL },
        };

        സ്ഥിരം ഘടന table *table;
        സ്ഥിരം അക്ഷരം *p;
        unsigned long long r = 0;
        unsigned n_entries, start_pos = 0;

        പ്രതിജ്ഞാ(t);
        പ്രതിജ്ഞാ(IN_SET(base, 1000, 1024));
        പ്രതിജ്ഞാ(size);

        എങ്കിൽ (base == 1000) {
                table = si;
                n_entries = ELEMENTSOF(si);
        } അല്ലെങ്കിൽ {
                table = iec;
                n_entries = ELEMENTSOF(iec);
        }

        p = t;
        ചെയ്യുക {
                unsigned long long l, tmp;
                ഇരട്ട frac = 0;
                അക്ഷരം *e;
                unsigned i;

                p += strspn(p, WHITESPACE);

                errno = 0;
                l = strtoull(p, &e, 10);
                എങ്കിൽ (errno > 0)
                        തിരികെ -errno;
                എങ്കിൽ (e == p)
                        തിരികെ -EINVAL;
                എങ്കിൽ (*p == '-')
                        തിരികെ -ERANGE;

                എങ്കിൽ (*e == '.') {
                        e++;

                        /* strtoull() itself would accept space/+/- */
                        എങ്കിൽ (ascii_isdigit(*e)) {
                                unsigned long long l2;
                                അക്ഷരം *e2;

                                l2 = strtoull(e, &e2, 10);
                                എങ്കിൽ (errno > 0)
                                        തിരികെ -errno;

                                /* Ignore failure. E.g. 10.M is valid */
                                frac = l2;
                                വേണ്ടി (; e < e2; e++)
                                        frac /= 10;
                        }
                }

                e += strspn(e, WHITESPACE);

                വേണ്ടി (i = start_pos; i < n_entries; i++)
                        എങ്കിൽ (startswith(e, table[i].suffix))
                                നിർത്തുക;

                എങ്കിൽ (i >= n_entries)
                        തിരികെ -EINVAL;

                എങ്കിൽ (l + (frac > 0) > ULLONG_MAX / table[i].factor)
                        തിരികെ -ERANGE;

                tmp = l * table[i].factor + (unsigned long long) (frac * table[i].factor);
                എങ്കിൽ (tmp > ULLONG_MAX - r)
                        തിരികെ -ERANGE;

                r += tmp;
                എങ്കിൽ ((unsigned long long) (uint64_t) r != r)
                        തിരികെ -ERANGE;

                p = e + strlen(table[i].suffix);

                start_pos = i + 1;

        } ആകുമ്പോൾ (*p);

        *size = r;

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ parse_sector_size(സ്ഥിരം അക്ഷരം *t, uint64_t *ret) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(t);
        പ്രതിജ്ഞാ(ret);

        uint64_t ss;

        r = safe_atou64(t, &ss);
        എങ്കിൽ (r < 0)
                തിരികെ log_error_errno(r, "Failed to parse sector size parameter %s", t);
        എങ്കിൽ (ss < 512 || ss > 4096) /* Allow up to 4K due to dm-crypt support and 4K alignment by the homed LUKS backend */
                തിരികെ log_error_errno(SYNTHETIC_ERRNO(ERANGE), "Sector size not between 512 and 4096: %s", t);
        എങ്കിൽ (!ISPOWEROF2(ss))
                തിരികെ log_error_errno(SYNTHETIC_ERRNO(EINVAL), "Sector size not power of 2: %s", t);

        *ret = ss;
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ parse_range(സ്ഥിരം അക്ഷരം *t, unsigned *lower, unsigned *upper) {
        _cleanup_free_ അക്ഷരം *word = ശൂന്യ;
        unsigned l, u;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(lower);
        പ്രതിജ്ഞാ(upper);

        /* Extract the lower bound. */
        r = extract_first_word(&t, &word, "-", EXTRACT_DONT_COALESCE_SEPARATORS);
        എങ്കിൽ (r < 0)
                തിരികെ r;
        എങ്കിൽ (r == 0)
                തിരികെ -EINVAL;

        r = safe_atou(word, &l);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        /* Check for the upper bound and extract it if needed */
        എങ്കിൽ (!t)
                /* Single number with no dashes. */
                u = l;
        അല്ലെങ്കിൽ എങ്കിൽ (!*t)
                /* Trailing dash is an error. */
                തിരികെ -EINVAL;
        അല്ലെങ്കിൽ {
                r = safe_atou(t, &u);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
        }

        *lower = l;
        *upper = u;
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ parse_errno(സ്ഥിരം അക്ഷരം *t) {
        പൂർണ്ണസംഖ്യ r, e;

        പ്രതിജ്ഞാ(t);

        r = errno_from_name(t);
        എങ്കിൽ (r > 0)
                തിരികെ r;

        r = safe_atoi(t, &e);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        /* 0 is also allowed here */
        എങ്കിൽ (!errno_is_valid(e) && e != 0)
                തിരികെ -ERANGE;

        തിരികെ e;
}

പൂർണ്ണസംഖ്യ parse_fd(സ്ഥിരം അക്ഷരം *t) {
        പൂർണ്ണസംഖ്യ r, fd;

        പ്രതിജ്ഞാ(t);

        r = safe_atoi(t, &fd);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (fd < 0)
                തിരികെ -EBADF;

        തിരികെ fd;
}

പൂർണ്ണസംഖ്യ parse_user_shell(സ്ഥിരം അക്ഷരം *s, അക്ഷരം **ret_sh, bool *ret_copy) {
        അക്ഷരം *sh;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(ret_sh);
        പ്രതിജ്ഞാ(ret_copy);

        എങ്കിൽ (path_is_absolute(s) && path_is_normalized(s)) {
                sh = strdup(s);
                എങ്കിൽ (!sh)
                        തിരികെ -ENOMEM;

                *ret_sh = sh;
                *ret_copy = false;
        } അല്ലെങ്കിൽ {
                r = parse_boolean(s);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                *ret_sh = ശൂന്യ;
                *ret_copy = r;
        }

        തിരികെ 0;
}

സ്ഥിതം സ്ഥിരം അക്ഷരം *mangle_base(സ്ഥിരം അക്ഷരം *s, unsigned *base) {
        സ്ഥിരം അക്ഷരം *k;

        പ്രതിജ്ഞാ(s);
        പ്രതിജ്ഞാ(base);

        /* Base already explicitly specified, then don't do anything. */
        എങ്കിൽ (SAFE_ATO_MASK_FLAGS(*base) != 0)
                തിരികെ s;

        /* Support Python 3 style "0b" and 0x" prefixes, because they truly make sense, much more than C's "0" prefix for octal. */
        k = STARTSWITH_SET(s, "0b", "0B");
        എങ്കിൽ (k) {
                *base = 2 | (*base & SAFE_ATO_ALL_FLAGS);
                തിരികെ k;
        }

        k = STARTSWITH_SET(s, "0o", "0O");
        എങ്കിൽ (k) {
                *base = 8 | (*base & SAFE_ATO_ALL_FLAGS);
                തിരികെ k;
        }

        തിരികെ s;
}

പൂർണ്ണസംഖ്യ safe_atou_full(സ്ഥിരം അക്ഷരം *s, unsigned base, unsigned *ret_u) {
        അക്ഷരം *x = ശൂന്യ;
        unsigned long l;

        പ്രതിജ്ഞാ(s);
        പ്രതിജ്ഞാ(SAFE_ATO_MASK_FLAGS(base) <= 16);

        /* strtoul() is happy to parse negative values, and silently converts them to unsigned values without
         * generating an error. We want a clean error, hence let's look for the "-" prefix on our own, and
         * generate an error. But let's do so only after strtoul() validated that the string is clean
         * otherwise, so that we return EINVAL preferably over ERANGE. */

        എങ്കിൽ (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_WHITESPACE) &&
            strchr(WHITESPACE, s[0]))
                തിരികെ -EINVAL;

        s += strspn(s, WHITESPACE);

        എങ്കിൽ (FLAGS_SET(base, SAFE_ATO_REFUSE_PLUS_MINUS) &&
            IN_SET(s[0], '+', '-'))
                തിരികെ -EINVAL; /* Note that we check the "-" prefix again a second time below, but return a
                                 * different error. I.e. if the SAFE_ATO_REFUSE_PLUS_MINUS flag is set we
                                 * blanket refuse +/- prefixed integers, while if it is missing we'll just
                                 * return ERANGE, because the string actually parses correctly, but doesn't
                                 * fit in the return type. */

        എങ്കിൽ (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_ZERO) &&
            s[0] == '0' && !streq(s, "0"))
                തിരികെ -EINVAL; /* This is particularly useful to avoid ambiguities between C's octal
                                 * notation and assumed-to-be-decimal integers with a leading zero. */

        s = mangle_base(s, &base);

        errno = 0;
        l = strtoul(s, &x, SAFE_ATO_MASK_FLAGS(base) /* Let's mask off the flags bits so that only the actual
                                                      * base is left */);
        എങ്കിൽ (errno > 0)
                തിരികെ -errno;
        എങ്കിൽ (!x || x == s || *x != 0)
                തിരികെ -EINVAL;
        എങ്കിൽ (l != 0 && s[0] == '-')
                തിരികെ -ERANGE;
        എങ്കിൽ ((unsigned long) (unsigned) l != l)
                തിരികെ -ERANGE;

        എങ്കിൽ (ret_u)
                *ret_u = (unsigned) l;

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ safe_atou_bounded(സ്ഥിരം അക്ഷരം *s, unsigned min, unsigned max, unsigned *ret) {
        unsigned v;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(ret);

        r = safe_atou(s, &v);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (v < min || v > max)
                തിരികെ -ERANGE;

        *ret = v;
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ safe_atoi(സ്ഥിരം അക്ഷരം *s, പൂർണ്ണസംഖ്യ *ret_i) {
        unsigned base = 0;
        അക്ഷരം *x = ശൂന്യ;
        long l;

        പ്രതിജ്ഞാ(s);

        s += strspn(s, WHITESPACE);
        s = mangle_base(s, &base);

        errno = 0;
        l = strtol(s, &x, base);
        എങ്കിൽ (errno > 0)
                തിരികെ -errno;
        എങ്കിൽ (!x || x == s || *x != 0)
                തിരികെ -EINVAL;
        എങ്കിൽ ((long) (പൂർണ്ണസംഖ്യ) l != l)
                തിരികെ -ERANGE;

        എങ്കിൽ (ret_i)
                *ret_i = (പൂർണ്ണസംഖ്യ) l;

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ safe_atollu_full(സ്ഥിരം അക്ഷരം *s, unsigned base, unsigned long long *ret_llu) {
        അക്ഷരം *x = ശൂന്യ;
        unsigned long long l;

        പ്രതിജ്ഞാ(s);
        പ്രതിജ്ഞാ(SAFE_ATO_MASK_FLAGS(base) <= 16);

        എങ്കിൽ (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_WHITESPACE) &&
            strchr(WHITESPACE, s[0]))
                തിരികെ -EINVAL;

        s += strspn(s, WHITESPACE);

        എങ്കിൽ (FLAGS_SET(base, SAFE_ATO_REFUSE_PLUS_MINUS) &&
            IN_SET(s[0], '+', '-'))
                തിരികെ -EINVAL;

        എങ്കിൽ (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_ZERO) &&
            s[0] == '0' && s[1] != 0)
                തിരികെ -EINVAL;

        s = mangle_base(s, &base);

        errno = 0;
        l = strtoull(s, &x, SAFE_ATO_MASK_FLAGS(base));
        എങ്കിൽ (errno > 0)
                തിരികെ -errno;
        എങ്കിൽ (!x || x == s || *x != 0)
                തിരികെ -EINVAL;
        എങ്കിൽ (l != 0 && s[0] == '-')
                തിരികെ -ERANGE;

        എങ്കിൽ (ret_llu)
                *ret_llu = l;

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ safe_atolli(സ്ഥിരം അക്ഷരം *s, long long *ret_lli) {
        unsigned base = 0;
        അക്ഷരം *x = ശൂന്യ;
        long long l;

        പ്രതിജ്ഞാ(s);

        s += strspn(s, WHITESPACE);
        s = mangle_base(s, &base);

        errno = 0;
        l = strtoll(s, &x, base);
        എങ്കിൽ (errno > 0)
                തിരികെ -errno;
        എങ്കിൽ (!x || x == s || *x != 0)
                തിരികെ -EINVAL;

        എങ്കിൽ (ret_lli)
                *ret_lli = l;

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ safe_atou8_full(സ്ഥിരം അക്ഷരം *s, unsigned base, uint8_t *ret) {
        unsigned u;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(ret);

        r = safe_atou_full(s, base, &u);
        എങ്കിൽ (r < 0)
                തിരികെ r;
        എങ്കിൽ (u > UINT8_MAX)
                തിരികെ -ERANGE;

        *ret = (uint8_t) u;
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ safe_atou16_full(സ്ഥിരം അക്ഷരം *s, unsigned base, uint16_t *ret) {
        unsigned u;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(ret);

        r = safe_atou_full(s, base, &u);
        എങ്കിൽ (r < 0)
                തിരികെ r;
        എങ്കിൽ (u > UINT16_MAX)
                തിരികെ -ERANGE;

        *ret = (uint16_t) u;
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ safe_atoi16(സ്ഥിരം അക്ഷരം *s, int16_t *ret) {
        unsigned base = 0;
        അക്ഷരം *x = ശൂന്യ;
        long l;

        പ്രതിജ്ഞാ(s);

        s += strspn(s, WHITESPACE);
        s = mangle_base(s, &base);

        errno = 0;
        l = strtol(s, &x, base);
        എങ്കിൽ (errno > 0)
                തിരികെ -errno;
        എങ്കിൽ (!x || x == s || *x != 0)
                തിരികെ -EINVAL;
        എങ്കിൽ ((long) (int16_t) l != l)
                തിരികെ -ERANGE;

        എങ്കിൽ (ret)
                *ret = (int16_t) l;

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ safe_atod(സ്ഥിരം അക്ഷരം *s, ഇരട്ട *ret_d) {
        _cleanup_(freelocalep) locale_t loc = (locale_t) 0;
        അക്ഷരം *x = ശൂന്യ;
        ഇരട്ട d = 0;

        പ്രതിജ്ഞാ(s);

        loc = newlocale(LC_NUMERIC_MASK, "C", (locale_t) 0);
        എങ്കിൽ (loc == (locale_t) 0)
                തിരികെ -errno;

        errno = 0;
        d = strtod_l(s, &x, loc);
        എങ്കിൽ (errno > 0)
                തിരികെ -errno;
        എങ്കിൽ (!x || x == s || *x != 0)
                തിരികെ -EINVAL;

        എങ്കിൽ (ret_d)
                *ret_d = d;

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ parse_fractional_part_u(സ്ഥിരം അക്ഷരം **p, size_t digits, unsigned *res) {
        unsigned val = 0;
        സ്ഥിരം അക്ഷരം *s;

        പ്രതിജ്ഞാ(p);
        പ്രതിജ്ഞാ(res);

        s = *p;

        /* accept any number of digits, strtoull is limited to 19 */
        വേണ്ടി (size_t i = 0; i < digits; i++, s++) {
                എങ്കിൽ (!ascii_isdigit(*s)) {
                        എങ്കിൽ (i == 0)
                                തിരികെ -EINVAL;

                        /* too few digits, pad with 0 */
                        വേണ്ടി (; i < digits; i++)
                                val *= 10;

                        നിർത്തുക;
                }

                val *= 10;
                val += *s - '0';
        }

        /* maybe round up */
        എങ്കിൽ (*s >= '5' && *s <= '9')
                val++;

        s += strspn(s, DIGITS);

        *p = s;
        *res = val;

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ parse_nice(സ്ഥിരം അക്ഷരം *s, പൂർണ്ണസംഖ്യ *ret) {
        പൂർണ്ണസംഖ്യ n, r;

        പ്രതിജ്ഞാ(ret);

        r = safe_atoi(s, &n);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (!nice_is_valid(n))
                തിരികെ -ERANGE;

        *ret = n;
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ parse_ip_port(സ്ഥിരം അക്ഷരം *s, uint16_t *ret) {
        uint16_t l;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(ret);

        r = safe_atou16_full(s, SAFE_ATO_REFUSE_LEADING_WHITESPACE, &l);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (l == 0)
                തിരികെ -EINVAL;

        *ret = l;

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ parse_ip_port_range(സ്ഥിരം അക്ഷരം *s, uint16_t *low, uint16_t *high, bool allow_zero) {
        unsigned l, h;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(low);
        പ്രതിജ്ഞാ(high);

        r = parse_range(s, &l, &h);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (l > 65535 || h > 65535)
                തിരികെ -EINVAL;

        എങ്കിൽ (!allow_zero && (l == 0 || h == 0))
                തിരികെ -EINVAL;

        എങ്കിൽ (h < l)
                തിരികെ -EINVAL;

        *low = l;
        *high = h;

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ parse_oom_score_adjust(സ്ഥിരം അക്ഷരം *s, പൂർണ്ണസംഖ്യ *ret) {
        പൂർണ്ണസംഖ്യ r, v;

        പ്രതിജ്ഞാ(s);
        പ്രതിജ്ഞാ(ret);

        r = safe_atoi(s, &v);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (!oom_score_adjust_is_valid(v))
                തിരികെ -ERANGE;

        *ret = v;
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ store_loadavg_fixed_point(unsigned long i, unsigned long f, loadavg_t *ret) {
        പ്രതിജ്ഞാ(ret);

        /* Validate both parts *before* shifting them, so that the shifts themselves cannot overflow.
         * Rejecting f >= 100 is equivalent to the old check of the shifted value against
         * LOADAVG_FIXED_POINT_1_0, since DIV_ROUND_UP(99 << 11, 100) == 2028 and
         * DIV_ROUND_UP(100 << 11, 100) == 2048. */
        എങ്കിൽ (i > (~0UL >> LOADAVG_PRECISION_BITS))
                തിരികെ -ERANGE;

        എങ്കിൽ (f >= 100)
                തിരികെ -ERANGE;

        *ret = (i << LOADAVG_PRECISION_BITS) | DIV_ROUND_UP(f << LOADAVG_PRECISION_BITS, 100);
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ parse_loadavg_fixed_point(സ്ഥിരം അക്ഷരം *s, loadavg_t *ret) {
        സ്ഥിരം അക്ഷരം *d, *f_str, *i_str;
        unsigned long i, f;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(s);
        പ്രതിജ്ഞാ(ret);

        d = strchr(s, '.');
        എങ്കിൽ (!d)
                തിരികെ -EINVAL;

        i_str = strndupa_safe(s, d - s);
        f_str = d + 1;

        r = safe_atolu_full(i_str, 10, &i);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        r = safe_atolu_full(f_str, 10, &f);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ store_loadavg_fixed_point(i, f, ret);
}

/* Limitations are described in https://www.netfilter.org/projects/nftables/manpage.html and
 * https://bugzilla.netfilter.org/show_bug.cgi?id=1175 */
bool nft_identifier_valid(സ്ഥിരം അക്ഷരം *id) {
        എങ്കിൽ (isempty(id))
                തിരികെ false;

        എങ്കിൽ (strlen(id) >= NFT_NAME_MAXLEN)
                തിരികെ false;

        എങ്കിൽ (!ascii_isalpha(id[0]))
                തിരികെ false;

        തിരികെ in_charset(id + 1, ALPHANUMERICAL "/\\_.");
}

പൂർണ്ണസംഖ്യ parse_capability_set(സ്ഥിരം അക്ഷരം *s, uint64_t initial, uint64_t *current) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(s);
        പ്രതിജ്ഞാ(current);

        എങ്കിൽ (isempty(s)) {
                *current = CAP_MASK_UNSET;
                തിരികെ 1;
        }

        bool invert = false;
        എങ്കിൽ (s[0] == '~') {
                invert = true;
                s++;
        }

        uint64_t parsed;
        r = capability_set_from_string(s, &parsed);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (parsed == 0 || *current == initial)
                /* "~" or uninitialized data -> replace */
                *current = invert ? all_capabilities() & ~parsed : parsed;
        അല്ലെങ്കിൽ {
                /* previous data -> merge */
                എങ്കിൽ (invert)
                        *current &= ~parsed;
                അല്ലെങ്കിൽ
                        *current |= parsed;
        }

        തിരികെ r;
}
