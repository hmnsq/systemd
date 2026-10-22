/* SPDX-License-Identifier: LGPL-2.1-or-later */
#رهنمود once

#ضمن <signal.h>     /* IWYU pragma: export */

#ضمن "forward.h"

صحيح reset_all_signal_handlers(فراغ);
صحيح reset_signal_ماسک(فراغ);

صحيح sigaction_many_داخلی(ثابت بنية sigaction *sa, ...);

#تعریف_کن ignore_سیگنالها(...)                                             \
        sigaction_many_داخلی(                                        \
                        &sigaction_ignore,                              \
                        __VA_ARGS__,                                    \
                        -1)

#تعریف_کن default_سیگنالها(...)                                            \
        sigaction_many_داخلی(                                        \
                        &sigaction_default,                             \
                        __VA_ARGS__,                                    \
                        -1)

#تعریف_کن sigaction_many(sa, ...)                                         \
        sigaction_many_داخلی(sa, __VA_ARGS__, -1)

صحيح sigset_جمع_کن_many_داخلی(sigset_t *ss, ...);
#تعریف_کن sigset_جمع_کن_many(...) sigset_جمع_کن_many_داخلی(__VA_ARGS__, -1)

صحيح sigprocmask_many_داخلی(صحيح how, sigset_t *ret_old_ماسک, ...);
#تعریف_کن sigprocmask_many(...) sigprocmask_many_داخلی(__VA_ARGS__, -1)

DECLARE_STRING_جدول_LOOKUP(إشارة, صحيح);
ثابت حرف* signal_کد_مقصد_string(صحيح signo, صحيح کد) _const_;

فراغ nop_signal_handler(صحيح sig);

ساكن مضمن فراغ بلوک_سیگنالها_reset(sigset_t **ss) {
        أكد(ss);

        إذا (!*ss)
                أرجع;

        assert_log(sigprocmask(SIG_SETMASK, *ss, عدم) >= 0);
}

#تعریف_کن بلوک_سیگنالها_2(...)                                              \
        sigset_t _saved_sigset;                                         \
        _cleanup_(بلوک_سیگنالها_reset) استفادهنشده sigset_t *_saved_sigsetp = \
                assert_log(sigprocmask_many(SIG_BLOCK, &_saved_sigset, __VA_ARGS__) >= 0) ? \
                &_saved_sigset : عدم;

#تعریف_کن SIGNO_INVALID (-EINVAL)

ساكن مضمن bool SIGNAL_VALID(صحيح signo) {
        أرجع signo > 0 && signo < _NSIG;
}

ساكن مضمن ثابت حرف* signal_مقصد_string_with_check(صحيح n) {
        إذا (!SIGNAL_VALID(n))
                أرجع عدم;

        أرجع signal_مقصد_string(n);
}

صحيح signal_is_blocked(صحيح sig);

صحيح autoreaping_enabled(فراغ);

صحيح برداشت_pending_signal_داخلی(صحيح sig, ...);
#تعریف_کن برداشت_pending_signal(...) برداشت_pending_signal_داخلی(__VA_ARGS__, -1)

فراغ propagate_signal(صحيح sig, siginfo_t *siginfo);

خارجي ثابت بنية sigaction sigaction_ignore;
خارجي ثابت بنية sigaction sigaction_default;
خارجي ثابت بنية sigaction sigaction_nop_nocldstop;

صحيح parse_signo(ثابت حرف *s, صحيح *ret);

ساكن مضمن bool si_کد_from_process(صحيح si_code) {
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

        أرجع si_code < 0 || درون_مجموعه(si_code, SI_USER, SI_QUEUE);
}

فراغ sigterm_process_گروه_handler(صحيح إشارة, siginfo_t *info, فراغ *ucontext);
