/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ <signal.h>     /* IWYU pragma: export */

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

ਪੂਰਨ reset_all_signal_handlers(ਖਾਲੀ);
ਪੂਰਨ reset_signal_ਨਕਾਬ(ਖਾਲੀ);

ਪੂਰਨ sigaction_many_ਅੰਦਰੂਨੀ(ਅਚਲ ਬਣਤਰ sigaction *sa, ...);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ignore_ਸੰਕੇਤ(...)                                             \
        sigaction_many_ਅੰਦਰੂਨੀ(                                        \
                        &sigaction_ignore,                              \
                        __VA_ARGS__,                                    \
                        -1)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ default_ਸੰਕੇਤ(...)                                            \
        sigaction_many_ਅੰਦਰੂਨੀ(                                        \
                        &sigaction_default,                             \
                        __VA_ARGS__,                                    \
                        -1)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ sigaction_many(sa, ...)                                         \
        sigaction_many_ਅੰਦਰੂਨੀ(sa, __VA_ARGS__, -1)

ਪੂਰਨ sigset_ਜੋੜੋ_many_ਅੰਦਰੂਨੀ(sigset_t *ss, ...);
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ sigset_ਜੋੜੋ_many(...) sigset_ਜੋੜੋ_many_ਅੰਦਰੂਨੀ(__VA_ARGS__, -1)

ਪੂਰਨ sigprocmask_many_ਅੰਦਰੂਨੀ(ਪੂਰਨ how, sigset_t *ret_old_ਨਕਾਬ, ...);
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ sigprocmask_many(...) sigprocmask_many_ਅੰਦਰੂਨੀ(__VA_ARGS__, -1)

DECLARE_STRING_ਸਾਰਣੀ_LOOKUP(ਸੰਕੇਤ, ਪੂਰਨ);
ਅਚਲ ਅੱਖਰ* signal_ਕੋਡ_ਮੰਜ਼ਿਲ_string(ਪੂਰਨ signo, ਪੂਰਨ ਕੋਡ) _const_;

ਖਾਲੀ nop_signal_handler(ਪੂਰਨ sig);

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ ਖੰਡ_ਸੰਕੇਤ_reset(sigset_t **ss) {
        ਪ੍ਰਤਿਜ੍ਞਾ(ss);

        ਜੇ (!*ss)
                ਵਾਪਸ;

        assert_log(sigprocmask(SIG_SETMASK, *ss, ਨਿਰਾ) >= 0);
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਖੰਡ_ਸੰਕੇਤ_2(...)                                              \
        sigset_t _saved_sigset;                                         \
        _cleanup_(ਖੰਡ_ਸੰਕੇਤ_reset) ਅਣਵਰਤਿਆ sigset_t *_saved_sigsetp = \
                assert_log(sigprocmask_many(SIG_BLOCK, &_saved_sigset, __VA_ARGS__) >= 0) ? \
                &_saved_sigset : ਨਿਰਾ;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIGNO_INVALID (-EINVAL)

ਸਥਿਰ ਸਤਰਅੰਦਰ bool SIGNAL_VALID(ਪੂਰਨ signo) {
        ਵਾਪਸ signo > 0 && signo < _NSIG;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਅਚਲ ਅੱਖਰ* signal_ਮੰਜ਼ਿਲ_string_with_check(ਪੂਰਨ n) {
        ਜੇ (!SIGNAL_VALID(n))
                ਵਾਪਸ ਨਿਰਾ;

        ਵਾਪਸ signal_ਮੰਜ਼ਿਲ_string(n);
}

ਪੂਰਨ signal_is_blocked(ਪੂਰਨ sig);

ਪੂਰਨ autoreaping_enabled(ਖਾਲੀ);

ਪੂਰਨ ਕੱਢੋ_pending_signal_ਅੰਦਰੂਨੀ(ਪੂਰਨ sig, ...);
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਕੱਢੋ_pending_signal(...) ਕੱਢੋ_pending_signal_ਅੰਦਰੂਨੀ(__VA_ARGS__, -1)

ਖਾਲੀ propagate_signal(ਪੂਰਨ sig, siginfo_t *siginfo);

ਬਾਹਰੀ ਅਚਲ ਬਣਤਰ sigaction sigaction_ignore;
ਬਾਹਰੀ ਅਚਲ ਬਣਤਰ sigaction sigaction_default;
ਬਾਹਰੀ ਅਚਲ ਬਣਤਰ sigaction sigaction_nop_nocldstop;

ਪੂਰਨ parse_signo(ਅਚਲ ਅੱਖਰ *s, ਪੂਰਨ *ret);

ਸਥਿਰ ਸਤਰਅੰਦਰ bool si_ਕੋਡ_from_process(ਪੂਰਨ si_code) {
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

        ਵਾਪਸ si_code < 0 || ਵਿੱਚ_ਸੈੱਟ(si_code, SI_USER, SI_QUEUE);
}

ਖਾਲੀ sigterm_process_ਸਮੂਹ_handler(ਪੂਰਨ ਸੰਕੇਤ, siginfo_t *info, ਖਾਲੀ *ucontext);
