/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل <signal.h>

#شامل "fileio.h"
#شامل "format-util.h"
#شامل "forward.h"
#شامل "string-util.h"

#تعریف_کرو procfs_file_alloca(pid, میدان)                                  \
        ({                                                              \
                pid_t _pid_ = (pid);                                    \
                ثابت حرف *میدان_2 = (میدان);                          \
                حرف *_r_;                                              \
                اگر (_pid_ == 0)                                         \
                        _r_ = strjoina("/proc/self/", میدان_2);         \
                ورنہ {                                                  \
                        أكد(_pid_ > 0);                              \
                        _r_ = newa(حرف, STRLEN("/proc/") + DECIMAL_STR_MAX(pid_t) + 1 + لڑی_لمبائی(میدان_2) + 1); \
                        sprintf(_r_, "/proc/" PID_FMT "/%s", _pid_, میدان_2); \
                }                                                       \
                (ثابت حرف*) _r_;                                      \
        })

ساکن اندرونی صحیح procfs_file_get_میدان(pid_t pid, ثابت حرف *name, ثابت حرف *کلید, حرف **ret) {
        واپس get_proc_میدان(procfs_file_alloca(pid, name), کلید, ret);
}

قسمتعریف شمار ProcessCmdlineFlags {
        PROCESS_CMDLINE_COMM_FALLBACK = 1 << 0,
        PROCESS_CMDLINE_USE_LOCALE    = 1 << 1,
        PROCESS_CMDLINE_QUOTE         = 1 << 2,
        PROCESS_CMDLINE_QUOTE_POSIX   = 1 << 3,
} ProcessCmdlineFlags;

صحیح pid_get_comm(pid_t pid, حرف **ret);
صحیح pidref_get_comm(ثابت PidRef *pid, حرف **ret);
صحیح pid_get_cmdline(pid_t pid, size_t max_columns, ProcessCmdlineFlags flags, حرف **ret);
صحیح pidref_get_cmdline(ثابت PidRef *pid, size_t max_columns, ProcessCmdlineFlags flags, حرف **ret);
صحیح pid_get_cmdline_strv(pid_t pid, ProcessCmdlineFlags flags, حرف ***ret);
صحیح pidref_get_cmdline_strv(ثابت PidRef *pid, ProcessCmdlineFlags flags, حرف ***ret);
صحیح get_process_exe(pid_t pid, حرف **ret);
صحیح pid_get_uid(pid_t pid, uid_t *ret);
صحیح pidref_get_uid(ثابت PidRef *pid, uid_t *ret);
صحیح get_process_gid(pid_t pid, gid_t *ret);
صحیح get_process_cwd(pid_t pid, حرف **ret);
صحیح get_process_جڑ(pid_t pid, حرف **ret);
صحیح get_process_environ(pid_t pid, حرف **ret);
صحیح pid_get_ppid(pid_t pid, pid_t *ret);
صحیح pidref_get_ppid(ثابت PidRef *pidref, pid_t *ret);
صحیح pidref_get_ppid_as_pidref(ثابت PidRef *pidref, PidRef *ret);
صحیح pid_get_آغاز_وقت(pid_t pid, usec_t *ret);
صحیح pidref_get_آغاز_وقت(ثابت PidRef *pid, usec_t *ret);
صحیح get_process_umask(pid_t pid, mode_t *ret);
صحیح pidref_has_sigcgt(ثابت PidRef *pidref, صحیح sig);
صحیح pidref_has_sigblk(ثابت PidRef *pidref, صحیح sig);

ساکن اندرونی bool SIGINFO_رمز_IS_DEAD(صحیح رمز) {
        واپس اندر_سیٹ(رمز, CLD_EXITED, CLD_KILLED, CLD_DUMPED);
}

قسمتعریف شمار WaitFlags {
        WAIT_LOG_ABNORMAL             = 1 << 0,
        WAIT_LOG_NON_ZERO_EXIT_حالت = 1 << 1,

        /* A shortcut for requesting the most complete logging */
        WAIT_LOG = WAIT_LOG_ABNORMAL|WAIT_LOG_NON_ZERO_EXIT_حالت,
} WaitFlags;

صحیح pidref_wait_for_terminate_and_check(ثابت حرف *name, PidRef *pidref, WaitFlags flags);

صحیح kill_and_sigcont(pid_t pid, صحیح sig);

صحیح procfs_get_stat_flags(ثابت حرف *راستہ, غیرعلامتی طویل طویل *ret);

صحیح pid_is_kernel_thread(pid_t pid);
صحیح pidref_is_kernel_thread(ثابت PidRef *pid);

صحیح getenv_for_pid(pid_t pid, ثابت حرف *میدان, حرف **ret);

صحیح pid_is_alive(pid_t pid);
صحیح pidref_is_alive(ثابت PidRef *pidref);
صحیح pid_is_unwaited(pid_t pid);
صحیح pidref_is_unwaited(PidRef *pidref);
صحیح pid_is_my_بچہ(pid_t pid);
صحیح pidref_is_my_بچہ(PidRef *pidref);
صحیح pidref_from_same_جڑ_fs(PidRef *a, PidRef *b);

bool is_main_thread(خالی);

#اگر_غیر_تعریف_شدہ PERSONALITY_INVALID
/* personality(2) documents that 0xFFFFFFFFUL is used for querying the
 * current personality, hence let's use that here as error
 * indicator. */
#تعریف_کرو PERSONALITY_INVALID 0xFFFFFFFFUL
#اگر_ختم

/* The personality() syscall returns a 32-bit value where the top three bytes are reserved for flags that
 * emulate historical or architectural quirks, and only the least significant byte reflects the actual
 * personality we're interested in. */
#تعریف_کرو OPINIONATED_PERSONALITY_نقاب 0xFFUL

غیرعلامتی طویل personality_from_string(ثابت حرف *s);
ثابت حرف* personality_مقصد_string(غیرعلامتی طویل p);

صحیح محفوظ_personality(غیرعلامتی طویل p);
صحیح opinionated_personality(غیرعلامتی طویل *ret);

DECLARE_STRING_جدول_LOOKUP(sigchld_رمز, صحیح);
DECLARE_STRING_جدول_LOOKUP_WITH_FALLBACK(sched_policy, صحیح);

ساکن اندرونی pid_t PTR_مقصد_PID(ثابت خالی *p) {
        واپس (pid_t) ((uintptr_t) p);
}

ساکن اندرونی خالی* PID_مقصد_PTR(pid_t pid) {
        واپس (خالی*) ((uintptr_t) pid);
}

خالی valgrind_summary_hack(خالی);

صحیح pid_compare_func(ثابت pid_t *a, ثابت pid_t *b);

bool nice_is_valid(صحیح n) _const_;

bool sched_policy_is_valid(صحیح policy) _const_;
bool sched_policy_supported(صحیح policy);
صحیح sched_get_ترجیح_min_محفوظ(صحیح policy);
صحیح sched_get_ترجیح_max_محفوظ(صحیح policy);

#تعریف_کرو PID_AUTOMATIC ((pid_t) INT_MIN) /* special value indicating "acquire pid from connection peer" */

ساکن اندرونی bool pid_is_valid(pid_t p) {
        واپس p > 0;
}

ساکن اندرونی bool pid_is_automatic(pid_t p) {
        واپس p == PID_AUTOMATIC;
}

pid_t getpid_cached(خالی);
خالی reset_cached_pid(خالی);

صحیح must_be_جڑ(خالی);

pid_t clone_with_nested_ڈھیر(صحیح (*fn)(خالی *), صحیح flags, خالی *userdata);

/* 💣 Note that FORK_NEW_USERNS, FORK_NEW_MOUNTNS, FORK_NEW_NETNS or FORK_NEW_PIDNS should not be called in threaded
 * programs, because they cause us to use raw_clone() which does not synchronize the glibc malloc() locks,
 * and thus will cause deadlocks if the parent uses threads and the child does memory allocations. Hence: if
 * the parent is threaded these flags may not be used. These flags cannot be used if the parent uses threads
 * or the child uses malloc(). 💣 */
قسمتعریف شمار ForkFlags {
        FORK_RESET_اشارات      = 1 <<  0, /* Reset all signal handlers and signal mask */
        FORK_CLOSE_ALL_FDS      = 1 <<  1, /* Close all open file descriptors in the child, except for 0,1,2 */
        FORK_DEATHSIG_SIGTERM   = 1 <<  2, /* Set PR_DEATHSIG in the child to SIGTERM */
        FORK_DEATHSIG_SIGINT    = 1 <<  3, /* Set PR_DEATHSIG in the child to SIGINT */
        FORK_DEATHSIG_SIGKILL   = 1 <<  4, /* Set PR_DEATHSIG in the child to SIGKILL */
        FORK_REARRANGE_STDIO    = 1 <<  5, /* Connect 0,1,2 to specified fds or /dev/null */
        FORK_REOPEN_LOG         = 1 <<  6, /* Reopen log connection */
        FORK_LOG                = 1 <<  7, /* Log above LOG_DEBUG log level about failures */
        FORK_WAIT               = 1 <<  8, /* Wait until child exited */
        FORK_MOUNTNS_SLAVE      = 1 <<  9, /* Make child's mount namespace MS_SLAVE */
        FORK_نجی_TMP        = 1 << 10, /* Mount new /tmp/ in the child (combine with FORK_NEW_MOUNTNS!) */
        FORK_RLIMIT_NOFILE_محفوظ = 1 << 11, /* Set RLIMIT_NOFILE soft limit to 1K for select() compat */
        FORK_STDOUT_مقصد_STDERR   = 1 << 12, /* Make stdout a copy of stderr */
        FORK_FLUSH_STDIO        = 1 << 13, /* fflush() stdout (and stderr) before forking */
        FORK_CLOEXEC_OFF        = 1 << 14, /* In the child: turn off O_CLOEXEC on all fds in except_fds[] */
        FORK_KEEP_NOTIFY_SOCKET = 1 << 15, /* Unless this specified, $NOTIFY_SOCKET will be unset. */
        FORK_DETACH             = 1 << 16, /* Double fork if needed to ensure PID1/subreaper is parent */
        FORK_PACK_FDS           = 1 << 17, /* Rearrange the passed FDs to be FD 3,4,5,etc. Updates the array in place (combine with FORK_CLOSE_ALL_FDS!) */
        FORK_NEW_MOUNTNS        = 1 << 18, /* Run child in its own mount namespace                               💣 DO NOT USE IN THREADED PROGRAMS! 💣 */
        FORK_NEW_USERNS         = 1 << 19, /* Run child in its own user namespace                                💣 DO NOT USE IN THREADED PROGRAMS! 💣 */
        FORK_NEW_NETNS          = 1 << 20, /* Run child in its own network namespace                             💣 DO NOT USE IN THREADED PROGRAMS! 💣 */
        FORK_NEW_PIDNS          = 1 << 21, /* Run child in its own PID namespace                                 💣 DO NOT USE IN THREADED PROGRAMS! 💣 */
        FORK_FREEZE             = 1 << 22, /* Don't return in child, just call freeze() instead */
        FORK_ALLOW_DLOPEN       = 1 << 23, /* Do not block dlopen() in child */
} ForkFlags;

صحیح pidref_محفوظ_fork_بھرا(
                ثابت حرف *name,
                ثابت صحیح stdio_fds[3],
                صحیح except_fds[],
                size_t n_except_fds,
                ForkFlags flags,
                PidRef *ret);

ساکن اندرونی صحیح pidref_محفوظ_fork(ثابت حرف *name, ForkFlags flags, PidRef *ret) {
        واپس pidref_محفوظ_fork_بھرا(name, تہی, تہی, 0, flags, ret);
}

صحیح namespace_fork_بھرا(
                ثابت حرف *outer_name,
                ثابت حرف *inner_name,
                صحیح except_fds[],
                size_t n_except_fds,
                ForkFlags flags,
                صحیح pidns_fd,
                صحیح mntns_fd,
                صحیح netns_fd,
                صحیح userns_fd,
                صحیح جڑ_fd,
                NamespaceEnterFlags namespace_flags,
                PidRef *ret);

صحیح namespace_fork(
                ثابت حرف *outer_name,
                ثابت حرف *inner_name,
                ForkFlags flags,
                صحیح pidns_fd,
                صحیح mntns_fd,
                صحیح netns_fd,
                صحیح userns_fd,
                صحیح جڑ_fd,
                PidRef *ret);

bool oom_score_adjust_is_valid(صحیح oa);
صحیح سیٹ_oom_score_adjust(صحیح قدر);
صحیح get_oom_score_adjust(صحیح *ret);

/* The highest possibly (theoretic) pid_t value on this architecture. */
#تعریف_کرو PID_T_MAX ((pid_t) INT32_MAX)
/* The maximum number of concurrent processes Linux allows on this architecture, as well as the highest valid PID value
 * the kernel will potentially assign. This reflects a value compiled into the kernel (PID_MAX_LIMIT), and sets the
 * upper boundary on what may be written to the /proc/sys/kernel/pid_max sysctl (but do note that the sysctl is off by
 * 1, since PID 0 can never exist and there can hence only be one process less than the limit would suggest). Since
 * these values are documented in proc(5) we feel quite confident that they are stable enough for the near future at
 * least to define them here too. */
#تعریف_کرو TASKS_MAX 4194303U

assert_cc(TASKS_MAX <= (غیرعلامتی طویل) PID_T_MAX);

صحیح setpriority_closest(صحیح priority);

_noreturn_ خالی freeze(خالی);

صحیح get_process_دھاگے(pid_t pid);

صحیح is_reaper_process(خالی);
صحیح make_reaper_process(bool b);

صحیح posix_spawn_wrapper(
                ثابت حرف *راستہ,
                حرف * ثابت *argv,
                حرف * ثابت *envp,
                ثابت حرف *cgroup,
                PidRef *ret_pidref);

صحیح proc_dir_open(DIR **ret);
صحیح proc_dir_read(DIR *d, pid_t *ret);
صحیح proc_dir_read_pidref(DIR *d, PidRef *ret);

صحیح محفوظ_mlockall(صحیح flags);

_noreturn_ خالی report_errno_and_exit(صحیح errno_fd, صحیح error);
صحیح read_errno(صحیح errno_fd);

صحیح prctl_محفوظ(صحیح op, غیرعلامتی طویل arg2, غیرعلامتی طویل arg3, غیرعلامتی طویل arg4, غیرعلامتی طویل arg5);

صحیح proc_سیٹ_comm(ثابت حرف *comm);
صحیح proc_سیٹ_nnp(خالی);
nsec_t proc_get_timerslack(خالی);
