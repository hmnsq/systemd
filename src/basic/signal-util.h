/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव <signal.h>     /* IWYU pragma: export */

#अन्तर्भाव "forward.h"

पूर्णाङ्क reset_all_signal_handlers(शून्य);
पूर्णाङ्क reset_signal_mask(शून्य);

पूर्णाङ्क sigaction_many_internal(ध्रुव संरचना sigaction *sa, ...);

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

पूर्णाङ्क sigset_add_many_internal(sigset_t *ss, ...);
#define sigset_add_many(...) sigset_add_many_internal(__VA_ARGS__, -1)

पूर्णाङ्क sigprocmask_many_internal(पूर्णाङ्क how, sigset_t *ret_old_mask, ...);
#define sigprocmask_many(...) sigprocmask_many_internal(__VA_ARGS__, -1)

DECLARE_STRING_TABLE_LOOKUP(सङ्केत, पूर्णाङ्क);
ध्रुव वर्ण* signal_code_to_string(पूर्णाङ्क signo, पूर्णाङ्क code) _const_;

शून्य nop_signal_handler(पूर्णाङ्क sig);

स्थिर अन्तःस्थ शून्य block_signals_reset(sigset_t **ss) {
        प्रतिज्ञा(ss);

        यदि (!*ss)
                प्रत्यागम;

        assert_log(sigprocmask(SIG_SETMASK, *ss, रिक्त) >= 0);
}

#define BLOCK_SIGNALS(...)                                              \
        sigset_t _saved_sigset;                                         \
        _cleanup_(block_signals_reset) _unused_ sigset_t *_saved_sigsetp = \
                assert_log(sigprocmask_many(SIG_BLOCK, &_saved_sigset, __VA_ARGS__) >= 0) ? \
                &_saved_sigset : रिक्त;

#define SIGNO_INVALID (-EINVAL)

स्थिर अन्तःस्थ bool SIGNAL_VALID(पूर्णाङ्क signo) {
        प्रत्यागम signo > 0 && signo < _NSIG;
}

स्थिर अन्तःस्थ ध्रुव वर्ण* signal_to_string_with_check(पूर्णाङ्क n) {
        यदि (!SIGNAL_VALID(n))
                प्रत्यागम रिक्त;

        प्रत्यागम signal_to_string(n);
}

पूर्णाङ्क signal_is_blocked(पूर्णाङ्क sig);

पूर्णाङ्क autoreaping_enabled(शून्य);

पूर्णाङ्क काढा_pending_signal_internal(पूर्णाङ्क sig, ...);
#define काढा_pending_signal(...) काढा_pending_signal_internal(__VA_ARGS__, -1)

शून्य propagate_signal(पूर्णाङ्क sig, siginfo_t *siginfo);

बाह्य ध्रुव संरचना sigaction sigaction_ignore;
बाह्य ध्रुव संरचना sigaction sigaction_default;
बाह्य ध्रुव संरचना sigaction sigaction_nop_nocldstop;

पूर्णाङ्क parse_signo(ध्रुव वर्ण *s, पूर्णाङ्क *ret);

स्थिर अन्तःस्थ bool si_code_from_process(पूर्णाङ्क si_code) {
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

        प्रत्यागम si_code < 0 || IN_SET(si_code, SI_USER, SI_QUEUE);
}

शून्य sigterm_process_group_handler(पूर्णाङ्क सङ्केत, siginfo_t *info, शून्य *ucontext);
