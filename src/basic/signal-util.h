/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ضمن <signal.h>     /* IWYU pragma: export */

#ضمن "forward.h"

صحيح reset_all_signal_handlers(فراغ);
صحيح reset_signal_mask(فراغ);

صحيح sigaction_many_internal(ثابت بنية sigaction *sa, ...);

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

صحيح sigset_add_many_internal(sigset_t *ss, ...);
#define sigset_add_many(...) sigset_add_many_internal(__VA_ARGS__, -1)

صحيح sigprocmask_many_internal(صحيح how, sigset_t *ret_old_mask, ...);
#define sigprocmask_many(...) sigprocmask_many_internal(__VA_ARGS__, -1)

DECLARE_STRING_TABLE_LOOKUP(إشارة, صحيح);
ثابت حرف* signal_code_to_string(صحيح signo, صحيح code) _const_;

فراغ nop_signal_handler(صحيح sig);

ساكن مضمن فراغ block_signals_reset(sigset_t **ss) {
        أكد(ss);

        إذا (!*ss)
                أرجع;

        assert_log(sigprocmask(SIG_SETMASK, *ss, عدم) >= 0);
}

#define BLOCK_SIGNALS(...)                                              \
        sigset_t _saved_sigset;                                         \
        _cleanup_(block_signals_reset) _unused_ sigset_t *_saved_sigsetp = \
                assert_log(sigprocmask_many(SIG_BLOCK, &_saved_sigset, __VA_ARGS__) >= 0) ? \
                &_saved_sigset : عدم;

#define SIGNO_INVALID (-EINVAL)

ساكن مضمن bool SIGNAL_VALID(صحيح signo) {
        أرجع signo > 0 && signo < _NSIG;
}

ساكن مضمن ثابت حرف* signal_to_string_with_check(صحيح n) {
        إذا (!SIGNAL_VALID(n))
                أرجع عدم;

        أرجع signal_to_string(n);
}

صحيح signal_is_blocked(صحيح sig);

صحيح autoreaping_enabled(فراغ);

صحيح دەرهێنان_pending_signal_internal(صحيح sig, ...);
#define دەرهێنان_pending_signal(...) دەرهێنان_pending_signal_internal(__VA_ARGS__, -1)

فراغ propagate_signal(صحيح sig, siginfo_t *siginfo);

خارجي ثابت بنية sigaction sigaction_ignore;
خارجي ثابت بنية sigaction sigaction_default;
خارجي ثابت بنية sigaction sigaction_nop_nocldstop;

صحيح parse_signo(ثابت حرف *s, صحيح *ret);

ساكن مضمن bool si_code_from_process(صحيح si_code) {
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

        أرجع si_code < 0 || IN_SET(si_code, SI_USER, SI_QUEUE);
}

فراغ sigterm_process_group_handler(صحيح إشارة, siginfo_t *info, فراغ *ucontext);
