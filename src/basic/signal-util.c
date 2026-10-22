/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ <threads.h>
#അന്തര്ഭാവ <unistd.h>

#അന്തര്ഭാവ "errno-util.h"
#അന്തര്ഭാവ "parse-util.h"
#അന്തര്ഭാവ "process-util.h"
#അന്തര്ഭാവ "signal-util.h"
#അന്തര്ഭാവ "stdio-util.h"
#അന്തര്ഭാവ "string-table.h"
#അന്തര്ഭാവ "string-util.h"

പൂർണ്ണസംഖ്യ reset_all_signal_handlers(ശൂന്യം) {
        പൂർണ്ണസംഖ്യ ret = 0, r;

        വേണ്ടി (പൂർണ്ണസംഖ്യ sig = 1; sig < _NSIG; sig++) {

                /* These two cannot be caught... */
                എങ്കിൽ (IN_SET(sig, SIGKILL, SIGSTOP))
                        തുടരുക;

                /* On Linux the first two RT signals are reserved by glibc, and sigaction() will return
                 * EINVAL for them. */
                r = RET_NERRNO(sigaction(sig, &sigaction_default, ശൂന്യ));
                എങ്കിൽ (r != -EINVAL)
                        RET_GATHER(ret, r);
        }

        തിരികെ ret;
}

പൂർണ്ണസംഖ്യ reset_signal_mask(ശൂന്യം) {
        sigset_t ss;

        എങ്കിൽ (sigemptyset(&ss) < 0)
                തിരികെ -errno;

        തിരികെ RET_NERRNO(sigprocmask(SIG_SETMASK, &ss, ശൂന്യ));
}

പൂർണ്ണസംഖ്യ sigaction_many_internal(സ്ഥിരം ഘടന sigaction *sa, ...) {
        പൂർണ്ണസംഖ്യ sig, r = 0;
        va_list ap;

        va_start(ap, sa);

        /* negative signal ends the list. 0 signal is skipped. */
        ആകുമ്പോൾ ((sig = va_arg(ap, പൂർണ്ണസംഖ്യ)) >= 0) {

                എങ്കിൽ (sig == 0)
                        തുടരുക;

                RET_GATHER(r, RET_NERRNO(sigaction(sig, sa, ശൂന്യ)));
        }

        va_end(ap);

        തിരികെ r;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ sigset_add_many_ap(sigset_t *ss, va_list ap) {
        പൂർണ്ണസംഖ്യ sig, r = 0;

        പ്രതിജ്ഞാ(ss);

        ആകുമ്പോൾ ((sig = va_arg(ap, പൂർണ്ണസംഖ്യ)) >= 0) {

                എങ്കിൽ (sig == 0)
                        തുടരുക;

                എങ്കിൽ (sigaddset(ss, sig) < 0) {
                        എങ്കിൽ (r >= 0)
                                r = -errno;
                }
        }

        തിരികെ r;
}

പൂർണ്ണസംഖ്യ sigset_add_many_internal(sigset_t *ss, ...) {
        va_list ap;
        പൂർണ്ണസംഖ്യ r;

        va_start(ap, ss);
        r = sigset_add_many_ap(ss, ap);
        va_end(ap);

        തിരികെ r;
}

പൂർണ്ണസംഖ്യ sigprocmask_many_internal(പൂർണ്ണസംഖ്യ how, sigset_t *ret_old_mask, ...) {
        va_list ap;
        sigset_t ss;
        പൂർണ്ണസംഖ്യ r;

        എങ്കിൽ (sigemptyset(&ss) < 0)
                തിരികെ -errno;

        va_start(ap, ret_old_mask);
        r = sigset_add_many_ap(&ss, ap);
        va_end(ap);

        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ RET_NERRNO(sigprocmask(how, &ss, ret_old_mask));
}

സ്ഥിതം സ്ഥിരം അക്ഷരം *സ്ഥിരം static_signal_table[] = {
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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP(static_signal, പൂർണ്ണസംഖ്യ);

സ്ഥിരം അക്ഷരം* signal_to_string(പൂർണ്ണസംഖ്യ signo) {
        സ്ഥിതം thread_local അക്ഷരം buf[STRLEN("RTMIN+") + DECIMAL_STR_MAX(പൂർണ്ണസംഖ്യ)];
        സ്ഥിരം അക്ഷരം *name;

        name = static_signal_to_string(signo);
        എങ്കിൽ (name)
                തിരികെ name;

        എങ്കിൽ (signo >= SIGRTMIN && signo <= SIGRTMAX)
                xsprintf(buf, "RTMIN+%d", signo - SIGRTMIN);
        അല്ലെങ്കിൽ
                xsprintf(buf, "%d", signo);

        തിരികെ buf;
}

പൂർണ്ണസംഖ്യ signal_from_string(സ്ഥിരം അക്ഷരം *s) {
        സ്ഥിരം അക്ഷരം *p;
        പൂർണ്ണസംഖ്യ signo, r;

        /* Check that the input is a signal number. */
        എങ്കിൽ (safe_atoi(s, &signo) >= 0) {
                എങ്കിൽ (SIGNAL_VALID(signo))
                        തിരികെ signo;
                അല്ലെങ്കിൽ
                        തിരികെ -ERANGE;
        }

        /* Drop "SIG" prefix. */
        എങ്കിൽ (startswith(s, "SIG"))
                s += 3;

        /* Check that the input is a signal name. */
        signo = static_signal_from_string(s);
        എങ്കിൽ (signo > 0)
                തിരികെ signo;

        /* Check that the input is RTMIN or
         * RTMIN+n (0 <= n <= SIGRTMAX-SIGRTMIN). */
        p = startswith(s, "RTMIN");
        എങ്കിൽ (p) {
                എങ്കിൽ (*p == '\0')
                        തിരികെ SIGRTMIN;
                എങ്കിൽ (*p != '+')
                        തിരികെ -EINVAL;

                r = safe_atoi(p, &signo);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                എങ്കിൽ (signo < 0 || signo > SIGRTMAX - SIGRTMIN)
                        തിരികെ -ERANGE;

                തിരികെ signo + SIGRTMIN;
        }

        /* Check that the input is RTMAX or
         * RTMAX-n (0 <= n <= SIGRTMAX-SIGRTMIN). */
        p = startswith(s, "RTMAX");
        എങ്കിൽ (p) {
                എങ്കിൽ (*p == '\0')
                        തിരികെ SIGRTMAX;
                എങ്കിൽ (*p != '-')
                        തിരികെ -EINVAL;

                r = safe_atoi(p, &signo);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                എങ്കിൽ (signo > 0 || signo < SIGRTMIN - SIGRTMAX)
                        തിരികെ -ERANGE;

                തിരികെ signo + SIGRTMAX;
        }

        തിരികെ -EINVAL;
}

സ്ഥിതം സ്ഥിരം അക്ഷരം *സ്ഥിരം sigill_code_table[] = {
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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigill_code, പൂർണ്ണസംഖ്യ);

സ്ഥിതം സ്ഥിരം അക്ഷരം *സ്ഥിരം sigfpe_code_table[] = {
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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigfpe_code, പൂർണ്ണസംഖ്യ);

സ്ഥിതം സ്ഥിരം അക്ഷരം *സ്ഥിരം sigsegv_code_table[] = {
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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigsegv_code, പൂർണ്ണസംഖ്യ);

സ്ഥിതം സ്ഥിരം അക്ഷരം *സ്ഥിരം sigbus_code_table[] = {
        [BUS_ADRALN]    = "BUS_ADRALN",
        [BUS_ADRERR]    = "BUS_ADRERR",
        [BUS_OBJERR]    = "BUS_OBJERR",
        [BUS_MCEERR_AR] = "BUS_MCEERR_AR",
        [BUS_MCEERR_AO] = "BUS_MCEERR_AO",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigbus_code, പൂർണ്ണസംഖ്യ);

സ്ഥിതം സ്ഥിരം അക്ഷരം *സ്ഥിരം sigtrap_code_table[] = {
        [TRAP_BRKPT]  = "TRAP_BRKPT",
        [TRAP_TRACE]  = "TRAP_TRACE",
        [TRAP_BRANCH] = "TRAP_BRANCH",
        [TRAP_HWBKPT] = "TRAP_HWBKPT",
        [TRAP_UNK]    = "TRAP_UNK",
        [TRAP_PERF]   = "TRAP_PERF",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigtrap_code, പൂർണ്ണസംഖ്യ);

സ്ഥിതം സ്ഥിരം അക്ഷരം *സ്ഥിരം sigsys_code_table[] = {
        [SYS_SECCOMP]       = "SYS_SECCOMP",
        [SYS_USER_DISPATCH] = "SYS_USER_DISPATCH",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigsys_code, പൂർണ്ണസംഖ്യ);

/* sigchld_code_table is already defined in src/basic/process-util.c with
 * decoded, lower-case values (CLD_EXITED -> "exited"). For consistency with
 * all other values decoded here, provide an uppercase variant. */
സ്ഥിതം സ്ഥിരം അക്ഷരം *സ്ഥിരം sigchld_uppercase_code_table[] = {
        [CLD_EXITED]    = "CLD_EXITED",
        [CLD_KILLED]    = "CLD_KILLED",
        [CLD_DUMPED]    = "CLD_DUMPED",
        [CLD_TRAPPED]   = "CLD_TRAPPED",
        [CLD_STOPPED]   = "CLD_STOPPED",
        [CLD_CONTINUED] = "CLD_CONTINUED",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigchld_uppercase_code, പൂർണ്ണസംഖ്യ);

സ്ഥിതം സ്ഥിരം അക്ഷരം *സ്ഥിരം sigpoll_code_table[] = {
        [POLL_IN]  = "POLL_IN",
        [POLL_OUT] = "POLL_OUT",
        [POLL_MSG] = "POLL_MSG",
        [POLL_ERR] = "POLL_ERR",
        [POLL_PRI] = "POLL_PRI",
        [POLL_HUP] = "POLL_HUP",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigpoll_code, പൂർണ്ണസംഖ്യ);

സ്ഥിരം അക്ഷരം* signal_code_to_string(പൂർണ്ണസംഖ്യ signo, പൂർണ്ണസംഖ്യ code) {
        തിരഞ്ഞെടുപ്പ് (code) {
        /* SI_KERNEL is 0x80 (128). Defining a table just for SI_KERNEL and
         * SI_USER would be a waste of .rodata space: special case them instead. */
        അവസ്ഥ SI_KERNEL:
                തിരികെ "SI_KERNEL";
        അവസ്ഥ SI_USER:
                തിരികെ "SI_USER";
        /* The following values are all negatives. SI_ASYNCNL is -60. Similarly
         * to the special case for SI_KERNEL above, avoid using a table for negative
         * values as well. */
        അവസ്ഥ SI_ASYNCNL:
                തിരികെ "SI_ASYNCNL";
        അവസ്ഥ SI_DETHREAD:
                തിരികെ "SI_DETHREAD";
        അവസ്ഥ SI_TKILL:
                തിരികെ "SI_TKILL";
        അവസ്ഥ SI_SIGIO:
                തിരികെ "SI_SIGIO";
        അവസ്ഥ SI_ASYNCIO:
                തിരികെ "SI_ASYNCIO";
        അവസ്ഥ SI_MESGQ:
                തിരികെ "SI_MESGQ";
        അവസ്ഥ SI_TIMER:
                തിരികെ "SI_TIMER";
        അവസ്ഥ SI_QUEUE:
                തിരികെ "SI_QUEUE";
        }

        തിരഞ്ഞെടുപ്പ് (signo) {
        അവസ്ഥ SIGILL:
                തിരികെ sigill_code_to_string(code);
        അവസ്ഥ SIGFPE:
                തിരികെ sigfpe_code_to_string(code);
        അവസ്ഥ SIGSEGV:
                തിരികെ sigsegv_code_to_string(code);
        അവസ്ഥ SIGBUS:
                തിരികെ sigbus_code_to_string(code);
        അവസ്ഥ SIGTRAP:
                തിരികെ sigtrap_code_to_string(code);
        അവസ്ഥ SIGCHLD:
                തിരികെ sigchld_uppercase_code_to_string(code);
        അവസ്ഥ SIGPOLL:
                തിരികെ sigpoll_code_to_string(code);
        അവസ്ഥ SIGSYS:
                തിരികെ sigsys_code_to_string(code);
        സ്ഥിരസ്ഥിതി:
                തിരികെ ശൂന്യ;
        }
}

ശൂന്യം nop_signal_handler(പൂർണ്ണസംഖ്യ sig) {
        /* nothing here */
}

പൂർണ്ണസംഖ്യ signal_is_blocked(പൂർണ്ണസംഖ്യ sig) {
        sigset_t ss;
        പൂർണ്ണസംഖ്യ r;

        r = pthread_sigmask(SIG_SETMASK, ശൂന്യ, &ss);
        എങ്കിൽ (r != 0)
                തിരികെ -r;

        തിരികെ RET_NERRNO(sigismember(&ss, sig));
}

പൂർണ്ണസംഖ്യ autoreaping_enabled(ശൂന്യം) {
        ഘടന sigaction sa;

        എങ്കിൽ (sigaction(SIGCHLD, /* act= */ ശൂന്യ, &sa) < 0)
                തിരികെ -errno;

        തിരികെ sa.sa_handler == SIG_IGN || FLAGS_SET(sa.sa_flags, SA_NOCLDWAIT);
}

പൂർണ്ണസംഖ്യ എടുക്കുക_pending_signal_internal(പൂർണ്ണസംഖ്യ sig, ...) {
        sigset_t ss;
        va_list ap;
        പൂർണ്ണസംഖ്യ r;

        എങ്കിൽ (sig < 0) /* Empty list? */
                തിരികെ -EINVAL;

        എങ്കിൽ (sigemptyset(&ss) < 0)
                തിരികെ -errno;

        /* Add first signal (if the signal is zero, we'll silently skip it, to make it easier to build
         * parameter lists where some element are sometimes off, similar to how sigset_add_many_ap() handles
         * this.) */
        എങ്കിൽ (sig > 0 && sigaddset(&ss, sig) < 0)
                തിരികെ -errno;

        /* Add all other signals */
        va_start(ap, sig);
        r = sigset_add_many_ap(&ss, ap);
        va_end(ap);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        r = sigtimedwait(&ss, ശൂന്യ, &(സ്ഥിരം ഘടന timespec) {});
        എങ്കിൽ (r < 0) {
                എങ്കിൽ (errno == EAGAIN)
                        തിരികെ 0;

                തിരികെ -errno;
        }

        തിരികെ r; /* Returns the signal popped */
}

ശൂന്യം propagate_signal(പൂർണ്ണസംഖ്യ sig, siginfo_t *siginfo) {
        pid_t p;

        /* To be called from a signal handler. Will raise the same signal again, in our process + in our threads.
         *
         * Note that we use getpid() instead of getpid_cached(). We might have forked with raw_clone()
         * earlier (see PID 1), and hence let's go to the raw syscall here. In particular as this is not
         * performance sensitive code.
         *
         * Note that we use kill() rather than raise() as fallback, for similar reasons. */

        p = getpid();

        എങ്കിൽ (rt_tgsigqueueinfo(p, gettid(), sig, siginfo) < 0)
                assert_se(kill(p, sig) >= 0);
}

സ്ഥിരം ഘടന sigaction sigaction_ignore = {
        .sa_handler = SIG_IGN,
        .sa_flags = SA_RESTART,
};

സ്ഥിരം ഘടന sigaction sigaction_default = {
        .sa_handler = SIG_DFL,
        .sa_flags = SA_RESTART,
};

സ്ഥിരം ഘടന sigaction sigaction_nop_nocldstop = {
        .sa_handler = nop_signal_handler,
        .sa_flags = SA_NOCLDSTOP|SA_RESTART,
};

പൂർണ്ണസംഖ്യ parse_signo(സ്ഥിരം അക്ഷരം *s, പൂർണ്ണസംഖ്യ *ret) {
        പൂർണ്ണസംഖ്യ sig, r;

        r = safe_atoi(s, &sig);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (!SIGNAL_VALID(sig))
                തിരികെ -EINVAL;

        എങ്കിൽ (ret)
                *ret = sig;

        തിരികെ 0;
}

ശൂന്യം sigterm_process_group_handler(പൂർണ്ണസംഖ്യ സൂചന, siginfo_t *info, ശൂന്യം *ucontext) {
        പ്രതിജ്ഞാ(സൂചന == SIGTERM);
        പ്രതിജ്ഞാ(info);

        /* If the sender is not us, propagate the signal to all processes in
         * the same process group */
        എങ്കിൽ (si_code_from_process(info->si_code) &&
            pid_is_valid(info->si_pid) &&
            info->si_pid != getpid_cached())
                (ശൂന്യം) kill(0, സൂചന);
}
