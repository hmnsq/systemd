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

        लागि (पूर्णाङ्क sig = 1; sig < _NSIG; sig++) {

                /* These two cannot be caught... */
                यदि (IN_SET(sig, SIGKILL, SIGSTOP))
                        जारी;

                /* On Linux the first two RT signals are reserved by glibc, and sigaction() will return
                 * EINVAL for them. */
                r = RET_NERRNO(sigaction(sig, &sigaction_default, रिक्त));
                यदि (r != -EINVAL)
                        RET_GATHER(ret, r);
        }

        फर्काऊ ret;
}

पूर्णाङ्क reset_signal_mask(शून्य) {
        sigset_t ss;

        यदि (sigemptyset(&ss) < 0)
                फर्काऊ -त्रुटि_सं;

        फर्काऊ RET_NERRNO(sigprocmask(SIG_SETMASK, &ss, रिक्त));
}

पूर्णाङ्क sigaction_many_internal(अचर संरचना sigaction *sa, ...) {
        पूर्णाङ्क sig, r = 0;
        बहु_सूची ap;

        बहु_शुरू(ap, sa);

        /* negative signal ends the list. 0 signal is skipped. */
        जबसम्म ((sig = बहु_तर्क(ap, पूर्णाङ्क)) >= 0) {

                यदि (sig == 0)
                        जारी;

                RET_GATHER(r, RET_NERRNO(sigaction(sig, sa, रिक्त)));
        }

        बहु_पूर्ण(ap);

        फर्काऊ r;
}

स्थिर पूर्णाङ्क sigset_add_many_ap(sigset_t *ss, बहु_सूची ap) {
        पूर्णाङ्क sig, r = 0;

        प्रतिज्ञा(ss);

        जबसम्म ((sig = बहु_तर्क(ap, पूर्णाङ्क)) >= 0) {

                यदि (sig == 0)
                        जारी;

                यदि (sigaddset(ss, sig) < 0) {
                        यदि (r >= 0)
                                r = -त्रुटि_सं;
                }
        }

        फर्काऊ r;
}

पूर्णाङ्क sigset_add_many_internal(sigset_t *ss, ...) {
        बहु_सूची ap;
        पूर्णाङ्क r;

        बहु_शुरू(ap, ss);
        r = sigset_add_many_ap(ss, ap);
        बहु_पूर्ण(ap);

        फर्काऊ r;
}

पूर्णाङ्क sigprocmask_many_internal(पूर्णाङ्क how, sigset_t *ret_old_mask, ...) {
        बहु_सूची ap;
        sigset_t ss;
        पूर्णाङ्क r;

        यदि (sigemptyset(&ss) < 0)
                फर्काऊ -त्रुटि_सं;

        बहु_शुरू(ap, ret_old_mask);
        r = sigset_add_many_ap(&ss, ap);
        बहु_पूर्ण(ap);

        यदि (r < 0)
                फर्काऊ r;

        फर्काऊ RET_NERRNO(sigprocmask(how, &ss, ret_old_mask));
}

स्थिर अचर वर्ण *अचर static_signal_table[] = {
        [SIGHUP]    = "HUP",
        [संक_विघ्न]    = "INT",
        [SIGQUIT]   = "QUIT",
        [संक_अवैध]    = "ILL",
        [SIGTRAP]   = "TRAP",
        [SIGABRT]   = "ABRT",
        [SIGBUS]    = "BUS",
        [संक_भ_त्रुटि]    = "FPE",
        [SIGKILL]   = "KILL",
        [SIGUSR1]   = "USR1",
        [संक_अंश]   = "SEGV",
        [SIGUSR2]   = "USR2",
        [SIGPIPE]   = "PIPE",
        [SIGALRM]   = "ALRM",
        [संक_इति]   = "TERM",
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

अचर वर्ण* signal_to_string(पूर्णाङ्क signo) {
        स्थिर thread_local वर्ण buf[STRLEN("RTMIN+") + DECIMAL_STR_MAX(पूर्णाङ्क)];
        अचर वर्ण *name;

        name = static_signal_to_string(signo);
        यदि (name)
                फर्काऊ name;

        यदि (signo >= SIGRTMIN && signo <= SIGRTMAX)
                xsprintf(buf, "RTMIN+%d", signo - SIGRTMIN);
        अन्यथा
                xsprintf(buf, "%d", signo);

        फर्काऊ buf;
}

पूर्णाङ्क signal_from_string(अचर वर्ण *s) {
        अचर वर्ण *p;
        पूर्णाङ्क signo, r;

        /* Check that the input is a signal number. */
        यदि (safe_atoi(s, &signo) >= 0) {
                यदि (SIGNAL_VALID(signo))
                        फर्काऊ signo;
                अन्यथा
                        फर्काऊ -दुस्फल;
        }

        /* Drop "SIG" prefix. */
        यदि (startswith(s, "SIG"))
                s += 3;

        /* Check that the input is a signal name. */
        signo = static_signal_from_string(s);
        यदि (signo > 0)
                फर्काऊ signo;

        /* Check that the input is RTMIN or
         * RTMIN+n (0 <= n <= SIGRTMAX-SIGRTMIN). */
        p = startswith(s, "RTMIN");
        यदि (p) {
                यदि (*p == '\0')
                        फर्काऊ SIGRTMIN;
                यदि (*p != '+')
                        फर्काऊ -EINVAL;

                r = safe_atoi(p, &signo);
                यदि (r < 0)
                        फर्काऊ r;

                यदि (signo < 0 || signo > SIGRTMAX - SIGRTMIN)
                        फर्काऊ -दुस्फल;

                फर्काऊ signo + SIGRTMIN;
        }

        /* Check that the input is RTMAX or
         * RTMAX-n (0 <= n <= SIGRTMAX-SIGRTMIN). */
        p = startswith(s, "RTMAX");
        यदि (p) {
                यदि (*p == '\0')
                        फर्काऊ SIGRTMAX;
                यदि (*p != '-')
                        फर्काऊ -EINVAL;

                r = safe_atoi(p, &signo);
                यदि (r < 0)
                        फर्काऊ r;

                यदि (signo > 0 || signo < SIGRTMIN - SIGRTMAX)
                        फर्काऊ -दुस्फल;

                फर्काऊ signo + SIGRTMAX;
        }

        फर्काऊ -EINVAL;
}

स्थिर अचर वर्ण *अचर sigill_code_table[] = {
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

स्थिर अचर वर्ण *अचर sigfpe_code_table[] = {
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

स्थिर अचर वर्ण *अचर sigsegv_code_table[] = {
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

स्थिर अचर वर्ण *अचर sigbus_code_table[] = {
        [BUS_ADRALN]    = "BUS_ADRALN",
        [BUS_ADRERR]    = "BUS_ADRERR",
        [BUS_OBJERR]    = "BUS_OBJERR",
        [BUS_MCEERR_AR] = "BUS_MCEERR_AR",
        [BUS_MCEERR_AO] = "BUS_MCEERR_AO",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigbus_code, पूर्णाङ्क);

स्थिर अचर वर्ण *अचर sigtrap_code_table[] = {
        [TRAP_BRKPT]  = "TRAP_BRKPT",
        [TRAP_TRACE]  = "TRAP_TRACE",
        [TRAP_BRANCH] = "TRAP_BRANCH",
        [TRAP_HWBKPT] = "TRAP_HWBKPT",
        [TRAP_UNK]    = "TRAP_UNK",
        [TRAP_PERF]   = "TRAP_PERF",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigtrap_code, पूर्णाङ्क);

स्थिर अचर वर्ण *अचर sigsys_code_table[] = {
        [SYS_SECCOMP]       = "SYS_SECCOMP",
        [SYS_USER_DISPATCH] = "SYS_USER_DISPATCH",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigsys_code, पूर्णाङ्क);

/* sigchld_code_table is already defined in src/basic/process-util.c with
 * decoded, lower-case values (CLD_EXITED -> "exited"). For consistency with
 * all other values decoded here, provide an uppercase variant. */
स्थिर अचर वर्ण *अचर sigchld_uppercase_code_table[] = {
        [CLD_EXITED]    = "CLD_EXITED",
        [CLD_KILLED]    = "CLD_KILLED",
        [CLD_DUMPED]    = "CLD_DUMPED",
        [CLD_TRAPPED]   = "CLD_TRAPPED",
        [CLD_STOPPED]   = "CLD_STOPPED",
        [CLD_CONTINUED] = "CLD_CONTINUED",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigchld_uppercase_code, पूर्णाङ्क);

स्थिर अचर वर्ण *अचर sigpoll_code_table[] = {
        [POLL_IN]  = "POLL_IN",
        [POLL_OUT] = "POLL_OUT",
        [POLL_MSG] = "POLL_MSG",
        [POLL_ERR] = "POLL_ERR",
        [POLL_PRI] = "POLL_PRI",
        [POLL_HUP] = "POLL_HUP",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigpoll_code, पूर्णाङ्क);

अचर वर्ण* signal_code_to_string(पूर्णाङ्क signo, पूर्णाङ्क code) {
        विकल्प (code) {
        /* SI_KERNEL is 0x80 (128). Defining a table just for SI_KERNEL and
         * SI_USER would be a waste of .rodata space: special case them instead. */
        पक्ष SI_KERNEL:
                फर्काऊ "SI_KERNEL";
        पक्ष SI_USER:
                फर्काऊ "SI_USER";
        /* The following values are all negatives. SI_ASYNCNL is -60. Similarly
         * to the special case for SI_KERNEL above, avoid using a table for negative
         * values as well. */
        पक्ष SI_ASYNCNL:
                फर्काऊ "SI_ASYNCNL";
        पक्ष SI_DETHREAD:
                फर्काऊ "SI_DETHREAD";
        पक्ष SI_TKILL:
                फर्काऊ "SI_TKILL";
        पक्ष SI_SIGIO:
                फर्काऊ "SI_SIGIO";
        पक्ष SI_ASYNCIO:
                फर्काऊ "SI_ASYNCIO";
        पक्ष SI_MESGQ:
                फर्काऊ "SI_MESGQ";
        पक्ष SI_TIMER:
                फर्काऊ "SI_TIMER";
        पक्ष SI_QUEUE:
                फर्काऊ "SI_QUEUE";
        }

        विकल्प (signo) {
        पक्ष संक_अवैध:
                फर्काऊ sigill_code_to_string(code);
        पक्ष संक_भ_त्रुटि:
                फर्काऊ sigfpe_code_to_string(code);
        पक्ष संक_अंश:
                फर्काऊ sigsegv_code_to_string(code);
        पक्ष SIGBUS:
                फर्काऊ sigbus_code_to_string(code);
        पक्ष SIGTRAP:
                फर्काऊ sigtrap_code_to_string(code);
        पक्ष SIGCHLD:
                फर्काऊ sigchld_uppercase_code_to_string(code);
        पक्ष SIGPOLL:
                फर्काऊ sigpoll_code_to_string(code);
        पक्ष SIGSYS:
                फर्काऊ sigsys_code_to_string(code);
        स्वभाव:
                फर्काऊ रिक्त;
        }
}

शून्य nop_signal_handler(पूर्णाङ्क sig) {
        /* nothing here */
}

पूर्णाङ्क signal_is_blocked(पूर्णाङ्क sig) {
        sigset_t ss;
        पूर्णाङ्क r;

        r = pthread_sigmask(SIG_SETMASK, रिक्त, &ss);
        यदि (r != 0)
                फर्काऊ -r;

        फर्काऊ RET_NERRNO(sigismember(&ss, sig));
}

पूर्णाङ्क autoreaping_enabled(शून्य) {
        संरचना sigaction sa;

        यदि (sigaction(SIGCHLD, /* act= */ रिक्त, &sa) < 0)
                फर्काऊ -त्रुटि_सं;

        फर्काऊ sa.sa_handler == संक_छोड़ो || FLAGS_SET(sa.sa_flags, SA_NOCLDWAIT);
}

पूर्णाङ्क झिक_pending_signal_internal(पूर्णाङ्क sig, ...) {
        sigset_t ss;
        बहु_सूची ap;
        पूर्णाङ्क r;

        यदि (sig < 0) /* Empty list? */
                फर्काऊ -EINVAL;

        यदि (sigemptyset(&ss) < 0)
                फर्काऊ -त्रुटि_सं;

        /* Add first signal (if the signal is zero, we'll silently skip it, to make it easier to build
         * parameter lists where some element are sometimes off, similar to how sigset_add_many_ap() handles
         * this.) */
        यदि (sig > 0 && sigaddset(&ss, sig) < 0)
                फर्काऊ -त्रुटि_सं;

        /* Add all other signals */
        बहु_शुरू(ap, sig);
        r = sigset_add_many_ap(&ss, ap);
        बहु_पूर्ण(ap);
        यदि (r < 0)
                फर्काऊ r;

        r = sigtimedwait(&ss, रिक्त, &(अचर संरचना timespec) {});
        यदि (r < 0) {
                यदि (त्रुटि_सं == EAGAIN)
                        फर्काऊ 0;

                फर्काऊ -त्रुटि_सं;
        }

        फर्काऊ r; /* Returns the signal popped */
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

        यदि (rt_tgsigqueueinfo(p, gettid(), sig, siginfo) < 0)
                assert_se(समाप्त(p, sig) >= 0);
}

अचर संरचना sigaction sigaction_ignore = {
        .sa_handler = संक_छोड़ो,
        .sa_flags = SA_RESTART,
};

अचर संरचना sigaction sigaction_default = {
        .sa_handler = संक_पूर्व,
        .sa_flags = SA_RESTART,
};

अचर संरचना sigaction sigaction_nop_nocldstop = {
        .sa_handler = nop_signal_handler,
        .sa_flags = SA_NOCLDSTOP|SA_RESTART,
};

पूर्णाङ्क parse_signo(अचर वर्ण *s, पूर्णाङ्क *ret) {
        पूर्णाङ्क sig, r;

        r = safe_atoi(s, &sig);
        यदि (r < 0)
                फर्काऊ r;

        यदि (!SIGNAL_VALID(sig))
                फर्काऊ -EINVAL;

        यदि (ret)
                *ret = sig;

        फर्काऊ 0;
}

शून्य sigterm_process_group_handler(पूर्णाङ्क सङ्केत, siginfo_t *info, शून्य *ucontext) {
        प्रतिज्ञा(सङ्केत == संक_इति);
        प्रतिज्ञा(info);

        /* If the sender is not us, propagate the signal to all processes in
         * the same process group */
        यदि (si_code_from_process(info->si_code) &&
            pid_is_valid(info->si_pid) &&
            info->si_pid != getpid_cached())
                (शून्य) समाप्त(0, सङ्केत);
}
