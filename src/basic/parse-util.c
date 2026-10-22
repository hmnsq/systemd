/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <linux/ipv6.h>
#अन्तर्भाव <linux/netfilter/nf_tables.h>
#अन्तर्भाव <stdio.h>
#अन्तर्भाव <stdlib.h>
#अन्तर्भाव <sys/socket.h>

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "capability-list.h"
#अन्तर्भाव "capability-util.h"
#अन्तर्भाव "errno-list.h"
#अन्तर्भाव "extract-word.h"
#अन्तर्भाव "locale-util.h"
#अन्तर्भाव "log.h"
#अन्तर्भाव "missing-network.h"
#अन्तर्भाव "parse-util.h"
#अन्तर्भाव "path-util.h"
#अन्तर्भाव "process-util.h"
#अन्तर्भाव "string-util.h"
#अन्तर्भाव "strv.h"

पूर्णाङ्क parse_boolean(अचर वर्ण *v) {
        यदि (!v)
                फर्काऊ -EINVAL;

        यदि (STRCASE_IN_SET(v,
                           "1",
                           "yes",
                           "y",
                           "true",
                           "t",
                           "on"))
                फर्काऊ 1;

        यदि (STRCASE_IN_SET(v,
                           "0",
                           "no",
                           "n",
                           "false",
                           "f",
                           "off"))
                फर्काऊ 0;

        फर्काऊ -EINVAL;
}

पूर्णाङ्क parse_tristate_full(अचर वर्ण *v, अचर वर्ण *third, पूर्णाङ्क *ret) {
        पूर्णाङ्क r;

        यदि (isempty(v) || streq_ptr(v, third)) { /* Empty string is always taken as the third/invalid/auto state */
                यदि (ret)
                        *ret = -1;
        } अन्यथा {
                r = parse_boolean(v);
                यदि (r < 0)
                        फर्काऊ r;

                यदि (ret)
                        *ret = r;
        }

        फर्काऊ 0;
}

पूर्णाङ्क parse_pid(अचर वर्ण *s, pid_t *ret) {
        अचिन्हित दीर्घ ul = 0;
        pid_t pid;
        पूर्णाङ्क r;

        प्रतिज्ञा(s);

        r = safe_atolu(s, &ul);
        यदि (r < 0)
                फर्काऊ r;

        pid = (pid_t) ul;

        यदि ((अचिन्हित दीर्घ) pid != ul)
                फर्काऊ -दुस्फल;

        यदि (!pid_is_valid(pid))
                फर्काऊ -दुस्फल;

        यदि (ret)
                *ret = pid;
        फर्काऊ 0;
}

पूर्णाङ्क parse_mode(अचर वर्ण *s, mode_t *ret) {
        अचिन्हित m;
        पूर्णाङ्क r;

        प्रतिज्ञा(s);

        r = safe_atou_full(s, 8 |
                           SAFE_ATO_REFUSE_PLUS_MINUS, /* Leading '+' or even '-' char? that's just weird,
                                                        * refuse. User might have wanted to add mode flags or
                                                        * so, but this parser doesn't allow that, so let's
                                                        * better be safe. */
                           &m);
        यदि (r < 0)
                फर्काऊ r;
        यदि (m > 07777)
                फर्काऊ -दुस्फल;

        यदि (ret)
                *ret = m;
        फर्काऊ 0;
}

पूर्णाङ्क parse_ifindex(अचर वर्ण *s) {
        पूर्णाङ्क ifi, r;

        प्रतिज्ञा(s);

        r = safe_atoi(s, &ifi);
        यदि (r < 0)
                फर्काऊ r;
        यदि (ifi <= 0)
                फर्काऊ -EINVAL;

        फर्काऊ ifi;
}

पूर्णाङ्क parse_mtu(पूर्णाङ्क family, अचर वर्ण *s, uint32_t *ret) {
        uint64_t u, m;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        r = parse_size(s, 1024, &u);
        यदि (r < 0)
                फर्काऊ r;

        यदि (u > UINT32_MAX)
                फर्काऊ -दुस्फल;

        विकल्प (family) {
        पक्ष AF_INET:
                m = IPV4_MIN_MTU; /* This is 68 */
                रोक;
        पक्ष AF_INET6:
                m = IPV6_MIN_MTU; /* This is 1280 */
                रोक;
        स्वभाव:
                m = 0;
        }

        यदि (u < m)
                फर्काऊ -दुस्फल;

        *ret = (uint32_t) u;
        फर्काऊ 0;
}

पूर्णाङ्क parse_size(अचर वर्ण *t, uint64_t base, uint64_t *size) {

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

        संरचना table {
                अचर वर्ण *suffix;
                अचिन्हित दीर्घ दीर्घ factor;
        };

        स्थिर अचर संरचना table iec[] = {
                { "E", 1024ULL*1024ULL*1024ULL*1024ULL*1024ULL*1024ULL },
                { "P", 1024ULL*1024ULL*1024ULL*1024ULL*1024ULL },
                { "T", 1024ULL*1024ULL*1024ULL*1024ULL },
                { "G", 1024ULL*1024ULL*1024ULL },
                { "M", 1024ULL*1024ULL },
                { "K", 1024ULL },
                { "B", 1ULL },
                { "",  1ULL },
        };

        स्थिर अचर संरचना table si[] = {
                { "E", 1000ULL*1000ULL*1000ULL*1000ULL*1000ULL*1000ULL },
                { "P", 1000ULL*1000ULL*1000ULL*1000ULL*1000ULL },
                { "T", 1000ULL*1000ULL*1000ULL*1000ULL },
                { "G", 1000ULL*1000ULL*1000ULL },
                { "M", 1000ULL*1000ULL },
                { "K", 1000ULL },
                { "B", 1ULL },
                { "",  1ULL },
        };

        अचर संरचना table *table;
        अचर वर्ण *p;
        अचिन्हित दीर्घ दीर्घ r = 0;
        अचिन्हित n_entries, start_pos = 0;

        प्रतिज्ञा(t);
        प्रतिज्ञा(IN_SET(base, 1000, 1024));
        प्रतिज्ञा(size);

        यदि (base == 1000) {
                table = si;
                n_entries = ELEMENTSOF(si);
        } अन्यथा {
                table = iec;
                n_entries = ELEMENTSOF(iec);
        }

        p = t;
        गर {
                अचिन्हित दीर्घ दीर्घ l, tmp;
                द्विगुण frac = 0;
                वर्ण *e;
                अचिन्हित i;

                p += म_अखोज(p, WHITESPACE);

                त्रुटि_सं = 0;
                l = strtoull(p, &e, 10);
                यदि (त्रुटि_सं > 0)
                        फर्काऊ -त्रुटि_सं;
                यदि (e == p)
                        फर्काऊ -EINVAL;
                यदि (*p == '-')
                        फर्काऊ -दुस्फल;

                यदि (*e == '.') {
                        e++;

                        /* strtoull() itself would accept space/+/- */
                        यदि (ascii_isdigit(*e)) {
                                अचिन्हित दीर्घ दीर्घ l2;
                                वर्ण *e2;

                                l2 = strtoull(e, &e2, 10);
                                यदि (त्रुटि_सं > 0)
                                        फर्काऊ -त्रुटि_सं;

                                /* Ignore failure. E.g. 10.M is valid */
                                frac = l2;
                                लागि (; e < e2; e++)
                                        frac /= 10;
                        }
                }

                e += म_अखोज(e, WHITESPACE);

                लागि (i = start_pos; i < n_entries; i++)
                        यदि (startswith(e, table[i].suffix))
                                रोक;

                यदि (i >= n_entries)
                        फर्काऊ -EINVAL;

                यदि (l + (frac > 0) > ULLONG_MAX / table[i].factor)
                        फर्काऊ -दुस्फल;

                tmp = l * table[i].factor + (अचिन्हित दीर्घ दीर्घ) (frac * table[i].factor);
                यदि (tmp > ULLONG_MAX - r)
                        फर्काऊ -दुस्फल;

                r += tmp;
                यदि ((अचिन्हित दीर्घ दीर्घ) (uint64_t) r != r)
                        फर्काऊ -दुस्फल;

                p = e + म_माप(table[i].suffix);

                start_pos = i + 1;

        } जबसम्म (*p);

        *size = r;

        फर्काऊ 0;
}

पूर्णाङ्क parse_sector_size(अचर वर्ण *t, uint64_t *ret) {
        पूर्णाङ्क r;

        प्रतिज्ञा(t);
        प्रतिज्ञा(ret);

        uint64_t ss;

        r = safe_atou64(t, &ss);
        यदि (r < 0)
                फर्काऊ log_error_errno(r, "Failed to parse sector size parameter %s", t);
        यदि (ss < 512 || ss > 4096) /* Allow up to 4K due to dm-crypt support and 4K alignment by the homed LUKS backend */
                फर्काऊ log_error_errno(SYNTHETIC_ERRNO(दुस्फल), "Sector size not between 512 and 4096: %s", t);
        यदि (!ISPOWEROF2(ss))
                फर्काऊ log_error_errno(SYNTHETIC_ERRNO(EINVAL), "Sector size not power of 2: %s", t);

        *ret = ss;
        फर्काऊ 0;
}

पूर्णाङ्क parse_range(अचर वर्ण *t, अचिन्हित *lower, अचिन्हित *upper) {
        _cleanup_free_ वर्ण *word = रिक्त;
        अचिन्हित l, u;
        पूर्णाङ्क r;

        प्रतिज्ञा(lower);
        प्रतिज्ञा(upper);

        /* Extract the lower bound. */
        r = extract_first_word(&t, &word, "-", EXTRACT_DONT_COALESCE_SEPARATORS);
        यदि (r < 0)
                फर्काऊ r;
        यदि (r == 0)
                फर्काऊ -EINVAL;

        r = safe_atou(word, &l);
        यदि (r < 0)
                फर्काऊ r;

        /* Check for the upper bound and extract it if needed */
        यदि (!t)
                /* Single number with no dashes. */
                u = l;
        अन्यथा यदि (!*t)
                /* Trailing dash is an error. */
                फर्काऊ -EINVAL;
        अन्यथा {
                r = safe_atou(t, &u);
                यदि (r < 0)
                        फर्काऊ r;
        }

        *lower = l;
        *upper = u;
        फर्काऊ 0;
}

पूर्णाङ्क parse_errno(अचर वर्ण *t) {
        पूर्णाङ्क r, e;

        प्रतिज्ञा(t);

        r = errno_from_name(t);
        यदि (r > 0)
                फर्काऊ r;

        r = safe_atoi(t, &e);
        यदि (r < 0)
                फर्काऊ r;

        /* 0 is also allowed here */
        यदि (!errno_is_valid(e) && e != 0)
                फर्काऊ -दुस्फल;

        फर्काऊ e;
}

पूर्णाङ्क parse_fd(अचर वर्ण *t) {
        पूर्णाङ्क r, fd;

        प्रतिज्ञा(t);

        r = safe_atoi(t, &fd);
        यदि (r < 0)
                फर्काऊ r;

        यदि (fd < 0)
                फर्काऊ -EBADF;

        फर्काऊ fd;
}

पूर्णाङ्क parse_user_shell(अचर वर्ण *s, वर्ण **ret_sh, bool *ret_copy) {
        वर्ण *sh;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret_sh);
        प्रतिज्ञा(ret_copy);

        यदि (path_is_absolute(s) && path_is_normalized(s)) {
                sh = strdup(s);
                यदि (!sh)
                        फर्काऊ -ENOMEM;

                *ret_sh = sh;
                *ret_copy = false;
        } अन्यथा {
                r = parse_boolean(s);
                यदि (r < 0)
                        फर्काऊ r;

                *ret_sh = रिक्त;
                *ret_copy = r;
        }

        फर्काऊ 0;
}

स्थिर अचर वर्ण *mangle_base(अचर वर्ण *s, अचिन्हित *base) {
        अचर वर्ण *k;

        प्रतिज्ञा(s);
        प्रतिज्ञा(base);

        /* Base already explicitly specified, then don't do anything. */
        यदि (SAFE_ATO_MASK_FLAGS(*base) != 0)
                फर्काऊ s;

        /* Support Python 3 style "0b" and 0x" prefixes, because they truly make sense, much more than C's "0" prefix for octal. */
        k = STARTSWITH_SET(s, "0b", "0B");
        यदि (k) {
                *base = 2 | (*base & SAFE_ATO_ALL_FLAGS);
                फर्काऊ k;
        }

        k = STARTSWITH_SET(s, "0o", "0O");
        यदि (k) {
                *base = 8 | (*base & SAFE_ATO_ALL_FLAGS);
                फर्काऊ k;
        }

        फर्काऊ s;
}

पूर्णाङ्क safe_atou_full(अचर वर्ण *s, अचिन्हित base, अचिन्हित *ret_u) {
        वर्ण *x = रिक्त;
        अचिन्हित दीर्घ l;

        प्रतिज्ञा(s);
        प्रतिज्ञा(SAFE_ATO_MASK_FLAGS(base) <= 16);

        /* strtoul() is happy to parse negative values, and silently converts them to unsigned values without
         * generating an error. We want a clean error, hence let's look for the "-" prefix on our own, and
         * generate an error. But let's do so only after strtoul() validated that the string is clean
         * otherwise, so that we return EINVAL preferably over ERANGE. */

        यदि (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_WHITESPACE) &&
            म_अक्षर(WHITESPACE, s[0]))
                फर्काऊ -EINVAL;

        s += म_अखोज(s, WHITESPACE);

        यदि (FLAGS_SET(base, SAFE_ATO_REFUSE_PLUS_MINUS) &&
            IN_SET(s[0], '+', '-'))
                फर्काऊ -EINVAL; /* Note that we check the "-" prefix again a second time below, but return a
                                 * different error. I.e. if the SAFE_ATO_REFUSE_PLUS_MINUS flag is set we
                                 * blanket refuse +/- prefixed integers, while if it is missing we'll just
                                 * return ERANGE, because the string actually parses correctly, but doesn't
                                 * fit in the return type. */

        यदि (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_ZERO) &&
            s[0] == '0' && !streq(s, "0"))
                फर्काऊ -EINVAL; /* This is particularly useful to avoid ambiguities between C's octal
                                 * notation and assumed-to-be-decimal integers with a leading zero. */

        s = mangle_base(s, &base);

        त्रुटि_सं = 0;
        l = म_से_अदीर्घ(s, &x, SAFE_ATO_MASK_FLAGS(base) /* Let's mask off the flags bits so that only the actual
                                                      * base is left */);
        यदि (त्रुटि_सं > 0)
                फर्काऊ -त्रुटि_सं;
        यदि (!x || x == s || *x != 0)
                फर्काऊ -EINVAL;
        यदि (l != 0 && s[0] == '-')
                फर्काऊ -दुस्फल;
        यदि ((अचिन्हित दीर्घ) (अचिन्हित) l != l)
                फर्काऊ -दुस्फल;

        यदि (ret_u)
                *ret_u = (अचिन्हित) l;

        फर्काऊ 0;
}

पूर्णाङ्क safe_atou_bounded(अचर वर्ण *s, अचिन्हित min, अचिन्हित max, अचिन्हित *ret) {
        अचिन्हित v;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        r = safe_atou(s, &v);
        यदि (r < 0)
                फर्काऊ r;

        यदि (v < min || v > max)
                फर्काऊ -दुस्फल;

        *ret = v;
        फर्काऊ 0;
}

पूर्णाङ्क safe_atoi(अचर वर्ण *s, पूर्णाङ्क *ret_i) {
        अचिन्हित base = 0;
        वर्ण *x = रिक्त;
        दीर्घ l;

        प्रतिज्ञा(s);

        s += म_अखोज(s, WHITESPACE);
        s = mangle_base(s, &base);

        त्रुटि_सं = 0;
        l = म_से_दीर्घ(s, &x, base);
        यदि (त्रुटि_सं > 0)
                फर्काऊ -त्रुटि_सं;
        यदि (!x || x == s || *x != 0)
                फर्काऊ -EINVAL;
        यदि ((दीर्घ) (पूर्णाङ्क) l != l)
                फर्काऊ -दुस्फल;

        यदि (ret_i)
                *ret_i = (पूर्णाङ्क) l;

        फर्काऊ 0;
}

पूर्णाङ्क safe_atollu_full(अचर वर्ण *s, अचिन्हित base, अचिन्हित दीर्घ दीर्घ *ret_llu) {
        वर्ण *x = रिक्त;
        अचिन्हित दीर्घ दीर्घ l;

        प्रतिज्ञा(s);
        प्रतिज्ञा(SAFE_ATO_MASK_FLAGS(base) <= 16);

        यदि (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_WHITESPACE) &&
            म_अक्षर(WHITESPACE, s[0]))
                फर्काऊ -EINVAL;

        s += म_अखोज(s, WHITESPACE);

        यदि (FLAGS_SET(base, SAFE_ATO_REFUSE_PLUS_MINUS) &&
            IN_SET(s[0], '+', '-'))
                फर्काऊ -EINVAL;

        यदि (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_ZERO) &&
            s[0] == '0' && s[1] != 0)
                फर्काऊ -EINVAL;

        s = mangle_base(s, &base);

        त्रुटि_सं = 0;
        l = strtoull(s, &x, SAFE_ATO_MASK_FLAGS(base));
        यदि (त्रुटि_सं > 0)
                फर्काऊ -त्रुटि_सं;
        यदि (!x || x == s || *x != 0)
                फर्काऊ -EINVAL;
        यदि (l != 0 && s[0] == '-')
                फर्काऊ -दुस्फल;

        यदि (ret_llu)
                *ret_llu = l;

        फर्काऊ 0;
}

पूर्णाङ्क safe_atolli(अचर वर्ण *s, दीर्घ दीर्घ *ret_lli) {
        अचिन्हित base = 0;
        वर्ण *x = रिक्त;
        दीर्घ दीर्घ l;

        प्रतिज्ञा(s);

        s += म_अखोज(s, WHITESPACE);
        s = mangle_base(s, &base);

        त्रुटि_सं = 0;
        l = strtoll(s, &x, base);
        यदि (त्रुटि_सं > 0)
                फर्काऊ -त्रुटि_सं;
        यदि (!x || x == s || *x != 0)
                फर्काऊ -EINVAL;

        यदि (ret_lli)
                *ret_lli = l;

        फर्काऊ 0;
}

पूर्णाङ्क safe_atou8_full(अचर वर्ण *s, अचिन्हित base, uint8_t *ret) {
        अचिन्हित u;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        r = safe_atou_full(s, base, &u);
        यदि (r < 0)
                फर्काऊ r;
        यदि (u > UINT8_MAX)
                फर्काऊ -दुस्फल;

        *ret = (uint8_t) u;
        फर्काऊ 0;
}

पूर्णाङ्क safe_atou16_full(अचर वर्ण *s, अचिन्हित base, uint16_t *ret) {
        अचिन्हित u;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        r = safe_atou_full(s, base, &u);
        यदि (r < 0)
                फर्काऊ r;
        यदि (u > UINT16_MAX)
                फर्काऊ -दुस्फल;

        *ret = (uint16_t) u;
        फर्काऊ 0;
}

पूर्णाङ्क safe_atoi16(अचर वर्ण *s, int16_t *ret) {
        अचिन्हित base = 0;
        वर्ण *x = रिक्त;
        दीर्घ l;

        प्रतिज्ञा(s);

        s += म_अखोज(s, WHITESPACE);
        s = mangle_base(s, &base);

        त्रुटि_सं = 0;
        l = म_से_दीर्घ(s, &x, base);
        यदि (त्रुटि_सं > 0)
                फर्काऊ -त्रुटि_सं;
        यदि (!x || x == s || *x != 0)
                फर्काऊ -EINVAL;
        यदि ((दीर्घ) (int16_t) l != l)
                फर्काऊ -दुस्फल;

        यदि (ret)
                *ret = (int16_t) l;

        फर्काऊ 0;
}

पूर्णाङ्क safe_atod(अचर वर्ण *s, द्विगुण *ret_d) {
        _cleanup_(freelocalep) locale_t loc = (locale_t) 0;
        वर्ण *x = रिक्त;
        द्विगुण d = 0;

        प्रतिज्ञा(s);

        loc = newlocale(LC_NUMERIC_MASK, "C", (locale_t) 0);
        यदि (loc == (locale_t) 0)
                फर्काऊ -त्रुटि_सं;

        त्रुटि_सं = 0;
        d = strtod_l(s, &x, loc);
        यदि (त्रुटि_सं > 0)
                फर्काऊ -त्रुटि_सं;
        यदि (!x || x == s || *x != 0)
                फर्काऊ -EINVAL;

        यदि (ret_d)
                *ret_d = d;

        फर्काऊ 0;
}

पूर्णाङ्क parse_fractional_part_u(अचर वर्ण **p, माप_प्रकार digits, अचिन्हित *res) {
        अचिन्हित val = 0;
        अचर वर्ण *s;

        प्रतिज्ञा(p);
        प्रतिज्ञा(res);

        s = *p;

        /* accept any number of digits, strtoull is limited to 19 */
        लागि (माप_प्रकार i = 0; i < digits; i++, s++) {
                यदि (!ascii_isdigit(*s)) {
                        यदि (i == 0)
                                फर्काऊ -EINVAL;

                        /* too few digits, pad with 0 */
                        लागि (; i < digits; i++)
                                val *= 10;

                        रोक;
                }

                val *= 10;
                val += *s - '0';
        }

        /* maybe round up */
        यदि (*s >= '5' && *s <= '9')
                val++;

        s += म_अखोज(s, DIGITS);

        *p = s;
        *res = val;

        फर्काऊ 0;
}

पूर्णाङ्क parse_nice(अचर वर्ण *s, पूर्णाङ्क *ret) {
        पूर्णाङ्क n, r;

        प्रतिज्ञा(ret);

        r = safe_atoi(s, &n);
        यदि (r < 0)
                फर्काऊ r;

        यदि (!nice_is_valid(n))
                फर्काऊ -दुस्फल;

        *ret = n;
        फर्काऊ 0;
}

पूर्णाङ्क parse_ip_port(अचर वर्ण *s, uint16_t *ret) {
        uint16_t l;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        r = safe_atou16_full(s, SAFE_ATO_REFUSE_LEADING_WHITESPACE, &l);
        यदि (r < 0)
                फर्काऊ r;

        यदि (l == 0)
                फर्काऊ -EINVAL;

        *ret = l;

        फर्काऊ 0;
}

पूर्णाङ्क parse_ip_port_range(अचर वर्ण *s, uint16_t *low, uint16_t *high, bool allow_zero) {
        अचिन्हित l, h;
        पूर्णाङ्क r;

        प्रतिज्ञा(low);
        प्रतिज्ञा(high);

        r = parse_range(s, &l, &h);
        यदि (r < 0)
                फर्काऊ r;

        यदि (l > 65535 || h > 65535)
                फर्काऊ -EINVAL;

        यदि (!allow_zero && (l == 0 || h == 0))
                फर्काऊ -EINVAL;

        यदि (h < l)
                फर्काऊ -EINVAL;

        *low = l;
        *high = h;

        फर्काऊ 0;
}

पूर्णाङ्क parse_oom_score_adjust(अचर वर्ण *s, पूर्णाङ्क *ret) {
        पूर्णाङ्क r, v;

        प्रतिज्ञा(s);
        प्रतिज्ञा(ret);

        r = safe_atoi(s, &v);
        यदि (r < 0)
                फर्काऊ r;

        यदि (!oom_score_adjust_is_valid(v))
                फर्काऊ -दुस्फल;

        *ret = v;
        फर्काऊ 0;
}

पूर्णाङ्क store_loadavg_fixed_point(अचिन्हित दीर्घ i, अचिन्हित दीर्घ f, loadavg_t *ret) {
        प्रतिज्ञा(ret);

        /* Validate both parts *before* shifting them, so that the shifts themselves cannot overflow.
         * Rejecting f >= 100 is equivalent to the old check of the shifted value against
         * LOADAVG_FIXED_POINT_1_0, since DIV_ROUND_UP(99 << 11, 100) == 2028 and
         * DIV_ROUND_UP(100 << 11, 100) == 2048. */
        यदि (i > (~0UL >> LOADAVG_PRECISION_BITS))
                फर्काऊ -दुस्फल;

        यदि (f >= 100)
                फर्काऊ -दुस्फल;

        *ret = (i << LOADAVG_PRECISION_BITS) | DIV_ROUND_UP(f << LOADAVG_PRECISION_BITS, 100);
        फर्काऊ 0;
}

पूर्णाङ्क parse_loadavg_fixed_point(अचर वर्ण *s, loadavg_t *ret) {
        अचर वर्ण *d, *f_str, *i_str;
        अचिन्हित दीर्घ i, f;
        पूर्णाङ्क r;

        प्रतिज्ञा(s);
        प्रतिज्ञा(ret);

        d = म_अक्षर(s, '.');
        यदि (!d)
                फर्काऊ -EINVAL;

        i_str = strndupa_safe(s, d - s);
        f_str = d + 1;

        r = safe_atolu_full(i_str, 10, &i);
        यदि (r < 0)
                फर्काऊ r;

        r = safe_atolu_full(f_str, 10, &f);
        यदि (r < 0)
                फर्काऊ r;

        फर्काऊ store_loadavg_fixed_point(i, f, ret);
}

/* Limitations are described in https://www.netfilter.org/projects/nftables/manpage.html and
 * https://bugzilla.netfilter.org/show_bug.cgi?id=1175 */
bool nft_identifier_valid(अचर वर्ण *id) {
        यदि (isempty(id))
                फर्काऊ false;

        यदि (म_माप(id) >= NFT_NAME_MAXLEN)
                फर्काऊ false;

        यदि (!ascii_isalpha(id[0]))
                फर्काऊ false;

        फर्काऊ in_charset(id + 1, ALPHANUMERICAL "/\\_.");
}

पूर्णाङ्क parse_capability_set(अचर वर्ण *s, uint64_t initial, uint64_t *current) {
        पूर्णाङ्क r;

        प्रतिज्ञा(s);
        प्रतिज्ञा(current);

        यदि (isempty(s)) {
                *current = CAP_MASK_UNSET;
                फर्काऊ 1;
        }

        bool invert = false;
        यदि (s[0] == '~') {
                invert = true;
                s++;
        }

        uint64_t parsed;
        r = capability_set_from_string(s, &parsed);
        यदि (r < 0)
                फर्काऊ r;

        यदि (parsed == 0 || *current == initial)
                /* "~" or uninitialized data -> replace */
                *current = invert ? all_capabilities() & ~parsed : parsed;
        अन्यथा {
                /* previous data -> merge */
                यदि (invert)
                        *current &= ~parsed;
                अन्यथा
                        *current |= parsed;
        }

        फर्काऊ r;
}
