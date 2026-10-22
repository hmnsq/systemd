/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل <signal.h>     /* IWYU pragma: export */

#شامل "forward.h"

صحیح reset_all_signal_handlers(خالی);
صحیح reset_signal_نقاب(خالی);

صحیح sigaction_many_اندرونی(ثابت ساخت sigaction *sa, ...);

#تعریف_کرو ignore_اشارات(...)                                             \
        sigaction_many_اندرونی(                                        \
                        &sigaction_ignore,                              \
                        __VA_ARGS__,                                    \
                        -1)

#تعریف_کرو default_اشارات(...)                                            \
        sigaction_many_اندرونی(                                        \
                        &sigaction_default,                             \
                        __VA_ARGS__,                                    \
                        -1)

#تعریف_کرو sigaction_many(sa, ...)                                         \
        sigaction_many_اندرونی(sa, __VA_ARGS__, -1)

صحیح sigset_جوڑو_many_اندرونی(sigset_t *ss, ...);
#تعریف_کرو sigset_جوڑو_many(...) sigset_جوڑو_many_اندرونی(__VA_ARGS__, -1)

صحیح sigprocmask_many_اندرونی(صحیح how, sigset_t *ret_old_نقاب, ...);
#تعریف_کرو sigprocmask_many(...) sigprocmask_many_اندرونی(__VA_ARGS__, -1)

DECLARE_STRING_جدول_LOOKUP(اشارہ, صحیح);
ثابت حرف* signal_رمز_مقصد_string(صحیح signo, صحیح رمز) _const_;

خالی nop_signal_handler(صحیح sig);

ساکن اندرونی خالی بلاک_اشارات_reset(sigset_t **ss) {
        أكد(ss);

        اگر (!*ss)
                واپس;

        assert_log(sigprocmask(SIG_SETMASK, *ss, تہی) >= 0);
}

#تعریف_کرو بلاک_اشارات_2(...)                                              \
        sigset_t _saved_sigset;                                         \
        _cleanup_(بلاک_اشارات_reset) غیر_مستعمل sigset_t *_saved_sigsetp = \
                assert_log(sigprocmask_many(SIG_BLOCK, &_saved_sigset, __VA_ARGS__) >= 0) ? \
                &_saved_sigset : تہی;

#تعریف_کرو SIGNO_INVALID (-EINVAL)

ساکن اندرونی bool SIGNAL_VALID(صحیح signo) {
        واپس signo > 0 && signo < _NSIG;
}

ساکن اندرونی ثابت حرف* signal_مقصد_string_with_check(صحیح n) {
        اگر (!SIGNAL_VALID(n))
                واپس تہی;

        واپس signal_مقصد_string(n);
}

صحیح signal_is_blocked(صحیح sig);

صحیح autoreaping_enabled(خالی);

صحیح نکالو_pending_signal_اندرونی(صحیح sig, ...);
#تعریف_کرو نکالو_pending_signal(...) نکالو_pending_signal_اندرونی(__VA_ARGS__, -1)

خالی propagate_signal(صحیح sig, siginfo_t *siginfo);

خارجی ثابت ساخت sigaction sigaction_ignore;
خارجی ثابت ساخت sigaction sigaction_default;
خارجی ثابت ساخت sigaction sigaction_nop_nocldstop;

صحیح parse_signo(ثابت حرف *s, صحیح *ret);

ساکن اندرونی bool si_رمز_from_process(صحیح si_code) {
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

        واپس si_code < 0 || اندر_سیٹ(si_code, SI_USER, SI_QUEUE);
}

خالی sigterm_process_گروہ_handler(صحیح اشارہ, siginfo_t *info, خالی *ucontext);
