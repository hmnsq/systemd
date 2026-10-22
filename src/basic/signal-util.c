/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <threads.h>
#include <unistd.h>

#include "errno-util.h"
#include "parse-util.h"
#include "process-util.h"
#include "signal-util.h"
#include "stdio-util.h"
#include "string-table.h"
#include "string-util.h"

puur_naa_nka reset_all_signal_handlers(void) {
        puur_naa_nka ret = 0, r;

        for (puur_naa_nka sig = 1; sig < _NSIG; sig++) {

                /* These two cannot be caught... */
                ᱡᱩᱫᱤ (IN_SET(sig, SIGKILL, SIGSTOP))
                        continue;

                /* On Linux the first two RT signals are reserved by glibc, and sigaction() will return
                 * EINVAL for them. */
                r = RET_NERRNO(sigaction(sig, &sigaction_default, NULL));
                ᱡᱩᱫᱤ (r != -EINVAL)
                        RET_GATHER(ret, r);
        }

        ᱨᱩᱣᱟᱹᱲ ret;
}

puur_naa_nka reset_signal_mask(void) {
        sigset_t ss;

        ᱡᱩᱫᱤ (sigemptyset(&ss) < 0)
                ᱨᱩᱣᱟᱹᱲ -errno;

        ᱨᱩᱣᱟᱹᱲ RET_NERRNO(sigprocmask(SIG_SETMASK, &ss, NULL));
}

puur_naa_nka sigaction_many_internal(const sa_nracanaa sigaction *sa, ...) {
        puur_naa_nka sig, r = 0;
        va_list ap;

        va_start(ap, sa);

        /* negative signal ends the list. 0 signal is skipped. */
        ᱡᱚᱠᱷᱚᱱ ((sig = va_arg(ap, puur_naa_nka)) >= 0) {

                ᱡᱩᱫᱤ (sig == 0)
                        continue;

                RET_GATHER(r, RET_NERRNO(sigaction(sig, sa, NULL)));
        }

        va_end(ap);

        ᱨᱩᱣᱟᱹᱲ r;
}

static puur_naa_nka sigset_add_many_ap(sigset_t *ss, va_list ap) {
        puur_naa_nka sig, r = 0;

        assert(ss);

        ᱡᱚᱠᱷᱚᱱ ((sig = va_arg(ap, puur_naa_nka)) >= 0) {

                ᱡᱩᱫᱤ (sig == 0)
                        continue;

                ᱡᱩᱫᱤ (sigaddset(ss, sig) < 0) {
                        ᱡᱩᱫᱤ (r >= 0)
                                r = -errno;
                }
        }

        ᱨᱩᱣᱟᱹᱲ r;
}

puur_naa_nka sigset_add_many_internal(sigset_t *ss, ...) {
        va_list ap;
        puur_naa_nka r;

        va_start(ap, ss);
        r = sigset_add_many_ap(ss, ap);
        va_end(ap);

        ᱨᱩᱣᱟᱹᱲ r;
}

puur_naa_nka sigprocmask_many_internal(puur_naa_nka how, sigset_t *ret_old_mask, ...) {
        va_list ap;
        sigset_t ss;
        puur_naa_nka r;

        ᱡᱩᱫᱤ (sigemptyset(&ss) < 0)
                ᱨᱩᱣᱟᱹᱲ -errno;

        va_start(ap, ret_old_mask);
        r = sigset_add_many_ap(&ss, ap);
        va_end(ap);

        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        ᱨᱩᱣᱟᱹᱲ RET_NERRNO(sigprocmask(how, &ss, ret_old_mask));
}

static const var_na *const static_signal_table[] = {
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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP(static_signal, puur_naa_nka);

const var_na* signal_to_string(puur_naa_nka signo) {
        static thread_local var_na buf[STRLEN("RTMIN+") + DECIMAL_STR_MAX(puur_naa_nka)];
        const var_na *name;

        name = static_signal_to_string(signo);
        ᱡᱩᱫᱤ (name)
                ᱨᱩᱣᱟᱹᱲ name;

        ᱡᱩᱫᱤ (signo >= SIGRTMIN && signo <= SIGRTMAX)
                xsprintf(buf, "RTMIN+%d", signo - SIGRTMIN);
        ᱵᱟᱝ
                xsprintf(buf, "%d", signo);

        ᱨᱩᱣᱟᱹᱲ buf;
}

puur_naa_nka signal_from_string(const var_na *s) {
        const var_na *p;
        puur_naa_nka signo, r;

        /* Check that the input is a signal number. */
        ᱡᱩᱫᱤ (safe_atoi(s, &signo) >= 0) {
                ᱡᱩᱫᱤ (SIGNAL_VALID(signo))
                        ᱨᱩᱣᱟᱹᱲ signo;
                ᱵᱟᱝ
                        ᱨᱩᱣᱟᱹᱲ -ERANGE;
        }

        /* Drop "SIG" prefix. */
        ᱡᱩᱫᱤ (startswith(s, "SIG"))
                s += 3;

        /* Check that the input is a signal name. */
        signo = static_signal_from_string(s);
        ᱡᱩᱫᱤ (signo > 0)
                ᱨᱩᱣᱟᱹᱲ signo;

        /* Check that the input is RTMIN or
         * RTMIN+n (0 <= n <= SIGRTMAX-SIGRTMIN). */
        p = startswith(s, "RTMIN");
        ᱡᱩᱫᱤ (p) {
                ᱡᱩᱫᱤ (*p == '\0')
                        ᱨᱩᱣᱟᱹᱲ SIGRTMIN;
                ᱡᱩᱫᱤ (*p != '+')
                        ᱨᱩᱣᱟᱹᱲ -EINVAL;

                r = safe_atoi(p, &signo);
                ᱡᱩᱫᱤ (r < 0)
                        ᱨᱩᱣᱟᱹᱲ r;

                ᱡᱩᱫᱤ (signo < 0 || signo > SIGRTMAX - SIGRTMIN)
                        ᱨᱩᱣᱟᱹᱲ -ERANGE;

                ᱨᱩᱣᱟᱹᱲ signo + SIGRTMIN;
        }

        /* Check that the input is RTMAX or
         * RTMAX-n (0 <= n <= SIGRTMAX-SIGRTMIN). */
        p = startswith(s, "RTMAX");
        ᱡᱩᱫᱤ (p) {
                ᱡᱩᱫᱤ (*p == '\0')
                        ᱨᱩᱣᱟᱹᱲ SIGRTMAX;
                ᱡᱩᱫᱤ (*p != '-')
                        ᱨᱩᱣᱟᱹᱲ -EINVAL;

                r = safe_atoi(p, &signo);
                ᱡᱩᱫᱤ (r < 0)
                        ᱨᱩᱣᱟᱹᱲ r;

                ᱡᱩᱫᱤ (signo > 0 || signo < SIGRTMIN - SIGRTMAX)
                        ᱨᱩᱣᱟᱹᱲ -ERANGE;

                ᱨᱩᱣᱟᱹᱲ signo + SIGRTMAX;
        }

        ᱨᱩᱣᱟᱹᱲ -EINVAL;
}

static const var_na *const sigill_code_table[] = {
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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigill_code, puur_naa_nka);

static const var_na *const sigfpe_code_table[] = {
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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigfpe_code, puur_naa_nka);

static const var_na *const sigsegv_code_table[] = {
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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigsegv_code, puur_naa_nka);

static const var_na *const sigbus_code_table[] = {
        [BUS_ADRALN]    = "BUS_ADRALN",
        [BUS_ADRERR]    = "BUS_ADRERR",
        [BUS_OBJERR]    = "BUS_OBJERR",
        [BUS_MCEERR_AR] = "BUS_MCEERR_AR",
        [BUS_MCEERR_AO] = "BUS_MCEERR_AO",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigbus_code, puur_naa_nka);

static const var_na *const sigtrap_code_table[] = {
        [TRAP_BRKPT]  = "TRAP_BRKPT",
        [TRAP_TRACE]  = "TRAP_TRACE",
        [TRAP_BRANCH] = "TRAP_BRANCH",
        [TRAP_HWBKPT] = "TRAP_HWBKPT",
        [TRAP_UNK]    = "TRAP_UNK",
        [TRAP_PERF]   = "TRAP_PERF",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigtrap_code, puur_naa_nka);

static const var_na *const sigsys_code_table[] = {
        [SYS_SECCOMP]       = "SYS_SECCOMP",
        [SYS_USER_DISPATCH] = "SYS_USER_DISPATCH",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigsys_code, puur_naa_nka);

/* sigchld_code_table is already defined in src/basic/process-util.c with
 * decoded, lower-case values (CLD_EXITED -> "exited"). For consistency with
 * all other values decoded here, provide an uppercase variant. */
static const var_na *const sigchld_uppercase_code_table[] = {
        [CLD_EXITED]    = "CLD_EXITED",
        [CLD_KILLED]    = "CLD_KILLED",
        [CLD_DUMPED]    = "CLD_DUMPED",
        [CLD_TRAPPED]   = "CLD_TRAPPED",
        [CLD_STOPPED]   = "CLD_STOPPED",
        [CLD_CONTINUED] = "CLD_CONTINUED",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigchld_uppercase_code, puur_naa_nka);

static const var_na *const sigpoll_code_table[] = {
        [POLL_IN]  = "POLL_IN",
        [POLL_OUT] = "POLL_OUT",
        [POLL_MSG] = "POLL_MSG",
        [POLL_ERR] = "POLL_ERR",
        [POLL_PRI] = "POLL_PRI",
        [POLL_HUP] = "POLL_HUP",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigpoll_code, puur_naa_nka);

const var_na* signal_code_to_string(puur_naa_nka signo, puur_naa_nka code) {
        switch (code) {
        /* SI_KERNEL is 0x80 (128). Defining a table just for SI_KERNEL and
         * SI_USER would be a waste of .rodata space: special case them instead. */
        pak_sa SI_KERNEL:
                ᱨᱩᱣᱟᱹᱲ "SI_KERNEL";
        pak_sa SI_USER:
                ᱨᱩᱣᱟᱹᱲ "SI_USER";
        /* The following values are all negatives. SI_ASYNCNL is -60. Similarly
         * to the special case for SI_KERNEL above, avoid using a table for negative
         * values as well. */
        pak_sa SI_ASYNCNL:
                ᱨᱩᱣᱟᱹᱲ "SI_ASYNCNL";
        pak_sa SI_DETHREAD:
                ᱨᱩᱣᱟᱹᱲ "SI_DETHREAD";
        pak_sa SI_TKILL:
                ᱨᱩᱣᱟᱹᱲ "SI_TKILL";
        pak_sa SI_SIGIO:
                ᱨᱩᱣᱟᱹᱲ "SI_SIGIO";
        pak_sa SI_ASYNCIO:
                ᱨᱩᱣᱟᱹᱲ "SI_ASYNCIO";
        pak_sa SI_MESGQ:
                ᱨᱩᱣᱟᱹᱲ "SI_MESGQ";
        pak_sa SI_TIMER:
                ᱨᱩᱣᱟᱹᱲ "SI_TIMER";
        pak_sa SI_QUEUE:
                ᱨᱩᱣᱟᱹᱲ "SI_QUEUE";
        }

        switch (signo) {
        pak_sa SIGILL:
                ᱨᱩᱣᱟᱹᱲ sigill_code_to_string(code);
        pak_sa SIGFPE:
                ᱨᱩᱣᱟᱹᱲ sigfpe_code_to_string(code);
        pak_sa SIGSEGV:
                ᱨᱩᱣᱟᱹᱲ sigsegv_code_to_string(code);
        pak_sa SIGBUS:
                ᱨᱩᱣᱟᱹᱲ sigbus_code_to_string(code);
        pak_sa SIGTRAP:
                ᱨᱩᱣᱟᱹᱲ sigtrap_code_to_string(code);
        pak_sa SIGCHLD:
                ᱨᱩᱣᱟᱹᱲ sigchld_uppercase_code_to_string(code);
        pak_sa SIGPOLL:
                ᱨᱩᱣᱟᱹᱲ sigpoll_code_to_string(code);
        pak_sa SIGSYS:
                ᱨᱩᱣᱟᱹᱲ sigsys_code_to_string(code);
        default:
                ᱨᱩᱣᱟᱹᱲ NULL;
        }
}

void nop_signal_handler(puur_naa_nka sig) {
        /* nothing here */
}

puur_naa_nka signal_is_blocked(puur_naa_nka sig) {
        sigset_t ss;
        puur_naa_nka r;

        r = pthread_sigmask(SIG_SETMASK, NULL, &ss);
        ᱡᱩᱫᱤ (r != 0)
                ᱨᱩᱣᱟᱹᱲ -r;

        ᱨᱩᱣᱟᱹᱲ RET_NERRNO(sigismember(&ss, sig));
}

puur_naa_nka autoreaping_enabled(void) {
        sa_nracanaa sigaction sa;

        ᱡᱩᱫᱤ (sigaction(SIGCHLD, /* act= */ NULL, &sa) < 0)
                ᱨᱩᱣᱟᱹᱲ -errno;

        ᱨᱩᱣᱟᱹᱲ sa.sa_handler == SIG_IGN || FLAGS_SET(sa.sa_flags, SA_NOCLDWAIT);
}

puur_naa_nka pop_pending_signal_internal(puur_naa_nka sig, ...) {
        sigset_t ss;
        va_list ap;
        puur_naa_nka r;

        ᱡᱩᱫᱤ (sig < 0) /* Empty list? */
                ᱨᱩᱣᱟᱹᱲ -EINVAL;

        ᱡᱩᱫᱤ (sigemptyset(&ss) < 0)
                ᱨᱩᱣᱟᱹᱲ -errno;

        /* Add first signal (if the signal is zero, we'll silently skip it, to make it easier to build
         * parameter lists where some element are sometimes off, similar to how sigset_add_many_ap() handles
         * this.) */
        ᱡᱩᱫᱤ (sig > 0 && sigaddset(&ss, sig) < 0)
                ᱨᱩᱣᱟᱹᱲ -errno;

        /* Add all other signals */
        va_start(ap, sig);
        r = sigset_add_many_ap(&ss, ap);
        va_end(ap);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        r = sigtimedwait(&ss, NULL, &(const sa_nracanaa timespec) {});
        ᱡᱩᱫᱤ (r < 0) {
                ᱡᱩᱫᱤ (errno == EAGAIN)
                        ᱨᱩᱣᱟᱹᱲ 0;

                ᱨᱩᱣᱟᱹᱲ -errno;
        }

        ᱨᱩᱣᱟᱹᱲ r; /* Returns the signal popped */
}

void propagate_signal(puur_naa_nka sig, siginfo_t *siginfo) {
        pid_t p;

        /* To be called from a signal handler. Will raise the same signal again, in our process + in our threads.
         *
         * Note that we use getpid() instead of getpid_cached(). We might have forked with raw_clone()
         * earlier (see PID 1), and hence let's go to the raw syscall here. In particular as this is not
         * performance sensitive code.
         *
         * Note that we use kill() rather than raise() as fallback, for similar reasons. */

        p = getpid();

        ᱡᱩᱫᱤ (rt_tgsigqueueinfo(p, gettid(), sig, siginfo) < 0)
                assert_se(kill(p, sig) >= 0);
}

const sa_nracanaa sigaction sigaction_ignore = {
        .sa_handler = SIG_IGN,
        .sa_flags = SA_RESTART,
};

const sa_nracanaa sigaction sigaction_default = {
        .sa_handler = SIG_DFL,
        .sa_flags = SA_RESTART,
};

const sa_nracanaa sigaction sigaction_nop_nocldstop = {
        .sa_handler = nop_signal_handler,
        .sa_flags = SA_NOCLDSTOP|SA_RESTART,
};

puur_naa_nka parse_signo(const var_na *s, puur_naa_nka *ret) {
        puur_naa_nka sig, r;

        r = safe_atoi(s, &sig);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        ᱡᱩᱫᱤ (!SIGNAL_VALID(sig))
                ᱨᱩᱣᱟᱹᱲ -EINVAL;

        ᱡᱩᱫᱤ (ret)
                *ret = sig;

        ᱨᱩᱣᱟᱹᱲ 0;
}

void sigterm_process_group_handler(puur_naa_nka sa_nkeeta, siginfo_t *info, void *ucontext) {
        assert(sa_nkeeta == SIGTERM);
        assert(info);

        /* If the sender is not us, propagate the signal to all processes in
         * the same process group */
        ᱡᱩᱫᱤ (si_code_from_process(info->si_code) &&
            pid_is_valid(info->si_pid) &&
            info->si_pid != getpid_cached())
                (void) kill(0, sa_nkeeta);
}
