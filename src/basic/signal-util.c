/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <threads.h>
#ಅನ್ತರ್ಭಾವ <unistd.h>

#ಅನ್ತರ್ಭಾವ "errno-util.h"
#ಅನ್ತರ್ಭಾವ "parse-util.h"
#ಅನ್ತರ್ಭಾವ "process-util.h"
#ಅನ್ತರ್ಭಾವ "signal-util.h"
#ಅನ್ತರ್ಭಾವ "stdio-util.h"
#ಅನ್ತರ್ಭಾವ "string-table.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"

ಪೂರ್ಣಾಂಕ reset_all_signal_handlers(ಶೂನ್ಯ) {
        ಪೂರ್ಣಾಂಕ ret = 0, r;

        ಗಾಗಿ (ಪೂರ್ಣಾಂಕ sig = 1; sig < _NSIG; sig++) {

                /* These two cannot be caught... */
                ಆದರೆ (ಒಳ_ಗಣ(sig, SIGKILL, SIGSTOP))
                        ಮುಂದುವರಿ;

                /* On Linux the first two RT signals are reserved by glibc, and sigaction() will return
                 * EINVAL for them. */
                r = RET_NERRNO(sigaction(sig, &sigaction_default, ಖಾಲಿ));
                ಆದರೆ (r != -EINVAL)
                        RET_GATHER(ret, r);
        }

        ಹಿಂತಿರುಗಿ ret;
}

ಪೂರ್ಣಾಂಕ reset_signal_ಮುಖವಾಡ(ಶೂನ್ಯ) {
        sigset_t ss;

        ಆದರೆ (sigemptyset(&ss) < 0)
                ಹಿಂತಿರುಗಿ -errno;

        ಹಿಂತಿರುಗಿ RET_NERRNO(sigprocmask(SIG_SETMASK, &ss, ಖಾಲಿ));
}

ಪೂರ್ಣಾಂಕ sigaction_many_ಆಂತರಿಕ(ಸ್ಥಿರಾಂಕ ರಚನೆ sigaction *sa, ...) {
        ಪೂರ್ಣಾಂಕ sig, r = 0;
        va_list ap;

        va_start(ap, sa);

        /* negative signal ends the list. 0 signal is skipped. */
        ಆಗ ((sig = va_arg(ap, ಪೂರ್ಣಾಂಕ)) >= 0) {

                ಆದರೆ (sig == 0)
                        ಮುಂದುವರಿ;

                RET_GATHER(r, RET_NERRNO(sigaction(sig, sa, ಖಾಲಿ)));
        }

        va_end(ap);

        ಹಿಂತಿರುಗಿ r;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sigset_ಕೂಡಿಸು_many_ap(sigset_t *ss, va_list ap) {
        ಪೂರ್ಣಾಂಕ sig, r = 0;

        ಪ್ರತಿಜ್ಞಾ(ss);

        ಆಗ ((sig = va_arg(ap, ಪೂರ್ಣಾಂಕ)) >= 0) {

                ಆದರೆ (sig == 0)
                        ಮುಂದುವರಿ;

                ಆದರೆ (sigaddset(ss, sig) < 0) {
                        ಆದರೆ (r >= 0)
                                r = -errno;
                }
        }

        ಹಿಂತಿರುಗಿ r;
}

ಪೂರ್ಣಾಂಕ sigset_ಕೂಡಿಸು_many_ಆಂತರಿಕ(sigset_t *ss, ...) {
        va_list ap;
        ಪೂರ್ಣಾಂಕ r;

        va_start(ap, ss);
        r = sigset_ಕೂಡಿಸು_many_ap(ss, ap);
        va_end(ap);

        ಹಿಂತಿರುಗಿ r;
}

ಪೂರ್ಣಾಂಕ sigprocmask_many_ಆಂತರಿಕ(ಪೂರ್ಣಾಂಕ how, sigset_t *ret_old_ಮುಖವಾಡ, ...) {
        va_list ap;
        sigset_t ss;
        ಪೂರ್ಣಾಂಕ r;

        ಆದರೆ (sigemptyset(&ss) < 0)
                ಹಿಂತಿರುಗಿ -errno;

        va_start(ap, ret_old_ಮುಖವಾಡ);
        r = sigset_ಕೂಡಿಸು_many_ap(&ss, ap);
        va_end(ap);

        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ RET_NERRNO(sigprocmask(how, &ss, ret_old_ಮುಖವಾಡ));
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸ್ಥಿರಾಂಕ static_signal_ಕೋಷ್ಟಕ[] = {
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
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SIGSTKFLT
        [SIGSTKFLT] = "STKFLT",  /* Linux on SPARC doesn't know SIGSTKFLT */
#ಆದರೆ_ಅಂತ್ಯ
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

DEFINE_ಖಾಸಗಿ_STRING_ಕೋಷ್ಟಕ_LOOKUP(static_signal, ಪೂರ್ಣಾಂಕ);

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* signal_ಗಮ್ಯ_string(ಪೂರ್ಣಾಂಕ signo) {
        ಸ್ಥಿರ thread_local ಅಕ್ಷರ buf[STRLEN("RTMIN+") + DECIMAL_STR_MAX(ಪೂರ್ಣಾಂಕ)];
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name;

        name = static_signal_ಗಮ್ಯ_string(signo);
        ಆದರೆ (name)
                ಹಿಂತಿರುಗಿ name;

        ಆದರೆ (signo >= SIGRTMIN && signo <= SIGRTMAX)
                xsprintf(buf, "RTMIN+%d", signo - SIGRTMIN);
        ಇಲ್ಲದಿದ್ದರೆ
                xsprintf(buf, "%d", signo);

        ಹಿಂತಿರುಗಿ buf;
}

ಪೂರ್ಣಾಂಕ signal_from_string(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p;
        ಪೂರ್ಣಾಂಕ signo, r;

        /* Check that the input is a signal number. */
        ಆದರೆ (ಸುರಕ್ಷಿತ_atoi(s, &signo) >= 0) {
                ಆದರೆ (SIGNAL_VALID(signo))
                        ಹಿಂತಿರುಗಿ signo;
                ಇಲ್ಲದಿದ್ದರೆ
                        ಹಿಂತಿರುಗಿ -ERANGE;
        }

        /* Drop "SIG" prefix. */
        ಆದರೆ (startswith(s, "SIG"))
                s += 3;

        /* Check that the input is a signal name. */
        signo = static_signal_from_string(s);
        ಆದರೆ (signo > 0)
                ಹಿಂತಿರುಗಿ signo;

        /* Check that the input is RTMIN or
         * RTMIN+n (0 <= n <= SIGRTMAX-SIGRTMIN). */
        p = startswith(s, "RTMIN");
        ಆದರೆ (p) {
                ಆದರೆ (*p == '\0')
                        ಹಿಂತಿರುಗಿ SIGRTMIN;
                ಆದರೆ (*p != '+')
                        ಹಿಂತಿರುಗಿ -EINVAL;

                r = ಸುರಕ್ಷಿತ_atoi(p, &signo);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                ಆದರೆ (signo < 0 || signo > SIGRTMAX - SIGRTMIN)
                        ಹಿಂತಿರುಗಿ -ERANGE;

                ಹಿಂತಿರುಗಿ signo + SIGRTMIN;
        }

        /* Check that the input is RTMAX or
         * RTMAX-n (0 <= n <= SIGRTMAX-SIGRTMIN). */
        p = startswith(s, "RTMAX");
        ಆದರೆ (p) {
                ಆದರೆ (*p == '\0')
                        ಹಿಂತಿರುಗಿ SIGRTMAX;
                ಆದರೆ (*p != '-')
                        ಹಿಂತಿರುಗಿ -EINVAL;

                r = ಸುರಕ್ಷಿತ_atoi(p, &signo);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                ಆದರೆ (signo > 0 || signo < SIGRTMIN - SIGRTMAX)
                        ಹಿಂತಿರುಗಿ -ERANGE;

                ಹಿಂತಿರುಗಿ signo + SIGRTMAX;
        }

        ಹಿಂತಿರುಗಿ -EINVAL;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸ್ಥಿರಾಂಕ sigill_ಸಂಕೇತ_ಕೋಷ್ಟಕ[] = {
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

DEFINE_ಖಾಸಗಿ_STRING_ಕೋಷ್ಟಕ_LOOKUP_ಗಮ್ಯ_STRING(sigill_ಸಂಕೇತ, ಪೂರ್ಣಾಂಕ);

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸ್ಥಿರಾಂಕ sigfpe_ಸಂಕೇತ_ಕೋಷ್ಟಕ[] = {
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

DEFINE_ಖಾಸಗಿ_STRING_ಕೋಷ್ಟಕ_LOOKUP_ಗಮ್ಯ_STRING(sigfpe_ಸಂಕೇತ, ಪೂರ್ಣಾಂಕ);

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸ್ಥಿರಾಂಕ sigsegv_ಸಂಕೇತ_ಕೋಷ್ಟಕ[] = {
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

DEFINE_ಖಾಸಗಿ_STRING_ಕೋಷ್ಟಕ_LOOKUP_ಗಮ್ಯ_STRING(sigsegv_ಸಂಕೇತ, ಪೂರ್ಣಾಂಕ);

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸ್ಥಿರಾಂಕ sigbus_ಸಂಕೇತ_ಕೋಷ್ಟಕ[] = {
        [BUS_ADRALN]    = "BUS_ADRALN",
        [BUS_ADRERR]    = "BUS_ADRERR",
        [BUS_OBJERR]    = "BUS_OBJERR",
        [BUS_MCEERR_AR] = "BUS_MCEERR_AR",
        [BUS_MCEERR_AO] = "BUS_MCEERR_AO",
};

DEFINE_ಖಾಸಗಿ_STRING_ಕೋಷ್ಟಕ_LOOKUP_ಗಮ್ಯ_STRING(sigbus_ಸಂಕೇತ, ಪೂರ್ಣಾಂಕ);

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸ್ಥಿರಾಂಕ sigtrap_ಸಂಕೇತ_ಕೋಷ್ಟಕ[] = {
        [TRAP_BRKPT]  = "TRAP_BRKPT",
        [TRAP_TRACE]  = "TRAP_TRACE",
        [TRAP_BRANCH] = "TRAP_BRANCH",
        [TRAP_HWBKPT] = "TRAP_HWBKPT",
        [TRAP_UNK]    = "TRAP_UNK",
        [TRAP_PERF]   = "TRAP_PERF",
};

DEFINE_ಖಾಸಗಿ_STRING_ಕೋಷ್ಟಕ_LOOKUP_ಗಮ್ಯ_STRING(sigtrap_ಸಂಕೇತ, ಪೂರ್ಣಾಂಕ);

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸ್ಥಿರಾಂಕ sigsys_ಸಂಕೇತ_ಕೋಷ್ಟಕ[] = {
        [SYS_SECCOMP]       = "SYS_SECCOMP",
        [SYS_USER_DISPATCH] = "SYS_USER_DISPATCH",
};

DEFINE_ಖಾಸಗಿ_STRING_ಕೋಷ್ಟಕ_LOOKUP_ಗಮ್ಯ_STRING(sigsys_ಸಂಕೇತ, ಪೂರ್ಣಾಂಕ);

/* sigchld_code_table is already defined in src/basic/process-util.c with
 * decoded, lower-case values (CLD_EXITED -> "exited"). For consistency with
 * all other values decoded here, provide an uppercase variant. */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸ್ಥಿರಾಂಕ sigchld_uppercase_ಸಂಕೇತ_ಕೋಷ್ಟಕ[] = {
        [CLD_EXITED]    = "CLD_EXITED",
        [CLD_KILLED]    = "CLD_KILLED",
        [CLD_DUMPED]    = "CLD_DUMPED",
        [CLD_TRAPPED]   = "CLD_TRAPPED",
        [CLD_STOPPED]   = "CLD_STOPPED",
        [CLD_CONTINUED] = "CLD_CONTINUED",
};

DEFINE_ಖಾಸಗಿ_STRING_ಕೋಷ್ಟಕ_LOOKUP_ಗಮ್ಯ_STRING(sigchld_uppercase_ಸಂಕೇತ, ಪೂರ್ಣಾಂಕ);

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸ್ಥಿರಾಂಕ sigpoll_ಸಂಕೇತ_ಕೋಷ್ಟಕ[] = {
        [POLL_IN]  = "POLL_IN",
        [POLL_OUT] = "POLL_OUT",
        [POLL_MSG] = "POLL_MSG",
        [POLL_ERR] = "POLL_ERR",
        [POLL_PRI] = "POLL_PRI",
        [POLL_HUP] = "POLL_HUP",
};

DEFINE_ಖಾಸಗಿ_STRING_ಕೋಷ್ಟಕ_LOOKUP_ಗಮ್ಯ_STRING(sigpoll_ಸಂಕೇತ, ಪೂರ್ಣಾಂಕ);

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* signal_ಸಂಕೇತ_ಗಮ್ಯ_string(ಪೂರ್ಣಾಂಕ signo, ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2) {
        ಆಯ್ಕೆ (ಸಂಕೇತ_2) {
        /* SI_KERNEL is 0x80 (128). Defining a table just for SI_KERNEL and
         * SI_USER would be a waste of .rodata space: special case them instead. */
        ಸ್ಥಿತಿ SI_KERNEL:
                ಹಿಂತಿರುಗಿ "SI_KERNEL";
        ಸ್ಥಿತಿ SI_USER:
                ಹಿಂತಿರುಗಿ "SI_USER";
        /* The following values are all negatives. SI_ASYNCNL is -60. Similarly
         * to the special case for SI_KERNEL above, avoid using a table for negative
         * values as well. */
        ಸ್ಥಿತಿ SI_ASYNCNL:
                ಹಿಂತಿರುಗಿ "SI_ASYNCNL";
        ಸ್ಥಿತಿ SI_DETHREAD:
                ಹಿಂತಿರುಗಿ "SI_DETHREAD";
        ಸ್ಥಿತಿ SI_TKILL:
                ಹಿಂತಿರುಗಿ "SI_TKILL";
        ಸ್ಥಿತಿ SI_SIGIO:
                ಹಿಂತಿರುಗಿ "SI_SIGIO";
        ಸ್ಥಿತಿ SI_ASYNCIO:
                ಹಿಂತಿರುಗಿ "SI_ASYNCIO";
        ಸ್ಥಿತಿ SI_MESGQ:
                ಹಿಂತಿರುಗಿ "SI_MESGQ";
        ಸ್ಥಿತಿ SI_TIMER:
                ಹಿಂತಿರುಗಿ "SI_TIMER";
        ಸ್ಥಿತಿ SI_QUEUE:
                ಹಿಂತಿರುಗಿ "SI_QUEUE";
        }

        ಆಯ್ಕೆ (signo) {
        ಸ್ಥಿತಿ SIGILL:
                ಹಿಂತಿರುಗಿ sigill_ಸಂಕೇತ_ಗಮ್ಯ_string(ಸಂಕೇತ_2);
        ಸ್ಥಿತಿ SIGFPE:
                ಹಿಂತಿರುಗಿ sigfpe_ಸಂಕೇತ_ಗಮ್ಯ_string(ಸಂಕೇತ_2);
        ಸ್ಥಿತಿ SIGSEGV:
                ಹಿಂತಿರುಗಿ sigsegv_ಸಂಕೇತ_ಗಮ್ಯ_string(ಸಂಕೇತ_2);
        ಸ್ಥಿತಿ SIGBUS:
                ಹಿಂತಿರುಗಿ sigbus_ಸಂಕೇತ_ಗಮ್ಯ_string(ಸಂಕೇತ_2);
        ಸ್ಥಿತಿ SIGTRAP:
                ಹಿಂತಿರುಗಿ sigtrap_ಸಂಕೇತ_ಗಮ್ಯ_string(ಸಂಕೇತ_2);
        ಸ್ಥಿತಿ SIGCHLD:
                ಹಿಂತಿರುಗಿ sigchld_uppercase_ಸಂಕೇತ_ಗಮ್ಯ_string(ಸಂಕೇತ_2);
        ಸ್ಥಿತಿ SIGPOLL:
                ಹಿಂತಿರುಗಿ sigpoll_ಸಂಕೇತ_ಗಮ್ಯ_string(ಸಂಕೇತ_2);
        ಸ್ಥಿತಿ SIGSYS:
                ಹಿಂತಿರುಗಿ sigsys_ಸಂಕೇತ_ಗಮ್ಯ_string(ಸಂಕೇತ_2);
        ಪೂರ್ವನಿಯೋಜಿತ:
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;
        }
}

ಶೂನ್ಯ nop_signal_handler(ಪೂರ್ಣಾಂಕ sig) {
        /* nothing here */
}

ಪೂರ್ಣಾಂಕ signal_is_blocked(ಪೂರ್ಣಾಂಕ sig) {
        sigset_t ss;
        ಪೂರ್ಣಾಂಕ r;

        r = pthread_sigmask(SIG_SETMASK, ಖಾಲಿ, &ss);
        ಆದರೆ (r != 0)
                ಹಿಂತಿರುಗಿ -r;

        ಹಿಂತಿರುಗಿ RET_NERRNO(sigismember(&ss, sig));
}

ಪೂರ್ಣಾಂಕ autoreaping_enabled(ಶೂನ್ಯ) {
        ರಚನೆ sigaction sa;

        ಆದರೆ (sigaction(SIGCHLD, /* act= */ ಖಾಲಿ, &sa) < 0)
                ಹಿಂತಿರುಗಿ -errno;

        ಹಿಂತಿರುಗಿ sa.sa_handler == SIG_IGN || FLAGS_ಗಣ(sa.sa_flags, SA_NOCLDWAIT);
}

ಪೂರ್ಣಾಂಕ ತೆಗೆ_pending_signal_ಆಂತರಿಕ(ಪೂರ್ಣಾಂಕ sig, ...) {
        sigset_t ss;
        va_list ap;
        ಪೂರ್ಣಾಂಕ r;

        ಆದರೆ (sig < 0) /* Empty list? */
                ಹಿಂತಿರುಗಿ -EINVAL;

        ಆದರೆ (sigemptyset(&ss) < 0)
                ಹಿಂತಿರುಗಿ -errno;

        /* Add first signal (if the signal is zero, we'll silently skip it, to make it easier to build
         * parameter lists where some element are sometimes off, similar to how sigset_add_many_ap() handles
         * this.) */
        ಆದರೆ (sig > 0 && sigaddset(&ss, sig) < 0)
                ಹಿಂತಿರುಗಿ -errno;

        /* Add all other signals */
        va_start(ap, sig);
        r = sigset_ಕೂಡಿಸು_many_ap(&ss, ap);
        va_end(ap);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        r = sigtimedwait(&ss, ಖಾಲಿ, &(ಸ್ಥಿರಾಂಕ ರಚನೆ timespec) {});
        ಆದರೆ (r < 0) {
                ಆದರೆ (errno == EAGAIN)
                        ಹಿಂತಿರುಗಿ 0;

                ಹಿಂತಿರುಗಿ -errno;
        }

        ಹಿಂತಿರುಗಿ r; /* Returns the signal popped */
}

ಶೂನ್ಯ propagate_signal(ಪೂರ್ಣಾಂಕ sig, siginfo_t *siginfo) {
        pid_t p;

        /* To be called from a signal handler. Will raise the same signal again, in our process + in our threads.
         *
         * Note that we use getpid() instead of getpid_cached(). We might have forked with raw_clone()
         * earlier (see PID 1), and hence let's go to the raw syscall here. In particular as this is not
         * performance sensitive code.
         *
         * Note that we use kill() rather than raise() as fallback, for similar reasons. */

        p = getpid();

        ಆದರೆ (rt_tgsigqueueinfo(p, gettid(), sig, siginfo) < 0)
                assert_se(kill(p, sig) >= 0);
}

ಸ್ಥಿರಾಂಕ ರಚನೆ sigaction sigaction_ignore = {
        .sa_handler = SIG_IGN,
        .sa_flags = SA_RESTART,
};

ಸ್ಥಿರಾಂಕ ರಚನೆ sigaction sigaction_default = {
        .sa_handler = SIG_DFL,
        .sa_flags = SA_RESTART,
};

ಸ್ಥಿರಾಂಕ ರಚನೆ sigaction sigaction_nop_nocldstop = {
        .sa_handler = nop_signal_handler,
        .sa_flags = SA_NOCLDSTOP|SA_RESTART,
};

ಪೂರ್ಣಾಂಕ parse_signo(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ಪೂರ್ಣಾಂಕ *ret) {
        ಪೂರ್ಣಾಂಕ sig, r;

        r = ಸುರಕ್ಷಿತ_atoi(s, &sig);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (!SIGNAL_VALID(sig))
                ಹಿಂತಿರುಗಿ -EINVAL;

        ಆದರೆ (ret)
                *ret = sig;

        ಹಿಂತಿರುಗಿ 0;
}

ಶೂನ್ಯ sigterm_process_ಗುಂಪು_handler(ಪೂರ್ಣಾಂಕ ಸಂಕೇತ, siginfo_t *info, ಶೂನ್ಯ *ucontext) {
        ಪ್ರತಿಜ್ಞಾ(ಸಂಕೇತ == SIGTERM);
        ಪ್ರತಿಜ್ಞಾ(info);

        /* If the sender is not us, propagate the signal to all processes in
         * the same process group */
        ಆದರೆ (si_ಸಂಕೇತ_from_process(info->si_code) &&
            pid_is_valid(info->si_pid) &&
            info->si_pid != getpid_cached())
                (ಶೂನ್ಯ) kill(0, ಸಂಕೇತ);
}
