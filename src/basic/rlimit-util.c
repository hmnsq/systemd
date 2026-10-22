/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ "alloc-util.h"
#அந்தர்பாவ "errno-util.h"
#அந்தர்பாவ "extract-word.h"
#அந்தர்பாவ "fd-util.h"
#அந்தர்பாவ "fileio.h"
#அந்தர்பாவ "format-util.h"
#அந்தர்பாவ "log.h"
#அந்தர்பாவ "parse-util.h"
#அந்தர்பாவ "process-util.h"
#அந்தர்பாவ "rlimit-util.h"
#அந்தர்பாவ "string-table.h"
#அந்தர்பாவ "strv.h"
#அந்தர்பாவ "time-util.h"

முழு setrlimit_closest(முழு resource, மாறிலி கட்டமைப்பு rlimit *rlim) {
        கட்டமைப்பு rlimit highest, fixed;

        ப்ரதிஜ்ஞா(rlim);

        எனில் (setrlimit(resource, rlim) >= 0)
                திரும்பு 0;

        எனில் (errno != EPERM)
                திரும்பு -errno;

        /* So we failed to set the desired setrlimit, then let's try
         * to get as close as we can */
        எனில் (getrlimit(resource, &highest) < 0)
                திரும்பு -errno;

        /* If the hard limit is unbounded anyway, then the EPERM had other reasons, let's propagate the original EPERM
         * then */
        எனில் (highest.rlim_max == RLIM_INFINITY)
                திரும்பு -EPERM;

        fixed = (கட்டமைப்பு rlimit) {
                .rlim_cur = MIN(rlim->rlim_cur, highest.rlim_max),
                .rlim_max = MIN(rlim->rlim_max, highest.rlim_max),
        };

        /* Shortcut things if we wouldn't change anything. */
        எனில் (fixed.rlim_cur == highest.rlim_cur &&
            fixed.rlim_max == highest.rlim_max)
                திரும்பு 0;

        log_debug("Failed at setting rlimit " RLIM_FMT " for resource RLIMIT_%s. Will attempt setting value " RLIM_FMT " instead.", rlim->rlim_max, rlimit_to_string(resource), fixed.rlim_max);

        திரும்பு RET_NERRNO(setrlimit(resource, &fixed));
}

முழு setrlimit_closest_all(மாறிலி கட்டமைப்பு rlimit *மாறிலி *rlim, முழு *which_failed) {
        முழு r;

        ப்ரதிஜ்ஞா(rlim);

        /* On failure returns the limit's index that failed in *which_failed, but only if non-NULL */

        க்கு (முழு i = 0; i < _RLIMIT_MAX; i++) {
                எனில் (!rlim[i])
                        தொடர்;

                r = setrlimit_closest(i, rlim[i]);
                எனில் (r < 0) {
                        எனில் (which_failed)
                                *which_failed = i;

                        திரும்பு r;
                }
        }

        எனில் (which_failed)
                *which_failed = -1;

        திரும்பு 0;
}

நிலையான முழு rlimit_parse_u64(மாறிலி எழுத்து *val, rlim_t *ret) {
        uint64_t u;
        முழு r;

        ப்ரதிஜ்ஞா(val);
        ப்ரதிஜ்ஞா(ret);

        எனில் (streq(val, "infinity")) {
                *ret = RLIM_INFINITY;
                திரும்பு 0;
        }

        /* setrlimit(2) suggests rlim_t is always 64-bit on Linux. */
        assert_cc(அளவு(rlim_t) == அளவு(uint64_t));

        r = safe_atou64(val, &u);
        எனில் (r < 0)
                திரும்பு r;
        எனில் (u >= (uint64_t) RLIM_INFINITY)
                திரும்பு -ERANGE;

        *ret = (rlim_t) u;
        திரும்பு 0;
}

நிலையான முழு rlimit_parse_size(மாறிலி எழுத்து *val, rlim_t *ret) {
        uint64_t u;
        முழு r;

        ப்ரதிஜ்ஞா(val);
        ப்ரதிஜ்ஞா(ret);

        எனில் (streq(val, "infinity")) {
                *ret = RLIM_INFINITY;
                திரும்பு 0;
        }

        r = parse_size(val, 1024, &u);
        எனில் (r < 0)
                திரும்பு r;
        எனில் (u >= (uint64_t) RLIM_INFINITY)
                திரும்பு -ERANGE;

        *ret = (rlim_t) u;
        திரும்பு 0;
}

நிலையான முழு rlimit_parse_sec(மாறிலி எழுத்து *val, rlim_t *ret) {
        uint64_t u;
        usec_t t;
        முழு r;

        ப்ரதிஜ்ஞா(val);
        ப்ரதிஜ்ஞா(ret);

        r = parse_sec(val, &t);
        எனில் (r < 0)
                திரும்பு r;
        எனில் (t == USEC_INFINITY) {
                *ret = RLIM_INFINITY;
                திரும்பு 0;
        }

        u = (uint64_t) DIV_ROUND_UP(t, USEC_PER_SEC);
        எனில் (u >= (uint64_t) RLIM_INFINITY)
                திரும்பு -ERANGE;

        *ret = (rlim_t) u;
        திரும்பு 0;
}

நிலையான முழு rlimit_parse_usec(மாறிலி எழுத்து *val, rlim_t *ret) {
        usec_t t;
        முழு r;

        ப்ரதிஜ்ஞா(val);
        ப்ரதிஜ்ஞா(ret);

        r = parse_time(val, &t, 1);
        எனில் (r < 0)
                திரும்பு r;
        எனில் (t == USEC_INFINITY) {
                *ret = RLIM_INFINITY;
                திரும்பு 0;
        }

        *ret = (rlim_t) t;
        திரும்பு 0;
}

நிலையான முழு rlimit_parse_nice(மாறிலி எழுத்து *val, rlim_t *ret) {
        uint64_t rl;
        முழு r;

        ப்ரதிஜ்ஞா(val);
        ப்ரதிஜ்ஞா(ret);

        /* So, Linux is weird. The range for RLIMIT_NICE is 40..1, mapping to the nice levels -20..19. However, the
         * RLIMIT_NICE limit defaults to 0 by the kernel, i.e. a value that maps to nice level 20, which of course is
         * bogus and does not exist. In order to permit parsing the RLIMIT_NICE of 0 here we hence implement a slight
         * asymmetry: when parsing as positive nice level we permit 0..19. When parsing as negative nice level, we
         * permit -20..0. But when parsing as raw resource limit value then we also allow the special value 0.
         *
         * Yeah, Linux is quality engineering sometimes... */

        எனில் (val[0] == '+') {

                /* Prefixed with "+": Parse as positive user-friendly nice value */
                r = safe_atou64(val + 1, &rl);
                எனில் (r < 0)
                        திரும்பு r;

                எனில் (rl >= PRIO_MAX)
                        திரும்பு -ERANGE;

                rl = 20 - rl;

        } இல்லையெனில் எனில் (val[0] == '-') {

                /* Prefixed with "-": Parse as negative user-friendly nice value */
                r = safe_atou64(val + 1, &rl);
                எனில் (r < 0)
                        திரும்பு r;

                எனில் (rl > (uint64_t) (-PRIO_MIN))
                        திரும்பு -ERANGE;

                rl = 20 + rl;
        } இல்லையெனில் {

                /* Not prefixed: parse as raw resource limit value */
                r = safe_atou64(val, &rl);
                எனில் (r < 0)
                        திரும்பு r;

                எனில் (rl > (uint64_t) (20 - PRIO_MIN))
                        திரும்பு -ERANGE;
        }

        *ret = (rlim_t) rl;
        திரும்பு 0;
}

நிலையான முழு (*மாறிலி rlimit_parse_table[_RLIMIT_MAX])(மாறிலி எழுத்து *val, rlim_t *ret) = {
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

முழு rlimit_parse_one(முழு resource, மாறிலி எழுத்து *val, rlim_t *ret) {
        ப்ரதிஜ்ஞா(val);
        ப்ரதிஜ்ஞா(ret);

        எனில் (resource < 0)
                திரும்பு -EINVAL;
        எனில் (resource >= _RLIMIT_MAX)
                திரும்பு -EINVAL;

        திரும்பு rlimit_parse_table[resource](val, ret);
}

முழு rlimit_parse(முழு resource, மாறிலி எழுத்து *val, கட்டமைப்பு rlimit *ret) {
        _cleanup_free_ எழுத்து *hard = வெற்று, *soft = வெற்று;
        rlim_t hl, sl;
        முழு r;

        ப்ரதிஜ்ஞா(val);
        ப்ரதிஜ்ஞா(ret);

        r = extract_first_word(&val, &soft, ":", EXTRACT_DONT_COALESCE_SEPARATORS);
        எனில் (r < 0)
                திரும்பு r;
        எனில் (r == 0)
                திரும்பு -EINVAL;

        r = rlimit_parse_one(resource, soft, &sl);
        எனில் (r < 0)
                திரும்பு r;

        r = extract_first_word(&val, &hard, ":", EXTRACT_DONT_COALESCE_SEPARATORS);
        எனில் (r < 0)
                திரும்பு r;
        எனில் (!isempty(val))
                திரும்பு -EINVAL;
        எனில் (r == 0)
                hl = sl;
        இல்லையெனில் {
                r = rlimit_parse_one(resource, hard, &hl);
                எனில் (r < 0)
                        திரும்பு r;
                எனில் (sl > hl)
                        திரும்பு -EILSEQ;
        }

        *ret = (கட்டமைப்பு rlimit) {
                .rlim_cur = sl,
                .rlim_max = hl,
        };

        திரும்பு 0;
}

முழு rlimit_format(மாறிலி கட்டமைப்பு rlimit *rl, எழுத்து **ret) {
        எழுத்து *s;

        ப்ரதிஜ்ஞா(rl);
        ப்ரதிஜ்ஞா(ret);

        எனில் (rl->rlim_cur >= RLIM_INFINITY && rl->rlim_max >= RLIM_INFINITY)
                s = strdup("infinity");
        இல்லையெனில் எனில் (rl->rlim_cur >= RLIM_INFINITY)
                s = asprintf_safe("infinity:" RLIM_FMT, rl->rlim_max);
        இல்லையெனில் எனில் (rl->rlim_max >= RLIM_INFINITY)
                s = asprintf_safe(RLIM_FMT ":infinity", rl->rlim_cur);
        இல்லையெனில் எனில் (rl->rlim_cur == rl->rlim_max)
                s = asprintf_safe(RLIM_FMT, rl->rlim_cur);
        இல்லையெனில்
                s = asprintf_safe(RLIM_FMT ":" RLIM_FMT, rl->rlim_cur, rl->rlim_max);
        எனில் (!s)
                திரும்பு -ENOMEM;

        *ret = s;
        திரும்பு 0;
}

நிலையான மாறிலி எழுத்து* மாறிலி rlimit_table[_RLIMIT_MAX] = {
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

DEFINE_STRING_TABLE_LOOKUP(rlimit, முழு);

வெறுமை rlimits_list(மாறிலி எழுத்து *prefix) {
        FOREACH_ELEMENT(field, rlimit_table)
                printf("%s%s\n", strempty(prefix), *field);
}

முழு rlimit_from_string_harder(மாறிலி எழுத்து *s) {
        மாறிலி எழுத்து *suffix;

        /* The official prefix */
        suffix = startswith(s, "RLIMIT_");
        எனில் (suffix)
                திரும்பு rlimit_from_string(suffix);

        /* Our own unit file setting prefix */
        suffix = startswith(s, "Limit");
        எனில் (suffix)
                திரும்பு rlimit_from_string(suffix);

        திரும்பு rlimit_from_string(s);
}

வெறுமை rlimit_free_all(கட்டமைப்பு rlimit **rl) {
        free_many((வெறுமை**) rl, _RLIMIT_MAX);
}

முழு rlimit_copy_all(கட்டமைப்பு rlimit* target[நிலையான _RLIMIT_MAX], கட்டமைப்பு rlimit* மாறிலி source[நிலையான _RLIMIT_MAX]) {
        கட்டமைப்பு rlimit* copy[_RLIMIT_MAX] = {};

        ப்ரதிஜ்ஞா(target);
        ப்ரதிஜ்ஞா(source);

        க்கு (முழு i = 0; i < _RLIMIT_MAX; i++) {
                எனில் (!source[i])
                        தொடர்;

                copy[i] = newdup(கட்டமைப்பு rlimit, source[i], 1);
                எனில் (!copy[i]) {
                        rlimit_free_all(copy);
                        திரும்பு -ENOMEM;
                }
        }

        memcpy(target, copy, அளவு(கட்டமைப்பு rlimit*) * _RLIMIT_MAX);
        திரும்பு 0;
}

முழு rlimit_nofile_bump(முழு limit) {
        முழு r;

        /* Bumps the (soft) RLIMIT_NOFILE resource limit as close as possible to the specified limit. If a negative
         * limit is specified, bumps it to the maximum the kernel and the hard resource limit allows. This call should
         * be used by all our programs that might need a lot of fds, and that know how to deal with high fd numbers
         * (i.e. do not use select() — which chokes on fds >= 1024) */

        எனில் (limit < 0)
                limit = read_nr_open();

        எனில் (limit < 3)
                limit = 3;

        r = setrlimit_closest(RLIMIT_NOFILE, &RLIMIT_MAKE_CONST(limit));
        எனில் (r < 0)
                திரும்பு log_debug_errno(r, "Failed to set RLIMIT_NOFILE: %m");

        திரும்பு 0;
}

முழு rlimit_nofile_safe(வெறுமை) {
        கட்டமைப்பு rlimit rl;

        /* Resets RLIMIT_NOFILE's soft limit FD_SETSIZE (i.e. 1024), for compatibility with software still using
         * select() */

        எனில் (getrlimit(RLIMIT_NOFILE, &rl) < 0)
                திரும்பு log_debug_errno(errno, "Failed to query RLIMIT_NOFILE: %m");

        எனில் (rl.rlim_cur <= FD_SETSIZE)
                திரும்பு 0;

        /* So we might have inherited a hard limit that's larger than the kernel's maximum limit as stored in
         * /proc/sys/fs/nr_open. If we pass this hard limit unmodified to setrlimit(), we'll get EPERM. To
         * make sure that doesn't happen, let's limit our hard limit to the value from nr_open. */
        rl.rlim_max = MIN(rl.rlim_max, (rlim_t) read_nr_open());
        rl.rlim_cur = MIN((rlim_t) FD_SETSIZE, rl.rlim_max);
        எனில் (setrlimit(RLIMIT_NOFILE, &rl) < 0)
                திரும்பு log_debug_errno(errno, "Failed to lower RLIMIT_NOFILE's soft limit to " RLIM_FMT ": %m", rl.rlim_cur);

        திரும்பு 1;
}

முழு pid_getrlimit(pid_t pid, முழு resource, கட்டமைப்பு rlimit *ret) {

        நிலையான மாறிலி எழுத்து * மாறிலி prefix_table[_RLIMIT_MAX] = {
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

        முழு r;

        ப்ரதிஜ்ஞா(resource >= 0);
        ப்ரதிஜ்ஞா(resource < _RLIMIT_MAX);
        ப்ரதிஜ்ஞா(pid >= 0);
        ப்ரதிஜ்ஞா(ret);

        எனில் (pid == 0 || pid == getpid_cached())
                திரும்பு RET_NERRNO(getrlimit(resource, ret));

        r = RET_NERRNO(prlimit(pid, resource, /* new_limit= */ வெற்று, ret));
        எனில் (!ERRNO_IS_NEG_PRIVILEGE(r))
                திரும்பு r;

        /* We don't have access? Then try to go via /proc/$PID/limits. Weirdly that's world readable in
         * contrast to querying the data via prlimit() */

        மாறிலி எழுத்து *p = procfs_file_alloca(pid, "limits");
        _cleanup_free_ எழுத்து *limits = வெற்று;

        r = read_full_file(p, &limits, /* ret_size= */ வெற்று);
        எனில் (r < 0)
                திரும்பு -EPERM; /* propagate original permission error if we can't access the limits file */

        _cleanup_strv_free_ எழுத்து **l = வெற்று;
        l = strv_split_newlines(limits);
        எனில் (!l)
                திரும்பு -ENOMEM;

        STRV_FOREACH(i, strv_skip(l, 1)) {
                _cleanup_free_ எழுத்து *soft = வெற்று, *hard = வெற்று;
                uint64_t sv, hv;
                மாறிலி எழுத்து *e;

                e = startswith(*i, prefix_table[resource]);
                எனில் (!e)
                        தொடர்;

                எனில் (*e != ' ')
                        தொடர்;

                e += strspn(e, WHITESPACE);

                size_t n;
                n = strcspn(e, WHITESPACE);
                எனில் (n == 0)
                        தொடர்;

                soft = strndup(e, n);
                எனில் (!soft)
                        திரும்பு -ENOMEM;

                e += n;
                எனில் (*e != ' ')
                        தொடர்;

                e += strspn(e, WHITESPACE);
                n = strcspn(e, WHITESPACE);
                எனில் (n == 0)
                        தொடர்;

                hard = strndup(e, n);
                எனில் (!hard)
                        திரும்பு -ENOMEM;

                எனில் (streq(soft, "unlimited"))
                        sv = RLIM_INFINITY;
                இல்லையெனில் {
                        r = safe_atou64(soft, &sv);
                        எனில் (r < 0)
                                திரும்பு r;
                }

                எனில் (streq(hard, "unlimited"))
                        hv = RLIM_INFINITY;
                இல்லையெனில் {
                        r = safe_atou64(hard, &hv);
                        எனில் (r < 0)
                                திரும்பு r;
                }

                *ret = (கட்டமைப்பு rlimit) {
                        .rlim_cur = sv,
                        .rlim_max = hv,
                };

                திரும்பு 0;
        }

        திரும்பு -ENOTRECOVERABLE;
}
