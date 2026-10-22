/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ <signal.h>     /* IWYU pragma: export */

#અન્તર્ભાવ "forward.h"

પૂર્ણાંક reset_all_signal_handlers(શૂન્ય);
પૂર્ણાંક reset_signal_mask(શૂન્ય);

પૂર્ણાંક sigaction_many_internal(અચલ સંરચના sigaction *sa, ...);

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

પૂર્ણાંક sigset_add_many_internal(sigset_t *ss, ...);
#define sigset_add_many(...) sigset_add_many_internal(__VA_ARGS__, -1)

પૂર્ણાંક sigprocmask_many_internal(પૂર્ણાંક how, sigset_t *ret_old_mask, ...);
#define sigprocmask_many(...) sigprocmask_many_internal(__VA_ARGS__, -1)

DECLARE_STRING_TABLE_LOOKUP(સંકેત, પૂર્ણાંક);
અચલ અક્ષર* signal_code_to_string(પૂર્ણાંક signo, પૂર્ણાંક code) _const_;

શૂન્ય nop_signal_handler(પૂર્ણાંક sig);

સ્થિર અંતઃરેખ શૂન્ય block_signals_reset(sigset_t **ss) {
        પ્રતિજ્ઞા(ss);

        જો (!*ss)
                પાછા;

        assert_log(sigprocmask(SIG_SETMASK, *ss, ખાલી) >= 0);
}

#define BLOCK_SIGNALS(...)                                              \
        sigset_t _saved_sigset;                                         \
        _cleanup_(block_signals_reset) _unused_ sigset_t *_saved_sigsetp = \
                assert_log(sigprocmask_many(SIG_BLOCK, &_saved_sigset, __VA_ARGS__) >= 0) ? \
                &_saved_sigset : ખાલી;

#define SIGNO_INVALID (-EINVAL)

સ્થિર અંતઃરેખ bool SIGNAL_VALID(પૂર્ણાંક signo) {
        પાછા signo > 0 && signo < _NSIG;
}

સ્થિર અંતઃરેખ અચલ અક્ષર* signal_to_string_with_check(પૂર્ણાંક n) {
        જો (!SIGNAL_VALID(n))
                પાછા ખાલી;

        પાછા signal_to_string(n);
}

પૂર્ણાંક signal_is_blocked(પૂર્ણાંક sig);

પૂર્ણાંક autoreaping_enabled(શૂન્ય);

પૂર્ણાંક કાઢો_pending_signal_internal(પૂર્ણાંક sig, ...);
#define કાઢો_pending_signal(...) કાઢો_pending_signal_internal(__VA_ARGS__, -1)

શૂન્ય propagate_signal(પૂર્ણાંક sig, siginfo_t *siginfo);

બાહ્ય અચલ સંરચના sigaction sigaction_ignore;
બાહ્ય અચલ સંરચના sigaction sigaction_default;
બાહ્ય અચલ સંરચના sigaction sigaction_nop_nocldstop;

પૂર્ણાંક parse_signo(અચલ અક્ષર *s, પૂર્ણાંક *ret);

સ્થિર અંતઃરેખ bool si_code_from_process(પૂર્ણાંક si_code) {
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

        પાછા si_code < 0 || IN_SET(si_code, SI_USER, SI_QUEUE);
}

શૂન્ય sigterm_process_group_handler(પૂર્ણાંક સંકેત, siginfo_t *info, શૂન્ય *ucontext);
