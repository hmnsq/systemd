/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ <threads.h>
#ଅନ୍ତର୍ଭାଵ <unistd.h>

#ଅନ୍ତର୍ଭାଵ "errno-util.h"
#ଅନ୍ତର୍ଭାଵ "parse-util.h"
#ଅନ୍ତର୍ଭାଵ "process-util.h"
#ଅନ୍ତର୍ଭାଵ "signal-util.h"
#ଅନ୍ତର୍ଭାଵ "stdio-util.h"
#ଅନ୍ତର୍ଭାଵ "string-table.h"
#ଅନ୍ତର୍ଭାଵ "string-util.h"

ପୂର୍ଣ୍ଣସଂଖ୍ୟା reset_all_signal_handlers(ଶୂନ୍ୟ) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = 0, r;

        ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା sig = 1; sig < _NSIG; sig++) {

                /* These two cannot be caught... */
                ଯଦି (IN_SET(sig, SIGKILL, SIGSTOP))
                        ଜାରି;

                /* On Linux the first two RT signals are reserved by glibc, and sigaction() will return
                 * EINVAL for them. */
                r = RET_NERRNO(sigaction(sig, &sigaction_default, ଖାଲି));
                ଯଦି (r != -EINVAL)
                        RET_GATHER(ret, r);
        }

        ଫେରନ୍ତୁ ret;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା reset_signal_mask(ଶୂନ୍ୟ) {
        sigset_t ss;

        ଯଦି (sigemptyset(&ss) < 0)
                ଫେରନ୍ତୁ -errno;

        ଫେରନ୍ତୁ RET_NERRNO(sigprocmask(SIG_SETMASK, &ss, ଖାଲି));
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sigaction_many_internal(ଅଚଳ ସଂରଚନା sigaction *sa, ...) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା sig, r = 0;
        va_list ap;

        va_start(ap, sa);

        /* negative signal ends the list. 0 signal is skipped. */
        ଯେତେବେଳେ ((sig = va_arg(ap, ପୂର୍ଣ୍ଣସଂଖ୍ୟା)) >= 0) {

                ଯଦି (sig == 0)
                        ଜାରି;

                RET_GATHER(r, RET_NERRNO(sigaction(sig, sa, ଖାଲି)));
        }

        va_end(ap);

        ଫେରନ୍ତୁ r;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sigset_add_many_ap(sigset_t *ss, va_list ap) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା sig, r = 0;

        ପ୍ରତିଜ୍ଞା(ss);

        ଯେତେବେଳେ ((sig = va_arg(ap, ପୂର୍ଣ୍ଣସଂଖ୍ୟା)) >= 0) {

                ଯଦି (sig == 0)
                        ଜାରି;

                ଯଦି (sigaddset(ss, sig) < 0) {
                        ଯଦି (r >= 0)
                                r = -errno;
                }
        }

        ଫେରନ୍ତୁ r;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sigset_add_many_internal(sigset_t *ss, ...) {
        va_list ap;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        va_start(ap, ss);
        r = sigset_add_many_ap(ss, ap);
        va_end(ap);

        ଫେରନ୍ତୁ r;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sigprocmask_many_internal(ପୂର୍ଣ୍ଣସଂଖ୍ୟା how, sigset_t *ret_old_mask, ...) {
        va_list ap;
        sigset_t ss;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ଯଦି (sigemptyset(&ss) < 0)
                ଫେରନ୍ତୁ -errno;

        va_start(ap, ret_old_mask);
        r = sigset_add_many_ap(&ss, ap);
        va_end(ap);

        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ RET_NERRNO(sigprocmask(how, &ss, ret_old_mask));
}

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର *ଅଚଳ static_signal_table[] = {
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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP(static_signal, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);

ଅଚଳ ଅକ୍ଷର* signal_to_string(ପୂର୍ଣ୍ଣସଂଖ୍ୟା signo) {
        ସ୍ଥିର thread_local ଅକ୍ଷର buf[STRLEN("RTMIN+") + DECIMAL_STR_MAX(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)];
        ଅଚଳ ଅକ୍ଷର *name;

        name = static_signal_to_string(signo);
        ଯଦି (name)
                ଫେରନ୍ତୁ name;

        ଯଦି (signo >= SIGRTMIN && signo <= SIGRTMAX)
                xsprintf(buf, "RTMIN+%d", signo - SIGRTMIN);
        ଅନ୍ୟଥା
                xsprintf(buf, "%d", signo);

        ଫେରନ୍ତୁ buf;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା signal_from_string(ଅଚଳ ଅକ୍ଷର *s) {
        ଅଚଳ ଅକ୍ଷର *p;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା signo, r;

        /* Check that the input is a signal number. */
        ଯଦି (safe_atoi(s, &signo) >= 0) {
                ଯଦି (SIGNAL_VALID(signo))
                        ଫେରନ୍ତୁ signo;
                ଅନ୍ୟଥା
                        ଫେରନ୍ତୁ -ERANGE;
        }

        /* Drop "SIG" prefix. */
        ଯଦି (startswith(s, "SIG"))
                s += 3;

        /* Check that the input is a signal name. */
        signo = static_signal_from_string(s);
        ଯଦି (signo > 0)
                ଫେରନ୍ତୁ signo;

        /* Check that the input is RTMIN or
         * RTMIN+n (0 <= n <= SIGRTMAX-SIGRTMIN). */
        p = startswith(s, "RTMIN");
        ଯଦି (p) {
                ଯଦି (*p == '\0')
                        ଫେରନ୍ତୁ SIGRTMIN;
                ଯଦି (*p != '+')
                        ଫେରନ୍ତୁ -EINVAL;

                r = safe_atoi(p, &signo);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                ଯଦି (signo < 0 || signo > SIGRTMAX - SIGRTMIN)
                        ଫେରନ୍ତୁ -ERANGE;

                ଫେରନ୍ତୁ signo + SIGRTMIN;
        }

        /* Check that the input is RTMAX or
         * RTMAX-n (0 <= n <= SIGRTMAX-SIGRTMIN). */
        p = startswith(s, "RTMAX");
        ଯଦି (p) {
                ଯଦି (*p == '\0')
                        ଫେରନ୍ତୁ SIGRTMAX;
                ଯଦି (*p != '-')
                        ଫେରନ୍ତୁ -EINVAL;

                r = safe_atoi(p, &signo);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                ଯଦି (signo > 0 || signo < SIGRTMIN - SIGRTMAX)
                        ଫେରନ୍ତୁ -ERANGE;

                ଫେରନ୍ତୁ signo + SIGRTMAX;
        }

        ଫେରନ୍ତୁ -EINVAL;
}

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର *ଅଚଳ sigill_code_table[] = {
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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigill_code, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର *ଅଚଳ sigfpe_code_table[] = {
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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigfpe_code, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର *ଅଚଳ sigsegv_code_table[] = {
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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigsegv_code, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର *ଅଚଳ sigbus_code_table[] = {
        [BUS_ADRALN]    = "BUS_ADRALN",
        [BUS_ADRERR]    = "BUS_ADRERR",
        [BUS_OBJERR]    = "BUS_OBJERR",
        [BUS_MCEERR_AR] = "BUS_MCEERR_AR",
        [BUS_MCEERR_AO] = "BUS_MCEERR_AO",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigbus_code, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର *ଅଚଳ sigtrap_code_table[] = {
        [TRAP_BRKPT]  = "TRAP_BRKPT",
        [TRAP_TRACE]  = "TRAP_TRACE",
        [TRAP_BRANCH] = "TRAP_BRANCH",
        [TRAP_HWBKPT] = "TRAP_HWBKPT",
        [TRAP_UNK]    = "TRAP_UNK",
        [TRAP_PERF]   = "TRAP_PERF",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigtrap_code, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର *ଅଚଳ sigsys_code_table[] = {
        [SYS_SECCOMP]       = "SYS_SECCOMP",
        [SYS_USER_DISPATCH] = "SYS_USER_DISPATCH",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigsys_code, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);

/* sigchld_code_table is already defined in src/basic/process-util.c with
 * decoded, lower-case values (CLD_EXITED -> "exited"). For consistency with
 * all other values decoded here, provide an uppercase variant. */
ସ୍ଥିର ଅଚଳ ଅକ୍ଷର *ଅଚଳ sigchld_uppercase_code_table[] = {
        [CLD_EXITED]    = "CLD_EXITED",
        [CLD_KILLED]    = "CLD_KILLED",
        [CLD_DUMPED]    = "CLD_DUMPED",
        [CLD_TRAPPED]   = "CLD_TRAPPED",
        [CLD_STOPPED]   = "CLD_STOPPED",
        [CLD_CONTINUED] = "CLD_CONTINUED",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigchld_uppercase_code, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର *ଅଚଳ sigpoll_code_table[] = {
        [POLL_IN]  = "POLL_IN",
        [POLL_OUT] = "POLL_OUT",
        [POLL_MSG] = "POLL_MSG",
        [POLL_ERR] = "POLL_ERR",
        [POLL_PRI] = "POLL_PRI",
        [POLL_HUP] = "POLL_HUP",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigpoll_code, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);

ଅଚଳ ଅକ୍ଷର* signal_code_to_string(ପୂର୍ଣ୍ଣସଂଖ୍ୟା signo, ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) {
        ବାଛ (code) {
        /* SI_KERNEL is 0x80 (128). Defining a table just for SI_KERNEL and
         * SI_USER would be a waste of .rodata space: special case them instead. */
        ସ୍ଥିତି SI_KERNEL:
                ଫେରନ୍ତୁ "SI_KERNEL";
        ସ୍ଥିତି SI_USER:
                ଫେରନ୍ତୁ "SI_USER";
        /* The following values are all negatives. SI_ASYNCNL is -60. Similarly
         * to the special case for SI_KERNEL above, avoid using a table for negative
         * values as well. */
        ସ୍ଥିତି SI_ASYNCNL:
                ଫେରନ୍ତୁ "SI_ASYNCNL";
        ସ୍ଥିତି SI_DETHREAD:
                ଫେରନ୍ତୁ "SI_DETHREAD";
        ସ୍ଥିତି SI_TKILL:
                ଫେରନ୍ତୁ "SI_TKILL";
        ସ୍ଥିତି SI_SIGIO:
                ଫେରନ୍ତୁ "SI_SIGIO";
        ସ୍ଥିତି SI_ASYNCIO:
                ଫେରନ୍ତୁ "SI_ASYNCIO";
        ସ୍ଥିତି SI_MESGQ:
                ଫେରନ୍ତୁ "SI_MESGQ";
        ସ୍ଥିତି SI_TIMER:
                ଫେରନ୍ତୁ "SI_TIMER";
        ସ୍ଥିତି SI_QUEUE:
                ଫେରନ୍ତୁ "SI_QUEUE";
        }

        ବାଛ (signo) {
        ସ୍ଥିତି SIGILL:
                ଫେରନ୍ତୁ sigill_code_to_string(code);
        ସ୍ଥିତି SIGFPE:
                ଫେରନ୍ତୁ sigfpe_code_to_string(code);
        ସ୍ଥିତି SIGSEGV:
                ଫେରନ୍ତୁ sigsegv_code_to_string(code);
        ସ୍ଥିତି SIGBUS:
                ଫେରନ୍ତୁ sigbus_code_to_string(code);
        ସ୍ଥିତି SIGTRAP:
                ଫେରନ୍ତୁ sigtrap_code_to_string(code);
        ସ୍ଥିତି SIGCHLD:
                ଫେରନ୍ତୁ sigchld_uppercase_code_to_string(code);
        ସ୍ଥିତି SIGPOLL:
                ଫେରନ୍ତୁ sigpoll_code_to_string(code);
        ସ୍ଥିତି SIGSYS:
                ଫେରନ୍ତୁ sigsys_code_to_string(code);
        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                ଫେରନ୍ତୁ ଖାଲି;
        }
}

ଶୂନ୍ୟ nop_signal_handler(ପୂର୍ଣ୍ଣସଂଖ୍ୟା sig) {
        /* nothing here */
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା signal_is_blocked(ପୂର୍ଣ୍ଣସଂଖ୍ୟା sig) {
        sigset_t ss;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        r = pthread_sigmask(SIG_SETMASK, ଖାଲି, &ss);
        ଯଦି (r != 0)
                ଫେରନ୍ତୁ -r;

        ଫେରନ୍ତୁ RET_NERRNO(sigismember(&ss, sig));
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା autoreaping_enabled(ଶୂନ୍ୟ) {
        ସଂରଚନା sigaction sa;

        ଯଦି (sigaction(SIGCHLD, /* act= */ ଖାଲି, &sa) < 0)
                ଫେରନ୍ତୁ -errno;

        ଫେରନ୍ତୁ sa.sa_handler == SIG_IGN || FLAGS_SET(sa.sa_flags, SA_NOCLDWAIT);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା କାଢ଼_pending_signal_internal(ପୂର୍ଣ୍ଣସଂଖ୍ୟା sig, ...) {
        sigset_t ss;
        va_list ap;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ଯଦି (sig < 0) /* Empty list? */
                ଫେରନ୍ତୁ -EINVAL;

        ଯଦି (sigemptyset(&ss) < 0)
                ଫେରନ୍ତୁ -errno;

        /* Add first signal (if the signal is zero, we'll silently skip it, to make it easier to build
         * parameter lists where some element are sometimes off, similar to how sigset_add_many_ap() handles
         * this.) */
        ଯଦି (sig > 0 && sigaddset(&ss, sig) < 0)
                ଫେରନ୍ତୁ -errno;

        /* Add all other signals */
        va_start(ap, sig);
        r = sigset_add_many_ap(&ss, ap);
        va_end(ap);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        r = sigtimedwait(&ss, ଖାଲି, &(ଅଚଳ ସଂରଚନା timespec) {});
        ଯଦି (r < 0) {
                ଯଦି (errno == EAGAIN)
                        ଫେରନ୍ତୁ 0;

                ଫେରନ୍ତୁ -errno;
        }

        ଫେରନ୍ତୁ r; /* Returns the signal popped */
}

ଶୂନ୍ୟ propagate_signal(ପୂର୍ଣ୍ଣସଂଖ୍ୟା sig, siginfo_t *siginfo) {
        pid_t p;

        /* To be called from a signal handler. Will raise the same signal again, in our process + in our threads.
         *
         * Note that we use getpid() instead of getpid_cached(). We might have forked with raw_clone()
         * earlier (see PID 1), and hence let's go to the raw syscall here. In particular as this is not
         * performance sensitive code.
         *
         * Note that we use kill() rather than raise() as fallback, for similar reasons. */

        p = getpid();

        ଯଦି (rt_tgsigqueueinfo(p, gettid(), sig, siginfo) < 0)
                assert_se(kill(p, sig) >= 0);
}

ଅଚଳ ସଂରଚନା sigaction sigaction_ignore = {
        .sa_handler = SIG_IGN,
        .sa_flags = SA_RESTART,
};

ଅଚଳ ସଂରଚନା sigaction sigaction_default = {
        .sa_handler = SIG_DFL,
        .sa_flags = SA_RESTART,
};

ଅଚଳ ସଂରଚନା sigaction sigaction_nop_nocldstop = {
        .sa_handler = nop_signal_handler,
        .sa_flags = SA_NOCLDSTOP|SA_RESTART,
};

ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_signo(ଅଚଳ ଅକ୍ଷର *s, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ret) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା sig, r;

        r = safe_atoi(s, &sig);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (!SIGNAL_VALID(sig))
                ଫେରନ୍ତୁ -EINVAL;

        ଯଦି (ret)
                *ret = sig;

        ଫେରନ୍ତୁ 0;
}

ଶୂନ୍ୟ sigterm_process_group_handler(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ସଙ୍କେତ, siginfo_t *info, ଶୂନ୍ୟ *ucontext) {
        ପ୍ରତିଜ୍ଞା(ସଙ୍କେତ == SIGTERM);
        ପ୍ରତିଜ୍ଞା(info);

        /* If the sender is not us, propagate the signal to all processes in
         * the same process group */
        ଯଦି (si_code_from_process(info->si_code) &&
            pid_is_valid(info->si_pid) &&
            info->si_pid != getpid_cached())
                (ଶୂନ୍ୟ) kill(0, ସଙ୍କେତ);
}
