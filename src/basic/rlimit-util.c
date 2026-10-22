/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ "alloc-util.h"
#అన్తర్భావ "errno-util.h"
#అన్తర్భావ "extract-word.h"
#అన్తర్భావ "fd-util.h"
#అన్తర్భావ "fileio.h"
#అన్తర్భావ "format-util.h"
#అన్తర్భావ "log.h"
#అన్తర్భావ "parse-util.h"
#అన్తర్భావ "process-util.h"
#అన్తర్భావ "rlimit-util.h"
#అన్తర్భావ "string-table.h"
#అన్తర్భావ "strv.h"
#అన్తర్భావ "time-util.h"

పూర్ణాంకం setrlimit_closest(పూర్ణాంకం resource, స్థిరరాశి నిర్మాణం rlimit *rlim) {
        నిర్మాణం rlimit highest, fixed;

        ప్రతిజ్ఞా(rlim);

        ఒకవేళ (setrlimit(resource, rlim) >= 0)
                తిరిగి 0;

        ఒకవేళ (errno != EPERM)
                తిరిగి -errno;

        /* So we failed to set the desired setrlimit, then let's try
         * to get as close as we can */
        ఒకవేళ (getrlimit(resource, &highest) < 0)
                తిరిగి -errno;

        /* If the hard limit is unbounded anyway, then the EPERM had other reasons, let's propagate the original EPERM
         * then */
        ఒకవేళ (highest.rlim_max == RLIM_INFINITY)
                తిరిగి -EPERM;

        fixed = (నిర్మాణం rlimit) {
                .rlim_cur = MIN(rlim->rlim_cur, highest.rlim_max),
                .rlim_max = MIN(rlim->rlim_max, highest.rlim_max),
        };

        /* Shortcut things if we wouldn't change anything. */
        ఒకవేళ (fixed.rlim_cur == highest.rlim_cur &&
            fixed.rlim_max == highest.rlim_max)
                తిరిగి 0;

        log_debug("Failed at setting rlimit " RLIM_FMT " for resource RLIMIT_%s. Will attempt setting value " RLIM_FMT " instead.", rlim->rlim_max, rlimit_గమ్యం_string(resource), fixed.rlim_max);

        తిరిగి RET_NERRNO(setrlimit(resource, &fixed));
}

పూర్ణాంకం setrlimit_closest_all(స్థిరరాశి నిర్మాణం rlimit *స్థిరరాశి *rlim, పూర్ణాంకం *which_failed) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(rlim);

        /* On failure returns the limit's index that failed in *which_failed, but only if non-NULL */

        కొరకు (పూర్ణాంకం i = 0; i < _RLIMIT_MAX; i++) {
                ఒకవేళ (!rlim[i])
                        కొనసాగు;

                r = setrlimit_closest(i, rlim[i]);
                ఒకవేళ (r < 0) {
                        ఒకవేళ (which_failed)
                                *which_failed = i;

                        తిరిగి r;
                }
        }

        ఒకవేళ (which_failed)
                *which_failed = -1;

        తిరిగి 0;
}

స్థిర పూర్ణాంకం rlimit_parse_u64(స్థిరరాశి అక్షరం *val, rlim_t *ret) {
        uint64_t u;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(val);
        ప్రతిజ్ఞా(ret);

        ఒకవేళ (streq(val, "infinity")) {
                *ret = RLIM_INFINITY;
                తిరిగి 0;
        }

        /* setrlimit(2) suggests rlim_t is always 64-bit on Linux. */
        assert_cc(పరిమాణం(rlim_t) == పరిమాణం(uint64_t));

        r = సురక్షిత_atou64(val, &u);
        ఒకవేళ (r < 0)
                తిరిగి r;
        ఒకవేళ (u >= (uint64_t) RLIM_INFINITY)
                తిరిగి -ERANGE;

        *ret = (rlim_t) u;
        తిరిగి 0;
}

స్థిర పూర్ణాంకం rlimit_parse_కొలత(స్థిరరాశి అక్షరం *val, rlim_t *ret) {
        uint64_t u;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(val);
        ప్రతిజ్ఞా(ret);

        ఒకవేళ (streq(val, "infinity")) {
                *ret = RLIM_INFINITY;
                తిరిగి 0;
        }

        r = parse_కొలత(val, 1024, &u);
        ఒకవేళ (r < 0)
                తిరిగి r;
        ఒకవేళ (u >= (uint64_t) RLIM_INFINITY)
                తిరిగి -ERANGE;

        *ret = (rlim_t) u;
        తిరిగి 0;
}

స్థిర పూర్ణాంకం rlimit_parse_sec(స్థిరరాశి అక్షరం *val, rlim_t *ret) {
        uint64_t u;
        usec_t t;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(val);
        ప్రతిజ్ఞా(ret);

        r = parse_sec(val, &t);
        ఒకవేళ (r < 0)
                తిరిగి r;
        ఒకవేళ (t == USEC_INFINITY) {
                *ret = RLIM_INFINITY;
                తిరిగి 0;
        }

        u = (uint64_t) DIV_విడత_పైకి(t, USEC_చొప్పున_SEC);
        ఒకవేళ (u >= (uint64_t) RLIM_INFINITY)
                తిరిగి -ERANGE;

        *ret = (rlim_t) u;
        తిరిగి 0;
}

స్థిర పూర్ణాంకం rlimit_parse_usec(స్థిరరాశి అక్షరం *val, rlim_t *ret) {
        usec_t t;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(val);
        ప్రతిజ్ఞా(ret);

        r = parse_సమయం(val, &t, 1);
        ఒకవేళ (r < 0)
                తిరిగి r;
        ఒకవేళ (t == USEC_INFINITY) {
                *ret = RLIM_INFINITY;
                తిరిగి 0;
        }

        *ret = (rlim_t) t;
        తిరిగి 0;
}

స్థిర పూర్ణాంకం rlimit_parse_nice(స్థిరరాశి అక్షరం *val, rlim_t *ret) {
        uint64_t rl;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(val);
        ప్రతిజ్ఞా(ret);

        /* So, Linux is weird. The range for RLIMIT_NICE is 40..1, mapping to the nice levels -20..19. However, the
         * RLIMIT_NICE limit defaults to 0 by the kernel, i.e. a value that maps to nice level 20, which of course is
         * bogus and does not exist. In order to permit parsing the RLIMIT_NICE of 0 here we hence implement a slight
         * asymmetry: when parsing as positive nice level we permit 0..19. When parsing as negative nice level, we
         * permit -20..0. But when parsing as raw resource limit value then we also allow the special value 0.
         *
         * Yeah, Linux is quality engineering sometimes... */

        ఒకవేళ (val[0] == '+') {

                /* Prefixed with "+": Parse as positive user-friendly nice value */
                r = సురక్షిత_atou64(val + 1, &rl);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                ఒకవేళ (rl >= PRIO_MAX)
                        తిరిగి -ERANGE;

                rl = 20 - rl;

        } లేకపోతే ఒకవేళ (val[0] == '-') {

                /* Prefixed with "-": Parse as negative user-friendly nice value */
                r = సురక్షిత_atou64(val + 1, &rl);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                ఒకవేళ (rl > (uint64_t) (-PRIO_MIN))
                        తిరిగి -ERANGE;

                rl = 20 + rl;
        } లేకపోతే {

                /* Not prefixed: parse as raw resource limit value */
                r = సురక్షిత_atou64(val, &rl);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                ఒకవేళ (rl > (uint64_t) (20 - PRIO_MIN))
                        తిరిగి -ERANGE;
        }

        *ret = (rlim_t) rl;
        తిరిగి 0;
}

స్థిర పూర్ణాంకం (*స్థిరరాశి rlimit_parse_పట్టిక[_RLIMIT_MAX])(స్థిరరాశి అక్షరం *val, rlim_t *ret) = {
        [RLIMIT_CPU]        = rlimit_parse_sec,
        [RLIMIT_FSIZE]      = rlimit_parse_కొలత,
        [RLIMIT_DATA]       = rlimit_parse_కొలత,
        [RLIMIT_STACK]      = rlimit_parse_కొలత,
        [RLIMIT_CORE]       = rlimit_parse_కొలత,
        [RLIMIT_RSS]        = rlimit_parse_కొలత,
        [RLIMIT_NOFILE]     = rlimit_parse_u64,
        [RLIMIT_AS]         = rlimit_parse_కొలత,
        [RLIMIT_NPROC]      = rlimit_parse_u64,
        [RLIMIT_MEMLOCK]    = rlimit_parse_కొలత,
        [RLIMIT_LOCKS]      = rlimit_parse_u64,
        [RLIMIT_SIGPENDING] = rlimit_parse_u64,
        [RLIMIT_MSGQUEUE]   = rlimit_parse_కొలత,
        [RLIMIT_NICE]       = rlimit_parse_nice,
        [RLIMIT_RTPRIO]     = rlimit_parse_u64,
        [RLIMIT_RTTIME]     = rlimit_parse_usec,
};

పూర్ణాంకం rlimit_parse_one(పూర్ణాంకం resource, స్థిరరాశి అక్షరం *val, rlim_t *ret) {
        ప్రతిజ్ఞా(val);
        ప్రతిజ్ఞా(ret);

        ఒకవేళ (resource < 0)
                తిరిగి -EINVAL;
        ఒకవేళ (resource >= _RLIMIT_MAX)
                తిరిగి -EINVAL;

        తిరిగి rlimit_parse_పట్టిక[resource](val, ret);
}

పూర్ణాంకం rlimit_parse(పూర్ణాంకం resource, స్థిరరాశి అక్షరం *val, నిర్మాణం rlimit *ret) {
        _cleanup_free_ అక్షరం *hard = ఖాళీ, *soft = ఖాళీ;
        rlim_t hl, sl;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(val);
        ప్రతిజ్ఞా(ret);

        r = వెలికితీయి_మొదటి_మాట(&val, &soft, ":", వెలికితీయి_DONT_COALESCE_SEPARATORS);
        ఒకవేళ (r < 0)
                తిరిగి r;
        ఒకవేళ (r == 0)
                తిరిగి -EINVAL;

        r = rlimit_parse_one(resource, soft, &sl);
        ఒకవేళ (r < 0)
                తిరిగి r;

        r = వెలికితీయి_మొదటి_మాట(&val, &hard, ":", వెలికితీయి_DONT_COALESCE_SEPARATORS);
        ఒకవేళ (r < 0)
                తిరిగి r;
        ఒకవేళ (!isempty(val))
                తిరిగి -EINVAL;
        ఒకవేళ (r == 0)
                hl = sl;
        లేకపోతే {
                r = rlimit_parse_one(resource, hard, &hl);
                ఒకవేళ (r < 0)
                        తిరిగి r;
                ఒకవేళ (sl > hl)
                        తిరిగి -EILSEQ;
        }

        *ret = (నిర్మాణం rlimit) {
                .rlim_cur = sl,
                .rlim_max = hl,
        };

        తిరిగి 0;
}

పూర్ణాంకం rlimit_format(స్థిరరాశి నిర్మాణం rlimit *rl, అక్షరం **ret) {
        అక్షరం *s;

        ప్రతిజ్ఞా(rl);
        ప్రతిజ్ఞా(ret);

        ఒకవేళ (rl->rlim_cur >= RLIM_INFINITY && rl->rlim_max >= RLIM_INFINITY)
                s = strdup("infinity");
        లేకపోతే ఒకవేళ (rl->rlim_cur >= RLIM_INFINITY)
                s = asprintf_సురక్షిత("infinity:" RLIM_FMT, rl->rlim_max);
        లేకపోతే ఒకవేళ (rl->rlim_max >= RLIM_INFINITY)
                s = asprintf_సురక్షిత(RLIM_FMT ":infinity", rl->rlim_cur);
        లేకపోతే ఒకవేళ (rl->rlim_cur == rl->rlim_max)
                s = asprintf_సురక్షిత(RLIM_FMT, rl->rlim_cur);
        లేకపోతే
                s = asprintf_సురక్షిత(RLIM_FMT ":" RLIM_FMT, rl->rlim_cur, rl->rlim_max);
        ఒకవేళ (!s)
                తిరిగి -ENOMEM;

        *ret = s;
        తిరిగి 0;
}

స్థిర స్థిరరాశి అక్షరం* స్థిరరాశి rlimit_పట్టిక[_RLIMIT_MAX] = {
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

DEFINE_STRING_పట్టిక_LOOKUP(rlimit, పూర్ణాంకం);

శూన్య rlimits_list(స్థిరరాశి అక్షరం *ఉపసర్గ) {
        FOREACH_ELEMENT(క్షేత్రం, rlimit_పట్టిక)
                రాయి("%s%s\n", strempty(ఉపసర్గ), *క్షేత్రం);
}

పూర్ణాంకం rlimit_from_string_harder(స్థిరరాశి అక్షరం *s) {
        స్థిరరాశి అక్షరం *suffix;

        /* The official prefix */
        suffix = startswith(s, "RLIMIT_");
        ఒకవేళ (suffix)
                తిరిగి rlimit_from_string(suffix);

        /* Our own unit file setting prefix */
        suffix = startswith(s, "Limit");
        ఒకవేళ (suffix)
                తిరిగి rlimit_from_string(suffix);

        తిరిగి rlimit_from_string(s);
}

శూన్య rlimit_free_all(నిర్మాణం rlimit **rl) {
        free_many((శూన్య**) rl, _RLIMIT_MAX);
}

పూర్ణాంకం rlimit_copy_all(నిర్మాణం rlimit* లక్ష్యం[స్థిర _RLIMIT_MAX], నిర్మాణం rlimit* స్థిరరాశి ఆధారం[స్థిర _RLIMIT_MAX]) {
        నిర్మాణం rlimit* copy[_RLIMIT_MAX] = {};

        ప్రతిజ్ఞా(లక్ష్యం);
        ప్రతిజ్ఞా(ఆధారం);

        కొరకు (పూర్ణాంకం i = 0; i < _RLIMIT_MAX; i++) {
                ఒకవేళ (!ఆధారం[i])
                        కొనసాగు;

                copy[i] = newdup(నిర్మాణం rlimit, ఆధారం[i], 1);
                ఒకవేళ (!copy[i]) {
                        rlimit_free_all(copy);
                        తిరిగి -ENOMEM;
                }
        }

        జ్ఞాపకం_నకలు(లక్ష్యం, copy, పరిమాణం(నిర్మాణం rlimit*) * _RLIMIT_MAX);
        తిరిగి 0;
}

పూర్ణాంకం rlimit_nofile_bump(పూర్ణాంకం limit) {
        పూర్ణాంకం r;

        /* Bumps the (soft) RLIMIT_NOFILE resource limit as close as possible to the specified limit. If a negative
         * limit is specified, bumps it to the maximum the kernel and the hard resource limit allows. This call should
         * be used by all our programs that might need a lot of fds, and that know how to deal with high fd numbers
         * (i.e. do not use select() — which chokes on fds >= 1024) */

        ఒకవేళ (limit < 0)
                limit = read_nr_open();

        ఒకవేళ (limit < 3)
                limit = 3;

        r = setrlimit_closest(RLIMIT_NOFILE, &RLIMIT_MAKE_CONST(limit));
        ఒకవేళ (r < 0)
                తిరిగి log_debug_errno(r, "Failed to set RLIMIT_NOFILE: %m");

        తిరిగి 0;
}

పూర్ణాంకం rlimit_nofile_సురక్షిత(శూన్య) {
        నిర్మాణం rlimit rl;

        /* Resets RLIMIT_NOFILE's soft limit FD_SETSIZE (i.e. 1024), for compatibility with software still using
         * select() */

        ఒకవేళ (getrlimit(RLIMIT_NOFILE, &rl) < 0)
                తిరిగి log_debug_errno(errno, "Failed to query RLIMIT_NOFILE: %m");

        ఒకవేళ (rl.rlim_cur <= FD_SETSIZE)
                తిరిగి 0;

        /* So we might have inherited a hard limit that's larger than the kernel's maximum limit as stored in
         * /proc/sys/fs/nr_open. If we pass this hard limit unmodified to setrlimit(), we'll get EPERM. To
         * make sure that doesn't happen, let's limit our hard limit to the value from nr_open. */
        rl.rlim_max = MIN(rl.rlim_max, (rlim_t) read_nr_open());
        rl.rlim_cur = MIN((rlim_t) FD_SETSIZE, rl.rlim_max);
        ఒకవేళ (setrlimit(RLIMIT_NOFILE, &rl) < 0)
                తిరిగి log_debug_errno(errno, "Failed to lower RLIMIT_NOFILE's soft limit to " RLIM_FMT ": %m", rl.rlim_cur);

        తిరిగి 1;
}

పూర్ణాంకం pid_getrlimit(pid_t pid, పూర్ణాంకం resource, నిర్మాణం rlimit *ret) {

        స్థిర స్థిరరాశి అక్షరం * స్థిరరాశి ఉపసర్గ_పట్టిక[_RLIMIT_MAX] = {
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

        పూర్ణాంకం r;

        ప్రతిజ్ఞా(resource >= 0);
        ప్రతిజ్ఞా(resource < _RLIMIT_MAX);
        ప్రతిజ్ఞా(pid >= 0);
        ప్రతిజ్ఞా(ret);

        ఒకవేళ (pid == 0 || pid == getpid_cached())
                తిరిగి RET_NERRNO(getrlimit(resource, ret));

        r = RET_NERRNO(prlimit(pid, resource, /* new_limit= */ ఖాళీ, ret));
        ఒకవేళ (!ERRNO_IS_NEG_PRIVILEGE(r))
                తిరిగి r;

        /* We don't have access? Then try to go via /proc/$PID/limits. Weirdly that's world readable in
         * contrast to querying the data via prlimit() */

        స్థిరరాశి అక్షరం *p = procfs_file_alloca(pid, "limits");
        _cleanup_free_ అక్షరం *limits = ఖాళీ;

        r = read_నిండిన_file(p, &limits, /* ret_size= */ ఖాళీ);
        ఒకవేళ (r < 0)
                తిరిగి -EPERM; /* propagate original permission error if we can't access the limits file */

        _cleanup_strv_free_ అక్షరం **l = ఖాళీ;
        l = strv_విభజించు_newlines(limits);
        ఒకవేళ (!l)
                తిరిగి -ENOMEM;

        STRV_FOREACH(i, strv_skip(l, 1)) {
                _cleanup_free_ అక్షరం *soft = ఖాళీ, *hard = ఖాళీ;
                uint64_t sv, hv;
                స్థిరరాశి అక్షరం *e;

                e = startswith(*i, ఉపసర్గ_పట్టిక[resource]);
                ఒకవేళ (!e)
                        కొనసాగు;

                ఒకవేళ (*e != ' ')
                        కొనసాగు;

                e += strspn(e, WHITESPACE);

                size_t n;
                n = strcspn(e, WHITESPACE);
                ఒకవేళ (n == 0)
                        కొనసాగు;

                soft = strndup(e, n);
                ఒకవేళ (!soft)
                        తిరిగి -ENOMEM;

                e += n;
                ఒకవేళ (*e != ' ')
                        కొనసాగు;

                e += strspn(e, WHITESPACE);
                n = strcspn(e, WHITESPACE);
                ఒకవేళ (n == 0)
                        కొనసాగు;

                hard = strndup(e, n);
                ఒకవేళ (!hard)
                        తిరిగి -ENOMEM;

                ఒకవేళ (streq(soft, "unlimited"))
                        sv = RLIM_INFINITY;
                లేకపోతే {
                        r = సురక్షిత_atou64(soft, &sv);
                        ఒకవేళ (r < 0)
                                తిరిగి r;
                }

                ఒకవేళ (streq(hard, "unlimited"))
                        hv = RLIM_INFINITY;
                లేకపోతే {
                        r = సురక్షిత_atou64(hard, &hv);
                        ఒకవేళ (r < 0)
                                తిరిగి r;
                }

                *ret = (నిర్మాణం rlimit) {
                        .rlim_cur = sv,
                        .rlim_max = hv,
                };

                తిరిగి 0;
        }

        తిరిగి -ENOTRECOVERABLE;
}
