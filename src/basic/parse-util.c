/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ <linux/ipv6.h>
#ଅନ୍ତର୍ଭାଵ <linux/netfilter/nf_tables.h>
#ଅନ୍ତର୍ଭାଵ <stdio.h>
#ଅନ୍ତର୍ଭାଵ <stdlib.h>
#ଅନ୍ତର୍ଭାଵ <sys/socket.h>

#ଅନ୍ତର୍ଭାଵ "alloc-util.h"
#ଅନ୍ତର୍ଭାଵ "capability-list.h"
#ଅନ୍ତର୍ଭାଵ "capability-util.h"
#ଅନ୍ତର୍ଭାଵ "errno-list.h"
#ଅନ୍ତର୍ଭାଵ "extract-word.h"
#ଅନ୍ତର୍ଭାଵ "locale-util.h"
#ଅନ୍ତର୍ଭାଵ "log.h"
#ଅନ୍ତର୍ଭାଵ "missing-network.h"
#ଅନ୍ତର୍ଭାଵ "parse-util.h"
#ଅନ୍ତର୍ଭାଵ "path-util.h"
#ଅନ୍ତର୍ଭାଵ "process-util.h"
#ଅନ୍ତର୍ଭାଵ "string-util.h"
#ଅନ୍ତର୍ଭାଵ "strv.h"

ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_boolean(ଅଚଳ ଅକ୍ଷର *v) {
        ଯଦି (!v)
                ଫେରନ୍ତୁ -EINVAL;

        ଯଦି (STRCASE_IN_SET(v,
                           "1",
                           "yes",
                           "y",
                           "true",
                           "t",
                           "on"))
                ଫେରନ୍ତୁ 1;

        ଯଦି (STRCASE_IN_SET(v,
                           "0",
                           "no",
                           "n",
                           "false",
                           "f",
                           "off"))
                ଫେରନ୍ତୁ 0;

        ଫେରନ୍ତୁ -EINVAL;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_tristate_full(ଅଚଳ ଅକ୍ଷର *v, ଅଚଳ ଅକ୍ଷର *third, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ret) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ଯଦି (isempty(v) || streq_ptr(v, third)) { /* Empty string is always taken as the third/invalid/auto state */
                ଯଦି (ret)
                        *ret = -1;
        } ଅନ୍ୟଥା {
                r = parse_boolean(v);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                ଯଦି (ret)
                        *ret = r;
        }

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_pid(ଅଚଳ ଅକ୍ଷର *s, pid_t *ret) {
        unsigned long ul = 0;
        pid_t pid;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(s);

        r = safe_atolu(s, &ul);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        pid = (pid_t) ul;

        ଯଦି ((unsigned long) pid != ul)
                ଫେରନ୍ତୁ -ERANGE;

        ଯଦି (!pid_is_valid(pid))
                ଫେରନ୍ତୁ -ERANGE;

        ଯଦି (ret)
                *ret = pid;
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_mode(ଅଚଳ ଅକ୍ଷର *s, mode_t *ret) {
        unsigned m;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(s);

        r = safe_atou_full(s, 8 |
                           SAFE_ATO_REFUSE_PLUS_MINUS, /* Leading '+' or even '-' char? that's just weird,
                                                        * refuse. User might have wanted to add mode flags or
                                                        * so, but this parser doesn't allow that, so let's
                                                        * better be safe. */
                           &m);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;
        ଯଦି (m > 07777)
                ଫେରନ୍ତୁ -ERANGE;

        ଯଦି (ret)
                *ret = m;
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_ifindex(ଅଚଳ ଅକ୍ଷର *s) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା ifi, r;

        ପ୍ରତିଜ୍ଞା(s);

        r = safe_atoi(s, &ifi);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;
        ଯଦି (ifi <= 0)
                ଫେରନ୍ତୁ -EINVAL;

        ଫେରନ୍ତୁ ifi;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_mtu(ପୂର୍ଣ୍ଣସଂଖ୍ୟା family, ଅଚଳ ଅକ୍ଷର *s, uint32_t *ret) {
        uint64_t u, m;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(ret);

        r = parse_size(s, 1024, &u);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (u > UINT32_MAX)
                ଫେରନ୍ତୁ -ERANGE;

        ବାଛ (family) {
        ସ୍ଥିତି AF_INET:
                m = IPV4_MIN_MTU; /* This is 68 */
                ଭାଙ୍ଗ;
        ସ୍ଥିତି AF_INET6:
                m = IPV6_MIN_MTU; /* This is 1280 */
                ଭାଙ୍ଗ;
        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                m = 0;
        }

        ଯଦି (u < m)
                ଫେରନ୍ତୁ -ERANGE;

        *ret = (uint32_t) u;
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_size(ଅଚଳ ଅକ୍ଷର *t, uint64_t base, uint64_t *size) {

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

        ସଂରଚନା table {
                ଅଚଳ ଅକ୍ଷର *suffix;
                unsigned long long factor;
        };

        ସ୍ଥିର ଅଚଳ ସଂରଚନା table iec[] = {
                { "E", 1024ULL*1024ULL*1024ULL*1024ULL*1024ULL*1024ULL },
                { "P", 1024ULL*1024ULL*1024ULL*1024ULL*1024ULL },
                { "T", 1024ULL*1024ULL*1024ULL*1024ULL },
                { "G", 1024ULL*1024ULL*1024ULL },
                { "M", 1024ULL*1024ULL },
                { "K", 1024ULL },
                { "B", 1ULL },
                { "",  1ULL },
        };

        ସ୍ଥିର ଅଚଳ ସଂରଚନା table si[] = {
                { "E", 1000ULL*1000ULL*1000ULL*1000ULL*1000ULL*1000ULL },
                { "P", 1000ULL*1000ULL*1000ULL*1000ULL*1000ULL },
                { "T", 1000ULL*1000ULL*1000ULL*1000ULL },
                { "G", 1000ULL*1000ULL*1000ULL },
                { "M", 1000ULL*1000ULL },
                { "K", 1000ULL },
                { "B", 1ULL },
                { "",  1ULL },
        };

        ଅଚଳ ସଂରଚନା table *table;
        ଅଚଳ ଅକ୍ଷର *p;
        unsigned long long r = 0;
        unsigned n_entries, start_pos = 0;

        ପ୍ରତିଜ୍ଞା(t);
        ପ୍ରତିଜ୍ଞା(IN_SET(base, 1000, 1024));
        ପ୍ରତିଜ୍ଞା(size);

        ଯଦି (base == 1000) {
                table = si;
                n_entries = ELEMENTSOF(si);
        } ଅନ୍ୟଥା {
                table = iec;
                n_entries = ELEMENTSOF(iec);
        }

        p = t;
        କର {
                unsigned long long l, tmp;
                ଦ୍ୱିଗୁଣ frac = 0;
                ଅକ୍ଷର *e;
                unsigned i;

                p += strspn(p, WHITESPACE);

                errno = 0;
                l = strtoull(p, &e, 10);
                ଯଦି (errno > 0)
                        ଫେରନ୍ତୁ -errno;
                ଯଦି (e == p)
                        ଫେରନ୍ତୁ -EINVAL;
                ଯଦି (*p == '-')
                        ଫେରନ୍ତୁ -ERANGE;

                ଯଦି (*e == '.') {
                        e++;

                        /* strtoull() itself would accept space/+/- */
                        ଯଦି (ascii_isdigit(*e)) {
                                unsigned long long l2;
                                ଅକ୍ଷର *e2;

                                l2 = strtoull(e, &e2, 10);
                                ଯଦି (errno > 0)
                                        ଫେରନ୍ତୁ -errno;

                                /* Ignore failure. E.g. 10.M is valid */
                                frac = l2;
                                ପାଇଁ (; e < e2; e++)
                                        frac /= 10;
                        }
                }

                e += strspn(e, WHITESPACE);

                ପାଇଁ (i = start_pos; i < n_entries; i++)
                        ଯଦି (startswith(e, table[i].suffix))
                                ଭାଙ୍ଗ;

                ଯଦି (i >= n_entries)
                        ଫେରନ୍ତୁ -EINVAL;

                ଯଦି (l + (frac > 0) > ULLONG_MAX / table[i].factor)
                        ଫେରନ୍ତୁ -ERANGE;

                tmp = l * table[i].factor + (unsigned long long) (frac * table[i].factor);
                ଯଦି (tmp > ULLONG_MAX - r)
                        ଫେରନ୍ତୁ -ERANGE;

                r += tmp;
                ଯଦି ((unsigned long long) (uint64_t) r != r)
                        ଫେରନ୍ତୁ -ERANGE;

                p = e + strlen(table[i].suffix);

                start_pos = i + 1;

        } ଯେତେବେଳେ (*p);

        *size = r;

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_sector_size(ଅଚଳ ଅକ୍ଷର *t, uint64_t *ret) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(t);
        ପ୍ରତିଜ୍ଞା(ret);

        uint64_t ss;

        r = safe_atou64(t, &ss);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ log_error_errno(r, "Failed to parse sector size parameter %s", t);
        ଯଦି (ss < 512 || ss > 4096) /* Allow up to 4K due to dm-crypt support and 4K alignment by the homed LUKS backend */
                ଫେରନ୍ତୁ log_error_errno(SYNTHETIC_ERRNO(ERANGE), "Sector size not between 512 and 4096: %s", t);
        ଯଦି (!ISPOWEROF2(ss))
                ଫେରନ୍ତୁ log_error_errno(SYNTHETIC_ERRNO(EINVAL), "Sector size not power of 2: %s", t);

        *ret = ss;
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_range(ଅଚଳ ଅକ୍ଷର *t, unsigned *lower, unsigned *upper) {
        _cleanup_free_ ଅକ୍ଷର *word = ଖାଲି;
        unsigned l, u;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(lower);
        ପ୍ରତିଜ୍ଞା(upper);

        /* Extract the lower bound. */
        r = extract_first_word(&t, &word, "-", EXTRACT_DONT_COALESCE_SEPARATORS);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;
        ଯଦି (r == 0)
                ଫେରନ୍ତୁ -EINVAL;

        r = safe_atou(word, &l);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        /* Check for the upper bound and extract it if needed */
        ଯଦି (!t)
                /* Single number with no dashes. */
                u = l;
        ଅନ୍ୟଥା ଯଦି (!*t)
                /* Trailing dash is an error. */
                ଫେରନ୍ତୁ -EINVAL;
        ଅନ୍ୟଥା {
                r = safe_atou(t, &u);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
        }

        *lower = l;
        *upper = u;
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_errno(ଅଚଳ ଅକ୍ଷର *t) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r, e;

        ପ୍ରତିଜ୍ଞା(t);

        r = errno_from_name(t);
        ଯଦି (r > 0)
                ଫେରନ୍ତୁ r;

        r = safe_atoi(t, &e);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        /* 0 is also allowed here */
        ଯଦି (!errno_is_valid(e) && e != 0)
                ଫେରନ୍ତୁ -ERANGE;

        ଫେରନ୍ତୁ e;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_fd(ଅଚଳ ଅକ୍ଷର *t) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r, fd;

        ପ୍ରତିଜ୍ଞା(t);

        r = safe_atoi(t, &fd);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (fd < 0)
                ଫେରନ୍ତୁ -EBADF;

        ଫେରନ୍ତୁ fd;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_user_shell(ଅଚଳ ଅକ୍ଷର *s, ଅକ୍ଷର **ret_sh, bool *ret_copy) {
        ଅକ୍ଷର *sh;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(ret_sh);
        ପ୍ରତିଜ୍ଞା(ret_copy);

        ଯଦି (path_is_absolute(s) && path_is_normalized(s)) {
                sh = strdup(s);
                ଯଦି (!sh)
                        ଫେରନ୍ତୁ -ENOMEM;

                *ret_sh = sh;
                *ret_copy = false;
        } ଅନ୍ୟଥା {
                r = parse_boolean(s);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                *ret_sh = ଖାଲି;
                *ret_copy = r;
        }

        ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର *mangle_base(ଅଚଳ ଅକ୍ଷର *s, unsigned *base) {
        ଅଚଳ ଅକ୍ଷର *k;

        ପ୍ରତିଜ୍ଞା(s);
        ପ୍ରତିଜ୍ଞା(base);

        /* Base already explicitly specified, then don't do anything. */
        ଯଦି (SAFE_ATO_MASK_FLAGS(*base) != 0)
                ଫେରନ୍ତୁ s;

        /* Support Python 3 style "0b" and 0x" prefixes, because they truly make sense, much more than C's "0" prefix for octal. */
        k = STARTSWITH_SET(s, "0b", "0B");
        ଯଦି (k) {
                *base = 2 | (*base & SAFE_ATO_ALL_FLAGS);
                ଫେରନ୍ତୁ k;
        }

        k = STARTSWITH_SET(s, "0o", "0O");
        ଯଦି (k) {
                *base = 8 | (*base & SAFE_ATO_ALL_FLAGS);
                ଫେରନ୍ତୁ k;
        }

        ଫେରନ୍ତୁ s;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା safe_atou_full(ଅଚଳ ଅକ୍ଷର *s, unsigned base, unsigned *ret_u) {
        ଅକ୍ଷର *x = ଖାଲି;
        unsigned long l;

        ପ୍ରତିଜ୍ଞା(s);
        ପ୍ରତିଜ୍ଞା(SAFE_ATO_MASK_FLAGS(base) <= 16);

        /* strtoul() is happy to parse negative values, and silently converts them to unsigned values without
         * generating an error. We want a clean error, hence let's look for the "-" prefix on our own, and
         * generate an error. But let's do so only after strtoul() validated that the string is clean
         * otherwise, so that we return EINVAL preferably over ERANGE. */

        ଯଦି (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_WHITESPACE) &&
            strchr(WHITESPACE, s[0]))
                ଫେରନ୍ତୁ -EINVAL;

        s += strspn(s, WHITESPACE);

        ଯଦି (FLAGS_SET(base, SAFE_ATO_REFUSE_PLUS_MINUS) &&
            IN_SET(s[0], '+', '-'))
                ଫେରନ୍ତୁ -EINVAL; /* Note that we check the "-" prefix again a second time below, but return a
                                 * different error. I.e. if the SAFE_ATO_REFUSE_PLUS_MINUS flag is set we
                                 * blanket refuse +/- prefixed integers, while if it is missing we'll just
                                 * return ERANGE, because the string actually parses correctly, but doesn't
                                 * fit in the return type. */

        ଯଦି (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_ZERO) &&
            s[0] == '0' && !streq(s, "0"))
                ଫେରନ୍ତୁ -EINVAL; /* This is particularly useful to avoid ambiguities between C's octal
                                 * notation and assumed-to-be-decimal integers with a leading zero. */

        s = mangle_base(s, &base);

        errno = 0;
        l = strtoul(s, &x, SAFE_ATO_MASK_FLAGS(base) /* Let's mask off the flags bits so that only the actual
                                                      * base is left */);
        ଯଦି (errno > 0)
                ଫେରନ୍ତୁ -errno;
        ଯଦି (!x || x == s || *x != 0)
                ଫେରନ୍ତୁ -EINVAL;
        ଯଦି (l != 0 && s[0] == '-')
                ଫେରନ୍ତୁ -ERANGE;
        ଯଦି ((unsigned long) (unsigned) l != l)
                ଫେରନ୍ତୁ -ERANGE;

        ଯଦି (ret_u)
                *ret_u = (unsigned) l;

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା safe_atou_bounded(ଅଚଳ ଅକ୍ଷର *s, unsigned min, unsigned max, unsigned *ret) {
        unsigned v;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(ret);

        r = safe_atou(s, &v);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (v < min || v > max)
                ଫେରନ୍ତୁ -ERANGE;

        *ret = v;
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା safe_atoi(ଅଚଳ ଅକ୍ଷର *s, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ret_i) {
        unsigned base = 0;
        ଅକ୍ଷର *x = ଖାଲି;
        long l;

        ପ୍ରତିଜ୍ଞା(s);

        s += strspn(s, WHITESPACE);
        s = mangle_base(s, &base);

        errno = 0;
        l = strtol(s, &x, base);
        ଯଦି (errno > 0)
                ଫେରନ୍ତୁ -errno;
        ଯଦି (!x || x == s || *x != 0)
                ଫେରନ୍ତୁ -EINVAL;
        ଯଦି ((long) (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) l != l)
                ଫେରନ୍ତୁ -ERANGE;

        ଯଦି (ret_i)
                *ret_i = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) l;

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା safe_atollu_full(ଅଚଳ ଅକ୍ଷର *s, unsigned base, unsigned long long *ret_llu) {
        ଅକ୍ଷର *x = ଖାଲି;
        unsigned long long l;

        ପ୍ରତିଜ୍ଞା(s);
        ପ୍ରତିଜ୍ଞା(SAFE_ATO_MASK_FLAGS(base) <= 16);

        ଯଦି (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_WHITESPACE) &&
            strchr(WHITESPACE, s[0]))
                ଫେରନ୍ତୁ -EINVAL;

        s += strspn(s, WHITESPACE);

        ଯଦି (FLAGS_SET(base, SAFE_ATO_REFUSE_PLUS_MINUS) &&
            IN_SET(s[0], '+', '-'))
                ଫେରନ୍ତୁ -EINVAL;

        ଯଦି (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_ZERO) &&
            s[0] == '0' && s[1] != 0)
                ଫେରନ୍ତୁ -EINVAL;

        s = mangle_base(s, &base);

        errno = 0;
        l = strtoull(s, &x, SAFE_ATO_MASK_FLAGS(base));
        ଯଦି (errno > 0)
                ଫେରନ୍ତୁ -errno;
        ଯଦି (!x || x == s || *x != 0)
                ଫେରନ୍ତୁ -EINVAL;
        ଯଦି (l != 0 && s[0] == '-')
                ଫେରନ୍ତୁ -ERANGE;

        ଯଦି (ret_llu)
                *ret_llu = l;

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା safe_atolli(ଅଚଳ ଅକ୍ଷର *s, long long *ret_lli) {
        unsigned base = 0;
        ଅକ୍ଷର *x = ଖାଲି;
        long long l;

        ପ୍ରତିଜ୍ଞା(s);

        s += strspn(s, WHITESPACE);
        s = mangle_base(s, &base);

        errno = 0;
        l = strtoll(s, &x, base);
        ଯଦି (errno > 0)
                ଫେରନ୍ତୁ -errno;
        ଯଦି (!x || x == s || *x != 0)
                ଫେରନ୍ତୁ -EINVAL;

        ଯଦି (ret_lli)
                *ret_lli = l;

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା safe_atou8_full(ଅଚଳ ଅକ୍ଷର *s, unsigned base, uint8_t *ret) {
        unsigned u;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(ret);

        r = safe_atou_full(s, base, &u);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;
        ଯଦି (u > UINT8_MAX)
                ଫେରନ୍ତୁ -ERANGE;

        *ret = (uint8_t) u;
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା safe_atou16_full(ଅଚଳ ଅକ୍ଷର *s, unsigned base, uint16_t *ret) {
        unsigned u;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(ret);

        r = safe_atou_full(s, base, &u);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;
        ଯଦି (u > UINT16_MAX)
                ଫେରନ୍ତୁ -ERANGE;

        *ret = (uint16_t) u;
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା safe_atoi16(ଅଚଳ ଅକ୍ଷର *s, int16_t *ret) {
        unsigned base = 0;
        ଅକ୍ଷର *x = ଖାଲି;
        long l;

        ପ୍ରତିଜ୍ଞା(s);

        s += strspn(s, WHITESPACE);
        s = mangle_base(s, &base);

        errno = 0;
        l = strtol(s, &x, base);
        ଯଦି (errno > 0)
                ଫେରନ୍ତୁ -errno;
        ଯଦି (!x || x == s || *x != 0)
                ଫେରନ୍ତୁ -EINVAL;
        ଯଦି ((long) (int16_t) l != l)
                ଫେରନ୍ତୁ -ERANGE;

        ଯଦି (ret)
                *ret = (int16_t) l;

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା safe_atod(ଅଚଳ ଅକ୍ଷର *s, ଦ୍ୱିଗୁଣ *ret_d) {
        _cleanup_(freelocalep) locale_t loc = (locale_t) 0;
        ଅକ୍ଷର *x = ଖାଲି;
        ଦ୍ୱିଗୁଣ d = 0;

        ପ୍ରତିଜ୍ଞା(s);

        loc = newlocale(LC_NUMERIC_MASK, "C", (locale_t) 0);
        ଯଦି (loc == (locale_t) 0)
                ଫେରନ୍ତୁ -errno;

        errno = 0;
        d = strtod_l(s, &x, loc);
        ଯଦି (errno > 0)
                ଫେରନ୍ତୁ -errno;
        ଯଦି (!x || x == s || *x != 0)
                ଫେରନ୍ତୁ -EINVAL;

        ଯଦି (ret_d)
                *ret_d = d;

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_fractional_part_u(ଅଚଳ ଅକ୍ଷର **p, size_t digits, unsigned *res) {
        unsigned val = 0;
        ଅଚଳ ଅକ୍ଷର *s;

        ପ୍ରତିଜ୍ଞା(p);
        ପ୍ରତିଜ୍ଞା(res);

        s = *p;

        /* accept any number of digits, strtoull is limited to 19 */
        ପାଇଁ (size_t i = 0; i < digits; i++, s++) {
                ଯଦି (!ascii_isdigit(*s)) {
                        ଯଦି (i == 0)
                                ଫେରନ୍ତୁ -EINVAL;

                        /* too few digits, pad with 0 */
                        ପାଇଁ (; i < digits; i++)
                                val *= 10;

                        ଭାଙ୍ଗ;
                }

                val *= 10;
                val += *s - '0';
        }

        /* maybe round up */
        ଯଦି (*s >= '5' && *s <= '9')
                val++;

        s += strspn(s, DIGITS);

        *p = s;
        *res = val;

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_nice(ଅଚଳ ଅକ୍ଷର *s, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ret) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା n, r;

        ପ୍ରତିଜ୍ଞା(ret);

        r = safe_atoi(s, &n);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (!nice_is_valid(n))
                ଫେରନ୍ତୁ -ERANGE;

        *ret = n;
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_ip_port(ଅଚଳ ଅକ୍ଷର *s, uint16_t *ret) {
        uint16_t l;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(ret);

        r = safe_atou16_full(s, SAFE_ATO_REFUSE_LEADING_WHITESPACE, &l);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (l == 0)
                ଫେରନ୍ତୁ -EINVAL;

        *ret = l;

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_ip_port_range(ଅଚଳ ଅକ୍ଷର *s, uint16_t *low, uint16_t *high, bool allow_zero) {
        unsigned l, h;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(low);
        ପ୍ରତିଜ୍ଞା(high);

        r = parse_range(s, &l, &h);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (l > 65535 || h > 65535)
                ଫେରନ୍ତୁ -EINVAL;

        ଯଦି (!allow_zero && (l == 0 || h == 0))
                ଫେରନ୍ତୁ -EINVAL;

        ଯଦି (h < l)
                ଫେରନ୍ତୁ -EINVAL;

        *low = l;
        *high = h;

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_oom_score_adjust(ଅଚଳ ଅକ୍ଷର *s, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ret) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r, v;

        ପ୍ରତିଜ୍ଞା(s);
        ପ୍ରତିଜ୍ଞା(ret);

        r = safe_atoi(s, &v);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (!oom_score_adjust_is_valid(v))
                ଫେରନ୍ତୁ -ERANGE;

        *ret = v;
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା store_loadavg_fixed_point(unsigned long i, unsigned long f, loadavg_t *ret) {
        ପ୍ରତିଜ୍ଞା(ret);

        /* Validate both parts *before* shifting them, so that the shifts themselves cannot overflow.
         * Rejecting f >= 100 is equivalent to the old check of the shifted value against
         * LOADAVG_FIXED_POINT_1_0, since DIV_ROUND_UP(99 << 11, 100) == 2028 and
         * DIV_ROUND_UP(100 << 11, 100) == 2048. */
        ଯଦି (i > (~0UL >> LOADAVG_PRECISION_BITS))
                ଫେରନ୍ତୁ -ERANGE;

        ଯଦି (f >= 100)
                ଫେରନ୍ତୁ -ERANGE;

        *ret = (i << LOADAVG_PRECISION_BITS) | DIV_ROUND_UP(f << LOADAVG_PRECISION_BITS, 100);
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_loadavg_fixed_point(ଅଚଳ ଅକ୍ଷର *s, loadavg_t *ret) {
        ଅଚଳ ଅକ୍ଷର *d, *f_str, *i_str;
        unsigned long i, f;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(s);
        ପ୍ରତିଜ୍ଞା(ret);

        d = strchr(s, '.');
        ଯଦି (!d)
                ଫେରନ୍ତୁ -EINVAL;

        i_str = strndupa_safe(s, d - s);
        f_str = d + 1;

        r = safe_atolu_full(i_str, 10, &i);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        r = safe_atolu_full(f_str, 10, &f);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ store_loadavg_fixed_point(i, f, ret);
}

/* Limitations are described in https://www.netfilter.org/projects/nftables/manpage.html and
 * https://bugzilla.netfilter.org/show_bug.cgi?id=1175 */
bool nft_identifier_valid(ଅଚଳ ଅକ୍ଷର *id) {
        ଯଦି (isempty(id))
                ଫେରନ୍ତୁ false;

        ଯଦି (strlen(id) >= NFT_NAME_MAXLEN)
                ଫେରନ୍ତୁ false;

        ଯଦି (!ascii_isalpha(id[0]))
                ଫେରନ୍ତୁ false;

        ଫେରନ୍ତୁ in_charset(id + 1, ALPHANUMERICAL "/\\_.");
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_capability_set(ଅଚଳ ଅକ୍ଷର *s, uint64_t initial, uint64_t *current) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(s);
        ପ୍ରତିଜ୍ଞା(current);

        ଯଦି (isempty(s)) {
                *current = CAP_MASK_UNSET;
                ଫେରନ୍ତୁ 1;
        }

        bool invert = false;
        ଯଦି (s[0] == '~') {
                invert = true;
                s++;
        }

        uint64_t parsed;
        r = capability_set_from_string(s, &parsed);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (parsed == 0 || *current == initial)
                /* "~" or uninitialized data -> replace */
                *current = invert ? all_capabilities() & ~parsed : parsed;
        ଅନ୍ୟଥା {
                /* previous data -> merge */
                ଯଦି (invert)
                        *current &= ~parsed;
                ଅନ୍ୟଥା
                        *current |= parsed;
        }

        ଫେରନ୍ତୁ r;
}
