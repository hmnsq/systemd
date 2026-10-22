/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव <signal.h>     /* IWYU pragma: export */

#अन्तर्भाव "forward.h"

पूर्णांक reset_all_signal_handlers(शून्य);
पूर्णांक reset_signal_mask(शून्य);

पूर्णांक sigaction_many_internal(स्थिरांक संरचना sigaction *sa, ...);

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

पूर्णांक sigset_add_many_internal(sigset_t *ss, ...);
#define sigset_add_many(...) sigset_add_many_internal(__VA_ARGS__, -1)

पूर्णांक sigprocmask_many_internal(पूर्णांक how, sigset_t *ret_old_mask, ...);
#define sigprocmask_many(...) sigprocmask_many_internal(__VA_ARGS__, -1)

DECLARE_STRING_TABLE_LOOKUP(संकेत, पूर्णांक);
स्थिरांक अक्षर* signal_code_to_string(पूर्णांक signo, पूर्णांक code) _const_;

शून्य nop_signal_handler(पूर्णांक sig);

स्थिर ओळीत शून्य block_signals_reset(sigset_t **ss) {
        प्रतिज्ञा(ss);

        जर (!*ss)
                परत;

        assert_log(sigprocmask(SIG_SETMASK, *ss, रिक्त) >= 0);
}

#define BLOCK_SIGNALS(...)                                              \
        sigset_t _saved_sigset;                                         \
        _cleanup_(block_signals_reset) _unused_ sigset_t *_saved_sigsetp = \
                assert_log(sigprocmask_many(SIG_BLOCK, &_saved_sigset, __VA_ARGS__) >= 0) ? \
                &_saved_sigset : रिक्त;

#define SIGNO_INVALID (-EINVAL)

स्थिर ओळीत bool SIGNAL_VALID(पूर्णांक signo) {
        परत signo > 0 && signo < _NSIG;
}

स्थिर ओळीत स्थिरांक अक्षर* signal_to_string_with_check(पूर्णांक n) {
        जर (!SIGNAL_VALID(n))
                परत रिक्त;

        परत signal_to_string(n);
}

पूर्णांक signal_is_blocked(पूर्णांक sig);

पूर्णांक autoreaping_enabled(शून्य);

पूर्णांक काढा_pending_signal_internal(पूर्णांक sig, ...);
#define काढा_pending_signal(...) काढा_pending_signal_internal(__VA_ARGS__, -1)

शून्य propagate_signal(पूर्णांक sig, siginfo_t *siginfo);

बाह्य स्थिरांक संरचना sigaction sigaction_ignore;
बाह्य स्थिरांक संरचना sigaction sigaction_default;
बाह्य स्थिरांक संरचना sigaction sigaction_nop_nocldstop;

पूर्णांक parse_signo(स्थिरांक अक्षर *s, पूर्णांक *ret);

स्थिर ओळीत bool si_code_from_process(पूर्णांक si_code) {
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

        परत si_code < 0 || IN_SET(si_code, SI_USER, SI_QUEUE);
}

शून्य sigterm_process_group_handler(पूर्णांक संकेत, siginfo_t *info, शून्य *ucontext);
