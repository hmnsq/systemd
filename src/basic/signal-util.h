/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#شامل <signal.h>     /* IWYU pragma: export */

#شامل "forward.h"

صحیح reset_all_signal_handlers(تهی);
صحیح reset_signal_mask(تهی);

صحیح sigaction_many_internal(ثابت ساختار sigaction *sa, ...);

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

صحیح sigset_add_many_internal(sigset_t *ss, ...);
#define sigset_add_many(...) sigset_add_many_internal(__VA_ARGS__, -1)

صحیح sigprocmask_many_internal(صحیح how, sigset_t *ret_old_mask, ...);
#define sigprocmask_many(...) sigprocmask_many_internal(__VA_ARGS__, -1)

DECLARE_STRING_TABLE_LOOKUP(سیگنال, صحیح);
ثابت نویسه* signal_code_to_string(صحیح signo, صحیح code) _const_;

تهی nop_signal_handler(صحیح sig);

ایستا درونخطی تهی block_signals_reset(sigset_t **ss) {
        أكد(ss);

        اگر (!*ss)
                بازگشت;

        assert_log(sigprocmask(SIG_SETMASK, *ss, پوچ) >= 0);
}

#define BLOCK_SIGNALS(...)                                              \
        sigset_t _saved_sigset;                                         \
        _cleanup_(block_signals_reset) _unused_ sigset_t *_saved_sigsetp = \
                assert_log(sigprocmask_many(SIG_BLOCK, &_saved_sigset, __VA_ARGS__) >= 0) ? \
                &_saved_sigset : پوچ;

#define SIGNO_INVALID (-EINVAL)

ایستا درونخطی bool SIGNAL_VALID(صحیح signo) {
        بازگشت signo > 0 && signo < _NSIG;
}

ایستا درونخطی ثابت نویسه* signal_to_string_with_check(صحیح n) {
        اگر (!SIGNAL_VALID(n))
                بازگشت پوچ;

        بازگشت signal_to_string(n);
}

صحیح signal_is_blocked(صحیح sig);

صحیح autoreaping_enabled(تهی);

صحیح برداشت_pending_signal_internal(صحیح sig, ...);
#define برداشت_pending_signal(...) برداشت_pending_signal_internal(__VA_ARGS__, -1)

تهی propagate_signal(صحیح sig, siginfo_t *siginfo);

بیرونی ثابت ساختار sigaction sigaction_ignore;
بیرونی ثابت ساختار sigaction sigaction_default;
بیرونی ثابت ساختار sigaction sigaction_nop_nocldstop;

صحیح parse_signo(ثابت نویسه *s, صحیح *ret);

ایستا درونخطی bool si_code_from_process(صحیح si_code) {
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

        بازگشت si_code < 0 || IN_SET(si_code, SI_USER, SI_QUEUE);
}

تهی sigterm_process_group_handler(صحیح سیگنال, siginfo_t *info, تهی *ucontext);
