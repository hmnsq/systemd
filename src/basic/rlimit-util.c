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

صحیح setrlimit_closest(صحیح resource, ثابت ساختار rlimit *rlim) {
        ساختار rlimit highest, fixed;

        أكد(rlim);

        اگر (setrlimit(resource, rlim) >= 0)
                بازگشت 0;

        اگر (errno != EPERM)
                بازگشت -errno;

        /* So we failed to set the desired setrlimit, then let's try
         * to get as close as we can */
        اگر (getrlimit(resource, &highest) < 0)
                بازگشت -errno;

        /* If the hard limit is unbounded anyway, then the EPERM had other reasons, let's propagate the original EPERM
         * then */
        اگر (highest.rlim_max == RLIM_INFINITY)
                بازگشت -EPERM;

        fixed = (ساختار rlimit) {
                .rlim_cur = MIN(rlim->rlim_cur, highest.rlim_max),
                .rlim_max = MIN(rlim->rlim_max, highest.rlim_max),
        };

        /* Shortcut things if we wouldn't change anything. */
        اگر (fixed.rlim_cur == highest.rlim_cur &&
            fixed.rlim_max == highest.rlim_max)
                بازگشت 0;

        log_debug("Failed at setting rlimit " RLIM_FMT " for resource RLIMIT_%s. Will attempt setting value " RLIM_FMT " instead.", rlim->rlim_max, rlimit_to_string(resource), fixed.rlim_max);

        بازگشت RET_NERRNO(setrlimit(resource, &fixed));
}

صحیح setrlimit_closest_all(ثابت ساختار rlimit *ثابت *rlim, صحیح *which_failed) {
        صحیح r;

        أكد(rlim);

        /* On failure returns the limit's index that failed in *which_failed, but only if non-NULL */

        برای (صحیح i = 0; i < _RLIMIT_MAX; i++) {
                اگر (!rlim[i])
                        ادامه;

                r = setrlimit_closest(i, rlim[i]);
                اگر (r < 0) {
                        اگر (which_failed)
                                *which_failed = i;

                        بازگشت r;
                }
        }

        اگر (which_failed)
                *which_failed = -1;

        بازگشت 0;
}

ایستا صحیح rlimit_parse_u64(ثابت نویسه *val, rlim_t *ret) {
        uint64_t u;
        صحیح r;

        أكد(val);
        أكد(ret);

        اگر (streq(val, "infinity")) {
                *ret = RLIM_INFINITY;
                بازگشت 0;
        }

        /* setrlimit(2) suggests rlim_t is always 64-bit on Linux. */
        assert_cc(اندازه(rlim_t) == اندازه(uint64_t));

        r = safe_atou64(val, &u);
        اگر (r < 0)
                بازگشت r;
        اگر (u >= (uint64_t) RLIM_INFINITY)
                بازگشت -ERANGE;

        *ret = (rlim_t) u;
        بازگشت 0;
}

ایستا صحیح rlimit_parse_size(ثابت نویسه *val, rlim_t *ret) {
        uint64_t u;
        صحیح r;

        أكد(val);
        أكد(ret);

        اگر (streq(val, "infinity")) {
                *ret = RLIM_INFINITY;
                بازگشت 0;
        }

        r = parse_size(val, 1024, &u);
        اگر (r < 0)
                بازگشت r;
        اگر (u >= (uint64_t) RLIM_INFINITY)
                بازگشت -ERANGE;

        *ret = (rlim_t) u;
        بازگشت 0;
}

ایستا صحیح rlimit_parse_sec(ثابت نویسه *val, rlim_t *ret) {
        uint64_t u;
        usec_t t;
        صحیح r;

        أكد(val);
        أكد(ret);

        r = parse_sec(val, &t);
        اگر (r < 0)
                بازگشت r;
        اگر (t == USEC_INFINITY) {
                *ret = RLIM_INFINITY;
                بازگشت 0;
        }

        u = (uint64_t) DIV_ROUND_UP(t, USEC_PER_SEC);
        اگر (u >= (uint64_t) RLIM_INFINITY)
                بازگشت -ERANGE;

        *ret = (rlim_t) u;
        بازگشت 0;
}

ایستا صحیح rlimit_parse_usec(ثابت نویسه *val, rlim_t *ret) {
        usec_t t;
        صحیح r;

        أكد(val);
        أكد(ret);

        r = parse_time(val, &t, 1);
        اگر (r < 0)
                بازگشت r;
        اگر (t == USEC_INFINITY) {
                *ret = RLIM_INFINITY;
                بازگشت 0;
        }

        *ret = (rlim_t) t;
        بازگشت 0;
}

ایستا صحیح rlimit_parse_nice(ثابت نویسه *val, rlim_t *ret) {
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
                r = safe_atou64(val + 1, &rl);
                اگر (r < 0)
                        بازگشت r;

                اگر (rl >= PRIO_MAX)
                        بازگشت -ERANGE;

                rl = 20 - rl;

        } وگرنه اگر (val[0] == '-') {

                /* Prefixed with "-": Parse as negative user-friendly nice value */
                r = safe_atou64(val + 1, &rl);
                اگر (r < 0)
                        بازگشت r;

                اگر (rl > (uint64_t) (-PRIO_MIN))
                        بازگشت -ERANGE;

                rl = 20 + rl;
        } وگرنه {

                /* Not prefixed: parse as raw resource limit value */
                r = safe_atou64(val, &rl);
                اگر (r < 0)
                        بازگشت r;

                اگر (rl > (uint64_t) (20 - PRIO_MIN))
                        بازگشت -ERANGE;
        }

        *ret = (rlim_t) rl;
        بازگشت 0;
}

ایستا صحیح (*ثابت rlimit_parse_table[_RLIMIT_MAX])(ثابت نویسه *val, rlim_t *ret) = {
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

صحیح rlimit_parse_one(صحیح resource, ثابت نویسه *val, rlim_t *ret) {
        أكد(val);
        أكد(ret);

        اگر (resource < 0)
                بازگشت -EINVAL;
        اگر (resource >= _RLIMIT_MAX)
                بازگشت -EINVAL;

        بازگشت rlimit_parse_table[resource](val, ret);
}

صحیح rlimit_parse(صحیح resource, ثابت نویسه *val, ساختار rlimit *ret) {
        _cleanup_free_ نویسه *hard = پوچ, *soft = پوچ;
        rlim_t hl, sl;
        صحیح r;

        أكد(val);
        أكد(ret);

        r = extract_first_word(&val, &soft, ":", EXTRACT_DONT_COALESCE_SEPARATORS);
        اگر (r < 0)
                بازگشت r;
        اگر (r == 0)
                بازگشت -EINVAL;

        r = rlimit_parse_one(resource, soft, &sl);
        اگر (r < 0)
                بازگشت r;

        r = extract_first_word(&val, &hard, ":", EXTRACT_DONT_COALESCE_SEPARATORS);
        اگر (r < 0)
                بازگشت r;
        اگر (!isempty(val))
                بازگشت -EINVAL;
        اگر (r == 0)
                hl = sl;
        وگرنه {
                r = rlimit_parse_one(resource, hard, &hl);
                اگر (r < 0)
                        بازگشت r;
                اگر (sl > hl)
                        بازگشت -EILSEQ;
        }

        *ret = (ساختار rlimit) {
                .rlim_cur = sl,
                .rlim_max = hl,
        };

        بازگشت 0;
}

صحیح rlimit_format(ثابت ساختار rlimit *rl, نویسه **ret) {
        نویسه *s;

        أكد(rl);
        أكد(ret);

        اگر (rl->rlim_cur >= RLIM_INFINITY && rl->rlim_max >= RLIM_INFINITY)
                s = strdup("infinity");
        وگرنه اگر (rl->rlim_cur >= RLIM_INFINITY)
                s = asprintf_safe("infinity:" RLIM_FMT, rl->rlim_max);
        وگرنه اگر (rl->rlim_max >= RLIM_INFINITY)
                s = asprintf_safe(RLIM_FMT ":infinity", rl->rlim_cur);
        وگرنه اگر (rl->rlim_cur == rl->rlim_max)
                s = asprintf_safe(RLIM_FMT, rl->rlim_cur);
        وگرنه
                s = asprintf_safe(RLIM_FMT ":" RLIM_FMT, rl->rlim_cur, rl->rlim_max);
        اگر (!s)
                بازگشت -ENOMEM;

        *ret = s;
        بازگشت 0;
}

ایستا ثابت نویسه* ثابت rlimit_table[_RLIMIT_MAX] = {
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

DEFINE_STRING_TABLE_LOOKUP(rlimit, صحیح);

تهی rlimits_list(ثابت نویسه *prefix) {
        FOREACH_ELEMENT(field, rlimit_table)
                printf("%s%s\n", strempty(prefix), *field);
}

صحیح rlimit_from_string_harder(ثابت نویسه *s) {
        ثابت نویسه *suffix;

        /* The official prefix */
        suffix = startswith(s, "RLIMIT_");
        اگر (suffix)
                بازگشت rlimit_from_string(suffix);

        /* Our own unit file setting prefix */
        suffix = startswith(s, "Limit");
        اگر (suffix)
                بازگشت rlimit_from_string(suffix);

        بازگشت rlimit_from_string(s);
}

تهی rlimit_free_all(ساختار rlimit **rl) {
        free_many((تهی**) rl, _RLIMIT_MAX);
}

صحیح rlimit_copy_all(ساختار rlimit* target[ایستا _RLIMIT_MAX], ساختار rlimit* ثابت source[ایستا _RLIMIT_MAX]) {
        ساختار rlimit* copy[_RLIMIT_MAX] = {};

        أكد(target);
        أكد(source);

        برای (صحیح i = 0; i < _RLIMIT_MAX; i++) {
                اگر (!source[i])
                        ادامه;

                copy[i] = newdup(ساختار rlimit, source[i], 1);
                اگر (!copy[i]) {
                        rlimit_free_all(copy);
                        بازگشت -ENOMEM;
                }
        }

        memcpy(target, copy, اندازه(ساختار rlimit*) * _RLIMIT_MAX);
        بازگشت 0;
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
                بازگشت log_debug_errno(r, "Failed to set RLIMIT_NOFILE: %m");

        بازگشت 0;
}

صحیح rlimit_nofile_safe(تهی) {
        ساختار rlimit rl;

        /* Resets RLIMIT_NOFILE's soft limit FD_SETSIZE (i.e. 1024), for compatibility with software still using
         * select() */

        اگر (getrlimit(RLIMIT_NOFILE, &rl) < 0)
                بازگشت log_debug_errno(errno, "Failed to query RLIMIT_NOFILE: %m");

        اگر (rl.rlim_cur <= FD_SETSIZE)
                بازگشت 0;

        /* So we might have inherited a hard limit that's larger than the kernel's maximum limit as stored in
         * /proc/sys/fs/nr_open. If we pass this hard limit unmodified to setrlimit(), we'll get EPERM. To
         * make sure that doesn't happen, let's limit our hard limit to the value from nr_open. */
        rl.rlim_max = MIN(rl.rlim_max, (rlim_t) read_nr_open());
        rl.rlim_cur = MIN((rlim_t) FD_SETSIZE, rl.rlim_max);
        اگر (setrlimit(RLIMIT_NOFILE, &rl) < 0)
                بازگشت log_debug_errno(errno, "Failed to lower RLIMIT_NOFILE's soft limit to " RLIM_FMT ": %m", rl.rlim_cur);

        بازگشت 1;
}

صحیح pid_getrlimit(pid_t pid, صحیح resource, ساختار rlimit *ret) {

        ایستا ثابت نویسه * ثابت prefix_table[_RLIMIT_MAX] = {
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
                بازگشت RET_NERRNO(getrlimit(resource, ret));

        r = RET_NERRNO(prlimit(pid, resource, /* new_limit= */ پوچ, ret));
        اگر (!ERRNO_IS_NEG_PRIVILEGE(r))
                بازگشت r;

        /* We don't have access? Then try to go via /proc/$PID/limits. Weirdly that's world readable in
         * contrast to querying the data via prlimit() */

        ثابت نویسه *p = procfs_file_alloca(pid, "limits");
        _cleanup_free_ نویسه *limits = پوچ;

        r = read_full_file(p, &limits, /* ret_size= */ پوچ);
        اگر (r < 0)
                بازگشت -EPERM; /* propagate original permission error if we can't access the limits file */

        _cleanup_strv_free_ نویسه **l = پوچ;
        l = strv_split_newlines(limits);
        اگر (!l)
                بازگشت -ENOMEM;

        STRV_FOREACH(i, strv_skip(l, 1)) {
                _cleanup_free_ نویسه *soft = پوچ, *hard = پوچ;
                uint64_t sv, hv;
                ثابت نویسه *e;

                e = startswith(*i, prefix_table[resource]);
                اگر (!e)
                        ادامه;

                اگر (*e != ' ')
                        ادامه;

                e += strspn(e, WHITESPACE);

                size_t n;
                n = strcspn(e, WHITESPACE);
                اگر (n == 0)
                        ادامه;

                soft = strndup(e, n);
                اگر (!soft)
                        بازگشت -ENOMEM;

                e += n;
                اگر (*e != ' ')
                        ادامه;

                e += strspn(e, WHITESPACE);
                n = strcspn(e, WHITESPACE);
                اگر (n == 0)
                        ادامه;

                hard = strndup(e, n);
                اگر (!hard)
                        بازگشت -ENOMEM;

                اگر (streq(soft, "unlimited"))
                        sv = RLIM_INFINITY;
                وگرنه {
                        r = safe_atou64(soft, &sv);
                        اگر (r < 0)
                                بازگشت r;
                }

                اگر (streq(hard, "unlimited"))
                        hv = RLIM_INFINITY;
                وگرنه {
                        r = safe_atou64(hard, &hv);
                        اگر (r < 0)
                                بازگشت r;
                }

                *ret = (ساختار rlimit) {
                        .rlim_cur = sv,
                        .rlim_max = hv,
                };

                بازگشت 0;
        }

        بازگشت -ENOTRECOVERABLE;
}
