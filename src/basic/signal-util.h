/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include <signal.h>     /* IWYU pragma: export */

#include "forward.h"

цяло reset_all_signal_handlers(void);
цяло reset_signal_mask(void);

цяло sigaction_many_internal(const struct sigaction *sa, ...);

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

цяло sigset_add_many_internal(sigset_t *ss, ...);
#define sigset_add_many(...) sigset_add_many_internal(__VA_ARGS__, -1)

цяло sigprocmask_many_internal(цяло how, sigset_t *ret_old_mask, ...);
#define sigprocmask_many(...) sigprocmask_many_internal(__VA_ARGS__, -1)

DECLARE_STRING_TABLE_LOOKUP(signal, цяло);
const char* signal_code_to_string(цяло signo, цяло code) _const_;

void nop_signal_handler(цяло sig);

static inline void block_signals_reset(sigset_t **ss) {
        assert(ss);

        ако (!*ss)
                върни;

        assert_log(sigprocmask(SIG_SETMASK, *ss, NULL) >= 0);
}

#define BLOCK_SIGNALS(...)                                              \
        sigset_t _saved_sigset;                                         \
        _cleanup_(block_signals_reset) _unused_ sigset_t *_saved_sigsetp = \
                assert_log(sigprocmask_many(SIG_BLOCK, &_saved_sigset, __VA_ARGS__) >= 0) ? \
                &_saved_sigset : NULL;

#define SIGNO_INVALID (-EINVAL)

static inline bool SIGNAL_VALID(цяло signo) {
        върни signo > 0 && signo < _NSIG;
}

static inline const char* signal_to_string_with_check(цяло n) {
        ако (!SIGNAL_VALID(n))
                върни NULL;

        върни signal_to_string(n);
}

цяло signal_is_blocked(цяло sig);

цяло autoreaping_enabled(void);

цяло pop_pending_signal_internal(цяло sig, ...);
#define pop_pending_signal(...) pop_pending_signal_internal(__VA_ARGS__, -1)

void propagate_signal(цяло sig, siginfo_t *siginfo);

extern const struct sigaction sigaction_ignore;
extern const struct sigaction sigaction_default;
extern const struct sigaction sigaction_nop_nocldstop;

цяло parse_signo(const char *s, цяло *ret);

static inline bool si_code_from_process(цяло si_code) {
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

        върни si_code < 0 || IN_SET(si_code, SI_USER, SI_QUEUE);
}

void sigterm_process_group_handler(цяло signal, siginfo_t *info, void *ucontext);
