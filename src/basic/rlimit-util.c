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

পূর্ণাঙ্ক setrlimit_closest(পূর্ণাঙ্ক resource, ধ্রুব সংরচনা rlimit *rlim) {
        সংরচনা rlimit highest, fixed;

        প্রতিজ্ঞা(rlim);

        যদি (setrlimit(resource, rlim) >= 0)
                প্রত্যাগম 0;

        যদি (errno != EPERM)
                প্রত্যাগম -errno;

        /* So we failed to set the desired setrlimit, then let's try
         * to get as close as we can */
        যদি (getrlimit(resource, &highest) < 0)
                প্রত্যাগম -errno;

        /* If the hard limit is unbounded anyway, then the EPERM had other reasons, let's propagate the original EPERM
         * then */
        যদি (highest.rlim_max == RLIM_INFINITY)
                প্রত্যাগম -EPERM;

        fixed = (সংরচনা rlimit) {
                .rlim_cur = MIN(rlim->rlim_cur, highest.rlim_max),
                .rlim_max = MIN(rlim->rlim_max, highest.rlim_max),
        };

        /* Shortcut things if we wouldn't change anything. */
        যদি (fixed.rlim_cur == highest.rlim_cur &&
            fixed.rlim_max == highest.rlim_max)
                প্রত্যাগম 0;

        log_debug("Failed at setting rlimit " RLIM_FMT " for resource RLIMIT_%s. Will attempt setting value " RLIM_FMT " instead.", rlim->rlim_max, rlimit_to_string(resource), fixed.rlim_max);

        প্রত্যাগম RET_NERRNO(setrlimit(resource, &fixed));
}

পূর্ণাঙ্ক setrlimit_closest_all(ধ্রুব সংরচনা rlimit *ধ্রুব *rlim, পূর্ণাঙ্ক *which_failed) {
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(rlim);

        /* On failure returns the limit's index that failed in *which_failed, but only if non-NULL */

        প্রতি (পূর্ণাঙ্ক i = 0; i < _RLIMIT_MAX; i++) {
                যদি (!rlim[i])
                        অনুবর্ত;

                r = setrlimit_closest(i, rlim[i]);
                যদি (r < 0) {
                        যদি (which_failed)
                                *which_failed = i;

                        প্রত্যাগম r;
                }
        }

        যদি (which_failed)
                *which_failed = -1;

        প্রত্যাগম 0;
}

স্থির পূর্ণাঙ্ক rlimit_parse_u64(ধ্রুব বর্ণ *val, rlim_t *ret) {
        uint64_t u;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(val);
        প্রতিজ্ঞা(ret);

        যদি (streq(val, "infinity")) {
                *ret = RLIM_INFINITY;
                প্রত্যাগম 0;
        }

        /* setrlimit(2) suggests rlim_t is always 64-bit on Linux. */
        assert_cc(পরিমাণ(rlim_t) == পরিমাণ(uint64_t));

        r = safe_atou64(val, &u);
        যদি (r < 0)
                প্রত্যাগম r;
        যদি (u >= (uint64_t) RLIM_INFINITY)
                প্রত্যাগম -ERANGE;

        *ret = (rlim_t) u;
        প্রত্যাগম 0;
}

স্থির পূর্ণাঙ্ক rlimit_parse_size(ধ্রুব বর্ণ *val, rlim_t *ret) {
        uint64_t u;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(val);
        প্রতিজ্ঞা(ret);

        যদি (streq(val, "infinity")) {
                *ret = RLIM_INFINITY;
                প্রত্যাগম 0;
        }

        r = parse_size(val, 1024, &u);
        যদি (r < 0)
                প্রত্যাগম r;
        যদি (u >= (uint64_t) RLIM_INFINITY)
                প্রত্যাগম -ERANGE;

        *ret = (rlim_t) u;
        প্রত্যাগম 0;
}

স্থির পূর্ণাঙ্ক rlimit_parse_sec(ধ্রুব বর্ণ *val, rlim_t *ret) {
        uint64_t u;
        usec_t t;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(val);
        প্রতিজ্ঞা(ret);

        r = parse_sec(val, &t);
        যদি (r < 0)
                প্রত্যাগম r;
        যদি (t == USEC_INFINITY) {
                *ret = RLIM_INFINITY;
                প্রত্যাগম 0;
        }

        u = (uint64_t) DIV_ROUND_UP(t, USEC_PER_SEC);
        যদি (u >= (uint64_t) RLIM_INFINITY)
                প্রত্যাগম -ERANGE;

        *ret = (rlim_t) u;
        প্রত্যাগম 0;
}

স্থির পূর্ণাঙ্ক rlimit_parse_usec(ধ্রুব বর্ণ *val, rlim_t *ret) {
        usec_t t;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(val);
        প্রতিজ্ঞা(ret);

        r = parse_time(val, &t, 1);
        যদি (r < 0)
                প্রত্যাগম r;
        যদি (t == USEC_INFINITY) {
                *ret = RLIM_INFINITY;
                প্রত্যাগম 0;
        }

        *ret = (rlim_t) t;
        প্রত্যাগম 0;
}

স্থির পূর্ণাঙ্ক rlimit_parse_nice(ধ্রুব বর্ণ *val, rlim_t *ret) {
        uint64_t rl;
        পূর্ণাঙ্ক r;

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
                        প্রত্যাগম r;

                যদি (rl >= PRIO_MAX)
                        প্রত্যাগম -ERANGE;

                rl = 20 - rl;

        } অন্যথা যদি (val[0] == '-') {

                /* Prefixed with "-": Parse as negative user-friendly nice value */
                r = safe_atou64(val + 1, &rl);
                যদি (r < 0)
                        প্রত্যাগম r;

                যদি (rl > (uint64_t) (-PRIO_MIN))
                        প্রত্যাগম -ERANGE;

                rl = 20 + rl;
        } অন্যথা {

                /* Not prefixed: parse as raw resource limit value */
                r = safe_atou64(val, &rl);
                যদি (r < 0)
                        প্রত্যাগম r;

                যদি (rl > (uint64_t) (20 - PRIO_MIN))
                        প্রত্যাগম -ERANGE;
        }

        *ret = (rlim_t) rl;
        প্রত্যাগম 0;
}

স্থির পূর্ণাঙ্ক (*ধ্রুব rlimit_parse_table[_RLIMIT_MAX])(ধ্রুব বর্ণ *val, rlim_t *ret) = {
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

পূর্ণাঙ্ক rlimit_parse_one(পূর্ণাঙ্ক resource, ধ্রুব বর্ণ *val, rlim_t *ret) {
        প্রতিজ্ঞা(val);
        প্রতিজ্ঞা(ret);

        যদি (resource < 0)
                প্রত্যাগম -EINVAL;
        যদি (resource >= _RLIMIT_MAX)
                প্রত্যাগম -EINVAL;

        প্রত্যাগম rlimit_parse_table[resource](val, ret);
}

পূর্ণাঙ্ক rlimit_parse(পূর্ণাঙ্ক resource, ধ্রুব বর্ণ *val, সংরচনা rlimit *ret) {
        _cleanup_free_ বর্ণ *hard = রিক্ত, *soft = রিক্ত;
        rlim_t hl, sl;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(val);
        প্রতিজ্ঞা(ret);

        r = extract_first_word(&val, &soft, ":", EXTRACT_DONT_COALESCE_SEPARATORS);
        যদি (r < 0)
                প্রত্যাগম r;
        যদি (r == 0)
                প্রত্যাগম -EINVAL;

        r = rlimit_parse_one(resource, soft, &sl);
        যদি (r < 0)
                প্রত্যাগম r;

        r = extract_first_word(&val, &hard, ":", EXTRACT_DONT_COALESCE_SEPARATORS);
        যদি (r < 0)
                প্রত্যাগম r;
        যদি (!isempty(val))
                প্রত্যাগম -EINVAL;
        যদি (r == 0)
                hl = sl;
        অন্যথা {
                r = rlimit_parse_one(resource, hard, &hl);
                যদি (r < 0)
                        প্রত্যাগম r;
                যদি (sl > hl)
                        প্রত্যাগম -EILSEQ;
        }

        *ret = (সংরচনা rlimit) {
                .rlim_cur = sl,
                .rlim_max = hl,
        };

        প্রত্যাগম 0;
}

পূর্ণাঙ্ক rlimit_format(ধ্রুব সংরচনা rlimit *rl, বর্ণ **ret) {
        বর্ণ *s;

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
                প্রত্যাগম -ENOMEM;

        *ret = s;
        প্রত্যাগম 0;
}

স্থির ধ্রুব বর্ণ* ধ্রুব rlimit_table[_RLIMIT_MAX] = {
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

DEFINE_STRING_TABLE_LOOKUP(rlimit, পূর্ণাঙ্ক);

শূন্য rlimits_list(ধ্রুব বর্ণ *prefix) {
        FOREACH_ELEMENT(field, rlimit_table)
                printf("%s%s\n", strempty(prefix), *field);
}

পূর্ণাঙ্ক rlimit_from_string_harder(ধ্রুব বর্ণ *s) {
        ধ্রুব বর্ণ *suffix;

        /* The official prefix */
        suffix = startswith(s, "RLIMIT_");
        যদি (suffix)
                প্রত্যাগম rlimit_from_string(suffix);

        /* Our own unit file setting prefix */
        suffix = startswith(s, "Limit");
        যদি (suffix)
                প্রত্যাগম rlimit_from_string(suffix);

        প্রত্যাগম rlimit_from_string(s);
}

শূন্য rlimit_free_all(সংরচনা rlimit **rl) {
        free_many((শূন্য**) rl, _RLIMIT_MAX);
}

পূর্ণাঙ্ক rlimit_copy_all(সংরচনা rlimit* target[স্থির _RLIMIT_MAX], সংরচনা rlimit* ধ্রুব source[স্থির _RLIMIT_MAX]) {
        সংরচনা rlimit* copy[_RLIMIT_MAX] = {};

        প্রতিজ্ঞা(target);
        প্রতিজ্ঞা(source);

        প্রতি (পূর্ণাঙ্ক i = 0; i < _RLIMIT_MAX; i++) {
                যদি (!source[i])
                        অনুবর্ত;

                copy[i] = newdup(সংরচনা rlimit, source[i], 1);
                যদি (!copy[i]) {
                        rlimit_free_all(copy);
                        প্রত্যাগম -ENOMEM;
                }
        }

        memcpy(target, copy, পরিমাণ(সংরচনা rlimit*) * _RLIMIT_MAX);
        প্রত্যাগম 0;
}

পূর্ণাঙ্ক rlimit_nofile_bump(পূর্ণাঙ্ক limit) {
        পূর্ণাঙ্ক r;

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
                প্রত্যাগম log_debug_errno(r, "Failed to set RLIMIT_NOFILE: %m");

        প্রত্যাগম 0;
}

পূর্ণাঙ্ক rlimit_nofile_safe(শূন্য) {
        সংরচনা rlimit rl;

        /* Resets RLIMIT_NOFILE's soft limit FD_SETSIZE (i.e. 1024), for compatibility with software still using
         * select() */

        যদি (getrlimit(RLIMIT_NOFILE, &rl) < 0)
                প্রত্যাগম log_debug_errno(errno, "Failed to query RLIMIT_NOFILE: %m");

        যদি (rl.rlim_cur <= FD_SETSIZE)
                প্রত্যাগম 0;

        /* So we might have inherited a hard limit that's larger than the kernel's maximum limit as stored in
         * /proc/sys/fs/nr_open. If we pass this hard limit unmodified to setrlimit(), we'll get EPERM. To
         * make sure that doesn't happen, let's limit our hard limit to the value from nr_open. */
        rl.rlim_max = MIN(rl.rlim_max, (rlim_t) read_nr_open());
        rl.rlim_cur = MIN((rlim_t) FD_SETSIZE, rl.rlim_max);
        যদি (setrlimit(RLIMIT_NOFILE, &rl) < 0)
                প্রত্যাগম log_debug_errno(errno, "Failed to lower RLIMIT_NOFILE's soft limit to " RLIM_FMT ": %m", rl.rlim_cur);

        প্রত্যাগম 1;
}

পূর্ণাঙ্ক pid_getrlimit(pid_t pid, পূর্ণাঙ্ক resource, সংরচনা rlimit *ret) {

        স্থির ধ্রুব বর্ণ * ধ্রুব prefix_table[_RLIMIT_MAX] = {
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

        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(resource >= 0);
        প্রতিজ্ঞা(resource < _RLIMIT_MAX);
        প্রতিজ্ঞা(pid >= 0);
        প্রতিজ্ঞা(ret);

        যদি (pid == 0 || pid == getpid_cached())
                প্রত্যাগম RET_NERRNO(getrlimit(resource, ret));

        r = RET_NERRNO(prlimit(pid, resource, /* new_limit= */ রিক্ত, ret));
        যদি (!ERRNO_IS_NEG_PRIVILEGE(r))
                প্রত্যাগম r;

        /* We don't have access? Then try to go via /proc/$PID/limits. Weirdly that's world readable in
         * contrast to querying the data via prlimit() */

        ধ্রুব বর্ণ *p = procfs_file_alloca(pid, "limits");
        _cleanup_free_ বর্ণ *limits = রিক্ত;

        r = read_full_file(p, &limits, /* ret_size= */ রিক্ত);
        যদি (r < 0)
                প্রত্যাগম -EPERM; /* propagate original permission error if we can't access the limits file */

        _cleanup_strv_free_ বর্ণ **l = রিক্ত;
        l = strv_split_newlines(limits);
        যদি (!l)
                প্রত্যাগম -ENOMEM;

        STRV_FOREACH(i, strv_skip(l, 1)) {
                _cleanup_free_ বর্ণ *soft = রিক্ত, *hard = রিক্ত;
                uint64_t sv, hv;
                ধ্রুব বর্ণ *e;

                e = startswith(*i, prefix_table[resource]);
                যদি (!e)
                        অনুবর্ত;

                যদি (*e != ' ')
                        অনুবর্ত;

                e += strspn(e, WHITESPACE);

                size_t n;
                n = strcspn(e, WHITESPACE);
                যদি (n == 0)
                        অনুবর্ত;

                soft = strndup(e, n);
                যদি (!soft)
                        প্রত্যাগম -ENOMEM;

                e += n;
                যদি (*e != ' ')
                        অনুবর্ত;

                e += strspn(e, WHITESPACE);
                n = strcspn(e, WHITESPACE);
                যদি (n == 0)
                        অনুবর্ত;

                hard = strndup(e, n);
                যদি (!hard)
                        প্রত্যাগম -ENOMEM;

                যদি (streq(soft, "unlimited"))
                        sv = RLIM_INFINITY;
                অন্যথা {
                        r = safe_atou64(soft, &sv);
                        যদি (r < 0)
                                প্রত্যাগম r;
                }

                যদি (streq(hard, "unlimited"))
                        hv = RLIM_INFINITY;
                অন্যথা {
                        r = safe_atou64(hard, &hv);
                        যদি (r < 0)
                                প্রত্যাগম r;
                }

                *ret = (সংরচনা rlimit) {
                        .rlim_cur = sv,
                        .rlim_max = hv,
                };

                প্রত্যাগম 0;
        }

        প্রত্যাগম -ENOTRECOVERABLE;
}
