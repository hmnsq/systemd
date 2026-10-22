/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ <threads.h>
#அந்தர்பாவ <unistd.h>

#அந்தர்பாவ "errno-util.h"
#அந்தர்பாவ "parse-util.h"
#அந்தர்பாவ "process-util.h"
#அந்தர்பாவ "signal-util.h"
#அந்தர்பாவ "stdio-util.h"
#அந்தர்பாவ "string-table.h"
#அந்தர்பாவ "string-util.h"

முழு reset_all_signal_handlers(வெறுமை) {
        முழு ret = 0, r;

        க்கு (முழு sig = 1; sig < _NSIG; sig++) {

                /* These two cannot be caught... */
                எனில் (IN_SET(sig, SIGKILL, SIGSTOP))
                        தொடர்;

                /* On Linux the first two RT signals are reserved by glibc, and sigaction() will return
                 * EINVAL for them. */
                r = RET_NERRNO(sigaction(sig, &sigaction_default, வெற்று));
                எனில் (r != -EINVAL)
                        RET_GATHER(ret, r);
        }

        திரும்பு ret;
}

முழு reset_signal_mask(வெறுமை) {
        sigset_t ss;

        எனில் (sigemptyset(&ss) < 0)
                திரும்பு -errno;

        திரும்பு RET_NERRNO(sigprocmask(SIG_SETMASK, &ss, வெற்று));
}

முழு sigaction_many_internal(மாறிலி கட்டமைப்பு sigaction *sa, ...) {
        முழு sig, r = 0;
        va_list ap;

        va_start(ap, sa);

        /* negative signal ends the list. 0 signal is skipped. */
        போது ((sig = va_arg(ap, முழு)) >= 0) {

                எனில் (sig == 0)
                        தொடர்;

                RET_GATHER(r, RET_NERRNO(sigaction(sig, sa, வெற்று)));
        }

        va_end(ap);

        திரும்பு r;
}

நிலையான முழு sigset_add_many_ap(sigset_t *ss, va_list ap) {
        முழு sig, r = 0;

        ப்ரதிஜ்ஞா(ss);

        போது ((sig = va_arg(ap, முழு)) >= 0) {

                எனில் (sig == 0)
                        தொடர்;

                எனில் (sigaddset(ss, sig) < 0) {
                        எனில் (r >= 0)
                                r = -errno;
                }
        }

        திரும்பு r;
}

முழு sigset_add_many_internal(sigset_t *ss, ...) {
        va_list ap;
        முழு r;

        va_start(ap, ss);
        r = sigset_add_many_ap(ss, ap);
        va_end(ap);

        திரும்பு r;
}

முழு sigprocmask_many_internal(முழு how, sigset_t *ret_old_mask, ...) {
        va_list ap;
        sigset_t ss;
        முழு r;

        எனில் (sigemptyset(&ss) < 0)
                திரும்பு -errno;

        va_start(ap, ret_old_mask);
        r = sigset_add_many_ap(&ss, ap);
        va_end(ap);

        எனில் (r < 0)
                திரும்பு r;

        திரும்பு RET_NERRNO(sigprocmask(how, &ss, ret_old_mask));
}

நிலையான மாறிலி எழுத்து *மாறிலி static_signal_table[] = {
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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP(static_signal, முழு);

மாறிலி எழுத்து* signal_to_string(முழு signo) {
        நிலையான thread_local எழுத்து buf[STRLEN("RTMIN+") + DECIMAL_STR_MAX(முழு)];
        மாறிலி எழுத்து *name;

        name = static_signal_to_string(signo);
        எனில் (name)
                திரும்பு name;

        எனில் (signo >= SIGRTMIN && signo <= SIGRTMAX)
                xsprintf(buf, "RTMIN+%d", signo - SIGRTMIN);
        இல்லையெனில்
                xsprintf(buf, "%d", signo);

        திரும்பு buf;
}

முழு signal_from_string(மாறிலி எழுத்து *s) {
        மாறிலி எழுத்து *p;
        முழு signo, r;

        /* Check that the input is a signal number. */
        எனில் (safe_atoi(s, &signo) >= 0) {
                எனில் (SIGNAL_VALID(signo))
                        திரும்பு signo;
                இல்லையெனில்
                        திரும்பு -ERANGE;
        }

        /* Drop "SIG" prefix. */
        எனில் (startswith(s, "SIG"))
                s += 3;

        /* Check that the input is a signal name. */
        signo = static_signal_from_string(s);
        எனில் (signo > 0)
                திரும்பு signo;

        /* Check that the input is RTMIN or
         * RTMIN+n (0 <= n <= SIGRTMAX-SIGRTMIN). */
        p = startswith(s, "RTMIN");
        எனில் (p) {
                எனில் (*p == '\0')
                        திரும்பு SIGRTMIN;
                எனில் (*p != '+')
                        திரும்பு -EINVAL;

                r = safe_atoi(p, &signo);
                எனில் (r < 0)
                        திரும்பு r;

                எனில் (signo < 0 || signo > SIGRTMAX - SIGRTMIN)
                        திரும்பு -ERANGE;

                திரும்பு signo + SIGRTMIN;
        }

        /* Check that the input is RTMAX or
         * RTMAX-n (0 <= n <= SIGRTMAX-SIGRTMIN). */
        p = startswith(s, "RTMAX");
        எனில் (p) {
                எனில் (*p == '\0')
                        திரும்பு SIGRTMAX;
                எனில் (*p != '-')
                        திரும்பு -EINVAL;

                r = safe_atoi(p, &signo);
                எனில் (r < 0)
                        திரும்பு r;

                எனில் (signo > 0 || signo < SIGRTMIN - SIGRTMAX)
                        திரும்பு -ERANGE;

                திரும்பு signo + SIGRTMAX;
        }

        திரும்பு -EINVAL;
}

நிலையான மாறிலி எழுத்து *மாறிலி sigill_code_table[] = {
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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigill_code, முழு);

நிலையான மாறிலி எழுத்து *மாறிலி sigfpe_code_table[] = {
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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigfpe_code, முழு);

நிலையான மாறிலி எழுத்து *மாறிலி sigsegv_code_table[] = {
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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigsegv_code, முழு);

நிலையான மாறிலி எழுத்து *மாறிலி sigbus_code_table[] = {
        [BUS_ADRALN]    = "BUS_ADRALN",
        [BUS_ADRERR]    = "BUS_ADRERR",
        [BUS_OBJERR]    = "BUS_OBJERR",
        [BUS_MCEERR_AR] = "BUS_MCEERR_AR",
        [BUS_MCEERR_AO] = "BUS_MCEERR_AO",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigbus_code, முழு);

நிலையான மாறிலி எழுத்து *மாறிலி sigtrap_code_table[] = {
        [TRAP_BRKPT]  = "TRAP_BRKPT",
        [TRAP_TRACE]  = "TRAP_TRACE",
        [TRAP_BRANCH] = "TRAP_BRANCH",
        [TRAP_HWBKPT] = "TRAP_HWBKPT",
        [TRAP_UNK]    = "TRAP_UNK",
        [TRAP_PERF]   = "TRAP_PERF",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigtrap_code, முழு);

நிலையான மாறிலி எழுத்து *மாறிலி sigsys_code_table[] = {
        [SYS_SECCOMP]       = "SYS_SECCOMP",
        [SYS_USER_DISPATCH] = "SYS_USER_DISPATCH",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigsys_code, முழு);

/* sigchld_code_table is already defined in src/basic/process-util.c with
 * decoded, lower-case values (CLD_EXITED -> "exited"). For consistency with
 * all other values decoded here, provide an uppercase variant. */
நிலையான மாறிலி எழுத்து *மாறிலி sigchld_uppercase_code_table[] = {
        [CLD_EXITED]    = "CLD_EXITED",
        [CLD_KILLED]    = "CLD_KILLED",
        [CLD_DUMPED]    = "CLD_DUMPED",
        [CLD_TRAPPED]   = "CLD_TRAPPED",
        [CLD_STOPPED]   = "CLD_STOPPED",
        [CLD_CONTINUED] = "CLD_CONTINUED",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigchld_uppercase_code, முழு);

நிலையான மாறிலி எழுத்து *மாறிலி sigpoll_code_table[] = {
        [POLL_IN]  = "POLL_IN",
        [POLL_OUT] = "POLL_OUT",
        [POLL_MSG] = "POLL_MSG",
        [POLL_ERR] = "POLL_ERR",
        [POLL_PRI] = "POLL_PRI",
        [POLL_HUP] = "POLL_HUP",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigpoll_code, முழு);

மாறிலி எழுத்து* signal_code_to_string(முழு signo, முழு code) {
        தேர்வு (code) {
        /* SI_KERNEL is 0x80 (128). Defining a table just for SI_KERNEL and
         * SI_USER would be a waste of .rodata space: special case them instead. */
        நிலை SI_KERNEL:
                திரும்பு "SI_KERNEL";
        நிலை SI_USER:
                திரும்பு "SI_USER";
        /* The following values are all negatives. SI_ASYNCNL is -60. Similarly
         * to the special case for SI_KERNEL above, avoid using a table for negative
         * values as well. */
        நிலை SI_ASYNCNL:
                திரும்பு "SI_ASYNCNL";
        நிலை SI_DETHREAD:
                திரும்பு "SI_DETHREAD";
        நிலை SI_TKILL:
                திரும்பு "SI_TKILL";
        நிலை SI_SIGIO:
                திரும்பு "SI_SIGIO";
        நிலை SI_ASYNCIO:
                திரும்பு "SI_ASYNCIO";
        நிலை SI_MESGQ:
                திரும்பு "SI_MESGQ";
        நிலை SI_TIMER:
                திரும்பு "SI_TIMER";
        நிலை SI_QUEUE:
                திரும்பு "SI_QUEUE";
        }

        தேர்வு (signo) {
        நிலை SIGILL:
                திரும்பு sigill_code_to_string(code);
        நிலை SIGFPE:
                திரும்பு sigfpe_code_to_string(code);
        நிலை SIGSEGV:
                திரும்பு sigsegv_code_to_string(code);
        நிலை SIGBUS:
                திரும்பு sigbus_code_to_string(code);
        நிலை SIGTRAP:
                திரும்பு sigtrap_code_to_string(code);
        நிலை SIGCHLD:
                திரும்பு sigchld_uppercase_code_to_string(code);
        நிலை SIGPOLL:
                திரும்பு sigpoll_code_to_string(code);
        நிலை SIGSYS:
                திரும்பு sigsys_code_to_string(code);
        இயல்பு:
                திரும்பு வெற்று;
        }
}

வெறுமை nop_signal_handler(முழு sig) {
        /* nothing here */
}

முழு signal_is_blocked(முழு sig) {
        sigset_t ss;
        முழு r;

        r = pthread_sigmask(SIG_SETMASK, வெற்று, &ss);
        எனில் (r != 0)
                திரும்பு -r;

        திரும்பு RET_NERRNO(sigismember(&ss, sig));
}

முழு autoreaping_enabled(வெறுமை) {
        கட்டமைப்பு sigaction sa;

        எனில் (sigaction(SIGCHLD, /* act= */ வெற்று, &sa) < 0)
                திரும்பு -errno;

        திரும்பு sa.sa_handler == SIG_IGN || FLAGS_SET(sa.sa_flags, SA_NOCLDWAIT);
}

முழு எடு_pending_signal_internal(முழு sig, ...) {
        sigset_t ss;
        va_list ap;
        முழு r;

        எனில் (sig < 0) /* Empty list? */
                திரும்பு -EINVAL;

        எனில் (sigemptyset(&ss) < 0)
                திரும்பு -errno;

        /* Add first signal (if the signal is zero, we'll silently skip it, to make it easier to build
         * parameter lists where some element are sometimes off, similar to how sigset_add_many_ap() handles
         * this.) */
        எனில் (sig > 0 && sigaddset(&ss, sig) < 0)
                திரும்பு -errno;

        /* Add all other signals */
        va_start(ap, sig);
        r = sigset_add_many_ap(&ss, ap);
        va_end(ap);
        எனில் (r < 0)
                திரும்பு r;

        r = sigtimedwait(&ss, வெற்று, &(மாறிலி கட்டமைப்பு timespec) {});
        எனில் (r < 0) {
                எனில் (errno == EAGAIN)
                        திரும்பு 0;

                திரும்பு -errno;
        }

        திரும்பு r; /* Returns the signal popped */
}

வெறுமை propagate_signal(முழு sig, siginfo_t *siginfo) {
        pid_t p;

        /* To be called from a signal handler. Will raise the same signal again, in our process + in our threads.
         *
         * Note that we use getpid() instead of getpid_cached(). We might have forked with raw_clone()
         * earlier (see PID 1), and hence let's go to the raw syscall here. In particular as this is not
         * performance sensitive code.
         *
         * Note that we use kill() rather than raise() as fallback, for similar reasons. */

        p = getpid();

        எனில் (rt_tgsigqueueinfo(p, gettid(), sig, siginfo) < 0)
                assert_se(kill(p, sig) >= 0);
}

மாறிலி கட்டமைப்பு sigaction sigaction_ignore = {
        .sa_handler = SIG_IGN,
        .sa_flags = SA_RESTART,
};

மாறிலி கட்டமைப்பு sigaction sigaction_default = {
        .sa_handler = SIG_DFL,
        .sa_flags = SA_RESTART,
};

மாறிலி கட்டமைப்பு sigaction sigaction_nop_nocldstop = {
        .sa_handler = nop_signal_handler,
        .sa_flags = SA_NOCLDSTOP|SA_RESTART,
};

முழு parse_signo(மாறிலி எழுத்து *s, முழு *ret) {
        முழு sig, r;

        r = safe_atoi(s, &sig);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (!SIGNAL_VALID(sig))
                திரும்பு -EINVAL;

        எனில் (ret)
                *ret = sig;

        திரும்பு 0;
}

வெறுமை sigterm_process_group_handler(முழு சமிக்ஞை, siginfo_t *info, வெறுமை *ucontext) {
        ப்ரதிஜ்ஞா(சமிக்ஞை == SIGTERM);
        ப்ரதிஜ்ஞா(info);

        /* If the sender is not us, propagate the signal to all processes in
         * the same process group */
        எனில் (si_code_from_process(info->si_code) &&
            pid_is_valid(info->si_pid) &&
            info->si_pid != getpid_cached())
                (வெறுமை) kill(0, சமிக்ஞை);
}
