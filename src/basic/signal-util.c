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

целое reset_all_signal_handlers(void) {
        целое ret = 0, r;

        для (целое sig = 1; sig < _NSIG; sig++) {

                /* These two cannot be caught... */
                если (IN_SET(sig, SIGKILL, SIGSTOP))
                        continue;

                /* On Linux the first two RT signals are reserved by glibc, and sigaction() will return
                 * EINVAL for them. */
                r = RET_NERRNO(sigaction(sig, &sigaction_default, NULL));
                если (r != -EINVAL)
                        RET_GATHER(ret, r);
        }

        вернуть ret;
}

целое reset_signal_mask(void) {
        sigset_t ss;

        если (sigemptyset(&ss) < 0)
                вернуть -errno;

        вернуть RET_NERRNO(sigprocmask(SIG_SETMASK, &ss, NULL));
}

целое sigaction_many_internal(const struct sigaction *sa, ...) {
        целое sig, r = 0;
        va_list ap;

        va_start(ap, sa);

        /* negative signal ends the list. 0 signal is skipped. */
        пока ((sig = va_arg(ap, целое)) >= 0) {

                если (sig == 0)
                        continue;

                RET_GATHER(r, RET_NERRNO(sigaction(sig, sa, NULL)));
        }

        va_end(ap);

        вернуть r;
}

static целое sigset_add_many_ap(sigset_t *ss, va_list ap) {
        целое sig, r = 0;

        assert(ss);

        пока ((sig = va_arg(ap, целое)) >= 0) {

                если (sig == 0)
                        continue;

                если (sigaddset(ss, sig) < 0) {
                        если (r >= 0)
                                r = -errno;
                }
        }

        вернуть r;
}

целое sigset_add_many_internal(sigset_t *ss, ...) {
        va_list ap;
        целое r;

        va_start(ap, ss);
        r = sigset_add_many_ap(ss, ap);
        va_end(ap);

        вернуть r;
}

целое sigprocmask_many_internal(целое how, sigset_t *ret_old_mask, ...) {
        va_list ap;
        sigset_t ss;
        целое r;

        если (sigemptyset(&ss) < 0)
                вернуть -errno;

        va_start(ap, ret_old_mask);
        r = sigset_add_many_ap(&ss, ap);
        va_end(ap);

        если (r < 0)
                вернуть r;

        вернуть RET_NERRNO(sigprocmask(how, &ss, ret_old_mask));
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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP(static_signal, целое);

const char* signal_to_string(целое signo) {
        static thread_local char buf[STRLEN("RTMIN+") + DECIMAL_STR_MAX(целое)];
        const char *name;

        name = static_signal_to_string(signo);
        если (name)
                вернуть name;

        если (signo >= SIGRTMIN && signo <= SIGRTMAX)
                xsprintf(buf, "RTMIN+%d", signo - SIGRTMIN);
        иначе
                xsprintf(buf, "%d", signo);

        вернуть buf;
}

целое signal_from_string(const char *s) {
        const char *p;
        целое signo, r;

        /* Check that the input is a signal number. */
        если (safe_atoi(s, &signo) >= 0) {
                если (SIGNAL_VALID(signo))
                        вернуть signo;
                иначе
                        вернуть -ERANGE;
        }

        /* Drop "SIG" prefix. */
        если (startswith(s, "SIG"))
                s += 3;

        /* Check that the input is a signal name. */
        signo = static_signal_from_string(s);
        если (signo > 0)
                вернуть signo;

        /* Check that the input is RTMIN or
         * RTMIN+n (0 <= n <= SIGRTMAX-SIGRTMIN). */
        p = startswith(s, "RTMIN");
        если (p) {
                если (*p == '\0')
                        вернуть SIGRTMIN;
                если (*p != '+')
                        вернуть -EINVAL;

                r = safe_atoi(p, &signo);
                если (r < 0)
                        вернуть r;

                если (signo < 0 || signo > SIGRTMAX - SIGRTMIN)
                        вернуть -ERANGE;

                вернуть signo + SIGRTMIN;
        }

        /* Check that the input is RTMAX or
         * RTMAX-n (0 <= n <= SIGRTMAX-SIGRTMIN). */
        p = startswith(s, "RTMAX");
        если (p) {
                если (*p == '\0')
                        вернуть SIGRTMAX;
                если (*p != '-')
                        вернуть -EINVAL;

                r = safe_atoi(p, &signo);
                если (r < 0)
                        вернуть r;

                если (signo > 0 || signo < SIGRTMIN - SIGRTMAX)
                        вернуть -ERANGE;

                вернуть signo + SIGRTMAX;
        }

        вернуть -EINVAL;
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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigill_code, целое);

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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigfpe_code, целое);

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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigsegv_code, целое);

static const char *const sigbus_code_table[] = {
        [BUS_ADRALN]    = "BUS_ADRALN",
        [BUS_ADRERR]    = "BUS_ADRERR",
        [BUS_OBJERR]    = "BUS_OBJERR",
        [BUS_MCEERR_AR] = "BUS_MCEERR_AR",
        [BUS_MCEERR_AO] = "BUS_MCEERR_AO",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigbus_code, целое);

static const char *const sigtrap_code_table[] = {
        [TRAP_BRKPT]  = "TRAP_BRKPT",
        [TRAP_TRACE]  = "TRAP_TRACE",
        [TRAP_BRANCH] = "TRAP_BRANCH",
        [TRAP_HWBKPT] = "TRAP_HWBKPT",
        [TRAP_UNK]    = "TRAP_UNK",
        [TRAP_PERF]   = "TRAP_PERF",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigtrap_code, целое);

static const char *const sigsys_code_table[] = {
        [SYS_SECCOMP]       = "SYS_SECCOMP",
        [SYS_USER_DISPATCH] = "SYS_USER_DISPATCH",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigsys_code, целое);

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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigchld_uppercase_code, целое);

static const char *const sigpoll_code_table[] = {
        [POLL_IN]  = "POLL_IN",
        [POLL_OUT] = "POLL_OUT",
        [POLL_MSG] = "POLL_MSG",
        [POLL_ERR] = "POLL_ERR",
        [POLL_PRI] = "POLL_PRI",
        [POLL_HUP] = "POLL_HUP",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigpoll_code, целое);

const char* signal_code_to_string(целое signo, целое code) {
        switch (code) {
        /* SI_KERNEL is 0x80 (128). Defining a table just for SI_KERNEL and
         * SI_USER would be a waste of .rodata space: special case them instead. */
        case SI_KERNEL:
                вернуть "SI_KERNEL";
        case SI_USER:
                вернуть "SI_USER";
        /* The following values are all negatives. SI_ASYNCNL is -60. Similarly
         * to the special case for SI_KERNEL above, avoid using a table for negative
         * values as well. */
        case SI_ASYNCNL:
                вернуть "SI_ASYNCNL";
        case SI_DETHREAD:
                вернуть "SI_DETHREAD";
        case SI_TKILL:
                вернуть "SI_TKILL";
        case SI_SIGIO:
                вернуть "SI_SIGIO";
        case SI_ASYNCIO:
                вернуть "SI_ASYNCIO";
        case SI_MESGQ:
                вернуть "SI_MESGQ";
        case SI_TIMER:
                вернуть "SI_TIMER";
        case SI_QUEUE:
                вернуть "SI_QUEUE";
        }

        switch (signo) {
        case SIGILL:
                вернуть sigill_code_to_string(code);
        case SIGFPE:
                вернуть sigfpe_code_to_string(code);
        case SIGSEGV:
                вернуть sigsegv_code_to_string(code);
        case SIGBUS:
                вернуть sigbus_code_to_string(code);
        case SIGTRAP:
                вернуть sigtrap_code_to_string(code);
        case SIGCHLD:
                вернуть sigchld_uppercase_code_to_string(code);
        case SIGPOLL:
                вернуть sigpoll_code_to_string(code);
        case SIGSYS:
                вернуть sigsys_code_to_string(code);
        default:
                вернуть NULL;
        }
}

void nop_signal_handler(целое sig) {
        /* nothing here */
}

целое signal_is_blocked(целое sig) {
        sigset_t ss;
        целое r;

        r = pthread_sigmask(SIG_SETMASK, NULL, &ss);
        если (r != 0)
                вернуть -r;

        вернуть RET_NERRNO(sigismember(&ss, sig));
}

целое autoreaping_enabled(void) {
        struct sigaction sa;

        если (sigaction(SIGCHLD, /* act= */ NULL, &sa) < 0)
                вернуть -errno;

        вернуть sa.sa_handler == SIG_IGN || FLAGS_SET(sa.sa_flags, SA_NOCLDWAIT);
}

целое pop_pending_signal_internal(целое sig, ...) {
        sigset_t ss;
        va_list ap;
        целое r;

        если (sig < 0) /* Empty list? */
                вернуть -EINVAL;

        если (sigemptyset(&ss) < 0)
                вернуть -errno;

        /* Add first signal (if the signal is zero, we'll silently skip it, to make it easier to build
         * parameter lists where some element are sometimes off, similar to how sigset_add_many_ap() handles
         * this.) */
        если (sig > 0 && sigaddset(&ss, sig) < 0)
                вернуть -errno;

        /* Add all other signals */
        va_start(ap, sig);
        r = sigset_add_many_ap(&ss, ap);
        va_end(ap);
        если (r < 0)
                вернуть r;

        r = sigtimedwait(&ss, NULL, &(const struct timespec) {});
        если (r < 0) {
                если (errno == EAGAIN)
                        вернуть 0;

                вернуть -errno;
        }

        вернуть r; /* Returns the signal popped */
}

void propagate_signal(целое sig, siginfo_t *siginfo) {
        pid_t p;

        /* To be called from a signal handler. Will raise the same signal again, in our process + in our threads.
         *
         * Note that we use getpid() instead of getpid_cached(). We might have forked with raw_clone()
         * earlier (see PID 1), and hence let's go to the raw syscall here. In particular as this is not
         * performance sensitive code.
         *
         * Note that we use kill() rather than raise() as fallback, for similar reasons. */

        p = getpid();

        если (rt_tgsigqueueinfo(p, gettid(), sig, siginfo) < 0)
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

целое parse_signo(const char *s, целое *ret) {
        целое sig, r;

        r = safe_atoi(s, &sig);
        если (r < 0)
                вернуть r;

        если (!SIGNAL_VALID(sig))
                вернуть -EINVAL;

        если (ret)
                *ret = sig;

        вернуть 0;
}

void sigterm_process_group_handler(целое signal, siginfo_t *info, void *ucontext) {
        assert(signal == SIGTERM);
        assert(info);

        /* If the sender is not us, propagate the signal to all processes in
         * the same process group */
        если (si_code_from_process(info->si_code) &&
            pid_is_valid(info->si_pid) &&
            info->si_pid != getpid_cached())
                (void) kill(0, signal);
}
