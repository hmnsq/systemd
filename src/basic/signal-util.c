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

ကိန်းပြည့် reset_all_signal_handlers(void) {
        ကိန်းပြည့် ret = 0, r;

        အတွက် (ကိန်းပြည့် sig = 1; sig < _NSIG; sig++) {

                /* These two cannot be caught... */
                အကယ်၍ (IN_SET(sig, SIGKILL, SIGSTOP))
                        continue;

                /* On Linux the first two RT signals are reserved by glibc, and sigaction() will return
                 * EINVAL for them. */
                r = RET_NERRNO(sigaction(sig, &sigaction_default, NULL));
                အကယ်၍ (r != -EINVAL)
                        RET_GATHER(ret, r);
        }

        ပြန်ပေး ret;
}

ကိန်းပြည့် reset_signal_mask(void) {
        sigset_t ss;

        အကယ်၍ (sigemptyset(&ss) < 0)
                ပြန်ပေး -errno;

        ပြန်ပေး RET_NERRNO(sigprocmask(SIG_SETMASK, &ss, NULL));
}

ကိန်းပြည့် sigaction_many_internal(const sa_nracanaa sigaction *sa, ...) {
        ကိန်းပြည့် sig, r = 0;
        va_list ap;

        va_start(ap, sa);

        /* negative signal ends the list. 0 signal is skipped. */
        နေစဉ် ((sig = va_arg(ap, ကိန်းပြည့်)) >= 0) {

                အကယ်၍ (sig == 0)
                        continue;

                RET_GATHER(r, RET_NERRNO(sigaction(sig, sa, NULL)));
        }

        va_end(ap);

        ပြန်ပေး r;
}

static ကိန်းပြည့် sigset_add_many_ap(sigset_t *ss, va_list ap) {
        ကိန်းပြည့် sig, r = 0;

        assert(ss);

        နေစဉ် ((sig = va_arg(ap, ကိန်းပြည့်)) >= 0) {

                အကယ်၍ (sig == 0)
                        continue;

                အကယ်၍ (sigaddset(ss, sig) < 0) {
                        အကယ်၍ (r >= 0)
                                r = -errno;
                }
        }

        ပြန်ပေး r;
}

ကိန်းပြည့် sigset_add_many_internal(sigset_t *ss, ...) {
        va_list ap;
        ကိန်းပြည့် r;

        va_start(ap, ss);
        r = sigset_add_many_ap(ss, ap);
        va_end(ap);

        ပြန်ပေး r;
}

ကိန်းပြည့် sigprocmask_many_internal(ကိန်းပြည့် how, sigset_t *ret_old_mask, ...) {
        va_list ap;
        sigset_t ss;
        ကိန်းပြည့် r;

        အကယ်၍ (sigemptyset(&ss) < 0)
                ပြန်ပေး -errno;

        va_start(ap, ret_old_mask);
        r = sigset_add_many_ap(&ss, ap);
        va_end(ap);

        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        ပြန်ပေး RET_NERRNO(sigprocmask(how, &ss, ret_old_mask));
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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP(static_signal, ကိန်းပြည့်);

const var_na* signal_to_string(ကိန်းပြည့် signo) {
        static thread_local var_na buf[STRLEN("RTMIN+") + DECIMAL_STR_MAX(ကိန်းပြည့်)];
        const var_na *name;

        name = static_signal_to_string(signo);
        အကယ်၍ (name)
                ပြန်ပေး name;

        အကယ်၍ (signo >= SIGRTMIN && signo <= SIGRTMAX)
                xsprintf(buf, "RTMIN+%d", signo - SIGRTMIN);
        မဟုတ်ပါက
                xsprintf(buf, "%d", signo);

        ပြန်ပေး buf;
}

ကိန်းပြည့် signal_from_string(const var_na *s) {
        const var_na *p;
        ကိန်းပြည့် signo, r;

        /* Check that the input is a signal number. */
        အကယ်၍ (safe_atoi(s, &signo) >= 0) {
                အကယ်၍ (SIGNAL_VALID(signo))
                        ပြန်ပေး signo;
                မဟုတ်ပါက
                        ပြန်ပေး -ERANGE;
        }

        /* Drop "SIG" prefix. */
        အကယ်၍ (startswith(s, "SIG"))
                s += 3;

        /* Check that the input is a signal name. */
        signo = static_signal_from_string(s);
        အကယ်၍ (signo > 0)
                ပြန်ပေး signo;

        /* Check that the input is RTMIN or
         * RTMIN+n (0 <= n <= SIGRTMAX-SIGRTMIN). */
        p = startswith(s, "RTMIN");
        အကယ်၍ (p) {
                အကယ်၍ (*p == '\0')
                        ပြန်ပေး SIGRTMIN;
                အကယ်၍ (*p != '+')
                        ပြန်ပေး -EINVAL;

                r = safe_atoi(p, &signo);
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;

                အကယ်၍ (signo < 0 || signo > SIGRTMAX - SIGRTMIN)
                        ပြန်ပေး -ERANGE;

                ပြန်ပေး signo + SIGRTMIN;
        }

        /* Check that the input is RTMAX or
         * RTMAX-n (0 <= n <= SIGRTMAX-SIGRTMIN). */
        p = startswith(s, "RTMAX");
        အကယ်၍ (p) {
                အကယ်၍ (*p == '\0')
                        ပြန်ပေး SIGRTMAX;
                အကယ်၍ (*p != '-')
                        ပြန်ပေး -EINVAL;

                r = safe_atoi(p, &signo);
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;

                အကယ်၍ (signo > 0 || signo < SIGRTMIN - SIGRTMAX)
                        ပြန်ပေး -ERANGE;

                ပြန်ပေး signo + SIGRTMAX;
        }

        ပြန်ပေး -EINVAL;
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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigill_code, ကိန်းပြည့်);

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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigfpe_code, ကိန်းပြည့်);

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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigsegv_code, ကိန်းပြည့်);

static const var_na *const sigbus_code_table[] = {
        [BUS_ADRALN]    = "BUS_ADRALN",
        [BUS_ADRERR]    = "BUS_ADRERR",
        [BUS_OBJERR]    = "BUS_OBJERR",
        [BUS_MCEERR_AR] = "BUS_MCEERR_AR",
        [BUS_MCEERR_AO] = "BUS_MCEERR_AO",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigbus_code, ကိန်းပြည့်);

static const var_na *const sigtrap_code_table[] = {
        [TRAP_BRKPT]  = "TRAP_BRKPT",
        [TRAP_TRACE]  = "TRAP_TRACE",
        [TRAP_BRANCH] = "TRAP_BRANCH",
        [TRAP_HWBKPT] = "TRAP_HWBKPT",
        [TRAP_UNK]    = "TRAP_UNK",
        [TRAP_PERF]   = "TRAP_PERF",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigtrap_code, ကိန်းပြည့်);

static const var_na *const sigsys_code_table[] = {
        [SYS_SECCOMP]       = "SYS_SECCOMP",
        [SYS_USER_DISPATCH] = "SYS_USER_DISPATCH",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigsys_code, ကိန်းပြည့်);

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

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigchld_uppercase_code, ကိန်းပြည့်);

static const var_na *const sigpoll_code_table[] = {
        [POLL_IN]  = "POLL_IN",
        [POLL_OUT] = "POLL_OUT",
        [POLL_MSG] = "POLL_MSG",
        [POLL_ERR] = "POLL_ERR",
        [POLL_PRI] = "POLL_PRI",
        [POLL_HUP] = "POLL_HUP",
};

DEFINE_PRIVATE_STRING_TABLE_LOOKUP_TO_STRING(sigpoll_code, ကိန်းပြည့်);

const var_na* signal_code_to_string(ကိန်းပြည့် signo, ကိန်းပြည့် code) {
        switch (code) {
        /* SI_KERNEL is 0x80 (128). Defining a table just for SI_KERNEL and
         * SI_USER would be a waste of .rodata space: special case them instead. */
        pak_sa SI_KERNEL:
                ပြန်ပေး "SI_KERNEL";
        pak_sa SI_USER:
                ပြန်ပေး "SI_USER";
        /* The following values are all negatives. SI_ASYNCNL is -60. Similarly
         * to the special case for SI_KERNEL above, avoid using a table for negative
         * values as well. */
        pak_sa SI_ASYNCNL:
                ပြန်ပေး "SI_ASYNCNL";
        pak_sa SI_DETHREAD:
                ပြန်ပေး "SI_DETHREAD";
        pak_sa SI_TKILL:
                ပြန်ပေး "SI_TKILL";
        pak_sa SI_SIGIO:
                ပြန်ပေး "SI_SIGIO";
        pak_sa SI_ASYNCIO:
                ပြန်ပေး "SI_ASYNCIO";
        pak_sa SI_MESGQ:
                ပြန်ပေး "SI_MESGQ";
        pak_sa SI_TIMER:
                ပြန်ပေး "SI_TIMER";
        pak_sa SI_QUEUE:
                ပြန်ပေး "SI_QUEUE";
        }

        switch (signo) {
        pak_sa SIGILL:
                ပြန်ပေး sigill_code_to_string(code);
        pak_sa SIGFPE:
                ပြန်ပေး sigfpe_code_to_string(code);
        pak_sa SIGSEGV:
                ပြန်ပေး sigsegv_code_to_string(code);
        pak_sa SIGBUS:
                ပြန်ပေး sigbus_code_to_string(code);
        pak_sa SIGTRAP:
                ပြန်ပေး sigtrap_code_to_string(code);
        pak_sa SIGCHLD:
                ပြန်ပေး sigchld_uppercase_code_to_string(code);
        pak_sa SIGPOLL:
                ပြန်ပေး sigpoll_code_to_string(code);
        pak_sa SIGSYS:
                ပြန်ပေး sigsys_code_to_string(code);
        default:
                ပြန်ပေး NULL;
        }
}

void nop_signal_handler(ကိန်းပြည့် sig) {
        /* nothing here */
}

ကိန်းပြည့် signal_is_blocked(ကိန်းပြည့် sig) {
        sigset_t ss;
        ကိန်းပြည့် r;

        r = pthread_sigmask(SIG_SETMASK, NULL, &ss);
        အကယ်၍ (r != 0)
                ပြန်ပေး -r;

        ပြန်ပေး RET_NERRNO(sigismember(&ss, sig));
}

ကိန်းပြည့် autoreaping_enabled(void) {
        sa_nracanaa sigaction sa;

        အကယ်၍ (sigaction(SIGCHLD, /* act= */ NULL, &sa) < 0)
                ပြန်ပေး -errno;

        ပြန်ပေး sa.sa_handler == SIG_IGN || FLAGS_SET(sa.sa_flags, SA_NOCLDWAIT);
}

ကိန်းပြည့် pop_pending_signal_internal(ကိန်းပြည့် sig, ...) {
        sigset_t ss;
        va_list ap;
        ကိန်းပြည့် r;

        အကယ်၍ (sig < 0) /* Empty list? */
                ပြန်ပေး -EINVAL;

        အကယ်၍ (sigemptyset(&ss) < 0)
                ပြန်ပေး -errno;

        /* Add first signal (if the signal is zero, we'll silently skip it, to make it easier to build
         * parameter lists where some element are sometimes off, similar to how sigset_add_many_ap() handles
         * this.) */
        အကယ်၍ (sig > 0 && sigaddset(&ss, sig) < 0)
                ပြန်ပေး -errno;

        /* Add all other signals */
        va_start(ap, sig);
        r = sigset_add_many_ap(&ss, ap);
        va_end(ap);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        r = sigtimedwait(&ss, NULL, &(const sa_nracanaa timespec) {});
        အကယ်၍ (r < 0) {
                အကယ်၍ (errno == EAGAIN)
                        ပြန်ပေး 0;

                ပြန်ပေး -errno;
        }

        ပြန်ပေး r; /* Returns the signal popped */
}

void propagate_signal(ကိန်းပြည့် sig, siginfo_t *siginfo) {
        pid_t p;

        /* To be called from a signal handler. Will raise the same signal again, in our process + in our threads.
         *
         * Note that we use getpid() instead of getpid_cached(). We might have forked with raw_clone()
         * earlier (see PID 1), and hence let's go to the raw syscall here. In particular as this is not
         * performance sensitive code.
         *
         * Note that we use kill() rather than raise() as fallback, for similar reasons. */

        p = getpid();

        အကယ်၍ (rt_tgsigqueueinfo(p, gettid(), sig, siginfo) < 0)
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

ကိန်းပြည့် parse_signo(const var_na *s, ကိန်းပြည့် *ret) {
        ကိန်းပြည့် sig, r;

        r = safe_atoi(s, &sig);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        အကယ်၍ (!SIGNAL_VALID(sig))
                ပြန်ပေး -EINVAL;

        အကယ်၍ (ret)
                *ret = sig;

        ပြန်ပေး 0;
}

void sigterm_process_group_handler(ကိန်းပြည့် sa_nkeeta, siginfo_t *info, void *ucontext) {
        assert(sa_nkeeta == SIGTERM);
        assert(info);

        /* If the sender is not us, propagate the signal to all processes in
         * the same process group */
        အကယ်၍ (si_code_from_process(info->si_code) &&
            pid_is_valid(info->si_pid) &&
            info->si_pid != getpid_cached())
                (void) kill(0, sa_nkeeta);
}
