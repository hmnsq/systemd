/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব <signal.h>     /* IWYU pragma: export */

#অন্তর্ভাব "forward.h"

পূর্ণসংখ্যা reset_all_signal_handlers(শূন্য);
পূর্ণসংখ্যা reset_signal_mask(শূন্য);

পূর্ণসংখ্যা sigaction_many_internal(ধ্রুবক কাঠামো sigaction *sa, ...);

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

পূর্ণসংখ্যা sigset_add_many_internal(sigset_t *ss, ...);
#define sigset_add_many(...) sigset_add_many_internal(__VA_ARGS__, -1)

পূর্ণসংখ্যা sigprocmask_many_internal(পূর্ণসংখ্যা how, sigset_t *ret_old_mask, ...);
#define sigprocmask_many(...) sigprocmask_many_internal(__VA_ARGS__, -1)

DECLARE_STRING_TABLE_LOOKUP(সংকেত, পূর্ণসংখ্যা);
ধ্রুবক অক্ষর* signal_code_to_string(পূর্ণসংখ্যা signo, পূর্ণসংখ্যা code) _const_;

শূন্য nop_signal_handler(পূর্ণসংখ্যা sig);

স্থির অন্তঃরেখ শূন্য block_signals_reset(sigset_t **ss) {
        প্রতিজ্ঞা(ss);

        যদি (!*ss)
                ফেরত;

        assert_log(sigprocmask(SIG_SETMASK, *ss, ফাঁকা) >= 0);
}

#define BLOCK_SIGNALS(...)                                              \
        sigset_t _saved_sigset;                                         \
        _cleanup_(block_signals_reset) _unused_ sigset_t *_saved_sigsetp = \
                assert_log(sigprocmask_many(SIG_BLOCK, &_saved_sigset, __VA_ARGS__) >= 0) ? \
                &_saved_sigset : ফাঁকা;

#define SIGNO_INVALID (-EINVAL)

স্থির অন্তঃরেখ bool SIGNAL_VALID(পূর্ণসংখ্যা signo) {
        ফেরত signo > 0 && signo < _NSIG;
}

স্থির অন্তঃরেখ ধ্রুবক অক্ষর* signal_to_string_with_check(পূর্ণসংখ্যা n) {
        যদি (!SIGNAL_VALID(n))
                ফেরত ফাঁকা;

        ফেরত signal_to_string(n);
}

পূর্ণসংখ্যা signal_is_blocked(পূর্ণসংখ্যা sig);

পূর্ণসংখ্যা autoreaping_enabled(শূন্য);

পূর্ণসংখ্যা বের_করো_pending_signal_internal(পূর্ণসংখ্যা sig, ...);
#define বের_করো_pending_signal(...) বের_করো_pending_signal_internal(__VA_ARGS__, -1)

শূন্য propagate_signal(পূর্ণসংখ্যা sig, siginfo_t *siginfo);

বাহ্যিক ধ্রুবক কাঠামো sigaction sigaction_ignore;
বাহ্যিক ধ্রুবক কাঠামো sigaction sigaction_default;
বাহ্যিক ধ্রুবক কাঠামো sigaction sigaction_nop_nocldstop;

পূর্ণসংখ্যা parse_signo(ধ্রুবক অক্ষর *s, পূর্ণসংখ্যা *ret);

স্থির অন্তঃরেখ bool si_code_from_process(পূর্ণসংখ্যা si_code) {
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

        ফেরত si_code < 0 || IN_SET(si_code, SI_USER, SI_QUEUE);
}

শূন্য sigterm_process_group_handler(পূর্ণসংখ্যা সংকেত, siginfo_t *info, শূন্য *ucontext);
