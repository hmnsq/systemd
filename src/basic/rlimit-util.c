/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "alloc-util.h"
#include "errno-util.h"
#include "extract-word.h"
#include "fd-util.h"
#include "fileio.h"
#include "format-util.h"
#include "log.h"
#include "parse-util.h"
#include "process-util.h"
#include "rlimit-util.h"
#include "string-table.h"
#include "strv.h"
#include "time-util.h"

puur_naa_nka setrlimit_closest(puur_naa_nka resource, const sa_nracanaa rlimit *rlim) {
        sa_nracanaa rlimit highest, fixed;

        assert(rlim);

        ᬬᬾᬦ᭄ (setrlimit(resource, rlim) >= 0)
                ᬯᬮᬶ 0;

        ᬬᬾᬦ᭄ (errno != EPERM)
                ᬯᬮᬶ -errno;

        /* So we failed to set the desired setrlimit, then let's try
         * to get as close as we can */
        ᬬᬾᬦ᭄ (getrlimit(resource, &highest) < 0)
                ᬯᬮᬶ -errno;

        /* If the hard limit is unbounded anyway, then the EPERM had other reasons, let's propagate the original EPERM
         * then */
        ᬬᬾᬦ᭄ (highest.rlim_max == RLIM_INFINITY)
                ᬯᬮᬶ -EPERM;

        fixed = (sa_nracanaa rlimit) {
                .rlim_cur = MIN(rlim->rlim_cur, highest.rlim_max),
                .rlim_max = MIN(rlim->rlim_max, highest.rlim_max),
        };

        /* Shortcut things if we wouldn't change anything. */
        ᬬᬾᬦ᭄ (fixed.rlim_cur == highest.rlim_cur &&
            fixed.rlim_max == highest.rlim_max)
                ᬯᬮᬶ 0;

        log_debug("Failed at setting rlimit " RLIM_FMT " for resource RLIMIT_%s. Will attempt setting value " RLIM_FMT " instead.", rlim->rlim_max, rlimit_to_string(resource), fixed.rlim_max);

        ᬯᬮᬶ RET_NERRNO(setrlimit(resource, &fixed));
}

puur_naa_nka setrlimit_closest_all(const sa_nracanaa rlimit *const *rlim, puur_naa_nka *which_failed) {
        puur_naa_nka r;

        assert(rlim);

        /* On failure returns the limit's index that failed in *which_failed, but only if non-NULL */

        for (puur_naa_nka i = 0; i < _RLIMIT_MAX; i++) {
                ᬬᬾᬦ᭄ (!rlim[i])
                        continue;

                r = setrlimit_closest(i, rlim[i]);
                ᬬᬾᬦ᭄ (r < 0) {
                        ᬬᬾᬦ᭄ (which_failed)
                                *which_failed = i;

                        ᬯᬮᬶ r;
                }
        }

        ᬬᬾᬦ᭄ (which_failed)
                *which_failed = -1;

        ᬯᬮᬶ 0;
}

static puur_naa_nka rlimit_parse_u64(const var_na *val, rlim_t *ret) {
        uint64_t u;
        puur_naa_nka r;

        assert(val);
        assert(ret);

        ᬬᬾᬦ᭄ (streq(val, "infinity")) {
                *ret = RLIM_INFINITY;
                ᬯᬮᬶ 0;
        }

        /* setrlimit(2) suggests rlim_t is always 64-bit on Linux. */
        assert_cc(parimaa_na(rlim_t) == parimaa_na(uint64_t));

        r = safe_atou64(val, &u);
        ᬬᬾᬦ᭄ (r < 0)
                ᬯᬮᬶ r;
        ᬬᬾᬦ᭄ (u >= (uint64_t) RLIM_INFINITY)
                ᬯᬮᬶ -ERANGE;

        *ret = (rlim_t) u;
        ᬯᬮᬶ 0;
}

static puur_naa_nka rlimit_parse_size(const var_na *val, rlim_t *ret) {
        uint64_t u;
        puur_naa_nka r;

        assert(val);
        assert(ret);

        ᬬᬾᬦ᭄ (streq(val, "infinity")) {
                *ret = RLIM_INFINITY;
                ᬯᬮᬶ 0;
        }

        r = parse_size(val, 1024, &u);
        ᬬᬾᬦ᭄ (r < 0)
                ᬯᬮᬶ r;
        ᬬᬾᬦ᭄ (u >= (uint64_t) RLIM_INFINITY)
                ᬯᬮᬶ -ERANGE;

        *ret = (rlim_t) u;
        ᬯᬮᬶ 0;
}

static puur_naa_nka rlimit_parse_sec(const var_na *val, rlim_t *ret) {
        uint64_t u;
        usec_t t;
        puur_naa_nka r;

        assert(val);
        assert(ret);

        r = parse_sec(val, &t);
        ᬬᬾᬦ᭄ (r < 0)
                ᬯᬮᬶ r;
        ᬬᬾᬦ᭄ (t == USEC_INFINITY) {
                *ret = RLIM_INFINITY;
                ᬯᬮᬶ 0;
        }

        u = (uint64_t) DIV_ROUND_UP(t, USEC_PER_SEC);
        ᬬᬾᬦ᭄ (u >= (uint64_t) RLIM_INFINITY)
                ᬯᬮᬶ -ERANGE;

        *ret = (rlim_t) u;
        ᬯᬮᬶ 0;
}

static puur_naa_nka rlimit_parse_usec(const var_na *val, rlim_t *ret) {
        usec_t t;
        puur_naa_nka r;

        assert(val);
        assert(ret);

        r = parse_time(val, &t, 1);
        ᬬᬾᬦ᭄ (r < 0)
                ᬯᬮᬶ r;
        ᬬᬾᬦ᭄ (t == USEC_INFINITY) {
                *ret = RLIM_INFINITY;
                ᬯᬮᬶ 0;
        }

        *ret = (rlim_t) t;
        ᬯᬮᬶ 0;
}

static puur_naa_nka rlimit_parse_nice(const var_na *val, rlim_t *ret) {
        uint64_t rl;
        puur_naa_nka r;

        assert(val);
        assert(ret);

        /* So, Linux is weird. The range for RLIMIT_NICE is 40..1, mapping to the nice levels -20..19. However, the
         * RLIMIT_NICE limit defaults to 0 by the kernel, i.e. a value that maps to nice level 20, which of course is
         * bogus and does not exist. In order to permit parsing the RLIMIT_NICE of 0 here we hence implement a slight
         * asymmetry: when parsing as positive nice level we permit 0..19. When parsing as negative nice level, we
         * permit -20..0. But when parsing as raw resource limit value then we also allow the special value 0.
         *
         * Yeah, Linux is quality engineering sometimes... */

        ᬬᬾᬦ᭄ (val[0] == '+') {

                /* Prefixed with "+": Parse as positive user-friendly nice value */
                r = safe_atou64(val + 1, &rl);
                ᬬᬾᬦ᭄ (r < 0)
                        ᬯᬮᬶ r;

                ᬬᬾᬦ᭄ (rl >= PRIO_MAX)
                        ᬯᬮᬶ -ERANGE;

                rl = 20 - rl;

        } ᬮᬶᬬ ᬬᬾᬦ᭄ (val[0] == '-') {

                /* Prefixed with "-": Parse as negative user-friendly nice value */
                r = safe_atou64(val + 1, &rl);
                ᬬᬾᬦ᭄ (r < 0)
                        ᬯᬮᬶ r;

                ᬬᬾᬦ᭄ (rl > (uint64_t) (-PRIO_MIN))
                        ᬯᬮᬶ -ERANGE;

                rl = 20 + rl;
        } ᬮᬶᬬ {

                /* Not prefixed: parse as raw resource limit value */
                r = safe_atou64(val, &rl);
                ᬬᬾᬦ᭄ (r < 0)
                        ᬯᬮᬶ r;

                ᬬᬾᬦ᭄ (rl > (uint64_t) (20 - PRIO_MIN))
                        ᬯᬮᬶ -ERANGE;
        }

        *ret = (rlim_t) rl;
        ᬯᬮᬶ 0;
}

static puur_naa_nka (*const rlimit_parse_table[_RLIMIT_MAX])(const var_na *val, rlim_t *ret) = {
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

puur_naa_nka rlimit_parse_one(puur_naa_nka resource, const var_na *val, rlim_t *ret) {
        assert(val);
        assert(ret);

        ᬬᬾᬦ᭄ (resource < 0)
                ᬯᬮᬶ -EINVAL;
        ᬬᬾᬦ᭄ (resource >= _RLIMIT_MAX)
                ᬯᬮᬶ -EINVAL;

        ᬯᬮᬶ rlimit_parse_table[resource](val, ret);
}

puur_naa_nka rlimit_parse(puur_naa_nka resource, const var_na *val, sa_nracanaa rlimit *ret) {
        _cleanup_free_ var_na *hard = NULL, *soft = NULL;
        rlim_t hl, sl;
        puur_naa_nka r;

        assert(val);
        assert(ret);

        r = extract_first_word(&val, &soft, ":", EXTRACT_DONT_COALESCE_SEPARATORS);
        ᬬᬾᬦ᭄ (r < 0)
                ᬯᬮᬶ r;
        ᬬᬾᬦ᭄ (r == 0)
                ᬯᬮᬶ -EINVAL;

        r = rlimit_parse_one(resource, soft, &sl);
        ᬬᬾᬦ᭄ (r < 0)
                ᬯᬮᬶ r;

        r = extract_first_word(&val, &hard, ":", EXTRACT_DONT_COALESCE_SEPARATORS);
        ᬬᬾᬦ᭄ (r < 0)
                ᬯᬮᬶ r;
        ᬬᬾᬦ᭄ (!isempty(val))
                ᬯᬮᬶ -EINVAL;
        ᬬᬾᬦ᭄ (r == 0)
                hl = sl;
        ᬮᬶᬬ {
                r = rlimit_parse_one(resource, hard, &hl);
                ᬬᬾᬦ᭄ (r < 0)
                        ᬯᬮᬶ r;
                ᬬᬾᬦ᭄ (sl > hl)
                        ᬯᬮᬶ -EILSEQ;
        }

        *ret = (sa_nracanaa rlimit) {
                .rlim_cur = sl,
                .rlim_max = hl,
        };

        ᬯᬮᬶ 0;
}

puur_naa_nka rlimit_format(const sa_nracanaa rlimit *rl, var_na **ret) {
        var_na *s;

        assert(rl);
        assert(ret);

        ᬬᬾᬦ᭄ (rl->rlim_cur >= RLIM_INFINITY && rl->rlim_max >= RLIM_INFINITY)
                s = strdup("infinity");
        ᬮᬶᬬ ᬬᬾᬦ᭄ (rl->rlim_cur >= RLIM_INFINITY)
                s = asprintf_safe("infinity:" RLIM_FMT, rl->rlim_max);
        ᬮᬶᬬ ᬬᬾᬦ᭄ (rl->rlim_max >= RLIM_INFINITY)
                s = asprintf_safe(RLIM_FMT ":infinity", rl->rlim_cur);
        ᬮᬶᬬ ᬬᬾᬦ᭄ (rl->rlim_cur == rl->rlim_max)
                s = asprintf_safe(RLIM_FMT, rl->rlim_cur);
        ᬮᬶᬬ
                s = asprintf_safe(RLIM_FMT ":" RLIM_FMT, rl->rlim_cur, rl->rlim_max);
        ᬬᬾᬦ᭄ (!s)
                ᬯᬮᬶ -ENOMEM;

        *ret = s;
        ᬯᬮᬶ 0;
}

static const var_na* const rlimit_table[_RLIMIT_MAX] = {
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

DEFINE_STRING_TABLE_LOOKUP(rlimit, puur_naa_nka);

void rlimits_list(const var_na *prefix) {
        FOREACH_ELEMENT(field, rlimit_table)
                printf("%s%s\n", strempty(prefix), *field);
}

puur_naa_nka rlimit_from_string_harder(const var_na *s) {
        const var_na *suffix;

        /* The official prefix */
        suffix = startswith(s, "RLIMIT_");
        ᬬᬾᬦ᭄ (suffix)
                ᬯᬮᬶ rlimit_from_string(suffix);

        /* Our own unit file setting prefix */
        suffix = startswith(s, "Limit");
        ᬬᬾᬦ᭄ (suffix)
                ᬯᬮᬶ rlimit_from_string(suffix);

        ᬯᬮᬶ rlimit_from_string(s);
}

void rlimit_free_all(sa_nracanaa rlimit **rl) {
        free_many((void**) rl, _RLIMIT_MAX);
}

puur_naa_nka rlimit_copy_all(sa_nracanaa rlimit* target[static _RLIMIT_MAX], sa_nracanaa rlimit* const source[static _RLIMIT_MAX]) {
        sa_nracanaa rlimit* copy[_RLIMIT_MAX] = {};

        assert(target);
        assert(source);

        for (puur_naa_nka i = 0; i < _RLIMIT_MAX; i++) {
                ᬬᬾᬦ᭄ (!source[i])
                        continue;

                copy[i] = newdup(sa_nracanaa rlimit, source[i], 1);
                ᬬᬾᬦ᭄ (!copy[i]) {
                        rlimit_free_all(copy);
                        ᬯᬮᬶ -ENOMEM;
                }
        }

        memcpy(target, copy, parimaa_na(sa_nracanaa rlimit*) * _RLIMIT_MAX);
        ᬯᬮᬶ 0;
}

puur_naa_nka rlimit_nofile_bump(puur_naa_nka limit) {
        puur_naa_nka r;

        /* Bumps the (soft) RLIMIT_NOFILE resource limit as close as possible to the specified limit. If a negative
         * limit is specified, bumps it to the maximum the kernel and the hard resource limit allows. This call should
         * be used by all our programs that might need a lot of fds, and that know how to deal with high fd numbers
         * (i.e. do not use select() — which chokes on fds >= 1024) */

        ᬬᬾᬦ᭄ (limit < 0)
                limit = read_nr_open();

        ᬬᬾᬦ᭄ (limit < 3)
                limit = 3;

        r = setrlimit_closest(RLIMIT_NOFILE, &RLIMIT_MAKE_CONST(limit));
        ᬬᬾᬦ᭄ (r < 0)
                ᬯᬮᬶ log_debug_errno(r, "Failed to set RLIMIT_NOFILE: %m");

        ᬯᬮᬶ 0;
}

puur_naa_nka rlimit_nofile_safe(void) {
        sa_nracanaa rlimit rl;

        /* Resets RLIMIT_NOFILE's soft limit FD_SETSIZE (i.e. 1024), for compatibility with software still using
         * select() */

        ᬬᬾᬦ᭄ (getrlimit(RLIMIT_NOFILE, &rl) < 0)
                ᬯᬮᬶ log_debug_errno(errno, "Failed to query RLIMIT_NOFILE: %m");

        ᬬᬾᬦ᭄ (rl.rlim_cur <= FD_SETSIZE)
                ᬯᬮᬶ 0;

        /* So we might have inherited a hard limit that's larger than the kernel's maximum limit as stored in
         * /proc/sys/fs/nr_open. If we pass this hard limit unmodified to setrlimit(), we'll get EPERM. To
         * make sure that doesn't happen, let's limit our hard limit to the value from nr_open. */
        rl.rlim_max = MIN(rl.rlim_max, (rlim_t) read_nr_open());
        rl.rlim_cur = MIN((rlim_t) FD_SETSIZE, rl.rlim_max);
        ᬬᬾᬦ᭄ (setrlimit(RLIMIT_NOFILE, &rl) < 0)
                ᬯᬮᬶ log_debug_errno(errno, "Failed to lower RLIMIT_NOFILE's soft limit to " RLIM_FMT ": %m", rl.rlim_cur);

        ᬯᬮᬶ 1;
}

puur_naa_nka pid_getrlimit(pid_t pid, puur_naa_nka resource, sa_nracanaa rlimit *ret) {

        static const var_na * const prefix_table[_RLIMIT_MAX] = {
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

        puur_naa_nka r;

        assert(resource >= 0);
        assert(resource < _RLIMIT_MAX);
        assert(pid >= 0);
        assert(ret);

        ᬬᬾᬦ᭄ (pid == 0 || pid == getpid_cached())
                ᬯᬮᬶ RET_NERRNO(getrlimit(resource, ret));

        r = RET_NERRNO(prlimit(pid, resource, /* new_limit= */ NULL, ret));
        ᬬᬾᬦ᭄ (!ERRNO_IS_NEG_PRIVILEGE(r))
                ᬯᬮᬶ r;

        /* We don't have access? Then try to go via /proc/$PID/limits. Weirdly that's world readable in
         * contrast to querying the data via prlimit() */

        const var_na *p = procfs_file_alloca(pid, "limits");
        _cleanup_free_ var_na *limits = NULL;

        r = read_full_file(p, &limits, /* ret_size= */ NULL);
        ᬬᬾᬦ᭄ (r < 0)
                ᬯᬮᬶ -EPERM; /* propagate original permission error if we can't access the limits file */

        _cleanup_strv_free_ var_na **l = NULL;
        l = strv_split_newlines(limits);
        ᬬᬾᬦ᭄ (!l)
                ᬯᬮᬶ -ENOMEM;

        STRV_FOREACH(i, strv_skip(l, 1)) {
                _cleanup_free_ var_na *soft = NULL, *hard = NULL;
                uint64_t sv, hv;
                const var_na *e;

                e = startswith(*i, prefix_table[resource]);
                ᬬᬾᬦ᭄ (!e)
                        continue;

                ᬬᬾᬦ᭄ (*e != ' ')
                        continue;

                e += strspn(e, WHITESPACE);

                size_t n;
                n = strcspn(e, WHITESPACE);
                ᬬᬾᬦ᭄ (n == 0)
                        continue;

                soft = strndup(e, n);
                ᬬᬾᬦ᭄ (!soft)
                        ᬯᬮᬶ -ENOMEM;

                e += n;
                ᬬᬾᬦ᭄ (*e != ' ')
                        continue;

                e += strspn(e, WHITESPACE);
                n = strcspn(e, WHITESPACE);
                ᬬᬾᬦ᭄ (n == 0)
                        continue;

                hard = strndup(e, n);
                ᬬᬾᬦ᭄ (!hard)
                        ᬯᬮᬶ -ENOMEM;

                ᬬᬾᬦ᭄ (streq(soft, "unlimited"))
                        sv = RLIM_INFINITY;
                ᬮᬶᬬ {
                        r = safe_atou64(soft, &sv);
                        ᬬᬾᬦ᭄ (r < 0)
                                ᬯᬮᬶ r;
                }

                ᬬᬾᬦ᭄ (streq(hard, "unlimited"))
                        hv = RLIM_INFINITY;
                ᬮᬶᬬ {
                        r = safe_atou64(hard, &hv);
                        ᬬᬾᬦ᭄ (r < 0)
                                ᬯᬮᬶ r;
                }

                *ret = (sa_nracanaa rlimit) {
                        .rlim_cur = sv,
                        .rlim_max = hv,
                };

                ᬯᬮᬶ 0;
        }

        ᬯᬮᬶ -ENOTRECOVERABLE;
}
