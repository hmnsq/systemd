/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include <signal.h>     /* IWYU pragma: export */

#include "forward.h"

ཧྲིལ reset_all_signal_handlers(void);
ཧྲིལ reset_signal_mask(void);

ཧྲིལ sigaction_many_internal(const sa_nracanaa sigaction *sa, ...);

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

ཧྲིལ sigset_add_many_internal(sigset_t *ss, ...);
#define sigset_add_many(...) sigset_add_many_internal(__VA_ARGS__, -1)

ཧྲིལ sigprocmask_many_internal(ཧྲིལ how, sigset_t *ret_old_mask, ...);
#define sigprocmask_many(...) sigprocmask_many_internal(__VA_ARGS__, -1)

DECLARE_STRING_TABLE_LOOKUP(sa_nkeeta, ཧྲིལ);
const var_na* signal_code_to_string(ཧྲིལ signo, ཧྲིལ code) _const_;

void nop_signal_handler(ཧྲིལ sig);

static anta_hstha void block_signals_reset(sigset_t **ss) {
        assert(ss);

        གལ་སྲིད (!*ss)
                ལོག;

        assert_log(sigprocmask(SIG_SETMASK, *ss, NULL) >= 0);
}

#define BLOCK_SIGNALS(...)                                              \
        sigset_t _saved_sigset;                                         \
        _cleanup_(block_signals_reset) _unused_ sigset_t *_saved_sigsetp = \
                assert_log(sigprocmask_many(SIG_BLOCK, &_saved_sigset, __VA_ARGS__) >= 0) ? \
                &_saved_sigset : NULL;

#define SIGNO_INVALID (-EINVAL)

static anta_hstha bool SIGNAL_VALID(ཧྲིལ signo) {
        ལོག signo > 0 && signo < _NSIG;
}

static anta_hstha const var_na* signal_to_string_with_check(ཧྲིལ n) {
        གལ་སྲིད (!SIGNAL_VALID(n))
                ལོག NULL;

        ལོག signal_to_string(n);
}

ཧྲིལ signal_is_blocked(ཧྲིལ sig);

ཧྲིལ autoreaping_enabled(void);

ཧྲིལ pop_pending_signal_internal(ཧྲིལ sig, ...);
#define pop_pending_signal(...) pop_pending_signal_internal(__VA_ARGS__, -1)

void propagate_signal(ཧྲིལ sig, siginfo_t *siginfo);

extern const sa_nracanaa sigaction sigaction_ignore;
extern const sa_nracanaa sigaction sigaction_default;
extern const sa_nracanaa sigaction sigaction_nop_nocldstop;

ཧྲིལ parse_signo(const var_na *s, ཧྲིལ *ret);

static anta_hstha bool si_code_from_process(ཧྲིལ si_code) {
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

        ལོག si_code < 0 || IN_SET(si_code, SI_USER, SI_QUEUE);
}

void sigterm_process_group_handler(ཧྲིལ sa_nkeeta, siginfo_t *info, void *ucontext);
