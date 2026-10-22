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

पूर्णांक parse_boolean(स्थिरांक अक्षर *v) {
        जर (!v)
                परत -EINVAL;

        जर (STRCASE_IN_SET(v,
                           "1",
                           "yes",
                           "y",
                           "true",
                           "t",
                           "on"))
                परत 1;

        जर (STRCASE_IN_SET(v,
                           "0",
                           "no",
                           "n",
                           "false",
                           "f",
                           "off"))
                परत 0;

        परत -EINVAL;
}

पूर्णांक parse_tristate_full(स्थिरांक अक्षर *v, स्थिरांक अक्षर *third, पूर्णांक *ret) {
        पूर्णांक r;

        जर (isempty(v) || streq_ptr(v, third)) { /* Empty string is always taken as the third/invalid/auto state */
                जर (ret)
                        *ret = -1;
        } अन्यथा {
                r = parse_boolean(v);
                जर (r < 0)
                        परत r;

                जर (ret)
                        *ret = r;
        }

        परत 0;
}

पूर्णांक parse_pid(स्थिरांक अक्षर *s, pid_t *ret) {
        अचिन्हित दीर्घ ul = 0;
        pid_t pid;
        पूर्णांक r;

        प्रतिज्ञा(s);

        r = safe_atolu(s, &ul);
        जर (r < 0)
                परत r;

        pid = (pid_t) ul;

        जर ((अचिन्हित दीर्घ) pid != ul)
                परत -दुस्फल;

        जर (!pid_is_valid(pid))
                परत -दुस्फल;

        जर (ret)
                *ret = pid;
        परत 0;
}

पूर्णांक parse_mode(स्थिरांक अक्षर *s, mode_t *ret) {
        अचिन्हित m;
        पूर्णांक r;

        प्रतिज्ञा(s);

        r = safe_atou_full(s, 8 |
                           SAFE_ATO_REFUSE_PLUS_MINUS, /* Leading '+' or even '-' char? that's just weird,
                                                        * refuse. User might have wanted to add mode flags or
                                                        * so, but this parser doesn't allow that, so let's
                                                        * better be safe. */
                           &m);
        जर (r < 0)
                परत r;
        जर (m > 07777)
                परत -दुस्फल;

        जर (ret)
                *ret = m;
        परत 0;
}

पूर्णांक parse_ifindex(स्थिरांक अक्षर *s) {
        पूर्णांक ifi, r;

        प्रतिज्ञा(s);

        r = safe_atoi(s, &ifi);
        जर (r < 0)
                परत r;
        जर (ifi <= 0)
                परत -EINVAL;

        परत ifi;
}

पूर्णांक parse_mtu(पूर्णांक family, स्थिरांक अक्षर *s, uint32_t *ret) {
        uint64_t u, m;
        पूर्णांक r;

        प्रतिज्ञा(ret);

        r = parse_size(s, 1024, &u);
        जर (r < 0)
                परत r;

        जर (u > UINT32_MAX)
                परत -दुस्फल;

        निवड (family) {
        स्थिती AF_INET:
                m = IPV4_MIN_MTU; /* This is 68 */
                थांबा;
        स्थिती AF_INET6:
                m = IPV6_MIN_MTU; /* This is 1280 */
                थांबा;
        पूर्वनिर्धारित:
                m = 0;
        }

        जर (u < m)
                परत -दुस्फल;

        *ret = (uint32_t) u;
        परत 0;
}

पूर्णांक parse_size(स्थिरांक अक्षर *t, uint64_t base, uint64_t *size) {

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
                स्थिरांक अक्षर *suffix;
                अचिन्हित दीर्घ दीर्घ factor;
        };

        स्थिर स्थिरांक संरचना table iec[] = {
                { "E", 1024ULL*1024ULL*1024ULL*1024ULL*1024ULL*1024ULL },
                { "P", 1024ULL*1024ULL*1024ULL*1024ULL*1024ULL },
                { "T", 1024ULL*1024ULL*1024ULL*1024ULL },
                { "G", 1024ULL*1024ULL*1024ULL },
                { "M", 1024ULL*1024ULL },
                { "K", 1024ULL },
                { "B", 1ULL },
                { "",  1ULL },
        };

        स्थिर स्थिरांक संरचना table si[] = {
                { "E", 1000ULL*1000ULL*1000ULL*1000ULL*1000ULL*1000ULL },
                { "P", 1000ULL*1000ULL*1000ULL*1000ULL*1000ULL },
                { "T", 1000ULL*1000ULL*1000ULL*1000ULL },
                { "G", 1000ULL*1000ULL*1000ULL },
                { "M", 1000ULL*1000ULL },
                { "K", 1000ULL },
                { "B", 1ULL },
                { "",  1ULL },
        };

        स्थिरांक संरचना table *table;
        स्थिरांक अक्षर *p;
        अचिन्हित दीर्घ दीर्घ r = 0;
        अचिन्हित n_entries, start_pos = 0;

        प्रतिज्ञा(t);
        प्रतिज्ञा(IN_SET(base, 1000, 1024));
        प्रतिज्ञा(size);

        जर (base == 1000) {
                table = si;
                n_entries = ELEMENTSOF(si);
        } अन्यथा {
                table = iec;
                n_entries = ELEMENTSOF(iec);
        }

        p = t;
        करा {
                अचिन्हित दीर्घ दीर्घ l, tmp;
                द्विगुण frac = 0;
                अक्षर *e;
                अचिन्हित i;

                p += म_अखोज(p, WHITESPACE);

                त्रुटि_सं = 0;
                l = strtoull(p, &e, 10);
                जर (त्रुटि_सं > 0)
                        परत -त्रुटि_सं;
                जर (e == p)
                        परत -EINVAL;
                जर (*p == '-')
                        परत -दुस्फल;

                जर (*e == '.') {
                        e++;

                        /* strtoull() itself would accept space/+/- */
                        जर (ascii_isdigit(*e)) {
                                अचिन्हित दीर्घ दीर्घ l2;
                                अक्षर *e2;

                                l2 = strtoull(e, &e2, 10);
                                जर (त्रुटि_सं > 0)
                                        परत -त्रुटि_सं;

                                /* Ignore failure. E.g. 10.M is valid */
                                frac = l2;
                                साठी (; e < e2; e++)
                                        frac /= 10;
                        }
                }

                e += म_अखोज(e, WHITESPACE);

                साठी (i = start_pos; i < n_entries; i++)
                        जर (startswith(e, table[i].suffix))
                                थांबा;

                जर (i >= n_entries)
                        परत -EINVAL;

                जर (l + (frac > 0) > ULLONG_MAX / table[i].factor)
                        परत -दुस्फल;

                tmp = l * table[i].factor + (अचिन्हित दीर्घ दीर्घ) (frac * table[i].factor);
                जर (tmp > ULLONG_MAX - r)
                        परत -दुस्फल;

                r += tmp;
                जर ((अचिन्हित दीर्घ दीर्घ) (uint64_t) r != r)
                        परत -दुस्फल;

                p = e + म_माप(table[i].suffix);

                start_pos = i + 1;

        } पर्यंत (*p);

        *size = r;

        परत 0;
}

पूर्णांक parse_sector_size(स्थिरांक अक्षर *t, uint64_t *ret) {
        पूर्णांक r;

        प्रतिज्ञा(t);
        प्रतिज्ञा(ret);

        uint64_t ss;

        r = safe_atou64(t, &ss);
        जर (r < 0)
                परत log_error_errno(r, "Failed to parse sector size parameter %s", t);
        जर (ss < 512 || ss > 4096) /* Allow up to 4K due to dm-crypt support and 4K alignment by the homed LUKS backend */
                परत log_error_errno(SYNTHETIC_ERRNO(दुस्फल), "Sector size not between 512 and 4096: %s", t);
        जर (!ISPOWEROF2(ss))
                परत log_error_errno(SYNTHETIC_ERRNO(EINVAL), "Sector size not power of 2: %s", t);

        *ret = ss;
        परत 0;
}

पूर्णांक parse_range(स्थिरांक अक्षर *t, अचिन्हित *lower, अचिन्हित *upper) {
        _cleanup_free_ अक्षर *word = रिक्त;
        अचिन्हित l, u;
        पूर्णांक r;

        प्रतिज्ञा(lower);
        प्रतिज्ञा(upper);

        /* Extract the lower bound. */
        r = extract_first_word(&t, &word, "-", EXTRACT_DONT_COALESCE_SEPARATORS);
        जर (r < 0)
                परत r;
        जर (r == 0)
                परत -EINVAL;

        r = safe_atou(word, &l);
        जर (r < 0)
                परत r;

        /* Check for the upper bound and extract it if needed */
        जर (!t)
                /* Single number with no dashes. */
                u = l;
        अन्यथा जर (!*t)
                /* Trailing dash is an error. */
                परत -EINVAL;
        अन्यथा {
                r = safe_atou(t, &u);
                जर (r < 0)
                        परत r;
        }

        *lower = l;
        *upper = u;
        परत 0;
}

पूर्णांक parse_errno(स्थिरांक अक्षर *t) {
        पूर्णांक r, e;

        प्रतिज्ञा(t);

        r = errno_from_name(t);
        जर (r > 0)
                परत r;

        r = safe_atoi(t, &e);
        जर (r < 0)
                परत r;

        /* 0 is also allowed here */
        जर (!errno_is_valid(e) && e != 0)
                परत -दुस्फल;

        परत e;
}

पूर्णांक parse_fd(स्थिरांक अक्षर *t) {
        पूर्णांक r, fd;

        प्रतिज्ञा(t);

        r = safe_atoi(t, &fd);
        जर (r < 0)
                परत r;

        जर (fd < 0)
                परत -EBADF;

        परत fd;
}

पूर्णांक parse_user_shell(स्थिरांक अक्षर *s, अक्षर **ret_sh, bool *ret_copy) {
        अक्षर *sh;
        पूर्णांक r;

        प्रतिज्ञा(ret_sh);
        प्रतिज्ञा(ret_copy);

        जर (path_is_absolute(s) && path_is_normalized(s)) {
                sh = strdup(s);
                जर (!sh)
                        परत -ENOMEM;

                *ret_sh = sh;
                *ret_copy = false;
        } अन्यथा {
                r = parse_boolean(s);
                जर (r < 0)
                        परत r;

                *ret_sh = रिक्त;
                *ret_copy = r;
        }

        परत 0;
}

स्थिर स्थिरांक अक्षर *mangle_base(स्थिरांक अक्षर *s, अचिन्हित *base) {
        स्थिरांक अक्षर *k;

        प्रतिज्ञा(s);
        प्रतिज्ञा(base);

        /* Base already explicitly specified, then don't do anything. */
        जर (SAFE_ATO_MASK_FLAGS(*base) != 0)
                परत s;

        /* Support Python 3 style "0b" and 0x" prefixes, because they truly make sense, much more than C's "0" prefix for octal. */
        k = STARTSWITH_SET(s, "0b", "0B");
        जर (k) {
                *base = 2 | (*base & SAFE_ATO_ALL_FLAGS);
                परत k;
        }

        k = STARTSWITH_SET(s, "0o", "0O");
        जर (k) {
                *base = 8 | (*base & SAFE_ATO_ALL_FLAGS);
                परत k;
        }

        परत s;
}

पूर्णांक safe_atou_full(स्थिरांक अक्षर *s, अचिन्हित base, अचिन्हित *ret_u) {
        अक्षर *x = रिक्त;
        अचिन्हित दीर्घ l;

        प्रतिज्ञा(s);
        प्रतिज्ञा(SAFE_ATO_MASK_FLAGS(base) <= 16);

        /* strtoul() is happy to parse negative values, and silently converts them to unsigned values without
         * generating an error. We want a clean error, hence let's look for the "-" prefix on our own, and
         * generate an error. But let's do so only after strtoul() validated that the string is clean
         * otherwise, so that we return EINVAL preferably over ERANGE. */

        जर (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_WHITESPACE) &&
            म_अक्षर(WHITESPACE, s[0]))
                परत -EINVAL;

        s += म_अखोज(s, WHITESPACE);

        जर (FLAGS_SET(base, SAFE_ATO_REFUSE_PLUS_MINUS) &&
            IN_SET(s[0], '+', '-'))
                परत -EINVAL; /* Note that we check the "-" prefix again a second time below, but return a
                                 * different error. I.e. if the SAFE_ATO_REFUSE_PLUS_MINUS flag is set we
                                 * blanket refuse +/- prefixed integers, while if it is missing we'll just
                                 * return ERANGE, because the string actually parses correctly, but doesn't
                                 * fit in the return type. */

        जर (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_ZERO) &&
            s[0] == '0' && !streq(s, "0"))
                परत -EINVAL; /* This is particularly useful to avoid ambiguities between C's octal
                                 * notation and assumed-to-be-decimal integers with a leading zero. */

        s = mangle_base(s, &base);

        त्रुटि_सं = 0;
        l = म_से_अदीर्घ(s, &x, SAFE_ATO_MASK_FLAGS(base) /* Let's mask off the flags bits so that only the actual
                                                      * base is left */);
        जर (त्रुटि_सं > 0)
                परत -त्रुटि_सं;
        जर (!x || x == s || *x != 0)
                परत -EINVAL;
        जर (l != 0 && s[0] == '-')
                परत -दुस्फल;
        जर ((अचिन्हित दीर्घ) (अचिन्हित) l != l)
                परत -दुस्फल;

        जर (ret_u)
                *ret_u = (अचिन्हित) l;

        परत 0;
}

पूर्णांक safe_atou_bounded(स्थिरांक अक्षर *s, अचिन्हित min, अचिन्हित max, अचिन्हित *ret) {
        अचिन्हित v;
        पूर्णांक r;

        प्रतिज्ञा(ret);

        r = safe_atou(s, &v);
        जर (r < 0)
                परत r;

        जर (v < min || v > max)
                परत -दुस्फल;

        *ret = v;
        परत 0;
}

पूर्णांक safe_atoi(स्थिरांक अक्षर *s, पूर्णांक *ret_i) {
        अचिन्हित base = 0;
        अक्षर *x = रिक्त;
        दीर्घ l;

        प्रतिज्ञा(s);

        s += म_अखोज(s, WHITESPACE);
        s = mangle_base(s, &base);

        त्रुटि_सं = 0;
        l = म_से_दीर्घ(s, &x, base);
        जर (त्रुटि_सं > 0)
                परत -त्रुटि_सं;
        जर (!x || x == s || *x != 0)
                परत -EINVAL;
        जर ((दीर्घ) (पूर्णांक) l != l)
                परत -दुस्फल;

        जर (ret_i)
                *ret_i = (पूर्णांक) l;

        परत 0;
}

पूर्णांक safe_atollu_full(स्थिरांक अक्षर *s, अचिन्हित base, अचिन्हित दीर्घ दीर्घ *ret_llu) {
        अक्षर *x = रिक्त;
        अचिन्हित दीर्घ दीर्घ l;

        प्रतिज्ञा(s);
        प्रतिज्ञा(SAFE_ATO_MASK_FLAGS(base) <= 16);

        जर (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_WHITESPACE) &&
            म_अक्षर(WHITESPACE, s[0]))
                परत -EINVAL;

        s += म_अखोज(s, WHITESPACE);

        जर (FLAGS_SET(base, SAFE_ATO_REFUSE_PLUS_MINUS) &&
            IN_SET(s[0], '+', '-'))
                परत -EINVAL;

        जर (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_ZERO) &&
            s[0] == '0' && s[1] != 0)
                परत -EINVAL;

        s = mangle_base(s, &base);

        त्रुटि_सं = 0;
        l = strtoull(s, &x, SAFE_ATO_MASK_FLAGS(base));
        जर (त्रुटि_सं > 0)
                परत -त्रुटि_सं;
        जर (!x || x == s || *x != 0)
                परत -EINVAL;
        जर (l != 0 && s[0] == '-')
                परत -दुस्फल;

        जर (ret_llu)
                *ret_llu = l;

        परत 0;
}

पूर्णांक safe_atolli(स्थिरांक अक्षर *s, दीर्घ दीर्घ *ret_lli) {
        अचिन्हित base = 0;
        अक्षर *x = रिक्त;
        दीर्घ दीर्घ l;

        प्रतिज्ञा(s);

        s += म_अखोज(s, WHITESPACE);
        s = mangle_base(s, &base);

        त्रुटि_सं = 0;
        l = strtoll(s, &x, base);
        जर (त्रुटि_सं > 0)
                परत -त्रुटि_सं;
        जर (!x || x == s || *x != 0)
                परत -EINVAL;

        जर (ret_lli)
                *ret_lli = l;

        परत 0;
}

पूर्णांक safe_atou8_full(स्थिरांक अक्षर *s, अचिन्हित base, uint8_t *ret) {
        अचिन्हित u;
        पूर्णांक r;

        प्रतिज्ञा(ret);

        r = safe_atou_full(s, base, &u);
        जर (r < 0)
                परत r;
        जर (u > UINT8_MAX)
                परत -दुस्फल;

        *ret = (uint8_t) u;
        परत 0;
}

पूर्णांक safe_atou16_full(स्थिरांक अक्षर *s, अचिन्हित base, uint16_t *ret) {
        अचिन्हित u;
        पूर्णांक r;

        प्रतिज्ञा(ret);

        r = safe_atou_full(s, base, &u);
        जर (r < 0)
                परत r;
        जर (u > UINT16_MAX)
                परत -दुस्फल;

        *ret = (uint16_t) u;
        परत 0;
}

पूर्णांक safe_atoi16(स्थिरांक अक्षर *s, int16_t *ret) {
        अचिन्हित base = 0;
        अक्षर *x = रिक्त;
        दीर्घ l;

        प्रतिज्ञा(s);

        s += म_अखोज(s, WHITESPACE);
        s = mangle_base(s, &base);

        त्रुटि_सं = 0;
        l = म_से_दीर्घ(s, &x, base);
        जर (त्रुटि_सं > 0)
                परत -त्रुटि_सं;
        जर (!x || x == s || *x != 0)
                परत -EINVAL;
        जर ((दीर्घ) (int16_t) l != l)
                परत -दुस्फल;

        जर (ret)
                *ret = (int16_t) l;

        परत 0;
}

पूर्णांक safe_atod(स्थिरांक अक्षर *s, द्विगुण *ret_d) {
        _cleanup_(freelocalep) locale_t loc = (locale_t) 0;
        अक्षर *x = रिक्त;
        द्विगुण d = 0;

        प्रतिज्ञा(s);

        loc = newlocale(LC_NUMERIC_MASK, "C", (locale_t) 0);
        जर (loc == (locale_t) 0)
                परत -त्रुटि_सं;

        त्रुटि_सं = 0;
        d = strtod_l(s, &x, loc);
        जर (त्रुटि_सं > 0)
                परत -त्रुटि_सं;
        जर (!x || x == s || *x != 0)
                परत -EINVAL;

        जर (ret_d)
                *ret_d = d;

        परत 0;
}

पूर्णांक parse_fractional_part_u(स्थिरांक अक्षर **p, माप_प्रकार digits, अचिन्हित *res) {
        अचिन्हित val = 0;
        स्थिरांक अक्षर *s;

        प्रतिज्ञा(p);
        प्रतिज्ञा(res);

        s = *p;

        /* accept any number of digits, strtoull is limited to 19 */
        साठी (माप_प्रकार i = 0; i < digits; i++, s++) {
                जर (!ascii_isdigit(*s)) {
                        जर (i == 0)
                                परत -EINVAL;

                        /* too few digits, pad with 0 */
                        साठी (; i < digits; i++)
                                val *= 10;

                        थांबा;
                }

                val *= 10;
                val += *s - '0';
        }

        /* maybe round up */
        जर (*s >= '5' && *s <= '9')
                val++;

        s += म_अखोज(s, DIGITS);

        *p = s;
        *res = val;

        परत 0;
}

पूर्णांक parse_nice(स्थिरांक अक्षर *s, पूर्णांक *ret) {
        पूर्णांक n, r;

        प्रतिज्ञा(ret);

        r = safe_atoi(s, &n);
        जर (r < 0)
                परत r;

        जर (!nice_is_valid(n))
                परत -दुस्फल;

        *ret = n;
        परत 0;
}

पूर्णांक parse_ip_port(स्थिरांक अक्षर *s, uint16_t *ret) {
        uint16_t l;
        पूर्णांक r;

        प्रतिज्ञा(ret);

        r = safe_atou16_full(s, SAFE_ATO_REFUSE_LEADING_WHITESPACE, &l);
        जर (r < 0)
                परत r;

        जर (l == 0)
                परत -EINVAL;

        *ret = l;

        परत 0;
}

पूर्णांक parse_ip_port_range(स्थिरांक अक्षर *s, uint16_t *low, uint16_t *high, bool allow_zero) {
        अचिन्हित l, h;
        पूर्णांक r;

        प्रतिज्ञा(low);
        प्रतिज्ञा(high);

        r = parse_range(s, &l, &h);
        जर (r < 0)
                परत r;

        जर (l > 65535 || h > 65535)
                परत -EINVAL;

        जर (!allow_zero && (l == 0 || h == 0))
                परत -EINVAL;

        जर (h < l)
                परत -EINVAL;

        *low = l;
        *high = h;

        परत 0;
}

पूर्णांक parse_oom_score_adjust(स्थिरांक अक्षर *s, पूर्णांक *ret) {
        पूर्णांक r, v;

        प्रतिज्ञा(s);
        प्रतिज्ञा(ret);

        r = safe_atoi(s, &v);
        जर (r < 0)
                परत r;

        जर (!oom_score_adjust_is_valid(v))
                परत -दुस्फल;

        *ret = v;
        परत 0;
}

पूर्णांक store_loadavg_fixed_point(अचिन्हित दीर्घ i, अचिन्हित दीर्घ f, loadavg_t *ret) {
        प्रतिज्ञा(ret);

        /* Validate both parts *before* shifting them, so that the shifts themselves cannot overflow.
         * Rejecting f >= 100 is equivalent to the old check of the shifted value against
         * LOADAVG_FIXED_POINT_1_0, since DIV_ROUND_UP(99 << 11, 100) == 2028 and
         * DIV_ROUND_UP(100 << 11, 100) == 2048. */
        जर (i > (~0UL >> LOADAVG_PRECISION_BITS))
                परत -दुस्फल;

        जर (f >= 100)
                परत -दुस्फल;

        *ret = (i << LOADAVG_PRECISION_BITS) | DIV_ROUND_UP(f << LOADAVG_PRECISION_BITS, 100);
        परत 0;
}

पूर्णांक parse_loadavg_fixed_point(स्थिरांक अक्षर *s, loadavg_t *ret) {
        स्थिरांक अक्षर *d, *f_str, *i_str;
        अचिन्हित दीर्घ i, f;
        पूर्णांक r;

        प्रतिज्ञा(s);
        प्रतिज्ञा(ret);

        d = म_अक्षर(s, '.');
        जर (!d)
                परत -EINVAL;

        i_str = strndupa_safe(s, d - s);
        f_str = d + 1;

        r = safe_atolu_full(i_str, 10, &i);
        जर (r < 0)
                परत r;

        r = safe_atolu_full(f_str, 10, &f);
        जर (r < 0)
                परत r;

        परत store_loadavg_fixed_point(i, f, ret);
}

/* Limitations are described in https://www.netfilter.org/projects/nftables/manpage.html and
 * https://bugzilla.netfilter.org/show_bug.cgi?id=1175 */
bool nft_identifier_valid(स्थिरांक अक्षर *id) {
        जर (isempty(id))
                परत false;

        जर (म_माप(id) >= NFT_NAME_MAXLEN)
                परत false;

        जर (!ascii_isalpha(id[0]))
                परत false;

        परत in_charset(id + 1, ALPHANUMERICAL "/\\_.");
}

पूर्णांक parse_capability_set(स्थिरांक अक्षर *s, uint64_t initial, uint64_t *current) {
        पूर्णांक r;

        प्रतिज्ञा(s);
        प्रतिज्ञा(current);

        जर (isempty(s)) {
                *current = CAP_MASK_UNSET;
                परत 1;
        }

        bool invert = false;
        जर (s[0] == '~') {
                invert = true;
                s++;
        }

        uint64_t parsed;
        r = capability_set_from_string(s, &parsed);
        जर (r < 0)
                परत r;

        जर (parsed == 0 || *current == initial)
                /* "~" or uninitialized data -> replace */
                *current = invert ? all_capabilities() & ~parsed : parsed;
        अन्यथा {
                /* previous data -> merge */
                जर (invert)
                        *current &= ~parsed;
                अन्यथा
                        *current |= parsed;
        }

        परत r;
}
