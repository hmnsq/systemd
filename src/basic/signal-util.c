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

số_nguyên reset_all_signal_handlers(void) {
        số_nguyên ret = 0, r;

        cho (số_nguyên sig = 1; sig < _NSIG; sig++) {

                /* These two cannot be caught... */
                nếu (IN_SET(sig, SIGKILL, SIGSTOP))
                        continue;

                /* On Linux the first two RT signals are reserved by glibc, and sigaction() will return
                 * EINVAL for them. */
                r = RET_NERRNO(sigaction(sig, &sigaction_default, NULL));
                nếu (r != -EINVAL)
                        RET_GATHER(ret, r);
        }

        trả_về ret;
}

số_nguyên reset_signal_mask(void) {
        sigset_t ss;

        nếu (sigemptyset(&ss) < 0)
                trả_về -errno;

        trả_về RET_NERRNO(sigprocmask(SIG_SETMASK, &ss, NULL));
}

số_nguyên sigaction_many_internal(const struct sigaction *sa, ...) {
        số_nguyên sig, r = 0;
        va_list ap;

        va_start(ap, sa);

        /* negative signal ends the list. 0 signal is skipped. */
        trong_khi ((sig = va_arg(ap, số_nguyên)) >= 0) {

                nếu (sig == 0)
                        continue;

                RET_GATHER(r, RET_NERRNO(sigaction(sig, sa, NULL)));
        }

        va_end(ap);

        trả_về r;
}

static số_nguyên sigset_add_many_ap(sigset_t *ss, va_list ap) {
        số_nguyên sig, r = 0;

        assert(ss);

        trong_khi ((sig = va_arg(ap, số_nguyên)) >= 0) {

                nếu (sig == 0)
                        continue;

                nếu (sigaddset(ss, sig) < 0) {
                        nếu (r >= 0)
                                r = -errno;
                }
        }

        trả_về r;
}

số_nguyên sigset_add_many_internal(sigset_t *ss, ...) {
        va_list ap;
        số_nguyên r;

        va_start(ap, ss);
        r = sigset_add_many_ap(ss, ap);
        va_end(ap);

        trả_về r;
}

số_nguyên sigprocmask_many_internal(số_nguyên how, sigset_t *ret_old_mask, ...) {
        va_list ap;
        sigset_t ss;
        số_nguyên r;

        nếu (sigemptyset(&ss) < 0)
                trả_về -errno;

        va_start(ap, ret_old_mask);
        r = sigset_add_many_ap(&ss, ap);
        va_end(ap);

        nếu (r < 0)
                trả_về r;

        trả_về RET_NERRNO(sigprocmask(how, &ss, ret_old_mask));
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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP(static_signal, số_nguyên);

const char* signal_to_string(số_nguyên signo) {
        static thread_local char buf[STRLEN("RTMIN+") + DECIMAL_STR_MAX(số_nguyên)];
        const char *name;

        name = static_signal_to_string(signo);
        nếu (name)
                trả_về name;

        nếu (signo >= SIGRTMIN && signo <= SIGRTMAX)
                xsprintf(buf, "RTMIN+%d", signo - SIGRTMIN);
        không_thì
                xsprintf(buf, "%d", signo);

        trả_về buf;
}

số_nguyên signal_from_string(const char *s) {
        const char *p;
        số_nguyên signo, r;

        /* Check that the input is a signal number. */
        nếu (safe_atoi(s, &signo) >= 0) {
                nếu (SIGNAL_VALID(signo))
                        trả_về signo;
                không_thì
                        trả_về -ERANGE;
        }

        /* Drop "SIG" prefix. */
        nếu (startswith(s, "SIG"))
                s += 3;

        /* Check that the input is a signal name. */
        signo = static_signal_from_string(s);
        nếu (signo > 0)
                trả_về signo;

        /* Check that the input is RTMIN or
         * RTMIN+n (0 <= n <= SIGRTMAX-SIGRTMIN). */
        p = startswith(s, "RTMIN");
        nếu (p) {
                nếu (*p == '\0')
                        trả_về SIGRTMIN;
                nếu (*p != '+')
                        trả_về -EINVAL;

                r = safe_atoi(p, &signo);
                nếu (r < 0)
                        trả_về r;

                nếu (signo < 0 || signo > SIGRTMAX - SIGRTMIN)
                        trả_về -ERANGE;

                trả_về signo + SIGRTMIN;
        }

        /* Check that the input is RTMAX or
         * RTMAX-n (0 <= n <= SIGRTMAX-SIGRTMIN). */
        p = startswith(s, "RTMAX");
        nếu (p) {
                nếu (*p == '\0')
                        trả_về SIGRTMAX;
                nếu (*p != '-')
                        trả_về -EINVAL;

                r = safe_atoi(p, &signo);
                nếu (r < 0)
                        trả_về r;

                nếu (signo > 0 || signo < SIGRTMIN - SIGRTMAX)
                        trả_về -ERANGE;

                trả_về signo + SIGRTMAX;
        }

        trả_về -EINVAL;
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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigill_code, số_nguyên);

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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigfpe_code, số_nguyên);

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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigsegv_code, số_nguyên);

static const char *const sigbus_code_table[] = {
        [BUS_ADRALN]    = "BUS_ADRALN",
        [BUS_ADRERR]    = "BUS_ADRERR",
        [BUS_OBJERR]    = "BUS_OBJERR",
        [BUS_MCEERR_AR] = "BUS_MCEERR_AR",
        [BUS_MCEERR_AO] = "BUS_MCEERR_AO",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigbus_code, số_nguyên);

static const char *const sigtrap_code_table[] = {
        [TRAP_BRKPT]  = "TRAP_BRKPT",
        [TRAP_TRACE]  = "TRAP_TRACE",
        [TRAP_BRANCH] = "TRAP_BRANCH",
        [TRAP_HWBKPT] = "TRAP_HWBKPT",
        [TRAP_UNK]    = "TRAP_UNK",
        [TRAP_PERF]   = "TRAP_PERF",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigtrap_code, số_nguyên);

static const char *const sigsys_code_table[] = {
        [SYS_SECCOMP]       = "SYS_SECCOMP",
        [SYS_USER_DISPATCH] = "SYS_USER_DISPATCH",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigsys_code, số_nguyên);

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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigchld_uppercase_code, số_nguyên);

static const char *const sigpoll_code_table[] = {
        [POLL_IN]  = "POLL_IN",
        [POLL_OUT] = "POLL_OUT",
        [POLL_MSG] = "POLL_MSG",
        [POLL_ERR] = "POLL_ERR",
        [POLL_PRI] = "POLL_PRI",
        [POLL_HUP] = "POLL_HUP",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigpoll_code, số_nguyên);

const char* signal_code_to_string(số_nguyên signo, số_nguyên code) {
        switch (code) {
        /* SI_KERNEL is 0x80 (128). Defining a table just for SI_KERNEL and
         * SI_USER would be a waste of .rodata space: special case them instead. */
        case SI_KERNEL:
                trả_về "SI_KERNEL";
        case SI_USER:
                trả_về "SI_USER";
        /* The following values are all negatives. SI_ASYNCNL is -60. Similarly
         * to the special case for SI_KERNEL above, avoid using a table for negative
         * values as well. */
        case SI_ASYNCNL:
                trả_về "SI_ASYNCNL";
        case SI_DETHREAD:
                trả_về "SI_DETHREAD";
        case SI_TKILL:
                trả_về "SI_TKILL";
        case SI_SIGIO:
                trả_về "SI_SIGIO";
        case SI_ASYNCIO:
                trả_về "SI_ASYNCIO";
        case SI_MESGQ:
                trả_về "SI_MESGQ";
        case SI_TIMER:
                trả_về "SI_TIMER";
        case SI_QUEUE:
                trả_về "SI_QUEUE";
        }

        switch (signo) {
        case SIGILL:
                trả_về sigill_code_to_string(code);
        case SIGFPE:
                trả_về sigfpe_code_to_string(code);
        case SIGSEGV:
                trả_về sigsegv_code_to_string(code);
        case SIGBUS:
                trả_về sigbus_code_to_string(code);
        case SIGTRAP:
                trả_về sigtrap_code_to_string(code);
        case SIGCHLD:
                trả_về sigchld_uppercase_code_to_string(code);
        case SIGPOLL:
                trả_về sigpoll_code_to_string(code);
        case SIGSYS:
                trả_về sigsys_code_to_string(code);
        default:
                trả_về NULL;
        }
}

void nop_signal_handler(số_nguyên sig) {
        /* nothing here */
}

số_nguyên signal_is_blocked(số_nguyên sig) {
        sigset_t ss;
        số_nguyên r;

        r = pthread_sigmask(SIG_SETMASK, NULL, &ss);
        nếu (r != 0)
                trả_về -r;

        trả_về RET_NERRNO(sigismember(&ss, sig));
}

số_nguyên autoreaping_enabled(void) {
        struct sigaction sa;

        nếu (sigaction(SIGCHLD, /* act= */ NULL, &sa) < 0)
                trả_về -errno;

        trả_về sa.sa_handler == SIG_IGN || FLAGS_SET(sa.sa_flags, SA_NOCLDWAIT);
}

số_nguyên pop_pending_signal_internal(số_nguyên sig, ...) {
        sigset_t ss;
        va_list ap;
        số_nguyên r;

        nếu (sig < 0) /* Empty list? */
                trả_về -EINVAL;

        nếu (sigemptyset(&ss) < 0)
                trả_về -errno;

        /* Add first signal (if the signal is zero, we'll silently skip it, to make it easier to build
         * parameter lists where some element are sometimes off, similar to how sigset_add_many_ap() handles
         * this.) */
        nếu (sig > 0 && sigaddset(&ss, sig) < 0)
                trả_về -errno;

        /* Add all other signals */
        va_start(ap, sig);
        r = sigset_add_many_ap(&ss, ap);
        va_end(ap);
        nếu (r < 0)
                trả_về r;

        r = sigtimedwait(&ss, NULL, &(const struct timespec) {});
        nếu (r < 0) {
                nếu (errno == EAGAIN)
                        trả_về 0;

                trả_về -errno;
        }

        trả_về r; /* Returns the signal popped */
}

void propagate_signal(số_nguyên sig, siginfo_t *siginfo) {
        pid_t p;

        /* To be called from a signal handler. Will raise the same signal again, in our process + in our threads.
         *
         * Note that we use getpid() instead of getpid_cached(). We might have forked with raw_clone()
         * earlier (see PID 1), and hence let's go to the raw syscall here. In particular as this is not
         * performance sensitive code.
         *
         * Note that we use kill() rather than raise() as fallback, for similar reasons. */

        p = getpid();

        nếu (rt_tgsigqueueinfo(p, gettid(), sig, siginfo) < 0)
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

số_nguyên parse_signo(const char *s, số_nguyên *ret) {
        số_nguyên sig, r;

        r = safe_atoi(s, &sig);
        nếu (r < 0)
                trả_về r;

        nếu (!SIGNAL_VALID(sig))
                trả_về -EINVAL;

        nếu (ret)
                *ret = sig;

        trả_về 0;
}

void sigterm_process_group_handler(số_nguyên signal, siginfo_t *info, void *ucontext) {
        assert(signal == SIGTERM);
        assert(info);

        /* If the sender is not us, propagate the signal to all processes in
         * the same process group */
        nếu (si_code_from_process(info->si_code) &&
            pid_is_valid(info->si_pid) &&
            info->si_pid != getpid_cached())
                (void) kill(0, signal);
}
