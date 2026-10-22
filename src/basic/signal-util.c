/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ <threads.h>
#અન્તર્ભાવ <unistd.h>

#અન્તર્ભાવ "errno-util.h"
#અન્તર્ભાવ "parse-util.h"
#અન્તર્ભાવ "process-util.h"
#અન્તર્ભાવ "signal-util.h"
#અન્તર્ભાવ "stdio-util.h"
#અન્તર્ભાવ "string-table.h"
#અન્તર્ભાવ "string-util.h"

પૂર્ણાંક reset_all_signal_handlers(શૂન્ય) {
        પૂર્ણાંક ret = 0, r;

        માટે (પૂર્ણાંક sig = 1; sig < _NSIG; sig++) {

                /* These two cannot be caught... */
                જો (IN_SET(sig, SIGKILL, SIGSTOP))
                        ચાલુ;

                /* On Linux the first two RT signals are reserved by glibc, and sigaction() will return
                 * EINVAL for them. */
                r = RET_NERRNO(sigaction(sig, &sigaction_default, ખાલી));
                જો (r != -EINVAL)
                        RET_GATHER(ret, r);
        }

        પાછા ret;
}

પૂર્ણાંક reset_signal_mask(શૂન્ય) {
        sigset_t ss;

        જો (sigemptyset(&ss) < 0)
                પાછા -errno;

        પાછા RET_NERRNO(sigprocmask(SIG_SETMASK, &ss, ખાલી));
}

પૂર્ણાંક sigaction_many_internal(અચલ સંરચના sigaction *sa, ...) {
        પૂર્ણાંક sig, r = 0;
        va_list ap;

        va_start(ap, sa);

        /* negative signal ends the list. 0 signal is skipped. */
        જ્યારે ((sig = va_arg(ap, પૂર્ણાંક)) >= 0) {

                જો (sig == 0)
                        ચાલુ;

                RET_GATHER(r, RET_NERRNO(sigaction(sig, sa, ખાલી)));
        }

        va_end(ap);

        પાછા r;
}

સ્થિર પૂર્ણાંક sigset_add_many_ap(sigset_t *ss, va_list ap) {
        પૂર્ણાંક sig, r = 0;

        પ્રતિજ્ઞા(ss);

        જ્યારે ((sig = va_arg(ap, પૂર્ણાંક)) >= 0) {

                જો (sig == 0)
                        ચાલુ;

                જો (sigaddset(ss, sig) < 0) {
                        જો (r >= 0)
                                r = -errno;
                }
        }

        પાછા r;
}

પૂર્ણાંક sigset_add_many_internal(sigset_t *ss, ...) {
        va_list ap;
        પૂર્ણાંક r;

        va_start(ap, ss);
        r = sigset_add_many_ap(ss, ap);
        va_end(ap);

        પાછા r;
}

પૂર્ણાંક sigprocmask_many_internal(પૂર્ણાંક how, sigset_t *ret_old_mask, ...) {
        va_list ap;
        sigset_t ss;
        પૂર્ણાંક r;

        જો (sigemptyset(&ss) < 0)
                પાછા -errno;

        va_start(ap, ret_old_mask);
        r = sigset_add_many_ap(&ss, ap);
        va_end(ap);

        જો (r < 0)
                પાછા r;

        પાછા RET_NERRNO(sigprocmask(how, &ss, ret_old_mask));
}

સ્થિર અચલ અક્ષર *અચલ static_signal_table[] = {
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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP(static_signal, પૂર્ણાંક);

અચલ અક્ષર* signal_to_string(પૂર્ણાંક signo) {
        સ્થિર thread_local અક્ષર buf[STRLEN("RTMIN+") + DECIMAL_STR_MAX(પૂર્ણાંક)];
        અચલ અક્ષર *name;

        name = static_signal_to_string(signo);
        જો (name)
                પાછા name;

        જો (signo >= SIGRTMIN && signo <= SIGRTMAX)
                xsprintf(buf, "RTMIN+%d", signo - SIGRTMIN);
        નહીંતર
                xsprintf(buf, "%d", signo);

        પાછા buf;
}

પૂર્ણાંક signal_from_string(અચલ અક્ષર *s) {
        અચલ અક્ષર *p;
        પૂર્ણાંક signo, r;

        /* Check that the input is a signal number. */
        જો (safe_atoi(s, &signo) >= 0) {
                જો (SIGNAL_VALID(signo))
                        પાછા signo;
                નહીંતર
                        પાછા -ERANGE;
        }

        /* Drop "SIG" prefix. */
        જો (startswith(s, "SIG"))
                s += 3;

        /* Check that the input is a signal name. */
        signo = static_signal_from_string(s);
        જો (signo > 0)
                પાછા signo;

        /* Check that the input is RTMIN or
         * RTMIN+n (0 <= n <= SIGRTMAX-SIGRTMIN). */
        p = startswith(s, "RTMIN");
        જો (p) {
                જો (*p == '\0')
                        પાછા SIGRTMIN;
                જો (*p != '+')
                        પાછા -EINVAL;

                r = safe_atoi(p, &signo);
                જો (r < 0)
                        પાછા r;

                જો (signo < 0 || signo > SIGRTMAX - SIGRTMIN)
                        પાછા -ERANGE;

                પાછા signo + SIGRTMIN;
        }

        /* Check that the input is RTMAX or
         * RTMAX-n (0 <= n <= SIGRTMAX-SIGRTMIN). */
        p = startswith(s, "RTMAX");
        જો (p) {
                જો (*p == '\0')
                        પાછા SIGRTMAX;
                જો (*p != '-')
                        પાછા -EINVAL;

                r = safe_atoi(p, &signo);
                જો (r < 0)
                        પાછા r;

                જો (signo > 0 || signo < SIGRTMIN - SIGRTMAX)
                        પાછા -ERANGE;

                પાછા signo + SIGRTMAX;
        }

        પાછા -EINVAL;
}

સ્થિર અચલ અક્ષર *અચલ sigill_code_table[] = {
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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigill_code, પૂર્ણાંક);

સ્થિર અચલ અક્ષર *અચલ sigfpe_code_table[] = {
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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigfpe_code, પૂર્ણાંક);

સ્થિર અચલ અક્ષર *અચલ sigsegv_code_table[] = {
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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigsegv_code, પૂર્ણાંક);

સ્થિર અચલ અક્ષર *અચલ sigbus_code_table[] = {
        [BUS_ADRALN]    = "BUS_ADRALN",
        [BUS_ADRERR]    = "BUS_ADRERR",
        [BUS_OBJERR]    = "BUS_OBJERR",
        [BUS_MCEERR_AR] = "BUS_MCEERR_AR",
        [BUS_MCEERR_AO] = "BUS_MCEERR_AO",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigbus_code, પૂર્ણાંક);

સ્થિર અચલ અક્ષર *અચલ sigtrap_code_table[] = {
        [TRAP_BRKPT]  = "TRAP_BRKPT",
        [TRAP_TRACE]  = "TRAP_TRACE",
        [TRAP_BRANCH] = "TRAP_BRANCH",
        [TRAP_HWBKPT] = "TRAP_HWBKPT",
        [TRAP_UNK]    = "TRAP_UNK",
        [TRAP_PERF]   = "TRAP_PERF",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigtrap_code, પૂર્ણાંક);

સ્થિર અચલ અક્ષર *અચલ sigsys_code_table[] = {
        [SYS_SECCOMP]       = "SYS_SECCOMP",
        [SYS_USER_DISPATCH] = "SYS_USER_DISPATCH",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigsys_code, પૂર્ણાંક);

/* sigchld_code_table is already defined in src/basic/process-util.c with
 * decoded, lower-case values (CLD_EXITED -> "exited"). For consistency with
 * all other values decoded here, provide an uppercase variant. */
સ્થિર અચલ અક્ષર *અચલ sigchld_uppercase_code_table[] = {
        [CLD_EXITED]    = "CLD_EXITED",
        [CLD_KILLED]    = "CLD_KILLED",
        [CLD_DUMPED]    = "CLD_DUMPED",
        [CLD_TRAPPED]   = "CLD_TRAPPED",
        [CLD_STOPPED]   = "CLD_STOPPED",
        [CLD_CONTINUED] = "CLD_CONTINUED",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigchld_uppercase_code, પૂર્ણાંક);

સ્થિર અચલ અક્ષર *અચલ sigpoll_code_table[] = {
        [POLL_IN]  = "POLL_IN",
        [POLL_OUT] = "POLL_OUT",
        [POLL_MSG] = "POLL_MSG",
        [POLL_ERR] = "POLL_ERR",
        [POLL_PRI] = "POLL_PRI",
        [POLL_HUP] = "POLL_HUP",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigpoll_code, પૂર્ણાંક);

અચલ અક્ષર* signal_code_to_string(પૂર્ણાંક signo, પૂર્ણાંક code) {
        પસંદગી (code) {
        /* SI_KERNEL is 0x80 (128). Defining a table just for SI_KERNEL and
         * SI_USER would be a waste of .rodata space: special case them instead. */
        સ્થિતિ SI_KERNEL:
                પાછા "SI_KERNEL";
        સ્થિતિ SI_USER:
                પાછા "SI_USER";
        /* The following values are all negatives. SI_ASYNCNL is -60. Similarly
         * to the special case for SI_KERNEL above, avoid using a table for negative
         * values as well. */
        સ્થિતિ SI_ASYNCNL:
                પાછા "SI_ASYNCNL";
        સ્થિતિ SI_DETHREAD:
                પાછા "SI_DETHREAD";
        સ્થિતિ SI_TKILL:
                પાછા "SI_TKILL";
        સ્થિતિ SI_SIGIO:
                પાછા "SI_SIGIO";
        સ્થિતિ SI_ASYNCIO:
                પાછા "SI_ASYNCIO";
        સ્થિતિ SI_MESGQ:
                પાછા "SI_MESGQ";
        સ્થિતિ SI_TIMER:
                પાછા "SI_TIMER";
        સ્થિતિ SI_QUEUE:
                પાછા "SI_QUEUE";
        }

        પસંદગી (signo) {
        સ્થિતિ SIGILL:
                પાછા sigill_code_to_string(code);
        સ્થિતિ SIGFPE:
                પાછા sigfpe_code_to_string(code);
        સ્થિતિ SIGSEGV:
                પાછા sigsegv_code_to_string(code);
        સ્થિતિ SIGBUS:
                પાછા sigbus_code_to_string(code);
        સ્થિતિ SIGTRAP:
                પાછા sigtrap_code_to_string(code);
        સ્થિતિ SIGCHLD:
                પાછા sigchld_uppercase_code_to_string(code);
        સ્થિતિ SIGPOLL:
                પાછા sigpoll_code_to_string(code);
        સ્થિતિ SIGSYS:
                પાછા sigsys_code_to_string(code);
        મૂળભૂત:
                પાછા ખાલી;
        }
}

શૂન્ય nop_signal_handler(પૂર્ણાંક sig) {
        /* nothing here */
}

પૂર્ણાંક signal_is_blocked(પૂર્ણાંક sig) {
        sigset_t ss;
        પૂર્ણાંક r;

        r = pthread_sigmask(SIG_SETMASK, ખાલી, &ss);
        જો (r != 0)
                પાછા -r;

        પાછા RET_NERRNO(sigismember(&ss, sig));
}

પૂર્ણાંક autoreaping_enabled(શૂન્ય) {
        સંરચના sigaction sa;

        જો (sigaction(SIGCHLD, /* act= */ ખાલી, &sa) < 0)
                પાછા -errno;

        પાછા sa.sa_handler == SIG_IGN || FLAGS_SET(sa.sa_flags, SA_NOCLDWAIT);
}

પૂર્ણાંક કાઢો_pending_signal_internal(પૂર્ણાંક sig, ...) {
        sigset_t ss;
        va_list ap;
        પૂર્ણાંક r;

        જો (sig < 0) /* Empty list? */
                પાછા -EINVAL;

        જો (sigemptyset(&ss) < 0)
                પાછા -errno;

        /* Add first signal (if the signal is zero, we'll silently skip it, to make it easier to build
         * parameter lists where some element are sometimes off, similar to how sigset_add_many_ap() handles
         * this.) */
        જો (sig > 0 && sigaddset(&ss, sig) < 0)
                પાછા -errno;

        /* Add all other signals */
        va_start(ap, sig);
        r = sigset_add_many_ap(&ss, ap);
        va_end(ap);
        જો (r < 0)
                પાછા r;

        r = sigtimedwait(&ss, ખાલી, &(અચલ સંરચના timespec) {});
        જો (r < 0) {
                જો (errno == EAGAIN)
                        પાછા 0;

                પાછા -errno;
        }

        પાછા r; /* Returns the signal popped */
}

શૂન્ય propagate_signal(પૂર્ણાંક sig, siginfo_t *siginfo) {
        pid_t p;

        /* To be called from a signal handler. Will raise the same signal again, in our process + in our threads.
         *
         * Note that we use getpid() instead of getpid_cached(). We might have forked with raw_clone()
         * earlier (see PID 1), and hence let's go to the raw syscall here. In particular as this is not
         * performance sensitive code.
         *
         * Note that we use kill() rather than raise() as fallback, for similar reasons. */

        p = getpid();

        જો (rt_tgsigqueueinfo(p, gettid(), sig, siginfo) < 0)
                assert_se(kill(p, sig) >= 0);
}

અચલ સંરચના sigaction sigaction_ignore = {
        .sa_handler = SIG_IGN,
        .sa_flags = SA_RESTART,
};

અચલ સંરચના sigaction sigaction_default = {
        .sa_handler = SIG_DFL,
        .sa_flags = SA_RESTART,
};

અચલ સંરચના sigaction sigaction_nop_nocldstop = {
        .sa_handler = nop_signal_handler,
        .sa_flags = SA_NOCLDSTOP|SA_RESTART,
};

પૂર્ણાંક parse_signo(અચલ અક્ષર *s, પૂર્ણાંક *ret) {
        પૂર્ણાંક sig, r;

        r = safe_atoi(s, &sig);
        જો (r < 0)
                પાછા r;

        જો (!SIGNAL_VALID(sig))
                પાછા -EINVAL;

        જો (ret)
                *ret = sig;

        પાછા 0;
}

શૂન્ય sigterm_process_group_handler(પૂર્ણાંક સંકેત, siginfo_t *info, શૂન્ય *ucontext) {
        પ્રતિજ્ઞા(સંકેત == SIGTERM);
        પ્રતિજ્ઞા(info);

        /* If the sender is not us, propagate the signal to all processes in
         * the same process group */
        જો (si_code_from_process(info->si_code) &&
            pid_is_valid(info->si_pid) &&
            info->si_pid != getpid_cached())
                (શૂન્ય) kill(0, સંકેત);
}
