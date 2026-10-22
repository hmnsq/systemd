/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ <threads.h>
#అన్తర్భావ <unistd.h>

#అన్తర్భావ "errno-util.h"
#అన్తర్భావ "parse-util.h"
#అన్తర్భావ "process-util.h"
#అన్తర్భావ "signal-util.h"
#అన్తర్భావ "stdio-util.h"
#అన్తర్భావ "string-table.h"
#అన్తర్భావ "string-util.h"

పూర్ణాంకం reset_all_signal_handlers(శూన్య) {
        పూర్ణాంకం ret = 0, r;

        కొరకు (పూర్ణాంకం sig = 1; sig < _NSIG; sig++) {

                /* These two cannot be caught... */
                ఒకవేళ (లోపల_సమితి(sig, SIGKILL, SIGSTOP))
                        కొనసాగు;

                /* On Linux the first two RT signals are reserved by glibc, and sigaction() will return
                 * EINVAL for them. */
                r = RET_NERRNO(sigaction(sig, &sigaction_default, ఖాళీ));
                ఒకవేళ (r != -EINVAL)
                        RET_GATHER(ret, r);
        }

        తిరిగి ret;
}

పూర్ణాంకం reset_signal_ముసుగు(శూన్య) {
        sigset_t ss;

        ఒకవేళ (sigemptyset(&ss) < 0)
                తిరిగి -errno;

        తిరిగి RET_NERRNO(sigprocmask(SIG_SETMASK, &ss, ఖాళీ));
}

పూర్ణాంకం sigaction_many_అంతర్గత(స్థిరరాశి నిర్మాణం sigaction *sa, ...) {
        పూర్ణాంకం sig, r = 0;
        va_list ap;

        va_start(ap, sa);

        /* negative signal ends the list. 0 signal is skipped. */
        అయితే ((sig = va_arg(ap, పూర్ణాంకం)) >= 0) {

                ఒకవేళ (sig == 0)
                        కొనసాగు;

                RET_GATHER(r, RET_NERRNO(sigaction(sig, sa, ఖాళీ)));
        }

        va_end(ap);

        తిరిగి r;
}

స్థిర పూర్ణాంకం sigset_కూడు_many_ap(sigset_t *ss, va_list ap) {
        పూర్ణాంకం sig, r = 0;

        ప్రతిజ్ఞా(ss);

        అయితే ((sig = va_arg(ap, పూర్ణాంకం)) >= 0) {

                ఒకవేళ (sig == 0)
                        కొనసాగు;

                ఒకవేళ (sigaddset(ss, sig) < 0) {
                        ఒకవేళ (r >= 0)
                                r = -errno;
                }
        }

        తిరిగి r;
}

పూర్ణాంకం sigset_కూడు_many_అంతర్గత(sigset_t *ss, ...) {
        va_list ap;
        పూర్ణాంకం r;

        va_start(ap, ss);
        r = sigset_కూడు_many_ap(ss, ap);
        va_end(ap);

        తిరిగి r;
}

పూర్ణాంకం sigprocmask_many_అంతర్గత(పూర్ణాంకం how, sigset_t *ret_old_ముసుగు, ...) {
        va_list ap;
        sigset_t ss;
        పూర్ణాంకం r;

        ఒకవేళ (sigemptyset(&ss) < 0)
                తిరిగి -errno;

        va_start(ap, ret_old_ముసుగు);
        r = sigset_కూడు_many_ap(&ss, ap);
        va_end(ap);

        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి RET_NERRNO(sigprocmask(how, &ss, ret_old_ముసుగు));
}

స్థిర స్థిరరాశి అక్షరం *స్థిరరాశి static_signal_పట్టిక[] = {
        [SIGHUP]    = "HUP",
        [SIGINT]    = "INT",
        [SIGQUIT]   = "QUIT",
        [SIGILL]    = "ILL",
        [SIGTRAP]   = "TRAP",
        [SIGABRT]   = "ABRT",
        [SIGBUS]    = "BUS",
        [SIGFPE]    = "FPE",
        [SIGKILL]   = "KILL",
        [SIGUSR1]   = "USR1",
        [SIGSEGV]   = "SEGV",
        [SIGUSR2]   = "USR2",
        [SIGPIPE]   = "PIPE",
        [SIGALRM]   = "ALRM",
        [SIGTERM]   = "TERM",
#నిర్వచితమైతే SIGSTKFLT
        [SIGSTKFLT] = "STKFLT",  /* Linux on SPARC doesn't know SIGSTKFLT */
#అయితే_ముగింపు
        [SIGCHLD]   = "CHLD",
        [SIGCONT]   = "CONT",
        [SIGSTOP]   = "STOP",
        [SIGTSTP]   = "TSTP",
        [SIGTTIN]   = "TTIN",
        [SIGTTOU]   = "TTOU",
        [SIGURG]    = "URG",
        [SIGXCPU]   = "XCPU",
        [SIGXFSZ]   = "XFSZ",
        [SIGVTALRM] = "VTALRM",
        [SIGPROF]   = "PROF",
        [SIGWINCH]  = "WINCH",
        [SIGIO]     = "IO",
        [SIGPWR]    = "PWR",
        [SIGSYS]    = "SYS"
};

DEFINE_వ్యక్తిగత_STRING_పట్టిక_LOOKUP(static_signal, పూర్ణాంకం);

స్థిరరాశి అక్షరం* signal_గమ్యం_string(పూర్ణాంకం signo) {
        స్థిర thread_local అక్షరం buf[STRLEN("RTMIN+") + DECIMAL_STR_MAX(పూర్ణాంకం)];
        స్థిరరాశి అక్షరం *name;

        name = static_signal_గమ్యం_string(signo);
        ఒకవేళ (name)
                తిరిగి name;

        ఒకవేళ (signo >= SIGRTMIN && signo <= SIGRTMAX)
                xsprintf(buf, "RTMIN+%d", signo - SIGRTMIN);
        లేకపోతే
                xsprintf(buf, "%d", signo);

        తిరిగి buf;
}

పూర్ణాంకం signal_from_string(స్థిరరాశి అక్షరం *s) {
        స్థిరరాశి అక్షరం *p;
        పూర్ణాంకం signo, r;

        /* Check that the input is a signal number. */
        ఒకవేళ (సురక్షిత_atoi(s, &signo) >= 0) {
                ఒకవేళ (SIGNAL_VALID(signo))
                        తిరిగి signo;
                లేకపోతే
                        తిరిగి -ERANGE;
        }

        /* Drop "SIG" prefix. */
        ఒకవేళ (startswith(s, "SIG"))
                s += 3;

        /* Check that the input is a signal name. */
        signo = static_signal_from_string(s);
        ఒకవేళ (signo > 0)
                తిరిగి signo;

        /* Check that the input is RTMIN or
         * RTMIN+n (0 <= n <= SIGRTMAX-SIGRTMIN). */
        p = startswith(s, "RTMIN");
        ఒకవేళ (p) {
                ఒకవేళ (*p == '\0')
                        తిరిగి SIGRTMIN;
                ఒకవేళ (*p != '+')
                        తిరిగి -EINVAL;

                r = సురక్షిత_atoi(p, &signo);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                ఒకవేళ (signo < 0 || signo > SIGRTMAX - SIGRTMIN)
                        తిరిగి -ERANGE;

                తిరిగి signo + SIGRTMIN;
        }

        /* Check that the input is RTMAX or
         * RTMAX-n (0 <= n <= SIGRTMAX-SIGRTMIN). */
        p = startswith(s, "RTMAX");
        ఒకవేళ (p) {
                ఒకవేళ (*p == '\0')
                        తిరిగి SIGRTMAX;
                ఒకవేళ (*p != '-')
                        తిరిగి -EINVAL;

                r = సురక్షిత_atoi(p, &signo);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                ఒకవేళ (signo > 0 || signo < SIGRTMIN - SIGRTMAX)
                        తిరిగి -ERANGE;

                తిరిగి signo + SIGRTMAX;
        }

        తిరిగి -EINVAL;
}

స్థిర స్థిరరాశి అక్షరం *స్థిరరాశి sigill_సంకేతం_పట్టిక[] = {
        [ILL_ILLOPC]   = "ILL_ILLOPC",
        [ILL_ILLOPN]   = "ILL_ILLOPN",
        [ILL_ILLADR]   = "ILL_ILLADR",
        [ILL_ILLTRP]   = "ILL_ILLTRP",
        [ILL_PRVOPC]   = "ILL_PRVOPC",
        [ILL_PRVREG]   = "ILL_PRVREG",
        [ILL_COPROC]   = "ILL_COPROC",
        [ILL_BADSTK]   = "ILL_BADSTK",
        [ILL_BADIADDR] = "ILL_BADIADDR",
};

DEFINE_వ్యక్తిగత_STRING_పట్టిక_LOOKUP_గమ్యం_STRING(sigill_సంకేతం, పూర్ణాంకం);

స్థిర స్థిరరాశి అక్షరం *స్థిరరాశి sigfpe_సంకేతం_పట్టిక[] = {
        [FPE_INTDIV]   = "FPE_INTDIV",
        [FPE_INTOVF]   = "FPE_INTOVF",
        [FPE_FLTDIV]   = "FPE_FLTDIV",
        [FPE_FLTOVF]   = "FPE_FLTOVF",
        [FPE_FLTUND]   = "FPE_FLTUND",
        [FPE_FLTRES]   = "FPE_FLTRES",
        [FPE_FLTINV]   = "FPE_FLTINV",
        [FPE_FLTSUB]   = "FPE_FLTSUB",
        [FPE_FLTUNK]   = "FPE_FLTUNK",
        [FPE_CONDTRAP] = "FPE_CONDTRAP",
};

DEFINE_వ్యక్తిగత_STRING_పట్టిక_LOOKUP_గమ్యం_STRING(sigfpe_సంకేతం, పూర్ణాంకం);

స్థిర స్థిరరాశి అక్షరం *స్థిరరాశి sigsegv_సంకేతం_పట్టిక[] = {
        [SEGV_MAPERR]  = "SEGV_MAPERR",
        [SEGV_ACCERR]  = "SEGV_ACCERR",
        [SEGV_BNDERR]  = "SEGV_BNDERR",
        [SEGV_PKUERR]  = "SEGV_PKUERR",
        [SEGV_ACCADI]  = "SEGV_ACCADI",
        [SEGV_ADIDERR] = "SEGV_ADIDERR",
        [SEGV_ADIPERR] = "SEGV_ADIPERR",
        [SEGV_MTEAERR] = "SEGV_MTEAERR",
        [SEGV_MTESERR] = "SEGV_MTESERR",
        [SEGV_CPERR]   = "SEGV_CPERR",
};

DEFINE_వ్యక్తిగత_STRING_పట్టిక_LOOKUP_గమ్యం_STRING(sigsegv_సంకేతం, పూర్ణాంకం);

స్థిర స్థిరరాశి అక్షరం *స్థిరరాశి sigbus_సంకేతం_పట్టిక[] = {
        [BUS_ADRALN]    = "BUS_ADRALN",
        [BUS_ADRERR]    = "BUS_ADRERR",
        [BUS_OBJERR]    = "BUS_OBJERR",
        [BUS_MCEERR_AR] = "BUS_MCEERR_AR",
        [BUS_MCEERR_AO] = "BUS_MCEERR_AO",
};

DEFINE_వ్యక్తిగత_STRING_పట్టిక_LOOKUP_గమ్యం_STRING(sigbus_సంకేతం, పూర్ణాంకం);

స్థిర స్థిరరాశి అక్షరం *స్థిరరాశి sigtrap_సంకేతం_పట్టిక[] = {
        [TRAP_BRKPT]  = "TRAP_BRKPT",
        [TRAP_TRACE]  = "TRAP_TRACE",
        [TRAP_BRANCH] = "TRAP_BRANCH",
        [TRAP_HWBKPT] = "TRAP_HWBKPT",
        [TRAP_UNK]    = "TRAP_UNK",
        [TRAP_PERF]   = "TRAP_PERF",
};

DEFINE_వ్యక్తిగత_STRING_పట్టిక_LOOKUP_గమ్యం_STRING(sigtrap_సంకేతం, పూర్ణాంకం);

స్థిర స్థిరరాశి అక్షరం *స్థిరరాశి sigsys_సంకేతం_పట్టిక[] = {
        [SYS_SECCOMP]       = "SYS_SECCOMP",
        [SYS_USER_DISPATCH] = "SYS_USER_DISPATCH",
};

DEFINE_వ్యక్తిగత_STRING_పట్టిక_LOOKUP_గమ్యం_STRING(sigsys_సంకేతం, పూర్ణాంకం);

/* sigchld_code_table is already defined in src/basic/process-util.c with
 * decoded, lower-case values (CLD_EXITED -> "exited"). For consistency with
 * all other values decoded here, provide an uppercase variant. */
స్థిర స్థిరరాశి అక్షరం *స్థిరరాశి sigchld_uppercase_సంకేతం_పట్టిక[] = {
        [CLD_EXITED]    = "CLD_EXITED",
        [CLD_KILLED]    = "CLD_KILLED",
        [CLD_DUMPED]    = "CLD_DUMPED",
        [CLD_TRAPPED]   = "CLD_TRAPPED",
        [CLD_STOPPED]   = "CLD_STOPPED",
        [CLD_CONTINUED] = "CLD_CONTINUED",
};

DEFINE_వ్యక్తిగత_STRING_పట్టిక_LOOKUP_గమ్యం_STRING(sigchld_uppercase_సంకేతం, పూర్ణాంకం);

స్థిర స్థిరరాశి అక్షరం *స్థిరరాశి sigpoll_సంకేతం_పట్టిక[] = {
        [POLL_IN]  = "POLL_IN",
        [POLL_OUT] = "POLL_OUT",
        [POLL_MSG] = "POLL_MSG",
        [POLL_ERR] = "POLL_ERR",
        [POLL_PRI] = "POLL_PRI",
        [POLL_HUP] = "POLL_HUP",
};

DEFINE_వ్యక్తిగత_STRING_పట్టిక_LOOKUP_గమ్యం_STRING(sigpoll_సంకేతం, పూర్ణాంకం);

స్థిరరాశి అక్షరం* signal_సంకేతం_గమ్యం_string(పూర్ణాంకం signo, పూర్ణాంకం సంకేతం_2) {
        ఎంపిక (సంకేతం_2) {
        /* SI_KERNEL is 0x80 (128). Defining a table just for SI_KERNEL and
         * SI_USER would be a waste of .rodata space: special case them instead. */
        స్థితి SI_KERNEL:
                తిరిగి "SI_KERNEL";
        స్థితి SI_USER:
                తిరిగి "SI_USER";
        /* The following values are all negatives. SI_ASYNCNL is -60. Similarly
         * to the special case for SI_KERNEL above, avoid using a table for negative
         * values as well. */
        స్థితి SI_ASYNCNL:
                తిరిగి "SI_ASYNCNL";
        స్థితి SI_DETHREAD:
                తిరిగి "SI_DETHREAD";
        స్థితి SI_TKILL:
                తిరిగి "SI_TKILL";
        స్థితి SI_SIGIO:
                తిరిగి "SI_SIGIO";
        స్థితి SI_ASYNCIO:
                తిరిగి "SI_ASYNCIO";
        స్థితి SI_MESGQ:
                తిరిగి "SI_MESGQ";
        స్థితి SI_TIMER:
                తిరిగి "SI_TIMER";
        స్థితి SI_QUEUE:
                తిరిగి "SI_QUEUE";
        }

        ఎంపిక (signo) {
        స్థితి SIGILL:
                తిరిగి sigill_సంకేతం_గమ్యం_string(సంకేతం_2);
        స్థితి SIGFPE:
                తిరిగి sigfpe_సంకేతం_గమ్యం_string(సంకేతం_2);
        స్థితి SIGSEGV:
                తిరిగి sigsegv_సంకేతం_గమ్యం_string(సంకేతం_2);
        స్థితి SIGBUS:
                తిరిగి sigbus_సంకేతం_గమ్యం_string(సంకేతం_2);
        స్థితి SIGTRAP:
                తిరిగి sigtrap_సంకేతం_గమ్యం_string(సంకేతం_2);
        స్థితి SIGCHLD:
                తిరిగి sigchld_uppercase_సంకేతం_గమ్యం_string(సంకేతం_2);
        స్థితి SIGPOLL:
                తిరిగి sigpoll_సంకేతం_గమ్యం_string(సంకేతం_2);
        స్థితి SIGSYS:
                తిరిగి sigsys_సంకేతం_గమ్యం_string(సంకేతం_2);
        అప్రమేయ:
                తిరిగి ఖాళీ;
        }
}

శూన్య nop_signal_handler(పూర్ణాంకం sig) {
        /* nothing here */
}

పూర్ణాంకం signal_is_blocked(పూర్ణాంకం sig) {
        sigset_t ss;
        పూర్ణాంకం r;

        r = pthread_sigmask(SIG_SETMASK, ఖాళీ, &ss);
        ఒకవేళ (r != 0)
                తిరిగి -r;

        తిరిగి RET_NERRNO(sigismember(&ss, sig));
}

పూర్ణాంకం autoreaping_enabled(శూన్య) {
        నిర్మాణం sigaction sa;

        ఒకవేళ (sigaction(SIGCHLD, /* act= */ ఖాళీ, &sa) < 0)
                తిరిగి -errno;

        తిరిగి sa.sa_handler == SIG_IGN || FLAGS_సమితి(sa.sa_flags, SA_NOCLDWAIT);
}

పూర్ణాంకం తీయి_pending_signal_అంతర్గత(పూర్ణాంకం sig, ...) {
        sigset_t ss;
        va_list ap;
        పూర్ణాంకం r;

        ఒకవేళ (sig < 0) /* Empty list? */
                తిరిగి -EINVAL;

        ఒకవేళ (sigemptyset(&ss) < 0)
                తిరిగి -errno;

        /* Add first signal (if the signal is zero, we'll silently skip it, to make it easier to build
         * parameter lists where some element are sometimes off, similar to how sigset_add_many_ap() handles
         * this.) */
        ఒకవేళ (sig > 0 && sigaddset(&ss, sig) < 0)
                తిరిగి -errno;

        /* Add all other signals */
        va_start(ap, sig);
        r = sigset_కూడు_many_ap(&ss, ap);
        va_end(ap);
        ఒకవేళ (r < 0)
                తిరిగి r;

        r = sigtimedwait(&ss, ఖాళీ, &(స్థిరరాశి నిర్మాణం timespec) {});
        ఒకవేళ (r < 0) {
                ఒకవేళ (errno == EAGAIN)
                        తిరిగి 0;

                తిరిగి -errno;
        }

        తిరిగి r; /* Returns the signal popped */
}

శూన్య propagate_signal(పూర్ణాంకం sig, siginfo_t *siginfo) {
        pid_t p;

        /* To be called from a signal handler. Will raise the same signal again, in our process + in our threads.
         *
         * Note that we use getpid() instead of getpid_cached(). We might have forked with raw_clone()
         * earlier (see PID 1), and hence let's go to the raw syscall here. In particular as this is not
         * performance sensitive code.
         *
         * Note that we use kill() rather than raise() as fallback, for similar reasons. */

        p = getpid();

        ఒకవేళ (rt_tgsigqueueinfo(p, gettid(), sig, siginfo) < 0)
                assert_se(kill(p, sig) >= 0);
}

స్థిరరాశి నిర్మాణం sigaction sigaction_ignore = {
        .sa_handler = SIG_IGN,
        .sa_flags = SA_RESTART,
};

స్థిరరాశి నిర్మాణం sigaction sigaction_default = {
        .sa_handler = SIG_DFL,
        .sa_flags = SA_RESTART,
};

స్థిరరాశి నిర్మాణం sigaction sigaction_nop_nocldstop = {
        .sa_handler = nop_signal_handler,
        .sa_flags = SA_NOCLDSTOP|SA_RESTART,
};

పూర్ణాంకం parse_signo(స్థిరరాశి అక్షరం *s, పూర్ణాంకం *ret) {
        పూర్ణాంకం sig, r;

        r = సురక్షిత_atoi(s, &sig);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (!SIGNAL_VALID(sig))
                తిరిగి -EINVAL;

        ఒకవేళ (ret)
                *ret = sig;

        తిరిగి 0;
}

శూన్య sigterm_process_సమూహం_handler(పూర్ణాంకం సంకేతం, siginfo_t *info, శూన్య *ucontext) {
        ప్రతిజ్ఞా(సంకేతం == SIGTERM);
        ప్రతిజ్ఞా(info);

        /* If the sender is not us, propagate the signal to all processes in
         * the same process group */
        ఒకవేళ (si_సంకేతం_from_process(info->si_code) &&
            pid_is_valid(info->si_pid) &&
            info->si_pid != getpid_cached())
                (శూన్య) kill(0, సంకేతం);
}
