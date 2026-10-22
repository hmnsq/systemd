/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#ضمن <signal.h>     /* IWYU pragma: export */

#ضمن "forward.h"

صحيح reset_all_signal_handlers(فراغ);
صحيح reset_signal_نقاب(فراغ);

صحيح sigaction_many_اندرونی(ثابت بنية sigaction *sa, ...);

#تعریف_کرو ignore_اشارے(...)                                             \
        sigaction_many_اندرونی(                                        \
                        &sigaction_ignore,                              \
                        __VA_ARGS__,                                    \
                        -1)

#تعریف_کرو default_اشارے(...)                                            \
        sigaction_many_اندرونی(                                        \
                        &sigaction_default,                             \
                        __VA_ARGS__,                                    \
                        -1)

#تعریف_کرو sigaction_many(sa, ...)                                         \
        sigaction_many_اندرونی(sa, __VA_ARGS__, -1)

صحيح sigset_جوڑو_many_اندرونی(sigset_t *ss, ...);
#تعریف_کرو sigset_جوڑو_many(...) sigset_جوڑو_many_اندرونی(__VA_ARGS__, -1)

صحيح sigprocmask_many_اندرونی(صحيح how, sigset_t *ret_old_نقاب, ...);
#تعریف_کرو sigprocmask_many(...) sigprocmask_many_اندرونی(__VA_ARGS__, -1)

DECLARE_STRING_جدول_LOOKUP(إشارة, صحيح);
ثابت حرف* signal_کوڈ_منزل_string(صحيح signo, صحيح کوڈ) _const_;

فراغ nop_signal_handler(صحيح sig);

ساكن مضمن فراغ بلاک_اشارے_reset(sigset_t **ss) {
        أكد(ss);

        إذا (!*ss)
                أرجع;

        assert_log(sigprocmask(SIG_SETMASK, *ss, عدم) >= 0);
}

#تعریف_کرو بلاک_اشارے_2(...)                                              \
        sigset_t _saved_sigset;                                         \
        _cleanup_(بلاک_اشارے_reset) بن_ورتیا sigset_t *_saved_sigsetp = \
                assert_log(sigprocmask_many(SIG_BLOCK, &_saved_sigset, __VA_ARGS__) >= 0) ? \
                &_saved_sigset : عدم;

#تعریف_کرو SIGNO_INVALID (-EINVAL)

ساكن مضمن bool SIGNAL_VALID(صحيح signo) {
        أرجع signo > 0 && signo < _NSIG;
}

ساكن مضمن ثابت حرف* signal_منزل_string_with_check(صحيح n) {
        إذا (!SIGNAL_VALID(n))
                أرجع عدم;

        أرجع signal_منزل_string(n);
}

صحيح signal_is_blocked(صحيح sig);

صحيح autoreaping_enabled(فراغ);

صحيح نکالو_pending_signal_اندرونی(صحيح sig, ...);
#تعریف_کرو نکالو_pending_signal(...) نکالو_pending_signal_اندرونی(__VA_ARGS__, -1)

فراغ propagate_signal(صحيح sig, siginfo_t *siginfo);

خارجي ثابت بنية sigaction sigaction_ignore;
خارجي ثابت بنية sigaction sigaction_default;
خارجي ثابت بنية sigaction sigaction_nop_nocldstop;

صحيح parse_signo(ثابت حرف *s, صحيح *ret);

ساكن مضمن bool si_کوڈ_from_process(صحيح si_code) {
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

        أرجع si_code < 0 || وچ_سیٹ(si_code, SI_USER, SI_QUEUE);
}

فراغ sigterm_process_گروہ_handler(صحيح إشارة, siginfo_t *info, فراغ *ucontext);
