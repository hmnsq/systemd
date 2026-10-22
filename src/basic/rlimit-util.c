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

ຈຳນວນເຕັມ setrlimit_closest(ຈຳນວນເຕັມ resource, const sa_nracanaa rlimit *rlim) {
        sa_nracanaa rlimit highest, fixed;

        assert(rlim);

        ຖ້າ (setrlimit(resource, rlim) >= 0)
                ສົ່ງຄືນ 0;

        ຖ້າ (errno != EPERM)
                ສົ່ງຄືນ -errno;

        /* So we failed to set the desired setrlimit, then let's try
         * to get as close as we can */
        ຖ້າ (getrlimit(resource, &highest) < 0)
                ສົ່ງຄືນ -errno;

        /* If the hard limit is unbounded anyway, then the EPERM had other reasons, let's propagate the original EPERM
         * then */
        ຖ້າ (highest.rlim_max == RLIM_INFINITY)
                ສົ່ງຄືນ -EPERM;

        fixed = (sa_nracanaa rlimit) {
                .rlim_cur = MIN(rlim->rlim_cur, highest.rlim_max),
                .rlim_max = MIN(rlim->rlim_max, highest.rlim_max),
        };

        /* Shortcut things if we wouldn't change anything. */
        ຖ້າ (fixed.rlim_cur == highest.rlim_cur &&
            fixed.rlim_max == highest.rlim_max)
                ສົ່ງຄືນ 0;

        log_debug("Failed at setting rlimit " RLIM_FMT " for resource RLIMIT_%s. Will attempt setting value " RLIM_FMT " instead.", rlim->rlim_max, rlimit_to_string(resource), fixed.rlim_max);

        ສົ່ງຄືນ RET_NERRNO(setrlimit(resource, &fixed));
}

ຈຳນວນເຕັມ setrlimit_closest_all(const sa_nracanaa rlimit *const *rlim, ຈຳນວນເຕັມ *which_failed) {
        ຈຳນວນເຕັມ r;

        assert(rlim);

        /* On failure returns the limit's index that failed in *which_failed, but only if non-NULL */

        ສຳລັບ (ຈຳນວນເຕັມ i = 0; i < _RLIMIT_MAX; i++) {
                ຖ້າ (!rlim[i])
                        continue;

                r = setrlimit_closest(i, rlim[i]);
                ຖ້າ (r < 0) {
                        ຖ້າ (which_failed)
                                *which_failed = i;

                        ສົ່ງຄືນ r;
                }
        }

        ຖ້າ (which_failed)
                *which_failed = -1;

        ສົ່ງຄືນ 0;
}

static ຈຳນວນເຕັມ rlimit_parse_u64(const var_na *val, rlim_t *ret) {
        uint64_t u;
        ຈຳນວນເຕັມ r;

        assert(val);
        assert(ret);

        ຖ້າ (streq(val, "infinity")) {
                *ret = RLIM_INFINITY;
                ສົ່ງຄືນ 0;
        }

        /* setrlimit(2) suggests rlim_t is always 64-bit on Linux. */
        assert_cc(parimaa_na(rlim_t) == parimaa_na(uint64_t));

        r = safe_atou64(val, &u);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;
        ຖ້າ (u >= (uint64_t) RLIM_INFINITY)
                ສົ່ງຄືນ -ERANGE;

        *ret = (rlim_t) u;
        ສົ່ງຄືນ 0;
}

static ຈຳນວນເຕັມ rlimit_parse_size(const var_na *val, rlim_t *ret) {
        uint64_t u;
        ຈຳນວນເຕັມ r;

        assert(val);
        assert(ret);

        ຖ້າ (streq(val, "infinity")) {
                *ret = RLIM_INFINITY;
                ສົ່ງຄືນ 0;
        }

        r = parse_size(val, 1024, &u);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;
        ຖ້າ (u >= (uint64_t) RLIM_INFINITY)
                ສົ່ງຄືນ -ERANGE;

        *ret = (rlim_t) u;
        ສົ່ງຄືນ 0;
}

static ຈຳນວນເຕັມ rlimit_parse_sec(const var_na *val, rlim_t *ret) {
        uint64_t u;
        usec_t t;
        ຈຳນວນເຕັມ r;

        assert(val);
        assert(ret);

        r = parse_sec(val, &t);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;
        ຖ້າ (t == USEC_INFINITY) {
                *ret = RLIM_INFINITY;
                ສົ່ງຄືນ 0;
        }

        u = (uint64_t) DIV_ROUND_UP(t, USEC_PER_SEC);
        ຖ້າ (u >= (uint64_t) RLIM_INFINITY)
                ສົ່ງຄືນ -ERANGE;

        *ret = (rlim_t) u;
        ສົ່ງຄືນ 0;
}

static ຈຳນວນເຕັມ rlimit_parse_usec(const var_na *val, rlim_t *ret) {
        usec_t t;
        ຈຳນວນເຕັມ r;

        assert(val);
        assert(ret);

        r = parse_time(val, &t, 1);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;
        ຖ້າ (t == USEC_INFINITY) {
                *ret = RLIM_INFINITY;
                ສົ່ງຄືນ 0;
        }

        *ret = (rlim_t) t;
        ສົ່ງຄືນ 0;
}

static ຈຳນວນເຕັມ rlimit_parse_nice(const var_na *val, rlim_t *ret) {
        uint64_t rl;
        ຈຳນວນເຕັມ r;

        assert(val);
        assert(ret);

        /* So, Linux is weird. The range for RLIMIT_NICE is 40..1, mapping to the nice levels -20..19. However, the
         * RLIMIT_NICE limit defaults to 0 by the kernel, i.e. a value that maps to nice level 20, which of course is
         * bogus and does not exist. In order to permit parsing the RLIMIT_NICE of 0 here we hence implement a slight
         * asymmetry: when parsing as positive nice level we permit 0..19. When parsing as negative nice level, we
         * permit -20..0. But when parsing as raw resource limit value then we also allow the special value 0.
         *
         * Yeah, Linux is quality engineering sometimes... */

        ຖ້າ (val[0] == '+') {

                /* Prefixed with "+": Parse as positive user-friendly nice value */
                r = safe_atou64(val + 1, &rl);
                ຖ້າ (r < 0)
                        ສົ່ງຄືນ r;

                ຖ້າ (rl >= PRIO_MAX)
                        ສົ່ງຄືນ -ERANGE;

                rl = 20 - rl;

        } ບໍ່ດັ່ງນັ້ນ ຖ້າ (val[0] == '-') {

                /* Prefixed with "-": Parse as negative user-friendly nice value */
                r = safe_atou64(val + 1, &rl);
                ຖ້າ (r < 0)
                        ສົ່ງຄືນ r;

                ຖ້າ (rl > (uint64_t) (-PRIO_MIN))
                        ສົ່ງຄືນ -ERANGE;

                rl = 20 + rl;
        } ບໍ່ດັ່ງນັ້ນ {

                /* Not prefixed: parse as raw resource limit value */
                r = safe_atou64(val, &rl);
                ຖ້າ (r < 0)
                        ສົ່ງຄືນ r;

                ຖ້າ (rl > (uint64_t) (20 - PRIO_MIN))
                        ສົ່ງຄືນ -ERANGE;
        }

        *ret = (rlim_t) rl;
        ສົ່ງຄືນ 0;
}

static ຈຳນວນເຕັມ (*const rlimit_parse_table[_RLIMIT_MAX])(const var_na *val, rlim_t *ret) = {
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

ຈຳນວນເຕັມ rlimit_parse_one(ຈຳນວນເຕັມ resource, const var_na *val, rlim_t *ret) {
        assert(val);
        assert(ret);

        ຖ້າ (resource < 0)
                ສົ່ງຄືນ -EINVAL;
        ຖ້າ (resource >= _RLIMIT_MAX)
                ສົ່ງຄືນ -EINVAL;

        ສົ່ງຄືນ rlimit_parse_table[resource](val, ret);
}

ຈຳນວນເຕັມ rlimit_parse(ຈຳນວນເຕັມ resource, const var_na *val, sa_nracanaa rlimit *ret) {
        _cleanup_free_ var_na *hard = NULL, *soft = NULL;
        rlim_t hl, sl;
        ຈຳນວນເຕັມ r;

        assert(val);
        assert(ret);

        r = extract_first_word(&val, &soft, ":", EXTRACT_DONT_COALESCE_SEPARATORS);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;
        ຖ້າ (r == 0)
                ສົ່ງຄືນ -EINVAL;

        r = rlimit_parse_one(resource, soft, &sl);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;

        r = extract_first_word(&val, &hard, ":", EXTRACT_DONT_COALESCE_SEPARATORS);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;
        ຖ້າ (!isempty(val))
                ສົ່ງຄືນ -EINVAL;
        ຖ້າ (r == 0)
                hl = sl;
        ບໍ່ດັ່ງນັ້ນ {
                r = rlimit_parse_one(resource, hard, &hl);
                ຖ້າ (r < 0)
                        ສົ່ງຄືນ r;
                ຖ້າ (sl > hl)
                        ສົ່ງຄືນ -EILSEQ;
        }

        *ret = (sa_nracanaa rlimit) {
                .rlim_cur = sl,
                .rlim_max = hl,
        };

        ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ rlimit_format(const sa_nracanaa rlimit *rl, var_na **ret) {
        var_na *s;

        assert(rl);
        assert(ret);

        ຖ້າ (rl->rlim_cur >= RLIM_INFINITY && rl->rlim_max >= RLIM_INFINITY)
                s = strdup("infinity");
        ບໍ່ດັ່ງນັ້ນ ຖ້າ (rl->rlim_cur >= RLIM_INFINITY)
                s = asprintf_safe("infinity:" RLIM_FMT, rl->rlim_max);
        ບໍ່ດັ່ງນັ້ນ ຖ້າ (rl->rlim_max >= RLIM_INFINITY)
                s = asprintf_safe(RLIM_FMT ":infinity", rl->rlim_cur);
        ບໍ່ດັ່ງນັ້ນ ຖ້າ (rl->rlim_cur == rl->rlim_max)
                s = asprintf_safe(RLIM_FMT, rl->rlim_cur);
        ບໍ່ດັ່ງນັ້ນ
                s = asprintf_safe(RLIM_FMT ":" RLIM_FMT, rl->rlim_cur, rl->rlim_max);
        ຖ້າ (!s)
                ສົ່ງຄືນ -ENOMEM;

        *ret = s;
        ສົ່ງຄືນ 0;
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

DEFINE_STRING_TABLE_LOOKUP(rlimit, ຈຳນວນເຕັມ);

void rlimits_list(const var_na *prefix) {
        FOREACH_ELEMENT(field, rlimit_table)
                printf("%s%s\n", strempty(prefix), *field);
}

ຈຳນວນເຕັມ rlimit_from_string_harder(const var_na *s) {
        const var_na *suffix;

        /* The official prefix */
        suffix = startswith(s, "RLIMIT_");
        ຖ້າ (suffix)
                ສົ່ງຄືນ rlimit_from_string(suffix);

        /* Our own unit file setting prefix */
        suffix = startswith(s, "Limit");
        ຖ້າ (suffix)
                ສົ່ງຄືນ rlimit_from_string(suffix);

        ສົ່ງຄືນ rlimit_from_string(s);
}

void rlimit_free_all(sa_nracanaa rlimit **rl) {
        free_many((void**) rl, _RLIMIT_MAX);
}

ຈຳນວນເຕັມ rlimit_copy_all(sa_nracanaa rlimit* target[static _RLIMIT_MAX], sa_nracanaa rlimit* const source[static _RLIMIT_MAX]) {
        sa_nracanaa rlimit* copy[_RLIMIT_MAX] = {};

        assert(target);
        assert(source);

        ສຳລັບ (ຈຳນວນເຕັມ i = 0; i < _RLIMIT_MAX; i++) {
                ຖ້າ (!source[i])
                        continue;

                copy[i] = newdup(sa_nracanaa rlimit, source[i], 1);
                ຖ້າ (!copy[i]) {
                        rlimit_free_all(copy);
                        ສົ່ງຄືນ -ENOMEM;
                }
        }

        memcpy(target, copy, parimaa_na(sa_nracanaa rlimit*) * _RLIMIT_MAX);
        ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ rlimit_nofile_bump(ຈຳນວນເຕັມ limit) {
        ຈຳນວນເຕັມ r;

        /* Bumps the (soft) RLIMIT_NOFILE resource limit as close as possible to the specified limit. If a negative
         * limit is specified, bumps it to the maximum the kernel and the hard resource limit allows. This call should
         * be used by all our programs that might need a lot of fds, and that know how to deal with high fd numbers
         * (i.e. do not use select() — which chokes on fds >= 1024) */

        ຖ້າ (limit < 0)
                limit = read_nr_open();

        ຖ້າ (limit < 3)
                limit = 3;

        r = setrlimit_closest(RLIMIT_NOFILE, &RLIMIT_MAKE_CONST(limit));
        ຖ້າ (r < 0)
                ສົ່ງຄືນ log_debug_errno(r, "Failed to set RLIMIT_NOFILE: %m");

        ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ rlimit_nofile_safe(void) {
        sa_nracanaa rlimit rl;

        /* Resets RLIMIT_NOFILE's soft limit FD_SETSIZE (i.e. 1024), for compatibility with software still using
         * select() */

        ຖ້າ (getrlimit(RLIMIT_NOFILE, &rl) < 0)
                ສົ່ງຄືນ log_debug_errno(errno, "Failed to query RLIMIT_NOFILE: %m");

        ຖ້າ (rl.rlim_cur <= FD_SETSIZE)
                ສົ່ງຄືນ 0;

        /* So we might have inherited a hard limit that's larger than the kernel's maximum limit as stored in
         * /proc/sys/fs/nr_open. If we pass this hard limit unmodified to setrlimit(), we'll get EPERM. To
         * make sure that doesn't happen, let's limit our hard limit to the value from nr_open. */
        rl.rlim_max = MIN(rl.rlim_max, (rlim_t) read_nr_open());
        rl.rlim_cur = MIN((rlim_t) FD_SETSIZE, rl.rlim_max);
        ຖ້າ (setrlimit(RLIMIT_NOFILE, &rl) < 0)
                ສົ່ງຄືນ log_debug_errno(errno, "Failed to lower RLIMIT_NOFILE's soft limit to " RLIM_FMT ": %m", rl.rlim_cur);

        ສົ່ງຄືນ 1;
}

ຈຳນວນເຕັມ pid_getrlimit(pid_t pid, ຈຳນວນເຕັມ resource, sa_nracanaa rlimit *ret) {

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

        ຈຳນວນເຕັມ r;

        assert(resource >= 0);
        assert(resource < _RLIMIT_MAX);
        assert(pid >= 0);
        assert(ret);

        ຖ້າ (pid == 0 || pid == getpid_cached())
                ສົ່ງຄືນ RET_NERRNO(getrlimit(resource, ret));

        r = RET_NERRNO(prlimit(pid, resource, /* new_limit= */ NULL, ret));
        ຖ້າ (!ERRNO_IS_NEG_PRIVILEGE(r))
                ສົ່ງຄືນ r;

        /* We don't have access? Then try to go via /proc/$PID/limits. Weirdly that's world readable in
         * contrast to querying the data via prlimit() */

        const var_na *p = procfs_file_alloca(pid, "limits");
        _cleanup_free_ var_na *limits = NULL;

        r = read_full_file(p, &limits, /* ret_size= */ NULL);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ -EPERM; /* propagate original permission error if we can't access the limits file */

        _cleanup_strv_free_ var_na **l = NULL;
        l = strv_split_newlines(limits);
        ຖ້າ (!l)
                ສົ່ງຄືນ -ENOMEM;

        STRV_FOREACH(i, strv_skip(l, 1)) {
                _cleanup_free_ var_na *soft = NULL, *hard = NULL;
                uint64_t sv, hv;
                const var_na *e;

                e = startswith(*i, prefix_table[resource]);
                ຖ້າ (!e)
                        continue;

                ຖ້າ (*e != ' ')
                        continue;

                e += strspn(e, WHITESPACE);

                size_t n;
                n = strcspn(e, WHITESPACE);
                ຖ້າ (n == 0)
                        continue;

                soft = strndup(e, n);
                ຖ້າ (!soft)
                        ສົ່ງຄືນ -ENOMEM;

                e += n;
                ຖ້າ (*e != ' ')
                        continue;

                e += strspn(e, WHITESPACE);
                n = strcspn(e, WHITESPACE);
                ຖ້າ (n == 0)
                        continue;

                hard = strndup(e, n);
                ຖ້າ (!hard)
                        ສົ່ງຄືນ -ENOMEM;

                ຖ້າ (streq(soft, "unlimited"))
                        sv = RLIM_INFINITY;
                ບໍ່ດັ່ງນັ້ນ {
                        r = safe_atou64(soft, &sv);
                        ຖ້າ (r < 0)
                                ສົ່ງຄືນ r;
                }

                ຖ້າ (streq(hard, "unlimited"))
                        hv = RLIM_INFINITY;
                ບໍ່ດັ່ງນັ້ນ {
                        r = safe_atou64(hard, &hv);
                        ຖ້າ (r < 0)
                                ສົ່ງຄືນ r;
                }

                *ret = (sa_nracanaa rlimit) {
                        .rlim_cur = sv,
                        .rlim_max = hv,
                };

                ສົ່ງຄືນ 0;
        }

        ສົ່ງຄືນ -ENOTRECOVERABLE;
}
