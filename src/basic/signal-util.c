/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ <threads.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <unistd.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "errno-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "parse-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "process-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "signal-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "stdio-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-table.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-util.h"

ਪੂਰਨ reset_all_signal_handlers(ਖਾਲੀ) {
        ਪੂਰਨ ret = 0, r;

        ਲਈ (ਪੂਰਨ sig = 1; sig < _NSIG; sig++) {

                /* These two cannot be caught... */
                ਜੇ (ਵਿੱਚ_ਸੈੱਟ(sig, SIGKILL, SIGSTOP))
                        ਜਾਰੀ;

                /* On Linux the first two RT signals are reserved by glibc, and sigaction() will return
                 * EINVAL for them. */
                r = RET_NERRNO(sigaction(sig, &sigaction_default, ਨਿਰਾ));
                ਜੇ (r != -EINVAL)
                        RET_GATHER(ret, r);
        }

        ਵਾਪਸ ret;
}

ਪੂਰਨ reset_signal_ਨਕਾਬ(ਖਾਲੀ) {
        sigset_t ss;

        ਜੇ (sigemptyset(&ss) < 0)
                ਵਾਪਸ -errno;

        ਵਾਪਸ RET_NERRNO(sigprocmask(SIG_SETMASK, &ss, ਨਿਰਾ));
}

ਪੂਰਨ sigaction_many_ਅੰਦਰੂਨੀ(ਅਚਲ ਬਣਤਰ sigaction *sa, ...) {
        ਪੂਰਨ sig, r = 0;
        va_list ap;

        va_start(ap, sa);

        /* negative signal ends the list. 0 signal is skipped. */
        ਜਦੋਂ ((sig = va_arg(ap, ਪੂਰਨ)) >= 0) {

                ਜੇ (sig == 0)
                        ਜਾਰੀ;

                RET_GATHER(r, RET_NERRNO(sigaction(sig, sa, ਨਿਰਾ)));
        }

        va_end(ap);

        ਵਾਪਸ r;
}

ਸਥਿਰ ਪੂਰਨ sigset_ਜੋੜੋ_many_ap(sigset_t *ss, va_list ap) {
        ਪੂਰਨ sig, r = 0;

        ਪ੍ਰਤਿਜ੍ਞਾ(ss);

        ਜਦੋਂ ((sig = va_arg(ap, ਪੂਰਨ)) >= 0) {

                ਜੇ (sig == 0)
                        ਜਾਰੀ;

                ਜੇ (sigaddset(ss, sig) < 0) {
                        ਜੇ (r >= 0)
                                r = -errno;
                }
        }

        ਵਾਪਸ r;
}

ਪੂਰਨ sigset_ਜੋੜੋ_many_ਅੰਦਰੂਨੀ(sigset_t *ss, ...) {
        va_list ap;
        ਪੂਰਨ r;

        va_start(ap, ss);
        r = sigset_ਜੋੜੋ_many_ap(ss, ap);
        va_end(ap);

        ਵਾਪਸ r;
}

ਪੂਰਨ sigprocmask_many_ਅੰਦਰੂਨੀ(ਪੂਰਨ how, sigset_t *ret_old_ਨਕਾਬ, ...) {
        va_list ap;
        sigset_t ss;
        ਪੂਰਨ r;

        ਜੇ (sigemptyset(&ss) < 0)
                ਵਾਪਸ -errno;

        va_start(ap, ret_old_ਨਕਾਬ);
        r = sigset_ਜੋੜੋ_many_ap(&ss, ap);
        va_end(ap);

        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ RET_NERRNO(sigprocmask(how, &ss, ret_old_ਨਕਾਬ));
}

ਸਥਿਰ ਅਚਲ ਅੱਖਰ *ਅਚਲ static_signal_ਸਾਰਣੀ[] = {
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
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SIGSTKFLT
        [SIGSTKFLT] = "STKFLT",  /* Linux on SPARC doesn't know SIGSTKFLT */
#ਜੇ_ਅੰਤ
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

DEFINE_ਨਿੱਜੀ_STRING_ਸਾਰਣੀ_LOOKUP(static_signal, ਪੂਰਨ);

ਅਚਲ ਅੱਖਰ* signal_ਮੰਜ਼ਿਲ_string(ਪੂਰਨ signo) {
        ਸਥਿਰ thread_local ਅੱਖਰ buf[STRLEN("RTMIN+") + DECIMAL_STR_MAX(ਪੂਰਨ)];
        ਅਚਲ ਅੱਖਰ *name;

        name = static_signal_ਮੰਜ਼ਿਲ_string(signo);
        ਜੇ (name)
                ਵਾਪਸ name;

        ਜੇ (signo >= SIGRTMIN && signo <= SIGRTMAX)
                xsprintf(buf, "RTMIN+%d", signo - SIGRTMIN);
        ਨਹੀਂਤਾਂ
                xsprintf(buf, "%d", signo);

        ਵਾਪਸ buf;
}

ਪੂਰਨ signal_from_string(ਅਚਲ ਅੱਖਰ *s) {
        ਅਚਲ ਅੱਖਰ *p;
        ਪੂਰਨ signo, r;

        /* Check that the input is a signal number. */
        ਜੇ (ਸੁਰੱਖਿਅਤ_atoi(s, &signo) >= 0) {
                ਜੇ (SIGNAL_VALID(signo))
                        ਵਾਪਸ signo;
                ਨਹੀਂਤਾਂ
                        ਵਾਪਸ -ERANGE;
        }

        /* Drop "SIG" prefix. */
        ਜੇ (startswith(s, "SIG"))
                s += 3;

        /* Check that the input is a signal name. */
        signo = static_signal_from_string(s);
        ਜੇ (signo > 0)
                ਵਾਪਸ signo;

        /* Check that the input is RTMIN or
         * RTMIN+n (0 <= n <= SIGRTMAX-SIGRTMIN). */
        p = startswith(s, "RTMIN");
        ਜੇ (p) {
                ਜੇ (*p == '\0')
                        ਵਾਪਸ SIGRTMIN;
                ਜੇ (*p != '+')
                        ਵਾਪਸ -EINVAL;

                r = ਸੁਰੱਖਿਅਤ_atoi(p, &signo);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                ਜੇ (signo < 0 || signo > SIGRTMAX - SIGRTMIN)
                        ਵਾਪਸ -ERANGE;

                ਵਾਪਸ signo + SIGRTMIN;
        }

        /* Check that the input is RTMAX or
         * RTMAX-n (0 <= n <= SIGRTMAX-SIGRTMIN). */
        p = startswith(s, "RTMAX");
        ਜੇ (p) {
                ਜੇ (*p == '\0')
                        ਵਾਪਸ SIGRTMAX;
                ਜੇ (*p != '-')
                        ਵਾਪਸ -EINVAL;

                r = ਸੁਰੱਖਿਅਤ_atoi(p, &signo);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                ਜੇ (signo > 0 || signo < SIGRTMIN - SIGRTMAX)
                        ਵਾਪਸ -ERANGE;

                ਵਾਪਸ signo + SIGRTMAX;
        }

        ਵਾਪਸ -EINVAL;
}

ਸਥਿਰ ਅਚਲ ਅੱਖਰ *ਅਚਲ sigill_ਕੋਡ_ਸਾਰਣੀ[] = {
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

DEFINE_ਨਿੱਜੀ_STRING_ਸਾਰਣੀ_LOOKUP_ਮੰਜ਼ਿਲ_STRING(sigill_ਕੋਡ, ਪੂਰਨ);

ਸਥਿਰ ਅਚਲ ਅੱਖਰ *ਅਚਲ sigfpe_ਕੋਡ_ਸਾਰਣੀ[] = {
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

DEFINE_ਨਿੱਜੀ_STRING_ਸਾਰਣੀ_LOOKUP_ਮੰਜ਼ਿਲ_STRING(sigfpe_ਕੋਡ, ਪੂਰਨ);

ਸਥਿਰ ਅਚਲ ਅੱਖਰ *ਅਚਲ sigsegv_ਕੋਡ_ਸਾਰਣੀ[] = {
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

DEFINE_ਨਿੱਜੀ_STRING_ਸਾਰਣੀ_LOOKUP_ਮੰਜ਼ਿਲ_STRING(sigsegv_ਕੋਡ, ਪੂਰਨ);

ਸਥਿਰ ਅਚਲ ਅੱਖਰ *ਅਚਲ sigbus_ਕੋਡ_ਸਾਰਣੀ[] = {
        [BUS_ADRALN]    = "BUS_ADRALN",
        [BUS_ADRERR]    = "BUS_ADRERR",
        [BUS_OBJERR]    = "BUS_OBJERR",
        [BUS_MCEERR_AR] = "BUS_MCEERR_AR",
        [BUS_MCEERR_AO] = "BUS_MCEERR_AO",
};

DEFINE_ਨਿੱਜੀ_STRING_ਸਾਰਣੀ_LOOKUP_ਮੰਜ਼ਿਲ_STRING(sigbus_ਕੋਡ, ਪੂਰਨ);

ਸਥਿਰ ਅਚਲ ਅੱਖਰ *ਅਚਲ sigtrap_ਕੋਡ_ਸਾਰਣੀ[] = {
        [TRAP_BRKPT]  = "TRAP_BRKPT",
        [TRAP_TRACE]  = "TRAP_TRACE",
        [TRAP_BRANCH] = "TRAP_BRANCH",
        [TRAP_HWBKPT] = "TRAP_HWBKPT",
        [TRAP_UNK]    = "TRAP_UNK",
        [TRAP_PERF]   = "TRAP_PERF",
};

DEFINE_ਨਿੱਜੀ_STRING_ਸਾਰਣੀ_LOOKUP_ਮੰਜ਼ਿਲ_STRING(sigtrap_ਕੋਡ, ਪੂਰਨ);

ਸਥਿਰ ਅਚਲ ਅੱਖਰ *ਅਚਲ sigsys_ਕੋਡ_ਸਾਰਣੀ[] = {
        [SYS_SECCOMP]       = "SYS_SECCOMP",
        [SYS_USER_DISPATCH] = "SYS_USER_DISPATCH",
};

DEFINE_ਨਿੱਜੀ_STRING_ਸਾਰਣੀ_LOOKUP_ਮੰਜ਼ਿਲ_STRING(sigsys_ਕੋਡ, ਪੂਰਨ);

/* sigchld_code_table is already defined in src/basic/process-util.c with
 * decoded, lower-case values (CLD_EXITED -> "exited"). For consistency with
 * all other values decoded here, provide an uppercase variant. */
ਸਥਿਰ ਅਚਲ ਅੱਖਰ *ਅਚਲ sigchld_uppercase_ਕੋਡ_ਸਾਰਣੀ[] = {
        [CLD_EXITED]    = "CLD_EXITED",
        [CLD_KILLED]    = "CLD_KILLED",
        [CLD_DUMPED]    = "CLD_DUMPED",
        [CLD_TRAPPED]   = "CLD_TRAPPED",
        [CLD_STOPPED]   = "CLD_STOPPED",
        [CLD_CONTINUED] = "CLD_CONTINUED",
};

DEFINE_ਨਿੱਜੀ_STRING_ਸਾਰਣੀ_LOOKUP_ਮੰਜ਼ਿਲ_STRING(sigchld_uppercase_ਕੋਡ, ਪੂਰਨ);

ਸਥਿਰ ਅਚਲ ਅੱਖਰ *ਅਚਲ sigpoll_ਕੋਡ_ਸਾਰਣੀ[] = {
        [POLL_IN]  = "POLL_IN",
        [POLL_OUT] = "POLL_OUT",
        [POLL_MSG] = "POLL_MSG",
        [POLL_ERR] = "POLL_ERR",
        [POLL_PRI] = "POLL_PRI",
        [POLL_HUP] = "POLL_HUP",
};

DEFINE_ਨਿੱਜੀ_STRING_ਸਾਰਣੀ_LOOKUP_ਮੰਜ਼ਿਲ_STRING(sigpoll_ਕੋਡ, ਪੂਰਨ);

ਅਚਲ ਅੱਖਰ* signal_ਕੋਡ_ਮੰਜ਼ਿਲ_string(ਪੂਰਨ signo, ਪੂਰਨ ਕੋਡ) {
        ਚੋਣ (ਕੋਡ) {
        /* SI_KERNEL is 0x80 (128). Defining a table just for SI_KERNEL and
         * SI_USER would be a waste of .rodata space: special case them instead. */
        ਹਾਲਤ SI_KERNEL:
                ਵਾਪਸ "SI_KERNEL";
        ਹਾਲਤ SI_USER:
                ਵਾਪਸ "SI_USER";
        /* The following values are all negatives. SI_ASYNCNL is -60. Similarly
         * to the special case for SI_KERNEL above, avoid using a table for negative
         * values as well. */
        ਹਾਲਤ SI_ASYNCNL:
                ਵਾਪਸ "SI_ASYNCNL";
        ਹਾਲਤ SI_DETHREAD:
                ਵਾਪਸ "SI_DETHREAD";
        ਹਾਲਤ SI_TKILL:
                ਵਾਪਸ "SI_TKILL";
        ਹਾਲਤ SI_SIGIO:
                ਵਾਪਸ "SI_SIGIO";
        ਹਾਲਤ SI_ASYNCIO:
                ਵਾਪਸ "SI_ASYNCIO";
        ਹਾਲਤ SI_MESGQ:
                ਵਾਪਸ "SI_MESGQ";
        ਹਾਲਤ SI_TIMER:
                ਵਾਪਸ "SI_TIMER";
        ਹਾਲਤ SI_QUEUE:
                ਵਾਪਸ "SI_QUEUE";
        }

        ਚੋਣ (signo) {
        ਹਾਲਤ SIGILL:
                ਵਾਪਸ sigill_ਕੋਡ_ਮੰਜ਼ਿਲ_string(ਕੋਡ);
        ਹਾਲਤ SIGFPE:
                ਵਾਪਸ sigfpe_ਕੋਡ_ਮੰਜ਼ਿਲ_string(ਕੋਡ);
        ਹਾਲਤ SIGSEGV:
                ਵਾਪਸ sigsegv_ਕੋਡ_ਮੰਜ਼ਿਲ_string(ਕੋਡ);
        ਹਾਲਤ SIGBUS:
                ਵਾਪਸ sigbus_ਕੋਡ_ਮੰਜ਼ਿਲ_string(ਕੋਡ);
        ਹਾਲਤ SIGTRAP:
                ਵਾਪਸ sigtrap_ਕੋਡ_ਮੰਜ਼ਿਲ_string(ਕੋਡ);
        ਹਾਲਤ SIGCHLD:
                ਵਾਪਸ sigchld_uppercase_ਕੋਡ_ਮੰਜ਼ਿਲ_string(ਕੋਡ);
        ਹਾਲਤ SIGPOLL:
                ਵਾਪਸ sigpoll_ਕੋਡ_ਮੰਜ਼ਿਲ_string(ਕੋਡ);
        ਹਾਲਤ SIGSYS:
                ਵਾਪਸ sigsys_ਕੋਡ_ਮੰਜ਼ਿਲ_string(ਕੋਡ);
        ਮੂਲ:
                ਵਾਪਸ ਨਿਰਾ;
        }
}

ਖਾਲੀ nop_signal_handler(ਪੂਰਨ sig) {
        /* nothing here */
}

ਪੂਰਨ signal_is_blocked(ਪੂਰਨ sig) {
        sigset_t ss;
        ਪੂਰਨ r;

        r = pthread_sigmask(SIG_SETMASK, ਨਿਰਾ, &ss);
        ਜੇ (r != 0)
                ਵਾਪਸ -r;

        ਵਾਪਸ RET_NERRNO(sigismember(&ss, sig));
}

ਪੂਰਨ autoreaping_enabled(ਖਾਲੀ) {
        ਬਣਤਰ sigaction sa;

        ਜੇ (sigaction(SIGCHLD, /* act= */ ਨਿਰਾ, &sa) < 0)
                ਵਾਪਸ -errno;

        ਵਾਪਸ sa.sa_handler == SIG_IGN || FLAGS_ਸੈੱਟ(sa.sa_flags, SA_NOCLDWAIT);
}

ਪੂਰਨ ਕੱਢੋ_pending_signal_ਅੰਦਰੂਨੀ(ਪੂਰਨ sig, ...) {
        sigset_t ss;
        va_list ap;
        ਪੂਰਨ r;

        ਜੇ (sig < 0) /* Empty list? */
                ਵਾਪਸ -EINVAL;

        ਜੇ (sigemptyset(&ss) < 0)
                ਵਾਪਸ -errno;

        /* Add first signal (if the signal is zero, we'll silently skip it, to make it easier to build
         * parameter lists where some element are sometimes off, similar to how sigset_add_many_ap() handles
         * this.) */
        ਜੇ (sig > 0 && sigaddset(&ss, sig) < 0)
                ਵਾਪਸ -errno;

        /* Add all other signals */
        va_start(ap, sig);
        r = sigset_ਜੋੜੋ_many_ap(&ss, ap);
        va_end(ap);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        r = sigtimedwait(&ss, ਨਿਰਾ, &(ਅਚਲ ਬਣਤਰ timespec) {});
        ਜੇ (r < 0) {
                ਜੇ (errno == EAGAIN)
                        ਵਾਪਸ 0;

                ਵਾਪਸ -errno;
        }

        ਵਾਪਸ r; /* Returns the signal popped */
}

ਖਾਲੀ propagate_signal(ਪੂਰਨ sig, siginfo_t *siginfo) {
        pid_t p;

        /* To be called from a signal handler. Will raise the same signal again, in our process + in our threads.
         *
         * Note that we use getpid() instead of getpid_cached(). We might have forked with raw_clone()
         * earlier (see PID 1), and hence let's go to the raw syscall here. In particular as this is not
         * performance sensitive code.
         *
         * Note that we use kill() rather than raise() as fallback, for similar reasons. */

        p = getpid();

        ਜੇ (rt_tgsigqueueinfo(p, gettid(), sig, siginfo) < 0)
                assert_se(kill(p, sig) >= 0);
}

ਅਚਲ ਬਣਤਰ sigaction sigaction_ignore = {
        .sa_handler = SIG_IGN,
        .sa_flags = SA_RESTART,
};

ਅਚਲ ਬਣਤਰ sigaction sigaction_default = {
        .sa_handler = SIG_DFL,
        .sa_flags = SA_RESTART,
};

ਅਚਲ ਬਣਤਰ sigaction sigaction_nop_nocldstop = {
        .sa_handler = nop_signal_handler,
        .sa_flags = SA_NOCLDSTOP|SA_RESTART,
};

ਪੂਰਨ parse_signo(ਅਚਲ ਅੱਖਰ *s, ਪੂਰਨ *ret) {
        ਪੂਰਨ sig, r;

        r = ਸੁਰੱਖਿਅਤ_atoi(s, &sig);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (!SIGNAL_VALID(sig))
                ਵਾਪਸ -EINVAL;

        ਜੇ (ret)
                *ret = sig;

        ਵਾਪਸ 0;
}

ਖਾਲੀ sigterm_process_ਸਮੂਹ_handler(ਪੂਰਨ ਸੰਕੇਤ, siginfo_t *info, ਖਾਲੀ *ucontext) {
        ਪ੍ਰਤਿਜ੍ਞਾ(ਸੰਕੇਤ == SIGTERM);
        ਪ੍ਰਤਿਜ੍ਞਾ(info);

        /* If the sender is not us, propagate the signal to all processes in
         * the same process group */
        ਜੇ (si_ਕੋਡ_from_process(info->si_code) &&
            pid_is_valid(info->si_pid) &&
            info->si_pid != getpid_cached())
                (ਖਾਲੀ) kill(0, ਸੰਕੇਤ);
}
