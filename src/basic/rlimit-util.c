/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ "alloc-util.h"
#ଅନ୍ତର୍ଭାଵ "errno-util.h"
#ଅନ୍ତର୍ଭାଵ "extract-word.h"
#ଅନ୍ତର୍ଭାଵ "fd-util.h"
#ଅନ୍ତର୍ଭାଵ "fileio.h"
#ଅନ୍ତର୍ଭାଵ "format-util.h"
#ଅନ୍ତର୍ଭାଵ "log.h"
#ଅନ୍ତର୍ଭାଵ "parse-util.h"
#ଅନ୍ତର୍ଭାଵ "process-util.h"
#ଅନ୍ତର୍ଭାଵ "rlimit-util.h"
#ଅନ୍ତର୍ଭାଵ "string-table.h"
#ଅନ୍ତର୍ଭାଵ "strv.h"
#ଅନ୍ତର୍ଭାଵ "time-util.h"

ପୂର୍ଣ୍ଣସଂଖ୍ୟା setrlimit_closest(ପୂର୍ଣ୍ଣସଂଖ୍ୟା resource, ଅଚଳ ସଂରଚନା rlimit *rlim) {
        ସଂରଚନା rlimit highest, fixed;

        ପ୍ରତିଜ୍ଞା(rlim);

        ଯଦି (setrlimit(resource, rlim) >= 0)
                ଫେରନ୍ତୁ 0;

        ଯଦି (errno != EPERM)
                ଫେରନ୍ତୁ -errno;

        /* So we failed to set the desired setrlimit, then let's try
         * to get as close as we can */
        ଯଦି (getrlimit(resource, &highest) < 0)
                ଫେରନ୍ତୁ -errno;

        /* If the hard limit is unbounded anyway, then the EPERM had other reasons, let's propagate the original EPERM
         * then */
        ଯଦି (highest.rlim_max == RLIM_INFINITY)
                ଫେରନ୍ତୁ -EPERM;

        fixed = (ସଂରଚନା rlimit) {
                .rlim_cur = MIN(rlim->rlim_cur, highest.rlim_max),
                .rlim_max = MIN(rlim->rlim_max, highest.rlim_max),
        };

        /* Shortcut things if we wouldn't change anything. */
        ଯଦି (fixed.rlim_cur == highest.rlim_cur &&
            fixed.rlim_max == highest.rlim_max)
                ଫେରନ୍ତୁ 0;

        log_debug("Failed at setting rlimit " RLIM_FMT " for resource RLIMIT_%s. Will attempt setting value " RLIM_FMT " instead.", rlim->rlim_max, rlimit_to_string(resource), fixed.rlim_max);

        ଫେରନ୍ତୁ RET_NERRNO(setrlimit(resource, &fixed));
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା setrlimit_closest_all(ଅଚଳ ସଂରଚନା rlimit *ଅଚଳ *rlim, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *which_failed) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(rlim);

        /* On failure returns the limit's index that failed in *which_failed, but only if non-NULL */

        ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < _RLIMIT_MAX; i++) {
                ଯଦି (!rlim[i])
                        ଜାରି;

                r = setrlimit_closest(i, rlim[i]);
                ଯଦି (r < 0) {
                        ଯଦି (which_failed)
                                *which_failed = i;

                        ଫେରନ୍ତୁ r;
                }
        }

        ଯଦି (which_failed)
                *which_failed = -1;

        ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା rlimit_parse_u64(ଅଚଳ ଅକ୍ଷର *val, rlim_t *ret) {
        uint64_t u;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(val);
        ପ୍ରତିଜ୍ଞା(ret);

        ଯଦି (streq(val, "infinity")) {
                *ret = RLIM_INFINITY;
                ଫେରନ୍ତୁ 0;
        }

        /* setrlimit(2) suggests rlim_t is always 64-bit on Linux. */
        assert_cc(ଆକାର(rlim_t) == ଆକାର(uint64_t));

        r = safe_atou64(val, &u);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;
        ଯଦି (u >= (uint64_t) RLIM_INFINITY)
                ଫେରନ୍ତୁ -ERANGE;

        *ret = (rlim_t) u;
        ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା rlimit_parse_size(ଅଚଳ ଅକ୍ଷର *val, rlim_t *ret) {
        uint64_t u;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(val);
        ପ୍ରତିଜ୍ଞା(ret);

        ଯଦି (streq(val, "infinity")) {
                *ret = RLIM_INFINITY;
                ଫେରନ୍ତୁ 0;
        }

        r = parse_size(val, 1024, &u);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;
        ଯଦି (u >= (uint64_t) RLIM_INFINITY)
                ଫେରନ୍ତୁ -ERANGE;

        *ret = (rlim_t) u;
        ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା rlimit_parse_sec(ଅଚଳ ଅକ୍ଷର *val, rlim_t *ret) {
        uint64_t u;
        usec_t t;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(val);
        ପ୍ରତିଜ୍ଞା(ret);

        r = parse_sec(val, &t);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;
        ଯଦି (t == USEC_INFINITY) {
                *ret = RLIM_INFINITY;
                ଫେରନ୍ତୁ 0;
        }

        u = (uint64_t) DIV_ROUND_UP(t, USEC_PER_SEC);
        ଯଦି (u >= (uint64_t) RLIM_INFINITY)
                ଫେରନ୍ତୁ -ERANGE;

        *ret = (rlim_t) u;
        ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା rlimit_parse_usec(ଅଚଳ ଅକ୍ଷର *val, rlim_t *ret) {
        usec_t t;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(val);
        ପ୍ରତିଜ୍ଞା(ret);

        r = parse_time(val, &t, 1);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;
        ଯଦି (t == USEC_INFINITY) {
                *ret = RLIM_INFINITY;
                ଫେରନ୍ତୁ 0;
        }

        *ret = (rlim_t) t;
        ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା rlimit_parse_nice(ଅଚଳ ଅକ୍ଷର *val, rlim_t *ret) {
        uint64_t rl;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(val);
        ପ୍ରତିଜ୍ଞା(ret);

        /* So, Linux is weird. The range for RLIMIT_NICE is 40..1, mapping to the nice levels -20..19. However, the
         * RLIMIT_NICE limit defaults to 0 by the kernel, i.e. a value that maps to nice level 20, which of course is
         * bogus and does not exist. In order to permit parsing the RLIMIT_NICE of 0 here we hence implement a slight
         * asymmetry: when parsing as positive nice level we permit 0..19. When parsing as negative nice level, we
         * permit -20..0. But when parsing as raw resource limit value then we also allow the special value 0.
         *
         * Yeah, Linux is quality engineering sometimes... */

        ଯଦି (val[0] == '+') {

                /* Prefixed with "+": Parse as positive user-friendly nice value */
                r = safe_atou64(val + 1, &rl);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                ଯଦି (rl >= PRIO_MAX)
                        ଫେରନ୍ତୁ -ERANGE;

                rl = 20 - rl;

        } ଅନ୍ୟଥା ଯଦି (val[0] == '-') {

                /* Prefixed with "-": Parse as negative user-friendly nice value */
                r = safe_atou64(val + 1, &rl);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                ଯଦି (rl > (uint64_t) (-PRIO_MIN))
                        ଫେରନ୍ତୁ -ERANGE;

                rl = 20 + rl;
        } ଅନ୍ୟଥା {

                /* Not prefixed: parse as raw resource limit value */
                r = safe_atou64(val, &rl);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                ଯଦି (rl > (uint64_t) (20 - PRIO_MIN))
                        ଫେରନ୍ତୁ -ERANGE;
        }

        *ret = (rlim_t) rl;
        ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*ଅଚଳ rlimit_parse_table[_RLIMIT_MAX])(ଅଚଳ ଅକ୍ଷର *val, rlim_t *ret) = {
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

ପୂର୍ଣ୍ଣସଂଖ୍ୟା rlimit_parse_one(ପୂର୍ଣ୍ଣସଂଖ୍ୟା resource, ଅଚଳ ଅକ୍ଷର *val, rlim_t *ret) {
        ପ୍ରତିଜ୍ଞା(val);
        ପ୍ରତିଜ୍ଞା(ret);

        ଯଦି (resource < 0)
                ଫେରନ୍ତୁ -EINVAL;
        ଯଦି (resource >= _RLIMIT_MAX)
                ଫେରନ୍ତୁ -EINVAL;

        ଫେରନ୍ତୁ rlimit_parse_table[resource](val, ret);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା rlimit_parse(ପୂର୍ଣ୍ଣସଂଖ୍ୟା resource, ଅଚଳ ଅକ୍ଷର *val, ସଂରଚନା rlimit *ret) {
        _cleanup_free_ ଅକ୍ଷର *hard = ଖାଲି, *soft = ଖାଲି;
        rlim_t hl, sl;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(val);
        ପ୍ରତିଜ୍ଞା(ret);

        r = extract_first_word(&val, &soft, ":", EXTRACT_DONT_COALESCE_SEPARATORS);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;
        ଯଦି (r == 0)
                ଫେରନ୍ତୁ -EINVAL;

        r = rlimit_parse_one(resource, soft, &sl);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        r = extract_first_word(&val, &hard, ":", EXTRACT_DONT_COALESCE_SEPARATORS);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;
        ଯଦି (!isempty(val))
                ଫେରନ୍ତୁ -EINVAL;
        ଯଦି (r == 0)
                hl = sl;
        ଅନ୍ୟଥା {
                r = rlimit_parse_one(resource, hard, &hl);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
                ଯଦି (sl > hl)
                        ଫେରନ୍ତୁ -EILSEQ;
        }

        *ret = (ସଂରଚନା rlimit) {
                .rlim_cur = sl,
                .rlim_max = hl,
        };

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା rlimit_format(ଅଚଳ ସଂରଚନା rlimit *rl, ଅକ୍ଷର **ret) {
        ଅକ୍ଷର *s;

        ପ୍ରତିଜ୍ଞା(rl);
        ପ୍ରତିଜ୍ଞା(ret);

        ଯଦି (rl->rlim_cur >= RLIM_INFINITY && rl->rlim_max >= RLIM_INFINITY)
                s = strdup("infinity");
        ଅନ୍ୟଥା ଯଦି (rl->rlim_cur >= RLIM_INFINITY)
                s = asprintf_safe("infinity:" RLIM_FMT, rl->rlim_max);
        ଅନ୍ୟଥା ଯଦି (rl->rlim_max >= RLIM_INFINITY)
                s = asprintf_safe(RLIM_FMT ":infinity", rl->rlim_cur);
        ଅନ୍ୟଥା ଯଦି (rl->rlim_cur == rl->rlim_max)
                s = asprintf_safe(RLIM_FMT, rl->rlim_cur);
        ଅନ୍ୟଥା
                s = asprintf_safe(RLIM_FMT ":" RLIM_FMT, rl->rlim_cur, rl->rlim_max);
        ଯଦି (!s)
                ଫେରନ୍ତୁ -ENOMEM;

        *ret = s;
        ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର* ଅଚଳ rlimit_table[_RLIMIT_MAX] = {
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

DEFINE_STRING_TABLE_LOOKUP(rlimit, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);

ଶୂନ୍ୟ rlimits_list(ଅଚଳ ଅକ୍ଷର *prefix) {
        FOREACH_ELEMENT(field, rlimit_table)
                printf("%s%s\n", strempty(prefix), *field);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା rlimit_from_string_harder(ଅଚଳ ଅକ୍ଷର *s) {
        ଅଚଳ ଅକ୍ଷର *suffix;

        /* The official prefix */
        suffix = startswith(s, "RLIMIT_");
        ଯଦି (suffix)
                ଫେରନ୍ତୁ rlimit_from_string(suffix);

        /* Our own unit file setting prefix */
        suffix = startswith(s, "Limit");
        ଯଦି (suffix)
                ଫେରନ୍ତୁ rlimit_from_string(suffix);

        ଫେରନ୍ତୁ rlimit_from_string(s);
}

ଶୂନ୍ୟ rlimit_free_all(ସଂରଚନା rlimit **rl) {
        free_many((ଶୂନ୍ୟ**) rl, _RLIMIT_MAX);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା rlimit_copy_all(ସଂରଚନା rlimit* target[ସ୍ଥିର _RLIMIT_MAX], ସଂରଚନା rlimit* ଅଚଳ source[ସ୍ଥିର _RLIMIT_MAX]) {
        ସଂରଚନା rlimit* copy[_RLIMIT_MAX] = {};

        ପ୍ରତିଜ୍ଞା(target);
        ପ୍ରତିଜ୍ଞା(source);

        ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < _RLIMIT_MAX; i++) {
                ଯଦି (!source[i])
                        ଜାରି;

                copy[i] = newdup(ସଂରଚନା rlimit, source[i], 1);
                ଯଦି (!copy[i]) {
                        rlimit_free_all(copy);
                        ଫେରନ୍ତୁ -ENOMEM;
                }
        }

        memcpy(target, copy, ଆକାର(ସଂରଚନା rlimit*) * _RLIMIT_MAX);
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା rlimit_nofile_bump(ପୂର୍ଣ୍ଣସଂଖ୍ୟା limit) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        /* Bumps the (soft) RLIMIT_NOFILE resource limit as close as possible to the specified limit. If a negative
         * limit is specified, bumps it to the maximum the kernel and the hard resource limit allows. This call should
         * be used by all our programs that might need a lot of fds, and that know how to deal with high fd numbers
         * (i.e. do not use select() — which chokes on fds >= 1024) */

        ଯଦି (limit < 0)
                limit = read_nr_open();

        ଯଦି (limit < 3)
                limit = 3;

        r = setrlimit_closest(RLIMIT_NOFILE, &RLIMIT_MAKE_CONST(limit));
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ log_debug_errno(r, "Failed to set RLIMIT_NOFILE: %m");

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା rlimit_nofile_safe(ଶୂନ୍ୟ) {
        ସଂରଚନା rlimit rl;

        /* Resets RLIMIT_NOFILE's soft limit FD_SETSIZE (i.e. 1024), for compatibility with software still using
         * select() */

        ଯଦି (getrlimit(RLIMIT_NOFILE, &rl) < 0)
                ଫେରନ୍ତୁ log_debug_errno(errno, "Failed to query RLIMIT_NOFILE: %m");

        ଯଦି (rl.rlim_cur <= FD_SETSIZE)
                ଫେରନ୍ତୁ 0;

        /* So we might have inherited a hard limit that's larger than the kernel's maximum limit as stored in
         * /proc/sys/fs/nr_open. If we pass this hard limit unmodified to setrlimit(), we'll get EPERM. To
         * make sure that doesn't happen, let's limit our hard limit to the value from nr_open. */
        rl.rlim_max = MIN(rl.rlim_max, (rlim_t) read_nr_open());
        rl.rlim_cur = MIN((rlim_t) FD_SETSIZE, rl.rlim_max);
        ଯଦି (setrlimit(RLIMIT_NOFILE, &rl) < 0)
                ଫେରନ୍ତୁ log_debug_errno(errno, "Failed to lower RLIMIT_NOFILE's soft limit to " RLIM_FMT ": %m", rl.rlim_cur);

        ଫେରନ୍ତୁ 1;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pid_getrlimit(pid_t pid, ପୂର୍ଣ୍ଣସଂଖ୍ୟା resource, ସଂରଚନା rlimit *ret) {

        ସ୍ଥିର ଅଚଳ ଅକ୍ଷର * ଅଚଳ prefix_table[_RLIMIT_MAX] = {
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

        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(resource >= 0);
        ପ୍ରତିଜ୍ଞା(resource < _RLIMIT_MAX);
        ପ୍ରତିଜ୍ଞା(pid >= 0);
        ପ୍ରତିଜ୍ଞା(ret);

        ଯଦି (pid == 0 || pid == getpid_cached())
                ଫେରନ୍ତୁ RET_NERRNO(getrlimit(resource, ret));

        r = RET_NERRNO(prlimit(pid, resource, /* new_limit= */ ଖାଲି, ret));
        ଯଦି (!ERRNO_IS_NEG_PRIVILEGE(r))
                ଫେରନ୍ତୁ r;

        /* We don't have access? Then try to go via /proc/$PID/limits. Weirdly that's world readable in
         * contrast to querying the data via prlimit() */

        ଅଚଳ ଅକ୍ଷର *p = procfs_file_alloca(pid, "limits");
        _cleanup_free_ ଅକ୍ଷର *limits = ଖାଲି;

        r = read_full_file(p, &limits, /* ret_size= */ ଖାଲି);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ -EPERM; /* propagate original permission error if we can't access the limits file */

        _cleanup_strv_free_ ଅକ୍ଷର **l = ଖାଲି;
        l = strv_split_newlines(limits);
        ଯଦି (!l)
                ଫେରନ୍ତୁ -ENOMEM;

        STRV_FOREACH(i, strv_skip(l, 1)) {
                _cleanup_free_ ଅକ୍ଷର *soft = ଖାଲି, *hard = ଖାଲି;
                uint64_t sv, hv;
                ଅଚଳ ଅକ୍ଷର *e;

                e = startswith(*i, prefix_table[resource]);
                ଯଦି (!e)
                        ଜାରି;

                ଯଦି (*e != ' ')
                        ଜାରି;

                e += strspn(e, WHITESPACE);

                size_t n;
                n = strcspn(e, WHITESPACE);
                ଯଦି (n == 0)
                        ଜାରି;

                soft = strndup(e, n);
                ଯଦି (!soft)
                        ଫେରନ୍ତୁ -ENOMEM;

                e += n;
                ଯଦି (*e != ' ')
                        ଜାରି;

                e += strspn(e, WHITESPACE);
                n = strcspn(e, WHITESPACE);
                ଯଦି (n == 0)
                        ଜାରି;

                hard = strndup(e, n);
                ଯଦି (!hard)
                        ଫେରନ୍ତୁ -ENOMEM;

                ଯଦି (streq(soft, "unlimited"))
                        sv = RLIM_INFINITY;
                ଅନ୍ୟଥା {
                        r = safe_atou64(soft, &sv);
                        ଯଦି (r < 0)
                                ଫେରନ୍ତୁ r;
                }

                ଯଦି (streq(hard, "unlimited"))
                        hv = RLIM_INFINITY;
                ଅନ୍ୟଥା {
                        r = safe_atou64(hard, &hv);
                        ଯଦି (r < 0)
                                ଫେରନ୍ତୁ r;
                }

                *ret = (ସଂରଚନା rlimit) {
                        .rlim_cur = sv,
                        .rlim_max = hv,
                };

                ଫେରନ୍ତୁ 0;
        }

        ଫେରନ୍ତୁ -ENOTRECOVERABLE;
}
