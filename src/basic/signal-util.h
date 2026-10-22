/* SPDX-License-Identifier: LGPL-2.1-or-later */
#هدايت once

#ضمن <signal.h>     /* IWYU pragma: export */

#ضمن "forward.h"

صحيح reset_all_signal_handlers(فراغ);
صحيح reset_signal_نقاب(فراغ);

صحيح sigaction_many_اندروني(ثابت بنية sigaction *sa, ...);

#وصف_ڪريو ignore_سگنل(...)                                             \
        sigaction_many_اندروني(                                        \
                        &sigaction_ignore,                              \
                        __VA_ARGS__,                                    \
                        -1)

#وصف_ڪريو default_سگنل(...)                                            \
        sigaction_many_اندروني(                                        \
                        &sigaction_default,                             \
                        __VA_ARGS__,                                    \
                        -1)

#وصف_ڪريو sigaction_many(sa, ...)                                         \
        sigaction_many_اندروني(sa, __VA_ARGS__, -1)

صحيح sigset_جوڙيو_many_اندروني(sigset_t *ss, ...);
#وصف_ڪريو sigset_جوڙيو_many(...) sigset_جوڙيو_many_اندروني(__VA_ARGS__, -1)

صحيح sigprocmask_many_اندروني(صحيح how, sigset_t *ret_old_نقاب, ...);
#وصف_ڪريو sigprocmask_many(...) sigprocmask_many_اندروني(__VA_ARGS__, -1)

DECLARE_STRING_جدول_LOOKUP(إشارة, صحيح);
ثابت حرف* signal_ڪوڊ_منزل_string(صحيح signo, صحيح ڪوڊ) _const_;

فراغ nop_signal_handler(صحيح sig);

ساكن مضمن فراغ بلاڪ_سگنل_reset(sigset_t **ss) {
        أكد(ss);

        إذا (!*ss)
                أرجع;

        assert_log(sigprocmask(SIG_SETMASK, *ss, عدم) >= 0);
}

#وصف_ڪريو بلاڪ_سگنل_2(...)                                              \
        sigset_t _saved_sigset;                                         \
        _cleanup_(بلاڪ_سگنل_reset) اڻ_استعمال_ٿيل sigset_t *_saved_sigsetp = \
                assert_log(sigprocmask_many(SIG_BLOCK, &_saved_sigset, __VA_ARGS__) >= 0) ? \
                &_saved_sigset : عدم;

#وصف_ڪريو SIGNO_INVALID (-EINVAL)

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

صحيح ڪڍو_pending_signal_اندروني(صحيح sig, ...);
#وصف_ڪريو ڪڍو_pending_signal(...) ڪڍو_pending_signal_اندروني(__VA_ARGS__, -1)

فراغ propagate_signal(صحيح sig, siginfo_t *siginfo);

خارجي ثابت بنية sigaction sigaction_ignore;
خارجي ثابت بنية sigaction sigaction_default;
خارجي ثابت بنية sigaction sigaction_nop_nocldstop;

صحيح parse_signo(ثابت حرف *s, صحيح *ret);

ساكن مضمن bool si_ڪوڊ_from_process(صحيح si_code) {
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

        أرجع si_code < 0 || اندريون_سيٽ(si_code, SI_USER, SI_QUEUE);
}

فراغ sigterm_process_گروهه_handler(صحيح إشارة, siginfo_t *info, فراغ *ucontext);
