/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ <linux/ipv6.h>
#అన్తర్భావ <linux/netfilter/nf_tables.h>
#అన్తర్భావ <stdio.h>
#అన్తర్భావ <stdlib.h>
#అన్తర్భావ <sys/socket.h>

#అన్తర్భావ "alloc-util.h"
#అన్తర్భావ "capability-list.h"
#అన్తర్భావ "capability-util.h"
#అన్తర్భావ "errno-list.h"
#అన్తర్భావ "extract-word.h"
#అన్తర్భావ "locale-util.h"
#అన్తర్భావ "log.h"
#అన్తర్భావ "missing-network.h"
#అన్తర్భావ "parse-util.h"
#అన్తర్భావ "path-util.h"
#అన్తర్భావ "process-util.h"
#అన్తర్భావ "string-util.h"
#అన్తర్భావ "strv.h"

పూర్ణాంకం parse_boolean(స్థిరరాశి అక్షరం *v) {
        ఒకవేళ (!v)
                తిరిగి -EINVAL;

        ఒకవేళ (STRCASE_లోపల_సమితి(v,
                           "1",
                           "yes",
                           "y",
                           "true",
                           "t",
                           "on"))
                తిరిగి 1;

        ఒకవేళ (STRCASE_లోపల_సమితి(v,
                           "0",
                           "no",
                           "n",
                           "false",
                           "f",
                           "off"))
                తిరిగి 0;

        తిరిగి -EINVAL;
}

పూర్ణాంకం parse_tristate_నిండిన(స్థిరరాశి అక్షరం *v, స్థిరరాశి అక్షరం *మూడవ, పూర్ణాంకం *ret) {
        పూర్ణాంకం r;

        ఒకవేళ (isempty(v) || streq_ptr(v, మూడవ)) { /* Empty string is always taken as the third/invalid/auto state */
                ఒకవేళ (ret)
                        *ret = -1;
        } లేకపోతే {
                r = parse_boolean(v);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                ఒకవేళ (ret)
                        *ret = r;
        }

        తిరిగి 0;
}

పూర్ణాంకం parse_pid(స్థిరరాశి అక్షరం *s, pid_t *ret) {
        గుర్తులేని దీర్ఘ ul = 0;
        pid_t pid;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(s);

        r = సురక్షిత_atolu(s, &ul);
        ఒకవేళ (r < 0)
                తిరిగి r;

        pid = (pid_t) ul;

        ఒకవేళ ((గుర్తులేని దీర్ఘ) pid != ul)
                తిరిగి -ERANGE;

        ఒకవేళ (!pid_is_valid(pid))
                తిరిగి -ERANGE;

        ఒకవేళ (ret)
                *ret = pid;
        తిరిగి 0;
}

పూర్ణాంకం parse_బహుళకం(స్థిరరాశి అక్షరం *s, mode_t *ret) {
        గుర్తులేని m;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(s);

        r = సురక్షిత_atou_నిండిన(s, 8 |
                           సురక్షిత_ATO_REFUSE_PLUS_MINUS, /* Leading '+' or even '-' char? that's just weird,
                                                        * refuse. User might have wanted to add mode flags or
                                                        * so, but this parser doesn't allow that, so let's
                                                        * better be safe. */
                           &m);
        ఒకవేళ (r < 0)
                తిరిగి r;
        ఒకవేళ (m > 07777)
                తిరిగి -ERANGE;

        ఒకవేళ (ret)
                *ret = m;
        తిరిగి 0;
}

పూర్ణాంకం parse_ifindex(స్థిరరాశి అక్షరం *s) {
        పూర్ణాంకం ifi, r;

        ప్రతిజ్ఞా(s);

        r = సురక్షిత_atoi(s, &ifi);
        ఒకవేళ (r < 0)
                తిరిగి r;
        ఒకవేళ (ifi <= 0)
                తిరిగి -EINVAL;

        తిరిగి ifi;
}

పూర్ణాంకం parse_mtu(పూర్ణాంకం family, స్థిరరాశి అక్షరం *s, uint32_t *ret) {
        uint64_t u, m;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(ret);

        r = parse_కొలత(s, 1024, &u);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (u > UINT32_MAX)
                తిరిగి -ERANGE;

        ఎంపిక (family) {
        స్థితి AF_INET:
                m = IPV4_MIN_MTU; /* This is 68 */
                ఆపు;
        స్థితి AF_INET6:
                m = IPV6_MIN_MTU; /* This is 1280 */
                ఆపు;
        అప్రమేయ:
                m = 0;
        }

        ఒకవేళ (u < m)
                తిరిగి -ERANGE;

        *ret = (uint32_t) u;
        తిరిగి 0;
}

పూర్ణాంకం parse_కొలత(స్థిరరాశి అక్షరం *t, uint64_t base, uint64_t *కొలత) {

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

        నిర్మాణం పట్టిక {
                స్థిరరాశి అక్షరం *suffix;
                గుర్తులేని దీర్ఘ దీర్ఘ గుణకం;
        };

        స్థిర స్థిరరాశి నిర్మాణం పట్టిక iec[] = {
                { "E", 1024ULL*1024ULL*1024ULL*1024ULL*1024ULL*1024ULL },
                { "P", 1024ULL*1024ULL*1024ULL*1024ULL*1024ULL },
                { "T", 1024ULL*1024ULL*1024ULL*1024ULL },
                { "G", 1024ULL*1024ULL*1024ULL },
                { "M", 1024ULL*1024ULL },
                { "K", 1024ULL },
                { "B", 1ULL },
                { "",  1ULL },
        };

        స్థిర స్థిరరాశి నిర్మాణం పట్టిక si[] = {
                { "E", 1000ULL*1000ULL*1000ULL*1000ULL*1000ULL*1000ULL },
                { "P", 1000ULL*1000ULL*1000ULL*1000ULL*1000ULL },
                { "T", 1000ULL*1000ULL*1000ULL*1000ULL },
                { "G", 1000ULL*1000ULL*1000ULL },
                { "M", 1000ULL*1000ULL },
                { "K", 1000ULL },
                { "B", 1ULL },
                { "",  1ULL },
        };

        స్థిరరాశి నిర్మాణం పట్టిక *పట్టిక;
        స్థిరరాశి అక్షరం *p;
        గుర్తులేని దీర్ఘ దీర్ఘ r = 0;
        గుర్తులేని n_entries, ప్రారంభం_pos = 0;

        ప్రతిజ్ఞా(t);
        ప్రతిజ్ఞా(లోపల_సమితి(base, 1000, 1024));
        ప్రతిజ్ఞా(కొలత);

        ఒకవేళ (base == 1000) {
                పట్టిక = si;
                n_entries = ELEMENTSOF(si);
        } లేకపోతే {
                పట్టిక = iec;
                n_entries = ELEMENTSOF(iec);
        }

        p = t;
        చేయి {
                గుర్తులేని దీర్ఘ దీర్ఘ l, tmp;
                ద్విగుణ frac = 0;
                అక్షరం *e;
                గుర్తులేని i;

                p += strspn(p, WHITESPACE);

                errno = 0;
                l = strtoull(p, &e, 10);
                ఒకవేళ (errno > 0)
                        తిరిగి -errno;
                ఒకవేళ (e == p)
                        తిరిగి -EINVAL;
                ఒకవేళ (*p == '-')
                        తిరిగి -ERANGE;

                ఒకవేళ (*e == '.') {
                        e++;

                        /* strtoull() itself would accept space/+/- */
                        ఒకవేళ (ascii_isdigit(*e)) {
                                గుర్తులేని దీర్ఘ దీర్ఘ l2;
                                అక్షరం *e2;

                                l2 = strtoull(e, &e2, 10);
                                ఒకవేళ (errno > 0)
                                        తిరిగి -errno;

                                /* Ignore failure. E.g. 10.M is valid */
                                frac = l2;
                                కొరకు (; e < e2; e++)
                                        frac /= 10;
                        }
                }

                e += strspn(e, WHITESPACE);

                కొరకు (i = ప్రారంభం_pos; i < n_entries; i++)
                        ఒకవేళ (startswith(e, పట్టిక[i].suffix))
                                ఆపు;

                ఒకవేళ (i >= n_entries)
                        తిరిగి -EINVAL;

                ఒకవేళ (l + (frac > 0) > ULLONG_MAX / పట్టిక[i].గుణకం)
                        తిరిగి -ERANGE;

                tmp = l * పట్టిక[i].గుణకం + (గుర్తులేని దీర్ఘ దీర్ఘ) (frac * పట్టిక[i].గుణకం);
                ఒకవేళ (tmp > ULLONG_MAX - r)
                        తిరిగి -ERANGE;

                r += tmp;
                ఒకవేళ ((గుర్తులేని దీర్ఘ దీర్ఘ) (uint64_t) r != r)
                        తిరిగి -ERANGE;

                p = e + అక్షరశ్రేణి_పొడవు(పట్టిక[i].suffix);

                ప్రారంభం_pos = i + 1;

        } అయితే (*p);

        *కొలత = r;

        తిరిగి 0;
}

పూర్ణాంకం parse_sector_కొలత(స్థిరరాశి అక్షరం *t, uint64_t *ret) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(t);
        ప్రతిజ్ఞా(ret);

        uint64_t ss;

        r = సురక్షిత_atou64(t, &ss);
        ఒకవేళ (r < 0)
                తిరిగి log_error_errno(r, "Failed to parse sector size parameter %s", t);
        ఒకవేళ (ss < 512 || ss > 4096) /* Allow up to 4K due to dm-crypt support and 4K alignment by the homed LUKS backend */
                తిరిగి log_error_errno(SYNTHETIC_ERRNO(ERANGE), "Sector size not between 512 and 4096: %s", t);
        ఒకవేళ (!ISPOWEROF2(ss))
                తిరిగి log_error_errno(SYNTHETIC_ERRNO(EINVAL), "Sector size not power of 2: %s", t);

        *ret = ss;
        తిరిగి 0;
}

పూర్ణాంకం parse_range(స్థిరరాశి అక్షరం *t, గుర్తులేని *క్రింది, గుర్తులేని *పై) {
        _cleanup_free_ అక్షరం *మాట = ఖాళీ;
        గుర్తులేని l, u;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(క్రింది);
        ప్రతిజ్ఞా(పై);

        /* Extract the lower bound. */
        r = వెలికితీయి_మొదటి_మాట(&t, &మాట, "-", వెలికితీయి_DONT_COALESCE_SEPARATORS);
        ఒకవేళ (r < 0)
                తిరిగి r;
        ఒకవేళ (r == 0)
                తిరిగి -EINVAL;

        r = సురక్షిత_atou(మాట, &l);
        ఒకవేళ (r < 0)
                తిరిగి r;

        /* Check for the upper bound and extract it if needed */
        ఒకవేళ (!t)
                /* Single number with no dashes. */
                u = l;
        లేకపోతే ఒకవేళ (!*t)
                /* Trailing dash is an error. */
                తిరిగి -EINVAL;
        లేకపోతే {
                r = సురక్షిత_atou(t, &u);
                ఒకవేళ (r < 0)
                        తిరిగి r;
        }

        *క్రింది = l;
        *పై = u;
        తిరిగి 0;
}

పూర్ణాంకం parse_errno(స్థిరరాశి అక్షరం *t) {
        పూర్ణాంకం r, e;

        ప్రతిజ్ఞా(t);

        r = errno_from_name(t);
        ఒకవేళ (r > 0)
                తిరిగి r;

        r = సురక్షిత_atoi(t, &e);
        ఒకవేళ (r < 0)
                తిరిగి r;

        /* 0 is also allowed here */
        ఒకవేళ (!errno_is_valid(e) && e != 0)
                తిరిగి -ERANGE;

        తిరిగి e;
}

పూర్ణాంకం parse_fd(స్థిరరాశి అక్షరం *t) {
        పూర్ణాంకం r, fd;

        ప్రతిజ్ఞా(t);

        r = సురక్షిత_atoi(t, &fd);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (fd < 0)
                తిరిగి -EBADF;

        తిరిగి fd;
}

పూర్ణాంకం parse_user_shell(స్థిరరాశి అక్షరం *s, అక్షరం **ret_sh, bool *ret_copy) {
        అక్షరం *sh;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(ret_sh);
        ప్రతిజ్ఞా(ret_copy);

        ఒకవేళ (మార్గం_is_absolute(s) && మార్గం_is_normalized(s)) {
                sh = strdup(s);
                ఒకవేళ (!sh)
                        తిరిగి -ENOMEM;

                *ret_sh = sh;
                *ret_copy = false;
        } లేకపోతే {
                r = parse_boolean(s);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                *ret_sh = ఖాళీ;
                *ret_copy = r;
        }

        తిరిగి 0;
}

స్థిర స్థిరరాశి అక్షరం *mangle_పునాది(స్థిరరాశి అక్షరం *s, గుర్తులేని *base) {
        స్థిరరాశి అక్షరం *k;

        ప్రతిజ్ఞా(s);
        ప్రతిజ్ఞా(base);

        /* Base already explicitly specified, then don't do anything. */
        ఒకవేళ (సురక్షిత_ATO_ముసుగు_FLAGS(*base) != 0)
                తిరిగి s;

        /* Support Python 3 style "0b" and 0x" prefixes, because they truly make sense, much more than C's "0" prefix for octal. */
        k = STARTSWITH_సమితి(s, "0b", "0B");
        ఒకవేళ (k) {
                *base = 2 | (*base & సురక్షిత_ATO_ALL_FLAGS);
                తిరిగి k;
        }

        k = STARTSWITH_సమితి(s, "0o", "0O");
        ఒకవేళ (k) {
                *base = 8 | (*base & సురక్షిత_ATO_ALL_FLAGS);
                తిరిగి k;
        }

        తిరిగి s;
}

పూర్ణాంకం సురక్షిత_atou_నిండిన(స్థిరరాశి అక్షరం *s, గుర్తులేని base, గుర్తులేని *ret_u) {
        అక్షరం *x = ఖాళీ;
        గుర్తులేని దీర్ఘ l;

        ప్రతిజ్ఞా(s);
        ప్రతిజ్ఞా(సురక్షిత_ATO_ముసుగు_FLAGS(base) <= 16);

        /* strtoul() is happy to parse negative values, and silently converts them to unsigned values without
         * generating an error. We want a clean error, hence let's look for the "-" prefix on our own, and
         * generate an error. But let's do so only after strtoul() validated that the string is clean
         * otherwise, so that we return EINVAL preferably over ERANGE. */

        ఒకవేళ (FLAGS_సమితి(base, సురక్షిత_ATO_REFUSE_LEADING_WHITESPACE) &&
            strchr(WHITESPACE, s[0]))
                తిరిగి -EINVAL;

        s += strspn(s, WHITESPACE);

        ఒకవేళ (FLAGS_సమితి(base, సురక్షిత_ATO_REFUSE_PLUS_MINUS) &&
            లోపల_సమితి(s[0], '+', '-'))
                తిరిగి -EINVAL; /* Note that we check the "-" prefix again a second time below, but return a
                                 * different error. I.e. if the SAFE_ATO_REFUSE_PLUS_MINUS flag is set we
                                 * blanket refuse +/- prefixed integers, while if it is missing we'll just
                                 * return ERANGE, because the string actually parses correctly, but doesn't
                                 * fit in the return type. */

        ఒకవేళ (FLAGS_సమితి(base, సురక్షిత_ATO_REFUSE_LEADING_ZERO) &&
            s[0] == '0' && !streq(s, "0"))
                తిరిగి -EINVAL; /* This is particularly useful to avoid ambiguities between C's octal
                                 * notation and assumed-to-be-decimal integers with a leading zero. */

        s = mangle_పునాది(s, &base);

        errno = 0;
        l = strtoul(s, &x, సురక్షిత_ATO_ముసుగు_FLAGS(base) /* Let's mask off the flags bits so that only the actual
                                                      * base is left */);
        ఒకవేళ (errno > 0)
                తిరిగి -errno;
        ఒకవేళ (!x || x == s || *x != 0)
                తిరిగి -EINVAL;
        ఒకవేళ (l != 0 && s[0] == '-')
                తిరిగి -ERANGE;
        ఒకవేళ ((గుర్తులేని దీర్ఘ) (గుర్తులేని) l != l)
                తిరిగి -ERANGE;

        ఒకవేళ (ret_u)
                *ret_u = (గుర్తులేని) l;

        తిరిగి 0;
}

పూర్ణాంకం సురక్షిత_atou_bounded(స్థిరరాశి అక్షరం *s, గుర్తులేని min, గుర్తులేని max, గుర్తులేని *ret) {
        గుర్తులేని v;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(ret);

        r = సురక్షిత_atou(s, &v);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (v < min || v > max)
                తిరిగి -ERANGE;

        *ret = v;
        తిరిగి 0;
}

పూర్ణాంకం సురక్షిత_atoi(స్థిరరాశి అక్షరం *s, పూర్ణాంకం *ret_i) {
        గుర్తులేని base = 0;
        అక్షరం *x = ఖాళీ;
        దీర్ఘ l;

        ప్రతిజ్ఞా(s);

        s += strspn(s, WHITESPACE);
        s = mangle_పునాది(s, &base);

        errno = 0;
        l = strtol(s, &x, base);
        ఒకవేళ (errno > 0)
                తిరిగి -errno;
        ఒకవేళ (!x || x == s || *x != 0)
                తిరిగి -EINVAL;
        ఒకవేళ ((దీర్ఘ) (పూర్ణాంకం) l != l)
                తిరిగి -ERANGE;

        ఒకవేళ (ret_i)
                *ret_i = (పూర్ణాంకం) l;

        తిరిగి 0;
}

పూర్ణాంకం సురక్షిత_atollu_నిండిన(స్థిరరాశి అక్షరం *s, గుర్తులేని base, గుర్తులేని దీర్ఘ దీర్ఘ *ret_llu) {
        అక్షరం *x = ఖాళీ;
        గుర్తులేని దీర్ఘ దీర్ఘ l;

        ప్రతిజ్ఞా(s);
        ప్రతిజ్ఞా(సురక్షిత_ATO_ముసుగు_FLAGS(base) <= 16);

        ఒకవేళ (FLAGS_సమితి(base, సురక్షిత_ATO_REFUSE_LEADING_WHITESPACE) &&
            strchr(WHITESPACE, s[0]))
                తిరిగి -EINVAL;

        s += strspn(s, WHITESPACE);

        ఒకవేళ (FLAGS_సమితి(base, సురక్షిత_ATO_REFUSE_PLUS_MINUS) &&
            లోపల_సమితి(s[0], '+', '-'))
                తిరిగి -EINVAL;

        ఒకవేళ (FLAGS_సమితి(base, సురక్షిత_ATO_REFUSE_LEADING_ZERO) &&
            s[0] == '0' && s[1] != 0)
                తిరిగి -EINVAL;

        s = mangle_పునాది(s, &base);

        errno = 0;
        l = strtoull(s, &x, సురక్షిత_ATO_ముసుగు_FLAGS(base));
        ఒకవేళ (errno > 0)
                తిరిగి -errno;
        ఒకవేళ (!x || x == s || *x != 0)
                తిరిగి -EINVAL;
        ఒకవేళ (l != 0 && s[0] == '-')
                తిరిగి -ERANGE;

        ఒకవేళ (ret_llu)
                *ret_llu = l;

        తిరిగి 0;
}

పూర్ణాంకం సురక్షిత_atolli(స్థిరరాశి అక్షరం *s, దీర్ఘ దీర్ఘ *ret_lli) {
        గుర్తులేని base = 0;
        అక్షరం *x = ఖాళీ;
        దీర్ఘ దీర్ఘ l;

        ప్రతిజ్ఞా(s);

        s += strspn(s, WHITESPACE);
        s = mangle_పునాది(s, &base);

        errno = 0;
        l = strtoll(s, &x, base);
        ఒకవేళ (errno > 0)
                తిరిగి -errno;
        ఒకవేళ (!x || x == s || *x != 0)
                తిరిగి -EINVAL;

        ఒకవేళ (ret_lli)
                *ret_lli = l;

        తిరిగి 0;
}

పూర్ణాంకం సురక్షిత_atou8_నిండిన(స్థిరరాశి అక్షరం *s, గుర్తులేని base, uint8_t *ret) {
        గుర్తులేని u;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(ret);

        r = సురక్షిత_atou_నిండిన(s, base, &u);
        ఒకవేళ (r < 0)
                తిరిగి r;
        ఒకవేళ (u > UINT8_MAX)
                తిరిగి -ERANGE;

        *ret = (uint8_t) u;
        తిరిగి 0;
}

పూర్ణాంకం సురక్షిత_atou16_నిండిన(స్థిరరాశి అక్షరం *s, గుర్తులేని base, uint16_t *ret) {
        గుర్తులేని u;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(ret);

        r = సురక్షిత_atou_నిండిన(s, base, &u);
        ఒకవేళ (r < 0)
                తిరిగి r;
        ఒకవేళ (u > UINT16_MAX)
                తిరిగి -ERANGE;

        *ret = (uint16_t) u;
        తిరిగి 0;
}

పూర్ణాంకం సురక్షిత_atoi16(స్థిరరాశి అక్షరం *s, int16_t *ret) {
        గుర్తులేని base = 0;
        అక్షరం *x = ఖాళీ;
        దీర్ఘ l;

        ప్రతిజ్ఞా(s);

        s += strspn(s, WHITESPACE);
        s = mangle_పునాది(s, &base);

        errno = 0;
        l = strtol(s, &x, base);
        ఒకవేళ (errno > 0)
                తిరిగి -errno;
        ఒకవేళ (!x || x == s || *x != 0)
                తిరిగి -EINVAL;
        ఒకవేళ ((దీర్ఘ) (int16_t) l != l)
                తిరిగి -ERANGE;

        ఒకవేళ (ret)
                *ret = (int16_t) l;

        తిరిగి 0;
}

పూర్ణాంకం సురక్షిత_atod(స్థిరరాశి అక్షరం *s, ద్విగుణ *ret_d) {
        _cleanup_(freelocalep) locale_t loc = (locale_t) 0;
        అక్షరం *x = ఖాళీ;
        ద్విగుణ d = 0;

        ప్రతిజ్ఞా(s);

        loc = newlocale(LC_NUMERIC_MASK, "C", (locale_t) 0);
        ఒకవేళ (loc == (locale_t) 0)
                తిరిగి -errno;

        errno = 0;
        d = strtod_l(s, &x, loc);
        ఒకవేళ (errno > 0)
                తిరిగి -errno;
        ఒకవేళ (!x || x == s || *x != 0)
                తిరిగి -EINVAL;

        ఒకవేళ (ret_d)
                *ret_d = d;

        తిరిగి 0;
}

పూర్ణాంకం parse_fractional_part_u(స్థిరరాశి అక్షరం **p, size_t digits, గుర్తులేని *res) {
        గుర్తులేని val = 0;
        స్థిరరాశి అక్షరం *s;

        ప్రతిజ్ఞా(p);
        ప్రతిజ్ఞా(res);

        s = *p;

        /* accept any number of digits, strtoull is limited to 19 */
        కొరకు (size_t i = 0; i < digits; i++, s++) {
                ఒకవేళ (!ascii_isdigit(*s)) {
                        ఒకవేళ (i == 0)
                                తిరిగి -EINVAL;

                        /* too few digits, pad with 0 */
                        కొరకు (; i < digits; i++)
                                val *= 10;

                        ఆపు;
                }

                val *= 10;
                val += *s - '0';
        }

        /* maybe round up */
        ఒకవేళ (*s >= '5' && *s <= '9')
                val++;

        s += strspn(s, DIGITS);

        *p = s;
        *res = val;

        తిరిగి 0;
}

పూర్ణాంకం parse_nice(స్థిరరాశి అక్షరం *s, పూర్ణాంకం *ret) {
        పూర్ణాంకం n, r;

        ప్రతిజ్ఞా(ret);

        r = సురక్షిత_atoi(s, &n);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (!nice_is_valid(n))
                తిరిగి -ERANGE;

        *ret = n;
        తిరిగి 0;
}

పూర్ణాంకం parse_ip_పోర్టు(స్థిరరాశి అక్షరం *s, uint16_t *ret) {
        uint16_t l;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(ret);

        r = సురక్షిత_atou16_నిండిన(s, సురక్షిత_ATO_REFUSE_LEADING_WHITESPACE, &l);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (l == 0)
                తిరిగి -EINVAL;

        *ret = l;

        తిరిగి 0;
}

పూర్ణాంకం parse_ip_పోర్టు_range(స్థిరరాశి అక్షరం *s, uint16_t *low, uint16_t *high, bool allow_zero) {
        గుర్తులేని l, h;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(low);
        ప్రతిజ్ఞా(high);

        r = parse_range(s, &l, &h);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (l > 65535 || h > 65535)
                తిరిగి -EINVAL;

        ఒకవేళ (!allow_zero && (l == 0 || h == 0))
                తిరిగి -EINVAL;

        ఒకవేళ (h < l)
                తిరిగి -EINVAL;

        *low = l;
        *high = h;

        తిరిగి 0;
}

పూర్ణాంకం parse_oom_score_adjust(స్థిరరాశి అక్షరం *s, పూర్ణాంకం *ret) {
        పూర్ణాంకం r, v;

        ప్రతిజ్ఞా(s);
        ప్రతిజ్ఞా(ret);

        r = సురక్షిత_atoi(s, &v);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (!oom_score_adjust_is_valid(v))
                తిరిగి -ERANGE;

        *ret = v;
        తిరిగి 0;
}

పూర్ణాంకం store_loadavg_fixed_బిందువు(గుర్తులేని దీర్ఘ i, గుర్తులేని దీర్ఘ f, loadavg_t *ret) {
        ప్రతిజ్ఞా(ret);

        /* Validate both parts *before* shifting them, so that the shifts themselves cannot overflow.
         * Rejecting f >= 100 is equivalent to the old check of the shifted value against
         * LOADAVG_FIXED_POINT_1_0, since DIV_ROUND_UP(99 << 11, 100) == 2028 and
         * DIV_ROUND_UP(100 << 11, 100) == 2048. */
        ఒకవేళ (i > (~0UL >> LOADAVG_PRECISION_బిట్లు))
                తిరిగి -ERANGE;

        ఒకవేళ (f >= 100)
                తిరిగి -ERANGE;

        *ret = (i << LOADAVG_PRECISION_బిట్లు) | DIV_విడత_పైకి(f << LOADAVG_PRECISION_బిట్లు, 100);
        తిరిగి 0;
}

పూర్ణాంకం parse_loadavg_fixed_బిందువు(స్థిరరాశి అక్షరం *s, loadavg_t *ret) {
        స్థిరరాశి అక్షరం *d, *f_str, *i_str;
        గుర్తులేని దీర్ఘ i, f;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(s);
        ప్రతిజ్ఞా(ret);

        d = strchr(s, '.');
        ఒకవేళ (!d)
                తిరిగి -EINVAL;

        i_str = strndupa_సురక్షిత(s, d - s);
        f_str = d + 1;

        r = సురక్షిత_atolu_నిండిన(i_str, 10, &i);
        ఒకవేళ (r < 0)
                తిరిగి r;

        r = సురక్షిత_atolu_నిండిన(f_str, 10, &f);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి store_loadavg_fixed_బిందువు(i, f, ret);
}

/* Limitations are described in https://www.netfilter.org/projects/nftables/manpage.html and
 * https://bugzilla.netfilter.org/show_bug.cgi?id=1175 */
bool nft_identifier_valid(స్థిరరాశి అక్షరం *id) {
        ఒకవేళ (isempty(id))
                తిరిగి false;

        ఒకవేళ (అక్షరశ్రేణి_పొడవు(id) >= NFT_NAME_MAXLEN)
                తిరిగి false;

        ఒకవేళ (!ascii_isalpha(id[0]))
                తిరిగి false;

        తిరిగి లోపల_charset(id + 1, ALPHANUMERICAL "/\\_.");
}

పూర్ణాంకం parse_capability_సమితి(స్థిరరాశి అక్షరం *s, uint64_t initial, uint64_t *current) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(s);
        ప్రతిజ్ఞా(current);

        ఒకవేళ (isempty(s)) {
                *current = CAP_ముసుగు_UNSET;
                తిరిగి 1;
        }

        bool invert = false;
        ఒకవేళ (s[0] == '~') {
                invert = true;
                s++;
        }

        uint64_t parsed;
        r = capability_సమితి_from_string(s, &parsed);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (parsed == 0 || *current == initial)
                /* "~" or uninitialized data -> replace */
                *current = invert ? all_capabilities() & ~parsed : parsed;
        లేకపోతే {
                /* previous data -> merge */
                ఒకవేళ (invert)
                        *current &= ~parsed;
                లేకపోతే
                        *current |= parsed;
        }

        తిరిగి r;
}
