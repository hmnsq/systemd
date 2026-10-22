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

ຈຳນວນເຕັມ reset_all_signal_handlers(void) {
        ຈຳນວນເຕັມ ret = 0, r;

        ສຳລັບ (ຈຳນວນເຕັມ sig = 1; sig < _NSIG; sig++) {

                /* These two cannot be caught... */
                ຖ້າ (IN_SET(sig, SIGKILL, SIGSTOP))
                        continue;

                /* On Linux the first two RT signals are reserved by glibc, and sigaction() will return
                 * EINVAL for them. */
                r = RET_NERRNO(sigaction(sig, &sigaction_default, NULL));
                ຖ້າ (r != -EINVAL)
                        RET_GATHER(ret, r);
        }

        ສົ່ງຄືນ ret;
}

ຈຳນວນເຕັມ reset_signal_mask(void) {
        sigset_t ss;

        ຖ້າ (sigemptyset(&ss) < 0)
                ສົ່ງຄືນ -errno;

        ສົ່ງຄືນ RET_NERRNO(sigprocmask(SIG_SETMASK, &ss, NULL));
}

ຈຳນວນເຕັມ sigaction_many_internal(const sa_nracanaa sigaction *sa, ...) {
        ຈຳນວນເຕັມ sig, r = 0;
        va_list ap;

        va_start(ap, sa);

        /* negative signal ends the list. 0 signal is skipped. */
        ໃນຂະນະທີ່ ((sig = va_arg(ap, ຈຳນວນເຕັມ)) >= 0) {

                ຖ້າ (sig == 0)
                        continue;

                RET_GATHER(r, RET_NERRNO(sigaction(sig, sa, NULL)));
        }

        va_end(ap);

        ສົ່ງຄືນ r;
}

static ຈຳນວນເຕັມ sigset_add_many_ap(sigset_t *ss, va_list ap) {
        ຈຳນວນເຕັມ sig, r = 0;

        assert(ss);

        ໃນຂະນະທີ່ ((sig = va_arg(ap, ຈຳນວນເຕັມ)) >= 0) {

                ຖ້າ (sig == 0)
                        continue;

                ຖ້າ (sigaddset(ss, sig) < 0) {
                        ຖ້າ (r >= 0)
                                r = -errno;
                }
        }

        ສົ່ງຄືນ r;
}

ຈຳນວນເຕັມ sigset_add_many_internal(sigset_t *ss, ...) {
        va_list ap;
        ຈຳນວນເຕັມ r;

        va_start(ap, ss);
        r = sigset_add_many_ap(ss, ap);
        va_end(ap);

        ສົ່ງຄືນ r;
}

ຈຳນວນເຕັມ sigprocmask_many_internal(ຈຳນວນເຕັມ how, sigset_t *ret_old_mask, ...) {
        va_list ap;
        sigset_t ss;
        ຈຳນວນເຕັມ r;

        ຖ້າ (sigemptyset(&ss) < 0)
                ສົ່ງຄືນ -errno;

        va_start(ap, ret_old_mask);
        r = sigset_add_many_ap(&ss, ap);
        va_end(ap);

        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;

        ສົ່ງຄືນ RET_NERRNO(sigprocmask(how, &ss, ret_old_mask));
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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP(static_signal, ຈຳນວນເຕັມ);

const var_na* signal_to_string(ຈຳນວນເຕັມ signo) {
        static thread_local var_na buf[STRLEN("RTMIN+") + DECIMAL_STR_MAX(ຈຳນວນເຕັມ)];
        const var_na *name;

        name = static_signal_to_string(signo);
        ຖ້າ (name)
                ສົ່ງຄືນ name;

        ຖ້າ (signo >= SIGRTMIN && signo <= SIGRTMAX)
                xsprintf(buf, "RTMIN+%d", signo - SIGRTMIN);
        ບໍ່ດັ່ງນັ້ນ
                xsprintf(buf, "%d", signo);

        ສົ່ງຄືນ buf;
}

ຈຳນວນເຕັມ signal_from_string(const var_na *s) {
        const var_na *p;
        ຈຳນວນເຕັມ signo, r;

        /* Check that the input is a signal number. */
        ຖ້າ (safe_atoi(s, &signo) >= 0) {
                ຖ້າ (SIGNAL_VALID(signo))
                        ສົ່ງຄືນ signo;
                ບໍ່ດັ່ງນັ້ນ
                        ສົ່ງຄືນ -ERANGE;
        }

        /* Drop "SIG" prefix. */
        ຖ້າ (startswith(s, "SIG"))
                s += 3;

        /* Check that the input is a signal name. */
        signo = static_signal_from_string(s);
        ຖ້າ (signo > 0)
                ສົ່ງຄືນ signo;

        /* Check that the input is RTMIN or
         * RTMIN+n (0 <= n <= SIGRTMAX-SIGRTMIN). */
        p = startswith(s, "RTMIN");
        ຖ້າ (p) {
                ຖ້າ (*p == '\0')
                        ສົ່ງຄືນ SIGRTMIN;
                ຖ້າ (*p != '+')
                        ສົ່ງຄືນ -EINVAL;

                r = safe_atoi(p, &signo);
                ຖ້າ (r < 0)
                        ສົ່ງຄືນ r;

                ຖ້າ (signo < 0 || signo > SIGRTMAX - SIGRTMIN)
                        ສົ່ງຄືນ -ERANGE;

                ສົ່ງຄືນ signo + SIGRTMIN;
        }

        /* Check that the input is RTMAX or
         * RTMAX-n (0 <= n <= SIGRTMAX-SIGRTMIN). */
        p = startswith(s, "RTMAX");
        ຖ້າ (p) {
                ຖ້າ (*p == '\0')
                        ສົ່ງຄືນ SIGRTMAX;
                ຖ້າ (*p != '-')
                        ສົ່ງຄືນ -EINVAL;

                r = safe_atoi(p, &signo);
                ຖ້າ (r < 0)
                        ສົ່ງຄືນ r;

                ຖ້າ (signo > 0 || signo < SIGRTMIN - SIGRTMAX)
                        ສົ່ງຄືນ -ERANGE;

                ສົ່ງຄືນ signo + SIGRTMAX;
        }

        ສົ່ງຄືນ -EINVAL;
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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigill_code, ຈຳນວນເຕັມ);

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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigfpe_code, ຈຳນວນເຕັມ);

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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigsegv_code, ຈຳນວນເຕັມ);

static const var_na *const sigbus_code_table[] = {
        [BUS_ADRALN]    = "BUS_ADRALN",
        [BUS_ADRERR]    = "BUS_ADRERR",
        [BUS_OBJERR]    = "BUS_OBJERR",
        [BUS_MCEERR_AR] = "BUS_MCEERR_AR",
        [BUS_MCEERR_AO] = "BUS_MCEERR_AO",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigbus_code, ຈຳນວນເຕັມ);

static const var_na *const sigtrap_code_table[] = {
        [TRAP_BRKPT]  = "TRAP_BRKPT",
        [TRAP_TRACE]  = "TRAP_TRACE",
        [TRAP_BRANCH] = "TRAP_BRANCH",
        [TRAP_HWBKPT] = "TRAP_HWBKPT",
        [TRAP_UNK]    = "TRAP_UNK",
        [TRAP_PERF]   = "TRAP_PERF",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigtrap_code, ຈຳນວນເຕັມ);

static const var_na *const sigsys_code_table[] = {
        [SYS_SECCOMP]       = "SYS_SECCOMP",
        [SYS_USER_DISPATCH] = "SYS_USER_DISPATCH",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigsys_code, ຈຳນວນເຕັມ);

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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigchld_uppercase_code, ຈຳນວນເຕັມ);

static const var_na *const sigpoll_code_table[] = {
        [POLL_IN]  = "POLL_IN",
        [POLL_OUT] = "POLL_OUT",
        [POLL_MSG] = "POLL_MSG",
        [POLL_ERR] = "POLL_ERR",
        [POLL_PRI] = "POLL_PRI",
        [POLL_HUP] = "POLL_HUP",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigpoll_code, ຈຳນວນເຕັມ);

const var_na* signal_code_to_string(ຈຳນວນເຕັມ signo, ຈຳນວນເຕັມ code) {
        switch (code) {
        /* SI_KERNEL is 0x80 (128). Defining a table just for SI_KERNEL and
         * SI_USER would be a waste of .rodata space: special case them instead. */
        pak_sa SI_KERNEL:
                ສົ່ງຄືນ "SI_KERNEL";
        pak_sa SI_USER:
                ສົ່ງຄືນ "SI_USER";
        /* The following values are all negatives. SI_ASYNCNL is -60. Similarly
         * to the special case for SI_KERNEL above, avoid using a table for negative
         * values as well. */
        pak_sa SI_ASYNCNL:
                ສົ່ງຄືນ "SI_ASYNCNL";
        pak_sa SI_DETHREAD:
                ສົ່ງຄືນ "SI_DETHREAD";
        pak_sa SI_TKILL:
                ສົ່ງຄືນ "SI_TKILL";
        pak_sa SI_SIGIO:
                ສົ່ງຄືນ "SI_SIGIO";
        pak_sa SI_ASYNCIO:
                ສົ່ງຄືນ "SI_ASYNCIO";
        pak_sa SI_MESGQ:
                ສົ່ງຄືນ "SI_MESGQ";
        pak_sa SI_TIMER:
                ສົ່ງຄືນ "SI_TIMER";
        pak_sa SI_QUEUE:
                ສົ່ງຄືນ "SI_QUEUE";
        }

        switch (signo) {
        pak_sa SIGILL:
                ສົ່ງຄືນ sigill_code_to_string(code);
        pak_sa SIGFPE:
                ສົ່ງຄືນ sigfpe_code_to_string(code);
        pak_sa SIGSEGV:
                ສົ່ງຄືນ sigsegv_code_to_string(code);
        pak_sa SIGBUS:
                ສົ່ງຄືນ sigbus_code_to_string(code);
        pak_sa SIGTRAP:
                ສົ່ງຄືນ sigtrap_code_to_string(code);
        pak_sa SIGCHLD:
                ສົ່ງຄືນ sigchld_uppercase_code_to_string(code);
        pak_sa SIGPOLL:
                ສົ່ງຄືນ sigpoll_code_to_string(code);
        pak_sa SIGSYS:
                ສົ່ງຄືນ sigsys_code_to_string(code);
        default:
                ສົ່ງຄືນ NULL;
        }
}

void nop_signal_handler(ຈຳນວນເຕັມ sig) {
        /* nothing here */
}

ຈຳນວນເຕັມ signal_is_blocked(ຈຳນວນເຕັມ sig) {
        sigset_t ss;
        ຈຳນວນເຕັມ r;

        r = pthread_sigmask(SIG_SETMASK, NULL, &ss);
        ຖ້າ (r != 0)
                ສົ່ງຄືນ -r;

        ສົ່ງຄືນ RET_NERRNO(sigismember(&ss, sig));
}

ຈຳນວນເຕັມ autoreaping_enabled(void) {
        sa_nracanaa sigaction sa;

        ຖ້າ (sigaction(SIGCHLD, /* act= */ NULL, &sa) < 0)
                ສົ່ງຄືນ -errno;

        ສົ່ງຄືນ sa.sa_handler == SIG_IGN || FLAGS_SET(sa.sa_flags, SA_NOCLDWAIT);
}

ຈຳນວນເຕັມ pop_pending_signal_internal(ຈຳນວນເຕັມ sig, ...) {
        sigset_t ss;
        va_list ap;
        ຈຳນວນເຕັມ r;

        ຖ້າ (sig < 0) /* Empty list? */
                ສົ່ງຄືນ -EINVAL;

        ຖ້າ (sigemptyset(&ss) < 0)
                ສົ່ງຄືນ -errno;

        /* Add first signal (if the signal is zero, we'll silently skip it, to make it easier to build
         * parameter lists where some element are sometimes off, similar to how sigset_add_many_ap() handles
         * this.) */
        ຖ້າ (sig > 0 && sigaddset(&ss, sig) < 0)
                ສົ່ງຄືນ -errno;

        /* Add all other signals */
        va_start(ap, sig);
        r = sigset_add_many_ap(&ss, ap);
        va_end(ap);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;

        r = sigtimedwait(&ss, NULL, &(const sa_nracanaa timespec) {});
        ຖ້າ (r < 0) {
                ຖ້າ (errno == EAGAIN)
                        ສົ່ງຄືນ 0;

                ສົ່ງຄືນ -errno;
        }

        ສົ່ງຄືນ r; /* Returns the signal popped */
}

void propagate_signal(ຈຳນວນເຕັມ sig, siginfo_t *siginfo) {
        pid_t p;

        /* To be called from a signal handler. Will raise the same signal again, in our process + in our threads.
         *
         * Note that we use getpid() instead of getpid_cached(). We might have forked with raw_clone()
         * earlier (see PID 1), and hence let's go to the raw syscall here. In particular as this is not
         * performance sensitive code.
         *
         * Note that we use kill() rather than raise() as fallback, for similar reasons. */

        p = getpid();

        ຖ້າ (rt_tgsigqueueinfo(p, gettid(), sig, siginfo) < 0)
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

ຈຳນວນເຕັມ parse_signo(const var_na *s, ຈຳນວນເຕັມ *ret) {
        ຈຳນວນເຕັມ sig, r;

        r = safe_atoi(s, &sig);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;

        ຖ້າ (!SIGNAL_VALID(sig))
                ສົ່ງຄືນ -EINVAL;

        ຖ້າ (ret)
                *ret = sig;

        ສົ່ງຄືນ 0;
}

void sigterm_process_group_handler(ຈຳນວນເຕັມ sa_nkeeta, siginfo_t *info, void *ucontext) {
        assert(sa_nkeeta == SIGTERM);
        assert(info);

        /* If the sender is not us, propagate the signal to all processes in
         * the same process group */
        ຖ້າ (si_code_from_process(info->si_code) &&
            pid_is_valid(info->si_pid) &&
            info->si_pid != getpid_cached())
                (void) kill(0, sa_nkeeta);
}
