/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن "alloc-util.h"
#ضمن "errno-util.h"
#ضمن "extract-word.h"
#ضمن "fd-util.h"
#ضمن "fileio.h"
#ضمن "format-util.h"
#ضمن "log.h"
#ضمن "parse-util.h"
#ضمن "process-util.h"
#ضمن "rlimit-util.h"
#ضمن "string-table.h"
#ضمن "strv.h"
#ضمن "time-util.h"

صحيح setrlimit_closest(صحيح resource, ثابت بنية rlimit *rlim) {
        بنية rlimit highest, fixed;

        أكد(rlim);

        𐡄𐡍 (setrlimit(resource, rlim) >= 0)
                أرجع 0;

        𐡄𐡍 (errno != EPERM)
                أرجع -errno;

        /* So we failed to set the desired setrlimit, then let's try
         * to get as close as we can */
        𐡄𐡍 (getrlimit(resource, &highest) < 0)
                أرجع -errno;

        /* If the hard limit is unbounded anyway, then the EPERM had other reasons, let's propagate the original EPERM
         * then */
        𐡄𐡍 (highest.rlim_max == RLIM_INFINITY)
                أرجع -EPERM;

        fixed = (بنية rlimit) {
                .rlim_cur = MIN(rlim->rlim_cur, highest.rlim_max),
                .rlim_max = MIN(rlim->rlim_max, highest.rlim_max),
        };

        /* Shortcut things if we wouldn't change anything. */
        𐡄𐡍 (fixed.rlim_cur == highest.rlim_cur &&
            fixed.rlim_max == highest.rlim_max)
                أرجع 0;

        log_debug("Failed at setting rlimit " RLIM_FMT " for resource RLIMIT_%s. Will attempt setting value " RLIM_FMT " instead.", rlim->rlim_max, rlimit_to_string(resource), fixed.rlim_max);

        أرجع RET_NERRNO(setrlimit(resource, &fixed));
}

صحيح setrlimit_closest_all(ثابت بنية rlimit *ثابت *rlim, صحيح *which_failed) {
        صحيح r;

        أكد(rlim);

        /* On failure returns the limit's index that failed in *which_failed, but only if non-NULL */

        لكل (صحيح i = 0; i < _RLIMIT_MAX; i++) {
                𐡄𐡍 (!rlim[i])
                        استمر;

                r = setrlimit_closest(i, rlim[i]);
                𐡄𐡍 (r < 0) {
                        𐡄𐡍 (which_failed)
                                *which_failed = i;

                        أرجع r;
                }
        }

        𐡄𐡍 (which_failed)
                *which_failed = -1;

        أرجع 0;
}

ساكن صحيح rlimit_parse_u64(ثابت حرف *val, rlim_t *ret) {
        uint64_t u;
        صحيح r;

        أكد(val);
        أكد(ret);

        𐡄𐡍 (streq(val, "infinity")) {
                *ret = RLIM_INFINITY;
                أرجع 0;
        }

        /* setrlimit(2) suggests rlim_t is always 64-bit on Linux. */
        assert_cc(حجم(rlim_t) == حجم(uint64_t));

        r = safe_atou64(val, &u);
        𐡄𐡍 (r < 0)
                أرجع r;
        𐡄𐡍 (u >= (uint64_t) RLIM_INFINITY)
                أرجع -ERANGE;

        *ret = (rlim_t) u;
        أرجع 0;
}

ساكن صحيح rlimit_parse_size(ثابت حرف *val, rlim_t *ret) {
        uint64_t u;
        صحيح r;

        أكد(val);
        أكد(ret);

        𐡄𐡍 (streq(val, "infinity")) {
                *ret = RLIM_INFINITY;
                أرجع 0;
        }

        r = parse_size(val, 1024, &u);
        𐡄𐡍 (r < 0)
                أرجع r;
        𐡄𐡍 (u >= (uint64_t) RLIM_INFINITY)
                أرجع -ERANGE;

        *ret = (rlim_t) u;
        أرجع 0;
}

ساكن صحيح rlimit_parse_sec(ثابت حرف *val, rlim_t *ret) {
        uint64_t u;
        usec_t t;
        صحيح r;

        أكد(val);
        أكد(ret);

        r = parse_sec(val, &t);
        𐡄𐡍 (r < 0)
                أرجع r;
        𐡄𐡍 (t == USEC_INFINITY) {
                *ret = RLIM_INFINITY;
                أرجع 0;
        }

        u = (uint64_t) DIV_ROUND_UP(t, USEC_PER_SEC);
        𐡄𐡍 (u >= (uint64_t) RLIM_INFINITY)
                أرجع -ERANGE;

        *ret = (rlim_t) u;
        أرجع 0;
}

ساكن صحيح rlimit_parse_usec(ثابت حرف *val, rlim_t *ret) {
        usec_t t;
        صحيح r;

        أكد(val);
        أكد(ret);

        r = parse_time(val, &t, 1);
        𐡄𐡍 (r < 0)
                أرجع r;
        𐡄𐡍 (t == USEC_INFINITY) {
                *ret = RLIM_INFINITY;
                أرجع 0;
        }

        *ret = (rlim_t) t;
        أرجع 0;
}

ساكن صحيح rlimit_parse_nice(ثابت حرف *val, rlim_t *ret) {
        uint64_t rl;
        صحيح r;

        أكد(val);
        أكد(ret);

        /* So, Linux is weird. The range for RLIMIT_NICE is 40..1, mapping to the nice levels -20..19. However, the
         * RLIMIT_NICE limit defaults to 0 by the kernel, i.e. a value that maps to nice level 20, which of course is
         * bogus and does not exist. In order to permit parsing the RLIMIT_NICE of 0 here we hence implement a slight
         * asymmetry: when parsing as positive nice level we permit 0..19. When parsing as negative nice level, we
         * permit -20..0. But when parsing as raw resource limit value then we also allow the special value 0.
         *
         * Yeah, Linux is quality engineering sometimes... */

        𐡄𐡍 (val[0] == '+') {

                /* Prefixed with "+": Parse as positive user-friendly nice value */
                r = safe_atou64(val + 1, &rl);
                𐡄𐡍 (r < 0)
                        أرجع r;

                𐡄𐡍 (rl >= PRIO_MAX)
                        أرجع -ERANGE;

                rl = 20 - rl;

        } وإلا 𐡄𐡍 (val[0] == '-') {

                /* Prefixed with "-": Parse as negative user-friendly nice value */
                r = safe_atou64(val + 1, &rl);
                𐡄𐡍 (r < 0)
                        أرجع r;

                𐡄𐡍 (rl > (uint64_t) (-PRIO_MIN))
                        أرجع -ERANGE;

                rl = 20 + rl;
        } وإلا {

                /* Not prefixed: parse as raw resource limit value */
                r = safe_atou64(val, &rl);
                𐡄𐡍 (r < 0)
                        أرجع r;

                𐡄𐡍 (rl > (uint64_t) (20 - PRIO_MIN))
                        أرجع -ERANGE;
        }

        *ret = (rlim_t) rl;
        أرجع 0;
}

ساكن صحيح (*ثابت rlimit_parse_table[_RLIMIT_MAX])(ثابت حرف *val, rlim_t *ret) = {
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

صحيح rlimit_parse_one(صحيح resource, ثابت حرف *val, rlim_t *ret) {
        أكد(val);
        أكد(ret);

        𐡄𐡍 (resource < 0)
                أرجع -EINVAL;
        𐡄𐡍 (resource >= _RLIMIT_MAX)
                أرجع -EINVAL;

        أرجع rlimit_parse_table[resource](val, ret);
}

صحيح rlimit_parse(صحيح resource, ثابت حرف *val, بنية rlimit *ret) {
        _cleanup_free_ حرف *hard = عدم, *soft = عدم;
        rlim_t hl, sl;
        صحيح r;

        أكد(val);
        أكد(ret);

        r = extract_first_word(&val, &soft, ":", EXTRACT_DONT_COALESCE_SEPARATORS);
        𐡄𐡍 (r < 0)
                أرجع r;
        𐡄𐡍 (r == 0)
                أرجع -EINVAL;

        r = rlimit_parse_one(resource, soft, &sl);
        𐡄𐡍 (r < 0)
                أرجع r;

        r = extract_first_word(&val, &hard, ":", EXTRACT_DONT_COALESCE_SEPARATORS);
        𐡄𐡍 (r < 0)
                أرجع r;
        𐡄𐡍 (!isempty(val))
                أرجع -EINVAL;
        𐡄𐡍 (r == 0)
                hl = sl;
        وإلا {
                r = rlimit_parse_one(resource, hard, &hl);
                𐡄𐡍 (r < 0)
                        أرجع r;
                𐡄𐡍 (sl > hl)
                        أرجع -EILSEQ;
        }

        *ret = (بنية rlimit) {
                .rlim_cur = sl,
                .rlim_max = hl,
        };

        أرجع 0;
}

صحيح rlimit_format(ثابت بنية rlimit *rl, حرف **ret) {
        حرف *s;

        أكد(rl);
        أكد(ret);

        𐡄𐡍 (rl->rlim_cur >= RLIM_INFINITY && rl->rlim_max >= RLIM_INFINITY)
                s = strdup("infinity");
        وإلا 𐡄𐡍 (rl->rlim_cur >= RLIM_INFINITY)
                s = asprintf_safe("infinity:" RLIM_FMT, rl->rlim_max);
        وإلا 𐡄𐡍 (rl->rlim_max >= RLIM_INFINITY)
                s = asprintf_safe(RLIM_FMT ":infinity", rl->rlim_cur);
        وإلا 𐡄𐡍 (rl->rlim_cur == rl->rlim_max)
                s = asprintf_safe(RLIM_FMT, rl->rlim_cur);
        وإلا
                s = asprintf_safe(RLIM_FMT ":" RLIM_FMT, rl->rlim_cur, rl->rlim_max);
        𐡄𐡍 (!s)
                أرجع -ENOMEM;

        *ret = s;
        أرجع 0;
}

ساكن ثابت حرف* ثابت rlimit_table[_RLIMIT_MAX] = {
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

DEFINE_STRING_TABLE_LOOKUP(rlimit, صحيح);

فراغ rlimits_list(ثابت حرف *prefix) {
        FOREACH_ELEMENT(field, rlimit_table)
                printf("%s%s\n", strempty(prefix), *field);
}

صحيح rlimit_from_string_harder(ثابت حرف *s) {
        ثابت حرف *suffix;

        /* The official prefix */
        suffix = startswith(s, "RLIMIT_");
        𐡄𐡍 (suffix)
                أرجع rlimit_from_string(suffix);

        /* Our own unit file setting prefix */
        suffix = startswith(s, "Limit");
        𐡄𐡍 (suffix)
                أرجع rlimit_from_string(suffix);

        أرجع rlimit_from_string(s);
}

فراغ rlimit_free_all(بنية rlimit **rl) {
        free_many((فراغ**) rl, _RLIMIT_MAX);
}

صحيح rlimit_copy_all(بنية rlimit* target[ساكن _RLIMIT_MAX], بنية rlimit* ثابت source[ساكن _RLIMIT_MAX]) {
        بنية rlimit* copy[_RLIMIT_MAX] = {};

        أكد(target);
        أكد(source);

        لكل (صحيح i = 0; i < _RLIMIT_MAX; i++) {
                𐡄𐡍 (!source[i])
                        استمر;

                copy[i] = newdup(بنية rlimit, source[i], 1);
                𐡄𐡍 (!copy[i]) {
                        rlimit_free_all(copy);
                        أرجع -ENOMEM;
                }
        }

        memcpy(target, copy, حجم(بنية rlimit*) * _RLIMIT_MAX);
        أرجع 0;
}

صحيح rlimit_nofile_bump(صحيح limit) {
        صحيح r;

        /* Bumps the (soft) RLIMIT_NOFILE resource limit as close as possible to the specified limit. If a negative
         * limit is specified, bumps it to the maximum the kernel and the hard resource limit allows. This call should
         * be used by all our programs that might need a lot of fds, and that know how to deal with high fd numbers
         * (i.e. do not use select() — which chokes on fds >= 1024) */

        𐡄𐡍 (limit < 0)
                limit = read_nr_open();

        𐡄𐡍 (limit < 3)
                limit = 3;

        r = setrlimit_closest(RLIMIT_NOFILE, &RLIMIT_MAKE_CONST(limit));
        𐡄𐡍 (r < 0)
                أرجع log_debug_errno(r, "Failed to set RLIMIT_NOFILE: %m");

        أرجع 0;
}

صحيح rlimit_nofile_safe(فراغ) {
        بنية rlimit rl;

        /* Resets RLIMIT_NOFILE's soft limit FD_SETSIZE (i.e. 1024), for compatibility with software still using
         * select() */

        𐡄𐡍 (getrlimit(RLIMIT_NOFILE, &rl) < 0)
                أرجع log_debug_errno(errno, "Failed to query RLIMIT_NOFILE: %m");

        𐡄𐡍 (rl.rlim_cur <= FD_SETSIZE)
                أرجع 0;

        /* So we might have inherited a hard limit that's larger than the kernel's maximum limit as stored in
         * /proc/sys/fs/nr_open. If we pass this hard limit unmodified to setrlimit(), we'll get EPERM. To
         * make sure that doesn't happen, let's limit our hard limit to the value from nr_open. */
        rl.rlim_max = MIN(rl.rlim_max, (rlim_t) read_nr_open());
        rl.rlim_cur = MIN((rlim_t) FD_SETSIZE, rl.rlim_max);
        𐡄𐡍 (setrlimit(RLIMIT_NOFILE, &rl) < 0)
                أرجع log_debug_errno(errno, "Failed to lower RLIMIT_NOFILE's soft limit to " RLIM_FMT ": %m", rl.rlim_cur);

        أرجع 1;
}

صحيح pid_getrlimit(pid_t pid, صحيح resource, بنية rlimit *ret) {

        ساكن ثابت حرف * ثابت prefix_table[_RLIMIT_MAX] = {
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

        صحيح r;

        أكد(resource >= 0);
        أكد(resource < _RLIMIT_MAX);
        أكد(pid >= 0);
        أكد(ret);

        𐡄𐡍 (pid == 0 || pid == getpid_cached())
                أرجع RET_NERRNO(getrlimit(resource, ret));

        r = RET_NERRNO(prlimit(pid, resource, /* new_limit= */ عدم, ret));
        𐡄𐡍 (!ERRNO_IS_NEG_PRIVILEGE(r))
                أرجع r;

        /* We don't have access? Then try to go via /proc/$PID/limits. Weirdly that's world readable in
         * contrast to querying the data via prlimit() */

        ثابت حرف *p = procfs_file_alloca(pid, "limits");
        _cleanup_free_ حرف *limits = عدم;

        r = read_full_file(p, &limits, /* ret_size= */ عدم);
        𐡄𐡍 (r < 0)
                أرجع -EPERM; /* propagate original permission error if we can't access the limits file */

        _cleanup_strv_free_ حرف **l = عدم;
        l = strv_split_newlines(limits);
        𐡄𐡍 (!l)
                أرجع -ENOMEM;

        STRV_FOREACH(i, strv_skip(l, 1)) {
                _cleanup_free_ حرف *soft = عدم, *hard = عدم;
                uint64_t sv, hv;
                ثابت حرف *e;

                e = startswith(*i, prefix_table[resource]);
                𐡄𐡍 (!e)
                        استمر;

                𐡄𐡍 (*e != ' ')
                        استمر;

                e += strspn(e, WHITESPACE);

                size_t n;
                n = strcspn(e, WHITESPACE);
                𐡄𐡍 (n == 0)
                        استمر;

                soft = strndup(e, n);
                𐡄𐡍 (!soft)
                        أرجع -ENOMEM;

                e += n;
                𐡄𐡍 (*e != ' ')
                        استمر;

                e += strspn(e, WHITESPACE);
                n = strcspn(e, WHITESPACE);
                𐡄𐡍 (n == 0)
                        استمر;

                hard = strndup(e, n);
                𐡄𐡍 (!hard)
                        أرجع -ENOMEM;

                𐡄𐡍 (streq(soft, "unlimited"))
                        sv = RLIM_INFINITY;
                وإلا {
                        r = safe_atou64(soft, &sv);
                        𐡄𐡍 (r < 0)
                                أرجع r;
                }

                𐡄𐡍 (streq(hard, "unlimited"))
                        hv = RLIM_INFINITY;
                وإلا {
                        r = safe_atou64(hard, &hv);
                        𐡄𐡍 (r < 0)
                                أرجع r;
                }

                *ret = (بنية rlimit) {
                        .rlim_cur = sv,
                        .rlim_max = hv,
                };

                أرجع 0;
        }

        أرجع -ENOTRECOVERABLE;
}
