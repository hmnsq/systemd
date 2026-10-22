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

цео reset_all_signal_handlers(void) {
        цео ret = 0, r;

        за (цео sig = 1; sig < _NSIG; sig++) {

                /* These two cannot be caught... */
                ако (IN_SET(sig, SIGKILL, SIGSTOP))
                        continue;

                /* On Linux the first two RT signals are reserved by glibc, and sigaction() will return
                 * EINVAL for them. */
                r = RET_NERRNO(sigaction(sig, &sigaction_default, NULL));
                ако (r != -EINVAL)
                        RET_GATHER(ret, r);
        }

        врати ret;
}

цео reset_signal_mask(void) {
        sigset_t ss;

        ако (sigemptyset(&ss) < 0)
                врати -errno;

        врати RET_NERRNO(sigprocmask(SIG_SETMASK, &ss, NULL));
}

цео sigaction_many_internal(const struct sigaction *sa, ...) {
        цео sig, r = 0;
        va_list ap;

        va_start(ap, sa);

        /* negative signal ends the list. 0 signal is skipped. */
        док ((sig = va_arg(ap, цео)) >= 0) {

                ако (sig == 0)
                        continue;

                RET_GATHER(r, RET_NERRNO(sigaction(sig, sa, NULL)));
        }

        va_end(ap);

        врати r;
}

static цео sigset_add_many_ap(sigset_t *ss, va_list ap) {
        цео sig, r = 0;

        assert(ss);

        док ((sig = va_arg(ap, цео)) >= 0) {

                ако (sig == 0)
                        continue;

                ако (sigaddset(ss, sig) < 0) {
                        ако (r >= 0)
                                r = -errno;
                }
        }

        врати r;
}

цео sigset_add_many_internal(sigset_t *ss, ...) {
        va_list ap;
        цео r;

        va_start(ap, ss);
        r = sigset_add_many_ap(ss, ap);
        va_end(ap);

        врати r;
}

цео sigprocmask_many_internal(цео how, sigset_t *ret_old_mask, ...) {
        va_list ap;
        sigset_t ss;
        цео r;

        ако (sigemptyset(&ss) < 0)
                врати -errno;

        va_start(ap, ret_old_mask);
        r = sigset_add_many_ap(&ss, ap);
        va_end(ap);

        ако (r < 0)
                врати r;

        врати RET_NERRNO(sigprocmask(how, &ss, ret_old_mask));
}

static const char *const static_signal_table[] = {
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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP(static_signal, цео);

const char* signal_to_string(цео signo) {
        static thread_local char buf[STRLEN("RTMIN+") + DECIMAL_STR_MAX(цео)];
        const char *name;

        name = static_signal_to_string(signo);
        ако (name)
                врати name;

        ако (signo >= SIGRTMIN && signo <= SIGRTMAX)
                xsprintf(buf, "RTMIN+%d", signo - SIGRTMIN);
        инако
                xsprintf(buf, "%d", signo);

        врати buf;
}

цео signal_from_string(const char *s) {
        const char *p;
        цео signo, r;

        /* Check that the input is a signal number. */
        ако (safe_atoi(s, &signo) >= 0) {
                ако (SIGNAL_VALID(signo))
                        врати signo;
                инако
                        врати -ERANGE;
        }

        /* Drop "SIG" prefix. */
        ако (startswith(s, "SIG"))
                s += 3;

        /* Check that the input is a signal name. */
        signo = static_signal_from_string(s);
        ако (signo > 0)
                врати signo;

        /* Check that the input is RTMIN or
         * RTMIN+n (0 <= n <= SIGRTMAX-SIGRTMIN). */
        p = startswith(s, "RTMIN");
        ако (p) {
                ако (*p == '\0')
                        врати SIGRTMIN;
                ако (*p != '+')
                        врати -EINVAL;

                r = safe_atoi(p, &signo);
                ако (r < 0)
                        врати r;

                ако (signo < 0 || signo > SIGRTMAX - SIGRTMIN)
                        врати -ERANGE;

                врати signo + SIGRTMIN;
        }

        /* Check that the input is RTMAX or
         * RTMAX-n (0 <= n <= SIGRTMAX-SIGRTMIN). */
        p = startswith(s, "RTMAX");
        ако (p) {
                ако (*p == '\0')
                        врати SIGRTMAX;
                ако (*p != '-')
                        врати -EINVAL;

                r = safe_atoi(p, &signo);
                ако (r < 0)
                        врати r;

                ако (signo > 0 || signo < SIGRTMIN - SIGRTMAX)
                        врати -ERANGE;

                врати signo + SIGRTMAX;
        }

        врати -EINVAL;
}

static const char *const sigill_code_table[] = {
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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigill_code, цео);

static const char *const sigfpe_code_table[] = {
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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigfpe_code, цео);

static const char *const sigsegv_code_table[] = {
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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigsegv_code, цео);

static const char *const sigbus_code_table[] = {
        [BUS_ADRALN]    = "BUS_ADRALN",
        [BUS_ADRERR]    = "BUS_ADRERR",
        [BUS_OBJERR]    = "BUS_OBJERR",
        [BUS_MCEERR_AR] = "BUS_MCEERR_AR",
        [BUS_MCEERR_AO] = "BUS_MCEERR_AO",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigbus_code, цео);

static const char *const sigtrap_code_table[] = {
        [TRAP_BRKPT]  = "TRAP_BRKPT",
        [TRAP_TRACE]  = "TRAP_TRACE",
        [TRAP_BRANCH] = "TRAP_BRANCH",
        [TRAP_HWBKPT] = "TRAP_HWBKPT",
        [TRAP_UNK]    = "TRAP_UNK",
        [TRAP_PERF]   = "TRAP_PERF",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigtrap_code, цео);

static const char *const sigsys_code_table[] = {
        [SYS_SECCOMP]       = "SYS_SECCOMP",
        [SYS_USER_DISPATCH] = "SYS_USER_DISPATCH",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigsys_code, цео);

/* sigchld_code_table is already defined in src/basic/process-util.c with
 * decoded, lower-case values (CLD_EXITED -> "exited"). For consistency with
 * all other values decoded here, provide an uppercase variant. */
static const char *const sigchld_uppercase_code_table[] = {
        [CLD_EXITED]    = "CLD_EXITED",
        [CLD_KILLED]    = "CLD_KILLED",
        [CLD_DUMPED]    = "CLD_DUMPED",
        [CLD_TRAPPED]   = "CLD_TRAPPED",
        [CLD_STOPPED]   = "CLD_STOPPED",
        [CLD_CONTINUED] = "CLD_CONTINUED",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigchld_uppercase_code, цео);

static const char *const sigpoll_code_table[] = {
        [POLL_IN]  = "POLL_IN",
        [POLL_OUT] = "POLL_OUT",
        [POLL_MSG] = "POLL_MSG",
        [POLL_ERR] = "POLL_ERR",
        [POLL_PRI] = "POLL_PRI",
        [POLL_HUP] = "POLL_HUP",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigpoll_code, цео);

const char* signal_code_to_string(цео signo, цео code) {
        switch (code) {
        /* SI_KERNEL is 0x80 (128). Defining a table just for SI_KERNEL and
         * SI_USER would be a waste of .rodata space: special case them instead. */
        case SI_KERNEL:
                врати "SI_KERNEL";
        case SI_USER:
                врати "SI_USER";
        /* The following values are all negatives. SI_ASYNCNL is -60. Similarly
         * to the special case for SI_KERNEL above, avoid using a table for negative
         * values as well. */
        case SI_ASYNCNL:
                врати "SI_ASYNCNL";
        case SI_DETHREAD:
                врати "SI_DETHREAD";
        case SI_TKILL:
                врати "SI_TKILL";
        case SI_SIGIO:
                врати "SI_SIGIO";
        case SI_ASYNCIO:
                врати "SI_ASYNCIO";
        case SI_MESGQ:
                врати "SI_MESGQ";
        case SI_TIMER:
                врати "SI_TIMER";
        case SI_QUEUE:
                врати "SI_QUEUE";
        }

        switch (signo) {
        case SIGILL:
                врати sigill_code_to_string(code);
        case SIGFPE:
                врати sigfpe_code_to_string(code);
        case SIGSEGV:
                врати sigsegv_code_to_string(code);
        case SIGBUS:
                врати sigbus_code_to_string(code);
        case SIGTRAP:
                врати sigtrap_code_to_string(code);
        case SIGCHLD:
                врати sigchld_uppercase_code_to_string(code);
        case SIGPOLL:
                врати sigpoll_code_to_string(code);
        case SIGSYS:
                врати sigsys_code_to_string(code);
        default:
                врати NULL;
        }
}

void nop_signal_handler(цео sig) {
        /* nothing here */
}

цео signal_is_blocked(цео sig) {
        sigset_t ss;
        цео r;

        r = pthread_sigmask(SIG_SETMASK, NULL, &ss);
        ако (r != 0)
                врати -r;

        врати RET_NERRNO(sigismember(&ss, sig));
}

цео autoreaping_enabled(void) {
        struct sigaction sa;

        ако (sigaction(SIGCHLD, /* act= */ NULL, &sa) < 0)
                врати -errno;

        врати sa.sa_handler == SIG_IGN || FLAGS_SET(sa.sa_flags, SA_NOCLDWAIT);
}

цео pop_pending_signal_internal(цео sig, ...) {
        sigset_t ss;
        va_list ap;
        цео r;

        ако (sig < 0) /* Empty list? */
                врати -EINVAL;

        ако (sigemptyset(&ss) < 0)
                врати -errno;

        /* Add first signal (if the signal is zero, we'll silently skip it, to make it easier to build
         * parameter lists where some element are sometimes off, similar to how sigset_add_many_ap() handles
         * this.) */
        ако (sig > 0 && sigaddset(&ss, sig) < 0)
                врати -errno;

        /* Add all other signals */
        va_start(ap, sig);
        r = sigset_add_many_ap(&ss, ap);
        va_end(ap);
        ако (r < 0)
                врати r;

        r = sigtimedwait(&ss, NULL, &(const struct timespec) {});
        ако (r < 0) {
                ако (errno == EAGAIN)
                        врати 0;

                врати -errno;
        }

        врати r; /* Returns the signal popped */
}

void propagate_signal(цео sig, siginfo_t *siginfo) {
        pid_t p;

        /* To be called from a signal handler. Will raise the same signal again, in our process + in our threads.
         *
         * Note that we use getpid() instead of getpid_cached(). We might have forked with raw_clone()
         * earlier (see PID 1), and hence let's go to the raw syscall here. In particular as this is not
         * performance sensitive code.
         *
         * Note that we use kill() rather than raise() as fallback, for similar reasons. */

        p = getpid();

        ако (rt_tgsigqueueinfo(p, gettid(), sig, siginfo) < 0)
                assert_se(kill(p, sig) >= 0);
}

const struct sigaction sigaction_ignore = {
        .sa_handler = SIG_IGN,
        .sa_flags = SA_RESTART,
};

const struct sigaction sigaction_default = {
        .sa_handler = SIG_DFL,
        .sa_flags = SA_RESTART,
};

const struct sigaction sigaction_nop_nocldstop = {
        .sa_handler = nop_signal_handler,
        .sa_flags = SA_NOCLDSTOP|SA_RESTART,
};

цео parse_signo(const char *s, цео *ret) {
        цео sig, r;

        r = safe_atoi(s, &sig);
        ако (r < 0)
                врати r;

        ако (!SIGNAL_VALID(sig))
                врати -EINVAL;

        ако (ret)
                *ret = sig;

        врати 0;
}

void sigterm_process_group_handler(цео signal, siginfo_t *info, void *ucontext) {
        assert(signal == SIGTERM);
        assert(info);

        /* If the sender is not us, propagate the signal to all processes in
         * the same process group */
        ако (si_code_from_process(info->si_code) &&
            pid_is_valid(info->si_pid) &&
            info->si_pid != getpid_cached())
                (void) kill(0, signal);
}
