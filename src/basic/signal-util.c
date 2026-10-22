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

পূর্ণাঙ্ক reset_all_signal_handlers(শূন্য) {
        পূর্ণাঙ্ক ret = 0, r;

        প্রতি (পূর্ণাঙ্ক sig = 1; sig < _NSIG; sig++) {

                /* These two cannot be caught... */
                যদি (IN_SET(sig, SIGKILL, SIGSTOP))
                        অনুবর্ত;

                /* On Linux the first two RT signals are reserved by glibc, and sigaction() will return
                 * EINVAL for them. */
                r = RET_NERRNO(sigaction(sig, &sigaction_default, রিক্ত));
                যদি (r != -EINVAL)
                        RET_GATHER(ret, r);
        }

        প্রত্যাগম ret;
}

পূর্ণাঙ্ক reset_signal_mask(শূন্য) {
        sigset_t ss;

        যদি (sigemptyset(&ss) < 0)
                প্রত্যাগম -errno;

        প্রত্যাগম RET_NERRNO(sigprocmask(SIG_SETMASK, &ss, রিক্ত));
}

পূর্ণাঙ্ক sigaction_many_internal(ধ্রুব সংরচনা sigaction *sa, ...) {
        পূর্ণাঙ্ক sig, r = 0;
        va_list ap;

        va_start(ap, sa);

        /* negative signal ends the list. 0 signal is skipped. */
        যাবত্ ((sig = va_arg(ap, পূর্ণাঙ্ক)) >= 0) {

                যদি (sig == 0)
                        অনুবর্ত;

                RET_GATHER(r, RET_NERRNO(sigaction(sig, sa, রিক্ত)));
        }

        va_end(ap);

        প্রত্যাগম r;
}

স্থির পূর্ণাঙ্ক sigset_add_many_ap(sigset_t *ss, va_list ap) {
        পূর্ণাঙ্ক sig, r = 0;

        প্রতিজ্ঞা(ss);

        যাবত্ ((sig = va_arg(ap, পূর্ণাঙ্ক)) >= 0) {

                যদি (sig == 0)
                        অনুবর্ত;

                যদি (sigaddset(ss, sig) < 0) {
                        যদি (r >= 0)
                                r = -errno;
                }
        }

        প্রত্যাগম r;
}

পূর্ণাঙ্ক sigset_add_many_internal(sigset_t *ss, ...) {
        va_list ap;
        পূর্ণাঙ্ক r;

        va_start(ap, ss);
        r = sigset_add_many_ap(ss, ap);
        va_end(ap);

        প্রত্যাগম r;
}

পূর্ণাঙ্ক sigprocmask_many_internal(পূর্ণাঙ্ক how, sigset_t *ret_old_mask, ...) {
        va_list ap;
        sigset_t ss;
        পূর্ণাঙ্ক r;

        যদি (sigemptyset(&ss) < 0)
                প্রত্যাগম -errno;

        va_start(ap, ret_old_mask);
        r = sigset_add_many_ap(&ss, ap);
        va_end(ap);

        যদি (r < 0)
                প্রত্যাগম r;

        প্রত্যাগম RET_NERRNO(sigprocmask(how, &ss, ret_old_mask));
}

স্থির ধ্রুব বর্ণ *ধ্রুব static_signal_table[] = {
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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP(static_signal, পূর্ণাঙ্ক);

ধ্রুব বর্ণ* signal_to_string(পূর্ণাঙ্ক signo) {
        স্থির thread_local বর্ণ buf[STRLEN("RTMIN+") + DECIMAL_STR_MAX(পূর্ণাঙ্ক)];
        ধ্রুব বর্ণ *name;

        name = static_signal_to_string(signo);
        যদি (name)
                প্রত্যাগম name;

        যদি (signo >= SIGRTMIN && signo <= SIGRTMAX)
                xsprintf(buf, "RTMIN+%d", signo - SIGRTMIN);
        অন্যথা
                xsprintf(buf, "%d", signo);

        প্রত্যাগম buf;
}

পূর্ণাঙ্ক signal_from_string(ধ্রুব বর্ণ *s) {
        ধ্রুব বর্ণ *p;
        পূর্ণাঙ্ক signo, r;

        /* Check that the input is a signal number. */
        যদি (safe_atoi(s, &signo) >= 0) {
                যদি (SIGNAL_VALID(signo))
                        প্রত্যাগম signo;
                অন্যথা
                        প্রত্যাগম -ERANGE;
        }

        /* Drop "SIG" prefix. */
        যদি (startswith(s, "SIG"))
                s += 3;

        /* Check that the input is a signal name. */
        signo = static_signal_from_string(s);
        যদি (signo > 0)
                প্রত্যাগম signo;

        /* Check that the input is RTMIN or
         * RTMIN+n (0 <= n <= SIGRTMAX-SIGRTMIN). */
        p = startswith(s, "RTMIN");
        যদি (p) {
                যদি (*p == '\0')
                        প্রত্যাগম SIGRTMIN;
                যদি (*p != '+')
                        প্রত্যাগম -EINVAL;

                r = safe_atoi(p, &signo);
                যদি (r < 0)
                        প্রত্যাগম r;

                যদি (signo < 0 || signo > SIGRTMAX - SIGRTMIN)
                        প্রত্যাগম -ERANGE;

                প্রত্যাগম signo + SIGRTMIN;
        }

        /* Check that the input is RTMAX or
         * RTMAX-n (0 <= n <= SIGRTMAX-SIGRTMIN). */
        p = startswith(s, "RTMAX");
        যদি (p) {
                যদি (*p == '\0')
                        প্রত্যাগম SIGRTMAX;
                যদি (*p != '-')
                        প্রত্যাগম -EINVAL;

                r = safe_atoi(p, &signo);
                যদি (r < 0)
                        প্রত্যাগম r;

                যদি (signo > 0 || signo < SIGRTMIN - SIGRTMAX)
                        প্রত্যাগম -ERANGE;

                প্রত্যাগম signo + SIGRTMAX;
        }

        প্রত্যাগম -EINVAL;
}

স্থির ধ্রুব বর্ণ *ধ্রুব sigill_code_table[] = {
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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigill_code, পূর্ণাঙ্ক);

স্থির ধ্রুব বর্ণ *ধ্রুব sigfpe_code_table[] = {
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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigfpe_code, পূর্ণাঙ্ক);

স্থির ধ্রুব বর্ণ *ধ্রুব sigsegv_code_table[] = {
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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigsegv_code, পূর্ণাঙ্ক);

স্থির ধ্রুব বর্ণ *ধ্রুব sigbus_code_table[] = {
        [BUS_ADRALN]    = "BUS_ADRALN",
        [BUS_ADRERR]    = "BUS_ADRERR",
        [BUS_OBJERR]    = "BUS_OBJERR",
        [BUS_MCEERR_AR] = "BUS_MCEERR_AR",
        [BUS_MCEERR_AO] = "BUS_MCEERR_AO",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigbus_code, পূর্ণাঙ্ক);

স্থির ধ্রুব বর্ণ *ধ্রুব sigtrap_code_table[] = {
        [TRAP_BRKPT]  = "TRAP_BRKPT",
        [TRAP_TRACE]  = "TRAP_TRACE",
        [TRAP_BRANCH] = "TRAP_BRANCH",
        [TRAP_HWBKPT] = "TRAP_HWBKPT",
        [TRAP_UNK]    = "TRAP_UNK",
        [TRAP_PERF]   = "TRAP_PERF",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigtrap_code, পূর্ণাঙ্ক);

স্থির ধ্রুব বর্ণ *ধ্রুব sigsys_code_table[] = {
        [SYS_SECCOMP]       = "SYS_SECCOMP",
        [SYS_USER_DISPATCH] = "SYS_USER_DISPATCH",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigsys_code, পূর্ণাঙ্ক);

/* sigchld_code_table is already defined in src/basic/process-util.c with
 * decoded, lower-case values (CLD_EXITED -> "exited"). For consistency with
 * all other values decoded here, provide an uppercase variant. */
স্থির ধ্রুব বর্ণ *ধ্রুব sigchld_uppercase_code_table[] = {
        [CLD_EXITED]    = "CLD_EXITED",
        [CLD_KILLED]    = "CLD_KILLED",
        [CLD_DUMPED]    = "CLD_DUMPED",
        [CLD_TRAPPED]   = "CLD_TRAPPED",
        [CLD_STOPPED]   = "CLD_STOPPED",
        [CLD_CONTINUED] = "CLD_CONTINUED",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigchld_uppercase_code, পূর্ণাঙ্ক);

স্থির ধ্রুব বর্ণ *ধ্রুব sigpoll_code_table[] = {
        [POLL_IN]  = "POLL_IN",
        [POLL_OUT] = "POLL_OUT",
        [POLL_MSG] = "POLL_MSG",
        [POLL_ERR] = "POLL_ERR",
        [POLL_PRI] = "POLL_PRI",
        [POLL_HUP] = "POLL_HUP",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigpoll_code, পূর্ণাঙ্ক);

ধ্রুব বর্ণ* signal_code_to_string(পূর্ণাঙ্ক signo, পূর্ণাঙ্ক code) {
        বিকল্প (code) {
        /* SI_KERNEL is 0x80 (128). Defining a table just for SI_KERNEL and
         * SI_USER would be a waste of .rodata space: special case them instead. */
        পক্ষ SI_KERNEL:
                প্রত্যাগম "SI_KERNEL";
        পক্ষ SI_USER:
                প্রত্যাগম "SI_USER";
        /* The following values are all negatives. SI_ASYNCNL is -60. Similarly
         * to the special case for SI_KERNEL above, avoid using a table for negative
         * values as well. */
        পক্ষ SI_ASYNCNL:
                প্রত্যাগম "SI_ASYNCNL";
        পক্ষ SI_DETHREAD:
                প্রত্যাগম "SI_DETHREAD";
        পক্ষ SI_TKILL:
                প্রত্যাগম "SI_TKILL";
        পক্ষ SI_SIGIO:
                প্রত্যাগম "SI_SIGIO";
        পক্ষ SI_ASYNCIO:
                প্রত্যাগম "SI_ASYNCIO";
        পক্ষ SI_MESGQ:
                প্রত্যাগম "SI_MESGQ";
        পক্ষ SI_TIMER:
                প্রত্যাগম "SI_TIMER";
        পক্ষ SI_QUEUE:
                প্রত্যাগম "SI_QUEUE";
        }

        বিকল্প (signo) {
        পক্ষ SIGILL:
                প্রত্যাগম sigill_code_to_string(code);
        পক্ষ SIGFPE:
                প্রত্যাগম sigfpe_code_to_string(code);
        পক্ষ SIGSEGV:
                প্রত্যাগম sigsegv_code_to_string(code);
        পক্ষ SIGBUS:
                প্রত্যাগম sigbus_code_to_string(code);
        পক্ষ SIGTRAP:
                প্রত্যাগম sigtrap_code_to_string(code);
        পক্ষ SIGCHLD:
                প্রত্যাগম sigchld_uppercase_code_to_string(code);
        পক্ষ SIGPOLL:
                প্রত্যাগম sigpoll_code_to_string(code);
        পক্ষ SIGSYS:
                প্রত্যাগম sigsys_code_to_string(code);
        স্বভাব:
                প্রত্যাগম রিক্ত;
        }
}

শূন্য nop_signal_handler(পূর্ণাঙ্ক sig) {
        /* nothing here */
}

পূর্ণাঙ্ক signal_is_blocked(পূর্ণাঙ্ক sig) {
        sigset_t ss;
        পূর্ণাঙ্ক r;

        r = pthread_sigmask(SIG_SETMASK, রিক্ত, &ss);
        যদি (r != 0)
                প্রত্যাগম -r;

        প্রত্যাগম RET_NERRNO(sigismember(&ss, sig));
}

পূর্ণাঙ্ক autoreaping_enabled(শূন্য) {
        সংরচনা sigaction sa;

        যদি (sigaction(SIGCHLD, /* act= */ রিক্ত, &sa) < 0)
                প্রত্যাগম -errno;

        প্রত্যাগম sa.sa_handler == SIG_IGN || FLAGS_SET(sa.sa_flags, SA_NOCLDWAIT);
}

পূর্ণাঙ্ক বের_করো_pending_signal_internal(পূর্ণাঙ্ক sig, ...) {
        sigset_t ss;
        va_list ap;
        পূর্ণাঙ্ক r;

        যদি (sig < 0) /* Empty list? */
                প্রত্যাগম -EINVAL;

        যদি (sigemptyset(&ss) < 0)
                প্রত্যাগম -errno;

        /* Add first signal (if the signal is zero, we'll silently skip it, to make it easier to build
         * parameter lists where some element are sometimes off, similar to how sigset_add_many_ap() handles
         * this.) */
        যদি (sig > 0 && sigaddset(&ss, sig) < 0)
                প্রত্যাগম -errno;

        /* Add all other signals */
        va_start(ap, sig);
        r = sigset_add_many_ap(&ss, ap);
        va_end(ap);
        যদি (r < 0)
                প্রত্যাগম r;

        r = sigtimedwait(&ss, রিক্ত, &(ধ্রুব সংরচনা timespec) {});
        যদি (r < 0) {
                যদি (errno == EAGAIN)
                        প্রত্যাগম 0;

                প্রত্যাগম -errno;
        }

        প্রত্যাগম r; /* Returns the signal popped */
}

শূন্য propagate_signal(পূর্ণাঙ্ক sig, siginfo_t *siginfo) {
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

ধ্রুব সংরচনা sigaction sigaction_ignore = {
        .sa_handler = SIG_IGN,
        .sa_flags = SA_RESTART,
};

ধ্রুব সংরচনা sigaction sigaction_default = {
        .sa_handler = SIG_DFL,
        .sa_flags = SA_RESTART,
};

ধ্রুব সংরচনা sigaction sigaction_nop_nocldstop = {
        .sa_handler = nop_signal_handler,
        .sa_flags = SA_NOCLDSTOP|SA_RESTART,
};

পূর্ণাঙ্ক parse_signo(ধ্রুব বর্ণ *s, পূর্ণাঙ্ক *ret) {
        পূর্ণাঙ্ক sig, r;

        r = safe_atoi(s, &sig);
        যদি (r < 0)
                প্রত্যাগম r;

        যদি (!SIGNAL_VALID(sig))
                প্রত্যাগম -EINVAL;

        যদি (ret)
                *ret = sig;

        প্রত্যাগম 0;
}

শূন্য sigterm_process_group_handler(পূর্ণাঙ্ক সঙ্কেত, siginfo_t *info, শূন্য *ucontext) {
        প্রতিজ্ঞা(সঙ্কেত == SIGTERM);
        প্রতিজ্ঞা(info);

        /* If the sender is not us, propagate the signal to all processes in
         * the same process group */
        যদি (si_code_from_process(info->si_code) &&
            pid_is_valid(info->si_pid) &&
            info->si_pid != getpid_cached())
                (শূন্য) kill(0, সঙ্কেত);
}
