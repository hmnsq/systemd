/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <threads.h>
#ضمن <unistd.h>

#ضمن "errno-util.h"
#ضمن "parse-util.h"
#ضمن "process-util.h"
#ضمن "signal-util.h"
#ضمن "stdio-util.h"
#ضمن "string-table.h"
#ضمن "string-util.h"

صحيح reset_all_signal_handlers(فراغ) {
        صحيح ret = 0, r;

        لكل (صحيح sig = 1; sig < _NSIG; sig++) {

                /* These two cannot be caught... */
                إذا (IN_SET(sig, SIGKILL, SIGSTOP))
                        استمر;

                /* On Linux the first two RT signals are reserved by glibc, and sigaction() will return
                 * EINVAL for them. */
                r = RET_NERRNO(sigaction(sig, &sigaction_default, عدم));
                إذا (r != -EINVAL)
                        RET_GATHER(ret, r);
        }

        أرجع ret;
}

صحيح reset_signal_mask(فراغ) {
        sigset_t ss;

        إذا (sigemptyset(&ss) < 0)
                أرجع -errno;

        أرجع RET_NERRNO(sigprocmask(SIG_SETMASK, &ss, عدم));
}

صحيح sigaction_many_internal(ثابت بنية sigaction *sa, ...) {
        صحيح sig, r = 0;
        va_list ap;

        va_start(ap, sa);

        /* negative signal ends the list. 0 signal is skipped. */
        طالما ((sig = va_arg(ap, صحيح)) >= 0) {

                إذا (sig == 0)
                        استمر;

                RET_GATHER(r, RET_NERRNO(sigaction(sig, sa, عدم)));
        }

        va_end(ap);

        أرجع r;
}

ساكن صحيح sigset_add_many_ap(sigset_t *ss, va_list ap) {
        صحيح sig, r = 0;

        أكد(ss);

        طالما ((sig = va_arg(ap, صحيح)) >= 0) {

                إذا (sig == 0)
                        استمر;

                إذا (sigaddset(ss, sig) < 0) {
                        إذا (r >= 0)
                                r = -errno;
                }
        }

        أرجع r;
}

صحيح sigset_add_many_internal(sigset_t *ss, ...) {
        va_list ap;
        صحيح r;

        va_start(ap, ss);
        r = sigset_add_many_ap(ss, ap);
        va_end(ap);

        أرجع r;
}

صحيح sigprocmask_many_internal(صحيح how, sigset_t *ret_old_mask, ...) {
        va_list ap;
        sigset_t ss;
        صحيح r;

        إذا (sigemptyset(&ss) < 0)
                أرجع -errno;

        va_start(ap, ret_old_mask);
        r = sigset_add_many_ap(&ss, ap);
        va_end(ap);

        إذا (r < 0)
                أرجع r;

        أرجع RET_NERRNO(sigprocmask(how, &ss, ret_old_mask));
}

ساكن ثابت حرف *ثابت static_signal_table[] = {
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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP(static_signal, صحيح);

ثابت حرف* signal_to_string(صحيح signo) {
        ساكن thread_local حرف buf[STRLEN("RTMIN+") + DECIMAL_STR_MAX(صحيح)];
        ثابت حرف *name;

        name = static_signal_to_string(signo);
        إذا (name)
                أرجع name;

        إذا (signo >= SIGRTMIN && signo <= SIGRTMAX)
                xsprintf(buf, "RTMIN+%d", signo - SIGRTMIN);
        وإلا
                xsprintf(buf, "%d", signo);

        أرجع buf;
}

صحيح signal_from_string(ثابت حرف *s) {
        ثابت حرف *p;
        صحيح signo, r;

        /* Check that the input is a signal number. */
        إذا (safe_atoi(s, &signo) >= 0) {
                إذا (SIGNAL_VALID(signo))
                        أرجع signo;
                وإلا
                        أرجع -ERANGE;
        }

        /* Drop "SIG" prefix. */
        إذا (startswith(s, "SIG"))
                s += 3;

        /* Check that the input is a signal name. */
        signo = static_signal_from_string(s);
        إذا (signo > 0)
                أرجع signo;

        /* Check that the input is RTMIN or
         * RTMIN+n (0 <= n <= SIGRTMAX-SIGRTMIN). */
        p = startswith(s, "RTMIN");
        إذا (p) {
                إذا (*p == '\0')
                        أرجع SIGRTMIN;
                إذا (*p != '+')
                        أرجع -EINVAL;

                r = safe_atoi(p, &signo);
                إذا (r < 0)
                        أرجع r;

                إذا (signo < 0 || signo > SIGRTMAX - SIGRTMIN)
                        أرجع -ERANGE;

                أرجع signo + SIGRTMIN;
        }

        /* Check that the input is RTMAX or
         * RTMAX-n (0 <= n <= SIGRTMAX-SIGRTMIN). */
        p = startswith(s, "RTMAX");
        إذا (p) {
                إذا (*p == '\0')
                        أرجع SIGRTMAX;
                إذا (*p != '-')
                        أرجع -EINVAL;

                r = safe_atoi(p, &signo);
                إذا (r < 0)
                        أرجع r;

                إذا (signo > 0 || signo < SIGRTMIN - SIGRTMAX)
                        أرجع -ERANGE;

                أرجع signo + SIGRTMAX;
        }

        أرجع -EINVAL;
}

ساكن ثابت حرف *ثابت sigill_code_table[] = {
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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigill_code, صحيح);

ساكن ثابت حرف *ثابت sigfpe_code_table[] = {
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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigfpe_code, صحيح);

ساكن ثابت حرف *ثابت sigsegv_code_table[] = {
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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigsegv_code, صحيح);

ساكن ثابت حرف *ثابت sigbus_code_table[] = {
        [BUS_ADRALN]    = "BUS_ADRALN",
        [BUS_ADRERR]    = "BUS_ADRERR",
        [BUS_OBJERR]    = "BUS_OBJERR",
        [BUS_MCEERR_AR] = "BUS_MCEERR_AR",
        [BUS_MCEERR_AO] = "BUS_MCEERR_AO",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigbus_code, صحيح);

ساكن ثابت حرف *ثابت sigtrap_code_table[] = {
        [TRAP_BRKPT]  = "TRAP_BRKPT",
        [TRAP_TRACE]  = "TRAP_TRACE",
        [TRAP_BRANCH] = "TRAP_BRANCH",
        [TRAP_HWBKPT] = "TRAP_HWBKPT",
        [TRAP_UNK]    = "TRAP_UNK",
        [TRAP_PERF]   = "TRAP_PERF",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigtrap_code, صحيح);

ساكن ثابت حرف *ثابت sigsys_code_table[] = {
        [SYS_SECCOMP]       = "SYS_SECCOMP",
        [SYS_USER_DISPATCH] = "SYS_USER_DISPATCH",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigsys_code, صحيح);

/* sigchld_code_table is already defined in src/basic/process-util.c with
 * decoded, lower-case values (CLD_EXITED -> "exited"). For consistency with
 * all other values decoded here, provide an uppercase variant. */
ساكن ثابت حرف *ثابت sigchld_uppercase_code_table[] = {
        [CLD_EXITED]    = "CLD_EXITED",
        [CLD_KILLED]    = "CLD_KILLED",
        [CLD_DUMPED]    = "CLD_DUMPED",
        [CLD_TRAPPED]   = "CLD_TRAPPED",
        [CLD_STOPPED]   = "CLD_STOPPED",
        [CLD_CONTINUED] = "CLD_CONTINUED",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigchld_uppercase_code, صحيح);

ساكن ثابت حرف *ثابت sigpoll_code_table[] = {
        [POLL_IN]  = "POLL_IN",
        [POLL_OUT] = "POLL_OUT",
        [POLL_MSG] = "POLL_MSG",
        [POLL_ERR] = "POLL_ERR",
        [POLL_PRI] = "POLL_PRI",
        [POLL_HUP] = "POLL_HUP",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigpoll_code, صحيح);

ثابت حرف* signal_code_to_string(صحيح signo, صحيح code) {
        بدل (code) {
        /* SI_KERNEL is 0x80 (128). Defining a table just for SI_KERNEL and
         * SI_USER would be a waste of .rodata space: special case them instead. */
        حالة SI_KERNEL:
                أرجع "SI_KERNEL";
        حالة SI_USER:
                أرجع "SI_USER";
        /* The following values are all negatives. SI_ASYNCNL is -60. Similarly
         * to the special case for SI_KERNEL above, avoid using a table for negative
         * values as well. */
        حالة SI_ASYNCNL:
                أرجع "SI_ASYNCNL";
        حالة SI_DETHREAD:
                أرجع "SI_DETHREAD";
        حالة SI_TKILL:
                أرجع "SI_TKILL";
        حالة SI_SIGIO:
                أرجع "SI_SIGIO";
        حالة SI_ASYNCIO:
                أرجع "SI_ASYNCIO";
        حالة SI_MESGQ:
                أرجع "SI_MESGQ";
        حالة SI_TIMER:
                أرجع "SI_TIMER";
        حالة SI_QUEUE:
                أرجع "SI_QUEUE";
        }

        بدل (signo) {
        حالة SIGILL:
                أرجع sigill_code_to_string(code);
        حالة SIGFPE:
                أرجع sigfpe_code_to_string(code);
        حالة SIGSEGV:
                أرجع sigsegv_code_to_string(code);
        حالة SIGBUS:
                أرجع sigbus_code_to_string(code);
        حالة SIGTRAP:
                أرجع sigtrap_code_to_string(code);
        حالة SIGCHLD:
                أرجع sigchld_uppercase_code_to_string(code);
        حالة SIGPOLL:
                أرجع sigpoll_code_to_string(code);
        حالة SIGSYS:
                أرجع sigsys_code_to_string(code);
        افتراضي:
                أرجع عدم;
        }
}

فراغ nop_signal_handler(صحيح sig) {
        /* nothing here */
}

صحيح signal_is_blocked(صحيح sig) {
        sigset_t ss;
        صحيح r;

        r = pthread_sigmask(SIG_SETMASK, عدم, &ss);
        إذا (r != 0)
                أرجع -r;

        أرجع RET_NERRNO(sigismember(&ss, sig));
}

صحيح autoreaping_enabled(فراغ) {
        بنية sigaction sa;

        إذا (sigaction(SIGCHLD, /* act= */ عدم, &sa) < 0)
                أرجع -errno;

        أرجع sa.sa_handler == SIG_IGN || FLAGS_SET(sa.sa_flags, SA_NOCLDWAIT);
}

صحيح نکالو_pending_signal_internal(صحيح sig, ...) {
        sigset_t ss;
        va_list ap;
        صحيح r;

        إذا (sig < 0) /* Empty list? */
                أرجع -EINVAL;

        إذا (sigemptyset(&ss) < 0)
                أرجع -errno;

        /* Add first signal (if the signal is zero, we'll silently skip it, to make it easier to build
         * parameter lists where some element are sometimes off, similar to how sigset_add_many_ap() handles
         * this.) */
        إذا (sig > 0 && sigaddset(&ss, sig) < 0)
                أرجع -errno;

        /* Add all other signals */
        va_start(ap, sig);
        r = sigset_add_many_ap(&ss, ap);
        va_end(ap);
        إذا (r < 0)
                أرجع r;

        r = sigtimedwait(&ss, عدم, &(ثابت بنية timespec) {});
        إذا (r < 0) {
                إذا (errno == EAGAIN)
                        أرجع 0;

                أرجع -errno;
        }

        أرجع r; /* Returns the signal popped */
}

فراغ propagate_signal(صحيح sig, siginfo_t *siginfo) {
        pid_t p;

        /* To be called from a signal handler. Will raise the same signal again, in our process + in our threads.
         *
         * Note that we use getpid() instead of getpid_cached(). We might have forked with raw_clone()
         * earlier (see PID 1), and hence let's go to the raw syscall here. In particular as this is not
         * performance sensitive code.
         *
         * Note that we use kill() rather than raise() as fallback, for similar reasons. */

        p = getpid();

        إذا (rt_tgsigqueueinfo(p, gettid(), sig, siginfo) < 0)
                assert_se(kill(p, sig) >= 0);
}

ثابت بنية sigaction sigaction_ignore = {
        .sa_handler = SIG_IGN,
        .sa_flags = SA_RESTART,
};

ثابت بنية sigaction sigaction_default = {
        .sa_handler = SIG_DFL,
        .sa_flags = SA_RESTART,
};

ثابت بنية sigaction sigaction_nop_nocldstop = {
        .sa_handler = nop_signal_handler,
        .sa_flags = SA_NOCLDSTOP|SA_RESTART,
};

صحيح parse_signo(ثابت حرف *s, صحيح *ret) {
        صحيح sig, r;

        r = safe_atoi(s, &sig);
        إذا (r < 0)
                أرجع r;

        إذا (!SIGNAL_VALID(sig))
                أرجع -EINVAL;

        إذا (ret)
                *ret = sig;

        أرجع 0;
}

فراغ sigterm_process_group_handler(صحيح إشارة, siginfo_t *info, فراغ *ucontext) {
        أكد(إشارة == SIGTERM);
        أكد(info);

        /* If the sender is not us, propagate the signal to all processes in
         * the same process group */
        إذا (si_code_from_process(info->si_code) &&
            pid_is_valid(info->si_pid) &&
            info->si_pid != getpid_cached())
                (فراغ) kill(0, إشارة);
}
