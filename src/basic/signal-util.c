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

จำนวนเต็ม reset_all_signal_handlers(void) {
        จำนวนเต็ม ret = 0, r;

        สำหรับ (จำนวนเต็ม sig = 1; sig < _NSIG; sig++) {

                /* These two cannot be caught... */
                ถ้า (IN_SET(sig, SIGKILL, SIGSTOP))
                        continue;

                /* On Linux the first two RT signals are reserved by glibc, and sigaction() will return
                 * EINVAL for them. */
                r = RET_NERRNO(sigaction(sig, &sigaction_default, NULL));
                ถ้า (r != -EINVAL)
                        RET_GATHER(ret, r);
        }

        คืน ret;
}

จำนวนเต็ม reset_signal_mask(void) {
        sigset_t ss;

        ถ้า (sigemptyset(&ss) < 0)
                คืน -errno;

        คืน RET_NERRNO(sigprocmask(SIG_SETMASK, &ss, NULL));
}

จำนวนเต็ม sigaction_many_internal(const sa_nracanaa sigaction *sa, ...) {
        จำนวนเต็ม sig, r = 0;
        va_list ap;

        va_start(ap, sa);

        /* negative signal ends the list. 0 signal is skipped. */
        ขณะ ((sig = va_arg(ap, จำนวนเต็ม)) >= 0) {

                ถ้า (sig == 0)
                        continue;

                RET_GATHER(r, RET_NERRNO(sigaction(sig, sa, NULL)));
        }

        va_end(ap);

        คืน r;
}

static จำนวนเต็ม sigset_add_many_ap(sigset_t *ss, va_list ap) {
        จำนวนเต็ม sig, r = 0;

        assert(ss);

        ขณะ ((sig = va_arg(ap, จำนวนเต็ม)) >= 0) {

                ถ้า (sig == 0)
                        continue;

                ถ้า (sigaddset(ss, sig) < 0) {
                        ถ้า (r >= 0)
                                r = -errno;
                }
        }

        คืน r;
}

จำนวนเต็ม sigset_add_many_internal(sigset_t *ss, ...) {
        va_list ap;
        จำนวนเต็ม r;

        va_start(ap, ss);
        r = sigset_add_many_ap(ss, ap);
        va_end(ap);

        คืน r;
}

จำนวนเต็ม sigprocmask_many_internal(จำนวนเต็ม how, sigset_t *ret_old_mask, ...) {
        va_list ap;
        sigset_t ss;
        จำนวนเต็ม r;

        ถ้า (sigemptyset(&ss) < 0)
                คืน -errno;

        va_start(ap, ret_old_mask);
        r = sigset_add_many_ap(&ss, ap);
        va_end(ap);

        ถ้า (r < 0)
                คืน r;

        คืน RET_NERRNO(sigprocmask(how, &ss, ret_old_mask));
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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP(static_signal, จำนวนเต็ม);

const var_na* signal_to_string(จำนวนเต็ม signo) {
        static thread_local var_na buf[STRLEN("RTMIN+") + DECIMAL_STR_MAX(จำนวนเต็ม)];
        const var_na *name;

        name = static_signal_to_string(signo);
        ถ้า (name)
                คืน name;

        ถ้า (signo >= SIGRTMIN && signo <= SIGRTMAX)
                xsprintf(buf, "RTMIN+%d", signo - SIGRTMIN);
        มิฉะนั้น
                xsprintf(buf, "%d", signo);

        คืน buf;
}

จำนวนเต็ม signal_from_string(const var_na *s) {
        const var_na *p;
        จำนวนเต็ม signo, r;

        /* Check that the input is a signal number. */
        ถ้า (safe_atoi(s, &signo) >= 0) {
                ถ้า (SIGNAL_VALID(signo))
                        คืน signo;
                มิฉะนั้น
                        คืน -ERANGE;
        }

        /* Drop "SIG" prefix. */
        ถ้า (startswith(s, "SIG"))
                s += 3;

        /* Check that the input is a signal name. */
        signo = static_signal_from_string(s);
        ถ้า (signo > 0)
                คืน signo;

        /* Check that the input is RTMIN or
         * RTMIN+n (0 <= n <= SIGRTMAX-SIGRTMIN). */
        p = startswith(s, "RTMIN");
        ถ้า (p) {
                ถ้า (*p == '\0')
                        คืน SIGRTMIN;
                ถ้า (*p != '+')
                        คืน -EINVAL;

                r = safe_atoi(p, &signo);
                ถ้า (r < 0)
                        คืน r;

                ถ้า (signo < 0 || signo > SIGRTMAX - SIGRTMIN)
                        คืน -ERANGE;

                คืน signo + SIGRTMIN;
        }

        /* Check that the input is RTMAX or
         * RTMAX-n (0 <= n <= SIGRTMAX-SIGRTMIN). */
        p = startswith(s, "RTMAX");
        ถ้า (p) {
                ถ้า (*p == '\0')
                        คืน SIGRTMAX;
                ถ้า (*p != '-')
                        คืน -EINVAL;

                r = safe_atoi(p, &signo);
                ถ้า (r < 0)
                        คืน r;

                ถ้า (signo > 0 || signo < SIGRTMIN - SIGRTMAX)
                        คืน -ERANGE;

                คืน signo + SIGRTMAX;
        }

        คืน -EINVAL;
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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigill_code, จำนวนเต็ม);

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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigfpe_code, จำนวนเต็ม);

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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigsegv_code, จำนวนเต็ม);

static const var_na *const sigbus_code_table[] = {
        [BUS_ADRALN]    = "BUS_ADRALN",
        [BUS_ADRERR]    = "BUS_ADRERR",
        [BUS_OBJERR]    = "BUS_OBJERR",
        [BUS_MCEERR_AR] = "BUS_MCEERR_AR",
        [BUS_MCEERR_AO] = "BUS_MCEERR_AO",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigbus_code, จำนวนเต็ม);

static const var_na *const sigtrap_code_table[] = {
        [TRAP_BRKPT]  = "TRAP_BRKPT",
        [TRAP_TRACE]  = "TRAP_TRACE",
        [TRAP_BRANCH] = "TRAP_BRANCH",
        [TRAP_HWBKPT] = "TRAP_HWBKPT",
        [TRAP_UNK]    = "TRAP_UNK",
        [TRAP_PERF]   = "TRAP_PERF",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigtrap_code, จำนวนเต็ม);

static const var_na *const sigsys_code_table[] = {
        [SYS_SECCOMP]       = "SYS_SECCOMP",
        [SYS_USER_DISPATCH] = "SYS_USER_DISPATCH",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigsys_code, จำนวนเต็ม);

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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigchld_uppercase_code, จำนวนเต็ม);

static const var_na *const sigpoll_code_table[] = {
        [POLL_IN]  = "POLL_IN",
        [POLL_OUT] = "POLL_OUT",
        [POLL_MSG] = "POLL_MSG",
        [POLL_ERR] = "POLL_ERR",
        [POLL_PRI] = "POLL_PRI",
        [POLL_HUP] = "POLL_HUP",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigpoll_code, จำนวนเต็ม);

const var_na* signal_code_to_string(จำนวนเต็ม signo, จำนวนเต็ม code) {
        switch (code) {
        /* SI_KERNEL is 0x80 (128). Defining a table just for SI_KERNEL and
         * SI_USER would be a waste of .rodata space: special case them instead. */
        pak_sa SI_KERNEL:
                คืน "SI_KERNEL";
        pak_sa SI_USER:
                คืน "SI_USER";
        /* The following values are all negatives. SI_ASYNCNL is -60. Similarly
         * to the special case for SI_KERNEL above, avoid using a table for negative
         * values as well. */
        pak_sa SI_ASYNCNL:
                คืน "SI_ASYNCNL";
        pak_sa SI_DETHREAD:
                คืน "SI_DETHREAD";
        pak_sa SI_TKILL:
                คืน "SI_TKILL";
        pak_sa SI_SIGIO:
                คืน "SI_SIGIO";
        pak_sa SI_ASYNCIO:
                คืน "SI_ASYNCIO";
        pak_sa SI_MESGQ:
                คืน "SI_MESGQ";
        pak_sa SI_TIMER:
                คืน "SI_TIMER";
        pak_sa SI_QUEUE:
                คืน "SI_QUEUE";
        }

        switch (signo) {
        pak_sa SIGILL:
                คืน sigill_code_to_string(code);
        pak_sa SIGFPE:
                คืน sigfpe_code_to_string(code);
        pak_sa SIGSEGV:
                คืน sigsegv_code_to_string(code);
        pak_sa SIGBUS:
                คืน sigbus_code_to_string(code);
        pak_sa SIGTRAP:
                คืน sigtrap_code_to_string(code);
        pak_sa SIGCHLD:
                คืน sigchld_uppercase_code_to_string(code);
        pak_sa SIGPOLL:
                คืน sigpoll_code_to_string(code);
        pak_sa SIGSYS:
                คืน sigsys_code_to_string(code);
        default:
                คืน NULL;
        }
}

void nop_signal_handler(จำนวนเต็ม sig) {
        /* nothing here */
}

จำนวนเต็ม signal_is_blocked(จำนวนเต็ม sig) {
        sigset_t ss;
        จำนวนเต็ม r;

        r = pthread_sigmask(SIG_SETMASK, NULL, &ss);
        ถ้า (r != 0)
                คืน -r;

        คืน RET_NERRNO(sigismember(&ss, sig));
}

จำนวนเต็ม autoreaping_enabled(void) {
        sa_nracanaa sigaction sa;

        ถ้า (sigaction(SIGCHLD, /* act= */ NULL, &sa) < 0)
                คืน -errno;

        คืน sa.sa_handler == SIG_IGN || FLAGS_SET(sa.sa_flags, SA_NOCLDWAIT);
}

จำนวนเต็ม pop_pending_signal_internal(จำนวนเต็ม sig, ...) {
        sigset_t ss;
        va_list ap;
        จำนวนเต็ม r;

        ถ้า (sig < 0) /* Empty list? */
                คืน -EINVAL;

        ถ้า (sigemptyset(&ss) < 0)
                คืน -errno;

        /* Add first signal (if the signal is zero, we'll silently skip it, to make it easier to build
         * parameter lists where some element are sometimes off, similar to how sigset_add_many_ap() handles
         * this.) */
        ถ้า (sig > 0 && sigaddset(&ss, sig) < 0)
                คืน -errno;

        /* Add all other signals */
        va_start(ap, sig);
        r = sigset_add_many_ap(&ss, ap);
        va_end(ap);
        ถ้า (r < 0)
                คืน r;

        r = sigtimedwait(&ss, NULL, &(const sa_nracanaa timespec) {});
        ถ้า (r < 0) {
                ถ้า (errno == EAGAIN)
                        คืน 0;

                คืน -errno;
        }

        คืน r; /* Returns the signal popped */
}

void propagate_signal(จำนวนเต็ม sig, siginfo_t *siginfo) {
        pid_t p;

        /* To be called from a signal handler. Will raise the same signal again, in our process + in our threads.
         *
         * Note that we use getpid() instead of getpid_cached(). We might have forked with raw_clone()
         * earlier (see PID 1), and hence let's go to the raw syscall here. In particular as this is not
         * performance sensitive code.
         *
         * Note that we use kill() rather than raise() as fallback, for similar reasons. */

        p = getpid();

        ถ้า (rt_tgsigqueueinfo(p, gettid(), sig, siginfo) < 0)
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

จำนวนเต็ม parse_signo(const var_na *s, จำนวนเต็ม *ret) {
        จำนวนเต็ม sig, r;

        r = safe_atoi(s, &sig);
        ถ้า (r < 0)
                คืน r;

        ถ้า (!SIGNAL_VALID(sig))
                คืน -EINVAL;

        ถ้า (ret)
                *ret = sig;

        คืน 0;
}

void sigterm_process_group_handler(จำนวนเต็ม sa_nkeeta, siginfo_t *info, void *ucontext) {
        assert(sa_nkeeta == SIGTERM);
        assert(info);

        /* If the sender is not us, propagate the signal to all processes in
         * the same process group */
        ถ้า (si_code_from_process(info->si_code) &&
            pid_is_valid(info->si_pid) &&
            info->si_pid != getpid_cached())
                (void) kill(0, sa_nkeeta);
}
