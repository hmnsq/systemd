/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "errno-util.h"
#अन्तर्भाव "extract-word.h"
#अन्तर्भाव "fd-util.h"
#अन्तर्भाव "fileio.h"
#अन्तर्भाव "format-util.h"
#अन्तर्भाव "log.h"
#अन्तर्भाव "parse-util.h"
#अन्तर्भाव "process-util.h"
#अन्तर्भाव "rlimit-util.h"
#अन्तर्भाव "string-table.h"
#अन्तर्भाव "strv.h"
#अन्तर्भाव "time-util.h"

पूर्णाङ्क setrlimit_closest(पूर्णाङ्क resource, ध्रुव संरचना rlimit *rlim) {
        संरचना rlimit highest, fixed;

        प्रतिज्ञा(rlim);

        यदि (setrlimit(resource, rlim) >= 0)
                प्रत्यागम 0;

        यदि (errno != EPERM)
                प्रत्यागम -errno;

        /* So we failed to set the desired setrlimit, then let's try
         * to get as close as we can */
        यदि (getrlimit(resource, &highest) < 0)
                प्रत्यागम -errno;

        /* If the hard limit is unbounded anyway, then the EPERM had other reasons, let's propagate the original EPERM
         * then */
        यदि (highest.rlim_max == RLIM_INFINITY)
                प्रत्यागम -EPERM;

        fixed = (संरचना rlimit) {
                .rlim_cur = MIN(rlim->rlim_cur, highest.rlim_max),
                .rlim_max = MIN(rlim->rlim_max, highest.rlim_max),
        };

        /* Shortcut things if we wouldn't change anything. */
        यदि (fixed.rlim_cur == highest.rlim_cur &&
            fixed.rlim_max == highest.rlim_max)
                प्रत्यागम 0;

        log_debug("Failed at setting rlimit " RLIM_FMT " for resource RLIMIT_%s. Will attempt setting value " RLIM_FMT " instead.", rlim->rlim_max, rlimit_to_string(resource), fixed.rlim_max);

        प्रत्यागम RET_NERRNO(setrlimit(resource, &fixed));
}

पूर्णाङ्क setrlimit_closest_all(ध्रुव संरचना rlimit *ध्रुव *rlim, पूर्णाङ्क *which_failed) {
        पूर्णाङ्क r;

        प्रतिज्ञा(rlim);

        /* On failure returns the limit's index that failed in *which_failed, but only if non-NULL */

        प्रति (पूर्णाङ्क i = 0; i < _RLIMIT_MAX; i++) {
                यदि (!rlim[i])
                        अनुवर्त;

                r = setrlimit_closest(i, rlim[i]);
                यदि (r < 0) {
                        यदि (which_failed)
                                *which_failed = i;

                        प्रत्यागम r;
                }
        }

        यदि (which_failed)
                *which_failed = -1;

        प्रत्यागम 0;
}

स्थिर पूर्णाङ्क rlimit_parse_u64(ध्रुव वर्ण *val, rlim_t *ret) {
        uint64_t u;
        पूर्णाङ्क r;

        प्रतिज्ञा(val);
        प्रतिज्ञा(ret);

        यदि (streq(val, "infinity")) {
                *ret = RLIM_INFINITY;
                प्रत्यागम 0;
        }

        /* setrlimit(2) suggests rlim_t is always 64-bit on Linux. */
        assert_cc(परिमाण(rlim_t) == परिमाण(uint64_t));

        r = safe_atou64(val, &u);
        यदि (r < 0)
                प्रत्यागम r;
        यदि (u >= (uint64_t) RLIM_INFINITY)
                प्रत्यागम -ERANGE;

        *ret = (rlim_t) u;
        प्रत्यागम 0;
}

स्थिर पूर्णाङ्क rlimit_parse_size(ध्रुव वर्ण *val, rlim_t *ret) {
        uint64_t u;
        पूर्णाङ्क r;

        प्रतिज्ञा(val);
        प्रतिज्ञा(ret);

        यदि (streq(val, "infinity")) {
                *ret = RLIM_INFINITY;
                प्रत्यागम 0;
        }

        r = parse_size(val, 1024, &u);
        यदि (r < 0)
                प्रत्यागम r;
        यदि (u >= (uint64_t) RLIM_INFINITY)
                प्रत्यागम -ERANGE;

        *ret = (rlim_t) u;
        प्रत्यागम 0;
}

स्थिर पूर्णाङ्क rlimit_parse_sec(ध्रुव वर्ण *val, rlim_t *ret) {
        uint64_t u;
        usec_t t;
        पूर्णाङ्क r;

        प्रतिज्ञा(val);
        प्रतिज्ञा(ret);

        r = parse_sec(val, &t);
        यदि (r < 0)
                प्रत्यागम r;
        यदि (t == USEC_INFINITY) {
                *ret = RLIM_INFINITY;
                प्रत्यागम 0;
        }

        u = (uint64_t) DIV_ROUND_UP(t, USEC_PER_SEC);
        यदि (u >= (uint64_t) RLIM_INFINITY)
                प्रत्यागम -ERANGE;

        *ret = (rlim_t) u;
        प्रत्यागम 0;
}

स्थिर पूर्णाङ्क rlimit_parse_usec(ध्रुव वर्ण *val, rlim_t *ret) {
        usec_t t;
        पूर्णाङ्क r;

        प्रतिज्ञा(val);
        प्रतिज्ञा(ret);

        r = parse_time(val, &t, 1);
        यदि (r < 0)
                प्रत्यागम r;
        यदि (t == USEC_INFINITY) {
                *ret = RLIM_INFINITY;
                प्रत्यागम 0;
        }

        *ret = (rlim_t) t;
        प्रत्यागम 0;
}

स्थिर पूर्णाङ्क rlimit_parse_nice(ध्रुव वर्ण *val, rlim_t *ret) {
        uint64_t rl;
        पूर्णाङ्क r;

        प्रतिज्ञा(val);
        प्रतिज्ञा(ret);

        /* So, Linux is weird. The range for RLIMIT_NICE is 40..1, mapping to the nice levels -20..19. However, the
         * RLIMIT_NICE limit defaults to 0 by the kernel, i.e. a value that maps to nice level 20, which of course is
         * bogus and does not exist. In order to permit parsing the RLIMIT_NICE of 0 here we hence implement a slight
         * asymmetry: when parsing as positive nice level we permit 0..19. When parsing as negative nice level, we
         * permit -20..0. But when parsing as raw resource limit value then we also allow the special value 0.
         *
         * Yeah, Linux is quality engineering sometimes... */

        यदि (val[0] == '+') {

                /* Prefixed with "+": Parse as positive user-friendly nice value */
                r = safe_atou64(val + 1, &rl);
                यदि (r < 0)
                        प्रत्यागम r;

                यदि (rl >= PRIO_MAX)
                        प्रत्यागम -ERANGE;

                rl = 20 - rl;

        } अन्यथा यदि (val[0] == '-') {

                /* Prefixed with "-": Parse as negative user-friendly nice value */
                r = safe_atou64(val + 1, &rl);
                यदि (r < 0)
                        प्रत्यागम r;

                यदि (rl > (uint64_t) (-PRIO_MIN))
                        प्रत्यागम -ERANGE;

                rl = 20 + rl;
        } अन्यथा {

                /* Not prefixed: parse as raw resource limit value */
                r = safe_atou64(val, &rl);
                यदि (r < 0)
                        प्रत्यागम r;

                यदि (rl > (uint64_t) (20 - PRIO_MIN))
                        प्रत्यागम -ERANGE;
        }

        *ret = (rlim_t) rl;
        प्रत्यागम 0;
}

स्थिर पूर्णाङ्क (*ध्रुव rlimit_parse_table[_RLIMIT_MAX])(ध्रुव वर्ण *val, rlim_t *ret) = {
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

पूर्णाङ्क rlimit_parse_one(पूर्णाङ्क resource, ध्रुव वर्ण *val, rlim_t *ret) {
        प्रतिज्ञा(val);
        प्रतिज्ञा(ret);

        यदि (resource < 0)
                प्रत्यागम -EINVAL;
        यदि (resource >= _RLIMIT_MAX)
                प्रत्यागम -EINVAL;

        प्रत्यागम rlimit_parse_table[resource](val, ret);
}

पूर्णाङ्क rlimit_parse(पूर्णाङ्क resource, ध्रुव वर्ण *val, संरचना rlimit *ret) {
        _cleanup_free_ वर्ण *hard = रिक्त, *soft = रिक्त;
        rlim_t hl, sl;
        पूर्णाङ्क r;

        प्रतिज्ञा(val);
        प्रतिज्ञा(ret);

        r = extract_first_word(&val, &soft, ":", EXTRACT_DONT_COALESCE_SEPARATORS);
        यदि (r < 0)
                प्रत्यागम r;
        यदि (r == 0)
                प्रत्यागम -EINVAL;

        r = rlimit_parse_one(resource, soft, &sl);
        यदि (r < 0)
                प्रत्यागम r;

        r = extract_first_word(&val, &hard, ":", EXTRACT_DONT_COALESCE_SEPARATORS);
        यदि (r < 0)
                प्रत्यागम r;
        यदि (!isempty(val))
                प्रत्यागम -EINVAL;
        यदि (r == 0)
                hl = sl;
        अन्यथा {
                r = rlimit_parse_one(resource, hard, &hl);
                यदि (r < 0)
                        प्रत्यागम r;
                यदि (sl > hl)
                        प्रत्यागम -EILSEQ;
        }

        *ret = (संरचना rlimit) {
                .rlim_cur = sl,
                .rlim_max = hl,
        };

        प्रत्यागम 0;
}

पूर्णाङ्क rlimit_format(ध्रुव संरचना rlimit *rl, वर्ण **ret) {
        वर्ण *s;

        प्रतिज्ञा(rl);
        प्रतिज्ञा(ret);

        यदि (rl->rlim_cur >= RLIM_INFINITY && rl->rlim_max >= RLIM_INFINITY)
                s = strdup("infinity");
        अन्यथा यदि (rl->rlim_cur >= RLIM_INFINITY)
                s = asprintf_safe("infinity:" RLIM_FMT, rl->rlim_max);
        अन्यथा यदि (rl->rlim_max >= RLIM_INFINITY)
                s = asprintf_safe(RLIM_FMT ":infinity", rl->rlim_cur);
        अन्यथा यदि (rl->rlim_cur == rl->rlim_max)
                s = asprintf_safe(RLIM_FMT, rl->rlim_cur);
        अन्यथा
                s = asprintf_safe(RLIM_FMT ":" RLIM_FMT, rl->rlim_cur, rl->rlim_max);
        यदि (!s)
                प्रत्यागम -ENOMEM;

        *ret = s;
        प्रत्यागम 0;
}

स्थिर ध्रुव वर्ण* ध्रुव rlimit_table[_RLIMIT_MAX] = {
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

DEFINE_STRING_TABLE_LOOKUP(rlimit, पूर्णाङ्क);

शून्य rlimits_list(ध्रुव वर्ण *prefix) {
        FOREACH_ELEMENT(field, rlimit_table)
                printf("%s%s\n", strempty(prefix), *field);
}

पूर्णाङ्क rlimit_from_string_harder(ध्रुव वर्ण *s) {
        ध्रुव वर्ण *suffix;

        /* The official prefix */
        suffix = startswith(s, "RLIMIT_");
        यदि (suffix)
                प्रत्यागम rlimit_from_string(suffix);

        /* Our own unit file setting prefix */
        suffix = startswith(s, "Limit");
        यदि (suffix)
                प्रत्यागम rlimit_from_string(suffix);

        प्रत्यागम rlimit_from_string(s);
}

शून्य rlimit_free_all(संरचना rlimit **rl) {
        free_many((शून्य**) rl, _RLIMIT_MAX);
}

पूर्णाङ्क rlimit_copy_all(संरचना rlimit* target[स्थिर _RLIMIT_MAX], संरचना rlimit* ध्रुव source[स्थिर _RLIMIT_MAX]) {
        संरचना rlimit* copy[_RLIMIT_MAX] = {};

        प्रतिज्ञा(target);
        प्रतिज्ञा(source);

        प्रति (पूर्णाङ्क i = 0; i < _RLIMIT_MAX; i++) {
                यदि (!source[i])
                        अनुवर्त;

                copy[i] = newdup(संरचना rlimit, source[i], 1);
                यदि (!copy[i]) {
                        rlimit_free_all(copy);
                        प्रत्यागम -ENOMEM;
                }
        }

        memcpy(target, copy, परिमाण(संरचना rlimit*) * _RLIMIT_MAX);
        प्रत्यागम 0;
}

पूर्णाङ्क rlimit_nofile_bump(पूर्णाङ्क limit) {
        पूर्णाङ्क r;

        /* Bumps the (soft) RLIMIT_NOFILE resource limit as close as possible to the specified limit. If a negative
         * limit is specified, bumps it to the maximum the kernel and the hard resource limit allows. This call should
         * be used by all our programs that might need a lot of fds, and that know how to deal with high fd numbers
         * (i.e. do not use select() — which chokes on fds >= 1024) */

        यदि (limit < 0)
                limit = read_nr_open();

        यदि (limit < 3)
                limit = 3;

        r = setrlimit_closest(RLIMIT_NOFILE, &RLIMIT_MAKE_CONST(limit));
        यदि (r < 0)
                प्रत्यागम log_debug_errno(r, "Failed to set RLIMIT_NOFILE: %m");

        प्रत्यागम 0;
}

पूर्णाङ्क rlimit_nofile_safe(शून्य) {
        संरचना rlimit rl;

        /* Resets RLIMIT_NOFILE's soft limit FD_SETSIZE (i.e. 1024), for compatibility with software still using
         * select() */

        यदि (getrlimit(RLIMIT_NOFILE, &rl) < 0)
                प्रत्यागम log_debug_errno(errno, "Failed to query RLIMIT_NOFILE: %m");

        यदि (rl.rlim_cur <= FD_SETSIZE)
                प्रत्यागम 0;

        /* So we might have inherited a hard limit that's larger than the kernel's maximum limit as stored in
         * /proc/sys/fs/nr_open. If we pass this hard limit unmodified to setrlimit(), we'll get EPERM. To
         * make sure that doesn't happen, let's limit our hard limit to the value from nr_open. */
        rl.rlim_max = MIN(rl.rlim_max, (rlim_t) read_nr_open());
        rl.rlim_cur = MIN((rlim_t) FD_SETSIZE, rl.rlim_max);
        यदि (setrlimit(RLIMIT_NOFILE, &rl) < 0)
                प्रत्यागम log_debug_errno(errno, "Failed to lower RLIMIT_NOFILE's soft limit to " RLIM_FMT ": %m", rl.rlim_cur);

        प्रत्यागम 1;
}

पूर्णाङ्क pid_getrlimit(pid_t pid, पूर्णाङ्क resource, संरचना rlimit *ret) {

        स्थिर ध्रुव वर्ण * ध्रुव prefix_table[_RLIMIT_MAX] = {
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

        पूर्णाङ्क r;

        प्रतिज्ञा(resource >= 0);
        प्रतिज्ञा(resource < _RLIMIT_MAX);
        प्रतिज्ञा(pid >= 0);
        प्रतिज्ञा(ret);

        यदि (pid == 0 || pid == getpid_cached())
                प्रत्यागम RET_NERRNO(getrlimit(resource, ret));

        r = RET_NERRNO(prlimit(pid, resource, /* new_limit= */ रिक्त, ret));
        यदि (!ERRNO_IS_NEG_PRIVILEGE(r))
                प्रत्यागम r;

        /* We don't have access? Then try to go via /proc/$PID/limits. Weirdly that's world readable in
         * contrast to querying the data via prlimit() */

        ध्रुव वर्ण *p = procfs_file_alloca(pid, "limits");
        _cleanup_free_ वर्ण *limits = रिक्त;

        r = read_full_file(p, &limits, /* ret_size= */ रिक्त);
        यदि (r < 0)
                प्रत्यागम -EPERM; /* propagate original permission error if we can't access the limits file */

        _cleanup_strv_free_ वर्ण **l = रिक्त;
        l = strv_split_newlines(limits);
        यदि (!l)
                प्रत्यागम -ENOMEM;

        STRV_FOREACH(i, strv_skip(l, 1)) {
                _cleanup_free_ वर्ण *soft = रिक्त, *hard = रिक्त;
                uint64_t sv, hv;
                ध्रुव वर्ण *e;

                e = startswith(*i, prefix_table[resource]);
                यदि (!e)
                        अनुवर्त;

                यदि (*e != ' ')
                        अनुवर्त;

                e += strspn(e, WHITESPACE);

                size_t n;
                n = strcspn(e, WHITESPACE);
                यदि (n == 0)
                        अनुवर्त;

                soft = strndup(e, n);
                यदि (!soft)
                        प्रत्यागम -ENOMEM;

                e += n;
                यदि (*e != ' ')
                        अनुवर्त;

                e += strspn(e, WHITESPACE);
                n = strcspn(e, WHITESPACE);
                यदि (n == 0)
                        अनुवर्त;

                hard = strndup(e, n);
                यदि (!hard)
                        प्रत्यागम -ENOMEM;

                यदि (streq(soft, "unlimited"))
                        sv = RLIM_INFINITY;
                अन्यथा {
                        r = safe_atou64(soft, &sv);
                        यदि (r < 0)
                                प्रत्यागम r;
                }

                यदि (streq(hard, "unlimited"))
                        hv = RLIM_INFINITY;
                अन्यथा {
                        r = safe_atou64(hard, &hv);
                        यदि (r < 0)
                                प्रत्यागम r;
                }

                *ret = (संरचना rlimit) {
                        .rlim_cur = sv,
                        .rlim_max = hv,
                };

                प्रत्यागम 0;
        }

        प्रत्यागम -ENOTRECOVERABLE;
}
