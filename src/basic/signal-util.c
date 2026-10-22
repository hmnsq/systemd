/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश <threads.h>
#समावेश <unistd.h>

#समावेश "errno-util.h"
#समावेश "parse-util.h"
#समावेश "process-util.h"
#समावेश "signal-util.h"
#समावेश "stdio-util.h"
#समावेश "string-table.h"
#समावेश "string-util.h"

पूर्णांक reset_all_signal_handlers(व्योम) {
        पूर्णांक ret = 0, r;

        क्रम (पूर्णांक sig = 1; sig < _NSIG; sig++) {

                /* These two cannot be caught... */
                अगर (IN_SET(sig, SIGKILL, SIGSTOP))
                        जारी;

                /* On Linux the first two RT signals are reserved by glibc, and sigaction() will return
                 * EINVAL for them. */
                r = RET_NERRNO(sigaction(sig, &sigaction_default, शून्य));
                अगर (r != -EINVAL)
                        RET_GATHER(ret, r);
        }

        वापस ret;
}

पूर्णांक reset_signal_mask(व्योम) {
        sigset_t ss;

        अगर (sigemptyset(&ss) < 0)
                वापस -त्रुटि_सं;

        वापस RET_NERRNO(sigprocmask(SIG_SETMASK, &ss, शून्य));
}

पूर्णांक sigaction_many_internal(स्थिर काष्ठा sigaction *sa, ...) {
        पूर्णांक sig, r = 0;
        बहु_सूची ap;

        बहु_शुरू(ap, sa);

        /* negative signal ends the list. 0 signal is skipped. */
        जबतक ((sig = बहु_तर्क(ap, पूर्णांक)) >= 0) {

                अगर (sig == 0)
                        जारी;

                RET_GATHER(r, RET_NERRNO(sigaction(sig, sa, शून्य)));
        }

        बहु_पूर्ण(ap);

        वापस r;
}

अटल पूर्णांक sigset_add_many_ap(sigset_t *ss, बहु_सूची ap) {
        पूर्णांक sig, r = 0;

        निश्चित(ss);

        जबतक ((sig = बहु_तर्क(ap, पूर्णांक)) >= 0) {

                अगर (sig == 0)
                        जारी;

                अगर (sigaddset(ss, sig) < 0) {
                        अगर (r >= 0)
                                r = -त्रुटि_सं;
                }
        }

        वापस r;
}

पूर्णांक sigset_add_many_internal(sigset_t *ss, ...) {
        बहु_सूची ap;
        पूर्णांक r;

        बहु_शुरू(ap, ss);
        r = sigset_add_many_ap(ss, ap);
        बहु_पूर्ण(ap);

        वापस r;
}

पूर्णांक sigprocmask_many_internal(पूर्णांक how, sigset_t *ret_old_mask, ...) {
        बहु_सूची ap;
        sigset_t ss;
        पूर्णांक r;

        अगर (sigemptyset(&ss) < 0)
                वापस -त्रुटि_सं;

        बहु_शुरू(ap, ret_old_mask);
        r = sigset_add_many_ap(&ss, ap);
        बहु_पूर्ण(ap);

        अगर (r < 0)
                वापस r;

        वापस RET_NERRNO(sigprocmask(how, &ss, ret_old_mask));
}

अटल स्थिर अक्षर *स्थिर static_signal_table[] = {
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
#अगर_घोषित SIGSTKFLT
        [SIGSTKFLT] = "STKFLT",  /* Linux on SPARC doesn't know SIGSTKFLT */
#पूर्ण_अगर
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

स्थिर अक्षर* signal_to_string(पूर्णांक signo) {
        अटल thread_local अक्षर buf[STRLEN("RTMIN+") + DECIMAL_STR_MAX(पूर्णांक)];
        स्थिर अक्षर *name;

        name = static_signal_to_string(signo);
        अगर (name)
                वापस name;

        अगर (signo >= SIGRTMIN && signo <= SIGRTMAX)
                xsprintf(buf, "RTMIN+%d", signo - SIGRTMIN);
        अन्यथा
                xsprintf(buf, "%d", signo);

        वापस buf;
}

पूर्णांक signal_from_string(स्थिर अक्षर *s) {
        स्थिर अक्षर *p;
        पूर्णांक signo, r;

        /* Check that the input is a signal number. */
        अगर (safe_atoi(s, &signo) >= 0) {
                अगर (SIGNAL_VALID(signo))
                        वापस signo;
                अन्यथा
                        वापस -दुस्फल;
        }

        /* Drop "SIG" prefix. */
        अगर (startswith(s, "SIG"))
                s += 3;

        /* Check that the input is a signal name. */
        signo = static_signal_from_string(s);
        अगर (signo > 0)
                वापस signo;

        /* Check that the input is RTMIN or
         * RTMIN+n (0 <= n <= SIGRTMAX-SIGRTMIN). */
        p = startswith(s, "RTMIN");
        अगर (p) {
                अगर (*p == '\0')
                        वापस SIGRTMIN;
                अगर (*p != '+')
                        वापस -EINVAL;

                r = safe_atoi(p, &signo);
                अगर (r < 0)
                        वापस r;

                अगर (signo < 0 || signo > SIGRTMAX - SIGRTMIN)
                        वापस -दुस्फल;

                वापस signo + SIGRTMIN;
        }

        /* Check that the input is RTMAX or
         * RTMAX-n (0 <= n <= SIGRTMAX-SIGRTMIN). */
        p = startswith(s, "RTMAX");
        अगर (p) {
                अगर (*p == '\0')
                        वापस SIGRTMAX;
                अगर (*p != '-')
                        वापस -EINVAL;

                r = safe_atoi(p, &signo);
                अगर (r < 0)
                        वापस r;

                अगर (signo > 0 || signo < SIGRTMIN - SIGRTMAX)
                        वापस -दुस्फल;

                वापस signo + SIGRTMAX;
        }

        वापस -EINVAL;
}

अटल स्थिर अक्षर *स्थिर sigill_code_table[] = {
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

अटल स्थिर अक्षर *स्थिर sigfpe_code_table[] = {
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

अटल स्थिर अक्षर *स्थिर sigsegv_code_table[] = {
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

अटल स्थिर अक्षर *स्थिर sigbus_code_table[] = {
        [BUS_ADRALN]    = "BUS_ADRALN",
        [BUS_ADRERR]    = "BUS_ADRERR",
        [BUS_OBJERR]    = "BUS_OBJERR",
        [BUS_MCEERR_AR] = "BUS_MCEERR_AR",
        [BUS_MCEERR_AO] = "BUS_MCEERR_AO",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigbus_code, पूर्णांक);

अटल स्थिर अक्षर *स्थिर sigtrap_code_table[] = {
        [TRAP_BRKPT]  = "TRAP_BRKPT",
        [TRAP_TRACE]  = "TRAP_TRACE",
        [TRAP_BRANCH] = "TRAP_BRANCH",
        [TRAP_HWBKPT] = "TRAP_HWBKPT",
        [TRAP_UNK]    = "TRAP_UNK",
        [TRAP_PERF]   = "TRAP_PERF",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigtrap_code, पूर्णांक);

अटल स्थिर अक्षर *स्थिर sigsys_code_table[] = {
        [SYS_SECCOMP]       = "SYS_SECCOMP",
        [SYS_USER_DISPATCH] = "SYS_USER_DISPATCH",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigsys_code, पूर्णांक);

/* sigchld_code_table is already defined in src/basic/process-util.c with
 * decoded, lower-case values (CLD_EXITED -> "exited"). For consistency with
 * all other values decoded here, provide an uppercase variant. */
अटल स्थिर अक्षर *स्थिर sigchld_uppercase_code_table[] = {
        [CLD_EXITED]    = "CLD_EXITED",
        [CLD_KILLED]    = "CLD_KILLED",
        [CLD_DUMPED]    = "CLD_DUMPED",
        [CLD_TRAPPED]   = "CLD_TRAPPED",
        [CLD_STOPPED]   = "CLD_STOPPED",
        [CLD_CONTINUED] = "CLD_CONTINUED",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigchld_uppercase_code, पूर्णांक);

अटल स्थिर अक्षर *स्थिर sigpoll_code_table[] = {
        [POLL_IN]  = "POLL_IN",
        [POLL_OUT] = "POLL_OUT",
        [POLL_MSG] = "POLL_MSG",
        [POLL_ERR] = "POLL_ERR",
        [POLL_PRI] = "POLL_PRI",
        [POLL_HUP] = "POLL_HUP",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigpoll_code, पूर्णांक);

स्थिर अक्षर* signal_code_to_string(पूर्णांक signo, पूर्णांक code) {
        चयन (code) {
        /* SI_KERNEL is 0x80 (128). Defining a table just for SI_KERNEL and
         * SI_USER would be a waste of .rodata space: special case them instead. */
        हाल SI_KERNEL:
                वापस "SI_KERNEL";
        हाल SI_USER:
                वापस "SI_USER";
        /* The following values are all negatives. SI_ASYNCNL is -60. Similarly
         * to the special case for SI_KERNEL above, avoid using a table for negative
         * values as well. */
        हाल SI_ASYNCNL:
                वापस "SI_ASYNCNL";
        हाल SI_DETHREAD:
                वापस "SI_DETHREAD";
        हाल SI_TKILL:
                वापस "SI_TKILL";
        हाल SI_SIGIO:
                वापस "SI_SIGIO";
        हाल SI_ASYNCIO:
                वापस "SI_ASYNCIO";
        हाल SI_MESGQ:
                वापस "SI_MESGQ";
        हाल SI_TIMER:
                वापस "SI_TIMER";
        हाल SI_QUEUE:
                वापस "SI_QUEUE";
        }

        चयन (signo) {
        हाल संक_अवैध:
                वापस sigill_code_to_string(code);
        हाल संक_भ_त्रुटि:
                वापस sigfpe_code_to_string(code);
        हाल संक_अंश:
                वापस sigsegv_code_to_string(code);
        हाल SIGBUS:
                वापस sigbus_code_to_string(code);
        हाल SIGTRAP:
                वापस sigtrap_code_to_string(code);
        हाल SIGCHLD:
                वापस sigchld_uppercase_code_to_string(code);
        हाल SIGPOLL:
                वापस sigpoll_code_to_string(code);
        हाल SIGSYS:
                वापस sigsys_code_to_string(code);
        शेष:
                वापस शून्य;
        }
}

व्योम nop_signal_handler(पूर्णांक sig) {
        /* nothing here */
}

पूर्णांक signal_is_blocked(पूर्णांक sig) {
        sigset_t ss;
        पूर्णांक r;

        r = pthread_sigmask(SIG_SETMASK, शून्य, &ss);
        अगर (r != 0)
                वापस -r;

        वापस RET_NERRNO(sigismember(&ss, sig));
}

पूर्णांक autoreaping_enabled(व्योम) {
        काष्ठा sigaction sa;

        अगर (sigaction(SIGCHLD, /* act= */ शून्य, &sa) < 0)
                वापस -त्रुटि_सं;

        वापस sa.sa_handler == संक_छोड़ो || FLAGS_SET(sa.sa_flags, SA_NOCLDWAIT);
}

पूर्णांक निकालो_pending_signal_internal(पूर्णांक sig, ...) {
        sigset_t ss;
        बहु_सूची ap;
        पूर्णांक r;

        अगर (sig < 0) /* Empty list? */
                वापस -EINVAL;

        अगर (sigemptyset(&ss) < 0)
                वापस -त्रुटि_सं;

        /* Add first signal (if the signal is zero, we'll silently skip it, to make it easier to build
         * parameter lists where some element are sometimes off, similar to how sigset_add_many_ap() handles
         * this.) */
        अगर (sig > 0 && sigaddset(&ss, sig) < 0)
                वापस -त्रुटि_सं;

        /* Add all other signals */
        बहु_शुरू(ap, sig);
        r = sigset_add_many_ap(&ss, ap);
        बहु_पूर्ण(ap);
        अगर (r < 0)
                वापस r;

        r = sigtimedwait(&ss, शून्य, &(स्थिर काष्ठा timespec) {});
        अगर (r < 0) {
                अगर (त्रुटि_सं == EAGAIN)
                        वापस 0;

                वापस -त्रुटि_सं;
        }

        वापस r; /* Returns the signal popped */
}

व्योम propagate_signal(पूर्णांक sig, siginfo_t *siginfo) {
        pid_t p;

        /* To be called from a signal handler. Will raise the same signal again, in our process + in our threads.
         *
         * Note that we use getpid() instead of getpid_cached(). We might have forked with raw_clone()
         * earlier (see PID 1), and hence let's go to the raw syscall here. In particular as this is not
         * performance sensitive code.
         *
         * Note that we use kill() rather than raise() as fallback, for similar reasons. */

        p = getpid();

        अगर (rt_tgsigqueueinfo(p, gettid(), sig, siginfo) < 0)
                assert_se(समाप्त(p, sig) >= 0);
}

स्थिर काष्ठा sigaction sigaction_ignore = {
        .sa_handler = संक_छोड़ो,
        .sa_flags = SA_RESTART,
};

स्थिर काष्ठा sigaction sigaction_default = {
        .sa_handler = संक_पूर्व,
        .sa_flags = SA_RESTART,
};

स्थिर काष्ठा sigaction sigaction_nop_nocldstop = {
        .sa_handler = nop_signal_handler,
        .sa_flags = SA_NOCLDSTOP|SA_RESTART,
};

पूर्णांक parse_signo(स्थिर अक्षर *s, पूर्णांक *ret) {
        पूर्णांक sig, r;

        r = safe_atoi(s, &sig);
        अगर (r < 0)
                वापस r;

        अगर (!SIGNAL_VALID(sig))
                वापस -EINVAL;

        अगर (ret)
                *ret = sig;

        वापस 0;
}

व्योम sigterm_process_group_handler(पूर्णांक संकेत, siginfo_t *info, व्योम *ucontext) {
        निश्चित(संकेत == संक_इति);
        निश्चित(info);

        /* If the sender is not us, propagate the signal to all processes in
         * the same process group */
        अगर (si_code_from_process(info->si_code) &&
            pid_is_valid(info->si_pid) &&
            info->si_pid != getpid_cached())
                (व्योम) समाप्त(0, संकेत);
}
