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

        אם (setrlimit(resource, rlim) >= 0)
                החזר 0;

        אם (errno != EPERM)
                החזר -errno;

        /* So we failed to set the desired setrlimit, then let's try
         * to get as close as we can */
        אם (getrlimit(resource, &highest) < 0)
                החזר -errno;

        /* If the hard limit is unbounded anyway, then the EPERM had other reasons, let's propagate the original EPERM
         * then */
        אם (highest.rlim_max == RLIM_INFINITY)
                החזר -EPERM;

        fixed = (بنية rlimit) {
                .rlim_cur = MIN(rlim->rlim_cur, highest.rlim_max),
                .rlim_max = MIN(rlim->rlim_max, highest.rlim_max),
        };

        /* Shortcut things if we wouldn't change anything. */
        אם (fixed.rlim_cur == highest.rlim_cur &&
            fixed.rlim_max == highest.rlim_max)
                החזר 0;

        log_debug("Failed at setting rlimit " RLIM_FMT " for resource RLIMIT_%s. Will attempt setting value " RLIM_FMT " instead.", rlim->rlim_max, rlimit_to_string(resource), fixed.rlim_max);

        החזר RET_NERRNO(setrlimit(resource, &fixed));
}

صحيح setrlimit_closest_all(ثابت بنية rlimit *ثابت *rlim, صحيح *which_failed) {
        صحيح r;

        أكد(rlim);

        /* On failure returns the limit's index that failed in *which_failed, but only if non-NULL */

        עבור (صحيح i = 0; i < _RLIMIT_MAX; i++) {
                אם (!rlim[i])
                        استمر;

                r = setrlimit_closest(i, rlim[i]);
                אם (r < 0) {
                        אם (which_failed)
                                *which_failed = i;

                        החזר r;
                }
        }

        אם (which_failed)
                *which_failed = -1;

        החזר 0;
}

ساكن صحيح rlimit_parse_u64(ثابت حرف *val, rlim_t *ret) {
        uint64_t u;
        صحيح r;

        أكد(val);
        أكد(ret);

        אם (streq(val, "infinity")) {
                *ret = RLIM_INFINITY;
                החזר 0;
        }

        /* setrlimit(2) suggests rlim_t is always 64-bit on Linux. */
        assert_cc(حجم(rlim_t) == حجم(uint64_t));

        r = safe_atou64(val, &u);
        אם (r < 0)
                החזר r;
        אם (u >= (uint64_t) RLIM_INFINITY)
                החזר -ERANGE;

        *ret = (rlim_t) u;
        החזר 0;
}

ساكن صحيح rlimit_parse_size(ثابت حرف *val, rlim_t *ret) {
        uint64_t u;
        صحيح r;

        أكد(val);
        أكد(ret);

        אם (streq(val, "infinity")) {
                *ret = RLIM_INFINITY;
                החזר 0;
        }

        r = parse_size(val, 1024, &u);
        אם (r < 0)
                החזר r;
        אם (u >= (uint64_t) RLIM_INFINITY)
                החזר -ERANGE;

        *ret = (rlim_t) u;
        החזר 0;
}

ساكن صحيح rlimit_parse_sec(ثابت حرف *val, rlim_t *ret) {
        uint64_t u;
        usec_t t;
        صحيح r;

        أكد(val);
        أكد(ret);

        r = parse_sec(val, &t);
        אם (r < 0)
                החזר r;
        אם (t == USEC_INFINITY) {
                *ret = RLIM_INFINITY;
                החזר 0;
        }

        u = (uint64_t) DIV_ROUND_UP(t, USEC_PER_SEC);
        אם (u >= (uint64_t) RLIM_INFINITY)
                החזר -ERANGE;

        *ret = (rlim_t) u;
        החזר 0;
}

ساكن صحيح rlimit_parse_usec(ثابت حرف *val, rlim_t *ret) {
        usec_t t;
        صحيح r;

        أكد(val);
        أكد(ret);

        r = parse_time(val, &t, 1);
        אם (r < 0)
                החזר r;
        אם (t == USEC_INFINITY) {
                *ret = RLIM_INFINITY;
                החזר 0;
        }

        *ret = (rlim_t) t;
        החזר 0;
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

        אם (val[0] == '+') {

                /* Prefixed with "+": Parse as positive user-friendly nice value */
                r = safe_atou64(val + 1, &rl);
                אם (r < 0)
                        החזר r;

                אם (rl >= PRIO_MAX)
                        החזר -ERANGE;

                rl = 20 - rl;

        } אחרת אם (val[0] == '-') {

                /* Prefixed with "-": Parse as negative user-friendly nice value */
                r = safe_atou64(val + 1, &rl);
                אם (r < 0)
                        החזר r;

                אם (rl > (uint64_t) (-PRIO_MIN))
                        החזר -ERANGE;

                rl = 20 + rl;
        } אחרת {

                /* Not prefixed: parse as raw resource limit value */
                r = safe_atou64(val, &rl);
                אם (r < 0)
                        החזר r;

                אם (rl > (uint64_t) (20 - PRIO_MIN))
                        החזר -ERANGE;
        }

        *ret = (rlim_t) rl;
        החזר 0;
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

        אם (resource < 0)
                החזר -EINVAL;
        אם (resource >= _RLIMIT_MAX)
                החזר -EINVAL;

        החזר rlimit_parse_table[resource](val, ret);
}

صحيح rlimit_parse(صحيح resource, ثابت حرف *val, بنية rlimit *ret) {
        _cleanup_free_ حرف *hard = عدم, *soft = عدم;
        rlim_t hl, sl;
        صحيح r;

        أكد(val);
        أكد(ret);

        r = extract_first_word(&val, &soft, ":", EXTRACT_DONT_COALESCE_SEPARATORS);
        אם (r < 0)
                החזר r;
        אם (r == 0)
                החזר -EINVAL;

        r = rlimit_parse_one(resource, soft, &sl);
        אם (r < 0)
                החזר r;

        r = extract_first_word(&val, &hard, ":", EXTRACT_DONT_COALESCE_SEPARATORS);
        אם (r < 0)
                החזר r;
        אם (!isempty(val))
                החזר -EINVAL;
        אם (r == 0)
                hl = sl;
        אחרת {
                r = rlimit_parse_one(resource, hard, &hl);
                אם (r < 0)
                        החזר r;
                אם (sl > hl)
                        החזר -EILSEQ;
        }

        *ret = (بنية rlimit) {
                .rlim_cur = sl,
                .rlim_max = hl,
        };

        החזר 0;
}

صحيح rlimit_format(ثابت بنية rlimit *rl, حرف **ret) {
        حرف *s;

        أكد(rl);
        أكد(ret);

        אם (rl->rlim_cur >= RLIM_INFINITY && rl->rlim_max >= RLIM_INFINITY)
                s = strdup("infinity");
        אחרת אם (rl->rlim_cur >= RLIM_INFINITY)
                s = asprintf_safe("infinity:" RLIM_FMT, rl->rlim_max);
        אחרת אם (rl->rlim_max >= RLIM_INFINITY)
                s = asprintf_safe(RLIM_FMT ":infinity", rl->rlim_cur);
        אחרת אם (rl->rlim_cur == rl->rlim_max)
                s = asprintf_safe(RLIM_FMT, rl->rlim_cur);
        אחרת
                s = asprintf_safe(RLIM_FMT ":" RLIM_FMT, rl->rlim_cur, rl->rlim_max);
        אם (!s)
                החזר -ENOMEM;

        *ret = s;
        החזר 0;
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
        אם (suffix)
                החזר rlimit_from_string(suffix);

        /* Our own unit file setting prefix */
        suffix = startswith(s, "Limit");
        אם (suffix)
                החזר rlimit_from_string(suffix);

        החזר rlimit_from_string(s);
}

فراغ rlimit_free_all(بنية rlimit **rl) {
        free_many((فراغ**) rl, _RLIMIT_MAX);
}

صحيح rlimit_copy_all(بنية rlimit* target[ساكن _RLIMIT_MAX], بنية rlimit* ثابت source[ساكن _RLIMIT_MAX]) {
        بنية rlimit* copy[_RLIMIT_MAX] = {};

        أكد(target);
        أكد(source);

        עבור (صحيح i = 0; i < _RLIMIT_MAX; i++) {
                אם (!source[i])
                        استمر;

                copy[i] = newdup(بنية rlimit, source[i], 1);
                אם (!copy[i]) {
                        rlimit_free_all(copy);
                        החזר -ENOMEM;
                }
        }

        memcpy(target, copy, حجم(بنية rlimit*) * _RLIMIT_MAX);
        החזר 0;
}

صحيح rlimit_nofile_bump(صحيح limit) {
        صحيح r;

        /* Bumps the (soft) RLIMIT_NOFILE resource limit as close as possible to the specified limit. If a negative
         * limit is specified, bumps it to the maximum the kernel and the hard resource limit allows. This call should
         * be used by all our programs that might need a lot of fds, and that know how to deal with high fd numbers
         * (i.e. do not use select() — which chokes on fds >= 1024) */

        אם (limit < 0)
                limit = read_nr_open();

        אם (limit < 3)
                limit = 3;

        r = setrlimit_closest(RLIMIT_NOFILE, &RLIMIT_MAKE_CONST(limit));
        אם (r < 0)
                החזר log_debug_errno(r, "Failed to set RLIMIT_NOFILE: %m");

        החזר 0;
}

صحيح rlimit_nofile_safe(فراغ) {
        بنية rlimit rl;

        /* Resets RLIMIT_NOFILE's soft limit FD_SETSIZE (i.e. 1024), for compatibility with software still using
         * select() */

        אם (getrlimit(RLIMIT_NOFILE, &rl) < 0)
                החזר log_debug_errno(errno, "Failed to query RLIMIT_NOFILE: %m");

        אם (rl.rlim_cur <= FD_SETSIZE)
                החזר 0;

        /* So we might have inherited a hard limit that's larger than the kernel's maximum limit as stored in
         * /proc/sys/fs/nr_open. If we pass this hard limit unmodified to setrlimit(), we'll get EPERM. To
         * make sure that doesn't happen, let's limit our hard limit to the value from nr_open. */
        rl.rlim_max = MIN(rl.rlim_max, (rlim_t) read_nr_open());
        rl.rlim_cur = MIN((rlim_t) FD_SETSIZE, rl.rlim_max);
        אם (setrlimit(RLIMIT_NOFILE, &rl) < 0)
                החזר log_debug_errno(errno, "Failed to lower RLIMIT_NOFILE's soft limit to " RLIM_FMT ": %m", rl.rlim_cur);

        החזר 1;
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

        אם (pid == 0 || pid == getpid_cached())
                החזר RET_NERRNO(getrlimit(resource, ret));

        r = RET_NERRNO(prlimit(pid, resource, /* new_limit= */ عدم, ret));
        אם (!ERRNO_IS_NEG_PRIVILEGE(r))
                החזר r;

        /* We don't have access? Then try to go via /proc/$PID/limits. Weirdly that's world readable in
         * contrast to querying the data via prlimit() */

        ثابت حرف *p = procfs_file_alloca(pid, "limits");
        _cleanup_free_ حرف *limits = عدم;

        r = read_full_file(p, &limits, /* ret_size= */ عدم);
        אם (r < 0)
                החזר -EPERM; /* propagate original permission error if we can't access the limits file */

        _cleanup_strv_free_ حرف **l = عدم;
        l = strv_split_newlines(limits);
        אם (!l)
                החזר -ENOMEM;

        STRV_FOREACH(i, strv_skip(l, 1)) {
                _cleanup_free_ حرف *soft = عدم, *hard = عدم;
                uint64_t sv, hv;
                ثابت حرف *e;

                e = startswith(*i, prefix_table[resource]);
                אם (!e)
                        استمر;

                אם (*e != ' ')
                        استمر;

                e += strspn(e, WHITESPACE);

                size_t n;
                n = strcspn(e, WHITESPACE);
                אם (n == 0)
                        استمر;

                soft = strndup(e, n);
                אם (!soft)
                        החזר -ENOMEM;

                e += n;
                אם (*e != ' ')
                        استمر;

                e += strspn(e, WHITESPACE);
                n = strcspn(e, WHITESPACE);
                אם (n == 0)
                        استمر;

                hard = strndup(e, n);
                אם (!hard)
                        החזר -ENOMEM;

                אם (streq(soft, "unlimited"))
                        sv = RLIM_INFINITY;
                אחרת {
                        r = safe_atou64(soft, &sv);
                        אם (r < 0)
                                החזר r;
                }

                אם (streq(hard, "unlimited"))
                        hv = RLIM_INFINITY;
                אחרת {
                        r = safe_atou64(hard, &hv);
                        אם (r < 0)
                                החזר r;
                }

                *ret = (بنية rlimit) {
                        .rlim_cur = sv,
                        .rlim_max = hv,
                };

                החזר 0;
        }

        החזר -ENOTRECOVERABLE;
}
