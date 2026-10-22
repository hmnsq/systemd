/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ "alloc-util.h"
#અન્તર્ભાવ "errno-util.h"
#અન્તર્ભાવ "extract-word.h"
#અન્તર્ભાવ "fd-util.h"
#અન્તર્ભાવ "fileio.h"
#અન્તર્ભાવ "format-util.h"
#અન્તર્ભાવ "log.h"
#અન્તર્ભાવ "parse-util.h"
#અન્તર્ભાવ "process-util.h"
#અન્તર્ભાવ "rlimit-util.h"
#અન્તર્ભાવ "string-table.h"
#અન્તર્ભાવ "strv.h"
#અન્તર્ભાવ "time-util.h"

પૂર્ણાંક setrlimit_closest(પૂર્ણાંક resource, અચલ સંરચના rlimit *rlim) {
        સંરચના rlimit highest, fixed;

        પ્રતિજ્ઞા(rlim);

        જો (setrlimit(resource, rlim) >= 0)
                પાછા 0;

        જો (errno != EPERM)
                પાછા -errno;

        /* So we failed to set the desired setrlimit, then let's try
         * to get as close as we can */
        જો (getrlimit(resource, &highest) < 0)
                પાછા -errno;

        /* If the hard limit is unbounded anyway, then the EPERM had other reasons, let's propagate the original EPERM
         * then */
        જો (highest.rlim_max == RLIM_INFINITY)
                પાછા -EPERM;

        fixed = (સંરચના rlimit) {
                .rlim_cur = MIN(rlim->rlim_cur, highest.rlim_max),
                .rlim_max = MIN(rlim->rlim_max, highest.rlim_max),
        };

        /* Shortcut things if we wouldn't change anything. */
        જો (fixed.rlim_cur == highest.rlim_cur &&
            fixed.rlim_max == highest.rlim_max)
                પાછા 0;

        log_debug("Failed at setting rlimit " RLIM_FMT " for resource RLIMIT_%s. Will attempt setting value " RLIM_FMT " instead.", rlim->rlim_max, rlimit_to_string(resource), fixed.rlim_max);

        પાછા RET_NERRNO(setrlimit(resource, &fixed));
}

પૂર્ણાંક setrlimit_closest_all(અચલ સંરચના rlimit *અચલ *rlim, પૂર્ણાંક *which_failed) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(rlim);

        /* On failure returns the limit's index that failed in *which_failed, but only if non-NULL */

        માટે (પૂર્ણાંક i = 0; i < _RLIMIT_MAX; i++) {
                જો (!rlim[i])
                        ચાલુ;

                r = setrlimit_closest(i, rlim[i]);
                જો (r < 0) {
                        જો (which_failed)
                                *which_failed = i;

                        પાછા r;
                }
        }

        જો (which_failed)
                *which_failed = -1;

        પાછા 0;
}

સ્થિર પૂર્ણાંક rlimit_parse_u64(અચલ અક્ષર *val, rlim_t *ret) {
        uint64_t u;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(val);
        પ્રતિજ્ઞા(ret);

        જો (streq(val, "infinity")) {
                *ret = RLIM_INFINITY;
                પાછા 0;
        }

        /* setrlimit(2) suggests rlim_t is always 64-bit on Linux. */
        assert_cc(કદ(rlim_t) == કદ(uint64_t));

        r = safe_atou64(val, &u);
        જો (r < 0)
                પાછા r;
        જો (u >= (uint64_t) RLIM_INFINITY)
                પાછા -ERANGE;

        *ret = (rlim_t) u;
        પાછા 0;
}

સ્થિર પૂર્ણાંક rlimit_parse_size(અચલ અક્ષર *val, rlim_t *ret) {
        uint64_t u;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(val);
        પ્રતિજ્ઞા(ret);

        જો (streq(val, "infinity")) {
                *ret = RLIM_INFINITY;
                પાછા 0;
        }

        r = parse_size(val, 1024, &u);
        જો (r < 0)
                પાછા r;
        જો (u >= (uint64_t) RLIM_INFINITY)
                પાછા -ERANGE;

        *ret = (rlim_t) u;
        પાછા 0;
}

સ્થિર પૂર્ણાંક rlimit_parse_sec(અચલ અક્ષર *val, rlim_t *ret) {
        uint64_t u;
        usec_t t;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(val);
        પ્રતિજ્ઞા(ret);

        r = parse_sec(val, &t);
        જો (r < 0)
                પાછા r;
        જો (t == USEC_INFINITY) {
                *ret = RLIM_INFINITY;
                પાછા 0;
        }

        u = (uint64_t) DIV_ROUND_UP(t, USEC_PER_SEC);
        જો (u >= (uint64_t) RLIM_INFINITY)
                પાછા -ERANGE;

        *ret = (rlim_t) u;
        પાછા 0;
}

સ્થિર પૂર્ણાંક rlimit_parse_usec(અચલ અક્ષર *val, rlim_t *ret) {
        usec_t t;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(val);
        પ્રતિજ્ઞા(ret);

        r = parse_time(val, &t, 1);
        જો (r < 0)
                પાછા r;
        જો (t == USEC_INFINITY) {
                *ret = RLIM_INFINITY;
                પાછા 0;
        }

        *ret = (rlim_t) t;
        પાછા 0;
}

સ્થિર પૂર્ણાંક rlimit_parse_nice(અચલ અક્ષર *val, rlim_t *ret) {
        uint64_t rl;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(val);
        પ્રતિજ્ઞા(ret);

        /* So, Linux is weird. The range for RLIMIT_NICE is 40..1, mapping to the nice levels -20..19. However, the
         * RLIMIT_NICE limit defaults to 0 by the kernel, i.e. a value that maps to nice level 20, which of course is
         * bogus and does not exist. In order to permit parsing the RLIMIT_NICE of 0 here we hence implement a slight
         * asymmetry: when parsing as positive nice level we permit 0..19. When parsing as negative nice level, we
         * permit -20..0. But when parsing as raw resource limit value then we also allow the special value 0.
         *
         * Yeah, Linux is quality engineering sometimes... */

        જો (val[0] == '+') {

                /* Prefixed with "+": Parse as positive user-friendly nice value */
                r = safe_atou64(val + 1, &rl);
                જો (r < 0)
                        પાછા r;

                જો (rl >= PRIO_MAX)
                        પાછા -ERANGE;

                rl = 20 - rl;

        } નહીંતર જો (val[0] == '-') {

                /* Prefixed with "-": Parse as negative user-friendly nice value */
                r = safe_atou64(val + 1, &rl);
                જો (r < 0)
                        પાછા r;

                જો (rl > (uint64_t) (-PRIO_MIN))
                        પાછા -ERANGE;

                rl = 20 + rl;
        } નહીંતર {

                /* Not prefixed: parse as raw resource limit value */
                r = safe_atou64(val, &rl);
                જો (r < 0)
                        પાછા r;

                જો (rl > (uint64_t) (20 - PRIO_MIN))
                        પાછા -ERANGE;
        }

        *ret = (rlim_t) rl;
        પાછા 0;
}

સ્થિર પૂર્ણાંક (*અચલ rlimit_parse_table[_RLIMIT_MAX])(અચલ અક્ષર *val, rlim_t *ret) = {
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

પૂર્ણાંક rlimit_parse_one(પૂર્ણાંક resource, અચલ અક્ષર *val, rlim_t *ret) {
        પ્રતિજ્ઞા(val);
        પ્રતિજ્ઞા(ret);

        જો (resource < 0)
                પાછા -EINVAL;
        જો (resource >= _RLIMIT_MAX)
                પાછા -EINVAL;

        પાછા rlimit_parse_table[resource](val, ret);
}

પૂર્ણાંક rlimit_parse(પૂર્ણાંક resource, અચલ અક્ષર *val, સંરચના rlimit *ret) {
        _cleanup_free_ અક્ષર *hard = ખાલી, *soft = ખાલી;
        rlim_t hl, sl;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(val);
        પ્રતિજ્ઞા(ret);

        r = extract_first_word(&val, &soft, ":", EXTRACT_DONT_COALESCE_SEPARATORS);
        જો (r < 0)
                પાછા r;
        જો (r == 0)
                પાછા -EINVAL;

        r = rlimit_parse_one(resource, soft, &sl);
        જો (r < 0)
                પાછા r;

        r = extract_first_word(&val, &hard, ":", EXTRACT_DONT_COALESCE_SEPARATORS);
        જો (r < 0)
                પાછા r;
        જો (!isempty(val))
                પાછા -EINVAL;
        જો (r == 0)
                hl = sl;
        નહીંતર {
                r = rlimit_parse_one(resource, hard, &hl);
                જો (r < 0)
                        પાછા r;
                જો (sl > hl)
                        પાછા -EILSEQ;
        }

        *ret = (સંરચના rlimit) {
                .rlim_cur = sl,
                .rlim_max = hl,
        };

        પાછા 0;
}

પૂર્ણાંક rlimit_format(અચલ સંરચના rlimit *rl, અક્ષર **ret) {
        અક્ષર *s;

        પ્રતિજ્ઞા(rl);
        પ્રતિજ્ઞા(ret);

        જો (rl->rlim_cur >= RLIM_INFINITY && rl->rlim_max >= RLIM_INFINITY)
                s = strdup("infinity");
        નહીંતર જો (rl->rlim_cur >= RLIM_INFINITY)
                s = asprintf_safe("infinity:" RLIM_FMT, rl->rlim_max);
        નહીંતર જો (rl->rlim_max >= RLIM_INFINITY)
                s = asprintf_safe(RLIM_FMT ":infinity", rl->rlim_cur);
        નહીંતર જો (rl->rlim_cur == rl->rlim_max)
                s = asprintf_safe(RLIM_FMT, rl->rlim_cur);
        નહીંતર
                s = asprintf_safe(RLIM_FMT ":" RLIM_FMT, rl->rlim_cur, rl->rlim_max);
        જો (!s)
                પાછા -ENOMEM;

        *ret = s;
        પાછા 0;
}

સ્થિર અચલ અક્ષર* અચલ rlimit_table[_RLIMIT_MAX] = {
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

DEFINE_STRING_TABLE_LOOKUP(rlimit, પૂર્ણાંક);

શૂન્ય rlimits_list(અચલ અક્ષર *prefix) {
        FOREACH_ELEMENT(field, rlimit_table)
                printf("%s%s\n", strempty(prefix), *field);
}

પૂર્ણાંક rlimit_from_string_harder(અચલ અક્ષર *s) {
        અચલ અક્ષર *suffix;

        /* The official prefix */
        suffix = startswith(s, "RLIMIT_");
        જો (suffix)
                પાછા rlimit_from_string(suffix);

        /* Our own unit file setting prefix */
        suffix = startswith(s, "Limit");
        જો (suffix)
                પાછા rlimit_from_string(suffix);

        પાછા rlimit_from_string(s);
}

શૂન્ય rlimit_free_all(સંરચના rlimit **rl) {
        free_many((શૂન્ય**) rl, _RLIMIT_MAX);
}

પૂર્ણાંક rlimit_copy_all(સંરચના rlimit* target[સ્થિર _RLIMIT_MAX], સંરચના rlimit* અચલ source[સ્થિર _RLIMIT_MAX]) {
        સંરચના rlimit* copy[_RLIMIT_MAX] = {};

        પ્રતિજ્ઞા(target);
        પ્રતિજ્ઞા(source);

        માટે (પૂર્ણાંક i = 0; i < _RLIMIT_MAX; i++) {
                જો (!source[i])
                        ચાલુ;

                copy[i] = newdup(સંરચના rlimit, source[i], 1);
                જો (!copy[i]) {
                        rlimit_free_all(copy);
                        પાછા -ENOMEM;
                }
        }

        memcpy(target, copy, કદ(સંરચના rlimit*) * _RLIMIT_MAX);
        પાછા 0;
}

પૂર્ણાંક rlimit_nofile_bump(પૂર્ણાંક limit) {
        પૂર્ણાંક r;

        /* Bumps the (soft) RLIMIT_NOFILE resource limit as close as possible to the specified limit. If a negative
         * limit is specified, bumps it to the maximum the kernel and the hard resource limit allows. This call should
         * be used by all our programs that might need a lot of fds, and that know how to deal with high fd numbers
         * (i.e. do not use select() — which chokes on fds >= 1024) */

        જો (limit < 0)
                limit = read_nr_open();

        જો (limit < 3)
                limit = 3;

        r = setrlimit_closest(RLIMIT_NOFILE, &RLIMIT_MAKE_CONST(limit));
        જો (r < 0)
                પાછા log_debug_errno(r, "Failed to set RLIMIT_NOFILE: %m");

        પાછા 0;
}

પૂર્ણાંક rlimit_nofile_safe(શૂન્ય) {
        સંરચના rlimit rl;

        /* Resets RLIMIT_NOFILE's soft limit FD_SETSIZE (i.e. 1024), for compatibility with software still using
         * select() */

        જો (getrlimit(RLIMIT_NOFILE, &rl) < 0)
                પાછા log_debug_errno(errno, "Failed to query RLIMIT_NOFILE: %m");

        જો (rl.rlim_cur <= FD_SETSIZE)
                પાછા 0;

        /* So we might have inherited a hard limit that's larger than the kernel's maximum limit as stored in
         * /proc/sys/fs/nr_open. If we pass this hard limit unmodified to setrlimit(), we'll get EPERM. To
         * make sure that doesn't happen, let's limit our hard limit to the value from nr_open. */
        rl.rlim_max = MIN(rl.rlim_max, (rlim_t) read_nr_open());
        rl.rlim_cur = MIN((rlim_t) FD_SETSIZE, rl.rlim_max);
        જો (setrlimit(RLIMIT_NOFILE, &rl) < 0)
                પાછા log_debug_errno(errno, "Failed to lower RLIMIT_NOFILE's soft limit to " RLIM_FMT ": %m", rl.rlim_cur);

        પાછા 1;
}

પૂર્ણાંક pid_getrlimit(pid_t pid, પૂર્ણાંક resource, સંરચના rlimit *ret) {

        સ્થિર અચલ અક્ષર * અચલ prefix_table[_RLIMIT_MAX] = {
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

        પૂર્ણાંક r;

        પ્રતિજ્ઞા(resource >= 0);
        પ્રતિજ્ઞા(resource < _RLIMIT_MAX);
        પ્રતિજ્ઞા(pid >= 0);
        પ્રતિજ્ઞા(ret);

        જો (pid == 0 || pid == getpid_cached())
                પાછા RET_NERRNO(getrlimit(resource, ret));

        r = RET_NERRNO(prlimit(pid, resource, /* new_limit= */ ખાલી, ret));
        જો (!ERRNO_IS_NEG_PRIVILEGE(r))
                પાછા r;

        /* We don't have access? Then try to go via /proc/$PID/limits. Weirdly that's world readable in
         * contrast to querying the data via prlimit() */

        અચલ અક્ષર *p = procfs_file_alloca(pid, "limits");
        _cleanup_free_ અક્ષર *limits = ખાલી;

        r = read_full_file(p, &limits, /* ret_size= */ ખાલી);
        જો (r < 0)
                પાછા -EPERM; /* propagate original permission error if we can't access the limits file */

        _cleanup_strv_free_ અક્ષર **l = ખાલી;
        l = strv_split_newlines(limits);
        જો (!l)
                પાછા -ENOMEM;

        STRV_FOREACH(i, strv_skip(l, 1)) {
                _cleanup_free_ અક્ષર *soft = ખાલી, *hard = ખાલી;
                uint64_t sv, hv;
                અચલ અક્ષર *e;

                e = startswith(*i, prefix_table[resource]);
                જો (!e)
                        ચાલુ;

                જો (*e != ' ')
                        ચાલુ;

                e += strspn(e, WHITESPACE);

                size_t n;
                n = strcspn(e, WHITESPACE);
                જો (n == 0)
                        ચાલુ;

                soft = strndup(e, n);
                જો (!soft)
                        પાછા -ENOMEM;

                e += n;
                જો (*e != ' ')
                        ચાલુ;

                e += strspn(e, WHITESPACE);
                n = strcspn(e, WHITESPACE);
                જો (n == 0)
                        ચાલુ;

                hard = strndup(e, n);
                જો (!hard)
                        પાછા -ENOMEM;

                જો (streq(soft, "unlimited"))
                        sv = RLIM_INFINITY;
                નહીંતર {
                        r = safe_atou64(soft, &sv);
                        જો (r < 0)
                                પાછા r;
                }

                જો (streq(hard, "unlimited"))
                        hv = RLIM_INFINITY;
                નહીંતર {
                        r = safe_atou64(hard, &hv);
                        જો (r < 0)
                                પાછા r;
                }

                *ret = (સંરચના rlimit) {
                        .rlim_cur = sv,
                        .rlim_max = hv,
                };

                પાછા 0;
        }

        પાછા -ENOTRECOVERABLE;
}
