/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "errno-util.h"
#ಅನ್ತರ್ಭಾವ "extract-word.h"
#ಅನ್ತರ್ಭಾವ "fd-util.h"
#ಅನ್ತರ್ಭಾವ "fileio.h"
#ಅನ್ತರ್ಭಾವ "format-util.h"
#ಅನ್ತರ್ಭಾವ "log.h"
#ಅನ್ತರ್ಭಾವ "parse-util.h"
#ಅನ್ತರ್ಭಾವ "process-util.h"
#ಅನ್ತರ್ಭಾವ "rlimit-util.h"
#ಅನ್ತರ್ಭಾವ "string-table.h"
#ಅನ್ತರ್ಭಾವ "strv.h"
#ಅನ್ತರ್ಭಾವ "time-util.h"

ಪೂರ್ಣಾಙ್ಕ setrlimit_closest(ಪೂರ್ಣಾಙ್ಕ resource, ಧ್ರುವ ಸಂರಚನಾ rlimit *rlim) {
        ಸಂರಚನಾ rlimit highest, fixed;

        ಪ್ರತಿಜ್ಞಾ(rlim);

        ಯದಿ (setrlimit(resource, rlim) >= 0)
                ಪ್ರತ್ಯಾಗಮ 0;

        ಯದಿ (errno != EPERM)
                ಪ್ರತ್ಯಾಗಮ -errno;

        /* So we failed to set the desired setrlimit, then let's try
         * to get as close as we can */
        ಯದಿ (getrlimit(resource, &highest) < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        /* If the hard limit is unbounded anyway, then the EPERM had other reasons, let's propagate the original EPERM
         * then */
        ಯದಿ (highest.rlim_max == RLIM_INFINITY)
                ಪ್ರತ್ಯಾಗಮ -EPERM;

        fixed = (ಸಂರಚನಾ rlimit) {
                .rlim_cur = MIN(rlim->rlim_cur, highest.rlim_max),
                .rlim_max = MIN(rlim->rlim_max, highest.rlim_max),
        };

        /* Shortcut things if we wouldn't change anything. */
        ಯದಿ (fixed.rlim_cur == highest.rlim_cur &&
            fixed.rlim_max == highest.rlim_max)
                ಪ್ರತ್ಯಾಗಮ 0;

        log_debug("Failed at setting rlimit " RLIM_FMT " for resource RLIMIT_%s. Will attempt setting value " RLIM_FMT " instead.", rlim->rlim_max, rlimit_to_string(resource), fixed.rlim_max);

        ಪ್ರತ್ಯಾಗಮ RET_NERRNO(setrlimit(resource, &fixed));
}

ಪೂರ್ಣಾಙ್ಕ setrlimit_closest_all(ಧ್ರುವ ಸಂರಚನಾ rlimit *ಧ್ರುವ *rlim, ಪೂರ್ಣಾಙ್ಕ *which_failed) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(rlim);

        /* On failure returns the limit's index that failed in *which_failed, but only if non-NULL */

        ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < _RLIMIT_MAX; i++) {
                ಯದಿ (!rlim[i])
                        ಅನುವರ್ತ;

                r = setrlimit_closest(i, rlim[i]);
                ಯದಿ (r < 0) {
                        ಯದಿ (which_failed)
                                *which_failed = i;

                        ಪ್ರತ್ಯಾಗಮ r;
                }
        }

        ಯದಿ (which_failed)
                *which_failed = -1;

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ rlimit_parse_u64(ಧ್ರುವ ವರ್ಣ *val, rlim_t *ret) {
        uint64_t u;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(val);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಯದಿ (streq(val, "infinity")) {
                *ret = RLIM_INFINITY;
                ಪ್ರತ್ಯಾಗಮ 0;
        }

        /* setrlimit(2) suggests rlim_t is always 64-bit on Linux. */
        assert_cc(ಪರಿಮಾಣ(rlim_t) == ಪರಿಮಾಣ(uint64_t));

        r = safe_atou64(val, &u);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;
        ಯದಿ (u >= (uint64_t) RLIM_INFINITY)
                ಪ್ರತ್ಯಾಗಮ -ERANGE;

        *ret = (rlim_t) u;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ rlimit_parse_size(ಧ್ರುವ ವರ್ಣ *val, rlim_t *ret) {
        uint64_t u;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(val);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಯದಿ (streq(val, "infinity")) {
                *ret = RLIM_INFINITY;
                ಪ್ರತ್ಯಾಗಮ 0;
        }

        r = parse_size(val, 1024, &u);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;
        ಯದಿ (u >= (uint64_t) RLIM_INFINITY)
                ಪ್ರತ್ಯಾಗಮ -ERANGE;

        *ret = (rlim_t) u;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ rlimit_parse_sec(ಧ್ರುವ ವರ್ಣ *val, rlim_t *ret) {
        uint64_t u;
        usec_t t;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(val);
        ಪ್ರತಿಜ್ಞಾ(ret);

        r = parse_sec(val, &t);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;
        ಯದಿ (t == USEC_INFINITY) {
                *ret = RLIM_INFINITY;
                ಪ್ರತ್ಯಾಗಮ 0;
        }

        u = (uint64_t) DIV_ROUND_UP(t, USEC_PER_SEC);
        ಯದಿ (u >= (uint64_t) RLIM_INFINITY)
                ಪ್ರತ್ಯಾಗಮ -ERANGE;

        *ret = (rlim_t) u;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ rlimit_parse_usec(ಧ್ರುವ ವರ್ಣ *val, rlim_t *ret) {
        usec_t t;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(val);
        ಪ್ರತಿಜ್ಞಾ(ret);

        r = parse_time(val, &t, 1);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;
        ಯದಿ (t == USEC_INFINITY) {
                *ret = RLIM_INFINITY;
                ಪ್ರತ್ಯಾಗಮ 0;
        }

        *ret = (rlim_t) t;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ rlimit_parse_nice(ಧ್ರುವ ವರ್ಣ *val, rlim_t *ret) {
        uint64_t rl;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(val);
        ಪ್ರತಿಜ್ಞಾ(ret);

        /* So, Linux is weird. The range for RLIMIT_NICE is 40..1, mapping to the nice levels -20..19. However, the
         * RLIMIT_NICE limit defaults to 0 by the kernel, i.e. a value that maps to nice level 20, which of course is
         * bogus and does not exist. In order to permit parsing the RLIMIT_NICE of 0 here we hence implement a slight
         * asymmetry: when parsing as positive nice level we permit 0..19. When parsing as negative nice level, we
         * permit -20..0. But when parsing as raw resource limit value then we also allow the special value 0.
         *
         * Yeah, Linux is quality engineering sometimes... */

        ಯದಿ (val[0] == '+') {

                /* Prefixed with "+": Parse as positive user-friendly nice value */
                r = safe_atou64(val + 1, &rl);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                ಯದಿ (rl >= PRIO_MAX)
                        ಪ್ರತ್ಯಾಗಮ -ERANGE;

                rl = 20 - rl;

        } ಅನ್ಯಥಾ ಯದಿ (val[0] == '-') {

                /* Prefixed with "-": Parse as negative user-friendly nice value */
                r = safe_atou64(val + 1, &rl);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                ಯದಿ (rl > (uint64_t) (-PRIO_MIN))
                        ಪ್ರತ್ಯಾಗಮ -ERANGE;

                rl = 20 + rl;
        } ಅನ್ಯಥಾ {

                /* Not prefixed: parse as raw resource limit value */
                r = safe_atou64(val, &rl);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                ಯದಿ (rl > (uint64_t) (20 - PRIO_MIN))
                        ಪ್ರತ್ಯಾಗಮ -ERANGE;
        }

        *ret = (rlim_t) rl;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ (*ಧ್ರುವ rlimit_parse_table[_RLIMIT_MAX])(ಧ್ರುವ ವರ್ಣ *val, rlim_t *ret) = {
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

ಪೂರ್ಣಾಙ್ಕ rlimit_parse_one(ಪೂರ್ಣಾಙ್ಕ resource, ಧ್ರುವ ವರ್ಣ *val, rlim_t *ret) {
        ಪ್ರತಿಜ್ಞಾ(val);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಯದಿ (resource < 0)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;
        ಯದಿ (resource >= _RLIMIT_MAX)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        ಪ್ರತ್ಯಾಗಮ rlimit_parse_table[resource](val, ret);
}

ಪೂರ್ಣಾಙ್ಕ rlimit_parse(ಪೂರ್ಣಾಙ್ಕ resource, ಧ್ರುವ ವರ್ಣ *val, ಸಂರಚನಾ rlimit *ret) {
        _cleanup_free_ ವರ್ಣ *hard = ರಿಕ್ತ, *soft = ರಿಕ್ತ;
        rlim_t hl, sl;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(val);
        ಪ್ರತಿಜ್ಞಾ(ret);

        r = extract_first_word(&val, &soft, ":", EXTRACT_DONT_COALESCE_SEPARATORS);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;
        ಯದಿ (r == 0)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        r = rlimit_parse_one(resource, soft, &sl);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        r = extract_first_word(&val, &hard, ":", EXTRACT_DONT_COALESCE_SEPARATORS);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;
        ಯದಿ (!isempty(val))
                ಪ್ರತ್ಯಾಗಮ -EINVAL;
        ಯದಿ (r == 0)
                hl = sl;
        ಅನ್ಯಥಾ {
                r = rlimit_parse_one(resource, hard, &hl);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
                ಯದಿ (sl > hl)
                        ಪ್ರತ್ಯಾಗಮ -EILSEQ;
        }

        *ret = (ಸಂರಚನಾ rlimit) {
                .rlim_cur = sl,
                .rlim_max = hl,
        };

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ rlimit_format(ಧ್ರುವ ಸಂರಚನಾ rlimit *rl, ವರ್ಣ **ret) {
        ವರ್ಣ *s;

        ಪ್ರತಿಜ್ಞಾ(rl);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಯದಿ (rl->rlim_cur >= RLIM_INFINITY && rl->rlim_max >= RLIM_INFINITY)
                s = strdup("infinity");
        ಅನ್ಯಥಾ ಯದಿ (rl->rlim_cur >= RLIM_INFINITY)
                s = asprintf_safe("infinity:" RLIM_FMT, rl->rlim_max);
        ಅನ್ಯಥಾ ಯದಿ (rl->rlim_max >= RLIM_INFINITY)
                s = asprintf_safe(RLIM_FMT ":infinity", rl->rlim_cur);
        ಅನ್ಯಥಾ ಯದಿ (rl->rlim_cur == rl->rlim_max)
                s = asprintf_safe(RLIM_FMT, rl->rlim_cur);
        ಅನ್ಯಥಾ
                s = asprintf_safe(RLIM_FMT ":" RLIM_FMT, rl->rlim_cur, rl->rlim_max);
        ಯದಿ (!s)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        *ret = s;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ* ಧ್ರುವ rlimit_table[_RLIMIT_MAX] = {
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

DEFINE_STRING_TABLE_LOOKUP(rlimit, ಪೂರ್ಣಾಙ್ಕ);

ಶೂನ್ಯ rlimits_list(ಧ್ರುವ ವರ್ಣ *prefix) {
        FOREACH_ELEMENT(field, rlimit_table)
                printf("%s%s\n", strempty(prefix), *field);
}

ಪೂರ್ಣಾಙ್ಕ rlimit_from_string_harder(ಧ್ರುವ ವರ್ಣ *s) {
        ಧ್ರುವ ವರ್ಣ *suffix;

        /* The official prefix */
        suffix = startswith(s, "RLIMIT_");
        ಯದಿ (suffix)
                ಪ್ರತ್ಯಾಗಮ rlimit_from_string(suffix);

        /* Our own unit file setting prefix */
        suffix = startswith(s, "Limit");
        ಯದಿ (suffix)
                ಪ್ರತ್ಯಾಗಮ rlimit_from_string(suffix);

        ಪ್ರತ್ಯಾಗಮ rlimit_from_string(s);
}

ಶೂನ್ಯ rlimit_free_all(ಸಂರಚನಾ rlimit **rl) {
        free_many((ಶೂನ್ಯ**) rl, _RLIMIT_MAX);
}

ಪೂರ್ಣಾಙ್ಕ rlimit_copy_all(ಸಂರಚನಾ rlimit* target[ಸ್ಥಿರ _RLIMIT_MAX], ಸಂರಚನಾ rlimit* ಧ್ರುವ source[ಸ್ಥಿರ _RLIMIT_MAX]) {
        ಸಂರಚನಾ rlimit* copy[_RLIMIT_MAX] = {};

        ಪ್ರತಿಜ್ಞಾ(target);
        ಪ್ರತಿಜ್ಞಾ(source);

        ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < _RLIMIT_MAX; i++) {
                ಯದಿ (!source[i])
                        ಅನುವರ್ತ;

                copy[i] = newdup(ಸಂರಚನಾ rlimit, source[i], 1);
                ಯದಿ (!copy[i]) {
                        rlimit_free_all(copy);
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;
                }
        }

        memcpy(target, copy, ಪರಿಮಾಣ(ಸಂರಚನಾ rlimit*) * _RLIMIT_MAX);
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ rlimit_nofile_bump(ಪೂರ್ಣಾಙ್ಕ limit) {
        ಪೂರ್ಣಾಙ್ಕ r;

        /* Bumps the (soft) RLIMIT_NOFILE resource limit as close as possible to the specified limit. If a negative
         * limit is specified, bumps it to the maximum the kernel and the hard resource limit allows. This call should
         * be used by all our programs that might need a lot of fds, and that know how to deal with high fd numbers
         * (i.e. do not use select() — which chokes on fds >= 1024) */

        ಯದಿ (limit < 0)
                limit = read_nr_open();

        ಯದಿ (limit < 3)
                limit = 3;

        r = setrlimit_closest(RLIMIT_NOFILE, &RLIMIT_MAKE_CONST(limit));
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ log_debug_errno(r, "Failed to set RLIMIT_NOFILE: %m");

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ rlimit_nofile_safe(ಶೂನ್ಯ) {
        ಸಂರಚನಾ rlimit rl;

        /* Resets RLIMIT_NOFILE's soft limit FD_SETSIZE (i.e. 1024), for compatibility with software still using
         * select() */

        ಯದಿ (getrlimit(RLIMIT_NOFILE, &rl) < 0)
                ಪ್ರತ್ಯಾಗಮ log_debug_errno(errno, "Failed to query RLIMIT_NOFILE: %m");

        ಯದಿ (rl.rlim_cur <= FD_SETSIZE)
                ಪ್ರತ್ಯಾಗಮ 0;

        /* So we might have inherited a hard limit that's larger than the kernel's maximum limit as stored in
         * /proc/sys/fs/nr_open. If we pass this hard limit unmodified to setrlimit(), we'll get EPERM. To
         * make sure that doesn't happen, let's limit our hard limit to the value from nr_open. */
        rl.rlim_max = MIN(rl.rlim_max, (rlim_t) read_nr_open());
        rl.rlim_cur = MIN((rlim_t) FD_SETSIZE, rl.rlim_max);
        ಯದಿ (setrlimit(RLIMIT_NOFILE, &rl) < 0)
                ಪ್ರತ್ಯಾಗಮ log_debug_errno(errno, "Failed to lower RLIMIT_NOFILE's soft limit to " RLIM_FMT ": %m", rl.rlim_cur);

        ಪ್ರತ್ಯಾಗಮ 1;
}

ಪೂರ್ಣಾಙ್ಕ pid_getrlimit(pid_t pid, ಪೂರ್ಣಾಙ್ಕ resource, ಸಂರಚನಾ rlimit *ret) {

        ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ * ಧ್ರುವ prefix_table[_RLIMIT_MAX] = {
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

        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(resource >= 0);
        ಪ್ರತಿಜ್ಞಾ(resource < _RLIMIT_MAX);
        ಪ್ರತಿಜ್ಞಾ(pid >= 0);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಯದಿ (pid == 0 || pid == getpid_cached())
                ಪ್ರತ್ಯಾಗಮ RET_NERRNO(getrlimit(resource, ret));

        r = RET_NERRNO(prlimit(pid, resource, /* new_limit= */ ರಿಕ್ತ, ret));
        ಯದಿ (!ERRNO_IS_NEG_PRIVILEGE(r))
                ಪ್ರತ್ಯಾಗಮ r;

        /* We don't have access? Then try to go via /proc/$PID/limits. Weirdly that's world readable in
         * contrast to querying the data via prlimit() */

        ಧ್ರುವ ವರ್ಣ *p = procfs_file_alloca(pid, "limits");
        _cleanup_free_ ವರ್ಣ *limits = ರಿಕ್ತ;

        r = read_full_file(p, &limits, /* ret_size= */ ರಿಕ್ತ);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ -EPERM; /* propagate original permission error if we can't access the limits file */

        _cleanup_strv_free_ ವರ್ಣ **l = ರಿಕ್ತ;
        l = strv_split_newlines(limits);
        ಯದಿ (!l)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        STRV_FOREACH(i, strv_skip(l, 1)) {
                _cleanup_free_ ವರ್ಣ *soft = ರಿಕ್ತ, *hard = ರಿಕ್ತ;
                uint64_t sv, hv;
                ಧ್ರುವ ವರ್ಣ *e;

                e = startswith(*i, prefix_table[resource]);
                ಯದಿ (!e)
                        ಅನುವರ್ತ;

                ಯದಿ (*e != ' ')
                        ಅನುವರ್ತ;

                e += strspn(e, WHITESPACE);

                size_t n;
                n = strcspn(e, WHITESPACE);
                ಯದಿ (n == 0)
                        ಅನುವರ್ತ;

                soft = strndup(e, n);
                ಯದಿ (!soft)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                e += n;
                ಯದಿ (*e != ' ')
                        ಅನುವರ್ತ;

                e += strspn(e, WHITESPACE);
                n = strcspn(e, WHITESPACE);
                ಯದಿ (n == 0)
                        ಅನುವರ್ತ;

                hard = strndup(e, n);
                ಯದಿ (!hard)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                ಯದಿ (streq(soft, "unlimited"))
                        sv = RLIM_INFINITY;
                ಅನ್ಯಥಾ {
                        r = safe_atou64(soft, &sv);
                        ಯದಿ (r < 0)
                                ಪ್ರತ್ಯಾಗಮ r;
                }

                ಯದಿ (streq(hard, "unlimited"))
                        hv = RLIM_INFINITY;
                ಅನ್ಯಥಾ {
                        r = safe_atou64(hard, &hv);
                        ಯದಿ (r < 0)
                                ಪ್ರತ್ಯಾಗಮ r;
                }

                *ret = (ಸಂರಚನಾ rlimit) {
                        .rlim_cur = sv,
                        .rlim_max = hv,
                };

                ಪ್ರತ್ಯಾಗಮ 0;
        }

        ಪ್ರತ್ಯಾಗಮ -ENOTRECOVERABLE;
}
