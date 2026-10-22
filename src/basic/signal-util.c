/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <threads.h>
#अन्तर्भाव <unistd.h>

#अन्तर्भाव "errno-util.h"
#अन्तर्भाव "parse-util.h"
#अन्तर्भाव "process-util.h"
#अन्तर्भाव "signal-util.h"
#अन्तर्भाव "stdio-util.h"
#अन्तर्भाव "string-table.h"
#अन्तर्भाव "string-util.h"

पूर्णाङ्क reset_all_signal_handlers(शून्य) {
        पूर्णाङ्क ret = 0, r;

        अत्थाय (पूर्णाङ्क sig = 1; sig < _NSIG; sig++) {

                /* These two cannot be caught... */
                सचे (IN_SET(sig, SIGKILL, SIGSTOP))
                        अनुवर्त;

                /* On Linux the first two RT signals are reserved by glibc, and sigaction() will return
                 * EINVAL for them. */
                r = RET_NERRNO(sigaction(sig, &sigaction_default, रिक्त));
                सचे (r != -EINVAL)
                        RET_GATHER(ret, r);
        }

        पटिनिवत्त ret;
}

पूर्णाङ्क reset_signal_mask(शून्य) {
        sigset_t ss;

        सचे (sigemptyset(&ss) < 0)
                पटिनिवत्त -errno;

        पटिनिवत्त RET_NERRNO(sigprocmask(SIG_SETMASK, &ss, रिक्त));
}

पूर्णाङ्क sigaction_many_internal(ध्रुव संरचना sigaction *sa, ...) {
        पूर्णाङ्क sig, r = 0;
        va_list ap;

        va_start(ap, sa);

        /* negative signal ends the list. 0 signal is skipped. */
        याव ((sig = va_arg(ap, पूर्णाङ्क)) >= 0) {

                सचे (sig == 0)
                        अनुवर्त;

                RET_GATHER(r, RET_NERRNO(sigaction(sig, sa, रिक्त)));
        }

        va_end(ap);

        पटिनिवत्त r;
}

स्थिर पूर्णाङ्क sigset_add_many_ap(sigset_t *ss, va_list ap) {
        पूर्णाङ्क sig, r = 0;

        प्रतिज्ञा(ss);

        याव ((sig = va_arg(ap, पूर्णाङ्क)) >= 0) {

                सचे (sig == 0)
                        अनुवर्त;

                सचे (sigaddset(ss, sig) < 0) {
                        सचे (r >= 0)
                                r = -errno;
                }
        }

        पटिनिवत्त r;
}

पूर्णाङ्क sigset_add_many_internal(sigset_t *ss, ...) {
        va_list ap;
        पूर्णाङ्क r;

        va_start(ap, ss);
        r = sigset_add_many_ap(ss, ap);
        va_end(ap);

        पटिनिवत्त r;
}

पूर्णाङ्क sigprocmask_many_internal(पूर्णाङ्क how, sigset_t *ret_old_mask, ...) {
        va_list ap;
        sigset_t ss;
        पूर्णाङ्क r;

        सचे (sigemptyset(&ss) < 0)
                पटिनिवत्त -errno;

        va_start(ap, ret_old_mask);
        r = sigset_add_many_ap(&ss, ap);
        va_end(ap);

        सचे (r < 0)
                पटिनिवत्त r;

        पटिनिवत्त RET_NERRNO(sigprocmask(how, &ss, ret_old_mask));
}

स्थिर ध्रुव वर्ण *ध्रुव static_signal_table[] = {
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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP(static_signal, पूर्णाङ्क);

ध्रुव वर्ण* signal_to_string(पूर्णाङ्क signo) {
        स्थिर thread_local वर्ण buf[STRLEN("RTMIN+") + DECIMAL_STR_MAX(पूर्णाङ्क)];
        ध्रुव वर्ण *name;

        name = static_signal_to_string(signo);
        सचे (name)
                पटिनिवत्त name;

        सचे (signo >= SIGRTMIN && signo <= SIGRTMAX)
                xsprintf(buf, "RTMIN+%d", signo - SIGRTMIN);
        अञ्ञथा
                xsprintf(buf, "%d", signo);

        पटिनिवत्त buf;
}

पूर्णाङ्क signal_from_string(ध्रुव वर्ण *s) {
        ध्रुव वर्ण *p;
        पूर्णाङ्क signo, r;

        /* Check that the input is a signal number. */
        सचे (safe_atoi(s, &signo) >= 0) {
                सचे (SIGNAL_VALID(signo))
                        पटिनिवत्त signo;
                अञ्ञथा
                        पटिनिवत्त -ERANGE;
        }

        /* Drop "SIG" prefix. */
        सचे (startswith(s, "SIG"))
                s += 3;

        /* Check that the input is a signal name. */
        signo = static_signal_from_string(s);
        सचे (signo > 0)
                पटिनिवत्त signo;

        /* Check that the input is RTMIN or
         * RTMIN+n (0 <= n <= SIGRTMAX-SIGRTMIN). */
        p = startswith(s, "RTMIN");
        सचे (p) {
                सचे (*p == '\0')
                        पटिनिवत्त SIGRTMIN;
                सचे (*p != '+')
                        पटिनिवत्त -EINVAL;

                r = safe_atoi(p, &signo);
                सचे (r < 0)
                        पटिनिवत्त r;

                सचे (signo < 0 || signo > SIGRTMAX - SIGRTMIN)
                        पटिनिवत्त -ERANGE;

                पटिनिवत्त signo + SIGRTMIN;
        }

        /* Check that the input is RTMAX or
         * RTMAX-n (0 <= n <= SIGRTMAX-SIGRTMIN). */
        p = startswith(s, "RTMAX");
        सचे (p) {
                सचे (*p == '\0')
                        पटिनिवत्त SIGRTMAX;
                सचे (*p != '-')
                        पटिनिवत्त -EINVAL;

                r = safe_atoi(p, &signo);
                सचे (r < 0)
                        पटिनिवत्त r;

                सचे (signo > 0 || signo < SIGRTMIN - SIGRTMAX)
                        पटिनिवत्त -ERANGE;

                पटिनिवत्त signo + SIGRTMAX;
        }

        पटिनिवत्त -EINVAL;
}

स्थिर ध्रुव वर्ण *ध्रुव sigill_code_table[] = {
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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigill_code, पूर्णाङ्क);

स्थिर ध्रुव वर्ण *ध्रुव sigfpe_code_table[] = {
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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigfpe_code, पूर्णाङ्क);

स्थिर ध्रुव वर्ण *ध्रुव sigsegv_code_table[] = {
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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigsegv_code, पूर्णाङ्क);

स्थिर ध्रुव वर्ण *ध्रुव sigbus_code_table[] = {
        [BUS_ADRALN]    = "BUS_ADRALN",
        [BUS_ADRERR]    = "BUS_ADRERR",
        [BUS_OBJERR]    = "BUS_OBJERR",
        [BUS_MCEERR_AR] = "BUS_MCEERR_AR",
        [BUS_MCEERR_AO] = "BUS_MCEERR_AO",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigbus_code, पूर्णाङ्क);

स्थिर ध्रुव वर्ण *ध्रुव sigtrap_code_table[] = {
        [TRAP_BRKPT]  = "TRAP_BRKPT",
        [TRAP_TRACE]  = "TRAP_TRACE",
        [TRAP_BRANCH] = "TRAP_BRANCH",
        [TRAP_HWBKPT] = "TRAP_HWBKPT",
        [TRAP_UNK]    = "TRAP_UNK",
        [TRAP_PERF]   = "TRAP_PERF",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigtrap_code, पूर्णाङ्क);

स्थिर ध्रुव वर्ण *ध्रुव sigsys_code_table[] = {
        [SYS_SECCOMP]       = "SYS_SECCOMP",
        [SYS_USER_DISPATCH] = "SYS_USER_DISPATCH",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigsys_code, पूर्णाङ्क);

/* sigchld_code_table is already defined in src/basic/process-util.c with
 * decoded, lower-case values (CLD_EXITED -> "exited"). For consistency with
 * all other values decoded here, provide an uppercase variant. */
स्थिर ध्रुव वर्ण *ध्रुव sigchld_uppercase_code_table[] = {
        [CLD_EXITED]    = "CLD_EXITED",
        [CLD_KILLED]    = "CLD_KILLED",
        [CLD_DUMPED]    = "CLD_DUMPED",
        [CLD_TRAPPED]   = "CLD_TRAPPED",
        [CLD_STOPPED]   = "CLD_STOPPED",
        [CLD_CONTINUED] = "CLD_CONTINUED",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigchld_uppercase_code, पूर्णाङ्क);

स्थिर ध्रुव वर्ण *ध्रुव sigpoll_code_table[] = {
        [POLL_IN]  = "POLL_IN",
        [POLL_OUT] = "POLL_OUT",
        [POLL_MSG] = "POLL_MSG",
        [POLL_ERR] = "POLL_ERR",
        [POLL_PRI] = "POLL_PRI",
        [POLL_HUP] = "POLL_HUP",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigpoll_code, पूर्णाङ्क);

ध्रुव वर्ण* signal_code_to_string(पूर्णाङ्क signo, पूर्णाङ्क code) {
        विकल्प (code) {
        /* SI_KERNEL is 0x80 (128). Defining a table just for SI_KERNEL and
         * SI_USER would be a waste of .rodata space: special case them instead. */
        पक्ष SI_KERNEL:
                पटिनिवत्त "SI_KERNEL";
        पक्ष SI_USER:
                पटिनिवत्त "SI_USER";
        /* The following values are all negatives. SI_ASYNCNL is -60. Similarly
         * to the special case for SI_KERNEL above, avoid using a table for negative
         * values as well. */
        पक्ष SI_ASYNCNL:
                पटिनिवत्त "SI_ASYNCNL";
        पक्ष SI_DETHREAD:
                पटिनिवत्त "SI_DETHREAD";
        पक्ष SI_TKILL:
                पटिनिवत्त "SI_TKILL";
        पक्ष SI_SIGIO:
                पटिनिवत्त "SI_SIGIO";
        पक्ष SI_ASYNCIO:
                पटिनिवत्त "SI_ASYNCIO";
        पक्ष SI_MESGQ:
                पटिनिवत्त "SI_MESGQ";
        पक्ष SI_TIMER:
                पटिनिवत्त "SI_TIMER";
        पक्ष SI_QUEUE:
                पटिनिवत्त "SI_QUEUE";
        }

        विकल्प (signo) {
        पक्ष SIGILL:
                पटिनिवत्त sigill_code_to_string(code);
        पक्ष SIGFPE:
                पटिनिवत्त sigfpe_code_to_string(code);
        पक्ष SIGSEGV:
                पटिनिवत्त sigsegv_code_to_string(code);
        पक्ष SIGBUS:
                पटिनिवत्त sigbus_code_to_string(code);
        पक्ष SIGTRAP:
                पटिनिवत्त sigtrap_code_to_string(code);
        पक्ष SIGCHLD:
                पटिनिवत्त sigchld_uppercase_code_to_string(code);
        पक्ष SIGPOLL:
                पटिनिवत्त sigpoll_code_to_string(code);
        पक्ष SIGSYS:
                पटिनिवत्त sigsys_code_to_string(code);
        स्वभाव:
                पटिनिवत्त रिक्त;
        }
}

शून्य nop_signal_handler(पूर्णाङ्क sig) {
        /* nothing here */
}

पूर्णाङ्क signal_is_blocked(पूर्णाङ्क sig) {
        sigset_t ss;
        पूर्णाङ्क r;

        r = pthread_sigmask(SIG_SETMASK, रिक्त, &ss);
        सचे (r != 0)
                पटिनिवत्त -r;

        पटिनिवत्त RET_NERRNO(sigismember(&ss, sig));
}

पूर्णाङ्क autoreaping_enabled(शून्य) {
        संरचना sigaction sa;

        सचे (sigaction(SIGCHLD, /* act= */ रिक्त, &sa) < 0)
                पटिनिवत्त -errno;

        पटिनिवत्त sa.sa_handler == SIG_IGN || FLAGS_SET(sa.sa_flags, SA_NOCLDWAIT);
}

पूर्णाङ्क निष्कासय_pending_signal_internal(पूर्णाङ्क sig, ...) {
        sigset_t ss;
        va_list ap;
        पूर्णाङ्क r;

        सचे (sig < 0) /* Empty list? */
                पटिनिवत्त -EINVAL;

        सचे (sigemptyset(&ss) < 0)
                पटिनिवत्त -errno;

        /* Add first signal (if the signal is zero, we'll silently skip it, to make it easier to build
         * parameter lists where some element are sometimes off, similar to how sigset_add_many_ap() handles
         * this.) */
        सचे (sig > 0 && sigaddset(&ss, sig) < 0)
                पटिनिवत्त -errno;

        /* Add all other signals */
        va_start(ap, sig);
        r = sigset_add_many_ap(&ss, ap);
        va_end(ap);
        सचे (r < 0)
                पटिनिवत्त r;

        r = sigtimedwait(&ss, रिक्त, &(ध्रुव संरचना timespec) {});
        सचे (r < 0) {
                सचे (errno == EAGAIN)
                        पटिनिवत्त 0;

                पटिनिवत्त -errno;
        }

        पटिनिवत्त r; /* Returns the signal popped */
}

शून्य propagate_signal(पूर्णाङ्क sig, siginfo_t *siginfo) {
        pid_t p;

        /* To be called from a signal handler. Will raise the same signal again, in our process + in our threads.
         *
         * Note that we use getpid() instead of getpid_cached(). We might have forked with raw_clone()
         * earlier (see PID 1), and hence let's go to the raw syscall here. In particular as this is not
         * performance sensitive code.
         *
         * Note that we use kill() rather than raise() as fallback, for similar reasons. */

        p = getpid();

        सचे (rt_tgsigqueueinfo(p, gettid(), sig, siginfo) < 0)
                assert_se(kill(p, sig) >= 0);
}

ध्रुव संरचना sigaction sigaction_ignore = {
        .sa_handler = SIG_IGN,
        .sa_flags = SA_RESTART,
};

ध्रुव संरचना sigaction sigaction_default = {
        .sa_handler = SIG_DFL,
        .sa_flags = SA_RESTART,
};

ध्रुव संरचना sigaction sigaction_nop_nocldstop = {
        .sa_handler = nop_signal_handler,
        .sa_flags = SA_NOCLDSTOP|SA_RESTART,
};

पूर्णाङ्क parse_signo(ध्रुव वर्ण *s, पूर्णाङ्क *ret) {
        पूर्णाङ्क sig, r;

        r = safe_atoi(s, &sig);
        सचे (r < 0)
                पटिनिवत्त r;

        सचे (!SIGNAL_VALID(sig))
                पटिनिवत्त -EINVAL;

        सचे (ret)
                *ret = sig;

        पटिनिवत्त 0;
}

शून्य sigterm_process_group_handler(पूर्णाङ्क सङ्केत, siginfo_t *info, शून्य *ucontext) {
        प्रतिज्ञा(सङ्केत == SIGTERM);
        प्रतिज्ञा(info);

        /* If the sender is not us, propagate the signal to all processes in
         * the same process group */
        सचे (si_code_from_process(info->si_code) &&
            pid_is_valid(info->si_pid) &&
            info->si_pid != getpid_cached())
                (शून्य) kill(0, सङ्केत);
}
