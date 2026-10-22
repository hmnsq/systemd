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

цяло setrlimit_closest(цяло resource, const struct rlimit *rlim) {
        struct rlimit highest, fixed;

        assert(rlim);

        ако (setrlimit(resource, rlim) >= 0)
                върни 0;

        ако (errno != EPERM)
                върни -errno;

        /* So we failed to set the desired setrlimit, then let's try
         * to get as close as we can */
        ако (getrlimit(resource, &highest) < 0)
                върни -errno;

        /* If the hard limit is unbounded anyway, then the EPERM had other reasons, let's propagate the original EPERM
         * then */
        ако (highest.rlim_max == RLIM_INFINITY)
                върни -EPERM;

        fixed = (struct rlimit) {
                .rlim_cur = MIN(rlim->rlim_cur, highest.rlim_max),
                .rlim_max = MIN(rlim->rlim_max, highest.rlim_max),
        };

        /* Shortcut things if we wouldn't change anything. */
        ако (fixed.rlim_cur == highest.rlim_cur &&
            fixed.rlim_max == highest.rlim_max)
                върни 0;

        log_debug("Failed at setting rlimit " RLIM_FMT " for resource RLIMIT_%s. Will attempt setting value " RLIM_FMT " instead.", rlim->rlim_max, rlimit_to_string(resource), fixed.rlim_max);

        върни RET_NERRNO(setrlimit(resource, &fixed));
}

цяло setrlimit_closest_all(const struct rlimit *const *rlim, цяло *which_failed) {
        цяло r;

        assert(rlim);

        /* On failure returns the limit's index that failed in *which_failed, but only if non-NULL */

        за (цяло i = 0; i < _RLIMIT_MAX; i++) {
                ако (!rlim[i])
                        continue;

                r = setrlimit_closest(i, rlim[i]);
                ако (r < 0) {
                        ако (which_failed)
                                *which_failed = i;

                        върни r;
                }
        }

        ако (which_failed)
                *which_failed = -1;

        върни 0;
}

static цяло rlimit_parse_u64(const char *val, rlim_t *ret) {
        uint64_t u;
        цяло r;

        assert(val);
        assert(ret);

        ако (streq(val, "infinity")) {
                *ret = RLIM_INFINITY;
                върни 0;
        }

        /* setrlimit(2) suggests rlim_t is always 64-bit on Linux. */
        assert_cc(sizeof(rlim_t) == sizeof(uint64_t));

        r = safe_atou64(val, &u);
        ако (r < 0)
                върни r;
        ако (u >= (uint64_t) RLIM_INFINITY)
                върни -ERANGE;

        *ret = (rlim_t) u;
        върни 0;
}

static цяло rlimit_parse_size(const char *val, rlim_t *ret) {
        uint64_t u;
        цяло r;

        assert(val);
        assert(ret);

        ако (streq(val, "infinity")) {
                *ret = RLIM_INFINITY;
                върни 0;
        }

        r = parse_size(val, 1024, &u);
        ако (r < 0)
                върни r;
        ако (u >= (uint64_t) RLIM_INFINITY)
                върни -ERANGE;

        *ret = (rlim_t) u;
        върни 0;
}

static цяло rlimit_parse_sec(const char *val, rlim_t *ret) {
        uint64_t u;
        usec_t t;
        цяло r;

        assert(val);
        assert(ret);

        r = parse_sec(val, &t);
        ако (r < 0)
                върни r;
        ако (t == USEC_INFINITY) {
                *ret = RLIM_INFINITY;
                върни 0;
        }

        u = (uint64_t) DIV_ROUND_UP(t, USEC_PER_SEC);
        ако (u >= (uint64_t) RLIM_INFINITY)
                върни -ERANGE;

        *ret = (rlim_t) u;
        върни 0;
}

static цяло rlimit_parse_usec(const char *val, rlim_t *ret) {
        usec_t t;
        цяло r;

        assert(val);
        assert(ret);

        r = parse_time(val, &t, 1);
        ако (r < 0)
                върни r;
        ако (t == USEC_INFINITY) {
                *ret = RLIM_INFINITY;
                върни 0;
        }

        *ret = (rlim_t) t;
        върни 0;
}

static цяло rlimit_parse_nice(const char *val, rlim_t *ret) {
        uint64_t rl;
        цяло r;

        assert(val);
        assert(ret);

        /* So, Linux is weird. The range for RLIMIT_NICE is 40..1, mapping to the nice levels -20..19. However, the
         * RLIMIT_NICE limit defaults to 0 by the kernel, i.e. a value that maps to nice level 20, which of course is
         * bogus and does not exist. In order to permit parsing the RLIMIT_NICE of 0 here we hence implement a slight
         * asymmetry: when parsing as positive nice level we permit 0..19. When parsing as negative nice level, we
         * permit -20..0. But when parsing as raw resource limit value then we also allow the special value 0.
         *
         * Yeah, Linux is quality engineering sometimes... */

        ако (val[0] == '+') {

                /* Prefixed with "+": Parse as positive user-friendly nice value */
                r = safe_atou64(val + 1, &rl);
                ако (r < 0)
                        върни r;

                ако (rl >= PRIO_MAX)
                        върни -ERANGE;

                rl = 20 - rl;

        } иначе ако (val[0] == '-') {

                /* Prefixed with "-": Parse as negative user-friendly nice value */
                r = safe_atou64(val + 1, &rl);
                ако (r < 0)
                        върни r;

                ако (rl > (uint64_t) (-PRIO_MIN))
                        върни -ERANGE;

                rl = 20 + rl;
        } иначе {

                /* Not prefixed: parse as raw resource limit value */
                r = safe_atou64(val, &rl);
                ако (r < 0)
                        върни r;

                ако (rl > (uint64_t) (20 - PRIO_MIN))
                        върни -ERANGE;
        }

        *ret = (rlim_t) rl;
        върни 0;
}

static цяло (*const rlimit_parse_table[_RLIMIT_MAX])(const char *val, rlim_t *ret) = {
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

цяло rlimit_parse_one(цяло resource, const char *val, rlim_t *ret) {
        assert(val);
        assert(ret);

        ако (resource < 0)
                върни -EINVAL;
        ако (resource >= _RLIMIT_MAX)
                върни -EINVAL;

        върни rlimit_parse_table[resource](val, ret);
}

цяло rlimit_parse(цяло resource, const char *val, struct rlimit *ret) {
        _cleanup_free_ char *hard = NULL, *soft = NULL;
        rlim_t hl, sl;
        цяло r;

        assert(val);
        assert(ret);

        r = extract_first_word(&val, &soft, ":", EXTRACT_DONT_COALESCE_SEPARATORS);
        ако (r < 0)
                върни r;
        ако (r == 0)
                върни -EINVAL;

        r = rlimit_parse_one(resource, soft, &sl);
        ако (r < 0)
                върни r;

        r = extract_first_word(&val, &hard, ":", EXTRACT_DONT_COALESCE_SEPARATORS);
        ако (r < 0)
                върни r;
        ако (!isempty(val))
                върни -EINVAL;
        ако (r == 0)
                hl = sl;
        иначе {
                r = rlimit_parse_one(resource, hard, &hl);
                ако (r < 0)
                        върни r;
                ако (sl > hl)
                        върни -EILSEQ;
        }

        *ret = (struct rlimit) {
                .rlim_cur = sl,
                .rlim_max = hl,
        };

        върни 0;
}

цяло rlimit_format(const struct rlimit *rl, char **ret) {
        char *s;

        assert(rl);
        assert(ret);

        ако (rl->rlim_cur >= RLIM_INFINITY && rl->rlim_max >= RLIM_INFINITY)
                s = strdup("infinity");
        иначе ако (rl->rlim_cur >= RLIM_INFINITY)
                s = asprintf_safe("infinity:" RLIM_FMT, rl->rlim_max);
        иначе ако (rl->rlim_max >= RLIM_INFINITY)
                s = asprintf_safe(RLIM_FMT ":infinity", rl->rlim_cur);
        иначе ако (rl->rlim_cur == rl->rlim_max)
                s = asprintf_safe(RLIM_FMT, rl->rlim_cur);
        иначе
                s = asprintf_safe(RLIM_FMT ":" RLIM_FMT, rl->rlim_cur, rl->rlim_max);
        ако (!s)
                върни -ENOMEM;

        *ret = s;
        върни 0;
}

static const char* const rlimit_table[_RLIMIT_MAX] = {
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

DEFINE_STRING_TABLE_LOOKUP(rlimit, цяло);

void rlimits_list(const char *prefix) {
        FOREACH_ELEMENT(field, rlimit_table)
                printf("%s%s\n", strempty(prefix), *field);
}

цяло rlimit_from_string_harder(const char *s) {
        const char *suffix;

        /* The official prefix */
        suffix = startswith(s, "RLIMIT_");
        ако (suffix)
                върни rlimit_from_string(suffix);

        /* Our own unit file setting prefix */
        suffix = startswith(s, "Limit");
        ако (suffix)
                върни rlimit_from_string(suffix);

        върни rlimit_from_string(s);
}

void rlimit_free_all(struct rlimit **rl) {
        free_many((void**) rl, _RLIMIT_MAX);
}

цяло rlimit_copy_all(struct rlimit* target[static _RLIMIT_MAX], struct rlimit* const source[static _RLIMIT_MAX]) {
        struct rlimit* copy[_RLIMIT_MAX] = {};

        assert(target);
        assert(source);

        за (цяло i = 0; i < _RLIMIT_MAX; i++) {
                ако (!source[i])
                        continue;

                copy[i] = newdup(struct rlimit, source[i], 1);
                ако (!copy[i]) {
                        rlimit_free_all(copy);
                        върни -ENOMEM;
                }
        }

        memcpy(target, copy, sizeof(struct rlimit*) * _RLIMIT_MAX);
        върни 0;
}

цяло rlimit_nofile_bump(цяло limit) {
        цяло r;

        /* Bumps the (soft) RLIMIT_NOFILE resource limit as close as possible to the specified limit. If a negative
         * limit is specified, bumps it to the maximum the kernel and the hard resource limit allows. This call should
         * be used by all our programs that might need a lot of fds, and that know how to deal with high fd numbers
         * (i.e. do not use select() — which chokes on fds >= 1024) */

        ако (limit < 0)
                limit = read_nr_open();

        ако (limit < 3)
                limit = 3;

        r = setrlimit_closest(RLIMIT_NOFILE, &RLIMIT_MAKE_CONST(limit));
        ако (r < 0)
                върни log_debug_errno(r, "Failed to set RLIMIT_NOFILE: %m");

        върни 0;
}

цяло rlimit_nofile_safe(void) {
        struct rlimit rl;

        /* Resets RLIMIT_NOFILE's soft limit FD_SETSIZE (i.e. 1024), for compatibility with software still using
         * select() */

        ако (getrlimit(RLIMIT_NOFILE, &rl) < 0)
                върни log_debug_errno(errno, "Failed to query RLIMIT_NOFILE: %m");

        ако (rl.rlim_cur <= FD_SETSIZE)
                върни 0;

        /* So we might have inherited a hard limit that's larger than the kernel's maximum limit as stored in
         * /proc/sys/fs/nr_open. If we pass this hard limit unmodified to setrlimit(), we'll get EPERM. To
         * make sure that doesn't happen, let's limit our hard limit to the value from nr_open. */
        rl.rlim_max = MIN(rl.rlim_max, (rlim_t) read_nr_open());
        rl.rlim_cur = MIN((rlim_t) FD_SETSIZE, rl.rlim_max);
        ако (setrlimit(RLIMIT_NOFILE, &rl) < 0)
                върни log_debug_errno(errno, "Failed to lower RLIMIT_NOFILE's soft limit to " RLIM_FMT ": %m", rl.rlim_cur);

        върни 1;
}

цяло pid_getrlimit(pid_t pid, цяло resource, struct rlimit *ret) {

        static const char * const prefix_table[_RLIMIT_MAX] = {
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

        цяло r;

        assert(resource >= 0);
        assert(resource < _RLIMIT_MAX);
        assert(pid >= 0);
        assert(ret);

        ако (pid == 0 || pid == getpid_cached())
                върни RET_NERRNO(getrlimit(resource, ret));

        r = RET_NERRNO(prlimit(pid, resource, /* new_limit= */ NULL, ret));
        ако (!ERRNO_IS_NEG_PRIVILEGE(r))
                върни r;

        /* We don't have access? Then try to go via /proc/$PID/limits. Weirdly that's world readable in
         * contrast to querying the data via prlimit() */

        const char *p = procfs_file_alloca(pid, "limits");
        _cleanup_free_ char *limits = NULL;

        r = read_full_file(p, &limits, /* ret_size= */ NULL);
        ако (r < 0)
                върни -EPERM; /* propagate original permission error if we can't access the limits file */

        _cleanup_strv_free_ char **l = NULL;
        l = strv_split_newlines(limits);
        ако (!l)
                върни -ENOMEM;

        STRV_FOREACH(i, strv_skip(l, 1)) {
                _cleanup_free_ char *soft = NULL, *hard = NULL;
                uint64_t sv, hv;
                const char *e;

                e = startswith(*i, prefix_table[resource]);
                ако (!e)
                        continue;

                ако (*e != ' ')
                        continue;

                e += strspn(e, WHITESPACE);

                size_t n;
                n = strcspn(e, WHITESPACE);
                ако (n == 0)
                        continue;

                soft = strndup(e, n);
                ако (!soft)
                        върни -ENOMEM;

                e += n;
                ако (*e != ' ')
                        continue;

                e += strspn(e, WHITESPACE);
                n = strcspn(e, WHITESPACE);
                ако (n == 0)
                        continue;

                hard = strndup(e, n);
                ако (!hard)
                        върни -ENOMEM;

                ако (streq(soft, "unlimited"))
                        sv = RLIM_INFINITY;
                иначе {
                        r = safe_atou64(soft, &sv);
                        ако (r < 0)
                                върни r;
                }

                ако (streq(hard, "unlimited"))
                        hv = RLIM_INFINITY;
                иначе {
                        r = safe_atou64(hard, &hv);
                        ако (r < 0)
                                върни r;
                }

                *ret = (struct rlimit) {
                        .rlim_cur = sv,
                        .rlim_max = hv,
                };

                върни 0;
        }

        върни -ENOTRECOVERABLE;
}
