/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ <signal.h>     /* IWYU pragma: export */

#ಅನ್ತರ್ಭಾವ "forward.h"

ಪೂರ್ಣಾಙ್ಕ reset_all_signal_handlers(ಶೂನ್ಯ);
ಪೂರ್ಣಾಙ್ಕ reset_signal_mask(ಶೂನ್ಯ);

ಪೂರ್ಣಾಙ್ಕ sigaction_many_internal(ಧ್ರುವ ಸಂರಚನಾ sigaction *sa, ...);

#define ignore_signals(...)                                             \
        sigaction_many_internal(                                        \
                        &sigaction_ignore,                              \
                        __VA_ARGS__,                                    \
                        -1)

#define default_signals(...)                                            \
        sigaction_many_internal(                                        \
                        &sigaction_default,                             \
                        __VA_ARGS__,                                    \
                        -1)

#define sigaction_many(sa, ...)                                         \
        sigaction_many_internal(sa, __VA_ARGS__, -1)

ಪೂರ್ಣಾಙ್ಕ sigset_add_many_internal(sigset_t *ss, ...);
#define sigset_add_many(...) sigset_add_many_internal(__VA_ARGS__, -1)

ಪೂರ್ಣಾಙ್ಕ sigprocmask_many_internal(ಪೂರ್ಣಾಙ್ಕ how, sigset_t *ret_old_mask, ...);
#define sigprocmask_many(...) sigprocmask_many_internal(__VA_ARGS__, -1)

DECLARE_STRING_TABLE_LOOKUP(ಸಙ್ಕೇತ, ಪೂರ್ಣಾಙ್ಕ);
ಧ್ರುವ ವರ್ಣ* signal_code_to_string(ಪೂರ್ಣಾಙ್ಕ signo, ಪೂರ್ಣಾಙ್ಕ code) _const_;

ಶೂನ್ಯ nop_signal_handler(ಪೂರ್ಣಾಙ್ಕ sig);

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ block_signals_reset(sigset_t **ss) {
        ಪ್ರತಿಜ್ಞಾ(ss);

        ಯದಿ (!*ss)
                ಪ್ರತ್ಯಾಗಮ;

        assert_log(sigprocmask(SIG_SETMASK, *ss, ರಿಕ್ತ) >= 0);
}

#define BLOCK_SIGNALS(...)                                              \
        sigset_t _saved_sigset;                                         \
        _cleanup_(block_signals_reset) _unused_ sigset_t *_saved_sigsetp = \
                assert_log(sigprocmask_many(SIG_BLOCK, &_saved_sigset, __VA_ARGS__) >= 0) ? \
                &_saved_sigset : ರಿಕ್ತ;

#define SIGNO_INVALID (-EINVAL)

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool SIGNAL_VALID(ಪೂರ್ಣಾಙ್ಕ signo) {
        ಪ್ರತ್ಯಾಗಮ signo > 0 && signo < _NSIG;
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಧ್ರುವ ವರ್ಣ* signal_to_string_with_check(ಪೂರ್ಣಾಙ್ಕ n) {
        ಯದಿ (!SIGNAL_VALID(n))
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        ಪ್ರತ್ಯಾಗಮ signal_to_string(n);
}

ಪೂರ್ಣಾಙ್ಕ signal_is_blocked(ಪೂರ್ಣಾಙ್ಕ sig);

ಪೂರ್ಣಾಙ್ಕ autoreaping_enabled(ಶೂನ್ಯ);

ಪೂರ್ಣಾಙ್ಕ ತೆಗೆ_pending_signal_internal(ಪೂರ್ಣಾಙ್ಕ sig, ...);
#define ತೆಗೆ_pending_signal(...) ತೆಗೆ_pending_signal_internal(__VA_ARGS__, -1)

ಶೂನ್ಯ propagate_signal(ಪೂರ್ಣಾಙ್ಕ sig, siginfo_t *siginfo);

ಬಾಹ್ಯ ಧ್ರುವ ಸಂರಚನಾ sigaction sigaction_ignore;
ಬಾಹ್ಯ ಧ್ರುವ ಸಂರಚನಾ sigaction sigaction_default;
ಬಾಹ್ಯ ಧ್ರುವ ಸಂರಚನಾ sigaction sigaction_nop_nocldstop;

ಪೂರ್ಣಾಙ್ಕ parse_signo(ಧ್ರುವ ವರ್ಣ *s, ಪೂರ್ಣಾಙ್ಕ *ret);

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool si_code_from_process(ಪೂರ್ಣಾಙ್ಕ si_code) {
        /* Returns true if the .si_code field of siginfo_t or the .ssi_code field of struct signalfd_siginfo
         * indicate that the signal originates from a userspace process, and hence the .si_pid/.ssi_pid field
         * is valid. This check is not obvious, since on one hand SI_USER/SI_QUEUE are supposed to be the
         * values that kill() and sigqueue() set, and that's documented in sigaction(2), but on the other
         * hand rt_sigqueueinfo(2) says userspace can actually set any value below zero. Hence check for
         * either.
         *
         * Also quoting POSIX:
         *
         * "On systems not supporting the XSI option, the si_pid and si_uid members of siginfo_t are only
         * required to be valid when si_code is SI_USER or SI_QUEUE. On XSI-conforming systems, they are also
         * valid for all si_code values less than or equal to 0; however, it is unspecified whether SI_USER
         * and SI_QUEUE have values less than or equal to zero, and therefore XSI applications should check
         * whether si_code has the value SI_USER or SI_QUEUE or is less than or equal to 0 to tell whether
         * si_pid and si_uid are valid."
         *
         * From: https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/signal.h.html */

        ಪ್ರತ್ಯಾಗಮ si_code < 0 || IN_SET(si_code, SI_USER, SI_QUEUE);
}

ಶೂನ್ಯ sigterm_process_group_handler(ಪೂರ್ಣಾಙ್ಕ ಸಙ್ಕೇತ, siginfo_t *info, ಶೂನ್ಯ *ucontext);
