/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ <linux/ipv6.h>
#அந்தர்பாவ <linux/netfilter/nf_tables.h>
#அந்தர்பாவ <stdio.h>
#அந்தர்பாவ <stdlib.h>
#அந்தர்பாவ <sys/socket.h>

#அந்தர்பாவ "alloc-util.h"
#அந்தர்பாவ "capability-list.h"
#அந்தர்பாவ "capability-util.h"
#அந்தர்பாவ "errno-list.h"
#அந்தர்பாவ "extract-word.h"
#அந்தர்பாவ "locale-util.h"
#அந்தர்பாவ "log.h"
#அந்தர்பாவ "missing-network.h"
#அந்தர்பாவ "parse-util.h"
#அந்தர்பாவ "path-util.h"
#அந்தர்பாவ "process-util.h"
#அந்தர்பாவ "string-util.h"
#அந்தர்பாவ "strv.h"

முழு parse_boolean(மாறிலி எழுத்து *v) {
        எனில் (!v)
                திரும்பு -EINVAL;

        எனில் (STRCASE_IN_SET(v,
                           "1",
                           "yes",
                           "y",
                           "true",
                           "t",
                           "on"))
                திரும்பு 1;

        எனில் (STRCASE_IN_SET(v,
                           "0",
                           "no",
                           "n",
                           "false",
                           "f",
                           "off"))
                திரும்பு 0;

        திரும்பு -EINVAL;
}

முழு parse_tristate_full(மாறிலி எழுத்து *v, மாறிலி எழுத்து *third, முழு *ret) {
        முழு r;

        எனில் (isempty(v) || streq_ptr(v, third)) { /* Empty string is always taken as the third/invalid/auto state */
                எனில் (ret)
                        *ret = -1;
        } இல்லையெனில் {
                r = parse_boolean(v);
                எனில் (r < 0)
                        திரும்பு r;

                எனில் (ret)
                        *ret = r;
        }

        திரும்பு 0;
}

முழு parse_pid(மாறிலி எழுத்து *s, pid_t *ret) {
        unsigned long ul = 0;
        pid_t pid;
        முழு r;

        ப்ரதிஜ்ஞா(s);

        r = safe_atolu(s, &ul);
        எனில் (r < 0)
                திரும்பு r;

        pid = (pid_t) ul;

        எனில் ((unsigned long) pid != ul)
                திரும்பு -ERANGE;

        எனில் (!pid_is_valid(pid))
                திரும்பு -ERANGE;

        எனில் (ret)
                *ret = pid;
        திரும்பு 0;
}

முழு parse_mode(மாறிலி எழுத்து *s, mode_t *ret) {
        unsigned m;
        முழு r;

        ப்ரதிஜ்ஞா(s);

        r = safe_atou_full(s, 8 |
                           SAFE_ATO_REFUSE_PLUS_MINUS, /* Leading '+' or even '-' char? that's just weird,
                                                        * refuse. User might have wanted to add mode flags or
                                                        * so, but this parser doesn't allow that, so let's
                                                        * better be safe. */
                           &m);
        எனில் (r < 0)
                திரும்பு r;
        எனில் (m > 07777)
                திரும்பு -ERANGE;

        எனில் (ret)
                *ret = m;
        திரும்பு 0;
}

முழு parse_ifindex(மாறிலி எழுத்து *s) {
        முழு ifi, r;

        ப்ரதிஜ்ஞா(s);

        r = safe_atoi(s, &ifi);
        எனில் (r < 0)
                திரும்பு r;
        எனில் (ifi <= 0)
                திரும்பு -EINVAL;

        திரும்பு ifi;
}

முழு parse_mtu(முழு family, மாறிலி எழுத்து *s, uint32_t *ret) {
        uint64_t u, m;
        முழு r;

        ப்ரதிஜ்ஞா(ret);

        r = parse_size(s, 1024, &u);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (u > UINT32_MAX)
                திரும்பு -ERANGE;

        தேர்வு (family) {
        நிலை AF_INET:
                m = IPV4_MIN_MTU; /* This is 68 */
                நிறுத்து;
        நிலை AF_INET6:
                m = IPV6_MIN_MTU; /* This is 1280 */
                நிறுத்து;
        இயல்பு:
                m = 0;
        }

        எனில் (u < m)
                திரும்பு -ERANGE;

        *ret = (uint32_t) u;
        திரும்பு 0;
}

முழு parse_size(மாறிலி எழுத்து *t, uint64_t base, uint64_t *size) {

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

        கட்டமைப்பு table {
                மாறிலி எழுத்து *suffix;
                unsigned long long factor;
        };

        நிலையான மாறிலி கட்டமைப்பு table iec[] = {
                { "E", 1024ULL*1024ULL*1024ULL*1024ULL*1024ULL*1024ULL },
                { "P", 1024ULL*1024ULL*1024ULL*1024ULL*1024ULL },
                { "T", 1024ULL*1024ULL*1024ULL*1024ULL },
                { "G", 1024ULL*1024ULL*1024ULL },
                { "M", 1024ULL*1024ULL },
                { "K", 1024ULL },
                { "B", 1ULL },
                { "",  1ULL },
        };

        நிலையான மாறிலி கட்டமைப்பு table si[] = {
                { "E", 1000ULL*1000ULL*1000ULL*1000ULL*1000ULL*1000ULL },
                { "P", 1000ULL*1000ULL*1000ULL*1000ULL*1000ULL },
                { "T", 1000ULL*1000ULL*1000ULL*1000ULL },
                { "G", 1000ULL*1000ULL*1000ULL },
                { "M", 1000ULL*1000ULL },
                { "K", 1000ULL },
                { "B", 1ULL },
                { "",  1ULL },
        };

        மாறிலி கட்டமைப்பு table *table;
        மாறிலி எழுத்து *p;
        unsigned long long r = 0;
        unsigned n_entries, start_pos = 0;

        ப்ரதிஜ்ஞா(t);
        ப்ரதிஜ்ஞா(IN_SET(base, 1000, 1024));
        ப்ரதிஜ்ஞா(size);

        எனில் (base == 1000) {
                table = si;
                n_entries = ELEMENTSOF(si);
        } இல்லையெனில் {
                table = iec;
                n_entries = ELEMENTSOF(iec);
        }

        p = t;
        செய் {
                unsigned long long l, tmp;
                இரட்டை frac = 0;
                எழுத்து *e;
                unsigned i;

                p += strspn(p, WHITESPACE);

                errno = 0;
                l = strtoull(p, &e, 10);
                எனில் (errno > 0)
                        திரும்பு -errno;
                எனில் (e == p)
                        திரும்பு -EINVAL;
                எனில் (*p == '-')
                        திரும்பு -ERANGE;

                எனில் (*e == '.') {
                        e++;

                        /* strtoull() itself would accept space/+/- */
                        எனில் (ascii_isdigit(*e)) {
                                unsigned long long l2;
                                எழுத்து *e2;

                                l2 = strtoull(e, &e2, 10);
                                எனில் (errno > 0)
                                        திரும்பு -errno;

                                /* Ignore failure. E.g. 10.M is valid */
                                frac = l2;
                                க்கு (; e < e2; e++)
                                        frac /= 10;
                        }
                }

                e += strspn(e, WHITESPACE);

                க்கு (i = start_pos; i < n_entries; i++)
                        எனில் (startswith(e, table[i].suffix))
                                நிறுத்து;

                எனில் (i >= n_entries)
                        திரும்பு -EINVAL;

                எனில் (l + (frac > 0) > ULLONG_MAX / table[i].factor)
                        திரும்பு -ERANGE;

                tmp = l * table[i].factor + (unsigned long long) (frac * table[i].factor);
                எனில் (tmp > ULLONG_MAX - r)
                        திரும்பு -ERANGE;

                r += tmp;
                எனில் ((unsigned long long) (uint64_t) r != r)
                        திரும்பு -ERANGE;

                p = e + strlen(table[i].suffix);

                start_pos = i + 1;

        } போது (*p);

        *size = r;

        திரும்பு 0;
}

முழு parse_sector_size(மாறிலி எழுத்து *t, uint64_t *ret) {
        முழு r;

        ப்ரதிஜ்ஞா(t);
        ப்ரதிஜ்ஞா(ret);

        uint64_t ss;

        r = safe_atou64(t, &ss);
        எனில் (r < 0)
                திரும்பு log_error_errno(r, "Failed to parse sector size parameter %s", t);
        எனில் (ss < 512 || ss > 4096) /* Allow up to 4K due to dm-crypt support and 4K alignment by the homed LUKS backend */
                திரும்பு log_error_errno(SYNTHETIC_ERRNO(ERANGE), "Sector size not between 512 and 4096: %s", t);
        எனில் (!ISPOWEROF2(ss))
                திரும்பு log_error_errno(SYNTHETIC_ERRNO(EINVAL), "Sector size not power of 2: %s", t);

        *ret = ss;
        திரும்பு 0;
}

முழு parse_range(மாறிலி எழுத்து *t, unsigned *lower, unsigned *upper) {
        _cleanup_free_ எழுத்து *word = வெற்று;
        unsigned l, u;
        முழு r;

        ப்ரதிஜ்ஞா(lower);
        ப்ரதிஜ்ஞா(upper);

        /* Extract the lower bound. */
        r = extract_first_word(&t, &word, "-", EXTRACT_DONT_COALESCE_SEPARATORS);
        எனில் (r < 0)
                திரும்பு r;
        எனில் (r == 0)
                திரும்பு -EINVAL;

        r = safe_atou(word, &l);
        எனில் (r < 0)
                திரும்பு r;

        /* Check for the upper bound and extract it if needed */
        எனில் (!t)
                /* Single number with no dashes. */
                u = l;
        இல்லையெனில் எனில் (!*t)
                /* Trailing dash is an error. */
                திரும்பு -EINVAL;
        இல்லையெனில் {
                r = safe_atou(t, &u);
                எனில் (r < 0)
                        திரும்பு r;
        }

        *lower = l;
        *upper = u;
        திரும்பு 0;
}

முழு parse_errno(மாறிலி எழுத்து *t) {
        முழு r, e;

        ப்ரதிஜ்ஞா(t);

        r = errno_from_name(t);
        எனில் (r > 0)
                திரும்பு r;

        r = safe_atoi(t, &e);
        எனில் (r < 0)
                திரும்பு r;

        /* 0 is also allowed here */
        எனில் (!errno_is_valid(e) && e != 0)
                திரும்பு -ERANGE;

        திரும்பு e;
}

முழு parse_fd(மாறிலி எழுத்து *t) {
        முழு r, fd;

        ப்ரதிஜ்ஞா(t);

        r = safe_atoi(t, &fd);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (fd < 0)
                திரும்பு -EBADF;

        திரும்பு fd;
}

முழு parse_user_shell(மாறிலி எழுத்து *s, எழுத்து **ret_sh, bool *ret_copy) {
        எழுத்து *sh;
        முழு r;

        ப்ரதிஜ்ஞா(ret_sh);
        ப்ரதிஜ்ஞா(ret_copy);

        எனில் (path_is_absolute(s) && path_is_normalized(s)) {
                sh = strdup(s);
                எனில் (!sh)
                        திரும்பு -ENOMEM;

                *ret_sh = sh;
                *ret_copy = false;
        } இல்லையெனில் {
                r = parse_boolean(s);
                எனில் (r < 0)
                        திரும்பு r;

                *ret_sh = வெற்று;
                *ret_copy = r;
        }

        திரும்பு 0;
}

நிலையான மாறிலி எழுத்து *mangle_base(மாறிலி எழுத்து *s, unsigned *base) {
        மாறிலி எழுத்து *k;

        ப்ரதிஜ்ஞா(s);
        ப்ரதிஜ்ஞா(base);

        /* Base already explicitly specified, then don't do anything. */
        எனில் (SAFE_ATO_MASK_FLAGS(*base) != 0)
                திரும்பு s;

        /* Support Python 3 style "0b" and 0x" prefixes, because they truly make sense, much more than C's "0" prefix for octal. */
        k = STARTSWITH_SET(s, "0b", "0B");
        எனில் (k) {
                *base = 2 | (*base & SAFE_ATO_ALL_FLAGS);
                திரும்பு k;
        }

        k = STARTSWITH_SET(s, "0o", "0O");
        எனில் (k) {
                *base = 8 | (*base & SAFE_ATO_ALL_FLAGS);
                திரும்பு k;
        }

        திரும்பு s;
}

முழு safe_atou_full(மாறிலி எழுத்து *s, unsigned base, unsigned *ret_u) {
        எழுத்து *x = வெற்று;
        unsigned long l;

        ப்ரதிஜ்ஞா(s);
        ப்ரதிஜ்ஞா(SAFE_ATO_MASK_FLAGS(base) <= 16);

        /* strtoul() is happy to parse negative values, and silently converts them to unsigned values without
         * generating an error. We want a clean error, hence let's look for the "-" prefix on our own, and
         * generate an error. But let's do so only after strtoul() validated that the string is clean
         * otherwise, so that we return EINVAL preferably over ERANGE. */

        எனில் (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_WHITESPACE) &&
            strchr(WHITESPACE, s[0]))
                திரும்பு -EINVAL;

        s += strspn(s, WHITESPACE);

        எனில் (FLAGS_SET(base, SAFE_ATO_REFUSE_PLUS_MINUS) &&
            IN_SET(s[0], '+', '-'))
                திரும்பு -EINVAL; /* Note that we check the "-" prefix again a second time below, but return a
                                 * different error. I.e. if the SAFE_ATO_REFUSE_PLUS_MINUS flag is set we
                                 * blanket refuse +/- prefixed integers, while if it is missing we'll just
                                 * return ERANGE, because the string actually parses correctly, but doesn't
                                 * fit in the return type. */

        எனில் (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_ZERO) &&
            s[0] == '0' && !streq(s, "0"))
                திரும்பு -EINVAL; /* This is particularly useful to avoid ambiguities between C's octal
                                 * notation and assumed-to-be-decimal integers with a leading zero. */

        s = mangle_base(s, &base);

        errno = 0;
        l = strtoul(s, &x, SAFE_ATO_MASK_FLAGS(base) /* Let's mask off the flags bits so that only the actual
                                                      * base is left */);
        எனில் (errno > 0)
                திரும்பு -errno;
        எனில் (!x || x == s || *x != 0)
                திரும்பு -EINVAL;
        எனில் (l != 0 && s[0] == '-')
                திரும்பு -ERANGE;
        எனில் ((unsigned long) (unsigned) l != l)
                திரும்பு -ERANGE;

        எனில் (ret_u)
                *ret_u = (unsigned) l;

        திரும்பு 0;
}

முழு safe_atou_bounded(மாறிலி எழுத்து *s, unsigned min, unsigned max, unsigned *ret) {
        unsigned v;
        முழு r;

        ப்ரதிஜ்ஞா(ret);

        r = safe_atou(s, &v);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (v < min || v > max)
                திரும்பு -ERANGE;

        *ret = v;
        திரும்பு 0;
}

முழு safe_atoi(மாறிலி எழுத்து *s, முழு *ret_i) {
        unsigned base = 0;
        எழுத்து *x = வெற்று;
        long l;

        ப்ரதிஜ்ஞா(s);

        s += strspn(s, WHITESPACE);
        s = mangle_base(s, &base);

        errno = 0;
        l = strtol(s, &x, base);
        எனில் (errno > 0)
                திரும்பு -errno;
        எனில் (!x || x == s || *x != 0)
                திரும்பு -EINVAL;
        எனில் ((long) (முழு) l != l)
                திரும்பு -ERANGE;

        எனில் (ret_i)
                *ret_i = (முழு) l;

        திரும்பு 0;
}

முழு safe_atollu_full(மாறிலி எழுத்து *s, unsigned base, unsigned long long *ret_llu) {
        எழுத்து *x = வெற்று;
        unsigned long long l;

        ப்ரதிஜ்ஞா(s);
        ப்ரதிஜ்ஞா(SAFE_ATO_MASK_FLAGS(base) <= 16);

        எனில் (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_WHITESPACE) &&
            strchr(WHITESPACE, s[0]))
                திரும்பு -EINVAL;

        s += strspn(s, WHITESPACE);

        எனில் (FLAGS_SET(base, SAFE_ATO_REFUSE_PLUS_MINUS) &&
            IN_SET(s[0], '+', '-'))
                திரும்பு -EINVAL;

        எனில் (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_ZERO) &&
            s[0] == '0' && s[1] != 0)
                திரும்பு -EINVAL;

        s = mangle_base(s, &base);

        errno = 0;
        l = strtoull(s, &x, SAFE_ATO_MASK_FLAGS(base));
        எனில் (errno > 0)
                திரும்பு -errno;
        எனில் (!x || x == s || *x != 0)
                திரும்பு -EINVAL;
        எனில் (l != 0 && s[0] == '-')
                திரும்பு -ERANGE;

        எனில் (ret_llu)
                *ret_llu = l;

        திரும்பு 0;
}

முழு safe_atolli(மாறிலி எழுத்து *s, long long *ret_lli) {
        unsigned base = 0;
        எழுத்து *x = வெற்று;
        long long l;

        ப்ரதிஜ்ஞா(s);

        s += strspn(s, WHITESPACE);
        s = mangle_base(s, &base);

        errno = 0;
        l = strtoll(s, &x, base);
        எனில் (errno > 0)
                திரும்பு -errno;
        எனில் (!x || x == s || *x != 0)
                திரும்பு -EINVAL;

        எனில் (ret_lli)
                *ret_lli = l;

        திரும்பு 0;
}

முழு safe_atou8_full(மாறிலி எழுத்து *s, unsigned base, uint8_t *ret) {
        unsigned u;
        முழு r;

        ப்ரதிஜ்ஞா(ret);

        r = safe_atou_full(s, base, &u);
        எனில் (r < 0)
                திரும்பு r;
        எனில் (u > UINT8_MAX)
                திரும்பு -ERANGE;

        *ret = (uint8_t) u;
        திரும்பு 0;
}

முழு safe_atou16_full(மாறிலி எழுத்து *s, unsigned base, uint16_t *ret) {
        unsigned u;
        முழு r;

        ப்ரதிஜ்ஞா(ret);

        r = safe_atou_full(s, base, &u);
        எனில் (r < 0)
                திரும்பு r;
        எனில் (u > UINT16_MAX)
                திரும்பு -ERANGE;

        *ret = (uint16_t) u;
        திரும்பு 0;
}

முழு safe_atoi16(மாறிலி எழுத்து *s, int16_t *ret) {
        unsigned base = 0;
        எழுத்து *x = வெற்று;
        long l;

        ப்ரதிஜ்ஞா(s);

        s += strspn(s, WHITESPACE);
        s = mangle_base(s, &base);

        errno = 0;
        l = strtol(s, &x, base);
        எனில் (errno > 0)
                திரும்பு -errno;
        எனில் (!x || x == s || *x != 0)
                திரும்பு -EINVAL;
        எனில் ((long) (int16_t) l != l)
                திரும்பு -ERANGE;

        எனில் (ret)
                *ret = (int16_t) l;

        திரும்பு 0;
}

முழு safe_atod(மாறிலி எழுத்து *s, இரட்டை *ret_d) {
        _cleanup_(freelocalep) locale_t loc = (locale_t) 0;
        எழுத்து *x = வெற்று;
        இரட்டை d = 0;

        ப்ரதிஜ்ஞா(s);

        loc = newlocale(LC_NUMERIC_MASK, "C", (locale_t) 0);
        எனில் (loc == (locale_t) 0)
                திரும்பு -errno;

        errno = 0;
        d = strtod_l(s, &x, loc);
        எனில் (errno > 0)
                திரும்பு -errno;
        எனில் (!x || x == s || *x != 0)
                திரும்பு -EINVAL;

        எனில் (ret_d)
                *ret_d = d;

        திரும்பு 0;
}

முழு parse_fractional_part_u(மாறிலி எழுத்து **p, size_t digits, unsigned *res) {
        unsigned val = 0;
        மாறிலி எழுத்து *s;

        ப்ரதிஜ்ஞா(p);
        ப்ரதிஜ்ஞா(res);

        s = *p;

        /* accept any number of digits, strtoull is limited to 19 */
        க்கு (size_t i = 0; i < digits; i++, s++) {
                எனில் (!ascii_isdigit(*s)) {
                        எனில் (i == 0)
                                திரும்பு -EINVAL;

                        /* too few digits, pad with 0 */
                        க்கு (; i < digits; i++)
                                val *= 10;

                        நிறுத்து;
                }

                val *= 10;
                val += *s - '0';
        }

        /* maybe round up */
        எனில் (*s >= '5' && *s <= '9')
                val++;

        s += strspn(s, DIGITS);

        *p = s;
        *res = val;

        திரும்பு 0;
}

முழு parse_nice(மாறிலி எழுத்து *s, முழு *ret) {
        முழு n, r;

        ப்ரதிஜ்ஞா(ret);

        r = safe_atoi(s, &n);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (!nice_is_valid(n))
                திரும்பு -ERANGE;

        *ret = n;
        திரும்பு 0;
}

முழு parse_ip_port(மாறிலி எழுத்து *s, uint16_t *ret) {
        uint16_t l;
        முழு r;

        ப்ரதிஜ்ஞா(ret);

        r = safe_atou16_full(s, SAFE_ATO_REFUSE_LEADING_WHITESPACE, &l);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (l == 0)
                திரும்பு -EINVAL;

        *ret = l;

        திரும்பு 0;
}

முழு parse_ip_port_range(மாறிலி எழுத்து *s, uint16_t *low, uint16_t *high, bool allow_zero) {
        unsigned l, h;
        முழு r;

        ப்ரதிஜ்ஞா(low);
        ப்ரதிஜ்ஞா(high);

        r = parse_range(s, &l, &h);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (l > 65535 || h > 65535)
                திரும்பு -EINVAL;

        எனில் (!allow_zero && (l == 0 || h == 0))
                திரும்பு -EINVAL;

        எனில் (h < l)
                திரும்பு -EINVAL;

        *low = l;
        *high = h;

        திரும்பு 0;
}

முழு parse_oom_score_adjust(மாறிலி எழுத்து *s, முழு *ret) {
        முழு r, v;

        ப்ரதிஜ்ஞா(s);
        ப்ரதிஜ்ஞா(ret);

        r = safe_atoi(s, &v);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (!oom_score_adjust_is_valid(v))
                திரும்பு -ERANGE;

        *ret = v;
        திரும்பு 0;
}

முழு store_loadavg_fixed_point(unsigned long i, unsigned long f, loadavg_t *ret) {
        ப்ரதிஜ்ஞா(ret);

        /* Validate both parts *before* shifting them, so that the shifts themselves cannot overflow.
         * Rejecting f >= 100 is equivalent to the old check of the shifted value against
         * LOADAVG_FIXED_POINT_1_0, since DIV_ROUND_UP(99 << 11, 100) == 2028 and
         * DIV_ROUND_UP(100 << 11, 100) == 2048. */
        எனில் (i > (~0UL >> LOADAVG_PRECISION_BITS))
                திரும்பு -ERANGE;

        எனில் (f >= 100)
                திரும்பு -ERANGE;

        *ret = (i << LOADAVG_PRECISION_BITS) | DIV_ROUND_UP(f << LOADAVG_PRECISION_BITS, 100);
        திரும்பு 0;
}

முழு parse_loadavg_fixed_point(மாறிலி எழுத்து *s, loadavg_t *ret) {
        மாறிலி எழுத்து *d, *f_str, *i_str;
        unsigned long i, f;
        முழு r;

        ப்ரதிஜ்ஞா(s);
        ப்ரதிஜ்ஞா(ret);

        d = strchr(s, '.');
        எனில் (!d)
                திரும்பு -EINVAL;

        i_str = strndupa_safe(s, d - s);
        f_str = d + 1;

        r = safe_atolu_full(i_str, 10, &i);
        எனில் (r < 0)
                திரும்பு r;

        r = safe_atolu_full(f_str, 10, &f);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு store_loadavg_fixed_point(i, f, ret);
}

/* Limitations are described in https://www.netfilter.org/projects/nftables/manpage.html and
 * https://bugzilla.netfilter.org/show_bug.cgi?id=1175 */
bool nft_identifier_valid(மாறிலி எழுத்து *id) {
        எனில் (isempty(id))
                திரும்பு false;

        எனில் (strlen(id) >= NFT_NAME_MAXLEN)
                திரும்பு false;

        எனில் (!ascii_isalpha(id[0]))
                திரும்பு false;

        திரும்பு in_charset(id + 1, ALPHANUMERICAL "/\\_.");
}

முழு parse_capability_set(மாறிலி எழுத்து *s, uint64_t initial, uint64_t *current) {
        முழு r;

        ப்ரதிஜ்ஞா(s);
        ப்ரதிஜ்ஞா(current);

        எனில் (isempty(s)) {
                *current = CAP_MASK_UNSET;
                திரும்பு 1;
        }

        bool invert = false;
        எனில் (s[0] == '~') {
                invert = true;
                s++;
        }

        uint64_t parsed;
        r = capability_set_from_string(s, &parsed);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (parsed == 0 || *current == initial)
                /* "~" or uninitialized data -> replace */
                *current = invert ? all_capabilities() & ~parsed : parsed;
        இல்லையெனில் {
                /* previous data -> merge */
                எனில் (invert)
                        *current &= ~parsed;
                இல்லையெனில்
                        *current |= parsed;
        }

        திரும்பு r;
}
