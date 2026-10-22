/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব "alloc-util.h"
#অন্তর্ভাব "errno-util.h"
#অন্তর্ভাব "extract-word.h"
#অন্তর্ভাব "fd-util.h"
#অন্তর্ভাব "fileio.h"
#অন্তর্ভাব "format-util.h"
#অন্তর্ভাব "log.h"
#অন্তর্ভাব "parse-util.h"
#অন্তর্ভাব "process-util.h"
#অন্তর্ভাব "rlimit-util.h"
#অন্তর্ভাব "string-table.h"
#অন্তর্ভাব "strv.h"
#অন্তর্ভাব "time-util.h"

পূর্ণসংখ্যা setrlimit_closest(পূর্ণসংখ্যা resource, ধ্রুবক কাঠামো rlimit *rlim) {
        কাঠামো rlimit highest, fixed;

        প্রতিজ্ঞা(rlim);

        যদি (setrlimit(resource, rlim) >= 0)
                ফেরত 0;

        যদি (errno != EPERM)
                ফেরত -errno;

        /* So we failed to set the desired setrlimit, then let's try
         * to get as close as we can */
        যদি (getrlimit(resource, &highest) < 0)
                ফেরত -errno;

        /* If the hard limit is unbounded anyway, then the EPERM had other reasons, let's propagate the original EPERM
         * then */
        যদি (highest.rlim_max == RLIM_INFINITY)
                ফেরত -EPERM;

        fixed = (কাঠামো rlimit) {
                .rlim_cur = MIN(rlim->rlim_cur, highest.rlim_max),
                .rlim_max = MIN(rlim->rlim_max, highest.rlim_max),
        };

        /* Shortcut things if we wouldn't change anything. */
        যদি (fixed.rlim_cur == highest.rlim_cur &&
            fixed.rlim_max == highest.rlim_max)
                ফেরত 0;

        log_debug("Failed at setting rlimit " RLIM_FMT " for resource RLIMIT_%s. Will attempt setting value " RLIM_FMT " instead.", rlim->rlim_max, rlimit_to_string(resource), fixed.rlim_max);

        ফেরত RET_NERRNO(setrlimit(resource, &fixed));
}

পূর্ণসংখ্যা setrlimit_closest_all(ধ্রুবক কাঠামো rlimit *ধ্রুবক *rlim, পূর্ণসংখ্যা *which_failed) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(rlim);

        /* On failure returns the limit's index that failed in *which_failed, but only if non-NULL */

        জন্য (পূর্ণসংখ্যা i = 0; i < _RLIMIT_MAX; i++) {
                যদি (!rlim[i])
                        চালু;

                r = setrlimit_closest(i, rlim[i]);
                যদি (r < 0) {
                        যদি (which_failed)
                                *which_failed = i;

                        ফেরত r;
                }
        }

        যদি (which_failed)
                *which_failed = -1;

        ফেরত 0;
}

স্থির পূর্ণসংখ্যা rlimit_parse_u64(ধ্রুবক অক্ষর *val, rlim_t *ret) {
        uint64_t u;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(val);
        প্রতিজ্ঞা(ret);

        যদি (streq(val, "infinity")) {
                *ret = RLIM_INFINITY;
                ফেরত 0;
        }

        /* setrlimit(2) suggests rlim_t is always 64-bit on Linux. */
        assert_cc(আকার(rlim_t) == আকার(uint64_t));

        r = safe_atou64(val, &u);
        যদি (r < 0)
                ফেরত r;
        যদি (u >= (uint64_t) RLIM_INFINITY)
                ফেরত -ERANGE;

        *ret = (rlim_t) u;
        ফেরত 0;
}

স্থির পূর্ণসংখ্যা rlimit_parse_আকার(ধ্রুবক অক্ষর *val, rlim_t *ret) {
        uint64_t u;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(val);
        প্রতিজ্ঞা(ret);

        যদি (streq(val, "infinity")) {
                *ret = RLIM_INFINITY;
                ফেরত 0;
        }

        r = parse_আকার(val, 1024, &u);
        যদি (r < 0)
                ফেরত r;
        যদি (u >= (uint64_t) RLIM_INFINITY)
                ফেরত -ERANGE;

        *ret = (rlim_t) u;
        ফেরত 0;
}

স্থির পূর্ণসংখ্যা rlimit_parse_sec(ধ্রুবক অক্ষর *val, rlim_t *ret) {
        uint64_t u;
        usec_t t;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(val);
        প্রতিজ্ঞা(ret);

        r = parse_sec(val, &t);
        যদি (r < 0)
                ফেরত r;
        যদি (t == USEC_INFINITY) {
                *ret = RLIM_INFINITY;
                ফেরত 0;
        }

        u = (uint64_t) DIV_ROUND_UP(t, USEC_PER_SEC);
        যদি (u >= (uint64_t) RLIM_INFINITY)
                ফেরত -ERANGE;

        *ret = (rlim_t) u;
        ফেরত 0;
}

স্থির পূর্ণসংখ্যা rlimit_parse_usec(ধ্রুবক অক্ষর *val, rlim_t *ret) {
        usec_t t;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(val);
        প্রতিজ্ঞা(ret);

        r = parse_time(val, &t, 1);
        যদি (r < 0)
                ফেরত r;
        যদি (t == USEC_INFINITY) {
                *ret = RLIM_INFINITY;
                ফেরত 0;
        }

        *ret = (rlim_t) t;
        ফেরত 0;
}

স্থির পূর্ণসংখ্যা rlimit_parse_nice(ধ্রুবক অক্ষর *val, rlim_t *ret) {
        uint64_t rl;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(val);
        প্রতিজ্ঞা(ret);

        /* So, Linux is weird. The range for RLIMIT_NICE is 40..1, mapping to the nice levels -20..19. However, the
         * RLIMIT_NICE limit defaults to 0 by the kernel, i.e. a value that maps to nice level 20, which of course is
         * bogus and does not exist. In order to permit parsing the RLIMIT_NICE of 0 here we hence implement a slight
         * asymmetry: when parsing as positive nice level we permit 0..19. When parsing as negative nice level, we
         * permit -20..0. But when parsing as raw resource limit value then we also allow the special value 0.
         *
         * Yeah, Linux is quality engineering sometimes... */

        যদি (val[0] == '+') {

                /* Prefixed with "+": Parse as positive user-friendly nice value */
                r = safe_atou64(val + 1, &rl);
                যদি (r < 0)
                        ফেরত r;

                যদি (rl >= PRIO_MAX)
                        ফেরত -ERANGE;

                rl = 20 - rl;

        } অন্যথা যদি (val[0] == '-') {

                /* Prefixed with "-": Parse as negative user-friendly nice value */
                r = safe_atou64(val + 1, &rl);
                যদি (r < 0)
                        ফেরত r;

                যদি (rl > (uint64_t) (-PRIO_MIN))
                        ফেরত -ERANGE;

                rl = 20 + rl;
        } অন্যথা {

                /* Not prefixed: parse as raw resource limit value */
                r = safe_atou64(val, &rl);
                যদি (r < 0)
                        ফেরত r;

                যদি (rl > (uint64_t) (20 - PRIO_MIN))
                        ফেরত -ERANGE;
        }

        *ret = (rlim_t) rl;
        ফেরত 0;
}

স্থির পূর্ণসংখ্যা (*ধ্রুবক rlimit_parse_table[_RLIMIT_MAX])(ধ্রুবক অক্ষর *val, rlim_t *ret) = {
        [RLIMIT_CPU]        = rlimit_parse_sec,
        [RLIMIT_FSIZE]      = rlimit_parse_আকার,
        [RLIMIT_DATA]       = rlimit_parse_আকার,
        [RLIMIT_STACK]      = rlimit_parse_আকার,
        [RLIMIT_CORE]       = rlimit_parse_আকার,
        [RLIMIT_RSS]        = rlimit_parse_আকার,
        [RLIMIT_NOFILE]     = rlimit_parse_u64,
        [RLIMIT_AS]         = rlimit_parse_আকার,
        [RLIMIT_NPROC]      = rlimit_parse_u64,
        [RLIMIT_MEMLOCK]    = rlimit_parse_আকার,
        [RLIMIT_LOCKS]      = rlimit_parse_u64,
        [RLIMIT_SIGPENDING] = rlimit_parse_u64,
        [RLIMIT_MSGQUEUE]   = rlimit_parse_আকার,
        [RLIMIT_NICE]       = rlimit_parse_nice,
        [RLIMIT_RTPRIO]     = rlimit_parse_u64,
        [RLIMIT_RTTIME]     = rlimit_parse_usec,
};

পূর্ণসংখ্যা rlimit_parse_one(পূর্ণসংখ্যা resource, ধ্রুবক অক্ষর *val, rlim_t *ret) {
        প্রতিজ্ঞা(val);
        প্রতিজ্ঞা(ret);

        যদি (resource < 0)
                ফেরত -EINVAL;
        যদি (resource >= _RLIMIT_MAX)
                ফেরত -EINVAL;

        ফেরত rlimit_parse_table[resource](val, ret);
}

পূর্ণসংখ্যা rlimit_parse(পূর্ণসংখ্যা resource, ধ্রুবক অক্ষর *val, কাঠামো rlimit *ret) {
        _cleanup_free_ অক্ষর *hard = ফাঁকা, *soft = ফাঁকা;
        rlim_t hl, sl;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(val);
        প্রতিজ্ঞা(ret);

        r = extract_first_word(&val, &soft, ":", EXTRACT_DONT_COALESCE_SEPARATORS);
        যদি (r < 0)
                ফেরত r;
        যদি (r == 0)
                ফেরত -EINVAL;

        r = rlimit_parse_one(resource, soft, &sl);
        যদি (r < 0)
                ফেরত r;

        r = extract_first_word(&val, &hard, ":", EXTRACT_DONT_COALESCE_SEPARATORS);
        যদি (r < 0)
                ফেরত r;
        যদি (!isempty(val))
                ফেরত -EINVAL;
        যদি (r == 0)
                hl = sl;
        অন্যথা {
                r = rlimit_parse_one(resource, hard, &hl);
                যদি (r < 0)
                        ফেরত r;
                যদি (sl > hl)
                        ফেরত -EILSEQ;
        }

        *ret = (কাঠামো rlimit) {
                .rlim_cur = sl,
                .rlim_max = hl,
        };

        ফেরত 0;
}

পূর্ণসংখ্যা rlimit_format(ধ্রুবক কাঠামো rlimit *rl, অক্ষর **ret) {
        অক্ষর *s;

        প্রতিজ্ঞা(rl);
        প্রতিজ্ঞা(ret);

        যদি (rl->rlim_cur >= RLIM_INFINITY && rl->rlim_max >= RLIM_INFINITY)
                s = strdup("infinity");
        অন্যথা যদি (rl->rlim_cur >= RLIM_INFINITY)
                s = asprintf_safe("infinity:" RLIM_FMT, rl->rlim_max);
        অন্যথা যদি (rl->rlim_max >= RLIM_INFINITY)
                s = asprintf_safe(RLIM_FMT ":infinity", rl->rlim_cur);
        অন্যথা যদি (rl->rlim_cur == rl->rlim_max)
                s = asprintf_safe(RLIM_FMT, rl->rlim_cur);
        অন্যথা
                s = asprintf_safe(RLIM_FMT ":" RLIM_FMT, rl->rlim_cur, rl->rlim_max);
        যদি (!s)
                ফেরত -ENOMEM;

        *ret = s;
        ফেরত 0;
}

স্থির ধ্রুবক অক্ষর* ধ্রুবক rlimit_table[_RLIMIT_MAX] = {
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

DEFINE_STRING_TABLE_LOOKUP(rlimit, পূর্ণসংখ্যা);

শূন্য rlimits_list(ধ্রুবক অক্ষর *prefix) {
        FOREACH_ELEMENT(field, rlimit_table)
                printf("%s%s\n", strempty(prefix), *field);
}

পূর্ণসংখ্যা rlimit_from_string_harder(ধ্রুবক অক্ষর *s) {
        ধ্রুবক অক্ষর *suffix;

        /* The official prefix */
        suffix = startswith(s, "RLIMIT_");
        যদি (suffix)
                ফেরত rlimit_from_string(suffix);

        /* Our own unit file setting prefix */
        suffix = startswith(s, "Limit");
        যদি (suffix)
                ফেরত rlimit_from_string(suffix);

        ফেরত rlimit_from_string(s);
}

শূন্য rlimit_free_all(কাঠামো rlimit **rl) {
        free_many((শূন্য**) rl, _RLIMIT_MAX);
}

পূর্ণসংখ্যা rlimit_copy_all(কাঠামো rlimit* target[স্থির _RLIMIT_MAX], কাঠামো rlimit* ধ্রুবক source[স্থির _RLIMIT_MAX]) {
        কাঠামো rlimit* copy[_RLIMIT_MAX] = {};

        প্রতিজ্ঞা(target);
        প্রতিজ্ঞা(source);

        জন্য (পূর্ণসংখ্যা i = 0; i < _RLIMIT_MAX; i++) {
                যদি (!source[i])
                        চালু;

                copy[i] = newdup(কাঠামো rlimit, source[i], 1);
                যদি (!copy[i]) {
                        rlimit_free_all(copy);
                        ফেরত -ENOMEM;
                }
        }

        memcpy(target, copy, আকার(কাঠামো rlimit*) * _RLIMIT_MAX);
        ফেরত 0;
}

পূর্ণসংখ্যা rlimit_nofile_bump(পূর্ণসংখ্যা limit) {
        পূর্ণসংখ্যা r;

        /* Bumps the (soft) RLIMIT_NOFILE resource limit as close as possible to the specified limit. If a negative
         * limit is specified, bumps it to the maximum the kernel and the hard resource limit allows. This call should
         * be used by all our programs that might need a lot of fds, and that know how to deal with high fd numbers
         * (i.e. do not use select() — which chokes on fds >= 1024) */

        যদি (limit < 0)
                limit = read_nr_open();

        যদি (limit < 3)
                limit = 3;

        r = setrlimit_closest(RLIMIT_NOFILE, &RLIMIT_MAKE_CONST(limit));
        যদি (r < 0)
                ফেরত log_debug_errno(r, "Failed to set RLIMIT_NOFILE: %m");

        ফেরত 0;
}

পূর্ণসংখ্যা rlimit_nofile_safe(শূন্য) {
        কাঠামো rlimit rl;

        /* Resets RLIMIT_NOFILE's soft limit FD_SETSIZE (i.e. 1024), for compatibility with software still using
         * select() */

        যদি (getrlimit(RLIMIT_NOFILE, &rl) < 0)
                ফেরত log_debug_errno(errno, "Failed to query RLIMIT_NOFILE: %m");

        যদি (rl.rlim_cur <= FD_SETSIZE)
                ফেরত 0;

        /* So we might have inherited a hard limit that's larger than the kernel's maximum limit as stored in
         * /proc/sys/fs/nr_open. If we pass this hard limit unmodified to setrlimit(), we'll get EPERM. To
         * make sure that doesn't happen, let's limit our hard limit to the value from nr_open. */
        rl.rlim_max = MIN(rl.rlim_max, (rlim_t) read_nr_open());
        rl.rlim_cur = MIN((rlim_t) FD_SETSIZE, rl.rlim_max);
        যদি (setrlimit(RLIMIT_NOFILE, &rl) < 0)
                ফেরত log_debug_errno(errno, "Failed to lower RLIMIT_NOFILE's soft limit to " RLIM_FMT ": %m", rl.rlim_cur);

        ফেরত 1;
}

পূর্ণসংখ্যা pid_getrlimit(pid_t pid, পূর্ণসংখ্যা resource, কাঠামো rlimit *ret) {

        স্থির ধ্রুবক অক্ষর * ধ্রুবক prefix_table[_RLIMIT_MAX] = {
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

        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(resource >= 0);
        প্রতিজ্ঞা(resource < _RLIMIT_MAX);
        প্রতিজ্ঞা(pid >= 0);
        প্রতিজ্ঞা(ret);

        যদি (pid == 0 || pid == getpid_cached())
                ফেরত RET_NERRNO(getrlimit(resource, ret));

        r = RET_NERRNO(prlimit(pid, resource, /* new_limit= */ ফাঁকা, ret));
        যদি (!ERRNO_IS_NEG_PRIVILEGE(r))
                ফেরত r;

        /* We don't have access? Then try to go via /proc/$PID/limits. Weirdly that's world readable in
         * contrast to querying the data via prlimit() */

        ধ্রুবক অক্ষর *p = procfs_file_alloca(pid, "limits");
        _cleanup_free_ অক্ষর *limits = ফাঁকা;

        r = read_full_file(p, &limits, /* ret_size= */ ফাঁকা);
        যদি (r < 0)
                ফেরত -EPERM; /* propagate original permission error if we can't access the limits file */

        _cleanup_strv_free_ অক্ষর **l = ফাঁকা;
        l = strv_split_newlines(limits);
        যদি (!l)
                ফেরত -ENOMEM;

        STRV_FOREACH(i, strv_skip(l, 1)) {
                _cleanup_free_ অক্ষর *soft = ফাঁকা, *hard = ফাঁকা;
                uint64_t sv, hv;
                ধ্রুবক অক্ষর *e;

                e = startswith(*i, prefix_table[resource]);
                যদি (!e)
                        চালু;

                যদি (*e != ' ')
                        চালু;

                e += strspn(e, WHITESPACE);

                size_t n;
                n = strcspn(e, WHITESPACE);
                যদি (n == 0)
                        চালু;

                soft = strndup(e, n);
                যদি (!soft)
                        ফেরত -ENOMEM;

                e += n;
                যদি (*e != ' ')
                        চালু;

                e += strspn(e, WHITESPACE);
                n = strcspn(e, WHITESPACE);
                যদি (n == 0)
                        চালু;

                hard = strndup(e, n);
                যদি (!hard)
                        ফেরত -ENOMEM;

                যদি (streq(soft, "unlimited"))
                        sv = RLIM_INFINITY;
                অন্যথা {
                        r = safe_atou64(soft, &sv);
                        যদি (r < 0)
                                ফেরত r;
                }

                যদি (streq(hard, "unlimited"))
                        hv = RLIM_INFINITY;
                অন্যথা {
                        r = safe_atou64(hard, &hv);
                        যদি (r < 0)
                                ফেরত r;
                }

                *ret = (কাঠামো rlimit) {
                        .rlim_cur = sv,
                        .rlim_max = hv,
                };

                ফেরত 0;
        }

        ফেরত -ENOTRECOVERABLE;
}
