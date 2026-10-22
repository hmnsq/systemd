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

पूर्णांक reset_all_signal_handlers(शून्य) {
        पूर्णांक ret = 0, r;

        साठी (पूर्णांक sig = 1; sig < _NSIG; sig++) {

                /* These two cannot be caught... */
                जर (IN_SET(sig, SIGKILL, SIGSTOP))
                        सुरूठेवा;

                /* On Linux the first two RT signals are reserved by glibc, and sigaction() will return
                 * EINVAL for them. */
                r = RET_NERRNO(sigaction(sig, &sigaction_default, रिक्त));
                जर (r != -EINVAL)
                        RET_GATHER(ret, r);
        }

        परत ret;
}

पूर्णांक reset_signal_mask(शून्य) {
        sigset_t ss;

        जर (sigemptyset(&ss) < 0)
                परत -त्रुटि_सं;

        परत RET_NERRNO(sigprocmask(SIG_SETMASK, &ss, रिक्त));
}

पूर्णांक sigaction_many_internal(स्थिरांक संरचना sigaction *sa, ...) {
        पूर्णांक sig, r = 0;
        बहु_सूची ap;

        बहु_शुरू(ap, sa);

        /* negative signal ends the list. 0 signal is skipped. */
        पर्यंत ((sig = बहु_तर्क(ap, पूर्णांक)) >= 0) {

                जर (sig == 0)
                        सुरूठेवा;

                RET_GATHER(r, RET_NERRNO(sigaction(sig, sa, रिक्त)));
        }

        बहु_पूर्ण(ap);

        परत r;
}

स्थिर पूर्णांक sigset_add_many_ap(sigset_t *ss, बहु_सूची ap) {
        पूर्णांक sig, r = 0;

        प्रतिज्ञा(ss);

        पर्यंत ((sig = बहु_तर्क(ap, पूर्णांक)) >= 0) {

                जर (sig == 0)
                        सुरूठेवा;

                जर (sigaddset(ss, sig) < 0) {
                        जर (r >= 0)
                                r = -त्रुटि_सं;
                }
        }

        परत r;
}

पूर्णांक sigset_add_many_internal(sigset_t *ss, ...) {
        बहु_सूची ap;
        पूर्णांक r;

        बहु_शुरू(ap, ss);
        r = sigset_add_many_ap(ss, ap);
        बहु_पूर्ण(ap);

        परत r;
}

पूर्णांक sigprocmask_many_internal(पूर्णांक how, sigset_t *ret_old_mask, ...) {
        बहु_सूची ap;
        sigset_t ss;
        पूर्णांक r;

        जर (sigemptyset(&ss) < 0)
                परत -त्रुटि_सं;

        बहु_शुरू(ap, ret_old_mask);
        r = sigset_add_many_ap(&ss, ap);
        बहु_पूर्ण(ap);

        जर (r < 0)
                परत r;

        परत RET_NERRNO(sigprocmask(how, &ss, ret_old_mask));
}

स्थिर स्थिरांक अक्षर *स्थिरांक static_signal_table[] = {
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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP(static_signal, पूर्णांक);

स्थिरांक अक्षर* signal_to_string(पूर्णांक signo) {
        स्थिर thread_local अक्षर buf[STRLEN("RTMIN+") + DECIMAL_STR_MAX(पूर्णांक)];
        स्थिरांक अक्षर *name;

        name = static_signal_to_string(signo);
        जर (name)
                परत name;

        जर (signo >= SIGRTMIN && signo <= SIGRTMAX)
                xsprintf(buf, "RTMIN+%d", signo - SIGRTMIN);
        अन्यथा
                xsprintf(buf, "%d", signo);

        परत buf;
}

पूर्णांक signal_from_string(स्थिरांक अक्षर *s) {
        स्थिरांक अक्षर *p;
        पूर्णांक signo, r;

        /* Check that the input is a signal number. */
        जर (safe_atoi(s, &signo) >= 0) {
                जर (SIGNAL_VALID(signo))
                        परत signo;
                अन्यथा
                        परत -दुस्फल;
        }

        /* Drop "SIG" prefix. */
        जर (startswith(s, "SIG"))
                s += 3;

        /* Check that the input is a signal name. */
        signo = static_signal_from_string(s);
        जर (signo > 0)
                परत signo;

        /* Check that the input is RTMIN or
         * RTMIN+n (0 <= n <= SIGRTMAX-SIGRTMIN). */
        p = startswith(s, "RTMIN");
        जर (p) {
                जर (*p == '\0')
                        परत SIGRTMIN;
                जर (*p != '+')
                        परत -EINVAL;

                r = safe_atoi(p, &signo);
                जर (r < 0)
                        परत r;

                जर (signo < 0 || signo > SIGRTMAX - SIGRTMIN)
                        परत -दुस्फल;

                परत signo + SIGRTMIN;
        }

        /* Check that the input is RTMAX or
         * RTMAX-n (0 <= n <= SIGRTMAX-SIGRTMIN). */
        p = startswith(s, "RTMAX");
        जर (p) {
                जर (*p == '\0')
                        परत SIGRTMAX;
                जर (*p != '-')
                        परत -EINVAL;

                r = safe_atoi(p, &signo);
                जर (r < 0)
                        परत r;

                जर (signo > 0 || signo < SIGRTMIN - SIGRTMAX)
                        परत -दुस्फल;

                परत signo + SIGRTMAX;
        }

        परत -EINVAL;
}

स्थिर स्थिरांक अक्षर *स्थिरांक sigill_code_table[] = {
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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigill_code, पूर्णांक);

स्थिर स्थिरांक अक्षर *स्थिरांक sigfpe_code_table[] = {
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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigfpe_code, पूर्णांक);

स्थिर स्थिरांक अक्षर *स्थिरांक sigsegv_code_table[] = {
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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigsegv_code, पूर्णांक);

स्थिर स्थिरांक अक्षर *स्थिरांक sigbus_code_table[] = {
        [BUS_ADRALN]    = "BUS_ADRALN",
        [BUS_ADRERR]    = "BUS_ADRERR",
        [BUS_OBJERR]    = "BUS_OBJERR",
        [BUS_MCEERR_AR] = "BUS_MCEERR_AR",
        [BUS_MCEERR_AO] = "BUS_MCEERR_AO",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigbus_code, पूर्णांक);

स्थिर स्थिरांक अक्षर *स्थिरांक sigtrap_code_table[] = {
        [TRAP_BRKPT]  = "TRAP_BRKPT",
        [TRAP_TRACE]  = "TRAP_TRACE",
        [TRAP_BRANCH] = "TRAP_BRANCH",
        [TRAP_HWBKPT] = "TRAP_HWBKPT",
        [TRAP_UNK]    = "TRAP_UNK",
        [TRAP_PERF]   = "TRAP_PERF",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigtrap_code, पूर्णांक);

स्थिर स्थिरांक अक्षर *स्थिरांक sigsys_code_table[] = {
        [SYS_SECCOMP]       = "SYS_SECCOMP",
        [SYS_USER_DISPATCH] = "SYS_USER_DISPATCH",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigsys_code, पूर्णांक);

/* sigchld_code_table is already defined in src/basic/process-util.c with
 * decoded, lower-case values (CLD_EXITED -> "exited"). For consistency with
 * all other values decoded here, provide an uppercase variant. */
स्थिर स्थिरांक अक्षर *स्थिरांक sigchld_uppercase_code_table[] = {
        [CLD_EXITED]    = "CLD_EXITED",
        [CLD_KILLED]    = "CLD_KILLED",
        [CLD_DUMPED]    = "CLD_DUMPED",
        [CLD_TRAPPED]   = "CLD_TRAPPED",
        [CLD_STOPPED]   = "CLD_STOPPED",
        [CLD_CONTINUED] = "CLD_CONTINUED",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigchld_uppercase_code, पूर्णांक);

स्थिर स्थिरांक अक्षर *स्थिरांक sigpoll_code_table[] = {
        [POLL_IN]  = "POLL_IN",
        [POLL_OUT] = "POLL_OUT",
        [POLL_MSG] = "POLL_MSG",
        [POLL_ERR] = "POLL_ERR",
        [POLL_PRI] = "POLL_PRI",
        [POLL_HUP] = "POLL_HUP",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigpoll_code, पूर्णांक);

स्थिरांक अक्षर* signal_code_to_string(पूर्णांक signo, पूर्णांक code) {
        निवड (code) {
        /* SI_KERNEL is 0x80 (128). Defining a table just for SI_KERNEL and
         * SI_USER would be a waste of .rodata space: special case them instead. */
        स्थिती SI_KERNEL:
                परत "SI_KERNEL";
        स्थिती SI_USER:
                परत "SI_USER";
        /* The following values are all negatives. SI_ASYNCNL is -60. Similarly
         * to the special case for SI_KERNEL above, avoid using a table for negative
         * values as well. */
        स्थिती SI_ASYNCNL:
                परत "SI_ASYNCNL";
        स्थिती SI_DETHREAD:
                परत "SI_DETHREAD";
        स्थिती SI_TKILL:
                परत "SI_TKILL";
        स्थिती SI_SIGIO:
                परत "SI_SIGIO";
        स्थिती SI_ASYNCIO:
                परत "SI_ASYNCIO";
        स्थिती SI_MESGQ:
                परत "SI_MESGQ";
        स्थिती SI_TIMER:
                परत "SI_TIMER";
        स्थिती SI_QUEUE:
                परत "SI_QUEUE";
        }

        निवड (signo) {
        स्थिती संक_अवैध:
                परत sigill_code_to_string(code);
        स्थिती संक_भ_त्रुटि:
                परत sigfpe_code_to_string(code);
        स्थिती संक_अंश:
                परत sigsegv_code_to_string(code);
        स्थिती SIGBUS:
                परत sigbus_code_to_string(code);
        स्थिती SIGTRAP:
                परत sigtrap_code_to_string(code);
        स्थिती SIGCHLD:
                परत sigchld_uppercase_code_to_string(code);
        स्थिती SIGPOLL:
                परत sigpoll_code_to_string(code);
        स्थिती SIGSYS:
                परत sigsys_code_to_string(code);
        पूर्वनिर्धारित:
                परत रिक्त;
        }
}

शून्य nop_signal_handler(पूर्णांक sig) {
        /* nothing here */
}

पूर्णांक signal_is_blocked(पूर्णांक sig) {
        sigset_t ss;
        पूर्णांक r;

        r = pthread_sigmask(SIG_SETMASK, रिक्त, &ss);
        जर (r != 0)
                परत -r;

        परत RET_NERRNO(sigismember(&ss, sig));
}

पूर्णांक autoreaping_enabled(शून्य) {
        संरचना sigaction sa;

        जर (sigaction(SIGCHLD, /* act= */ रिक्त, &sa) < 0)
                परत -त्रुटि_सं;

        परत sa.sa_handler == संक_छोड़ो || FLAGS_SET(sa.sa_flags, SA_NOCLDWAIT);
}

पूर्णांक काढा_pending_signal_internal(पूर्णांक sig, ...) {
        sigset_t ss;
        बहु_सूची ap;
        पूर्णांक r;

        जर (sig < 0) /* Empty list? */
                परत -EINVAL;

        जर (sigemptyset(&ss) < 0)
                परत -त्रुटि_सं;

        /* Add first signal (if the signal is zero, we'll silently skip it, to make it easier to build
         * parameter lists where some element are sometimes off, similar to how sigset_add_many_ap() handles
         * this.) */
        जर (sig > 0 && sigaddset(&ss, sig) < 0)
                परत -त्रुटि_सं;

        /* Add all other signals */
        बहु_शुरू(ap, sig);
        r = sigset_add_many_ap(&ss, ap);
        बहु_पूर्ण(ap);
        जर (r < 0)
                परत r;

        r = sigtimedwait(&ss, रिक्त, &(स्थिरांक संरचना timespec) {});
        जर (r < 0) {
                जर (त्रुटि_सं == EAGAIN)
                        परत 0;

                परत -त्रुटि_सं;
        }

        परत r; /* Returns the signal popped */
}

शून्य propagate_signal(पूर्णांक sig, siginfo_t *siginfo) {
        pid_t p;

        /* To be called from a signal handler. Will raise the same signal again, in our process + in our threads.
         *
         * Note that we use getpid() instead of getpid_cached(). We might have forked with raw_clone()
         * earlier (see PID 1), and hence let's go to the raw syscall here. In particular as this is not
         * performance sensitive code.
         *
         * Note that we use kill() rather than raise() as fallback, for similar reasons. */

        p = getpid();

        जर (rt_tgsigqueueinfo(p, gettid(), sig, siginfo) < 0)
                assert_se(समाप्त(p, sig) >= 0);
}

स्थिरांक संरचना sigaction sigaction_ignore = {
        .sa_handler = संक_छोड़ो,
        .sa_flags = SA_RESTART,
};

स्थिरांक संरचना sigaction sigaction_default = {
        .sa_handler = संक_पूर्व,
        .sa_flags = SA_RESTART,
};

स्थिरांक संरचना sigaction sigaction_nop_nocldstop = {
        .sa_handler = nop_signal_handler,
        .sa_flags = SA_NOCLDSTOP|SA_RESTART,
};

पूर्णांक parse_signo(स्थिरांक अक्षर *s, पूर्णांक *ret) {
        पूर्णांक sig, r;

        r = safe_atoi(s, &sig);
        जर (r < 0)
                परत r;

        जर (!SIGNAL_VALID(sig))
                परत -EINVAL;

        जर (ret)
                *ret = sig;

        परत 0;
}

शून्य sigterm_process_group_handler(पूर्णांक संकेत, siginfo_t *info, शून्य *ucontext) {
        प्रतिज्ञा(संकेत == संक_इति);
        प्रतिज्ञा(info);

        /* If the sender is not us, propagate the signal to all processes in
         * the same process group */
        जर (si_code_from_process(info->si_code) &&
            pid_is_valid(info->si_pid) &&
            info->si_pid != getpid_cached())
                (शून्य) समाप्त(0, संकेत);
}
