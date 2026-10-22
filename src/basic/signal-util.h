/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ <signal.h>     /* IWYU pragma: export */

#ಅನ್ತರ್ಭಾವ "forward.h"

ಪೂರ್ಣಾಂಕ reset_all_signal_handlers(ಶೂನ್ಯ);
ಪೂರ್ಣಾಂಕ reset_signal_ಮುಖವಾಡ(ಶೂನ್ಯ);

ಪೂರ್ಣಾಂಕ sigaction_many_ಆಂತರಿಕ(ಸ್ಥಿರಾಂಕ ರಚನೆ sigaction *sa, ...);

#ವ್ಯಾಖ್ಯಾನಿಸು ignore_ಸಂಕೇತಗಳು(...)                                             \
        sigaction_many_ಆಂತರಿಕ(                                        \
                        &sigaction_ignore,                              \
                        __VA_ARGS__,                                    \
                        -1)

#ವ್ಯಾಖ್ಯಾನಿಸು default_ಸಂಕೇತಗಳು(...)                                            \
        sigaction_many_ಆಂತರಿಕ(                                        \
                        &sigaction_default,                             \
                        __VA_ARGS__,                                    \
                        -1)

#ವ್ಯಾಖ್ಯಾನಿಸು sigaction_many(sa, ...)                                         \
        sigaction_many_ಆಂತರಿಕ(sa, __VA_ARGS__, -1)

ಪೂರ್ಣಾಂಕ sigset_ಕೂಡಿಸು_many_ಆಂತರಿಕ(sigset_t *ss, ...);
#ವ್ಯಾಖ್ಯಾನಿಸು sigset_ಕೂಡಿಸು_many(...) sigset_ಕೂಡಿಸು_many_ಆಂತರಿಕ(__VA_ARGS__, -1)

ಪೂರ್ಣಾಂಕ sigprocmask_many_ಆಂತರಿಕ(ಪೂರ್ಣಾಂಕ how, sigset_t *ret_old_ಮುಖವಾಡ, ...);
#ವ್ಯಾಖ್ಯಾನಿಸು sigprocmask_many(...) sigprocmask_many_ಆಂತರಿಕ(__VA_ARGS__, -1)

DECLARE_STRING_ಕೋಷ್ಟಕ_LOOKUP(ಸಂಕೇತ, ಪೂರ್ಣಾಂಕ);
ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* signal_ಸಂಕೇತ_ಗಮ್ಯ_string(ಪೂರ್ಣಾಂಕ signo, ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2) _const_;

ಶೂನ್ಯ nop_signal_handler(ಪೂರ್ಣಾಂಕ sig);

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ ಖಂಡ_ಸಂಕೇತಗಳು_reset(sigset_t **ss) {
        ಪ್ರತಿಜ್ಞಾ(ss);

        ಆದರೆ (!*ss)
                ಹಿಂತಿರುಗಿ;

        assert_log(sigprocmask(SIG_SETMASK, *ss, ಖಾಲಿ) >= 0);
}

#ವ್ಯಾಖ್ಯಾನಿಸು ಖಂಡ_ಸಂಕೇತಗಳು_2(...)                                              \
        sigset_t _saved_sigset;                                         \
        _cleanup_(ಖಂಡ_ಸಂಕೇತಗಳು_reset) ಬಳಸದ sigset_t *_saved_sigsetp = \
                assert_log(sigprocmask_many(SIG_BLOCK, &_saved_sigset, __VA_ARGS__) >= 0) ? \
                &_saved_sigset : ಖಾಲಿ;

#ವ್ಯಾಖ್ಯಾನಿಸು SIGNO_INVALID (-EINVAL)

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool SIGNAL_VALID(ಪೂರ್ಣಾಂಕ signo) {
        ಹಿಂತಿರುಗಿ signo > 0 && signo < _NSIG;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* signal_ಗಮ್ಯ_string_with_check(ಪೂರ್ಣಾಂಕ n) {
        ಆದರೆ (!SIGNAL_VALID(n))
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಹಿಂತಿರುಗಿ signal_ಗಮ್ಯ_string(n);
}

ಪೂರ್ಣಾಂಕ signal_is_blocked(ಪೂರ್ಣಾಂಕ sig);

ಪೂರ್ಣಾಂಕ autoreaping_enabled(ಶೂನ್ಯ);

ಪೂರ್ಣಾಂಕ ತೆಗೆ_pending_signal_ಆಂತರಿಕ(ಪೂರ್ಣಾಂಕ sig, ...);
#ವ್ಯಾಖ್ಯಾನಿಸು ತೆಗೆ_pending_signal(...) ತೆಗೆ_pending_signal_ಆಂತರಿಕ(__VA_ARGS__, -1)

ಶೂನ್ಯ propagate_signal(ಪೂರ್ಣಾಂಕ sig, siginfo_t *siginfo);

ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ sigaction sigaction_ignore;
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ sigaction sigaction_default;
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ sigaction sigaction_nop_nocldstop;

ಪೂರ್ಣಾಂಕ parse_signo(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ಪೂರ್ಣಾಂಕ *ret);

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool si_ಸಂಕೇತ_from_process(ಪೂರ್ಣಾಂಕ si_code) {
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

        ಹಿಂತಿರುಗಿ si_code < 0 || ಒಳ_ಗಣ(si_code, SI_USER, SI_QUEUE);
}

ಶೂನ್ಯ sigterm_process_ಗುಂಪು_handler(ಪೂರ್ಣಾಂಕ ಸಂಕೇತ, siginfo_t *info, ಶೂನ್ಯ *ucontext);
