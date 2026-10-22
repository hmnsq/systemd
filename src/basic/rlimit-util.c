/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل "alloc-util.h"
#شامل "errno-util.h"
#شامل "extract-word.h"
#شامل "fd-util.h"
#شامل "fileio.h"
#شامل "format-util.h"
#شامل "log.h"
#شامل "parse-util.h"
#شامل "process-util.h"
#شامل "rlimit-util.h"
#شامل "string-table.h"
#شامل "strv.h"
#شامل "time-util.h"

صحیح setrlimit_closest(صحیح resource, ثابت ساخت rlimit *rlim) {
        ساخت rlimit highest, fixed;

        أكد(rlim);

        اگر (setrlimit(resource, rlim) >= 0)
                واپس 0;

        اگر (errno != EPERM)
                واپس -errno;

        /* So we failed to set the desired setrlimit, then let's try
         * to get as close as we can */
        اگر (getrlimit(resource, &highest) < 0)
                واپس -errno;

        /* If the hard limit is unbounded anyway, then the EPERM had other reasons, let's propagate the original EPERM
         * then */
        اگر (highest.rlim_max == RLIM_INFINITY)
                واپس -EPERM;

        fixed = (ساخت rlimit) {
                .rlim_cur = MIN(rlim->rlim_cur, highest.rlim_max),
                .rlim_max = MIN(rlim->rlim_max, highest.rlim_max),
        };

        /* Shortcut things if we wouldn't change anything. */
        اگر (fixed.rlim_cur == highest.rlim_cur &&
            fixed.rlim_max == highest.rlim_max)
                واپس 0;

        log_debug("Failed at setting rlimit " RLIM_FMT " for resource RLIMIT_%s. Will attempt setting value " RLIM_FMT " instead.", rlim->rlim_max, rlimit_مقصد_string(resource), fixed.rlim_max);

        واپس RET_NERRNO(setrlimit(resource, &fixed));
}

صحیح setrlimit_closest_all(ثابت ساخت rlimit *ثابت *rlim, صحیح *which_failed) {
        صحیح r;

        أكد(rlim);

        /* On failure returns the limit's index that failed in *which_failed, but only if non-NULL */

        برائے (صحیح i = 0; i < _RLIMIT_MAX; i++) {
                اگر (!rlim[i])
                        جاری;

                r = setrlimit_closest(i, rlim[i]);
                اگر (r < 0) {
                        اگر (which_failed)
                                *which_failed = i;

                        واپس r;
                }
        }

        اگر (which_failed)
                *which_failed = -1;

        واپس 0;
}

ساکن صحیح rlimit_parse_u64(ثابت حرف *val, rlim_t *ret) {
        uint64_t u;
        صحیح r;

        أكد(val);
        أكد(ret);

        اگر (streq(val, "infinity")) {
                *ret = RLIM_INFINITY;
                واپس 0;
        }

        /* setrlimit(2) suggests rlim_t is always 64-bit on Linux. */
        assert_cc(حجم(rlim_t) == حجم(uint64_t));

        r = محفوظ_atou64(val, &u);
        اگر (r < 0)
                واپس r;
        اگر (u >= (uint64_t) RLIM_INFINITY)
                واپس -ERANGE;

        *ret = (rlim_t) u;
        واپس 0;
}

ساکن صحیح rlimit_parse_سائز(ثابت حرف *val, rlim_t *ret) {
        uint64_t u;
        صحیح r;

        أكد(val);
        أكد(ret);

        اگر (streq(val, "infinity")) {
                *ret = RLIM_INFINITY;
                واپس 0;
        }

        r = parse_سائز(val, 1024, &u);
        اگر (r < 0)
                واپس r;
        اگر (u >= (uint64_t) RLIM_INFINITY)
                واپس -ERANGE;

        *ret = (rlim_t) u;
        واپس 0;
}

ساکن صحیح rlimit_parse_sec(ثابت حرف *val, rlim_t *ret) {
        uint64_t u;
        usec_t t;
        صحیح r;

        أكد(val);
        أكد(ret);

        r = parse_sec(val, &t);
        اگر (r < 0)
                واپس r;
        اگر (t == USEC_INFINITY) {
                *ret = RLIM_INFINITY;
                واپس 0;
        }

        u = (uint64_t) DIV_باری_اوپر(t, USEC_فی_SEC);
        اگر (u >= (uint64_t) RLIM_INFINITY)
                واپس -ERANGE;

        *ret = (rlim_t) u;
        واپس 0;
}

ساکن صحیح rlimit_parse_usec(ثابت حرف *val, rlim_t *ret) {
        usec_t t;
        صحیح r;

        أكد(val);
        أكد(ret);

        r = parse_وقت(val, &t, 1);
        اگر (r < 0)
                واپس r;
        اگر (t == USEC_INFINITY) {
                *ret = RLIM_INFINITY;
                واپس 0;
        }

        *ret = (rlim_t) t;
        واپس 0;
}

ساکن صحیح rlimit_parse_nice(ثابت حرف *val, rlim_t *ret) {
        uint64_t rl;
        صحیح r;

        أكد(val);
        أكد(ret);

        /* So, Linux is weird. The range for RLIMIT_NICE is 40..1, mapping to the nice levels -20..19. However, the
         * RLIMIT_NICE limit defaults to 0 by the kernel, i.e. a value that maps to nice level 20, which of course is
         * bogus and does not exist. In order to permit parsing the RLIMIT_NICE of 0 here we hence implement a slight
         * asymmetry: when parsing as positive nice level we permit 0..19. When parsing as negative nice level, we
         * permit -20..0. But when parsing as raw resource limit value then we also allow the special value 0.
         *
         * Yeah, Linux is quality engineering sometimes... */

        اگر (val[0] == '+') {

                /* Prefixed with "+": Parse as positive user-friendly nice value */
                r = محفوظ_atou64(val + 1, &rl);
                اگر (r < 0)
                        واپس r;

                اگر (rl >= PRIO_MAX)
                        واپس -ERANGE;

                rl = 20 - rl;

        } ورنہ اگر (val[0] == '-') {

                /* Prefixed with "-": Parse as negative user-friendly nice value */
                r = محفوظ_atou64(val + 1, &rl);
                اگر (r < 0)
                        واپس r;

                اگر (rl > (uint64_t) (-PRIO_MIN))
                        واپس -ERANGE;

                rl = 20 + rl;
        } ورنہ {

                /* Not prefixed: parse as raw resource limit value */
                r = محفوظ_atou64(val, &rl);
                اگر (r < 0)
                        واپس r;

                اگر (rl > (uint64_t) (20 - PRIO_MIN))
                        واپس -ERANGE;
        }

        *ret = (rlim_t) rl;
        واپس 0;
}

ساکن صحیح (*ثابت rlimit_parse_جدول[_RLIMIT_MAX])(ثابت حرف *val, rlim_t *ret) = {
        [RLIMIT_CPU]        = rlimit_parse_sec,
        [RLIMIT_FSIZE]      = rlimit_parse_سائز,
        [RLIMIT_DATA]       = rlimit_parse_سائز,
        [RLIMIT_STACK]      = rlimit_parse_سائز,
        [RLIMIT_CORE]       = rlimit_parse_سائز,
        [RLIMIT_RSS]        = rlimit_parse_سائز,
        [RLIMIT_NOFILE]     = rlimit_parse_u64,
        [RLIMIT_AS]         = rlimit_parse_سائز,
        [RLIMIT_NPROC]      = rlimit_parse_u64,
        [RLIMIT_MEMLOCK]    = rlimit_parse_سائز,
        [RLIMIT_LOCKS]      = rlimit_parse_u64,
        [RLIMIT_SIGPENDING] = rlimit_parse_u64,
        [RLIMIT_MSGQUEUE]   = rlimit_parse_سائز,
        [RLIMIT_NICE]       = rlimit_parse_nice,
        [RLIMIT_RTPRIO]     = rlimit_parse_u64,
        [RLIMIT_RTTIME]     = rlimit_parse_usec,
};

صحیح rlimit_parse_one(صحیح resource, ثابت حرف *val, rlim_t *ret) {
        أكد(val);
        أكد(ret);

        اگر (resource < 0)
                واپس -EINVAL;
        اگر (resource >= _RLIMIT_MAX)
                واپس -EINVAL;

        واپس rlimit_parse_جدول[resource](val, ret);
}

صحیح rlimit_parse(صحیح resource, ثابت حرف *val, ساخت rlimit *ret) {
        _cleanup_free_ حرف *hard = تہی, *soft = تہی;
        rlim_t hl, sl;
        صحیح r;

        أكد(val);
        أكد(ret);

        r = نکالو_پہلا_لفظ(&val, &soft, ":", نکالو_DONT_COALESCE_SEPARATORS);
        اگر (r < 0)
                واپس r;
        اگر (r == 0)
                واپس -EINVAL;

        r = rlimit_parse_one(resource, soft, &sl);
        اگر (r < 0)
                واپس r;

        r = نکالو_پہلا_لفظ(&val, &hard, ":", نکالو_DONT_COALESCE_SEPARATORS);
        اگر (r < 0)
                واپس r;
        اگر (!isempty(val))
                واپس -EINVAL;
        اگر (r == 0)
                hl = sl;
        ورنہ {
                r = rlimit_parse_one(resource, hard, &hl);
                اگر (r < 0)
                        واپس r;
                اگر (sl > hl)
                        واپس -EILSEQ;
        }

        *ret = (ساخت rlimit) {
                .rlim_cur = sl,
                .rlim_max = hl,
        };

        واپس 0;
}

صحیح rlimit_format(ثابت ساخت rlimit *rl, حرف **ret) {
        حرف *s;

        أكد(rl);
        أكد(ret);

        اگر (rl->rlim_cur >= RLIM_INFINITY && rl->rlim_max >= RLIM_INFINITY)
                s = strdup("infinity");
        ورنہ اگر (rl->rlim_cur >= RLIM_INFINITY)
                s = asprintf_محفوظ("infinity:" RLIM_FMT, rl->rlim_max);
        ورنہ اگر (rl->rlim_max >= RLIM_INFINITY)
                s = asprintf_محفوظ(RLIM_FMT ":infinity", rl->rlim_cur);
        ورنہ اگر (rl->rlim_cur == rl->rlim_max)
                s = asprintf_محفوظ(RLIM_FMT, rl->rlim_cur);
        ورنہ
                s = asprintf_محفوظ(RLIM_FMT ":" RLIM_FMT, rl->rlim_cur, rl->rlim_max);
        اگر (!s)
                واپس -ENOMEM;

        *ret = s;
        واپس 0;
}

ساکن ثابت حرف* ثابت rlimit_جدول[_RLIMIT_MAX] = {
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

DEFINE_STRING_جدول_LOOKUP(rlimit, صحیح);

خالی rlimits_list(ثابت حرف *سابقہ) {
        FOREACH_ELEMENT(میدان, rlimit_جدول)
                لکھو("%s%s\n", strempty(سابقہ), *میدان);
}

صحیح rlimit_from_string_harder(ثابت حرف *s) {
        ثابت حرف *suffix;

        /* The official prefix */
        suffix = startswith(s, "RLIMIT_");
        اگر (suffix)
                واپس rlimit_from_string(suffix);

        /* Our own unit file setting prefix */
        suffix = startswith(s, "Limit");
        اگر (suffix)
                واپس rlimit_from_string(suffix);

        واپس rlimit_from_string(s);
}

خالی rlimit_free_all(ساخت rlimit **rl) {
        free_many((خالی**) rl, _RLIMIT_MAX);
}

صحیح rlimit_copy_all(ساخت rlimit* ہدف[ساکن _RLIMIT_MAX], ساخت rlimit* ثابت ماخذ[ساکن _RLIMIT_MAX]) {
        ساخت rlimit* copy[_RLIMIT_MAX] = {};

        أكد(ہدف);
        أكد(ماخذ);

        برائے (صحیح i = 0; i < _RLIMIT_MAX; i++) {
                اگر (!ماخذ[i])
                        جاری;

                copy[i] = newdup(ساخت rlimit, ماخذ[i], 1);
                اگر (!copy[i]) {
                        rlimit_free_all(copy);
                        واپس -ENOMEM;
                }
        }

        حافظہ_نقل(ہدف, copy, حجم(ساخت rlimit*) * _RLIMIT_MAX);
        واپس 0;
}

صحیح rlimit_nofile_bump(صحیح limit) {
        صحیح r;

        /* Bumps the (soft) RLIMIT_NOFILE resource limit as close as possible to the specified limit. If a negative
         * limit is specified, bumps it to the maximum the kernel and the hard resource limit allows. This call should
         * be used by all our programs that might need a lot of fds, and that know how to deal with high fd numbers
         * (i.e. do not use select() — which chokes on fds >= 1024) */

        اگر (limit < 0)
                limit = read_nr_open();

        اگر (limit < 3)
                limit = 3;

        r = setrlimit_closest(RLIMIT_NOFILE, &RLIMIT_MAKE_CONST(limit));
        اگر (r < 0)
                واپس log_debug_errno(r, "Failed to set RLIMIT_NOFILE: %m");

        واپس 0;
}

صحیح rlimit_nofile_محفوظ(خالی) {
        ساخت rlimit rl;

        /* Resets RLIMIT_NOFILE's soft limit FD_SETSIZE (i.e. 1024), for compatibility with software still using
         * select() */

        اگر (getrlimit(RLIMIT_NOFILE, &rl) < 0)
                واپس log_debug_errno(errno, "Failed to query RLIMIT_NOFILE: %m");

        اگر (rl.rlim_cur <= FD_SETSIZE)
                واپس 0;

        /* So we might have inherited a hard limit that's larger than the kernel's maximum limit as stored in
         * /proc/sys/fs/nr_open. If we pass this hard limit unmodified to setrlimit(), we'll get EPERM. To
         * make sure that doesn't happen, let's limit our hard limit to the value from nr_open. */
        rl.rlim_max = MIN(rl.rlim_max, (rlim_t) read_nr_open());
        rl.rlim_cur = MIN((rlim_t) FD_SETSIZE, rl.rlim_max);
        اگر (setrlimit(RLIMIT_NOFILE, &rl) < 0)
                واپس log_debug_errno(errno, "Failed to lower RLIMIT_NOFILE's soft limit to " RLIM_FMT ": %m", rl.rlim_cur);

        واپس 1;
}

صحیح pid_getrlimit(pid_t pid, صحیح resource, ساخت rlimit *ret) {

        ساکن ثابت حرف * ثابت سابقہ_جدول[_RLIMIT_MAX] = {
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

        صحیح r;

        أكد(resource >= 0);
        أكد(resource < _RLIMIT_MAX);
        أكد(pid >= 0);
        أكد(ret);

        اگر (pid == 0 || pid == getpid_cached())
                واپس RET_NERRNO(getrlimit(resource, ret));

        r = RET_NERRNO(prlimit(pid, resource, /* new_limit= */ تہی, ret));
        اگر (!ERRNO_IS_NEG_PRIVILEGE(r))
                واپس r;

        /* We don't have access? Then try to go via /proc/$PID/limits. Weirdly that's world readable in
         * contrast to querying the data via prlimit() */

        ثابت حرف *p = procfs_file_alloca(pid, "limits");
        _cleanup_free_ حرف *limits = تہی;

        r = read_بھرا_file(p, &limits, /* ret_size= */ تہی);
        اگر (r < 0)
                واپس -EPERM; /* propagate original permission error if we can't access the limits file */

        _cleanup_strv_free_ حرف **l = تہی;
        l = strv_بانٹو_newlines(limits);
        اگر (!l)
                واپس -ENOMEM;

        STRV_FOREACH(i, strv_skip(l, 1)) {
                _cleanup_free_ حرف *soft = تہی, *hard = تہی;
                uint64_t sv, hv;
                ثابت حرف *e;

                e = startswith(*i, سابقہ_جدول[resource]);
                اگر (!e)
                        جاری;

                اگر (*e != ' ')
                        جاری;

                e += strspn(e, WHITESPACE);

                size_t n;
                n = strcspn(e, WHITESPACE);
                اگر (n == 0)
                        جاری;

                soft = strndup(e, n);
                اگر (!soft)
                        واپس -ENOMEM;

                e += n;
                اگر (*e != ' ')
                        جاری;

                e += strspn(e, WHITESPACE);
                n = strcspn(e, WHITESPACE);
                اگر (n == 0)
                        جاری;

                hard = strndup(e, n);
                اگر (!hard)
                        واپس -ENOMEM;

                اگر (streq(soft, "unlimited"))
                        sv = RLIM_INFINITY;
                ورنہ {
                        r = محفوظ_atou64(soft, &sv);
                        اگر (r < 0)
                                واپس r;
                }

                اگر (streq(hard, "unlimited"))
                        hv = RLIM_INFINITY;
                ورنہ {
                        r = محفوظ_atou64(hard, &hv);
                        اگر (r < 0)
                                واپس r;
                }

                *ret = (ساخت rlimit) {
                        .rlim_cur = sv,
                        .rlim_max = hv,
                };

                واپس 0;
        }

        واپس -ENOTRECOVERABLE;
}
