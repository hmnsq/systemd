/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <threads.h>
#شامل <unistd.h>

#شامل "errno-util.h"
#شامل "parse-util.h"
#شامل "process-util.h"
#شامل "signal-util.h"
#شامل "stdio-util.h"
#شامل "string-table.h"
#شامل "string-util.h"

صحیح reset_all_signal_handlers(خالی) {
        صحیح ret = 0, r;

        برائے (صحیح sig = 1; sig < _NSIG; sig++) {

                /* These two cannot be caught... */
                اگر (اندر_سیٹ(sig, SIGKILL, SIGSTOP))
                        جاری;

                /* On Linux the first two RT signals are reserved by glibc, and sigaction() will return
                 * EINVAL for them. */
                r = RET_NERRNO(sigaction(sig, &sigaction_default, تہی));
                اگر (r != -EINVAL)
                        RET_GATHER(ret, r);
        }

        واپس ret;
}

صحیح reset_signal_نقاب(خالی) {
        sigset_t ss;

        اگر (sigemptyset(&ss) < 0)
                واپس -errno;

        واپس RET_NERRNO(sigprocmask(SIG_SETMASK, &ss, تہی));
}

صحیح sigaction_many_اندرونی(ثابت ساخت sigaction *sa, ...) {
        صحیح sig, r = 0;
        va_list ap;

        va_start(ap, sa);

        /* negative signal ends the list. 0 signal is skipped. */
        جب ((sig = va_arg(ap, صحیح)) >= 0) {

                اگر (sig == 0)
                        جاری;

                RET_GATHER(r, RET_NERRNO(sigaction(sig, sa, تہی)));
        }

        va_end(ap);

        واپس r;
}

ساکن صحیح sigset_جوڑو_many_ap(sigset_t *ss, va_list ap) {
        صحیح sig, r = 0;

        أكد(ss);

        جب ((sig = va_arg(ap, صحیح)) >= 0) {

                اگر (sig == 0)
                        جاری;

                اگر (sigaddset(ss, sig) < 0) {
                        اگر (r >= 0)
                                r = -errno;
                }
        }

        واپس r;
}

صحیح sigset_جوڑو_many_اندرونی(sigset_t *ss, ...) {
        va_list ap;
        صحیح r;

        va_start(ap, ss);
        r = sigset_جوڑو_many_ap(ss, ap);
        va_end(ap);

        واپس r;
}

صحیح sigprocmask_many_اندرونی(صحیح how, sigset_t *ret_old_نقاب, ...) {
        va_list ap;
        sigset_t ss;
        صحیح r;

        اگر (sigemptyset(&ss) < 0)
                واپس -errno;

        va_start(ap, ret_old_نقاب);
        r = sigset_جوڑو_many_ap(&ss, ap);
        va_end(ap);

        اگر (r < 0)
                واپس r;

        واپس RET_NERRNO(sigprocmask(how, &ss, ret_old_نقاب));
}

ساکن ثابت حرف *ثابت static_signal_جدول[] = {
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
#اگر_تعریف_شدہ SIGSTKFLT
        [SIGSTKFLT] = "STKFLT",  /* Linux on SPARC doesn't know SIGSTKFLT */
#اگر_ختم
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

DEFINE_نجی_STRING_جدول_LOOKUP(static_signal, صحیح);

ثابت حرف* signal_مقصد_string(صحیح signo) {
        ساکن thread_local حرف buf[STRLEN("RTMIN+") + DECIMAL_STR_MAX(صحیح)];
        ثابت حرف *name;

        name = static_signal_مقصد_string(signo);
        اگر (name)
                واپس name;

        اگر (signo >= SIGRTMIN && signo <= SIGRTMAX)
                xsprintf(buf, "RTMIN+%d", signo - SIGRTMIN);
        ورنہ
                xsprintf(buf, "%d", signo);

        واپس buf;
}

صحیح signal_from_string(ثابت حرف *s) {
        ثابت حرف *p;
        صحیح signo, r;

        /* Check that the input is a signal number. */
        اگر (محفوظ_atoi(s, &signo) >= 0) {
                اگر (SIGNAL_VALID(signo))
                        واپس signo;
                ورنہ
                        واپس -ERANGE;
        }

        /* Drop "SIG" prefix. */
        اگر (startswith(s, "SIG"))
                s += 3;

        /* Check that the input is a signal name. */
        signo = static_signal_from_string(s);
        اگر (signo > 0)
                واپس signo;

        /* Check that the input is RTMIN or
         * RTMIN+n (0 <= n <= SIGRTMAX-SIGRTMIN). */
        p = startswith(s, "RTMIN");
        اگر (p) {
                اگر (*p == '\0')
                        واپس SIGRTMIN;
                اگر (*p != '+')
                        واپس -EINVAL;

                r = محفوظ_atoi(p, &signo);
                اگر (r < 0)
                        واپس r;

                اگر (signo < 0 || signo > SIGRTMAX - SIGRTMIN)
                        واپس -ERANGE;

                واپس signo + SIGRTMIN;
        }

        /* Check that the input is RTMAX or
         * RTMAX-n (0 <= n <= SIGRTMAX-SIGRTMIN). */
        p = startswith(s, "RTMAX");
        اگر (p) {
                اگر (*p == '\0')
                        واپس SIGRTMAX;
                اگر (*p != '-')
                        واپس -EINVAL;

                r = محفوظ_atoi(p, &signo);
                اگر (r < 0)
                        واپس r;

                اگر (signo > 0 || signo < SIGRTMIN - SIGRTMAX)
                        واپس -ERANGE;

                واپس signo + SIGRTMAX;
        }

        واپس -EINVAL;
}

ساکن ثابت حرف *ثابت sigill_رمز_جدول[] = {
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

DEFINE_نجی_STRING_جدول_LOOKUP_مقصد_STRING(sigill_رمز, صحیح);

ساکن ثابت حرف *ثابت sigfpe_رمز_جدول[] = {
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

DEFINE_نجی_STRING_جدول_LOOKUP_مقصد_STRING(sigfpe_رمز, صحیح);

ساکن ثابت حرف *ثابت sigsegv_رمز_جدول[] = {
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

DEFINE_نجی_STRING_جدول_LOOKUP_مقصد_STRING(sigsegv_رمز, صحیح);

ساکن ثابت حرف *ثابت sigbus_رمز_جدول[] = {
        [BUS_ADRALN]    = "BUS_ADRALN",
        [BUS_ADRERR]    = "BUS_ADRERR",
        [BUS_OBJERR]    = "BUS_OBJERR",
        [BUS_MCEERR_AR] = "BUS_MCEERR_AR",
        [BUS_MCEERR_AO] = "BUS_MCEERR_AO",
};

DEFINE_نجی_STRING_جدول_LOOKUP_مقصد_STRING(sigbus_رمز, صحیح);

ساکن ثابت حرف *ثابت sigtrap_رمز_جدول[] = {
        [TRAP_BRKPT]  = "TRAP_BRKPT",
        [TRAP_TRACE]  = "TRAP_TRACE",
        [TRAP_BRANCH] = "TRAP_BRANCH",
        [TRAP_HWBKPT] = "TRAP_HWBKPT",
        [TRAP_UNK]    = "TRAP_UNK",
        [TRAP_PERF]   = "TRAP_PERF",
};

DEFINE_نجی_STRING_جدول_LOOKUP_مقصد_STRING(sigtrap_رمز, صحیح);

ساکن ثابت حرف *ثابت sigsys_رمز_جدول[] = {
        [SYS_SECCOMP]       = "SYS_SECCOMP",
        [SYS_USER_DISPATCH] = "SYS_USER_DISPATCH",
};

DEFINE_نجی_STRING_جدول_LOOKUP_مقصد_STRING(sigsys_رمز, صحیح);

/* sigchld_code_table is already defined in src/basic/process-util.c with
 * decoded, lower-case values (CLD_EXITED -> "exited"). For consistency with
 * all other values decoded here, provide an uppercase variant. */
ساکن ثابت حرف *ثابت sigchld_uppercase_رمز_جدول[] = {
        [CLD_EXITED]    = "CLD_EXITED",
        [CLD_KILLED]    = "CLD_KILLED",
        [CLD_DUMPED]    = "CLD_DUMPED",
        [CLD_TRAPPED]   = "CLD_TRAPPED",
        [CLD_STOPPED]   = "CLD_STOPPED",
        [CLD_CONTINUED] = "CLD_CONTINUED",
};

DEFINE_نجی_STRING_جدول_LOOKUP_مقصد_STRING(sigchld_uppercase_رمز, صحیح);

ساکن ثابت حرف *ثابت sigpoll_رمز_جدول[] = {
        [POLL_IN]  = "POLL_IN",
        [POLL_OUT] = "POLL_OUT",
        [POLL_MSG] = "POLL_MSG",
        [POLL_ERR] = "POLL_ERR",
        [POLL_PRI] = "POLL_PRI",
        [POLL_HUP] = "POLL_HUP",
};

DEFINE_نجی_STRING_جدول_LOOKUP_مقصد_STRING(sigpoll_رمز, صحیح);

ثابت حرف* signal_رمز_مقصد_string(صحیح signo, صحیح رمز) {
        انتخاب (رمز) {
        /* SI_KERNEL is 0x80 (128). Defining a table just for SI_KERNEL and
         * SI_USER would be a waste of .rodata space: special case them instead. */
        حالت SI_KERNEL:
                واپس "SI_KERNEL";
        حالت SI_USER:
                واپس "SI_USER";
        /* The following values are all negatives. SI_ASYNCNL is -60. Similarly
         * to the special case for SI_KERNEL above, avoid using a table for negative
         * values as well. */
        حالت SI_ASYNCNL:
                واپس "SI_ASYNCNL";
        حالت SI_DETHREAD:
                واپس "SI_DETHREAD";
        حالت SI_TKILL:
                واپس "SI_TKILL";
        حالت SI_SIGIO:
                واپس "SI_SIGIO";
        حالت SI_ASYNCIO:
                واپس "SI_ASYNCIO";
        حالت SI_MESGQ:
                واپس "SI_MESGQ";
        حالت SI_TIMER:
                واپس "SI_TIMER";
        حالت SI_QUEUE:
                واپس "SI_QUEUE";
        }

        انتخاب (signo) {
        حالت SIGILL:
                واپس sigill_رمز_مقصد_string(رمز);
        حالت SIGFPE:
                واپس sigfpe_رمز_مقصد_string(رمز);
        حالت SIGSEGV:
                واپس sigsegv_رمز_مقصد_string(رمز);
        حالت SIGBUS:
                واپس sigbus_رمز_مقصد_string(رمز);
        حالت SIGTRAP:
                واپس sigtrap_رمز_مقصد_string(رمز);
        حالت SIGCHLD:
                واپس sigchld_uppercase_رمز_مقصد_string(رمز);
        حالت SIGPOLL:
                واپس sigpoll_رمز_مقصد_string(رمز);
        حالت SIGSYS:
                واپس sigsys_رمز_مقصد_string(رمز);
        طےشدہ:
                واپس تہی;
        }
}

خالی nop_signal_handler(صحیح sig) {
        /* nothing here */
}

صحیح signal_is_blocked(صحیح sig) {
        sigset_t ss;
        صحیح r;

        r = pthread_sigmask(SIG_SETMASK, تہی, &ss);
        اگر (r != 0)
                واپس -r;

        واپس RET_NERRNO(sigismember(&ss, sig));
}

صحیح autoreaping_enabled(خالی) {
        ساخت sigaction sa;

        اگر (sigaction(SIGCHLD, /* act= */ تہی, &sa) < 0)
                واپس -errno;

        واپس sa.sa_handler == SIG_IGN || FLAGS_سیٹ(sa.sa_flags, SA_NOCLDWAIT);
}

صحیح نکالو_pending_signal_اندرونی(صحیح sig, ...) {
        sigset_t ss;
        va_list ap;
        صحیح r;

        اگر (sig < 0) /* Empty list? */
                واپس -EINVAL;

        اگر (sigemptyset(&ss) < 0)
                واپس -errno;

        /* Add first signal (if the signal is zero, we'll silently skip it, to make it easier to build
         * parameter lists where some element are sometimes off, similar to how sigset_add_many_ap() handles
         * this.) */
        اگر (sig > 0 && sigaddset(&ss, sig) < 0)
                واپس -errno;

        /* Add all other signals */
        va_start(ap, sig);
        r = sigset_جوڑو_many_ap(&ss, ap);
        va_end(ap);
        اگر (r < 0)
                واپس r;

        r = sigtimedwait(&ss, تہی, &(ثابت ساخت timespec) {});
        اگر (r < 0) {
                اگر (errno == EAGAIN)
                        واپس 0;

                واپس -errno;
        }

        واپس r; /* Returns the signal popped */
}

خالی propagate_signal(صحیح sig, siginfo_t *siginfo) {
        pid_t p;

        /* To be called from a signal handler. Will raise the same signal again, in our process + in our threads.
         *
         * Note that we use getpid() instead of getpid_cached(). We might have forked with raw_clone()
         * earlier (see PID 1), and hence let's go to the raw syscall here. In particular as this is not
         * performance sensitive code.
         *
         * Note that we use kill() rather than raise() as fallback, for similar reasons. */

        p = getpid();

        اگر (rt_tgsigqueueinfo(p, gettid(), sig, siginfo) < 0)
                assert_se(kill(p, sig) >= 0);
}

ثابت ساخت sigaction sigaction_ignore = {
        .sa_handler = SIG_IGN,
        .sa_flags = SA_RESTART,
};

ثابت ساخت sigaction sigaction_default = {
        .sa_handler = SIG_DFL,
        .sa_flags = SA_RESTART,
};

ثابت ساخت sigaction sigaction_nop_nocldstop = {
        .sa_handler = nop_signal_handler,
        .sa_flags = SA_NOCLDSTOP|SA_RESTART,
};

صحیح parse_signo(ثابت حرف *s, صحیح *ret) {
        صحیح sig, r;

        r = محفوظ_atoi(s, &sig);
        اگر (r < 0)
                واپس r;

        اگر (!SIGNAL_VALID(sig))
                واپس -EINVAL;

        اگر (ret)
                *ret = sig;

        واپس 0;
}

خالی sigterm_process_گروہ_handler(صحیح اشارہ, siginfo_t *info, خالی *ucontext) {
        أكد(اشارہ == SIGTERM);
        أكد(info);

        /* If the sender is not us, propagate the signal to all processes in
         * the same process group */
        اگر (si_رمز_from_process(info->si_code) &&
            pid_is_valid(info->si_pid) &&
            info->si_pid != getpid_cached())
                (خالی) kill(0, اشارہ);
}
