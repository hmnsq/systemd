/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ "alloc-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "errno-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "extract-word.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fd-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fileio.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "format-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "log.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "parse-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "process-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "rlimit-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-table.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "strv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "time-util.h"

ਪੂਰਨ setrlimit_closest(ਪੂਰਨ resource, ਅਚਲ ਬਣਤਰ rlimit *rlim) {
        ਬਣਤਰ rlimit highest, fixed;

        ਪ੍ਰਤਿਜ੍ਞਾ(rlim);

        ਜੇ (setrlimit(resource, rlim) >= 0)
                ਵਾਪਸ 0;

        ਜੇ (errno != EPERM)
                ਵਾਪਸ -errno;

        /* So we failed to set the desired setrlimit, then let's try
         * to get as close as we can */
        ਜੇ (getrlimit(resource, &highest) < 0)
                ਵਾਪਸ -errno;

        /* If the hard limit is unbounded anyway, then the EPERM had other reasons, let's propagate the original EPERM
         * then */
        ਜੇ (highest.rlim_max == RLIM_INFINITY)
                ਵਾਪਸ -EPERM;

        fixed = (ਬਣਤਰ rlimit) {
                .rlim_cur = MIN(rlim->rlim_cur, highest.rlim_max),
                .rlim_max = MIN(rlim->rlim_max, highest.rlim_max),
        };

        /* Shortcut things if we wouldn't change anything. */
        ਜੇ (fixed.rlim_cur == highest.rlim_cur &&
            fixed.rlim_max == highest.rlim_max)
                ਵਾਪਸ 0;

        log_debug("Failed at setting rlimit " RLIM_FMT " for resource RLIMIT_%s. Will attempt setting value " RLIM_FMT " instead.", rlim->rlim_max, rlimit_ਮੰਜ਼ਿਲ_string(resource), fixed.rlim_max);

        ਵਾਪਸ RET_NERRNO(setrlimit(resource, &fixed));
}

ਪੂਰਨ setrlimit_closest_all(ਅਚਲ ਬਣਤਰ rlimit *ਅਚਲ *rlim, ਪੂਰਨ *which_failed) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(rlim);

        /* On failure returns the limit's index that failed in *which_failed, but only if non-NULL */

        ਲਈ (ਪੂਰਨ i = 0; i < _RLIMIT_MAX; i++) {
                ਜੇ (!rlim[i])
                        ਜਾਰੀ;

                r = setrlimit_closest(i, rlim[i]);
                ਜੇ (r < 0) {
                        ਜੇ (which_failed)
                                *which_failed = i;

                        ਵਾਪਸ r;
                }
        }

        ਜੇ (which_failed)
                *which_failed = -1;

        ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ rlimit_parse_u64(ਅਚਲ ਅੱਖਰ *val, rlim_t *ret) {
        uint64_t u;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(val);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ਜੇ (streq(val, "infinity")) {
                *ret = RLIM_INFINITY;
                ਵਾਪਸ 0;
        }

        /* setrlimit(2) suggests rlim_t is always 64-bit on Linux. */
        assert_cc(ਆਕਾਰ(rlim_t) == ਆਕਾਰ(uint64_t));

        r = ਸੁਰੱਖਿਅਤ_atou64(val, &u);
        ਜੇ (r < 0)
                ਵਾਪਸ r;
        ਜੇ (u >= (uint64_t) RLIM_INFINITY)
                ਵਾਪਸ -ERANGE;

        *ret = (rlim_t) u;
        ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ rlimit_parse_ਮਾਪ(ਅਚਲ ਅੱਖਰ *val, rlim_t *ret) {
        uint64_t u;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(val);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ਜੇ (streq(val, "infinity")) {
                *ret = RLIM_INFINITY;
                ਵਾਪਸ 0;
        }

        r = parse_ਮਾਪ(val, 1024, &u);
        ਜੇ (r < 0)
                ਵਾਪਸ r;
        ਜੇ (u >= (uint64_t) RLIM_INFINITY)
                ਵਾਪਸ -ERANGE;

        *ret = (rlim_t) u;
        ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ rlimit_parse_sec(ਅਚਲ ਅੱਖਰ *val, rlim_t *ret) {
        uint64_t u;
        usec_t t;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(val);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        r = parse_sec(val, &t);
        ਜੇ (r < 0)
                ਵਾਪਸ r;
        ਜੇ (t == USEC_INFINITY) {
                *ret = RLIM_INFINITY;
                ਵਾਪਸ 0;
        }

        u = (uint64_t) DIV_ਗੇੜਾ_ਉੱਪਰ(t, USEC_ਪ੍ਰਤੀ_SEC);
        ਜੇ (u >= (uint64_t) RLIM_INFINITY)
                ਵਾਪਸ -ERANGE;

        *ret = (rlim_t) u;
        ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ rlimit_parse_usec(ਅਚਲ ਅੱਖਰ *val, rlim_t *ret) {
        usec_t t;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(val);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        r = parse_ਸਮਾਂ(val, &t, 1);
        ਜੇ (r < 0)
                ਵਾਪਸ r;
        ਜੇ (t == USEC_INFINITY) {
                *ret = RLIM_INFINITY;
                ਵਾਪਸ 0;
        }

        *ret = (rlim_t) t;
        ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ rlimit_parse_nice(ਅਚਲ ਅੱਖਰ *val, rlim_t *ret) {
        uint64_t rl;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(val);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        /* So, Linux is weird. The range for RLIMIT_NICE is 40..1, mapping to the nice levels -20..19. However, the
         * RLIMIT_NICE limit defaults to 0 by the kernel, i.e. a value that maps to nice level 20, which of course is
         * bogus and does not exist. In order to permit parsing the RLIMIT_NICE of 0 here we hence implement a slight
         * asymmetry: when parsing as positive nice level we permit 0..19. When parsing as negative nice level, we
         * permit -20..0. But when parsing as raw resource limit value then we also allow the special value 0.
         *
         * Yeah, Linux is quality engineering sometimes... */

        ਜੇ (val[0] == '+') {

                /* Prefixed with "+": Parse as positive user-friendly nice value */
                r = ਸੁਰੱਖਿਅਤ_atou64(val + 1, &rl);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                ਜੇ (rl >= PRIO_MAX)
                        ਵਾਪਸ -ERANGE;

                rl = 20 - rl;

        } ਨਹੀਂਤਾਂ ਜੇ (val[0] == '-') {

                /* Prefixed with "-": Parse as negative user-friendly nice value */
                r = ਸੁਰੱਖਿਅਤ_atou64(val + 1, &rl);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                ਜੇ (rl > (uint64_t) (-PRIO_MIN))
                        ਵਾਪਸ -ERANGE;

                rl = 20 + rl;
        } ਨਹੀਂਤਾਂ {

                /* Not prefixed: parse as raw resource limit value */
                r = ਸੁਰੱਖਿਅਤ_atou64(val, &rl);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                ਜੇ (rl > (uint64_t) (20 - PRIO_MIN))
                        ਵਾਪਸ -ERANGE;
        }

        *ret = (rlim_t) rl;
        ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ (*ਅਚਲ rlimit_parse_ਸਾਰਣੀ[_RLIMIT_MAX])(ਅਚਲ ਅੱਖਰ *val, rlim_t *ret) = {
        [RLIMIT_CPU]        = rlimit_parse_sec,
        [RLIMIT_FSIZE]      = rlimit_parse_ਮਾਪ,
        [RLIMIT_DATA]       = rlimit_parse_ਮਾਪ,
        [RLIMIT_STACK]      = rlimit_parse_ਮਾਪ,
        [RLIMIT_CORE]       = rlimit_parse_ਮਾਪ,
        [RLIMIT_RSS]        = rlimit_parse_ਮਾਪ,
        [RLIMIT_NOFILE]     = rlimit_parse_u64,
        [RLIMIT_AS]         = rlimit_parse_ਮਾਪ,
        [RLIMIT_NPROC]      = rlimit_parse_u64,
        [RLIMIT_MEMLOCK]    = rlimit_parse_ਮਾਪ,
        [RLIMIT_LOCKS]      = rlimit_parse_u64,
        [RLIMIT_SIGPENDING] = rlimit_parse_u64,
        [RLIMIT_MSGQUEUE]   = rlimit_parse_ਮਾਪ,
        [RLIMIT_NICE]       = rlimit_parse_nice,
        [RLIMIT_RTPRIO]     = rlimit_parse_u64,
        [RLIMIT_RTTIME]     = rlimit_parse_usec,
};

ਪੂਰਨ rlimit_parse_one(ਪੂਰਨ resource, ਅਚਲ ਅੱਖਰ *val, rlim_t *ret) {
        ਪ੍ਰਤਿਜ੍ਞਾ(val);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ਜੇ (resource < 0)
                ਵਾਪਸ -EINVAL;
        ਜੇ (resource >= _RLIMIT_MAX)
                ਵਾਪਸ -EINVAL;

        ਵਾਪਸ rlimit_parse_ਸਾਰਣੀ[resource](val, ret);
}

ਪੂਰਨ rlimit_parse(ਪੂਰਨ resource, ਅਚਲ ਅੱਖਰ *val, ਬਣਤਰ rlimit *ret) {
        _cleanup_free_ ਅੱਖਰ *hard = ਨਿਰਾ, *soft = ਨਿਰਾ;
        rlim_t hl, sl;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(val);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        r = ਕੱਢੋ_ਪਹਿਲਾ_ਸ਼ਬਦ(&val, &soft, ":", ਕੱਢੋ_DONT_COALESCE_SEPARATORS);
        ਜੇ (r < 0)
                ਵਾਪਸ r;
        ਜੇ (r == 0)
                ਵਾਪਸ -EINVAL;

        r = rlimit_parse_one(resource, soft, &sl);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        r = ਕੱਢੋ_ਪਹਿਲਾ_ਸ਼ਬਦ(&val, &hard, ":", ਕੱਢੋ_DONT_COALESCE_SEPARATORS);
        ਜੇ (r < 0)
                ਵਾਪਸ r;
        ਜੇ (!isempty(val))
                ਵਾਪਸ -EINVAL;
        ਜੇ (r == 0)
                hl = sl;
        ਨਹੀਂਤਾਂ {
                r = rlimit_parse_one(resource, hard, &hl);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
                ਜੇ (sl > hl)
                        ਵਾਪਸ -EILSEQ;
        }

        *ret = (ਬਣਤਰ rlimit) {
                .rlim_cur = sl,
                .rlim_max = hl,
        };

        ਵਾਪਸ 0;
}

ਪੂਰਨ rlimit_format(ਅਚਲ ਬਣਤਰ rlimit *rl, ਅੱਖਰ **ret) {
        ਅੱਖਰ *s;

        ਪ੍ਰਤਿਜ੍ਞਾ(rl);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ਜੇ (rl->rlim_cur >= RLIM_INFINITY && rl->rlim_max >= RLIM_INFINITY)
                s = strdup("infinity");
        ਨਹੀਂਤਾਂ ਜੇ (rl->rlim_cur >= RLIM_INFINITY)
                s = asprintf_ਸੁਰੱਖਿਅਤ("infinity:" RLIM_FMT, rl->rlim_max);
        ਨਹੀਂਤਾਂ ਜੇ (rl->rlim_max >= RLIM_INFINITY)
                s = asprintf_ਸੁਰੱਖਿਅਤ(RLIM_FMT ":infinity", rl->rlim_cur);
        ਨਹੀਂਤਾਂ ਜੇ (rl->rlim_cur == rl->rlim_max)
                s = asprintf_ਸੁਰੱਖਿਅਤ(RLIM_FMT, rl->rlim_cur);
        ਨਹੀਂਤਾਂ
                s = asprintf_ਸੁਰੱਖਿਅਤ(RLIM_FMT ":" RLIM_FMT, rl->rlim_cur, rl->rlim_max);
        ਜੇ (!s)
                ਵਾਪਸ -ENOMEM;

        *ret = s;
        ਵਾਪਸ 0;
}

ਸਥਿਰ ਅਚਲ ਅੱਖਰ* ਅਚਲ rlimit_ਸਾਰਣੀ[_RLIMIT_MAX] = {
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

DEFINE_STRING_ਸਾਰਣੀ_LOOKUP(rlimit, ਪੂਰਨ);

ਖਾਲੀ rlimits_list(ਅਚਲ ਅੱਖਰ *ਅਗੇਤਰ) {
        FOREACH_ELEMENT(ਖੇਤਰ, rlimit_ਸਾਰਣੀ)
                ਲਿਖੋ("%s%s\n", strempty(ਅਗੇਤਰ), *ਖੇਤਰ);
}

ਪੂਰਨ rlimit_from_string_harder(ਅਚਲ ਅੱਖਰ *s) {
        ਅਚਲ ਅੱਖਰ *suffix;

        /* The official prefix */
        suffix = startswith(s, "RLIMIT_");
        ਜੇ (suffix)
                ਵਾਪਸ rlimit_from_string(suffix);

        /* Our own unit file setting prefix */
        suffix = startswith(s, "Limit");
        ਜੇ (suffix)
                ਵਾਪਸ rlimit_from_string(suffix);

        ਵਾਪਸ rlimit_from_string(s);
}

ਖਾਲੀ rlimit_free_all(ਬਣਤਰ rlimit **rl) {
        free_many((ਖਾਲੀ**) rl, _RLIMIT_MAX);
}

ਪੂਰਨ rlimit_copy_all(ਬਣਤਰ rlimit* ਨਿਸ਼ਾਨਾ[ਸਥਿਰ _RLIMIT_MAX], ਬਣਤਰ rlimit* ਅਚਲ ਸਰੋਤ[ਸਥਿਰ _RLIMIT_MAX]) {
        ਬਣਤਰ rlimit* copy[_RLIMIT_MAX] = {};

        ਪ੍ਰਤਿਜ੍ਞਾ(ਨਿਸ਼ਾਨਾ);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਸਰੋਤ);

        ਲਈ (ਪੂਰਨ i = 0; i < _RLIMIT_MAX; i++) {
                ਜੇ (!ਸਰੋਤ[i])
                        ਜਾਰੀ;

                copy[i] = newdup(ਬਣਤਰ rlimit, ਸਰੋਤ[i], 1);
                ਜੇ (!copy[i]) {
                        rlimit_free_all(copy);
                        ਵਾਪਸ -ENOMEM;
                }
        }

        ਸਮ੍ਰਿਤੀ_ਨਕਲ(ਨਿਸ਼ਾਨਾ, copy, ਆਕਾਰ(ਬਣਤਰ rlimit*) * _RLIMIT_MAX);
        ਵਾਪਸ 0;
}

ਪੂਰਨ rlimit_nofile_bump(ਪੂਰਨ limit) {
        ਪੂਰਨ r;

        /* Bumps the (soft) RLIMIT_NOFILE resource limit as close as possible to the specified limit. If a negative
         * limit is specified, bumps it to the maximum the kernel and the hard resource limit allows. This call should
         * be used by all our programs that might need a lot of fds, and that know how to deal with high fd numbers
         * (i.e. do not use select() — which chokes on fds >= 1024) */

        ਜੇ (limit < 0)
                limit = read_nr_open();

        ਜੇ (limit < 3)
                limit = 3;

        r = setrlimit_closest(RLIMIT_NOFILE, &RLIMIT_MAKE_CONST(limit));
        ਜੇ (r < 0)
                ਵਾਪਸ log_debug_errno(r, "Failed to set RLIMIT_NOFILE: %m");

        ਵਾਪਸ 0;
}

ਪੂਰਨ rlimit_nofile_ਸੁਰੱਖਿਅਤ(ਖਾਲੀ) {
        ਬਣਤਰ rlimit rl;

        /* Resets RLIMIT_NOFILE's soft limit FD_SETSIZE (i.e. 1024), for compatibility with software still using
         * select() */

        ਜੇ (getrlimit(RLIMIT_NOFILE, &rl) < 0)
                ਵਾਪਸ log_debug_errno(errno, "Failed to query RLIMIT_NOFILE: %m");

        ਜੇ (rl.rlim_cur <= FD_SETSIZE)
                ਵਾਪਸ 0;

        /* So we might have inherited a hard limit that's larger than the kernel's maximum limit as stored in
         * /proc/sys/fs/nr_open. If we pass this hard limit unmodified to setrlimit(), we'll get EPERM. To
         * make sure that doesn't happen, let's limit our hard limit to the value from nr_open. */
        rl.rlim_max = MIN(rl.rlim_max, (rlim_t) read_nr_open());
        rl.rlim_cur = MIN((rlim_t) FD_SETSIZE, rl.rlim_max);
        ਜੇ (setrlimit(RLIMIT_NOFILE, &rl) < 0)
                ਵਾਪਸ log_debug_errno(errno, "Failed to lower RLIMIT_NOFILE's soft limit to " RLIM_FMT ": %m", rl.rlim_cur);

        ਵਾਪਸ 1;
}

ਪੂਰਨ pid_getrlimit(pid_t pid, ਪੂਰਨ resource, ਬਣਤਰ rlimit *ret) {

        ਸਥਿਰ ਅਚਲ ਅੱਖਰ * ਅਚਲ ਅਗੇਤਰ_ਸਾਰਣੀ[_RLIMIT_MAX] = {
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

        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(resource >= 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(resource < _RLIMIT_MAX);
        ਪ੍ਰਤਿਜ੍ਞਾ(pid >= 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ਜੇ (pid == 0 || pid == getpid_cached())
                ਵਾਪਸ RET_NERRNO(getrlimit(resource, ret));

        r = RET_NERRNO(prlimit(pid, resource, /* new_limit= */ ਨਿਰਾ, ret));
        ਜੇ (!ERRNO_IS_NEG_PRIVILEGE(r))
                ਵਾਪਸ r;

        /* We don't have access? Then try to go via /proc/$PID/limits. Weirdly that's world readable in
         * contrast to querying the data via prlimit() */

        ਅਚਲ ਅੱਖਰ *p = procfs_file_alloca(pid, "limits");
        _cleanup_free_ ਅੱਖਰ *limits = ਨਿਰਾ;

        r = read_ਭਰਿਆ_file(p, &limits, /* ret_size= */ ਨਿਰਾ);
        ਜੇ (r < 0)
                ਵਾਪਸ -EPERM; /* propagate original permission error if we can't access the limits file */

        _cleanup_strv_free_ ਅੱਖਰ **l = ਨਿਰਾ;
        l = strv_ਵੰਡੋ_newlines(limits);
        ਜੇ (!l)
                ਵਾਪਸ -ENOMEM;

        STRV_FOREACH(i, strv_skip(l, 1)) {
                _cleanup_free_ ਅੱਖਰ *soft = ਨਿਰਾ, *hard = ਨਿਰਾ;
                uint64_t sv, hv;
                ਅਚਲ ਅੱਖਰ *e;

                e = startswith(*i, ਅਗੇਤਰ_ਸਾਰਣੀ[resource]);
                ਜੇ (!e)
                        ਜਾਰੀ;

                ਜੇ (*e != ' ')
                        ਜਾਰੀ;

                e += strspn(e, WHITESPACE);

                size_t n;
                n = strcspn(e, WHITESPACE);
                ਜੇ (n == 0)
                        ਜਾਰੀ;

                soft = strndup(e, n);
                ਜੇ (!soft)
                        ਵਾਪਸ -ENOMEM;

                e += n;
                ਜੇ (*e != ' ')
                        ਜਾਰੀ;

                e += strspn(e, WHITESPACE);
                n = strcspn(e, WHITESPACE);
                ਜੇ (n == 0)
                        ਜਾਰੀ;

                hard = strndup(e, n);
                ਜੇ (!hard)
                        ਵਾਪਸ -ENOMEM;

                ਜੇ (streq(soft, "unlimited"))
                        sv = RLIM_INFINITY;
                ਨਹੀਂਤਾਂ {
                        r = ਸੁਰੱਖਿਅਤ_atou64(soft, &sv);
                        ਜੇ (r < 0)
                                ਵਾਪਸ r;
                }

                ਜੇ (streq(hard, "unlimited"))
                        hv = RLIM_INFINITY;
                ਨਹੀਂਤਾਂ {
                        r = ਸੁਰੱਖਿਅਤ_atou64(hard, &hv);
                        ਜੇ (r < 0)
                                ਵਾਪਸ r;
                }

                *ret = (ਬਣਤਰ rlimit) {
                        .rlim_cur = sv,
                        .rlim_max = hv,
                };

                ਵਾਪਸ 0;
        }

        ਵਾਪਸ -ENOTRECOVERABLE;
}
