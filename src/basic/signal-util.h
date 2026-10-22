/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ <signal.h>     /* IWYU pragma: export */

#ଅନ୍ତର୍ଭାଵ "forward.h"

ପୂର୍ଣ୍ଣସଂଖ୍ୟା reset_all_signal_handlers(ଶୂନ୍ୟ);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା reset_signal_mask(ଶୂନ୍ୟ);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sigaction_many_internal(ଅଚଳ ସଂରଚନା sigaction *sa, ...);

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

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sigset_add_many_internal(sigset_t *ss, ...);
#define sigset_add_many(...) sigset_add_many_internal(__VA_ARGS__, -1)

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sigprocmask_many_internal(ପୂର୍ଣ୍ଣସଂଖ୍ୟା how, sigset_t *ret_old_mask, ...);
#define sigprocmask_many(...) sigprocmask_many_internal(__VA_ARGS__, -1)

DECLARE_STRING_TABLE_LOOKUP(ସଙ୍କେତ, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
ଅଚଳ ଅକ୍ଷର* signal_code_to_string(ପୂର୍ଣ୍ଣସଂଖ୍ୟା signo, ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) _const_;

ଶୂନ୍ୟ nop_signal_handler(ପୂର୍ଣ୍ଣସଂଖ୍ୟା sig);

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ block_signals_reset(sigset_t **ss) {
        ପ୍ରତିଜ୍ଞା(ss);

        ଯଦି (!*ss)
                ଫେରନ୍ତୁ;

        assert_log(sigprocmask(SIG_SETMASK, *ss, ଖାଲି) >= 0);
}

#define BLOCK_SIGNALS(...)                                              \
        sigset_t _saved_sigset;                                         \
        _cleanup_(block_signals_reset) _unused_ sigset_t *_saved_sigsetp = \
                assert_log(sigprocmask_many(SIG_BLOCK, &_saved_sigset, __VA_ARGS__) >= 0) ? \
                &_saved_sigset : ଖାଲି;

#define SIGNO_INVALID (-EINVAL)

ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool SIGNAL_VALID(ପୂର୍ଣ୍ଣସଂଖ୍ୟା signo) {
        ଫେରନ୍ତୁ signo > 0 && signo < _NSIG;
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଅଚଳ ଅକ୍ଷର* signal_to_string_with_check(ପୂର୍ଣ୍ଣସଂଖ୍ୟା n) {
        ଯଦି (!SIGNAL_VALID(n))
                ଫେରନ୍ତୁ ଖାଲି;

        ଫେରନ୍ତୁ signal_to_string(n);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା signal_is_blocked(ପୂର୍ଣ୍ଣସଂଖ୍ୟା sig);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା autoreaping_enabled(ଶୂନ୍ୟ);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା କାଢ଼_pending_signal_internal(ପୂର୍ଣ୍ଣସଂଖ୍ୟା sig, ...);
#define କାଢ଼_pending_signal(...) କାଢ଼_pending_signal_internal(__VA_ARGS__, -1)

ଶୂନ୍ୟ propagate_signal(ପୂର୍ଣ୍ଣସଂଖ୍ୟା sig, siginfo_t *siginfo);

ବାହ୍ୟ ଅଚଳ ସଂରଚନା sigaction sigaction_ignore;
ବାହ୍ୟ ଅଚଳ ସଂରଚନା sigaction sigaction_default;
ବାହ୍ୟ ଅଚଳ ସଂରଚନା sigaction sigaction_nop_nocldstop;

ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_signo(ଅଚଳ ଅକ୍ଷର *s, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ret);

ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool si_code_from_process(ପୂର୍ଣ୍ଣସଂଖ୍ୟା si_code) {
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

        ଫେରନ୍ତୁ si_code < 0 || IN_SET(si_code, SI_USER, SI_QUEUE);
}

ଶୂନ୍ୟ sigterm_process_group_handler(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ସଙ୍କେତ, siginfo_t *info, ଶୂନ୍ୟ *ucontext);
