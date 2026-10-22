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

صحیح reset_all_signal_handlers(تهی) {
        صحیح ret = 0, r;

        برای (صحیح sig = 1; sig < _NSIG; sig++) {

                /* These two cannot be caught... */
                اگر (IN_SET(sig, SIGKILL, SIGSTOP))
                        ادامه;

                /* On Linux the first two RT signals are reserved by glibc, and sigaction() will return
                 * EINVAL for them. */
                r = RET_NERRNO(sigaction(sig, &sigaction_default, پوچ));
                اگر (r != -EINVAL)
                        RET_GATHER(ret, r);
        }

        بازگشت ret;
}

صحیح reset_signal_mask(تهی) {
        sigset_t ss;

        اگر (sigemptyset(&ss) < 0)
                بازگشت -errno;

        بازگشت RET_NERRNO(sigprocmask(SIG_SETMASK, &ss, پوچ));
}

صحیح sigaction_many_internal(ثابت ساختار sigaction *sa, ...) {
        صحیح sig, r = 0;
        va_list ap;

        va_start(ap, sa);

        /* negative signal ends the list. 0 signal is skipped. */
        وقتی ((sig = va_arg(ap, صحیح)) >= 0) {

                اگر (sig == 0)
                        ادامه;

                RET_GATHER(r, RET_NERRNO(sigaction(sig, sa, پوچ)));
        }

        va_end(ap);

        بازگشت r;
}

ایستا صحیح sigset_add_many_ap(sigset_t *ss, va_list ap) {
        صحیح sig, r = 0;

        أكد(ss);

        وقتی ((sig = va_arg(ap, صحیح)) >= 0) {

                اگر (sig == 0)
                        ادامه;

                اگر (sigaddset(ss, sig) < 0) {
                        اگر (r >= 0)
                                r = -errno;
                }
        }

        بازگشت r;
}

صحیح sigset_add_many_internal(sigset_t *ss, ...) {
        va_list ap;
        صحیح r;

        va_start(ap, ss);
        r = sigset_add_many_ap(ss, ap);
        va_end(ap);

        بازگشت r;
}

صحیح sigprocmask_many_internal(صحیح how, sigset_t *ret_old_mask, ...) {
        va_list ap;
        sigset_t ss;
        صحیح r;

        اگر (sigemptyset(&ss) < 0)
                بازگشت -errno;

        va_start(ap, ret_old_mask);
        r = sigset_add_many_ap(&ss, ap);
        va_end(ap);

        اگر (r < 0)
                بازگشت r;

        بازگشت RET_NERRNO(sigprocmask(how, &ss, ret_old_mask));
}

ایستا ثابت نویسه *ثابت static_signal_table[] = {
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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP(static_signal, صحیح);

ثابت نویسه* signal_to_string(صحیح signo) {
        ایستا thread_local نویسه buf[STRLEN("RTMIN+") + DECIMAL_STR_MAX(صحیح)];
        ثابت نویسه *name;

        name = static_signal_to_string(signo);
        اگر (name)
                بازگشت name;

        اگر (signo >= SIGRTMIN && signo <= SIGRTMAX)
                xsprintf(buf, "RTMIN+%d", signo - SIGRTMIN);
        وگرنه
                xsprintf(buf, "%d", signo);

        بازگشت buf;
}

صحیح signal_from_string(ثابت نویسه *s) {
        ثابت نویسه *p;
        صحیح signo, r;

        /* Check that the input is a signal number. */
        اگر (safe_atoi(s, &signo) >= 0) {
                اگر (SIGNAL_VALID(signo))
                        بازگشت signo;
                وگرنه
                        بازگشت -ERANGE;
        }

        /* Drop "SIG" prefix. */
        اگر (startswith(s, "SIG"))
                s += 3;

        /* Check that the input is a signal name. */
        signo = static_signal_from_string(s);
        اگر (signo > 0)
                بازگشت signo;

        /* Check that the input is RTMIN or
         * RTMIN+n (0 <= n <= SIGRTMAX-SIGRTMIN). */
        p = startswith(s, "RTMIN");
        اگر (p) {
                اگر (*p == '\0')
                        بازگشت SIGRTMIN;
                اگر (*p != '+')
                        بازگشت -EINVAL;

                r = safe_atoi(p, &signo);
                اگر (r < 0)
                        بازگشت r;

                اگر (signo < 0 || signo > SIGRTMAX - SIGRTMIN)
                        بازگشت -ERANGE;

                بازگشت signo + SIGRTMIN;
        }

        /* Check that the input is RTMAX or
         * RTMAX-n (0 <= n <= SIGRTMAX-SIGRTMIN). */
        p = startswith(s, "RTMAX");
        اگر (p) {
                اگر (*p == '\0')
                        بازگشت SIGRTMAX;
                اگر (*p != '-')
                        بازگشت -EINVAL;

                r = safe_atoi(p, &signo);
                اگر (r < 0)
                        بازگشت r;

                اگر (signo > 0 || signo < SIGRTMIN - SIGRTMAX)
                        بازگشت -ERANGE;

                بازگشت signo + SIGRTMAX;
        }

        بازگشت -EINVAL;
}

ایستا ثابت نویسه *ثابت sigill_code_table[] = {
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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigill_code, صحیح);

ایستا ثابت نویسه *ثابت sigfpe_code_table[] = {
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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigfpe_code, صحیح);

ایستا ثابت نویسه *ثابت sigsegv_code_table[] = {
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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigsegv_code, صحیح);

ایستا ثابت نویسه *ثابت sigbus_code_table[] = {
        [BUS_ADRALN]    = "BUS_ADRALN",
        [BUS_ADRERR]    = "BUS_ADRERR",
        [BUS_OBJERR]    = "BUS_OBJERR",
        [BUS_MCEERR_AR] = "BUS_MCEERR_AR",
        [BUS_MCEERR_AO] = "BUS_MCEERR_AO",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigbus_code, صحیح);

ایستا ثابت نویسه *ثابت sigtrap_code_table[] = {
        [TRAP_BRKPT]  = "TRAP_BRKPT",
        [TRAP_TRACE]  = "TRAP_TRACE",
        [TRAP_BRANCH] = "TRAP_BRANCH",
        [TRAP_HWBKPT] = "TRAP_HWBKPT",
        [TRAP_UNK]    = "TRAP_UNK",
        [TRAP_PERF]   = "TRAP_PERF",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigtrap_code, صحیح);

ایستا ثابت نویسه *ثابت sigsys_code_table[] = {
        [SYS_SECCOMP]       = "SYS_SECCOMP",
        [SYS_USER_DISPATCH] = "SYS_USER_DISPATCH",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigsys_code, صحیح);

/* sigchld_code_table is already defined in src/basic/process-util.c with
 * decoded, lower-case values (CLD_EXITED -> "exited"). For consistency with
 * all other values decoded here, provide an uppercase variant. */
ایستا ثابت نویسه *ثابت sigchld_uppercase_code_table[] = {
        [CLD_EXITED]    = "CLD_EXITED",
        [CLD_KILLED]    = "CLD_KILLED",
        [CLD_DUMPED]    = "CLD_DUMPED",
        [CLD_TRAPPED]   = "CLD_TRAPPED",
        [CLD_STOPPED]   = "CLD_STOPPED",
        [CLD_CONTINUED] = "CLD_CONTINUED",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigchld_uppercase_code, صحیح);

ایستا ثابت نویسه *ثابت sigpoll_code_table[] = {
        [POLL_IN]  = "POLL_IN",
        [POLL_OUT] = "POLL_OUT",
        [POLL_MSG] = "POLL_MSG",
        [POLL_ERR] = "POLL_ERR",
        [POLL_PRI] = "POLL_PRI",
        [POLL_HUP] = "POLL_HUP",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigpoll_code, صحیح);

ثابت نویسه* signal_code_to_string(صحیح signo, صحیح code) {
        انتخاب (code) {
        /* SI_KERNEL is 0x80 (128). Defining a table just for SI_KERNEL and
         * SI_USER would be a waste of .rodata space: special case them instead. */
        حالت SI_KERNEL:
                بازگشت "SI_KERNEL";
        حالت SI_USER:
                بازگشت "SI_USER";
        /* The following values are all negatives. SI_ASYNCNL is -60. Similarly
         * to the special case for SI_KERNEL above, avoid using a table for negative
         * values as well. */
        حالت SI_ASYNCNL:
                بازگشت "SI_ASYNCNL";
        حالت SI_DETHREAD:
                بازگشت "SI_DETHREAD";
        حالت SI_TKILL:
                بازگشت "SI_TKILL";
        حالت SI_SIGIO:
                بازگشت "SI_SIGIO";
        حالت SI_ASYNCIO:
                بازگشت "SI_ASYNCIO";
        حالت SI_MESGQ:
                بازگشت "SI_MESGQ";
        حالت SI_TIMER:
                بازگشت "SI_TIMER";
        حالت SI_QUEUE:
                بازگشت "SI_QUEUE";
        }

        انتخاب (signo) {
        حالت SIGILL:
                بازگشت sigill_code_to_string(code);
        حالت SIGFPE:
                بازگشت sigfpe_code_to_string(code);
        حالت SIGSEGV:
                بازگشت sigsegv_code_to_string(code);
        حالت SIGBUS:
                بازگشت sigbus_code_to_string(code);
        حالت SIGTRAP:
                بازگشت sigtrap_code_to_string(code);
        حالت SIGCHLD:
                بازگشت sigchld_uppercase_code_to_string(code);
        حالت SIGPOLL:
                بازگشت sigpoll_code_to_string(code);
        حالت SIGSYS:
                بازگشت sigsys_code_to_string(code);
        پیشفرض:
                بازگشت پوچ;
        }
}

تهی nop_signal_handler(صحیح sig) {
        /* nothing here */
}

صحیح signal_is_blocked(صحیح sig) {
        sigset_t ss;
        صحیح r;

        r = pthread_sigmask(SIG_SETMASK, پوچ, &ss);
        اگر (r != 0)
                بازگشت -r;

        بازگشت RET_NERRNO(sigismember(&ss, sig));
}

صحیح autoreaping_enabled(تهی) {
        ساختار sigaction sa;

        اگر (sigaction(SIGCHLD, /* act= */ پوچ, &sa) < 0)
                بازگشت -errno;

        بازگشت sa.sa_handler == SIG_IGN || FLAGS_SET(sa.sa_flags, SA_NOCLDWAIT);
}

صحیح برداشت_pending_signal_internal(صحیح sig, ...) {
        sigset_t ss;
        va_list ap;
        صحیح r;

        اگر (sig < 0) /* Empty list? */
                بازگشت -EINVAL;

        اگر (sigemptyset(&ss) < 0)
                بازگشت -errno;

        /* Add first signal (if the signal is zero, we'll silently skip it, to make it easier to build
         * parameter lists where some element are sometimes off, similar to how sigset_add_many_ap() handles
         * this.) */
        اگر (sig > 0 && sigaddset(&ss, sig) < 0)
                بازگشت -errno;

        /* Add all other signals */
        va_start(ap, sig);
        r = sigset_add_many_ap(&ss, ap);
        va_end(ap);
        اگر (r < 0)
                بازگشت r;

        r = sigtimedwait(&ss, پوچ, &(ثابت ساختار timespec) {});
        اگر (r < 0) {
                اگر (errno == EAGAIN)
                        بازگشت 0;

                بازگشت -errno;
        }

        بازگشت r; /* Returns the signal popped */
}

تهی propagate_signal(صحیح sig, siginfo_t *siginfo) {
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

ثابت ساختار sigaction sigaction_ignore = {
        .sa_handler = SIG_IGN,
        .sa_flags = SA_RESTART,
};

ثابت ساختار sigaction sigaction_default = {
        .sa_handler = SIG_DFL,
        .sa_flags = SA_RESTART,
};

ثابت ساختار sigaction sigaction_nop_nocldstop = {
        .sa_handler = nop_signal_handler,
        .sa_flags = SA_NOCLDSTOP|SA_RESTART,
};

صحیح parse_signo(ثابت نویسه *s, صحیح *ret) {
        صحیح sig, r;

        r = safe_atoi(s, &sig);
        اگر (r < 0)
                بازگشت r;

        اگر (!SIGNAL_VALID(sig))
                بازگشت -EINVAL;

        اگر (ret)
                *ret = sig;

        بازگشت 0;
}

تهی sigterm_process_group_handler(صحیح سیگنال, siginfo_t *info, تهی *ucontext) {
        أكد(سیگنال == SIGTERM);
        أكد(info);

        /* If the sender is not us, propagate the signal to all processes in
         * the same process group */
        اگر (si_code_from_process(info->si_code) &&
            pid_is_valid(info->si_pid) &&
            info->si_pid != getpid_cached())
                (تهی) kill(0, سیگنال);
}
