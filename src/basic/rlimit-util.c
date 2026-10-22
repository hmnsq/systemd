/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश "alloc-util.h"
#समावेश "errno-util.h"
#समावेश "extract-word.h"
#समावेश "fd-util.h"
#समावेश "fileio.h"
#समावेश "format-util.h"
#समावेश "log.h"
#समावेश "parse-util.h"
#समावेश "process-util.h"
#समावेश "rlimit-util.h"
#समावेश "string-table.h"
#समावेश "strv.h"
#समावेश "time-util.h"

पूर्णांक setrlimit_closest(पूर्णांक resource, स्थिर काष्ठा rlimit *rlim) {
        काष्ठा rlimit highest, fixed;

        निश्चित(rlim);

        अगर (setrlimit(resource, rlim) >= 0)
                वापस 0;

        अगर (त्रुटि_सं != EPERM)
                वापस -त्रुटि_सं;

        /* So we failed to set the desired setrlimit, then let's try
         * to get as close as we can */
        अगर (getrlimit(resource, &highest) < 0)
                वापस -त्रुटि_सं;

        /* If the hard limit is unbounded anyway, then the EPERM had other reasons, let's propagate the original EPERM
         * then */
        अगर (highest.rlim_max == RLIM_INFINITY)
                वापस -EPERM;

        fixed = (काष्ठा rlimit) {
                .rlim_cur = MIN(rlim->rlim_cur, highest.rlim_max),
                .rlim_max = MIN(rlim->rlim_max, highest.rlim_max),
        };

        /* Shortcut things if we wouldn't change anything. */
        अगर (fixed.rlim_cur == highest.rlim_cur &&
            fixed.rlim_max == highest.rlim_max)
                वापस 0;

        log_debug("Failed at setting rlimit " RLIM_FMT " for resource RLIMIT_%s. Will attempt setting value " RLIM_FMT " instead.", rlim->rlim_max, rlimit_to_string(resource), fixed.rlim_max);

        वापस RET_NERRNO(setrlimit(resource, &fixed));
}

पूर्णांक setrlimit_closest_all(स्थिर काष्ठा rlimit *स्थिर *rlim, पूर्णांक *which_failed) {
        पूर्णांक r;

        निश्चित(rlim);

        /* On failure returns the limit's index that failed in *which_failed, but only if non-NULL */

        क्रम (पूर्णांक i = 0; i < _RLIMIT_MAX; i++) {
                अगर (!rlim[i])
                        जारी;

                r = setrlimit_closest(i, rlim[i]);
                अगर (r < 0) {
                        अगर (which_failed)
                                *which_failed = i;

                        वापस r;
                }
        }

        अगर (which_failed)
                *which_failed = -1;

        वापस 0;
}

अटल पूर्णांक rlimit_parse_u64(स्थिर अक्षर *val, rlim_t *ret) {
        uint64_t u;
        पूर्णांक r;

        निश्चित(val);
        निश्चित(ret);

        अगर (streq(val, "infinity")) {
                *ret = RLIM_INFINITY;
                वापस 0;
        }

        /* setrlimit(2) suggests rlim_t is always 64-bit on Linux. */
        assert_cc(माप(rlim_t) == माप(uint64_t));

        r = safe_atou64(val, &u);
        अगर (r < 0)
                वापस r;
        अगर (u >= (uint64_t) RLIM_INFINITY)
                वापस -दुस्फल;

        *ret = (rlim_t) u;
        वापस 0;
}

अटल पूर्णांक rlimit_parse_आकार(स्थिर अक्षर *val, rlim_t *ret) {
        uint64_t u;
        पूर्णांक r;

        निश्चित(val);
        निश्चित(ret);

        अगर (streq(val, "infinity")) {
                *ret = RLIM_INFINITY;
                वापस 0;
        }

        r = parse_आकार(val, 1024, &u);
        अगर (r < 0)
                वापस r;
        अगर (u >= (uint64_t) RLIM_INFINITY)
                वापस -दुस्फल;

        *ret = (rlim_t) u;
        वापस 0;
}

अटल पूर्णांक rlimit_parse_sec(स्थिर अक्षर *val, rlim_t *ret) {
        uint64_t u;
        usec_t t;
        पूर्णांक r;

        निश्चित(val);
        निश्चित(ret);

        r = parse_sec(val, &t);
        अगर (r < 0)
                वापस r;
        अगर (t == USEC_INFINITY) {
                *ret = RLIM_INFINITY;
                वापस 0;
        }

        u = (uint64_t) DIV_ROUND_UP(t, USEC_PER_SEC);
        अगर (u >= (uint64_t) RLIM_INFINITY)
                वापस -दुस्फल;

        *ret = (rlim_t) u;
        वापस 0;
}

अटल पूर्णांक rlimit_parse_usec(स्थिर अक्षर *val, rlim_t *ret) {
        usec_t t;
        पूर्णांक r;

        निश्चित(val);
        निश्चित(ret);

        r = parse_time(val, &t, 1);
        अगर (r < 0)
                वापस r;
        अगर (t == USEC_INFINITY) {
                *ret = RLIM_INFINITY;
                वापस 0;
        }

        *ret = (rlim_t) t;
        वापस 0;
}

अटल पूर्णांक rlimit_parse_nice(स्थिर अक्षर *val, rlim_t *ret) {
        uint64_t rl;
        पूर्णांक r;

        निश्चित(val);
        निश्चित(ret);

        /* So, Linux is weird. The range for RLIMIT_NICE is 40..1, mapping to the nice levels -20..19. However, the
         * RLIMIT_NICE limit defaults to 0 by the kernel, i.e. a value that maps to nice level 20, which of course is
         * bogus and does not exist. In order to permit parsing the RLIMIT_NICE of 0 here we hence implement a slight
         * asymmetry: when parsing as positive nice level we permit 0..19. When parsing as negative nice level, we
         * permit -20..0. But when parsing as raw resource limit value then we also allow the special value 0.
         *
         * Yeah, Linux is quality engineering sometimes... */

        अगर (val[0] == '+') {

                /* Prefixed with "+": Parse as positive user-friendly nice value */
                r = safe_atou64(val + 1, &rl);
                अगर (r < 0)
                        वापस r;

                अगर (rl >= PRIO_MAX)
                        वापस -दुस्फल;

                rl = 20 - rl;

        } अन्यथा अगर (val[0] == '-') {

                /* Prefixed with "-": Parse as negative user-friendly nice value */
                r = safe_atou64(val + 1, &rl);
                अगर (r < 0)
                        वापस r;

                अगर (rl > (uint64_t) (-PRIO_MIN))
                        वापस -दुस्फल;

                rl = 20 + rl;
        } अन्यथा {

                /* Not prefixed: parse as raw resource limit value */
                r = safe_atou64(val, &rl);
                अगर (r < 0)
                        वापस r;

                अगर (rl > (uint64_t) (20 - PRIO_MIN))
                        वापस -दुस्फल;
        }

        *ret = (rlim_t) rl;
        वापस 0;
}

अटल पूर्णांक (*स्थिर rlimit_parse_table[_RLIMIT_MAX])(स्थिर अक्षर *val, rlim_t *ret) = {
        [RLIMIT_CPU]        = rlimit_parse_sec,
        [RLIMIT_FSIZE]      = rlimit_parse_आकार,
        [RLIMIT_DATA]       = rlimit_parse_आकार,
        [RLIMIT_STACK]      = rlimit_parse_आकार,
        [RLIMIT_CORE]       = rlimit_parse_आकार,
        [RLIMIT_RSS]        = rlimit_parse_आकार,
        [RLIMIT_NOFILE]     = rlimit_parse_u64,
        [RLIMIT_AS]         = rlimit_parse_आकार,
        [RLIMIT_NPROC]      = rlimit_parse_u64,
        [RLIMIT_MEMLOCK]    = rlimit_parse_आकार,
        [RLIMIT_LOCKS]      = rlimit_parse_u64,
        [RLIMIT_SIGPENDING] = rlimit_parse_u64,
        [RLIMIT_MSGQUEUE]   = rlimit_parse_आकार,
        [RLIMIT_NICE]       = rlimit_parse_nice,
        [RLIMIT_RTPRIO]     = rlimit_parse_u64,
        [RLIMIT_RTTIME]     = rlimit_parse_usec,
};

पूर्णांक rlimit_parse_one(पूर्णांक resource, स्थिर अक्षर *val, rlim_t *ret) {
        निश्चित(val);
        निश्चित(ret);

        अगर (resource < 0)
                वापस -EINVAL;
        अगर (resource >= _RLIMIT_MAX)
                वापस -EINVAL;

        वापस rlimit_parse_table[resource](val, ret);
}

पूर्णांक rlimit_parse(पूर्णांक resource, स्थिर अक्षर *val, काष्ठा rlimit *ret) {
        _cleanup_free_ अक्षर *hard = शून्य, *soft = शून्य;
        rlim_t hl, sl;
        पूर्णांक r;

        निश्चित(val);
        निश्चित(ret);

        r = extract_first_word(&val, &soft, ":", EXTRACT_DONT_COALESCE_SEPARATORS);
        अगर (r < 0)
                वापस r;
        अगर (r == 0)
                वापस -EINVAL;

        r = rlimit_parse_one(resource, soft, &sl);
        अगर (r < 0)
                वापस r;

        r = extract_first_word(&val, &hard, ":", EXTRACT_DONT_COALESCE_SEPARATORS);
        अगर (r < 0)
                वापस r;
        अगर (!isempty(val))
                वापस -EINVAL;
        अगर (r == 0)
                hl = sl;
        अन्यथा {
                r = rlimit_parse_one(resource, hard, &hl);
                अगर (r < 0)
                        वापस r;
                अगर (sl > hl)
                        वापस -EILSEQ;
        }

        *ret = (काष्ठा rlimit) {
                .rlim_cur = sl,
                .rlim_max = hl,
        };

        वापस 0;
}

पूर्णांक rlimit_format(स्थिर काष्ठा rlimit *rl, अक्षर **ret) {
        अक्षर *s;

        निश्चित(rl);
        निश्चित(ret);

        अगर (rl->rlim_cur >= RLIM_INFINITY && rl->rlim_max >= RLIM_INFINITY)
                s = strdup("infinity");
        अन्यथा अगर (rl->rlim_cur >= RLIM_INFINITY)
                s = asprintf_safe("infinity:" RLIM_FMT, rl->rlim_max);
        अन्यथा अगर (rl->rlim_max >= RLIM_INFINITY)
                s = asprintf_safe(RLIM_FMT ":infinity", rl->rlim_cur);
        अन्यथा अगर (rl->rlim_cur == rl->rlim_max)
                s = asprintf_safe(RLIM_FMT, rl->rlim_cur);
        अन्यथा
                s = asprintf_safe(RLIM_FMT ":" RLIM_FMT, rl->rlim_cur, rl->rlim_max);
        अगर (!s)
                वापस -ENOMEM;

        *ret = s;
        वापस 0;
}

अटल स्थिर अक्षर* स्थिर rlimit_table[_RLIMIT_MAX] = {
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

DEFINE_STRING_TABLE_LOOKUP(rlimit, पूर्णांक);

व्योम rlimits_list(स्थिर अक्षर *prefix) {
        FOREACH_ELEMENT(field, rlimit_table)
                म_लिखो("%s%s\n", strempty(prefix), *field);
}

पूर्णांक rlimit_from_string_harder(स्थिर अक्षर *s) {
        स्थिर अक्षर *suffix;

        /* The official prefix */
        suffix = startswith(s, "RLIMIT_");
        अगर (suffix)
                वापस rlimit_from_string(suffix);

        /* Our own unit file setting prefix */
        suffix = startswith(s, "Limit");
        अगर (suffix)
                वापस rlimit_from_string(suffix);

        वापस rlimit_from_string(s);
}

व्योम rlimit_free_all(काष्ठा rlimit **rl) {
        free_many((व्योम**) rl, _RLIMIT_MAX);
}

पूर्णांक rlimit_copy_all(काष्ठा rlimit* target[अटल _RLIMIT_MAX], काष्ठा rlimit* स्थिर source[अटल _RLIMIT_MAX]) {
        काष्ठा rlimit* copy[_RLIMIT_MAX] = {};

        निश्चित(target);
        निश्चित(source);

        क्रम (पूर्णांक i = 0; i < _RLIMIT_MAX; i++) {
                अगर (!source[i])
                        जारी;

                copy[i] = newdup(काष्ठा rlimit, source[i], 1);
                अगर (!copy[i]) {
                        rlimit_free_all(copy);
                        वापस -ENOMEM;
                }
        }

        स_नकल(target, copy, माप(काष्ठा rlimit*) * _RLIMIT_MAX);
        वापस 0;
}

पूर्णांक rlimit_nofile_bump(पूर्णांक limit) {
        पूर्णांक r;

        /* Bumps the (soft) RLIMIT_NOFILE resource limit as close as possible to the specified limit. If a negative
         * limit is specified, bumps it to the maximum the kernel and the hard resource limit allows. This call should
         * be used by all our programs that might need a lot of fds, and that know how to deal with high fd numbers
         * (i.e. do not use select() — which chokes on fds >= 1024) */

        अगर (limit < 0)
                limit = read_nr_open();

        अगर (limit < 3)
                limit = 3;

        r = setrlimit_closest(RLIMIT_NOFILE, &RLIMIT_MAKE_CONST(limit));
        अगर (r < 0)
                वापस log_debug_errno(r, "Failed to set RLIMIT_NOFILE: %m");

        वापस 0;
}

पूर्णांक rlimit_nofile_safe(व्योम) {
        काष्ठा rlimit rl;

        /* Resets RLIMIT_NOFILE's soft limit FD_SETSIZE (i.e. 1024), for compatibility with software still using
         * select() */

        अगर (getrlimit(RLIMIT_NOFILE, &rl) < 0)
                वापस log_debug_errno(त्रुटि_सं, "Failed to query RLIMIT_NOFILE: %m");

        अगर (rl.rlim_cur <= FD_SETSIZE)
                वापस 0;

        /* So we might have inherited a hard limit that's larger than the kernel's maximum limit as stored in
         * /proc/sys/fs/nr_open. If we pass this hard limit unmodified to setrlimit(), we'll get EPERM. To
         * make sure that doesn't happen, let's limit our hard limit to the value from nr_open. */
        rl.rlim_max = MIN(rl.rlim_max, (rlim_t) read_nr_open());
        rl.rlim_cur = MIN((rlim_t) FD_SETSIZE, rl.rlim_max);
        अगर (setrlimit(RLIMIT_NOFILE, &rl) < 0)
                वापस log_debug_errno(त्रुटि_सं, "Failed to lower RLIMIT_NOFILE's soft limit to " RLIM_FMT ": %m", rl.rlim_cur);

        वापस 1;
}

पूर्णांक pid_getrlimit(pid_t pid, पूर्णांक resource, काष्ठा rlimit *ret) {

        अटल स्थिर अक्षर * स्थिर prefix_table[_RLIMIT_MAX] = {
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

        पूर्णांक r;

        निश्चित(resource >= 0);
        निश्चित(resource < _RLIMIT_MAX);
        निश्चित(pid >= 0);
        निश्चित(ret);

        अगर (pid == 0 || pid == getpid_cached())
                वापस RET_NERRNO(getrlimit(resource, ret));

        r = RET_NERRNO(prlimit(pid, resource, /* new_limit= */ शून्य, ret));
        अगर (!ERRNO_IS_NEG_PRIVILEGE(r))
                वापस r;

        /* We don't have access? Then try to go via /proc/$PID/limits. Weirdly that's world readable in
         * contrast to querying the data via prlimit() */

        स्थिर अक्षर *p = procfs_file_alloca(pid, "limits");
        _cleanup_free_ अक्षर *limits = शून्य;

        r = read_full_file(p, &limits, /* ret_size= */ शून्य);
        अगर (r < 0)
                वापस -EPERM; /* propagate original permission error if we can't access the limits file */

        _cleanup_strv_free_ अक्षर **l = शून्य;
        l = strv_split_newlines(limits);
        अगर (!l)
                वापस -ENOMEM;

        STRV_FOREACH(i, strv_skip(l, 1)) {
                _cleanup_free_ अक्षर *soft = शून्य, *hard = शून्य;
                uint64_t sv, hv;
                स्थिर अक्षर *e;

                e = startswith(*i, prefix_table[resource]);
                अगर (!e)
                        जारी;

                अगर (*e != ' ')
                        जारी;

                e += म_अखोज(e, WHITESPACE);

                माप_प्रकार n;
                n = म_खोज(e, WHITESPACE);
                अगर (n == 0)
                        जारी;

                soft = strndup(e, n);
                अगर (!soft)
                        वापस -ENOMEM;

                e += n;
                अगर (*e != ' ')
                        जारी;

                e += म_अखोज(e, WHITESPACE);
                n = म_खोज(e, WHITESPACE);
                अगर (n == 0)
                        जारी;

                hard = strndup(e, n);
                अगर (!hard)
                        वापस -ENOMEM;

                अगर (streq(soft, "unlimited"))
                        sv = RLIM_INFINITY;
                अन्यथा {
                        r = safe_atou64(soft, &sv);
                        अगर (r < 0)
                                वापस r;
                }

                अगर (streq(hard, "unlimited"))
                        hv = RLIM_INFINITY;
                अन्यथा {
                        r = safe_atou64(hard, &hv);
                        अगर (r < 0)
                                वापस r;
                }

                *ret = (काष्ठा rlimit) {
                        .rlim_cur = sv,
                        .rlim_max = hv,
                };

                वापस 0;
        }

        वापस -ENOTRECOVERABLE;
}
