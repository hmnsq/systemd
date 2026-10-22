/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ <signal.h>     /* IWYU pragma: export */

#అన్తర్భావ "forward.h"

పూర్ణాంకం reset_all_signal_handlers(శూన్య);
పూర్ణాంకం reset_signal_ముసుగు(శూన్య);

పూర్ణాంకం sigaction_many_అంతర్గత(స్థిరరాశి నిర్మాణం sigaction *sa, ...);

#నిర్వచించు ignore_సంకేతాలు(...)                                             \
        sigaction_many_అంతర్గత(                                        \
                        &sigaction_ignore,                              \
                        __VA_ARGS__,                                    \
                        -1)

#నిర్వచించు default_సంకేతాలు(...)                                            \
        sigaction_many_అంతర్గత(                                        \
                        &sigaction_default,                             \
                        __VA_ARGS__,                                    \
                        -1)

#నిర్వచించు sigaction_many(sa, ...)                                         \
        sigaction_many_అంతర్గత(sa, __VA_ARGS__, -1)

పూర్ణాంకం sigset_కూడు_many_అంతర్గత(sigset_t *ss, ...);
#నిర్వచించు sigset_కూడు_many(...) sigset_కూడు_many_అంతర్గత(__VA_ARGS__, -1)

పూర్ణాంకం sigprocmask_many_అంతర్గత(పూర్ణాంకం how, sigset_t *ret_old_ముసుగు, ...);
#నిర్వచించు sigprocmask_many(...) sigprocmask_many_అంతర్గత(__VA_ARGS__, -1)

DECLARE_STRING_పట్టిక_LOOKUP(సంకేతం, పూర్ణాంకం);
స్థిరరాశి అక్షరం* signal_సంకేతం_గమ్యం_string(పూర్ణాంకం signo, పూర్ణాంకం సంకేతం_2) _const_;

శూన్య nop_signal_handler(పూర్ణాంకం sig);

స్థిర అంతఃవరుస శూన్య ఖండం_సంకేతాలు_reset(sigset_t **ss) {
        ప్రతిజ్ఞా(ss);

        ఒకవేళ (!*ss)
                తిరిగి;

        assert_log(sigprocmask(SIG_SETMASK, *ss, ఖాళీ) >= 0);
}

#నిర్వచించు ఖండం_సంకేతాలు_2(...)                                              \
        sigset_t _saved_sigset;                                         \
        _cleanup_(ఖండం_సంకేతాలు_reset) వాడని sigset_t *_saved_sigsetp = \
                assert_log(sigprocmask_many(SIG_BLOCK, &_saved_sigset, __VA_ARGS__) >= 0) ? \
                &_saved_sigset : ఖాళీ;

#నిర్వచించు SIGNO_INVALID (-EINVAL)

స్థిర అంతఃవరుస bool SIGNAL_VALID(పూర్ణాంకం signo) {
        తిరిగి signo > 0 && signo < _NSIG;
}

స్థిర అంతఃవరుస స్థిరరాశి అక్షరం* signal_గమ్యం_string_with_check(పూర్ణాంకం n) {
        ఒకవేళ (!SIGNAL_VALID(n))
                తిరిగి ఖాళీ;

        తిరిగి signal_గమ్యం_string(n);
}

పూర్ణాంకం signal_is_blocked(పూర్ణాంకం sig);

పూర్ణాంకం autoreaping_enabled(శూన్య);

పూర్ణాంకం తీయి_pending_signal_అంతర్గత(పూర్ణాంకం sig, ...);
#నిర్వచించు తీయి_pending_signal(...) తీయి_pending_signal_అంతర్గత(__VA_ARGS__, -1)

శూన్య propagate_signal(పూర్ణాంకం sig, siginfo_t *siginfo);

బాహ్య స్థిరరాశి నిర్మాణం sigaction sigaction_ignore;
బాహ్య స్థిరరాశి నిర్మాణం sigaction sigaction_default;
బాహ్య స్థిరరాశి నిర్మాణం sigaction sigaction_nop_nocldstop;

పూర్ణాంకం parse_signo(స్థిరరాశి అక్షరం *s, పూర్ణాంకం *ret);

స్థిర అంతఃవరుస bool si_సంకేతం_from_process(పూర్ణాంకం si_code) {
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

        తిరిగి si_code < 0 || లోపల_సమితి(si_code, SI_USER, SI_QUEUE);
}

శూన్య sigterm_process_సమూహం_handler(పూర్ణాంకం సంకేతం, siginfo_t *info, శూన్య *ucontext);
