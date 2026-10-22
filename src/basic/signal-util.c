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

ចំនួនគត់ reset_all_signal_handlers(void) {
        ចំនួនគត់ ret = 0, r;

        សម្រាប់ (ចំនួនគត់ sig = 1; sig < _NSIG; sig++) {

                /* These two cannot be caught... */
                ប្រសិនបើ (IN_SET(sig, SIGKILL, SIGSTOP))
                        continue;

                /* On Linux the first two RT signals are reserved by glibc, and sigaction() will return
                 * EINVAL for them. */
                r = RET_NERRNO(sigaction(sig, &sigaction_default, NULL));
                ប្រសិនបើ (r != -EINVAL)
                        RET_GATHER(ret, r);
        }

        ត្រឡប់ ret;
}

ចំនួនគត់ reset_signal_mask(void) {
        sigset_t ss;

        ប្រសិនបើ (sigemptyset(&ss) < 0)
                ត្រឡប់ -errno;

        ត្រឡប់ RET_NERRNO(sigprocmask(SIG_SETMASK, &ss, NULL));
}

ចំនួនគត់ sigaction_many_internal(const sa_nracanaa sigaction *sa, ...) {
        ចំនួនគត់ sig, r = 0;
        va_list ap;

        va_start(ap, sa);

        /* negative signal ends the list. 0 signal is skipped. */
        ខណៈពេល ((sig = va_arg(ap, ចំនួនគត់)) >= 0) {

                ប្រសិនបើ (sig == 0)
                        continue;

                RET_GATHER(r, RET_NERRNO(sigaction(sig, sa, NULL)));
        }

        va_end(ap);

        ត្រឡប់ r;
}

static ចំនួនគត់ sigset_add_many_ap(sigset_t *ss, va_list ap) {
        ចំនួនគត់ sig, r = 0;

        assert(ss);

        ខណៈពេល ((sig = va_arg(ap, ចំនួនគត់)) >= 0) {

                ប្រសិនបើ (sig == 0)
                        continue;

                ប្រសិនបើ (sigaddset(ss, sig) < 0) {
                        ប្រសិនបើ (r >= 0)
                                r = -errno;
                }
        }

        ត្រឡប់ r;
}

ចំនួនគត់ sigset_add_many_internal(sigset_t *ss, ...) {
        va_list ap;
        ចំនួនគត់ r;

        va_start(ap, ss);
        r = sigset_add_many_ap(ss, ap);
        va_end(ap);

        ត្រឡប់ r;
}

ចំនួនគត់ sigprocmask_many_internal(ចំនួនគត់ how, sigset_t *ret_old_mask, ...) {
        va_list ap;
        sigset_t ss;
        ចំនួនគត់ r;

        ប្រសិនបើ (sigemptyset(&ss) < 0)
                ត្រឡប់ -errno;

        va_start(ap, ret_old_mask);
        r = sigset_add_many_ap(&ss, ap);
        va_end(ap);

        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        ត្រឡប់ RET_NERRNO(sigprocmask(how, &ss, ret_old_mask));
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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP(static_signal, ចំនួនគត់);

const var_na* signal_to_string(ចំនួនគត់ signo) {
        static thread_local var_na buf[STRLEN("RTMIN+") + DECIMAL_STR_MAX(ចំនួនគត់)];
        const var_na *name;

        name = static_signal_to_string(signo);
        ប្រសិនបើ (name)
                ត្រឡប់ name;

        ប្រសិនបើ (signo >= SIGRTMIN && signo <= SIGRTMAX)
                xsprintf(buf, "RTMIN+%d", signo - SIGRTMIN);
        បើមិនដូច្នេះ
                xsprintf(buf, "%d", signo);

        ត្រឡប់ buf;
}

ចំនួនគត់ signal_from_string(const var_na *s) {
        const var_na *p;
        ចំនួនគត់ signo, r;

        /* Check that the input is a signal number. */
        ប្រសិនបើ (safe_atoi(s, &signo) >= 0) {
                ប្រសិនបើ (SIGNAL_VALID(signo))
                        ត្រឡប់ signo;
                បើមិនដូច្នេះ
                        ត្រឡប់ -ERANGE;
        }

        /* Drop "SIG" prefix. */
        ប្រសិនបើ (startswith(s, "SIG"))
                s += 3;

        /* Check that the input is a signal name. */
        signo = static_signal_from_string(s);
        ប្រសិនបើ (signo > 0)
                ត្រឡប់ signo;

        /* Check that the input is RTMIN or
         * RTMIN+n (0 <= n <= SIGRTMAX-SIGRTMIN). */
        p = startswith(s, "RTMIN");
        ប្រសិនបើ (p) {
                ប្រសិនបើ (*p == '\0')
                        ត្រឡប់ SIGRTMIN;
                ប្រសិនបើ (*p != '+')
                        ត្រឡប់ -EINVAL;

                r = safe_atoi(p, &signo);
                ប្រសិនបើ (r < 0)
                        ត្រឡប់ r;

                ប្រសិនបើ (signo < 0 || signo > SIGRTMAX - SIGRTMIN)
                        ត្រឡប់ -ERANGE;

                ត្រឡប់ signo + SIGRTMIN;
        }

        /* Check that the input is RTMAX or
         * RTMAX-n (0 <= n <= SIGRTMAX-SIGRTMIN). */
        p = startswith(s, "RTMAX");
        ប្រសិនបើ (p) {
                ប្រសិនបើ (*p == '\0')
                        ត្រឡប់ SIGRTMAX;
                ប្រសិនបើ (*p != '-')
                        ត្រឡប់ -EINVAL;

                r = safe_atoi(p, &signo);
                ប្រសិនបើ (r < 0)
                        ត្រឡប់ r;

                ប្រសិនបើ (signo > 0 || signo < SIGRTMIN - SIGRTMAX)
                        ត្រឡប់ -ERANGE;

                ត្រឡប់ signo + SIGRTMAX;
        }

        ត្រឡប់ -EINVAL;
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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigill_code, ចំនួនគត់);

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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigfpe_code, ចំនួនគត់);

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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigsegv_code, ចំនួនគត់);

static const var_na *const sigbus_code_table[] = {
        [BUS_ADRALN]    = "BUS_ADRALN",
        [BUS_ADRERR]    = "BUS_ADRERR",
        [BUS_OBJERR]    = "BUS_OBJERR",
        [BUS_MCEERR_AR] = "BUS_MCEERR_AR",
        [BUS_MCEERR_AO] = "BUS_MCEERR_AO",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigbus_code, ចំនួនគត់);

static const var_na *const sigtrap_code_table[] = {
        [TRAP_BRKPT]  = "TRAP_BRKPT",
        [TRAP_TRACE]  = "TRAP_TRACE",
        [TRAP_BRANCH] = "TRAP_BRANCH",
        [TRAP_HWBKPT] = "TRAP_HWBKPT",
        [TRAP_UNK]    = "TRAP_UNK",
        [TRAP_PERF]   = "TRAP_PERF",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigtrap_code, ចំនួនគត់);

static const var_na *const sigsys_code_table[] = {
        [SYS_SECCOMP]       = "SYS_SECCOMP",
        [SYS_USER_DISPATCH] = "SYS_USER_DISPATCH",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigsys_code, ចំនួនគត់);

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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigchld_uppercase_code, ចំនួនគត់);

static const var_na *const sigpoll_code_table[] = {
        [POLL_IN]  = "POLL_IN",
        [POLL_OUT] = "POLL_OUT",
        [POLL_MSG] = "POLL_MSG",
        [POLL_ERR] = "POLL_ERR",
        [POLL_PRI] = "POLL_PRI",
        [POLL_HUP] = "POLL_HUP",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigpoll_code, ចំនួនគត់);

const var_na* signal_code_to_string(ចំនួនគត់ signo, ចំនួនគត់ code) {
        switch (code) {
        /* SI_KERNEL is 0x80 (128). Defining a table just for SI_KERNEL and
         * SI_USER would be a waste of .rodata space: special case them instead. */
        pak_sa SI_KERNEL:
                ត្រឡប់ "SI_KERNEL";
        pak_sa SI_USER:
                ត្រឡប់ "SI_USER";
        /* The following values are all negatives. SI_ASYNCNL is -60. Similarly
         * to the special case for SI_KERNEL above, avoid using a table for negative
         * values as well. */
        pak_sa SI_ASYNCNL:
                ត្រឡប់ "SI_ASYNCNL";
        pak_sa SI_DETHREAD:
                ត្រឡប់ "SI_DETHREAD";
        pak_sa SI_TKILL:
                ត្រឡប់ "SI_TKILL";
        pak_sa SI_SIGIO:
                ត្រឡប់ "SI_SIGIO";
        pak_sa SI_ASYNCIO:
                ត្រឡប់ "SI_ASYNCIO";
        pak_sa SI_MESGQ:
                ត្រឡប់ "SI_MESGQ";
        pak_sa SI_TIMER:
                ត្រឡប់ "SI_TIMER";
        pak_sa SI_QUEUE:
                ត្រឡប់ "SI_QUEUE";
        }

        switch (signo) {
        pak_sa SIGILL:
                ត្រឡប់ sigill_code_to_string(code);
        pak_sa SIGFPE:
                ត្រឡប់ sigfpe_code_to_string(code);
        pak_sa SIGSEGV:
                ត្រឡប់ sigsegv_code_to_string(code);
        pak_sa SIGBUS:
                ត្រឡប់ sigbus_code_to_string(code);
        pak_sa SIGTRAP:
                ត្រឡប់ sigtrap_code_to_string(code);
        pak_sa SIGCHLD:
                ត្រឡប់ sigchld_uppercase_code_to_string(code);
        pak_sa SIGPOLL:
                ត្រឡប់ sigpoll_code_to_string(code);
        pak_sa SIGSYS:
                ត្រឡប់ sigsys_code_to_string(code);
        default:
                ត្រឡប់ NULL;
        }
}

void nop_signal_handler(ចំនួនគត់ sig) {
        /* nothing here */
}

ចំនួនគត់ signal_is_blocked(ចំនួនគត់ sig) {
        sigset_t ss;
        ចំនួនគត់ r;

        r = pthread_sigmask(SIG_SETMASK, NULL, &ss);
        ប្រសិនបើ (r != 0)
                ត្រឡប់ -r;

        ត្រឡប់ RET_NERRNO(sigismember(&ss, sig));
}

ចំនួនគត់ autoreaping_enabled(void) {
        sa_nracanaa sigaction sa;

        ប្រសិនបើ (sigaction(SIGCHLD, /* act= */ NULL, &sa) < 0)
                ត្រឡប់ -errno;

        ត្រឡប់ sa.sa_handler == SIG_IGN || FLAGS_SET(sa.sa_flags, SA_NOCLDWAIT);
}

ចំនួនគត់ pop_pending_signal_internal(ចំនួនគត់ sig, ...) {
        sigset_t ss;
        va_list ap;
        ចំនួនគត់ r;

        ប្រសិនបើ (sig < 0) /* Empty list? */
                ត្រឡប់ -EINVAL;

        ប្រសិនបើ (sigemptyset(&ss) < 0)
                ត្រឡប់ -errno;

        /* Add first signal (if the signal is zero, we'll silently skip it, to make it easier to build
         * parameter lists where some element are sometimes off, similar to how sigset_add_many_ap() handles
         * this.) */
        ប្រសិនបើ (sig > 0 && sigaddset(&ss, sig) < 0)
                ត្រឡប់ -errno;

        /* Add all other signals */
        va_start(ap, sig);
        r = sigset_add_many_ap(&ss, ap);
        va_end(ap);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        r = sigtimedwait(&ss, NULL, &(const sa_nracanaa timespec) {});
        ប្រសិនបើ (r < 0) {
                ប្រសិនបើ (errno == EAGAIN)
                        ត្រឡប់ 0;

                ត្រឡប់ -errno;
        }

        ត្រឡប់ r; /* Returns the signal popped */
}

void propagate_signal(ចំនួនគត់ sig, siginfo_t *siginfo) {
        pid_t p;

        /* To be called from a signal handler. Will raise the same signal again, in our process + in our threads.
         *
         * Note that we use getpid() instead of getpid_cached(). We might have forked with raw_clone()
         * earlier (see PID 1), and hence let's go to the raw syscall here. In particular as this is not
         * performance sensitive code.
         *
         * Note that we use kill() rather than raise() as fallback, for similar reasons. */

        p = getpid();

        ប្រសិនបើ (rt_tgsigqueueinfo(p, gettid(), sig, siginfo) < 0)
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

ចំនួនគត់ parse_signo(const var_na *s, ចំនួនគត់ *ret) {
        ចំនួនគត់ sig, r;

        r = safe_atoi(s, &sig);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        ប្រសិនបើ (!SIGNAL_VALID(sig))
                ត្រឡប់ -EINVAL;

        ប្រសិនបើ (ret)
                *ret = sig;

        ត្រឡប់ 0;
}

void sigterm_process_group_handler(ចំនួនគត់ sa_nkeeta, siginfo_t *info, void *ucontext) {
        assert(sa_nkeeta == SIGTERM);
        assert(info);

        /* If the sender is not us, propagate the signal to all processes in
         * the same process group */
        ប្រសិនបើ (si_code_from_process(info->si_code) &&
            pid_is_valid(info->si_pid) &&
            info->si_pid != getpid_cached())
                (void) kill(0, sa_nkeeta);
}
