/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <threads.h>
#অন্তর্ভাব <unistd.h>

#অন্তর্ভাব "errno-util.h"
#অন্তর্ভাব "parse-util.h"
#অন্তর্ভাব "process-util.h"
#অন্তর্ভাব "signal-util.h"
#অন্তর্ভাব "stdio-util.h"
#অন্তর্ভাব "string-table.h"
#অন্তর্ভাব "string-util.h"

পূর্ণসংখ্যা reset_all_signal_handlers(শূন্য) {
        পূর্ণসংখ্যা ret = 0, r;

        জন্য (পূর্ণসংখ্যা sig = 1; sig < _NSIG; sig++) {

                /* These two cannot be caught... */
                যদি (IN_SET(sig, SIGKILL, SIGSTOP))
                        চালু;

                /* On Linux the first two RT signals are reserved by glibc, and sigaction() will return
                 * EINVAL for them. */
                r = RET_NERRNO(sigaction(sig, &sigaction_default, ফাঁকা));
                যদি (r != -EINVAL)
                        RET_GATHER(ret, r);
        }

        ফেরত ret;
}

পূর্ণসংখ্যা reset_signal_mask(শূন্য) {
        sigset_t ss;

        যদি (sigemptyset(&ss) < 0)
                ফেরত -errno;

        ফেরত RET_NERRNO(sigprocmask(SIG_SETMASK, &ss, ফাঁকা));
}

পূর্ণসংখ্যা sigaction_many_internal(ধ্রুবক কাঠামো sigaction *sa, ...) {
        পূর্ণসংখ্যা sig, r = 0;
        va_list ap;

        va_start(ap, sa);

        /* negative signal ends the list. 0 signal is skipped. */
        যখন ((sig = va_arg(ap, পূর্ণসংখ্যা)) >= 0) {

                যদি (sig == 0)
                        চালু;

                RET_GATHER(r, RET_NERRNO(sigaction(sig, sa, ফাঁকা)));
        }

        va_end(ap);

        ফেরত r;
}

স্থির পূর্ণসংখ্যা sigset_add_many_ap(sigset_t *ss, va_list ap) {
        পূর্ণসংখ্যা sig, r = 0;

        প্রতিজ্ঞা(ss);

        যখন ((sig = va_arg(ap, পূর্ণসংখ্যা)) >= 0) {

                যদি (sig == 0)
                        চালু;

                যদি (sigaddset(ss, sig) < 0) {
                        যদি (r >= 0)
                                r = -errno;
                }
        }

        ফেরত r;
}

পূর্ণসংখ্যা sigset_add_many_internal(sigset_t *ss, ...) {
        va_list ap;
        পূর্ণসংখ্যা r;

        va_start(ap, ss);
        r = sigset_add_many_ap(ss, ap);
        va_end(ap);

        ফেরত r;
}

পূর্ণসংখ্যা sigprocmask_many_internal(পূর্ণসংখ্যা how, sigset_t *ret_old_mask, ...) {
        va_list ap;
        sigset_t ss;
        পূর্ণসংখ্যা r;

        যদি (sigemptyset(&ss) < 0)
                ফেরত -errno;

        va_start(ap, ret_old_mask);
        r = sigset_add_many_ap(&ss, ap);
        va_end(ap);

        যদি (r < 0)
                ফেরত r;

        ফেরত RET_NERRNO(sigprocmask(how, &ss, ret_old_mask));
}

স্থির ধ্রুবক অক্ষর *ধ্রুবক static_signal_table[] = {
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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP(static_signal, পূর্ণসংখ্যা);

ধ্রুবক অক্ষর* signal_to_string(পূর্ণসংখ্যা signo) {
        স্থির thread_local অক্ষর buf[STRLEN("RTMIN+") + DECIMAL_STR_MAX(পূর্ণসংখ্যা)];
        ধ্রুবক অক্ষর *name;

        name = static_signal_to_string(signo);
        যদি (name)
                ফেরত name;

        যদি (signo >= SIGRTMIN && signo <= SIGRTMAX)
                xsprintf(buf, "RTMIN+%d", signo - SIGRTMIN);
        অন্যথা
                xsprintf(buf, "%d", signo);

        ফেরত buf;
}

পূর্ণসংখ্যা signal_from_string(ধ্রুবক অক্ষর *s) {
        ধ্রুবক অক্ষর *p;
        পূর্ণসংখ্যা signo, r;

        /* Check that the input is a signal number. */
        যদি (safe_atoi(s, &signo) >= 0) {
                যদি (SIGNAL_VALID(signo))
                        ফেরত signo;
                অন্যথা
                        ফেরত -ERANGE;
        }

        /* Drop "SIG" prefix. */
        যদি (startswith(s, "SIG"))
                s += 3;

        /* Check that the input is a signal name. */
        signo = static_signal_from_string(s);
        যদি (signo > 0)
                ফেরত signo;

        /* Check that the input is RTMIN or
         * RTMIN+n (0 <= n <= SIGRTMAX-SIGRTMIN). */
        p = startswith(s, "RTMIN");
        যদি (p) {
                যদি (*p == '\0')
                        ফেরত SIGRTMIN;
                যদি (*p != '+')
                        ফেরত -EINVAL;

                r = safe_atoi(p, &signo);
                যদি (r < 0)
                        ফেরত r;

                যদি (signo < 0 || signo > SIGRTMAX - SIGRTMIN)
                        ফেরত -ERANGE;

                ফেরত signo + SIGRTMIN;
        }

        /* Check that the input is RTMAX or
         * RTMAX-n (0 <= n <= SIGRTMAX-SIGRTMIN). */
        p = startswith(s, "RTMAX");
        যদি (p) {
                যদি (*p == '\0')
                        ফেরত SIGRTMAX;
                যদি (*p != '-')
                        ফেরত -EINVAL;

                r = safe_atoi(p, &signo);
                যদি (r < 0)
                        ফেরত r;

                যদি (signo > 0 || signo < SIGRTMIN - SIGRTMAX)
                        ফেরত -ERANGE;

                ফেরত signo + SIGRTMAX;
        }

        ফেরত -EINVAL;
}

স্থির ধ্রুবক অক্ষর *ধ্রুবক sigill_code_table[] = {
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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigill_code, পূর্ণসংখ্যা);

স্থির ধ্রুবক অক্ষর *ধ্রুবক sigfpe_code_table[] = {
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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigfpe_code, পূর্ণসংখ্যা);

স্থির ধ্রুবক অক্ষর *ধ্রুবক sigsegv_code_table[] = {
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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigsegv_code, পূর্ণসংখ্যা);

স্থির ধ্রুবক অক্ষর *ধ্রুবক sigbus_code_table[] = {
        [BUS_ADRALN]    = "BUS_ADRALN",
        [BUS_ADRERR]    = "BUS_ADRERR",
        [BUS_OBJERR]    = "BUS_OBJERR",
        [BUS_MCEERR_AR] = "BUS_MCEERR_AR",
        [BUS_MCEERR_AO] = "BUS_MCEERR_AO",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigbus_code, পূর্ণসংখ্যা);

স্থির ধ্রুবক অক্ষর *ধ্রুবক sigtrap_code_table[] = {
        [TRAP_BRKPT]  = "TRAP_BRKPT",
        [TRAP_TRACE]  = "TRAP_TRACE",
        [TRAP_BRANCH] = "TRAP_BRANCH",
        [TRAP_HWBKPT] = "TRAP_HWBKPT",
        [TRAP_UNK]    = "TRAP_UNK",
        [TRAP_PERF]   = "TRAP_PERF",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigtrap_code, পূর্ণসংখ্যা);

স্থির ধ্রুবক অক্ষর *ধ্রুবক sigsys_code_table[] = {
        [SYS_SECCOMP]       = "SYS_SECCOMP",
        [SYS_USER_DISPATCH] = "SYS_USER_DISPATCH",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigsys_code, পূর্ণসংখ্যা);

/* sigchld_code_table is already defined in src/basic/process-util.c with
 * decoded, lower-case values (CLD_EXITED -> "exited"). For consistency with
 * all other values decoded here, provide an uppercase variant. */
স্থির ধ্রুবক অক্ষর *ধ্রুবক sigchld_uppercase_code_table[] = {
        [CLD_EXITED]    = "CLD_EXITED",
        [CLD_KILLED]    = "CLD_KILLED",
        [CLD_DUMPED]    = "CLD_DUMPED",
        [CLD_TRAPPED]   = "CLD_TRAPPED",
        [CLD_STOPPED]   = "CLD_STOPPED",
        [CLD_CONTINUED] = "CLD_CONTINUED",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigchld_uppercase_code, পূর্ণসংখ্যা);

স্থির ধ্রুবক অক্ষর *ধ্রুবক sigpoll_code_table[] = {
        [POLL_IN]  = "POLL_IN",
        [POLL_OUT] = "POLL_OUT",
        [POLL_MSG] = "POLL_MSG",
        [POLL_ERR] = "POLL_ERR",
        [POLL_PRI] = "POLL_PRI",
        [POLL_HUP] = "POLL_HUP",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigpoll_code, পূর্ণসংখ্যা);

ধ্রুবক অক্ষর* signal_code_to_string(পূর্ণসংখ্যা signo, পূর্ণসংখ্যা code) {
        নির্বাচন (code) {
        /* SI_KERNEL is 0x80 (128). Defining a table just for SI_KERNEL and
         * SI_USER would be a waste of .rodata space: special case them instead. */
        ক্ষেত্র SI_KERNEL:
                ফেরত "SI_KERNEL";
        ক্ষেত্র SI_USER:
                ফেরত "SI_USER";
        /* The following values are all negatives. SI_ASYNCNL is -60. Similarly
         * to the special case for SI_KERNEL above, avoid using a table for negative
         * values as well. */
        ক্ষেত্র SI_ASYNCNL:
                ফেরত "SI_ASYNCNL";
        ক্ষেত্র SI_DETHREAD:
                ফেরত "SI_DETHREAD";
        ক্ষেত্র SI_TKILL:
                ফেরত "SI_TKILL";
        ক্ষেত্র SI_SIGIO:
                ফেরত "SI_SIGIO";
        ক্ষেত্র SI_ASYNCIO:
                ফেরত "SI_ASYNCIO";
        ক্ষেত্র SI_MESGQ:
                ফেরত "SI_MESGQ";
        ক্ষেত্র SI_TIMER:
                ফেরত "SI_TIMER";
        ক্ষেত্র SI_QUEUE:
                ফেরত "SI_QUEUE";
        }

        নির্বাচন (signo) {
        ক্ষেত্র SIGILL:
                ফেরত sigill_code_to_string(code);
        ক্ষেত্র SIGFPE:
                ফেরত sigfpe_code_to_string(code);
        ক্ষেত্র SIGSEGV:
                ফেরত sigsegv_code_to_string(code);
        ক্ষেত্র SIGBUS:
                ফেরত sigbus_code_to_string(code);
        ক্ষেত্র SIGTRAP:
                ফেরত sigtrap_code_to_string(code);
        ক্ষেত্র SIGCHLD:
                ফেরত sigchld_uppercase_code_to_string(code);
        ক্ষেত্র SIGPOLL:
                ফেরত sigpoll_code_to_string(code);
        ক্ষেত্র SIGSYS:
                ফেরত sigsys_code_to_string(code);
        পূর্বনির্ধারিত:
                ফেরত ফাঁকা;
        }
}

শূন্য nop_signal_handler(পূর্ণসংখ্যা sig) {
        /* nothing here */
}

পূর্ণসংখ্যা signal_is_blocked(পূর্ণসংখ্যা sig) {
        sigset_t ss;
        পূর্ণসংখ্যা r;

        r = pthread_sigmask(SIG_SETMASK, ফাঁকা, &ss);
        যদি (r != 0)
                ফেরত -r;

        ফেরত RET_NERRNO(sigismember(&ss, sig));
}

পূর্ণসংখ্যা autoreaping_enabled(শূন্য) {
        কাঠামো sigaction sa;

        যদি (sigaction(SIGCHLD, /* act= */ ফাঁকা, &sa) < 0)
                ফেরত -errno;

        ফেরত sa.sa_handler == SIG_IGN || FLAGS_SET(sa.sa_flags, SA_NOCLDWAIT);
}

পূর্ণসংখ্যা বের_করো_pending_signal_internal(পূর্ণসংখ্যা sig, ...) {
        sigset_t ss;
        va_list ap;
        পূর্ণসংখ্যা r;

        যদি (sig < 0) /* Empty list? */
                ফেরত -EINVAL;

        যদি (sigemptyset(&ss) < 0)
                ফেরত -errno;

        /* Add first signal (if the signal is zero, we'll silently skip it, to make it easier to build
         * parameter lists where some element are sometimes off, similar to how sigset_add_many_ap() handles
         * this.) */
        যদি (sig > 0 && sigaddset(&ss, sig) < 0)
                ফেরত -errno;

        /* Add all other signals */
        va_start(ap, sig);
        r = sigset_add_many_ap(&ss, ap);
        va_end(ap);
        যদি (r < 0)
                ফেরত r;

        r = sigtimedwait(&ss, ফাঁকা, &(ধ্রুবক কাঠামো timespec) {});
        যদি (r < 0) {
                যদি (errno == EAGAIN)
                        ফেরত 0;

                ফেরত -errno;
        }

        ফেরত r; /* Returns the signal popped */
}

শূন্য propagate_signal(পূর্ণসংখ্যা sig, siginfo_t *siginfo) {
        pid_t p;

        /* To be called from a signal handler. Will raise the same signal again, in our process + in our threads.
         *
         * Note that we use getpid() instead of getpid_cached(). We might have forked with raw_clone()
         * earlier (see PID 1), and hence let's go to the raw syscall here. In particular as this is not
         * performance sensitive code.
         *
         * Note that we use kill() rather than raise() as fallback, for similar reasons. */

        p = getpid();

        যদি (rt_tgsigqueueinfo(p, gettid(), sig, siginfo) < 0)
                assert_se(kill(p, sig) >= 0);
}

ধ্রুবক কাঠামো sigaction sigaction_ignore = {
        .sa_handler = SIG_IGN,
        .sa_flags = SA_RESTART,
};

ধ্রুবক কাঠামো sigaction sigaction_default = {
        .sa_handler = SIG_DFL,
        .sa_flags = SA_RESTART,
};

ধ্রুবক কাঠামো sigaction sigaction_nop_nocldstop = {
        .sa_handler = nop_signal_handler,
        .sa_flags = SA_NOCLDSTOP|SA_RESTART,
};

পূর্ণসংখ্যা parse_signo(ধ্রুবক অক্ষর *s, পূর্ণসংখ্যা *ret) {
        পূর্ণসংখ্যা sig, r;

        r = safe_atoi(s, &sig);
        যদি (r < 0)
                ফেরত r;

        যদি (!SIGNAL_VALID(sig))
                ফেরত -EINVAL;

        যদি (ret)
                *ret = sig;

        ফেরত 0;
}

শূন্য sigterm_process_group_handler(পূর্ণসংখ্যা সংকেত, siginfo_t *info, শূন্য *ucontext) {
        প্রতিজ্ঞা(সংকেত == SIGTERM);
        প্রতিজ্ঞা(info);

        /* If the sender is not us, propagate the signal to all processes in
         * the same process group */
        যদি (si_code_from_process(info->si_code) &&
            pid_is_valid(info->si_pid) &&
            info->si_pid != getpid_cached())
                (শূন্য) kill(0, সংকেত);
}
