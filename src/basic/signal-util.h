/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ <signal.h>     /* IWYU pragma: export */

#അന്തര്ഭാവ "forward.h"

പൂർണ്ണസംഖ്യ reset_all_signal_handlers(ശൂന്യം);
പൂർണ്ണസംഖ്യ reset_signal_mask(ശൂന്യം);

പൂർണ്ണസംഖ്യ sigaction_many_internal(സ്ഥിരം ഘടന sigaction *sa, ...);

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

പൂർണ്ണസംഖ്യ sigset_add_many_internal(sigset_t *ss, ...);
#define sigset_add_many(...) sigset_add_many_internal(__VA_ARGS__, -1)

പൂർണ്ണസംഖ്യ sigprocmask_many_internal(പൂർണ്ണസംഖ്യ how, sigset_t *ret_old_mask, ...);
#define sigprocmask_many(...) sigprocmask_many_internal(__VA_ARGS__, -1)

DECLARE_STRING_TABLE_LOOKUP(സൂചന, പൂർണ്ണസംഖ്യ);
സ്ഥിരം അക്ഷരം* signal_code_to_string(പൂർണ്ണസംഖ്യ signo, പൂർണ്ണസംഖ്യ code) _const_;

ശൂന്യം nop_signal_handler(പൂർണ്ണസംഖ്യ sig);

സ്ഥിതം വരിയിൽ ശൂന്യം block_signals_reset(sigset_t **ss) {
        പ്രതിജ്ഞാ(ss);

        എങ്കിൽ (!*ss)
                തിരികെ;

        assert_log(sigprocmask(SIG_SETMASK, *ss, ശൂന്യ) >= 0);
}

#define BLOCK_SIGNALS(...)                                              \
        sigset_t _saved_sigset;                                         \
        _cleanup_(block_signals_reset) _unused_ sigset_t *_saved_sigsetp = \
                assert_log(sigprocmask_many(SIG_BLOCK, &_saved_sigset, __VA_ARGS__) >= 0) ? \
                &_saved_sigset : ശൂന്യ;

#define SIGNO_INVALID (-EINVAL)

സ്ഥിതം വരിയിൽ bool SIGNAL_VALID(പൂർണ്ണസംഖ്യ signo) {
        തിരികെ signo > 0 && signo < _NSIG;
}

സ്ഥിതം വരിയിൽ സ്ഥിരം അക്ഷരം* signal_to_string_with_check(പൂർണ്ണസംഖ്യ n) {
        എങ്കിൽ (!SIGNAL_VALID(n))
                തിരികെ ശൂന്യ;

        തിരികെ signal_to_string(n);
}

പൂർണ്ണസംഖ്യ signal_is_blocked(പൂർണ്ണസംഖ്യ sig);

പൂർണ്ണസംഖ്യ autoreaping_enabled(ശൂന്യം);

പൂർണ്ണസംഖ്യ എടുക്കുക_pending_signal_internal(പൂർണ്ണസംഖ്യ sig, ...);
#define എടുക്കുക_pending_signal(...) എടുക്കുക_pending_signal_internal(__VA_ARGS__, -1)

ശൂന്യം propagate_signal(പൂർണ്ണസംഖ്യ sig, siginfo_t *siginfo);

ബാഹ്യ സ്ഥിരം ഘടന sigaction sigaction_ignore;
ബാഹ്യ സ്ഥിരം ഘടന sigaction sigaction_default;
ബാഹ്യ സ്ഥിരം ഘടന sigaction sigaction_nop_nocldstop;

പൂർണ്ണസംഖ്യ parse_signo(സ്ഥിരം അക്ഷരം *s, പൂർണ്ണസംഖ്യ *ret);

സ്ഥിതം വരിയിൽ bool si_code_from_process(പൂർണ്ണസംഖ്യ si_code) {
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

        തിരികെ si_code < 0 || IN_SET(si_code, SI_USER, SI_QUEUE);
}

ശൂന്യം sigterm_process_group_handler(പൂർണ്ണസംഖ്യ സൂചന, siginfo_t *info, ശൂന്യം *ucontext);
