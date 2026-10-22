/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ <signal.h>     /* IWYU pragma: export */

#அந்தர்பாவ "forward.h"

முழு reset_all_signal_handlers(வெறுமை);
முழு reset_signal_mask(வெறுமை);

முழு sigaction_many_internal(மாறிலி கட்டமைப்பு sigaction *sa, ...);

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

முழு sigset_add_many_internal(sigset_t *ss, ...);
#define sigset_add_many(...) sigset_add_many_internal(__VA_ARGS__, -1)

முழு sigprocmask_many_internal(முழு how, sigset_t *ret_old_mask, ...);
#define sigprocmask_many(...) sigprocmask_many_internal(__VA_ARGS__, -1)

DECLARE_STRING_TABLE_LOOKUP(சமிக்ஞை, முழு);
மாறிலி எழுத்து* signal_code_to_string(முழு signo, முழு code) _const_;

வெறுமை nop_signal_handler(முழு sig);

நிலையான உள்வரி வெறுமை block_signals_reset(sigset_t **ss) {
        ப்ரதிஜ்ஞா(ss);

        எனில் (!*ss)
                திரும்பு;

        assert_log(sigprocmask(SIG_SETMASK, *ss, வெற்று) >= 0);
}

#define BLOCK_SIGNALS(...)                                              \
        sigset_t _saved_sigset;                                         \
        _cleanup_(block_signals_reset) _unused_ sigset_t *_saved_sigsetp = \
                assert_log(sigprocmask_many(SIG_BLOCK, &_saved_sigset, __VA_ARGS__) >= 0) ? \
                &_saved_sigset : வெற்று;

#define SIGNO_INVALID (-EINVAL)

நிலையான உள்வரி bool SIGNAL_VALID(முழு signo) {
        திரும்பு signo > 0 && signo < _NSIG;
}

நிலையான உள்வரி மாறிலி எழுத்து* signal_to_string_with_check(முழு n) {
        எனில் (!SIGNAL_VALID(n))
                திரும்பு வெற்று;

        திரும்பு signal_to_string(n);
}

முழு signal_is_blocked(முழு sig);

முழு autoreaping_enabled(வெறுமை);

முழு எடு_pending_signal_internal(முழு sig, ...);
#define எடு_pending_signal(...) எடு_pending_signal_internal(__VA_ARGS__, -1)

வெறுமை propagate_signal(முழு sig, siginfo_t *siginfo);

வெளி மாறிலி கட்டமைப்பு sigaction sigaction_ignore;
வெளி மாறிலி கட்டமைப்பு sigaction sigaction_default;
வெளி மாறிலி கட்டமைப்பு sigaction sigaction_nop_nocldstop;

முழு parse_signo(மாறிலி எழுத்து *s, முழு *ret);

நிலையான உள்வரி bool si_code_from_process(முழு si_code) {
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

        திரும்பு si_code < 0 || IN_SET(si_code, SI_USER, SI_QUEUE);
}

வெறுமை sigterm_process_group_handler(முழு சமிக்ஞை, siginfo_t *info, வெறுமை *ucontext);
