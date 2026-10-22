/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <threads.h>
#ಅನ್ತರ್ಭಾವ <unistd.h>

#ಅನ್ತರ್ಭಾವ "errno-util.h"
#ಅನ್ತರ್ಭಾವ "parse-util.h"
#ಅನ್ತರ್ಭಾವ "process-util.h"
#ಅನ್ತರ್ಭಾವ "signal-util.h"
#ಅನ್ತರ್ಭಾವ "stdio-util.h"
#ಅನ್ತರ್ಭಾವ "string-table.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"

ಪೂರ್ಣಾಙ್ಕ reset_all_signal_handlers(ಶೂನ್ಯ) {
        ಪೂರ್ಣಾಙ್ಕ ret = 0, r;

        ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ sig = 1; sig < _NSIG; sig++) {

                /* These two cannot be caught... */
                ಯದಿ (IN_SET(sig, SIGKILL, SIGSTOP))
                        ಅನುವರ್ತ;

                /* On Linux the first two RT signals are reserved by glibc, and sigaction() will return
                 * EINVAL for them. */
                r = RET_NERRNO(sigaction(sig, &sigaction_default, ರಿಕ್ತ));
                ಯದಿ (r != -EINVAL)
                        RET_GATHER(ret, r);
        }

        ಪ್ರತ್ಯಾಗಮ ret;
}

ಪೂರ್ಣಾಙ್ಕ reset_signal_mask(ಶೂನ್ಯ) {
        sigset_t ss;

        ಯದಿ (sigemptyset(&ss) < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಪ್ರತ್ಯಾಗಮ RET_NERRNO(sigprocmask(SIG_SETMASK, &ss, ರಿಕ್ತ));
}

ಪೂರ್ಣಾಙ್ಕ sigaction_many_internal(ಧ್ರುವ ಸಂರಚನಾ sigaction *sa, ...) {
        ಪೂರ್ಣಾಙ್ಕ sig, r = 0;
        va_list ap;

        va_start(ap, sa);

        /* negative signal ends the list. 0 signal is skipped. */
        ಯಾವತ್ ((sig = va_arg(ap, ಪೂರ್ಣಾಙ್ಕ)) >= 0) {

                ಯದಿ (sig == 0)
                        ಅನುವರ್ತ;

                RET_GATHER(r, RET_NERRNO(sigaction(sig, sa, ರಿಕ್ತ)));
        }

        va_end(ap);

        ಪ್ರತ್ಯಾಗಮ r;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sigset_add_many_ap(sigset_t *ss, va_list ap) {
        ಪೂರ್ಣಾಙ್ಕ sig, r = 0;

        ಪ್ರತಿಜ್ಞಾ(ss);

        ಯಾವತ್ ((sig = va_arg(ap, ಪೂರ್ಣಾಙ್ಕ)) >= 0) {

                ಯದಿ (sig == 0)
                        ಅನುವರ್ತ;

                ಯದಿ (sigaddset(ss, sig) < 0) {
                        ಯದಿ (r >= 0)
                                r = -errno;
                }
        }

        ಪ್ರತ್ಯಾಗಮ r;
}

ಪೂರ್ಣಾಙ್ಕ sigset_add_many_internal(sigset_t *ss, ...) {
        va_list ap;
        ಪೂರ್ಣಾಙ್ಕ r;

        va_start(ap, ss);
        r = sigset_add_many_ap(ss, ap);
        va_end(ap);

        ಪ್ರತ್ಯಾಗಮ r;
}

ಪೂರ್ಣಾಙ್ಕ sigprocmask_many_internal(ಪೂರ್ಣಾಙ್ಕ how, sigset_t *ret_old_mask, ...) {
        va_list ap;
        sigset_t ss;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಯದಿ (sigemptyset(&ss) < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        va_start(ap, ret_old_mask);
        r = sigset_add_many_ap(&ss, ap);
        va_end(ap);

        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ RET_NERRNO(sigprocmask(how, &ss, ret_old_mask));
}

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ *ಧ್ರುವ static_signal_table[] = {
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
#ifdef SIGSTKFLT
        [SIGSTKFLT] = "STKFLT",  /* Linux on SPARC doesn't know SIGSTKFLT */
#endif
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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP(static_signal, ಪೂರ್ಣಾಙ್ಕ);

ಧ್ರುವ ವರ್ಣ* signal_to_string(ಪೂರ್ಣಾಙ್ಕ signo) {
        ಸ್ಥಿರ thread_local ವರ್ಣ buf[STRLEN("RTMIN+") + DECIMAL_STR_MAX(ಪೂರ್ಣಾಙ್ಕ)];
        ಧ್ರುವ ವರ್ಣ *name;

        name = static_signal_to_string(signo);
        ಯದಿ (name)
                ಪ್ರತ್ಯಾಗಮ name;

        ಯದಿ (signo >= SIGRTMIN && signo <= SIGRTMAX)
                xsprintf(buf, "RTMIN+%d", signo - SIGRTMIN);
        ಅನ್ಯಥಾ
                xsprintf(buf, "%d", signo);

        ಪ್ರತ್ಯಾಗಮ buf;
}

ಪೂರ್ಣಾಙ್ಕ signal_from_string(ಧ್ರುವ ವರ್ಣ *s) {
        ಧ್ರುವ ವರ್ಣ *p;
        ಪೂರ್ಣಾಙ್ಕ signo, r;

        /* Check that the input is a signal number. */
        ಯದಿ (safe_atoi(s, &signo) >= 0) {
                ಯದಿ (SIGNAL_VALID(signo))
                        ಪ್ರತ್ಯಾಗಮ signo;
                ಅನ್ಯಥಾ
                        ಪ್ರತ್ಯಾಗಮ -ERANGE;
        }

        /* Drop "SIG" prefix. */
        ಯದಿ (startswith(s, "SIG"))
                s += 3;

        /* Check that the input is a signal name. */
        signo = static_signal_from_string(s);
        ಯದಿ (signo > 0)
                ಪ್ರತ್ಯಾಗಮ signo;

        /* Check that the input is RTMIN or
         * RTMIN+n (0 <= n <= SIGRTMAX-SIGRTMIN). */
        p = startswith(s, "RTMIN");
        ಯದಿ (p) {
                ಯದಿ (*p == '\0')
                        ಪ್ರತ್ಯಾಗಮ SIGRTMIN;
                ಯದಿ (*p != '+')
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                r = safe_atoi(p, &signo);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                ಯದಿ (signo < 0 || signo > SIGRTMAX - SIGRTMIN)
                        ಪ್ರತ್ಯಾಗಮ -ERANGE;

                ಪ್ರತ್ಯಾಗಮ signo + SIGRTMIN;
        }

        /* Check that the input is RTMAX or
         * RTMAX-n (0 <= n <= SIGRTMAX-SIGRTMIN). */
        p = startswith(s, "RTMAX");
        ಯದಿ (p) {
                ಯದಿ (*p == '\0')
                        ಪ್ರತ್ಯಾಗಮ SIGRTMAX;
                ಯದಿ (*p != '-')
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                r = safe_atoi(p, &signo);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                ಯದಿ (signo > 0 || signo < SIGRTMIN - SIGRTMAX)
                        ಪ್ರತ್ಯಾಗಮ -ERANGE;

                ಪ್ರತ್ಯಾಗಮ signo + SIGRTMAX;
        }

        ಪ್ರತ್ಯಾಗಮ -EINVAL;
}

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ *ಧ್ರುವ sigill_code_table[] = {
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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigill_code, ಪೂರ್ಣಾಙ್ಕ);

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ *ಧ್ರುವ sigfpe_code_table[] = {
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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigfpe_code, ಪೂರ್ಣಾಙ್ಕ);

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ *ಧ್ರುವ sigsegv_code_table[] = {
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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigsegv_code, ಪೂರ್ಣಾಙ್ಕ);

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ *ಧ್ರುವ sigbus_code_table[] = {
        [BUS_ADRALN]    = "BUS_ADRALN",
        [BUS_ADRERR]    = "BUS_ADRERR",
        [BUS_OBJERR]    = "BUS_OBJERR",
        [BUS_MCEERR_AR] = "BUS_MCEERR_AR",
        [BUS_MCEERR_AO] = "BUS_MCEERR_AO",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigbus_code, ಪೂರ್ಣಾಙ್ಕ);

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ *ಧ್ರುವ sigtrap_code_table[] = {
        [TRAP_BRKPT]  = "TRAP_BRKPT",
        [TRAP_TRACE]  = "TRAP_TRACE",
        [TRAP_BRANCH] = "TRAP_BRANCH",
        [TRAP_HWBKPT] = "TRAP_HWBKPT",
        [TRAP_UNK]    = "TRAP_UNK",
        [TRAP_PERF]   = "TRAP_PERF",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigtrap_code, ಪೂರ್ಣಾಙ್ಕ);

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ *ಧ್ರುವ sigsys_code_table[] = {
        [SYS_SECCOMP]       = "SYS_SECCOMP",
        [SYS_USER_DISPATCH] = "SYS_USER_DISPATCH",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigsys_code, ಪೂರ್ಣಾಙ್ಕ);

/* sigchld_code_table is already defined in src/basic/process-util.c with
 * decoded, lower-case values (CLD_EXITED -> "exited"). For consistency with
 * all other values decoded here, provide an uppercase variant. */
ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ *ಧ್ರುವ sigchld_uppercase_code_table[] = {
        [CLD_EXITED]    = "CLD_EXITED",
        [CLD_KILLED]    = "CLD_KILLED",
        [CLD_DUMPED]    = "CLD_DUMPED",
        [CLD_TRAPPED]   = "CLD_TRAPPED",
        [CLD_STOPPED]   = "CLD_STOPPED",
        [CLD_CONTINUED] = "CLD_CONTINUED",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigchld_uppercase_code, ಪೂರ್ಣಾಙ್ಕ);

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ *ಧ್ರುವ sigpoll_code_table[] = {
        [POLL_IN]  = "POLL_IN",
        [POLL_OUT] = "POLL_OUT",
        [POLL_MSG] = "POLL_MSG",
        [POLL_ERR] = "POLL_ERR",
        [POLL_PRI] = "POLL_PRI",
        [POLL_HUP] = "POLL_HUP",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigpoll_code, ಪೂರ್ಣಾಙ್ಕ);

ಧ್ರುವ ವರ್ಣ* signal_code_to_string(ಪೂರ್ಣಾಙ್ಕ signo, ಪೂರ್ಣಾಙ್ಕ code) {
        ವಿಕಲ್ಪ (code) {
        /* SI_KERNEL is 0x80 (128). Defining a table just for SI_KERNEL and
         * SI_USER would be a waste of .rodata space: special case them instead. */
        ಪಕ್ಷ SI_KERNEL:
                ಪ್ರತ್ಯಾಗಮ "SI_KERNEL";
        ಪಕ್ಷ SI_USER:
                ಪ್ರತ್ಯಾಗಮ "SI_USER";
        /* The following values are all negatives. SI_ASYNCNL is -60. Similarly
         * to the special case for SI_KERNEL above, avoid using a table for negative
         * values as well. */
        ಪಕ್ಷ SI_ASYNCNL:
                ಪ್ರತ್ಯಾಗಮ "SI_ASYNCNL";
        ಪಕ್ಷ SI_DETHREAD:
                ಪ್ರತ್ಯಾಗಮ "SI_DETHREAD";
        ಪಕ್ಷ SI_TKILL:
                ಪ್ರತ್ಯಾಗಮ "SI_TKILL";
        ಪಕ್ಷ SI_SIGIO:
                ಪ್ರತ್ಯಾಗಮ "SI_SIGIO";
        ಪಕ್ಷ SI_ASYNCIO:
                ಪ್ರತ್ಯಾಗಮ "SI_ASYNCIO";
        ಪಕ್ಷ SI_MESGQ:
                ಪ್ರತ್ಯಾಗಮ "SI_MESGQ";
        ಪಕ್ಷ SI_TIMER:
                ಪ್ರತ್ಯಾಗಮ "SI_TIMER";
        ಪಕ್ಷ SI_QUEUE:
                ಪ್ರತ್ಯಾಗಮ "SI_QUEUE";
        }

        ವಿಕಲ್ಪ (signo) {
        ಪಕ್ಷ SIGILL:
                ಪ್ರತ್ಯಾಗಮ sigill_code_to_string(code);
        ಪಕ್ಷ SIGFPE:
                ಪ್ರತ್ಯಾಗಮ sigfpe_code_to_string(code);
        ಪಕ್ಷ SIGSEGV:
                ಪ್ರತ್ಯಾಗಮ sigsegv_code_to_string(code);
        ಪಕ್ಷ SIGBUS:
                ಪ್ರತ್ಯಾಗಮ sigbus_code_to_string(code);
        ಪಕ್ಷ SIGTRAP:
                ಪ್ರತ್ಯಾಗಮ sigtrap_code_to_string(code);
        ಪಕ್ಷ SIGCHLD:
                ಪ್ರತ್ಯಾಗಮ sigchld_uppercase_code_to_string(code);
        ಪಕ್ಷ SIGPOLL:
                ಪ್ರತ್ಯಾಗಮ sigpoll_code_to_string(code);
        ಪಕ್ಷ SIGSYS:
                ಪ್ರತ್ಯಾಗಮ sigsys_code_to_string(code);
        ಸ್ವಭಾವ:
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
        }
}

ಶೂನ್ಯ nop_signal_handler(ಪೂರ್ಣಾಙ್ಕ sig) {
        /* nothing here */
}

ಪೂರ್ಣಾಙ್ಕ signal_is_blocked(ಪೂರ್ಣಾಙ್ಕ sig) {
        sigset_t ss;
        ಪೂರ್ಣಾಙ್ಕ r;

        r = pthread_sigmask(SIG_SETMASK, ರಿಕ್ತ, &ss);
        ಯದಿ (r != 0)
                ಪ್ರತ್ಯಾಗಮ -r;

        ಪ್ರತ್ಯಾಗಮ RET_NERRNO(sigismember(&ss, sig));
}

ಪೂರ್ಣಾಙ್ಕ autoreaping_enabled(ಶೂನ್ಯ) {
        ಸಂರಚನಾ sigaction sa;

        ಯದಿ (sigaction(SIGCHLD, /* act= */ ರಿಕ್ತ, &sa) < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಪ್ರತ್ಯಾಗಮ sa.sa_handler == SIG_IGN || FLAGS_SET(sa.sa_flags, SA_NOCLDWAIT);
}

ಪೂರ್ಣಾಙ್ಕ ತೆಗೆ_pending_signal_internal(ಪೂರ್ಣಾಙ್ಕ sig, ...) {
        sigset_t ss;
        va_list ap;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಯದಿ (sig < 0) /* Empty list? */
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        ಯದಿ (sigemptyset(&ss) < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        /* Add first signal (if the signal is zero, we'll silently skip it, to make it easier to build
         * parameter lists where some element are sometimes off, similar to how sigset_add_many_ap() handles
         * this.) */
        ಯದಿ (sig > 0 && sigaddset(&ss, sig) < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        /* Add all other signals */
        va_start(ap, sig);
        r = sigset_add_many_ap(&ss, ap);
        va_end(ap);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        r = sigtimedwait(&ss, ರಿಕ್ತ, &(ಧ್ರುವ ಸಂರಚನಾ timespec) {});
        ಯದಿ (r < 0) {
                ಯದಿ (errno == EAGAIN)
                        ಪ್ರತ್ಯಾಗಮ 0;

                ಪ್ರತ್ಯಾಗಮ -errno;
        }

        ಪ್ರತ್ಯಾಗಮ r; /* Returns the signal popped */
}

ಶೂನ್ಯ propagate_signal(ಪೂರ್ಣಾಙ್ಕ sig, siginfo_t *siginfo) {
        pid_t p;

        /* To be called from a signal handler. Will raise the same signal again, in our process + in our threads.
         *
         * Note that we use getpid() instead of getpid_cached(). We might have forked with raw_clone()
         * earlier (see PID 1), and hence let's go to the raw syscall here. In particular as this is not
         * performance sensitive code.
         *
         * Note that we use kill() rather than raise() as fallback, for similar reasons. */

        p = getpid();

        ಯದಿ (rt_tgsigqueueinfo(p, gettid(), sig, siginfo) < 0)
                assert_se(kill(p, sig) >= 0);
}

ಧ್ರುವ ಸಂರಚನಾ sigaction sigaction_ignore = {
        .sa_handler = SIG_IGN,
        .sa_flags = SA_RESTART,
};

ಧ್ರುವ ಸಂರಚನಾ sigaction sigaction_default = {
        .sa_handler = SIG_DFL,
        .sa_flags = SA_RESTART,
};

ಧ್ರುವ ಸಂರಚನಾ sigaction sigaction_nop_nocldstop = {
        .sa_handler = nop_signal_handler,
        .sa_flags = SA_NOCLDSTOP|SA_RESTART,
};

ಪೂರ್ಣಾಙ್ಕ parse_signo(ಧ್ರುವ ವರ್ಣ *s, ಪೂರ್ಣಾಙ್ಕ *ret) {
        ಪೂರ್ಣಾಙ್ಕ sig, r;

        r = safe_atoi(s, &sig);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (!SIGNAL_VALID(sig))
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        ಯದಿ (ret)
                *ret = sig;

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಶೂನ್ಯ sigterm_process_group_handler(ಪೂರ್ಣಾಙ್ಕ ಸಙ್ಕೇತ, siginfo_t *info, ಶೂನ್ಯ *ucontext) {
        ಪ್ರತಿಜ್ಞಾ(ಸಙ್ಕೇತ == SIGTERM);
        ಪ್ರತಿಜ್ಞಾ(info);

        /* If the sender is not us, propagate the signal to all processes in
         * the same process group */
        ಯದಿ (si_code_from_process(info->si_code) &&
            pid_is_valid(info->si_pid) &&
            info->si_pid != getpid_cached())
                (ಶೂನ್ಯ) kill(0, ಸಙ್ಕೇತ);
}
