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

ಪೂರ್ಣಾಂಕ setrlimit_closest(ಪೂರ್ಣಾಂಕ resource, ಸ್ಥಿರಾಂಕ ರಚನೆ rlimit *rlim) {
        ರಚನೆ rlimit highest, fixed;

        ಪ್ರತಿಜ್ಞಾ(rlim);

        ಆದರೆ (setrlimit(resource, rlim) >= 0)
                ಹಿಂತಿರುಗಿ 0;

        ಆದರೆ (errno != EPERM)
                ಹಿಂತಿರುಗಿ -errno;

        /* So we failed to set the desired setrlimit, then let's try
         * to get as close as we can */
        ಆದರೆ (getrlimit(resource, &highest) < 0)
                ಹಿಂತಿರುಗಿ -errno;

        /* If the hard limit is unbounded anyway, then the EPERM had other reasons, let's propagate the original EPERM
         * then */
        ಆದರೆ (highest.rlim_max == RLIM_INFINITY)
                ಹಿಂತಿರುಗಿ -EPERM;

        fixed = (ರಚನೆ rlimit) {
                .rlim_cur = MIN(rlim->rlim_cur, highest.rlim_max),
                .rlim_max = MIN(rlim->rlim_max, highest.rlim_max),
        };

        /* Shortcut things if we wouldn't change anything. */
        ಆದರೆ (fixed.rlim_cur == highest.rlim_cur &&
            fixed.rlim_max == highest.rlim_max)
                ಹಿಂತಿರುಗಿ 0;

        log_debug("Failed at setting rlimit " RLIM_FMT " for resource RLIMIT_%s. Will attempt setting value " RLIM_FMT " instead.", rlim->rlim_max, rlimit_ಗಮ್ಯ_string(resource), fixed.rlim_max);

        ಹಿಂತಿರುಗಿ RET_NERRNO(setrlimit(resource, &fixed));
}

ಪೂರ್ಣಾಂಕ setrlimit_closest_all(ಸ್ಥಿರಾಂಕ ರಚನೆ rlimit *ಸ್ಥಿರಾಂಕ *rlim, ಪೂರ್ಣಾಂಕ *which_failed) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(rlim);

        /* On failure returns the limit's index that failed in *which_failed, but only if non-NULL */

        ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < _RLIMIT_MAX; i++) {
                ಆದರೆ (!rlim[i])
                        ಮುಂದುವರಿ;

                r = setrlimit_closest(i, rlim[i]);
                ಆದರೆ (r < 0) {
                        ಆದರೆ (which_failed)
                                *which_failed = i;

                        ಹಿಂತಿರುಗಿ r;
                }
        }

        ಆದರೆ (which_failed)
                *which_failed = -1;

        ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ rlimit_parse_u64(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *val, rlim_t *ret) {
        uint64_t u;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(val);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಆದರೆ (streq(val, "infinity")) {
                *ret = RLIM_INFINITY;
                ಹಿಂತಿರುಗಿ 0;
        }

        /* setrlimit(2) suggests rlim_t is always 64-bit on Linux. */
        assert_cc(ಗಾತ್ರ(rlim_t) == ಗಾತ್ರ(uint64_t));

        r = ಸುರಕ್ಷಿತ_atou64(val, &u);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;
        ಆದರೆ (u >= (uint64_t) RLIM_INFINITY)
                ಹಿಂತಿರುಗಿ -ERANGE;

        *ret = (rlim_t) u;
        ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ rlimit_parse_ಗಾತ್ರ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *val, rlim_t *ret) {
        uint64_t u;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(val);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಆದರೆ (streq(val, "infinity")) {
                *ret = RLIM_INFINITY;
                ಹಿಂತಿರುಗಿ 0;
        }

        r = parse_ಗಾತ್ರ(val, 1024, &u);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;
        ಆದರೆ (u >= (uint64_t) RLIM_INFINITY)
                ಹಿಂತಿರುಗಿ -ERANGE;

        *ret = (rlim_t) u;
        ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ rlimit_parse_sec(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *val, rlim_t *ret) {
        uint64_t u;
        usec_t t;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(val);
        ಪ್ರತಿಜ್ಞಾ(ret);

        r = parse_sec(val, &t);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;
        ಆದರೆ (t == USEC_INFINITY) {
                *ret = RLIM_INFINITY;
                ಹಿಂತಿರುಗಿ 0;
        }

        u = (uint64_t) DIV_ಸುತ್ತು_ಮೇಲೆ(t, USEC_ಪ್ರತಿ_SEC);
        ಆದರೆ (u >= (uint64_t) RLIM_INFINITY)
                ಹಿಂತಿರುಗಿ -ERANGE;

        *ret = (rlim_t) u;
        ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ rlimit_parse_usec(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *val, rlim_t *ret) {
        usec_t t;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(val);
        ಪ್ರತಿಜ್ಞಾ(ret);

        r = parse_ಸಮಯ(val, &t, 1);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;
        ಆದರೆ (t == USEC_INFINITY) {
                *ret = RLIM_INFINITY;
                ಹಿಂತಿರುಗಿ 0;
        }

        *ret = (rlim_t) t;
        ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ rlimit_parse_nice(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *val, rlim_t *ret) {
        uint64_t rl;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(val);
        ಪ್ರತಿಜ್ಞಾ(ret);

        /* So, Linux is weird. The range for RLIMIT_NICE is 40..1, mapping to the nice levels -20..19. However, the
         * RLIMIT_NICE limit defaults to 0 by the kernel, i.e. a value that maps to nice level 20, which of course is
         * bogus and does not exist. In order to permit parsing the RLIMIT_NICE of 0 here we hence implement a slight
         * asymmetry: when parsing as positive nice level we permit 0..19. When parsing as negative nice level, we
         * permit -20..0. But when parsing as raw resource limit value then we also allow the special value 0.
         *
         * Yeah, Linux is quality engineering sometimes... */

        ಆದರೆ (val[0] == '+') {

                /* Prefixed with "+": Parse as positive user-friendly nice value */
                r = ಸುರಕ್ಷಿತ_atou64(val + 1, &rl);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                ಆದರೆ (rl >= PRIO_MAX)
                        ಹಿಂತಿರುಗಿ -ERANGE;

                rl = 20 - rl;

        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (val[0] == '-') {

                /* Prefixed with "-": Parse as negative user-friendly nice value */
                r = ಸುರಕ್ಷಿತ_atou64(val + 1, &rl);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                ಆದರೆ (rl > (uint64_t) (-PRIO_MIN))
                        ಹಿಂತಿರುಗಿ -ERANGE;

                rl = 20 + rl;
        } ಇಲ್ಲದಿದ್ದರೆ {

                /* Not prefixed: parse as raw resource limit value */
                r = ಸುರಕ್ಷಿತ_atou64(val, &rl);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                ಆದರೆ (rl > (uint64_t) (20 - PRIO_MIN))
                        ಹಿಂತಿರುಗಿ -ERANGE;
        }

        *ret = (rlim_t) rl;
        ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ (*ಸ್ಥಿರಾಂಕ rlimit_parse_ಕೋಷ್ಟಕ[_RLIMIT_MAX])(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *val, rlim_t *ret) = {
        [RLIMIT_CPU]        = rlimit_parse_sec,
        [RLIMIT_FSIZE]      = rlimit_parse_ಗಾತ್ರ,
        [RLIMIT_DATA]       = rlimit_parse_ಗಾತ್ರ,
        [RLIMIT_STACK]      = rlimit_parse_ಗಾತ್ರ,
        [RLIMIT_CORE]       = rlimit_parse_ಗಾತ್ರ,
        [RLIMIT_RSS]        = rlimit_parse_ಗಾತ್ರ,
        [RLIMIT_NOFILE]     = rlimit_parse_u64,
        [RLIMIT_AS]         = rlimit_parse_ಗಾತ್ರ,
        [RLIMIT_NPROC]      = rlimit_parse_u64,
        [RLIMIT_MEMLOCK]    = rlimit_parse_ಗಾತ್ರ,
        [RLIMIT_LOCKS]      = rlimit_parse_u64,
        [RLIMIT_SIGPENDING] = rlimit_parse_u64,
        [RLIMIT_MSGQUEUE]   = rlimit_parse_ಗಾತ್ರ,
        [RLIMIT_NICE]       = rlimit_parse_nice,
        [RLIMIT_RTPRIO]     = rlimit_parse_u64,
        [RLIMIT_RTTIME]     = rlimit_parse_usec,
};

ಪೂರ್ಣಾಂಕ rlimit_parse_one(ಪೂರ್ಣಾಂಕ resource, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *val, rlim_t *ret) {
        ಪ್ರತಿಜ್ಞಾ(val);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಆದರೆ (resource < 0)
                ಹಿಂತಿರುಗಿ -EINVAL;
        ಆದರೆ (resource >= _RLIMIT_MAX)
                ಹಿಂತಿರುಗಿ -EINVAL;

        ಹಿಂತಿರುಗಿ rlimit_parse_ಕೋಷ್ಟಕ[resource](val, ret);
}

ಪೂರ್ಣಾಂಕ rlimit_parse(ಪೂರ್ಣಾಂಕ resource, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *val, ರಚನೆ rlimit *ret) {
        _cleanup_free_ ಅಕ್ಷರ *hard = ಖಾಲಿ, *soft = ಖಾಲಿ;
        rlim_t hl, sl;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(val);
        ಪ್ರತಿಜ್ಞಾ(ret);

        r = ಹೊರತೆಗೆ_ಮೊದಲ_ಶಬ್ದ(&val, &soft, ":", ಹೊರತೆಗೆ_DONT_COALESCE_SEPARATORS);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;
        ಆದರೆ (r == 0)
                ಹಿಂತಿರುಗಿ -EINVAL;

        r = rlimit_parse_one(resource, soft, &sl);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        r = ಹೊರತೆಗೆ_ಮೊದಲ_ಶಬ್ದ(&val, &hard, ":", ಹೊರತೆಗೆ_DONT_COALESCE_SEPARATORS);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;
        ಆದರೆ (!isempty(val))
                ಹಿಂತಿರುಗಿ -EINVAL;
        ಆದರೆ (r == 0)
                hl = sl;
        ಇಲ್ಲದಿದ್ದರೆ {
                r = rlimit_parse_one(resource, hard, &hl);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
                ಆದರೆ (sl > hl)
                        ಹಿಂತಿರುಗಿ -EILSEQ;
        }

        *ret = (ರಚನೆ rlimit) {
                .rlim_cur = sl,
                .rlim_max = hl,
        };

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ rlimit_format(ಸ್ಥಿರಾಂಕ ರಚನೆ rlimit *rl, ಅಕ್ಷರ **ret) {
        ಅಕ್ಷರ *s;

        ಪ್ರತಿಜ್ಞಾ(rl);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಆದರೆ (rl->rlim_cur >= RLIM_INFINITY && rl->rlim_max >= RLIM_INFINITY)
                s = strdup("infinity");
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (rl->rlim_cur >= RLIM_INFINITY)
                s = asprintf_ಸುರಕ್ಷಿತ("infinity:" RLIM_FMT, rl->rlim_max);
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (rl->rlim_max >= RLIM_INFINITY)
                s = asprintf_ಸುರಕ್ಷಿತ(RLIM_FMT ":infinity", rl->rlim_cur);
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (rl->rlim_cur == rl->rlim_max)
                s = asprintf_ಸುರಕ್ಷಿತ(RLIM_FMT, rl->rlim_cur);
        ಇಲ್ಲದಿದ್ದರೆ
                s = asprintf_ಸುರಕ್ಷಿತ(RLIM_FMT ":" RLIM_FMT, rl->rlim_cur, rl->rlim_max);
        ಆದರೆ (!s)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        *ret = s;
        ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಸ್ಥಿರಾಂಕ rlimit_ಕೋಷ್ಟಕ[_RLIMIT_MAX] = {
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

DEFINE_STRING_ಕೋಷ್ಟಕ_LOOKUP(rlimit, ಪೂರ್ಣಾಂಕ);

ಶೂನ್ಯ rlimits_list(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪೂರ್ವಪ್ರತ್ಯಯ) {
        FOREACH_ELEMENT(ಕ್ಷೇತ್ರ, rlimit_ಕೋಷ್ಟಕ)
                ಬರೆ("%s%s\n", strempty(ಪೂರ್ವಪ್ರತ್ಯಯ), *ಕ್ಷೇತ್ರ);
}

ಪೂರ್ಣಾಂಕ rlimit_from_string_harder(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *suffix;

        /* The official prefix */
        suffix = startswith(s, "RLIMIT_");
        ಆದರೆ (suffix)
                ಹಿಂತಿರುಗಿ rlimit_from_string(suffix);

        /* Our own unit file setting prefix */
        suffix = startswith(s, "Limit");
        ಆದರೆ (suffix)
                ಹಿಂತಿರುಗಿ rlimit_from_string(suffix);

        ಹಿಂತಿರುಗಿ rlimit_from_string(s);
}

ಶೂನ್ಯ rlimit_free_all(ರಚನೆ rlimit **rl) {
        free_many((ಶೂನ್ಯ**) rl, _RLIMIT_MAX);
}

ಪೂರ್ಣಾಂಕ rlimit_copy_all(ರಚನೆ rlimit* ಗುರಿ[ಸ್ಥಿರ _RLIMIT_MAX], ರಚನೆ rlimit* ಸ್ಥಿರಾಂಕ ಮೂಲ[ಸ್ಥಿರ _RLIMIT_MAX]) {
        ರಚನೆ rlimit* copy[_RLIMIT_MAX] = {};

        ಪ್ರತಿಜ್ಞಾ(ಗುರಿ);
        ಪ್ರತಿಜ್ಞಾ(ಮೂಲ);

        ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < _RLIMIT_MAX; i++) {
                ಆದರೆ (!ಮೂಲ[i])
                        ಮುಂದುವರಿ;

                copy[i] = newdup(ರಚನೆ rlimit, ಮೂಲ[i], 1);
                ಆದರೆ (!copy[i]) {
                        rlimit_free_all(copy);
                        ಹಿಂತಿರುಗಿ -ENOMEM;
                }
        }

        ಸ್ಮರಣೆ_ನಕಲು(ಗುರಿ, copy, ಗಾತ್ರ(ರಚನೆ rlimit*) * _RLIMIT_MAX);
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ rlimit_nofile_bump(ಪೂರ್ಣಾಂಕ limit) {
        ಪೂರ್ಣಾಂಕ r;

        /* Bumps the (soft) RLIMIT_NOFILE resource limit as close as possible to the specified limit. If a negative
         * limit is specified, bumps it to the maximum the kernel and the hard resource limit allows. This call should
         * be used by all our programs that might need a lot of fds, and that know how to deal with high fd numbers
         * (i.e. do not use select() — which chokes on fds >= 1024) */

        ಆದರೆ (limit < 0)
                limit = read_nr_open();

        ಆದರೆ (limit < 3)
                limit = 3;

        r = setrlimit_closest(RLIMIT_NOFILE, &RLIMIT_MAKE_CONST(limit));
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ log_debug_errno(r, "Failed to set RLIMIT_NOFILE: %m");

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ rlimit_nofile_ಸುರಕ್ಷಿತ(ಶೂನ್ಯ) {
        ರಚನೆ rlimit rl;

        /* Resets RLIMIT_NOFILE's soft limit FD_SETSIZE (i.e. 1024), for compatibility with software still using
         * select() */

        ಆದರೆ (getrlimit(RLIMIT_NOFILE, &rl) < 0)
                ಹಿಂತಿರುಗಿ log_debug_errno(errno, "Failed to query RLIMIT_NOFILE: %m");

        ಆದರೆ (rl.rlim_cur <= FD_SETSIZE)
                ಹಿಂತಿರುಗಿ 0;

        /* So we might have inherited a hard limit that's larger than the kernel's maximum limit as stored in
         * /proc/sys/fs/nr_open. If we pass this hard limit unmodified to setrlimit(), we'll get EPERM. To
         * make sure that doesn't happen, let's limit our hard limit to the value from nr_open. */
        rl.rlim_max = MIN(rl.rlim_max, (rlim_t) read_nr_open());
        rl.rlim_cur = MIN((rlim_t) FD_SETSIZE, rl.rlim_max);
        ಆದರೆ (setrlimit(RLIMIT_NOFILE, &rl) < 0)
                ಹಿಂತಿರುಗಿ log_debug_errno(errno, "Failed to lower RLIMIT_NOFILE's soft limit to " RLIM_FMT ": %m", rl.rlim_cur);

        ಹಿಂತಿರುಗಿ 1;
}

ಪೂರ್ಣಾಂಕ pid_getrlimit(pid_t pid, ಪೂರ್ಣಾಂಕ resource, ರಚನೆ rlimit *ret) {

        ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ ಪೂರ್ವಪ್ರತ್ಯಯ_ಕೋಷ್ಟಕ[_RLIMIT_MAX] = {
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

        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(resource >= 0);
        ಪ್ರತಿಜ್ಞಾ(resource < _RLIMIT_MAX);
        ಪ್ರತಿಜ್ಞಾ(pid >= 0);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಆದರೆ (pid == 0 || pid == getpid_cached())
                ಹಿಂತಿರುಗಿ RET_NERRNO(getrlimit(resource, ret));

        r = RET_NERRNO(prlimit(pid, resource, /* new_limit= */ ಖಾಲಿ, ret));
        ಆದರೆ (!ERRNO_IS_NEG_PRIVILEGE(r))
                ಹಿಂತಿರುಗಿ r;

        /* We don't have access? Then try to go via /proc/$PID/limits. Weirdly that's world readable in
         * contrast to querying the data via prlimit() */

        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = procfs_file_alloca(pid, "limits");
        _cleanup_free_ ಅಕ್ಷರ *limits = ಖಾಲಿ;

        r = read_ತುಂಬಿದ_file(p, &limits, /* ret_size= */ ಖಾಲಿ);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ -EPERM; /* propagate original permission error if we can't access the limits file */

        _cleanup_strv_free_ ಅಕ್ಷರ **l = ಖಾಲಿ;
        l = strv_ವಿಭಜಿಸು_newlines(limits);
        ಆದರೆ (!l)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        STRV_FOREACH(i, strv_skip(l, 1)) {
                _cleanup_free_ ಅಕ್ಷರ *soft = ಖಾಲಿ, *hard = ಖಾಲಿ;
                uint64_t sv, hv;
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e;

                e = startswith(*i, ಪೂರ್ವಪ್ರತ್ಯಯ_ಕೋಷ್ಟಕ[resource]);
                ಆದರೆ (!e)
                        ಮುಂದುವರಿ;

                ಆದರೆ (*e != ' ')
                        ಮುಂದುವರಿ;

                e += strspn(e, WHITESPACE);

                size_t n;
                n = strcspn(e, WHITESPACE);
                ಆದರೆ (n == 0)
                        ಮುಂದುವರಿ;

                soft = strndup(e, n);
                ಆದರೆ (!soft)
                        ಹಿಂತಿರುಗಿ -ENOMEM;

                e += n;
                ಆದರೆ (*e != ' ')
                        ಮುಂದುವರಿ;

                e += strspn(e, WHITESPACE);
                n = strcspn(e, WHITESPACE);
                ಆದರೆ (n == 0)
                        ಮುಂದುವರಿ;

                hard = strndup(e, n);
                ಆದರೆ (!hard)
                        ಹಿಂತಿರುಗಿ -ENOMEM;

                ಆದರೆ (streq(soft, "unlimited"))
                        sv = RLIM_INFINITY;
                ಇಲ್ಲದಿದ್ದರೆ {
                        r = ಸುರಕ್ಷಿತ_atou64(soft, &sv);
                        ಆದರೆ (r < 0)
                                ಹಿಂತಿರುಗಿ r;
                }

                ಆದರೆ (streq(hard, "unlimited"))
                        hv = RLIM_INFINITY;
                ಇಲ್ಲದಿದ್ದರೆ {
                        r = ಸುರಕ್ಷಿತ_atou64(hard, &hv);
                        ಆದರೆ (r < 0)
                                ಹಿಂತಿರುಗಿ r;
                }

                *ret = (ರಚನೆ rlimit) {
                        .rlim_cur = sv,
                        .rlim_max = hv,
                };

                ಹಿಂತಿರುಗಿ 0;
        }

        ಹಿಂತಿರುಗಿ -ENOTRECOVERABLE;
}
