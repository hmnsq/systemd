/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <linux/ipv6.h>
#شامل <linux/netfilter/nf_tables.h>
#شامل <stdio.h>
#شامل <stdlib.h>
#شامل <sys/socket.h>

#شامل "alloc-util.h"
#شامل "capability-list.h"
#شامل "capability-util.h"
#شامل "errno-list.h"
#شامل "extract-word.h"
#شامل "locale-util.h"
#شامل "log.h"
#شامل "missing-network.h"
#شامل "parse-util.h"
#شامل "path-util.h"
#شامل "process-util.h"
#شامل "string-util.h"
#شامل "strv.h"

صحیح parse_boolean(ثابت حرف *v) {
        اگر (!v)
                واپس -EINVAL;

        اگر (STRCASE_اندر_سیٹ(v,
                           "1",
                           "yes",
                           "y",
                           "true",
                           "t",
                           "on"))
                واپس 1;

        اگر (STRCASE_اندر_سیٹ(v,
                           "0",
                           "no",
                           "n",
                           "false",
                           "f",
                           "off"))
                واپس 0;

        واپس -EINVAL;
}

صحیح parse_tristate_بھرا(ثابت حرف *v, ثابت حرف *تیسرا, صحیح *ret) {
        صحیح r;

        اگر (isempty(v) || streq_ptr(v, تیسرا)) { /* Empty string is always taken as the third/invalid/auto state */
                اگر (ret)
                        *ret = -1;
        } ورنہ {
                r = parse_boolean(v);
                اگر (r < 0)
                        واپس r;

                اگر (ret)
                        *ret = r;
        }

        واپس 0;
}

صحیح parse_pid(ثابت حرف *s, pid_t *ret) {
        غیرعلامتی طویل ul = 0;
        pid_t pid;
        صحیح r;

        أكد(s);

        r = محفوظ_atolu(s, &ul);
        اگر (r < 0)
                واپس r;

        pid = (pid_t) ul;

        اگر ((غیرعلامتی طویل) pid != ul)
                واپس -ERANGE;

        اگر (!pid_is_valid(pid))
                واپس -ERANGE;

        اگر (ret)
                *ret = pid;
        واپس 0;
}

صحیح parse_منوال(ثابت حرف *s, mode_t *ret) {
        غیرعلامتی m;
        صحیح r;

        أكد(s);

        r = محفوظ_atou_بھرا(s, 8 |
                           محفوظ_ATO_REFUSE_PLUS_MINUS, /* Leading '+' or even '-' char? that's just weird,
                                                        * refuse. User might have wanted to add mode flags or
                                                        * so, but this parser doesn't allow that, so let's
                                                        * better be safe. */
                           &m);
        اگر (r < 0)
                واپس r;
        اگر (m > 07777)
                واپس -ERANGE;

        اگر (ret)
                *ret = m;
        واپس 0;
}

صحیح parse_ifindex(ثابت حرف *s) {
        صحیح ifi, r;

        أكد(s);

        r = محفوظ_atoi(s, &ifi);
        اگر (r < 0)
                واپس r;
        اگر (ifi <= 0)
                واپس -EINVAL;

        واپس ifi;
}

صحیح parse_mtu(صحیح family, ثابت حرف *s, uint32_t *ret) {
        uint64_t u, m;
        صحیح r;

        أكد(ret);

        r = parse_سائز(s, 1024, &u);
        اگر (r < 0)
                واپس r;

        اگر (u > UINT32_MAX)
                واپس -ERANGE;

        انتخاب (family) {
        حالت AF_INET:
                m = IPV4_MIN_MTU; /* This is 68 */
                توڑو;
        حالت AF_INET6:
                m = IPV6_MIN_MTU; /* This is 1280 */
                توڑو;
        طےشدہ:
                m = 0;
        }

        اگر (u < m)
                واپس -ERANGE;

        *ret = (uint32_t) u;
        واپس 0;
}

صحیح parse_سائز(ثابت حرف *t, uint64_t base, uint64_t *سائز) {

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

        ساخت جدول {
                ثابت حرف *suffix;
                غیرعلامتی طویل طویل ضربی_عدد;
        };

        ساکن ثابت ساخت جدول iec[] = {
                { "E", 1024ULL*1024ULL*1024ULL*1024ULL*1024ULL*1024ULL },
                { "P", 1024ULL*1024ULL*1024ULL*1024ULL*1024ULL },
                { "T", 1024ULL*1024ULL*1024ULL*1024ULL },
                { "G", 1024ULL*1024ULL*1024ULL },
                { "M", 1024ULL*1024ULL },
                { "K", 1024ULL },
                { "B", 1ULL },
                { "",  1ULL },
        };

        ساکن ثابت ساخت جدول si[] = {
                { "E", 1000ULL*1000ULL*1000ULL*1000ULL*1000ULL*1000ULL },
                { "P", 1000ULL*1000ULL*1000ULL*1000ULL*1000ULL },
                { "T", 1000ULL*1000ULL*1000ULL*1000ULL },
                { "G", 1000ULL*1000ULL*1000ULL },
                { "M", 1000ULL*1000ULL },
                { "K", 1000ULL },
                { "B", 1ULL },
                { "",  1ULL },
        };

        ثابت ساخت جدول *جدول;
        ثابت حرف *p;
        غیرعلامتی طویل طویل r = 0;
        غیرعلامتی n_entries, آغاز_pos = 0;

        أكد(t);
        أكد(اندر_سیٹ(base, 1000, 1024));
        أكد(سائز);

        اگر (base == 1000) {
                جدول = si;
                n_entries = ELEMENTSOF(si);
        } ورنہ {
                جدول = iec;
                n_entries = ELEMENTSOF(iec);
        }

        p = t;
        کرو {
                غیرعلامتی طویل طویل l, tmp;
                دوہرا frac = 0;
                حرف *e;
                غیرعلامتی i;

                p += strspn(p, WHITESPACE);

                errno = 0;
                l = strtoull(p, &e, 10);
                اگر (errno > 0)
                        واپس -errno;
                اگر (e == p)
                        واپس -EINVAL;
                اگر (*p == '-')
                        واپس -ERANGE;

                اگر (*e == '.') {
                        e++;

                        /* strtoull() itself would accept space/+/- */
                        اگر (ascii_isdigit(*e)) {
                                غیرعلامتی طویل طویل l2;
                                حرف *e2;

                                l2 = strtoull(e, &e2, 10);
                                اگر (errno > 0)
                                        واپس -errno;

                                /* Ignore failure. E.g. 10.M is valid */
                                frac = l2;
                                برائے (; e < e2; e++)
                                        frac /= 10;
                        }
                }

                e += strspn(e, WHITESPACE);

                برائے (i = آغاز_pos; i < n_entries; i++)
                        اگر (startswith(e, جدول[i].suffix))
                                توڑو;

                اگر (i >= n_entries)
                        واپس -EINVAL;

                اگر (l + (frac > 0) > ULLONG_MAX / جدول[i].ضربی_عدد)
                        واپس -ERANGE;

                tmp = l * جدول[i].ضربی_عدد + (غیرعلامتی طویل طویل) (frac * جدول[i].ضربی_عدد);
                اگر (tmp > ULLONG_MAX - r)
                        واپس -ERANGE;

                r += tmp;
                اگر ((غیرعلامتی طویل طویل) (uint64_t) r != r)
                        واپس -ERANGE;

                p = e + لڑی_لمبائی(جدول[i].suffix);

                آغاز_pos = i + 1;

        } جب (*p);

        *سائز = r;

        واپس 0;
}

صحیح parse_sector_سائز(ثابت حرف *t, uint64_t *ret) {
        صحیح r;

        أكد(t);
        أكد(ret);

        uint64_t ss;

        r = محفوظ_atou64(t, &ss);
        اگر (r < 0)
                واپس log_error_errno(r, "Failed to parse sector size parameter %s", t);
        اگر (ss < 512 || ss > 4096) /* Allow up to 4K due to dm-crypt support and 4K alignment by the homed LUKS backend */
                واپس log_error_errno(SYNTHETIC_ERRNO(ERANGE), "Sector size not between 512 and 4096: %s", t);
        اگر (!ISPOWEROF2(ss))
                واپس log_error_errno(SYNTHETIC_ERRNO(EINVAL), "Sector size not power of 2: %s", t);

        *ret = ss;
        واپس 0;
}

صحیح parse_range(ثابت حرف *t, غیرعلامتی *زیریں, غیرعلامتی *بالائی) {
        _cleanup_free_ حرف *لفظ = تہی;
        غیرعلامتی l, u;
        صحیح r;

        أكد(زیریں);
        أكد(بالائی);

        /* Extract the lower bound. */
        r = نکالو_پہلا_لفظ(&t, &لفظ, "-", نکالو_DONT_COALESCE_SEPARATORS);
        اگر (r < 0)
                واپس r;
        اگر (r == 0)
                واپس -EINVAL;

        r = محفوظ_atou(لفظ, &l);
        اگر (r < 0)
                واپس r;

        /* Check for the upper bound and extract it if needed */
        اگر (!t)
                /* Single number with no dashes. */
                u = l;
        ورنہ اگر (!*t)
                /* Trailing dash is an error. */
                واپس -EINVAL;
        ورنہ {
                r = محفوظ_atou(t, &u);
                اگر (r < 0)
                        واپس r;
        }

        *زیریں = l;
        *بالائی = u;
        واپس 0;
}

صحیح parse_errno(ثابت حرف *t) {
        صحیح r, e;

        أكد(t);

        r = errno_from_name(t);
        اگر (r > 0)
                واپس r;

        r = محفوظ_atoi(t, &e);
        اگر (r < 0)
                واپس r;

        /* 0 is also allowed here */
        اگر (!errno_is_valid(e) && e != 0)
                واپس -ERANGE;

        واپس e;
}

صحیح parse_fd(ثابت حرف *t) {
        صحیح r, fd;

        أكد(t);

        r = محفوظ_atoi(t, &fd);
        اگر (r < 0)
                واپس r;

        اگر (fd < 0)
                واپس -EBADF;

        واپس fd;
}

صحیح parse_user_shell(ثابت حرف *s, حرف **ret_sh, bool *ret_copy) {
        حرف *sh;
        صحیح r;

        أكد(ret_sh);
        أكد(ret_copy);

        اگر (راستہ_is_absolute(s) && راستہ_is_normalized(s)) {
                sh = strdup(s);
                اگر (!sh)
                        واپس -ENOMEM;

                *ret_sh = sh;
                *ret_copy = false;
        } ورنہ {
                r = parse_boolean(s);
                اگر (r < 0)
                        واپس r;

                *ret_sh = تہی;
                *ret_copy = r;
        }

        واپس 0;
}

ساکن ثابت حرف *mangle_بنیاد(ثابت حرف *s, غیرعلامتی *base) {
        ثابت حرف *k;

        أكد(s);
        أكد(base);

        /* Base already explicitly specified, then don't do anything. */
        اگر (محفوظ_ATO_نقاب_FLAGS(*base) != 0)
                واپس s;

        /* Support Python 3 style "0b" and 0x" prefixes, because they truly make sense, much more than C's "0" prefix for octal. */
        k = STARTSWITH_سیٹ(s, "0b", "0B");
        اگر (k) {
                *base = 2 | (*base & محفوظ_ATO_ALL_FLAGS);
                واپس k;
        }

        k = STARTSWITH_سیٹ(s, "0o", "0O");
        اگر (k) {
                *base = 8 | (*base & محفوظ_ATO_ALL_FLAGS);
                واپس k;
        }

        واپس s;
}

صحیح محفوظ_atou_بھرا(ثابت حرف *s, غیرعلامتی base, غیرعلامتی *ret_u) {
        حرف *x = تہی;
        غیرعلامتی طویل l;

        أكد(s);
        أكد(محفوظ_ATO_نقاب_FLAGS(base) <= 16);

        /* strtoul() is happy to parse negative values, and silently converts them to unsigned values without
         * generating an error. We want a clean error, hence let's look for the "-" prefix on our own, and
         * generate an error. But let's do so only after strtoul() validated that the string is clean
         * otherwise, so that we return EINVAL preferably over ERANGE. */

        اگر (FLAGS_سیٹ(base, محفوظ_ATO_REFUSE_LEADING_WHITESPACE) &&
            strchr(WHITESPACE, s[0]))
                واپس -EINVAL;

        s += strspn(s, WHITESPACE);

        اگر (FLAGS_سیٹ(base, محفوظ_ATO_REFUSE_PLUS_MINUS) &&
            اندر_سیٹ(s[0], '+', '-'))
                واپس -EINVAL; /* Note that we check the "-" prefix again a second time below, but return a
                                 * different error. I.e. if the SAFE_ATO_REFUSE_PLUS_MINUS flag is set we
                                 * blanket refuse +/- prefixed integers, while if it is missing we'll just
                                 * return ERANGE, because the string actually parses correctly, but doesn't
                                 * fit in the return type. */

        اگر (FLAGS_سیٹ(base, محفوظ_ATO_REFUSE_LEADING_ZERO) &&
            s[0] == '0' && !streq(s, "0"))
                واپس -EINVAL; /* This is particularly useful to avoid ambiguities between C's octal
                                 * notation and assumed-to-be-decimal integers with a leading zero. */

        s = mangle_بنیاد(s, &base);

        errno = 0;
        l = strtoul(s, &x, محفوظ_ATO_نقاب_FLAGS(base) /* Let's mask off the flags bits so that only the actual
                                                      * base is left */);
        اگر (errno > 0)
                واپس -errno;
        اگر (!x || x == s || *x != 0)
                واپس -EINVAL;
        اگر (l != 0 && s[0] == '-')
                واپس -ERANGE;
        اگر ((غیرعلامتی طویل) (غیرعلامتی) l != l)
                واپس -ERANGE;

        اگر (ret_u)
                *ret_u = (غیرعلامتی) l;

        واپس 0;
}

صحیح محفوظ_atou_bounded(ثابت حرف *s, غیرعلامتی min, غیرعلامتی max, غیرعلامتی *ret) {
        غیرعلامتی v;
        صحیح r;

        أكد(ret);

        r = محفوظ_atou(s, &v);
        اگر (r < 0)
                واپس r;

        اگر (v < min || v > max)
                واپس -ERANGE;

        *ret = v;
        واپس 0;
}

صحیح محفوظ_atoi(ثابت حرف *s, صحیح *ret_i) {
        غیرعلامتی base = 0;
        حرف *x = تہی;
        طویل l;

        أكد(s);

        s += strspn(s, WHITESPACE);
        s = mangle_بنیاد(s, &base);

        errno = 0;
        l = strtol(s, &x, base);
        اگر (errno > 0)
                واپس -errno;
        اگر (!x || x == s || *x != 0)
                واپس -EINVAL;
        اگر ((طویل) (صحیح) l != l)
                واپس -ERANGE;

        اگر (ret_i)
                *ret_i = (صحیح) l;

        واپس 0;
}

صحیح محفوظ_atollu_بھرا(ثابت حرف *s, غیرعلامتی base, غیرعلامتی طویل طویل *ret_llu) {
        حرف *x = تہی;
        غیرعلامتی طویل طویل l;

        أكد(s);
        أكد(محفوظ_ATO_نقاب_FLAGS(base) <= 16);

        اگر (FLAGS_سیٹ(base, محفوظ_ATO_REFUSE_LEADING_WHITESPACE) &&
            strchr(WHITESPACE, s[0]))
                واپس -EINVAL;

        s += strspn(s, WHITESPACE);

        اگر (FLAGS_سیٹ(base, محفوظ_ATO_REFUSE_PLUS_MINUS) &&
            اندر_سیٹ(s[0], '+', '-'))
                واپس -EINVAL;

        اگر (FLAGS_سیٹ(base, محفوظ_ATO_REFUSE_LEADING_ZERO) &&
            s[0] == '0' && s[1] != 0)
                واپس -EINVAL;

        s = mangle_بنیاد(s, &base);

        errno = 0;
        l = strtoull(s, &x, محفوظ_ATO_نقاب_FLAGS(base));
        اگر (errno > 0)
                واپس -errno;
        اگر (!x || x == s || *x != 0)
                واپس -EINVAL;
        اگر (l != 0 && s[0] == '-')
                واپس -ERANGE;

        اگر (ret_llu)
                *ret_llu = l;

        واپس 0;
}

صحیح محفوظ_atolli(ثابت حرف *s, طویل طویل *ret_lli) {
        غیرعلامتی base = 0;
        حرف *x = تہی;
        طویل طویل l;

        أكد(s);

        s += strspn(s, WHITESPACE);
        s = mangle_بنیاد(s, &base);

        errno = 0;
        l = strtoll(s, &x, base);
        اگر (errno > 0)
                واپس -errno;
        اگر (!x || x == s || *x != 0)
                واپس -EINVAL;

        اگر (ret_lli)
                *ret_lli = l;

        واپس 0;
}

صحیح محفوظ_atou8_بھرا(ثابت حرف *s, غیرعلامتی base, uint8_t *ret) {
        غیرعلامتی u;
        صحیح r;

        أكد(ret);

        r = محفوظ_atou_بھرا(s, base, &u);
        اگر (r < 0)
                واپس r;
        اگر (u > UINT8_MAX)
                واپس -ERANGE;

        *ret = (uint8_t) u;
        واپس 0;
}

صحیح محفوظ_atou16_بھرا(ثابت حرف *s, غیرعلامتی base, uint16_t *ret) {
        غیرعلامتی u;
        صحیح r;

        أكد(ret);

        r = محفوظ_atou_بھرا(s, base, &u);
        اگر (r < 0)
                واپس r;
        اگر (u > UINT16_MAX)
                واپس -ERANGE;

        *ret = (uint16_t) u;
        واپس 0;
}

صحیح محفوظ_atoi16(ثابت حرف *s, int16_t *ret) {
        غیرعلامتی base = 0;
        حرف *x = تہی;
        طویل l;

        أكد(s);

        s += strspn(s, WHITESPACE);
        s = mangle_بنیاد(s, &base);

        errno = 0;
        l = strtol(s, &x, base);
        اگر (errno > 0)
                واپس -errno;
        اگر (!x || x == s || *x != 0)
                واپس -EINVAL;
        اگر ((طویل) (int16_t) l != l)
                واپس -ERANGE;

        اگر (ret)
                *ret = (int16_t) l;

        واپس 0;
}

صحیح محفوظ_atod(ثابت حرف *s, دوہرا *ret_d) {
        _cleanup_(freelocalep) locale_t loc = (locale_t) 0;
        حرف *x = تہی;
        دوہرا d = 0;

        أكد(s);

        loc = newlocale(LC_NUMERIC_MASK, "C", (locale_t) 0);
        اگر (loc == (locale_t) 0)
                واپس -errno;

        errno = 0;
        d = strtod_l(s, &x, loc);
        اگر (errno > 0)
                واپس -errno;
        اگر (!x || x == s || *x != 0)
                واپس -EINVAL;

        اگر (ret_d)
                *ret_d = d;

        واپس 0;
}

صحیح parse_fractional_part_u(ثابت حرف **p, size_t digits, غیرعلامتی *res) {
        غیرعلامتی val = 0;
        ثابت حرف *s;

        أكد(p);
        أكد(res);

        s = *p;

        /* accept any number of digits, strtoull is limited to 19 */
        برائے (size_t i = 0; i < digits; i++, s++) {
                اگر (!ascii_isdigit(*s)) {
                        اگر (i == 0)
                                واپس -EINVAL;

                        /* too few digits, pad with 0 */
                        برائے (; i < digits; i++)
                                val *= 10;

                        توڑو;
                }

                val *= 10;
                val += *s - '0';
        }

        /* maybe round up */
        اگر (*s >= '5' && *s <= '9')
                val++;

        s += strspn(s, DIGITS);

        *p = s;
        *res = val;

        واپس 0;
}

صحیح parse_nice(ثابت حرف *s, صحیح *ret) {
        صحیح n, r;

        أكد(ret);

        r = محفوظ_atoi(s, &n);
        اگر (r < 0)
                واپس r;

        اگر (!nice_is_valid(n))
                واپس -ERANGE;

        *ret = n;
        واپس 0;
}

صحیح parse_ip_پورٹ(ثابت حرف *s, uint16_t *ret) {
        uint16_t l;
        صحیح r;

        أكد(ret);

        r = محفوظ_atou16_بھرا(s, محفوظ_ATO_REFUSE_LEADING_WHITESPACE, &l);
        اگر (r < 0)
                واپس r;

        اگر (l == 0)
                واپس -EINVAL;

        *ret = l;

        واپس 0;
}

صحیح parse_ip_پورٹ_range(ثابت حرف *s, uint16_t *low, uint16_t *high, bool allow_zero) {
        غیرعلامتی l, h;
        صحیح r;

        أكد(low);
        أكد(high);

        r = parse_range(s, &l, &h);
        اگر (r < 0)
                واپس r;

        اگر (l > 65535 || h > 65535)
                واپس -EINVAL;

        اگر (!allow_zero && (l == 0 || h == 0))
                واپس -EINVAL;

        اگر (h < l)
                واپس -EINVAL;

        *low = l;
        *high = h;

        واپس 0;
}

صحیح parse_oom_score_adjust(ثابت حرف *s, صحیح *ret) {
        صحیح r, v;

        أكد(s);
        أكد(ret);

        r = محفوظ_atoi(s, &v);
        اگر (r < 0)
                واپس r;

        اگر (!oom_score_adjust_is_valid(v))
                واپس -ERANGE;

        *ret = v;
        واپس 0;
}

صحیح store_loadavg_fixed_نقطہ(غیرعلامتی طویل i, غیرعلامتی طویل f, loadavg_t *ret) {
        أكد(ret);

        /* Validate both parts *before* shifting them, so that the shifts themselves cannot overflow.
         * Rejecting f >= 100 is equivalent to the old check of the shifted value against
         * LOADAVG_FIXED_POINT_1_0, since DIV_ROUND_UP(99 << 11, 100) == 2028 and
         * DIV_ROUND_UP(100 << 11, 100) == 2048. */
        اگر (i > (~0UL >> LOADAVG_PRECISION_بٹ))
                واپس -ERANGE;

        اگر (f >= 100)
                واپس -ERANGE;

        *ret = (i << LOADAVG_PRECISION_بٹ) | DIV_باری_اوپر(f << LOADAVG_PRECISION_بٹ, 100);
        واپس 0;
}

صحیح parse_loadavg_fixed_نقطہ(ثابت حرف *s, loadavg_t *ret) {
        ثابت حرف *d, *f_str, *i_str;
        غیرعلامتی طویل i, f;
        صحیح r;

        أكد(s);
        أكد(ret);

        d = strchr(s, '.');
        اگر (!d)
                واپس -EINVAL;

        i_str = strndupa_محفوظ(s, d - s);
        f_str = d + 1;

        r = محفوظ_atolu_بھرا(i_str, 10, &i);
        اگر (r < 0)
                واپس r;

        r = محفوظ_atolu_بھرا(f_str, 10, &f);
        اگر (r < 0)
                واپس r;

        واپس store_loadavg_fixed_نقطہ(i, f, ret);
}

/* Limitations are described in https://www.netfilter.org/projects/nftables/manpage.html and
 * https://bugzilla.netfilter.org/show_bug.cgi?id=1175 */
bool nft_identifier_valid(ثابت حرف *id) {
        اگر (isempty(id))
                واپس false;

        اگر (لڑی_لمبائی(id) >= NFT_NAME_MAXLEN)
                واپس false;

        اگر (!ascii_isalpha(id[0]))
                واپس false;

        واپس اندر_charset(id + 1, ALPHANUMERICAL "/\\_.");
}

صحیح parse_capability_سیٹ(ثابت حرف *s, uint64_t initial, uint64_t *current) {
        صحیح r;

        أكد(s);
        أكد(current);

        اگر (isempty(s)) {
                *current = CAP_نقاب_UNSET;
                واپس 1;
        }

        bool invert = false;
        اگر (s[0] == '~') {
                invert = true;
                s++;
        }

        uint64_t parsed;
        r = capability_سیٹ_from_string(s, &parsed);
        اگر (r < 0)
                واپس r;

        اگر (parsed == 0 || *current == initial)
                /* "~" or uninitialized data -> replace */
                *current = invert ? all_capabilities() & ~parsed : parsed;
        ورنہ {
                /* previous data -> merge */
                اگر (invert)
                        *current &= ~parsed;
                ورنہ
                        *current |= parsed;
        }

        واپس r;
}
