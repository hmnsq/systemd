/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ "alloc-util.h"
#അന്തര്ഭാവ "errno-util.h"
#അന്തര്ഭാവ "extract-word.h"
#അന്തര്ഭാവ "fd-util.h"
#അന്തര്ഭാവ "fileio.h"
#അന്തര്ഭാവ "format-util.h"
#അന്തര്ഭാവ "log.h"
#അന്തര്ഭാവ "parse-util.h"
#അന്തര്ഭാവ "process-util.h"
#അന്തര്ഭാവ "rlimit-util.h"
#അന്തര്ഭാവ "string-table.h"
#അന്തര്ഭാവ "strv.h"
#അന്തര്ഭാവ "time-util.h"

പൂർണ്ണസംഖ്യ setrlimit_closest(പൂർണ്ണസംഖ്യ resource, സ്ഥിരം ഘടന rlimit *rlim) {
        ഘടന rlimit highest, fixed;

        പ്രതിജ്ഞാ(rlim);

        എങ്കിൽ (setrlimit(resource, rlim) >= 0)
                തിരികെ 0;

        എങ്കിൽ (errno != EPERM)
                തിരികെ -errno;

        /* So we failed to set the desired setrlimit, then let's try
         * to get as close as we can */
        എങ്കിൽ (getrlimit(resource, &highest) < 0)
                തിരികെ -errno;

        /* If the hard limit is unbounded anyway, then the EPERM had other reasons, let's propagate the original EPERM
         * then */
        എങ്കിൽ (highest.rlim_max == RLIM_INFINITY)
                തിരികെ -EPERM;

        fixed = (ഘടന rlimit) {
                .rlim_cur = MIN(rlim->rlim_cur, highest.rlim_max),
                .rlim_max = MIN(rlim->rlim_max, highest.rlim_max),
        };

        /* Shortcut things if we wouldn't change anything. */
        എങ്കിൽ (fixed.rlim_cur == highest.rlim_cur &&
            fixed.rlim_max == highest.rlim_max)
                തിരികെ 0;

        log_debug("Failed at setting rlimit " RLIM_FMT " for resource RLIMIT_%s. Will attempt setting value " RLIM_FMT " instead.", rlim->rlim_max, rlimit_to_string(resource), fixed.rlim_max);

        തിരികെ RET_NERRNO(setrlimit(resource, &fixed));
}

പൂർണ്ണസംഖ്യ setrlimit_closest_all(സ്ഥിരം ഘടന rlimit *സ്ഥിരം *rlim, പൂർണ്ണസംഖ്യ *which_failed) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(rlim);

        /* On failure returns the limit's index that failed in *which_failed, but only if non-NULL */

        വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < _RLIMIT_MAX; i++) {
                എങ്കിൽ (!rlim[i])
                        തുടരുക;

                r = setrlimit_closest(i, rlim[i]);
                എങ്കിൽ (r < 0) {
                        എങ്കിൽ (which_failed)
                                *which_failed = i;

                        തിരികെ r;
                }
        }

        എങ്കിൽ (which_failed)
                *which_failed = -1;

        തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ rlimit_parse_u64(സ്ഥിരം അക്ഷരം *val, rlim_t *ret) {
        uint64_t u;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(val);
        പ്രതിജ്ഞാ(ret);

        എങ്കിൽ (streq(val, "infinity")) {
                *ret = RLIM_INFINITY;
                തിരികെ 0;
        }

        /* setrlimit(2) suggests rlim_t is always 64-bit on Linux. */
        assert_cc(വലുപ്പം(rlim_t) == വലുപ്പം(uint64_t));

        r = safe_atou64(val, &u);
        എങ്കിൽ (r < 0)
                തിരികെ r;
        എങ്കിൽ (u >= (uint64_t) RLIM_INFINITY)
                തിരികെ -ERANGE;

        *ret = (rlim_t) u;
        തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ rlimit_parse_size(സ്ഥിരം അക്ഷരം *val, rlim_t *ret) {
        uint64_t u;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(val);
        പ്രതിജ്ഞാ(ret);

        എങ്കിൽ (streq(val, "infinity")) {
                *ret = RLIM_INFINITY;
                തിരികെ 0;
        }

        r = parse_size(val, 1024, &u);
        എങ്കിൽ (r < 0)
                തിരികെ r;
        എങ്കിൽ (u >= (uint64_t) RLIM_INFINITY)
                തിരികെ -ERANGE;

        *ret = (rlim_t) u;
        തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ rlimit_parse_sec(സ്ഥിരം അക്ഷരം *val, rlim_t *ret) {
        uint64_t u;
        usec_t t;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(val);
        പ്രതിജ്ഞാ(ret);

        r = parse_sec(val, &t);
        എങ്കിൽ (r < 0)
                തിരികെ r;
        എങ്കിൽ (t == USEC_INFINITY) {
                *ret = RLIM_INFINITY;
                തിരികെ 0;
        }

        u = (uint64_t) DIV_ROUND_UP(t, USEC_PER_SEC);
        എങ്കിൽ (u >= (uint64_t) RLIM_INFINITY)
                തിരികെ -ERANGE;

        *ret = (rlim_t) u;
        തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ rlimit_parse_usec(സ്ഥിരം അക്ഷരം *val, rlim_t *ret) {
        usec_t t;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(val);
        പ്രതിജ്ഞാ(ret);

        r = parse_time(val, &t, 1);
        എങ്കിൽ (r < 0)
                തിരികെ r;
        എങ്കിൽ (t == USEC_INFINITY) {
                *ret = RLIM_INFINITY;
                തിരികെ 0;
        }

        *ret = (rlim_t) t;
        തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ rlimit_parse_nice(സ്ഥിരം അക്ഷരം *val, rlim_t *ret) {
        uint64_t rl;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(val);
        പ്രതിജ്ഞാ(ret);

        /* So, Linux is weird. The range for RLIMIT_NICE is 40..1, mapping to the nice levels -20..19. However, the
         * RLIMIT_NICE limit defaults to 0 by the kernel, i.e. a value that maps to nice level 20, which of course is
         * bogus and does not exist. In order to permit parsing the RLIMIT_NICE of 0 here we hence implement a slight
         * asymmetry: when parsing as positive nice level we permit 0..19. When parsing as negative nice level, we
         * permit -20..0. But when parsing as raw resource limit value then we also allow the special value 0.
         *
         * Yeah, Linux is quality engineering sometimes... */

        എങ്കിൽ (val[0] == '+') {

                /* Prefixed with "+": Parse as positive user-friendly nice value */
                r = safe_atou64(val + 1, &rl);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                എങ്കിൽ (rl >= PRIO_MAX)
                        തിരികെ -ERANGE;

                rl = 20 - rl;

        } അല്ലെങ്കിൽ എങ്കിൽ (val[0] == '-') {

                /* Prefixed with "-": Parse as negative user-friendly nice value */
                r = safe_atou64(val + 1, &rl);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                എങ്കിൽ (rl > (uint64_t) (-PRIO_MIN))
                        തിരികെ -ERANGE;

                rl = 20 + rl;
        } അല്ലെങ്കിൽ {

                /* Not prefixed: parse as raw resource limit value */
                r = safe_atou64(val, &rl);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                എങ്കിൽ (rl > (uint64_t) (20 - PRIO_MIN))
                        തിരികെ -ERANGE;
        }

        *ret = (rlim_t) rl;
        തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ (*സ്ഥിരം rlimit_parse_table[_RLIMIT_MAX])(സ്ഥിരം അക്ഷരം *val, rlim_t *ret) = {
        [RLIMIT_CPU]        = rlimit_parse_sec,
        [RLIMIT_FSIZE]      = rlimit_parse_size,
        [RLIMIT_DATA]       = rlimit_parse_size,
        [RLIMIT_STACK]      = rlimit_parse_size,
        [RLIMIT_CORE]       = rlimit_parse_size,
        [RLIMIT_RSS]        = rlimit_parse_size,
        [RLIMIT_NOFILE]     = rlimit_parse_u64,
        [RLIMIT_AS]         = rlimit_parse_size,
        [RLIMIT_NPROC]      = rlimit_parse_u64,
        [RLIMIT_MEMLOCK]    = rlimit_parse_size,
        [RLIMIT_LOCKS]      = rlimit_parse_u64,
        [RLIMIT_SIGPENDING] = rlimit_parse_u64,
        [RLIMIT_MSGQUEUE]   = rlimit_parse_size,
        [RLIMIT_NICE]       = rlimit_parse_nice,
        [RLIMIT_RTPRIO]     = rlimit_parse_u64,
        [RLIMIT_RTTIME]     = rlimit_parse_usec,
};

പൂർണ്ണസംഖ്യ rlimit_parse_one(പൂർണ്ണസംഖ്യ resource, സ്ഥിരം അക്ഷരം *val, rlim_t *ret) {
        പ്രതിജ്ഞാ(val);
        പ്രതിജ്ഞാ(ret);

        എങ്കിൽ (resource < 0)
                തിരികെ -EINVAL;
        എങ്കിൽ (resource >= _RLIMIT_MAX)
                തിരികെ -EINVAL;

        തിരികെ rlimit_parse_table[resource](val, ret);
}

പൂർണ്ണസംഖ്യ rlimit_parse(പൂർണ്ണസംഖ്യ resource, സ്ഥിരം അക്ഷരം *val, ഘടന rlimit *ret) {
        _cleanup_free_ അക്ഷരം *hard = ശൂന്യ, *soft = ശൂന്യ;
        rlim_t hl, sl;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(val);
        പ്രതിജ്ഞാ(ret);

        r = extract_first_word(&val, &soft, ":", EXTRACT_DONT_COALESCE_SEPARATORS);
        എങ്കിൽ (r < 0)
                തിരികെ r;
        എങ്കിൽ (r == 0)
                തിരികെ -EINVAL;

        r = rlimit_parse_one(resource, soft, &sl);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        r = extract_first_word(&val, &hard, ":", EXTRACT_DONT_COALESCE_SEPARATORS);
        എങ്കിൽ (r < 0)
                തിരികെ r;
        എങ്കിൽ (!isempty(val))
                തിരികെ -EINVAL;
        എങ്കിൽ (r == 0)
                hl = sl;
        അല്ലെങ്കിൽ {
                r = rlimit_parse_one(resource, hard, &hl);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
                എങ്കിൽ (sl > hl)
                        തിരികെ -EILSEQ;
        }

        *ret = (ഘടന rlimit) {
                .rlim_cur = sl,
                .rlim_max = hl,
        };

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ rlimit_format(സ്ഥിരം ഘടന rlimit *rl, അക്ഷരം **ret) {
        അക്ഷരം *s;

        പ്രതിജ്ഞാ(rl);
        പ്രതിജ്ഞാ(ret);

        എങ്കിൽ (rl->rlim_cur >= RLIM_INFINITY && rl->rlim_max >= RLIM_INFINITY)
                s = strdup("infinity");
        അല്ലെങ്കിൽ എങ്കിൽ (rl->rlim_cur >= RLIM_INFINITY)
                s = asprintf_safe("infinity:" RLIM_FMT, rl->rlim_max);
        അല്ലെങ്കിൽ എങ്കിൽ (rl->rlim_max >= RLIM_INFINITY)
                s = asprintf_safe(RLIM_FMT ":infinity", rl->rlim_cur);
        അല്ലെങ്കിൽ എങ്കിൽ (rl->rlim_cur == rl->rlim_max)
                s = asprintf_safe(RLIM_FMT, rl->rlim_cur);
        അല്ലെങ്കിൽ
                s = asprintf_safe(RLIM_FMT ":" RLIM_FMT, rl->rlim_cur, rl->rlim_max);
        എങ്കിൽ (!s)
                തിരികെ -ENOMEM;

        *ret = s;
        തിരികെ 0;
}

സ്ഥിതം സ്ഥിരം അക്ഷരം* സ്ഥിരം rlimit_table[_RLIMIT_MAX] = {
        [RLIMIT_AS]         = "AS",
        [RLIMIT_CORE]       = "CORE",
        [RLIMIT_CPU]        = "CPU",
        [RLIMIT_DATA]       = "DATA",
        [RLIMIT_FSIZE]      = "FSIZE",
        [RLIMIT_LOCKS]      = "LOCKS",
        [RLIMIT_MEMLOCK]    = "MEMLOCK",
        [RLIMIT_MSGQUEUE]   = "MSGQUEUE",
        [RLIMIT_NICE]       = "NICE",
        [RLIMIT_NOFILE]     = "NOFILE",
        [RLIMIT_NPROC]      = "NPROC",
        [RLIMIT_RSS]        = "RSS",
        [RLIMIT_RTPRIO]     = "RTPRIO",
        [RLIMIT_RTTIME]     = "RTTIME",
        [RLIMIT_SIGPENDING] = "SIGPENDING",
        [RLIMIT_STACK]      = "STACK",
};

DEFINE_STRING_TABLE_LOOKUP(rlimit, പൂർണ്ണസംഖ്യ);

ശൂന്യം rlimits_list(സ്ഥിരം അക്ഷരം *prefix) {
        FOREACH_ELEMENT(field, rlimit_table)
                printf("%s%s\n", strempty(prefix), *field);
}

പൂർണ്ണസംഖ്യ rlimit_from_string_harder(സ്ഥിരം അക്ഷരം *s) {
        സ്ഥിരം അക്ഷരം *suffix;

        /* The official prefix */
        suffix = startswith(s, "RLIMIT_");
        എങ്കിൽ (suffix)
                തിരികെ rlimit_from_string(suffix);

        /* Our own unit file setting prefix */
        suffix = startswith(s, "Limit");
        എങ്കിൽ (suffix)
                തിരികെ rlimit_from_string(suffix);

        തിരികെ rlimit_from_string(s);
}

ശൂന്യം rlimit_free_all(ഘടന rlimit **rl) {
        free_many((ശൂന്യം**) rl, _RLIMIT_MAX);
}

പൂർണ്ണസംഖ്യ rlimit_copy_all(ഘടന rlimit* target[സ്ഥിതം _RLIMIT_MAX], ഘടന rlimit* സ്ഥിരം source[സ്ഥിതം _RLIMIT_MAX]) {
        ഘടന rlimit* copy[_RLIMIT_MAX] = {};

        പ്രതിജ്ഞാ(target);
        പ്രതിജ്ഞാ(source);

        വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < _RLIMIT_MAX; i++) {
                എങ്കിൽ (!source[i])
                        തുടരുക;

                copy[i] = newdup(ഘടന rlimit, source[i], 1);
                എങ്കിൽ (!copy[i]) {
                        rlimit_free_all(copy);
                        തിരികെ -ENOMEM;
                }
        }

        memcpy(target, copy, വലുപ്പം(ഘടന rlimit*) * _RLIMIT_MAX);
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ rlimit_nofile_bump(പൂർണ്ണസംഖ്യ limit) {
        പൂർണ്ണസംഖ്യ r;

        /* Bumps the (soft) RLIMIT_NOFILE resource limit as close as possible to the specified limit. If a negative
         * limit is specified, bumps it to the maximum the kernel and the hard resource limit allows. This call should
         * be used by all our programs that might need a lot of fds, and that know how to deal with high fd numbers
         * (i.e. do not use select() — which chokes on fds >= 1024) */

        എങ്കിൽ (limit < 0)
                limit = read_nr_open();

        എങ്കിൽ (limit < 3)
                limit = 3;

        r = setrlimit_closest(RLIMIT_NOFILE, &RLIMIT_MAKE_CONST(limit));
        എങ്കിൽ (r < 0)
                തിരികെ log_debug_errno(r, "Failed to set RLIMIT_NOFILE: %m");

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ rlimit_nofile_safe(ശൂന്യം) {
        ഘടന rlimit rl;

        /* Resets RLIMIT_NOFILE's soft limit FD_SETSIZE (i.e. 1024), for compatibility with software still using
         * select() */

        എങ്കിൽ (getrlimit(RLIMIT_NOFILE, &rl) < 0)
                തിരികെ log_debug_errno(errno, "Failed to query RLIMIT_NOFILE: %m");

        എങ്കിൽ (rl.rlim_cur <= FD_SETSIZE)
                തിരികെ 0;

        /* So we might have inherited a hard limit that's larger than the kernel's maximum limit as stored in
         * /proc/sys/fs/nr_open. If we pass this hard limit unmodified to setrlimit(), we'll get EPERM. To
         * make sure that doesn't happen, let's limit our hard limit to the value from nr_open. */
        rl.rlim_max = MIN(rl.rlim_max, (rlim_t) read_nr_open());
        rl.rlim_cur = MIN((rlim_t) FD_SETSIZE, rl.rlim_max);
        എങ്കിൽ (setrlimit(RLIMIT_NOFILE, &rl) < 0)
                തിരികെ log_debug_errno(errno, "Failed to lower RLIMIT_NOFILE's soft limit to " RLIM_FMT ": %m", rl.rlim_cur);

        തിരികെ 1;
}

പൂർണ്ണസംഖ്യ pid_getrlimit(pid_t pid, പൂർണ്ണസംഖ്യ resource, ഘടന rlimit *ret) {

        സ്ഥിതം സ്ഥിരം അക്ഷരം * സ്ഥിരം prefix_table[_RLIMIT_MAX] = {
                [RLIMIT_CPU]        = "Max cpu time",
                [RLIMIT_FSIZE]      = "Max file size",
                [RLIMIT_DATA]       = "Max data size",
                [RLIMIT_STACK]      = "Max stack size",
                [RLIMIT_CORE]       = "Max core file size",
                [RLIMIT_RSS]        = "Max resident set",
                [RLIMIT_NPROC]      = "Max processes",
                [RLIMIT_NOFILE]     = "Max open files",
                [RLIMIT_MEMLOCK]    = "Max locked memory",
                [RLIMIT_AS]         = "Max address space",
                [RLIMIT_LOCKS]      = "Max file locks",
                [RLIMIT_SIGPENDING] = "Max pending signals",
                [RLIMIT_MSGQUEUE]   = "Max msgqueue size",
                [RLIMIT_NICE]       = "Max nice priority",
                [RLIMIT_RTPRIO]     = "Max realtime priority",
                [RLIMIT_RTTIME]     = "Max realtime timeout",
        };

        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(resource >= 0);
        പ്രതിജ്ഞാ(resource < _RLIMIT_MAX);
        പ്രതിജ്ഞാ(pid >= 0);
        പ്രതിജ്ഞാ(ret);

        എങ്കിൽ (pid == 0 || pid == getpid_cached())
                തിരികെ RET_NERRNO(getrlimit(resource, ret));

        r = RET_NERRNO(prlimit(pid, resource, /* new_limit= */ ശൂന്യ, ret));
        എങ്കിൽ (!ERRNO_IS_NEG_PRIVILEGE(r))
                തിരികെ r;

        /* We don't have access? Then try to go via /proc/$PID/limits. Weirdly that's world readable in
         * contrast to querying the data via prlimit() */

        സ്ഥിരം അക്ഷരം *p = procfs_file_alloca(pid, "limits");
        _cleanup_free_ അക്ഷരം *limits = ശൂന്യ;

        r = read_full_file(p, &limits, /* ret_size= */ ശൂന്യ);
        എങ്കിൽ (r < 0)
                തിരികെ -EPERM; /* propagate original permission error if we can't access the limits file */

        _cleanup_strv_free_ അക്ഷരം **l = ശൂന്യ;
        l = strv_split_newlines(limits);
        എങ്കിൽ (!l)
                തിരികെ -ENOMEM;

        STRV_FOREACH(i, strv_skip(l, 1)) {
                _cleanup_free_ അക്ഷരം *soft = ശൂന്യ, *hard = ശൂന്യ;
                uint64_t sv, hv;
                സ്ഥിരം അക്ഷരം *e;

                e = startswith(*i, prefix_table[resource]);
                എങ്കിൽ (!e)
                        തുടരുക;

                എങ്കിൽ (*e != ' ')
                        തുടരുക;

                e += strspn(e, WHITESPACE);

                size_t n;
                n = strcspn(e, WHITESPACE);
                എങ്കിൽ (n == 0)
                        തുടരുക;

                soft = strndup(e, n);
                എങ്കിൽ (!soft)
                        തിരികെ -ENOMEM;

                e += n;
                എങ്കിൽ (*e != ' ')
                        തുടരുക;

                e += strspn(e, WHITESPACE);
                n = strcspn(e, WHITESPACE);
                എങ്കിൽ (n == 0)
                        തുടരുക;

                hard = strndup(e, n);
                എങ്കിൽ (!hard)
                        തിരികെ -ENOMEM;

                എങ്കിൽ (streq(soft, "unlimited"))
                        sv = RLIM_INFINITY;
                അല്ലെങ്കിൽ {
                        r = safe_atou64(soft, &sv);
                        എങ്കിൽ (r < 0)
                                തിരികെ r;
                }

                എങ്കിൽ (streq(hard, "unlimited"))
                        hv = RLIM_INFINITY;
                അല്ലെങ്കിൽ {
                        r = safe_atou64(hard, &hv);
                        എങ്കിൽ (r < 0)
                                തിരികെ r;
                }

                *ret = (ഘടന rlimit) {
                        .rlim_cur = sv,
                        .rlim_max = hv,
                };

                തിരികെ 0;
        }

        തിരികെ -ENOTRECOVERABLE;
}
