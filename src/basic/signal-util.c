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

整数 reset_all_signal_handlers(void) {
        整数 ret = 0, r;

        为 (整数 sig = 1; sig < _NSIG; sig++) {

                /* These two cannot be caught... */
                如果 (IN_SET(sig, SIGKILL, SIGSTOP))
                        continue;

                /* On Linux the first two RT signals are reserved by glibc, and sigaction() will return
                 * EINVAL for them. */
                r = RET_NERRNO(sigaction(sig, &sigaction_default, NULL));
                如果 (r != -EINVAL)
                        RET_GATHER(ret, r);
        }

        返回 ret;
}

整数 reset_signal_mask(void) {
        sigset_t ss;

        如果 (sigemptyset(&ss) < 0)
                返回 -errno;

        返回 RET_NERRNO(sigprocmask(SIG_SETMASK, &ss, NULL));
}

整数 sigaction_many_internal(const struct sigaction *sa, ...) {
        整数 sig, r = 0;
        va_list ap;

        va_start(ap, sa);

        /* negative signal ends the list. 0 signal is skipped. */
        当 ((sig = va_arg(ap, 整数)) >= 0) {

                如果 (sig == 0)
                        continue;

                RET_GATHER(r, RET_NERRNO(sigaction(sig, sa, NULL)));
        }

        va_end(ap);

        返回 r;
}

static 整数 sigset_add_many_ap(sigset_t *ss, va_list ap) {
        整数 sig, r = 0;

        assert(ss);

        当 ((sig = va_arg(ap, 整数)) >= 0) {

                如果 (sig == 0)
                        continue;

                如果 (sigaddset(ss, sig) < 0) {
                        如果 (r >= 0)
                                r = -errno;
                }
        }

        返回 r;
}

整数 sigset_add_many_internal(sigset_t *ss, ...) {
        va_list ap;
        整数 r;

        va_start(ap, ss);
        r = sigset_add_many_ap(ss, ap);
        va_end(ap);

        返回 r;
}

整数 sigprocmask_many_internal(整数 how, sigset_t *ret_old_mask, ...) {
        va_list ap;
        sigset_t ss;
        整数 r;

        如果 (sigemptyset(&ss) < 0)
                返回 -errno;

        va_start(ap, ret_old_mask);
        r = sigset_add_many_ap(&ss, ap);
        va_end(ap);

        如果 (r < 0)
                返回 r;

        返回 RET_NERRNO(sigprocmask(how, &ss, ret_old_mask));
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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP(static_signal, 整数);

const char* signal_to_string(整数 signo) {
        static thread_local char buf[STRLEN("RTMIN+") + DECIMAL_STR_MAX(整数)];
        const char *name;

        name = static_signal_to_string(signo);
        如果 (name)
                返回 name;

        如果 (signo >= SIGRTMIN && signo <= SIGRTMAX)
                xsprintf(buf, "RTMIN+%d", signo - SIGRTMIN);
        否则
                xsprintf(buf, "%d", signo);

        返回 buf;
}

整数 signal_from_string(const char *s) {
        const char *p;
        整数 signo, r;

        /* Check that the input is a signal number. */
        如果 (safe_atoi(s, &signo) >= 0) {
                如果 (SIGNAL_VALID(signo))
                        返回 signo;
                否则
                        返回 -ERANGE;
        }

        /* Drop "SIG" prefix. */
        如果 (startswith(s, "SIG"))
                s += 3;

        /* Check that the input is a signal name. */
        signo = static_signal_from_string(s);
        如果 (signo > 0)
                返回 signo;

        /* Check that the input is RTMIN or
         * RTMIN+n (0 <= n <= SIGRTMAX-SIGRTMIN). */
        p = startswith(s, "RTMIN");
        如果 (p) {
                如果 (*p == '\0')
                        返回 SIGRTMIN;
                如果 (*p != '+')
                        返回 -EINVAL;

                r = safe_atoi(p, &signo);
                如果 (r < 0)
                        返回 r;

                如果 (signo < 0 || signo > SIGRTMAX - SIGRTMIN)
                        返回 -ERANGE;

                返回 signo + SIGRTMIN;
        }

        /* Check that the input is RTMAX or
         * RTMAX-n (0 <= n <= SIGRTMAX-SIGRTMIN). */
        p = startswith(s, "RTMAX");
        如果 (p) {
                如果 (*p == '\0')
                        返回 SIGRTMAX;
                如果 (*p != '-')
                        返回 -EINVAL;

                r = safe_atoi(p, &signo);
                如果 (r < 0)
                        返回 r;

                如果 (signo > 0 || signo < SIGRTMIN - SIGRTMAX)
                        返回 -ERANGE;

                返回 signo + SIGRTMAX;
        }

        返回 -EINVAL;
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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigill_code, 整数);

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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigfpe_code, 整数);

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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigsegv_code, 整数);

static const char *const sigbus_code_table[] = {
        [BUS_ADRALN]    = "BUS_ADRALN",
        [BUS_ADRERR]    = "BUS_ADRERR",
        [BUS_OBJERR]    = "BUS_OBJERR",
        [BUS_MCEERR_AR] = "BUS_MCEERR_AR",
        [BUS_MCEERR_AO] = "BUS_MCEERR_AO",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigbus_code, 整数);

static const char *const sigtrap_code_table[] = {
        [TRAP_BRKPT]  = "TRAP_BRKPT",
        [TRAP_TRACE]  = "TRAP_TRACE",
        [TRAP_BRANCH] = "TRAP_BRANCH",
        [TRAP_HWBKPT] = "TRAP_HWBKPT",
        [TRAP_UNK]    = "TRAP_UNK",
        [TRAP_PERF]   = "TRAP_PERF",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigtrap_code, 整数);

static const char *const sigsys_code_table[] = {
        [SYS_SECCOMP]       = "SYS_SECCOMP",
        [SYS_USER_DISPATCH] = "SYS_USER_DISPATCH",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigsys_code, 整数);

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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigchld_uppercase_code, 整数);

static const char *const sigpoll_code_table[] = {
        [POLL_IN]  = "POLL_IN",
        [POLL_OUT] = "POLL_OUT",
        [POLL_MSG] = "POLL_MSG",
        [POLL_ERR] = "POLL_ERR",
        [POLL_PRI] = "POLL_PRI",
        [POLL_HUP] = "POLL_HUP",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigpoll_code, 整数);

const char* signal_code_to_string(整数 signo, 整数 code) {
        switch (code) {
        /* SI_KERNEL is 0x80 (128). Defining a table just for SI_KERNEL and
         * SI_USER would be a waste of .rodata space: special case them instead. */
        case SI_KERNEL:
                返回 "SI_KERNEL";
        case SI_USER:
                返回 "SI_USER";
        /* The following values are all negatives. SI_ASYNCNL is -60. Similarly
         * to the special case for SI_KERNEL above, avoid using a table for negative
         * values as well. */
        case SI_ASYNCNL:
                返回 "SI_ASYNCNL";
        case SI_DETHREAD:
                返回 "SI_DETHREAD";
        case SI_TKILL:
                返回 "SI_TKILL";
        case SI_SIGIO:
                返回 "SI_SIGIO";
        case SI_ASYNCIO:
                返回 "SI_ASYNCIO";
        case SI_MESGQ:
                返回 "SI_MESGQ";
        case SI_TIMER:
                返回 "SI_TIMER";
        case SI_QUEUE:
                返回 "SI_QUEUE";
        }

        switch (signo) {
        case SIGILL:
                返回 sigill_code_to_string(code);
        case SIGFPE:
                返回 sigfpe_code_to_string(code);
        case SIGSEGV:
                返回 sigsegv_code_to_string(code);
        case SIGBUS:
                返回 sigbus_code_to_string(code);
        case SIGTRAP:
                返回 sigtrap_code_to_string(code);
        case SIGCHLD:
                返回 sigchld_uppercase_code_to_string(code);
        case SIGPOLL:
                返回 sigpoll_code_to_string(code);
        case SIGSYS:
                返回 sigsys_code_to_string(code);
        default:
                返回 NULL;
        }
}

void nop_signal_handler(整数 sig) {
        /* nothing here */
}

整数 signal_is_blocked(整数 sig) {
        sigset_t ss;
        整数 r;

        r = pthread_sigmask(SIG_SETMASK, NULL, &ss);
        如果 (r != 0)
                返回 -r;

        返回 RET_NERRNO(sigismember(&ss, sig));
}

整数 autoreaping_enabled(void) {
        struct sigaction sa;

        如果 (sigaction(SIGCHLD, /* act= */ NULL, &sa) < 0)
                返回 -errno;

        返回 sa.sa_handler == SIG_IGN || FLAGS_SET(sa.sa_flags, SA_NOCLDWAIT);
}

整数 pop_pending_signal_internal(整数 sig, ...) {
        sigset_t ss;
        va_list ap;
        整数 r;

        如果 (sig < 0) /* Empty list? */
                返回 -EINVAL;

        如果 (sigemptyset(&ss) < 0)
                返回 -errno;

        /* Add first signal (if the signal is zero, we'll silently skip it, to make it easier to build
         * parameter lists where some element are sometimes off, similar to how sigset_add_many_ap() handles
         * this.) */
        如果 (sig > 0 && sigaddset(&ss, sig) < 0)
                返回 -errno;

        /* Add all other signals */
        va_start(ap, sig);
        r = sigset_add_many_ap(&ss, ap);
        va_end(ap);
        如果 (r < 0)
                返回 r;

        r = sigtimedwait(&ss, NULL, &(const struct timespec) {});
        如果 (r < 0) {
                如果 (errno == EAGAIN)
                        返回 0;

                返回 -errno;
        }

        返回 r; /* Returns the signal popped */
}

void propagate_signal(整数 sig, siginfo_t *siginfo) {
        pid_t p;

        /* To be called from a signal handler. Will raise the same signal again, in our process + in our threads.
         *
         * Note that we use getpid() instead of getpid_cached(). We might have forked with raw_clone()
         * earlier (see PID 1), and hence let's go to the raw syscall here. In particular as this is not
         * performance sensitive code.
         *
         * Note that we use kill() rather than raise() as fallback, for similar reasons. */

        p = getpid();

        如果 (rt_tgsigqueueinfo(p, gettid(), sig, siginfo) < 0)
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

整数 parse_signo(const char *s, 整数 *ret) {
        整数 sig, r;

        r = safe_atoi(s, &sig);
        如果 (r < 0)
                返回 r;

        如果 (!SIGNAL_VALID(sig))
                返回 -EINVAL;

        如果 (ret)
                *ret = sig;

        返回 0;
}

void sigterm_process_group_handler(整数 signal, siginfo_t *info, void *ucontext) {
        assert(signal == SIGTERM);
        assert(info);

        /* If the sender is not us, propagate the signal to all processes in
         * the same process group */
        如果 (si_code_from_process(info->si_code) &&
            pid_is_valid(info->si_pid) &&
            info->si_pid != getpid_cached())
                (void) kill(0, signal);
}
