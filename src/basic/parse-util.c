/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश <linux/ipv6.h>
#समावेश <linux/netfilter/nf_tables.h>
#समावेश <stdio.h>
#समावेश <stdlib.h>
#समावेश <sys/socket.h>

#समावेश "alloc-util.h"
#समावेश "capability-list.h"
#समावेश "capability-util.h"
#समावेश "errno-list.h"
#समावेश "extract-word.h"
#समावेश "locale-util.h"
#समावेश "log.h"
#समावेश "missing-network.h"
#समावेश "parse-util.h"
#समावेश "path-util.h"
#समावेश "process-util.h"
#समावेश "string-util.h"
#समावेश "strv.h"

पूर्णांक parse_boolean(स्थिर अक्षर *v) {
        अगर (!v)
                वापस -EINVAL;

        अगर (STRCASE_IN_SET(v,
                           "1",
                           "yes",
                           "y",
                           "true",
                           "t",
                           "on"))
                वापस 1;

        अगर (STRCASE_IN_SET(v,
                           "0",
                           "no",
                           "n",
                           "false",
                           "f",
                           "off"))
                वापस 0;

        वापस -EINVAL;
}

पूर्णांक parse_tristate_full(स्थिर अक्षर *v, स्थिर अक्षर *third, पूर्णांक *ret) {
        पूर्णांक r;

        अगर (isempty(v) || streq_ptr(v, third)) { /* Empty string is always taken as the third/invalid/auto state */
                अगर (ret)
                        *ret = -1;
        } अन्यथा {
                r = parse_boolean(v);
                अगर (r < 0)
                        वापस r;

                अगर (ret)
                        *ret = r;
        }

        वापस 0;
}

पूर्णांक parse_pid(स्थिर अक्षर *s, pid_t *ret) {
        अचिन्हित दीर्घ ul = 0;
        pid_t pid;
        पूर्णांक r;

        निश्चित(s);

        r = safe_atolu(s, &ul);
        अगर (r < 0)
                वापस r;

        pid = (pid_t) ul;

        अगर ((अचिन्हित दीर्घ) pid != ul)
                वापस -दुस्फल;

        अगर (!pid_is_valid(pid))
                वापस -दुस्फल;

        अगर (ret)
                *ret = pid;
        वापस 0;
}

पूर्णांक parse_mode(स्थिर अक्षर *s, mode_t *ret) {
        अचिन्हित m;
        पूर्णांक r;

        निश्चित(s);

        r = safe_atou_full(s, 8 |
                           SAFE_ATO_REFUSE_PLUS_MINUS, /* Leading '+' or even '-' char? that's just weird,
                                                        * refuse. User might have wanted to add mode flags or
                                                        * so, but this parser doesn't allow that, so let's
                                                        * better be safe. */
                           &m);
        अगर (r < 0)
                वापस r;
        अगर (m > 07777)
                वापस -दुस्फल;

        अगर (ret)
                *ret = m;
        वापस 0;
}

पूर्णांक parse_ifindex(स्थिर अक्षर *s) {
        पूर्णांक ifi, r;

        निश्चित(s);

        r = safe_atoi(s, &ifi);
        अगर (r < 0)
                वापस r;
        अगर (ifi <= 0)
                वापस -EINVAL;

        वापस ifi;
}

पूर्णांक parse_mtu(पूर्णांक family, स्थिर अक्षर *s, uint32_t *ret) {
        uint64_t u, m;
        पूर्णांक r;

        निश्चित(ret);

        r = parse_आकार(s, 1024, &u);
        अगर (r < 0)
                वापस r;

        अगर (u > UINT32_MAX)
                वापस -दुस्फल;

        चयन (family) {
        हाल AF_INET:
                m = IPV4_MIN_MTU; /* This is 68 */
                अवरोध;
        हाल AF_INET6:
                m = IPV6_MIN_MTU; /* This is 1280 */
                अवरोध;
        शेष:
                m = 0;
        }

        अगर (u < m)
                वापस -दुस्फल;

        *ret = (uint32_t) u;
        वापस 0;
}

पूर्णांक parse_आकार(स्थिर अक्षर *t, uint64_t base, uint64_t *आकार) {

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

        काष्ठा table {
                स्थिर अक्षर *suffix;
                अचिन्हित दीर्घ दीर्घ factor;
        };

        अटल स्थिर काष्ठा table iec[] = {
                { "E", 1024ULL*1024ULL*1024ULL*1024ULL*1024ULL*1024ULL },
                { "P", 1024ULL*1024ULL*1024ULL*1024ULL*1024ULL },
                { "T", 1024ULL*1024ULL*1024ULL*1024ULL },
                { "G", 1024ULL*1024ULL*1024ULL },
                { "M", 1024ULL*1024ULL },
                { "K", 1024ULL },
                { "B", 1ULL },
                { "",  1ULL },
        };

        अटल स्थिर काष्ठा table si[] = {
                { "E", 1000ULL*1000ULL*1000ULL*1000ULL*1000ULL*1000ULL },
                { "P", 1000ULL*1000ULL*1000ULL*1000ULL*1000ULL },
                { "T", 1000ULL*1000ULL*1000ULL*1000ULL },
                { "G", 1000ULL*1000ULL*1000ULL },
                { "M", 1000ULL*1000ULL },
                { "K", 1000ULL },
                { "B", 1ULL },
                { "",  1ULL },
        };

        स्थिर काष्ठा table *table;
        स्थिर अक्षर *p;
        अचिन्हित दीर्घ दीर्घ r = 0;
        अचिन्हित n_entries, start_pos = 0;

        निश्चित(t);
        निश्चित(IN_SET(base, 1000, 1024));
        निश्चित(आकार);

        अगर (base == 1000) {
                table = si;
                n_entries = ELEMENTSOF(si);
        } अन्यथा {
                table = iec;
                n_entries = ELEMENTSOF(iec);
        }

        p = t;
        करो {
                अचिन्हित दीर्घ दीर्घ l, tmp;
                द्विगुन frac = 0;
                अक्षर *e;
                अचिन्हित i;

                p += म_अखोज(p, WHITESPACE);

                त्रुटि_सं = 0;
                l = strtoull(p, &e, 10);
                अगर (त्रुटि_सं > 0)
                        वापस -त्रुटि_सं;
                अगर (e == p)
                        वापस -EINVAL;
                अगर (*p == '-')
                        वापस -दुस्फल;

                अगर (*e == '.') {
                        e++;

                        /* strtoull() itself would accept space/+/- */
                        अगर (ascii_isdigit(*e)) {
                                अचिन्हित दीर्घ दीर्घ l2;
                                अक्षर *e2;

                                l2 = strtoull(e, &e2, 10);
                                अगर (त्रुटि_सं > 0)
                                        वापस -त्रुटि_सं;

                                /* Ignore failure. E.g. 10.M is valid */
                                frac = l2;
                                क्रम (; e < e2; e++)
                                        frac /= 10;
                        }
                }

                e += म_अखोज(e, WHITESPACE);

                क्रम (i = start_pos; i < n_entries; i++)
                        अगर (startswith(e, table[i].suffix))
                                अवरोध;

                अगर (i >= n_entries)
                        वापस -EINVAL;

                अगर (l + (frac > 0) > ULLONG_MAX / table[i].factor)
                        वापस -दुस्फल;

                tmp = l * table[i].factor + (अचिन्हित दीर्घ दीर्घ) (frac * table[i].factor);
                अगर (tmp > ULLONG_MAX - r)
                        वापस -दुस्फल;

                r += tmp;
                अगर ((अचिन्हित दीर्घ दीर्घ) (uint64_t) r != r)
                        वापस -दुस्फल;

                p = e + म_माप(table[i].suffix);

                start_pos = i + 1;

        } जबतक (*p);

        *आकार = r;

        वापस 0;
}

पूर्णांक parse_sector_आकार(स्थिर अक्षर *t, uint64_t *ret) {
        पूर्णांक r;

        निश्चित(t);
        निश्चित(ret);

        uint64_t ss;

        r = safe_atou64(t, &ss);
        अगर (r < 0)
                वापस log_error_errno(r, "Failed to parse sector size parameter %s", t);
        अगर (ss < 512 || ss > 4096) /* Allow up to 4K due to dm-crypt support and 4K alignment by the homed LUKS backend */
                वापस log_error_errno(SYNTHETIC_ERRNO(दुस्फल), "Sector size not between 512 and 4096: %s", t);
        अगर (!ISPOWEROF2(ss))
                वापस log_error_errno(SYNTHETIC_ERRNO(EINVAL), "Sector size not power of 2: %s", t);

        *ret = ss;
        वापस 0;
}

पूर्णांक parse_range(स्थिर अक्षर *t, अचिन्हित *lower, अचिन्हित *upper) {
        _cleanup_free_ अक्षर *word = शून्य;
        अचिन्हित l, u;
        पूर्णांक r;

        निश्चित(lower);
        निश्चित(upper);

        /* Extract the lower bound. */
        r = extract_first_word(&t, &word, "-", EXTRACT_DONT_COALESCE_SEPARATORS);
        अगर (r < 0)
                वापस r;
        अगर (r == 0)
                वापस -EINVAL;

        r = safe_atou(word, &l);
        अगर (r < 0)
                वापस r;

        /* Check for the upper bound and extract it if needed */
        अगर (!t)
                /* Single number with no dashes. */
                u = l;
        अन्यथा अगर (!*t)
                /* Trailing dash is an error. */
                वापस -EINVAL;
        अन्यथा {
                r = safe_atou(t, &u);
                अगर (r < 0)
                        वापस r;
        }

        *lower = l;
        *upper = u;
        वापस 0;
}

पूर्णांक parse_errno(स्थिर अक्षर *t) {
        पूर्णांक r, e;

        निश्चित(t);

        r = errno_from_name(t);
        अगर (r > 0)
                वापस r;

        r = safe_atoi(t, &e);
        अगर (r < 0)
                वापस r;

        /* 0 is also allowed here */
        अगर (!errno_is_valid(e) && e != 0)
                वापस -दुस्फल;

        वापस e;
}

पूर्णांक parse_fd(स्थिर अक्षर *t) {
        पूर्णांक r, fd;

        निश्चित(t);

        r = safe_atoi(t, &fd);
        अगर (r < 0)
                वापस r;

        अगर (fd < 0)
                वापस -EBADF;

        वापस fd;
}

पूर्णांक parse_user_shell(स्थिर अक्षर *s, अक्षर **ret_sh, bool *ret_copy) {
        अक्षर *sh;
        पूर्णांक r;

        निश्चित(ret_sh);
        निश्चित(ret_copy);

        अगर (path_is_absolute(s) && path_is_normalized(s)) {
                sh = strdup(s);
                अगर (!sh)
                        वापस -ENOMEM;

                *ret_sh = sh;
                *ret_copy = false;
        } अन्यथा {
                r = parse_boolean(s);
                अगर (r < 0)
                        वापस r;

                *ret_sh = शून्य;
                *ret_copy = r;
        }

        वापस 0;
}

अटल स्थिर अक्षर *mangle_base(स्थिर अक्षर *s, अचिन्हित *base) {
        स्थिर अक्षर *k;

        निश्चित(s);
        निश्चित(base);

        /* Base already explicitly specified, then don't do anything. */
        अगर (SAFE_ATO_MASK_FLAGS(*base) != 0)
                वापस s;

        /* Support Python 3 style "0b" and 0x" prefixes, because they truly make sense, much more than C's "0" prefix for octal. */
        k = STARTSWITH_SET(s, "0b", "0B");
        अगर (k) {
                *base = 2 | (*base & SAFE_ATO_ALL_FLAGS);
                वापस k;
        }

        k = STARTSWITH_SET(s, "0o", "0O");
        अगर (k) {
                *base = 8 | (*base & SAFE_ATO_ALL_FLAGS);
                वापस k;
        }

        वापस s;
}

पूर्णांक safe_atou_full(स्थिर अक्षर *s, अचिन्हित base, अचिन्हित *ret_u) {
        अक्षर *x = शून्य;
        अचिन्हित दीर्घ l;

        निश्चित(s);
        निश्चित(SAFE_ATO_MASK_FLAGS(base) <= 16);

        /* strtoul() is happy to parse negative values, and silently converts them to unsigned values without
         * generating an error. We want a clean error, hence let's look for the "-" prefix on our own, and
         * generate an error. But let's do so only after strtoul() validated that the string is clean
         * otherwise, so that we return EINVAL preferably over ERANGE. */

        अगर (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_WHITESPACE) &&
            म_अक्षर(WHITESPACE, s[0]))
                वापस -EINVAL;

        s += म_अखोज(s, WHITESPACE);

        अगर (FLAGS_SET(base, SAFE_ATO_REFUSE_PLUS_MINUS) &&
            IN_SET(s[0], '+', '-'))
                वापस -EINVAL; /* Note that we check the "-" prefix again a second time below, but return a
                                 * different error. I.e. if the SAFE_ATO_REFUSE_PLUS_MINUS flag is set we
                                 * blanket refuse +/- prefixed integers, while if it is missing we'll just
                                 * return ERANGE, because the string actually parses correctly, but doesn't
                                 * fit in the return type. */

        अगर (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_ZERO) &&
            s[0] == '0' && !streq(s, "0"))
                वापस -EINVAL; /* This is particularly useful to avoid ambiguities between C's octal
                                 * notation and assumed-to-be-decimal integers with a leading zero. */

        s = mangle_base(s, &base);

        त्रुटि_सं = 0;
        l = म_से_अदीर्घ(s, &x, SAFE_ATO_MASK_FLAGS(base) /* Let's mask off the flags bits so that only the actual
                                                      * base is left */);
        अगर (त्रुटि_सं > 0)
                वापस -त्रुटि_सं;
        अगर (!x || x == s || *x != 0)
                वापस -EINVAL;
        अगर (l != 0 && s[0] == '-')
                वापस -दुस्फल;
        अगर ((अचिन्हित दीर्घ) (अचिन्हित) l != l)
                वापस -दुस्फल;

        अगर (ret_u)
                *ret_u = (अचिन्हित) l;

        वापस 0;
}

पूर्णांक safe_atou_bounded(स्थिर अक्षर *s, अचिन्हित min, अचिन्हित max, अचिन्हित *ret) {
        अचिन्हित v;
        पूर्णांक r;

        निश्चित(ret);

        r = safe_atou(s, &v);
        अगर (r < 0)
                वापस r;

        अगर (v < min || v > max)
                वापस -दुस्फल;

        *ret = v;
        वापस 0;
}

पूर्णांक safe_atoi(स्थिर अक्षर *s, पूर्णांक *ret_i) {
        अचिन्हित base = 0;
        अक्षर *x = शून्य;
        दीर्घ l;

        निश्चित(s);

        s += म_अखोज(s, WHITESPACE);
        s = mangle_base(s, &base);

        त्रुटि_सं = 0;
        l = म_से_दीर्घ(s, &x, base);
        अगर (त्रुटि_सं > 0)
                वापस -त्रुटि_सं;
        अगर (!x || x == s || *x != 0)
                वापस -EINVAL;
        अगर ((दीर्घ) (पूर्णांक) l != l)
                वापस -दुस्फल;

        अगर (ret_i)
                *ret_i = (पूर्णांक) l;

        वापस 0;
}

पूर्णांक safe_atollu_full(स्थिर अक्षर *s, अचिन्हित base, अचिन्हित दीर्घ दीर्घ *ret_llu) {
        अक्षर *x = शून्य;
        अचिन्हित दीर्घ दीर्घ l;

        निश्चित(s);
        निश्चित(SAFE_ATO_MASK_FLAGS(base) <= 16);

        अगर (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_WHITESPACE) &&
            म_अक्षर(WHITESPACE, s[0]))
                वापस -EINVAL;

        s += म_अखोज(s, WHITESPACE);

        अगर (FLAGS_SET(base, SAFE_ATO_REFUSE_PLUS_MINUS) &&
            IN_SET(s[0], '+', '-'))
                वापस -EINVAL;

        अगर (FLAGS_SET(base, SAFE_ATO_REFUSE_LEADING_ZERO) &&
            s[0] == '0' && s[1] != 0)
                वापस -EINVAL;

        s = mangle_base(s, &base);

        त्रुटि_सं = 0;
        l = strtoull(s, &x, SAFE_ATO_MASK_FLAGS(base));
        अगर (त्रुटि_सं > 0)
                वापस -त्रुटि_सं;
        अगर (!x || x == s || *x != 0)
                वापस -EINVAL;
        अगर (l != 0 && s[0] == '-')
                वापस -दुस्फल;

        अगर (ret_llu)
                *ret_llu = l;

        वापस 0;
}

पूर्णांक safe_atolli(स्थिर अक्षर *s, दीर्घ दीर्घ *ret_lli) {
        अचिन्हित base = 0;
        अक्षर *x = शून्य;
        दीर्घ दीर्घ l;

        निश्चित(s);

        s += म_अखोज(s, WHITESPACE);
        s = mangle_base(s, &base);

        त्रुटि_सं = 0;
        l = strtoll(s, &x, base);
        अगर (त्रुटि_सं > 0)
                वापस -त्रुटि_सं;
        अगर (!x || x == s || *x != 0)
                वापस -EINVAL;

        अगर (ret_lli)
                *ret_lli = l;

        वापस 0;
}

पूर्णांक safe_atou8_full(स्थिर अक्षर *s, अचिन्हित base, uint8_t *ret) {
        अचिन्हित u;
        पूर्णांक r;

        निश्चित(ret);

        r = safe_atou_full(s, base, &u);
        अगर (r < 0)
                वापस r;
        अगर (u > UINT8_MAX)
                वापस -दुस्फल;

        *ret = (uint8_t) u;
        वापस 0;
}

पूर्णांक safe_atou16_full(स्थिर अक्षर *s, अचिन्हित base, uint16_t *ret) {
        अचिन्हित u;
        पूर्णांक r;

        निश्चित(ret);

        r = safe_atou_full(s, base, &u);
        अगर (r < 0)
                वापस r;
        अगर (u > UINT16_MAX)
                वापस -दुस्फल;

        *ret = (uint16_t) u;
        वापस 0;
}

पूर्णांक safe_atoi16(स्थिर अक्षर *s, int16_t *ret) {
        अचिन्हित base = 0;
        अक्षर *x = शून्य;
        दीर्घ l;

        निश्चित(s);

        s += म_अखोज(s, WHITESPACE);
        s = mangle_base(s, &base);

        त्रुटि_सं = 0;
        l = म_से_दीर्घ(s, &x, base);
        अगर (त्रुटि_सं > 0)
                वापस -त्रुटि_सं;
        अगर (!x || x == s || *x != 0)
                वापस -EINVAL;
        अगर ((दीर्घ) (int16_t) l != l)
                वापस -दुस्फल;

        अगर (ret)
                *ret = (int16_t) l;

        वापस 0;
}

पूर्णांक safe_atod(स्थिर अक्षर *s, द्विगुन *ret_d) {
        _cleanup_(freelocalep) locale_t loc = (locale_t) 0;
        अक्षर *x = शून्य;
        द्विगुन d = 0;

        निश्चित(s);

        loc = newlocale(LC_NUMERIC_MASK, "C", (locale_t) 0);
        अगर (loc == (locale_t) 0)
                वापस -त्रुटि_सं;

        त्रुटि_सं = 0;
        d = strtod_l(s, &x, loc);
        अगर (त्रुटि_सं > 0)
                वापस -त्रुटि_सं;
        अगर (!x || x == s || *x != 0)
                वापस -EINVAL;

        अगर (ret_d)
                *ret_d = d;

        वापस 0;
}

पूर्णांक parse_fractional_part_u(स्थिर अक्षर **p, माप_प्रकार digits, अचिन्हित *res) {
        अचिन्हित val = 0;
        स्थिर अक्षर *s;

        निश्चित(p);
        निश्चित(res);

        s = *p;

        /* accept any number of digits, strtoull is limited to 19 */
        क्रम (माप_प्रकार i = 0; i < digits; i++, s++) {
                अगर (!ascii_isdigit(*s)) {
                        अगर (i == 0)
                                वापस -EINVAL;

                        /* too few digits, pad with 0 */
                        क्रम (; i < digits; i++)
                                val *= 10;

                        अवरोध;
                }

                val *= 10;
                val += *s - '0';
        }

        /* maybe round up */
        अगर (*s >= '5' && *s <= '9')
                val++;

        s += म_अखोज(s, DIGITS);

        *p = s;
        *res = val;

        वापस 0;
}

पूर्णांक parse_nice(स्थिर अक्षर *s, पूर्णांक *ret) {
        पूर्णांक n, r;

        निश्चित(ret);

        r = safe_atoi(s, &n);
        अगर (r < 0)
                वापस r;

        अगर (!nice_is_valid(n))
                वापस -दुस्फल;

        *ret = n;
        वापस 0;
}

पूर्णांक parse_ip_port(स्थिर अक्षर *s, uint16_t *ret) {
        uint16_t l;
        पूर्णांक r;

        निश्चित(ret);

        r = safe_atou16_full(s, SAFE_ATO_REFUSE_LEADING_WHITESPACE, &l);
        अगर (r < 0)
                वापस r;

        अगर (l == 0)
                वापस -EINVAL;

        *ret = l;

        वापस 0;
}

पूर्णांक parse_ip_port_range(स्थिर अक्षर *s, uint16_t *low, uint16_t *high, bool allow_zero) {
        अचिन्हित l, h;
        पूर्णांक r;

        निश्चित(low);
        निश्चित(high);

        r = parse_range(s, &l, &h);
        अगर (r < 0)
                वापस r;

        अगर (l > 65535 || h > 65535)
                वापस -EINVAL;

        अगर (!allow_zero && (l == 0 || h == 0))
                वापस -EINVAL;

        अगर (h < l)
                वापस -EINVAL;

        *low = l;
        *high = h;

        वापस 0;
}

पूर्णांक parse_oom_score_adjust(स्थिर अक्षर *s, पूर्णांक *ret) {
        पूर्णांक r, v;

        निश्चित(s);
        निश्चित(ret);

        r = safe_atoi(s, &v);
        अगर (r < 0)
                वापस r;

        अगर (!oom_score_adjust_is_valid(v))
                वापस -दुस्फल;

        *ret = v;
        वापस 0;
}

पूर्णांक store_loadavg_fixed_point(अचिन्हित दीर्घ i, अचिन्हित दीर्घ f, loadavg_t *ret) {
        निश्चित(ret);

        /* Validate both parts *before* shifting them, so that the shifts themselves cannot overflow.
         * Rejecting f >= 100 is equivalent to the old check of the shifted value against
         * LOADAVG_FIXED_POINT_1_0, since DIV_ROUND_UP(99 << 11, 100) == 2028 and
         * DIV_ROUND_UP(100 << 11, 100) == 2048. */
        अगर (i > (~0UL >> LOADAVG_PRECISION_BITS))
                वापस -दुस्फल;

        अगर (f >= 100)
                वापस -दुस्फल;

        *ret = (i << LOADAVG_PRECISION_BITS) | DIV_ROUND_UP(f << LOADAVG_PRECISION_BITS, 100);
        वापस 0;
}

पूर्णांक parse_loadavg_fixed_point(स्थिर अक्षर *s, loadavg_t *ret) {
        स्थिर अक्षर *d, *f_str, *i_str;
        अचिन्हित दीर्घ i, f;
        पूर्णांक r;

        निश्चित(s);
        निश्चित(ret);

        d = म_अक्षर(s, '.');
        अगर (!d)
                वापस -EINVAL;

        i_str = strndupa_safe(s, d - s);
        f_str = d + 1;

        r = safe_atolu_full(i_str, 10, &i);
        अगर (r < 0)
                वापस r;

        r = safe_atolu_full(f_str, 10, &f);
        अगर (r < 0)
                वापस r;

        वापस store_loadavg_fixed_point(i, f, ret);
}

/* Limitations are described in https://www.netfilter.org/projects/nftables/manpage.html and
 * https://bugzilla.netfilter.org/show_bug.cgi?id=1175 */
bool nft_identifier_valid(स्थिर अक्षर *id) {
        अगर (isempty(id))
                वापस false;

        अगर (म_माप(id) >= NFT_NAME_MAXLEN)
                वापस false;

        अगर (!ascii_isalpha(id[0]))
                वापस false;

        वापस in_charset(id + 1, ALPHANUMERICAL "/\\_.");
}

पूर्णांक parse_capability_set(स्थिर अक्षर *s, uint64_t initial, uint64_t *current) {
        पूर्णांक r;

        निश्चित(s);
        निश्चित(current);

        अगर (isempty(s)) {
                *current = CAP_MASK_UNSET;
                वापस 1;
        }

        bool invert = false;
        अगर (s[0] == '~') {
                invert = true;
                s++;
        }

        uint64_t parsed;
        r = capability_set_from_string(s, &parsed);
        अगर (r < 0)
                वापस r;

        अगर (parsed == 0 || *current == initial)
                /* "~" or uninitialized data -> replace */
                *current = invert ? all_capabilities() & ~parsed : parsed;
        अन्यथा {
                /* previous data -> merge */
                अगर (invert)
                        *current &= ~parsed;
                अन्यथा
                        *current |= parsed;
        }

        वापस r;
}
