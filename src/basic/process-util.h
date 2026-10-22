/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ <signal.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "fileio.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "format-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-util.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ procfs_file_alloca(pid, ਖੇਤਰ)                                  \
        ({                                                              \
                pid_t _pid_ = (pid);                                    \
                ਅਚਲ ਅੱਖਰ *ਖੇਤਰ_2 = (ਖੇਤਰ);                          \
                ਅੱਖਰ *_r_;                                              \
                ਜੇ (_pid_ == 0)                                         \
                        _r_ = strjoina("/proc/self/", ਖੇਤਰ_2);         \
                ਨਹੀਂਤਾਂ {                                                  \
                        ਪ੍ਰਤਿਜ੍ਞਾ(_pid_ > 0);                              \
                        _r_ = newa(ਅੱਖਰ, STRLEN("/proc/") + DECIMAL_STR_MAX(pid_t) + 1 + ਲੜੀ_ਲੰਬਾਈ(ਖੇਤਰ_2) + 1); \
                        sprintf(_r_, "/proc/" PID_FMT "/%s", _pid_, ਖੇਤਰ_2); \
                }                                                       \
                (ਅਚਲ ਅੱਖਰ*) _r_;                                      \
        })

ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ procfs_file_get_ਖੇਤਰ(pid_t pid, ਅਚਲ ਅੱਖਰ *name, ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ, ਅੱਖਰ **ret) {
        ਵਾਪਸ get_proc_ਖੇਤਰ(procfs_file_alloca(pid, name), ਕੁੰਜੀ, ret);
}

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ ProcessCmdlineFlags {
        PROCESS_CMDLINE_COMM_FALLBACK = 1 << 0,
        PROCESS_CMDLINE_USE_LOCALE    = 1 << 1,
        PROCESS_CMDLINE_QUOTE         = 1 << 2,
        PROCESS_CMDLINE_QUOTE_POSIX   = 1 << 3,
} ProcessCmdlineFlags;

ਪੂਰਨ pid_get_comm(pid_t pid, ਅੱਖਰ **ret);
ਪੂਰਨ pidref_get_comm(ਅਚਲ PidRef *pid, ਅੱਖਰ **ret);
ਪੂਰਨ pid_get_cmdline(pid_t pid, size_t max_columns, ProcessCmdlineFlags flags, ਅੱਖਰ **ret);
ਪੂਰਨ pidref_get_cmdline(ਅਚਲ PidRef *pid, size_t max_columns, ProcessCmdlineFlags flags, ਅੱਖਰ **ret);
ਪੂਰਨ pid_get_cmdline_strv(pid_t pid, ProcessCmdlineFlags flags, ਅੱਖਰ ***ret);
ਪੂਰਨ pidref_get_cmdline_strv(ਅਚਲ PidRef *pid, ProcessCmdlineFlags flags, ਅੱਖਰ ***ret);
ਪੂਰਨ get_process_exe(pid_t pid, ਅੱਖਰ **ret);
ਪੂਰਨ pid_get_uid(pid_t pid, uid_t *ret);
ਪੂਰਨ pidref_get_uid(ਅਚਲ PidRef *pid, uid_t *ret);
ਪੂਰਨ get_process_gid(pid_t pid, gid_t *ret);
ਪੂਰਨ get_process_cwd(pid_t pid, ਅੱਖਰ **ret);
ਪੂਰਨ get_process_ਜੜ੍ਹ(pid_t pid, ਅੱਖਰ **ret);
ਪੂਰਨ get_process_environ(pid_t pid, ਅੱਖਰ **ret);
ਪੂਰਨ pid_get_ppid(pid_t pid, pid_t *ret);
ਪੂਰਨ pidref_get_ppid(ਅਚਲ PidRef *pidref, pid_t *ret);
ਪੂਰਨ pidref_get_ppid_as_pidref(ਅਚਲ PidRef *pidref, PidRef *ret);
ਪੂਰਨ pid_get_ਸ਼ੁਰੂ_ਸਮਾਂ(pid_t pid, usec_t *ret);
ਪੂਰਨ pidref_get_ਸ਼ੁਰੂ_ਸਮਾਂ(ਅਚਲ PidRef *pid, usec_t *ret);
ਪੂਰਨ get_process_umask(pid_t pid, mode_t *ret);
ਪੂਰਨ pidref_has_sigcgt(ਅਚਲ PidRef *pidref, ਪੂਰਨ sig);
ਪੂਰਨ pidref_has_sigblk(ਅਚਲ PidRef *pidref, ਪੂਰਨ sig);

ਸਥਿਰ ਸਤਰਅੰਦਰ bool SIGINFO_ਕੋਡ_IS_DEAD(ਪੂਰਨ ਕੋਡ) {
        ਵਾਪਸ ਵਿੱਚ_ਸੈੱਟ(ਕੋਡ, CLD_EXITED, CLD_KILLED, CLD_DUMPED);
}

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ WaitFlags {
        WAIT_LOG_ABNORMAL             = 1 << 0,
        WAIT_LOG_NON_ZERO_EXIT_ਸਥਿਤੀ = 1 << 1,

        /* A shortcut for requesting the most complete logging */
        WAIT_LOG = WAIT_LOG_ABNORMAL|WAIT_LOG_NON_ZERO_EXIT_ਸਥਿਤੀ,
} WaitFlags;

ਪੂਰਨ pidref_wait_for_terminate_and_check(ਅਚਲ ਅੱਖਰ *name, PidRef *pidref, WaitFlags flags);

ਪੂਰਨ kill_and_sigcont(pid_t pid, ਪੂਰਨ sig);

ਪੂਰਨ procfs_get_stat_flags(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਲੰਮਾ *ret);

ਪੂਰਨ pid_is_kernel_thread(pid_t pid);
ਪੂਰਨ pidref_is_kernel_thread(ਅਚਲ PidRef *pid);

ਪੂਰਨ getenv_for_pid(pid_t pid, ਅਚਲ ਅੱਖਰ *ਖੇਤਰ, ਅੱਖਰ **ret);

ਪੂਰਨ pid_is_alive(pid_t pid);
ਪੂਰਨ pidref_is_alive(ਅਚਲ PidRef *pidref);
ਪੂਰਨ pid_is_unwaited(pid_t pid);
ਪੂਰਨ pidref_is_unwaited(PidRef *pidref);
ਪੂਰਨ pid_is_my_ਬੱਚਾ(pid_t pid);
ਪੂਰਨ pidref_is_my_ਬੱਚਾ(PidRef *pidref);
ਪੂਰਨ pidref_from_same_ਜੜ੍ਹ_fs(PidRef *a, PidRef *b);

bool is_main_thread(ਖਾਲੀ);

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ PERSONALITY_INVALID
/* personality(2) documents that 0xFFFFFFFFUL is used for querying the
 * current personality, hence let's use that here as error
 * indicator. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PERSONALITY_INVALID 0xFFFFFFFFUL
#ਜੇ_ਅੰਤ

/* The personality() syscall returns a 32-bit value where the top three bytes are reserved for flags that
 * emulate historical or architectural quirks, and only the least significant byte reflects the actual
 * personality we're interested in. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OPINIONATED_PERSONALITY_ਨਕਾਬ 0xFFUL

ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ personality_from_string(ਅਚਲ ਅੱਖਰ *s);
ਅਚਲ ਅੱਖਰ* personality_ਮੰਜ਼ਿਲ_string(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ p);

ਪੂਰਨ ਸੁਰੱਖਿਅਤ_personality(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ p);
ਪੂਰਨ opinionated_personality(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *ret);

DECLARE_STRING_ਸਾਰਣੀ_LOOKUP(sigchld_ਕੋਡ, ਪੂਰਨ);
DECLARE_STRING_ਸਾਰਣੀ_LOOKUP_WITH_FALLBACK(sched_policy, ਪੂਰਨ);

ਸਥਿਰ ਸਤਰਅੰਦਰ pid_t PTR_ਮੰਜ਼ਿਲ_PID(ਅਚਲ ਖਾਲੀ *p) {
        ਵਾਪਸ (pid_t) ((uintptr_t) p);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ* PID_ਮੰਜ਼ਿਲ_PTR(pid_t pid) {
        ਵਾਪਸ (ਖਾਲੀ*) ((uintptr_t) pid);
}

ਖਾਲੀ valgrind_summary_hack(ਖਾਲੀ);

ਪੂਰਨ pid_compare_func(ਅਚਲ pid_t *a, ਅਚਲ pid_t *b);

bool nice_is_valid(ਪੂਰਨ n) _const_;

bool sched_policy_is_valid(ਪੂਰਨ policy) _const_;
bool sched_policy_supported(ਪੂਰਨ policy);
ਪੂਰਨ sched_get_ਤਰਜੀਹ_min_ਸੁਰੱਖਿਅਤ(ਪੂਰਨ policy);
ਪੂਰਨ sched_get_ਤਰਜੀਹ_max_ਸੁਰੱਖਿਅਤ(ਪੂਰਨ policy);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PID_AUTOMATIC ((pid_t) INT_MIN) /* special value indicating "acquire pid from connection peer" */

ਸਥਿਰ ਸਤਰਅੰਦਰ bool pid_is_valid(pid_t p) {
        ਵਾਪਸ p > 0;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ bool pid_is_automatic(pid_t p) {
        ਵਾਪਸ p == PID_AUTOMATIC;
}

pid_t getpid_cached(ਖਾਲੀ);
ਖਾਲੀ reset_cached_pid(ਖਾਲੀ);

ਪੂਰਨ must_be_ਜੜ੍ਹ(ਖਾਲੀ);

pid_t clone_with_nested_ਢੇਰ(ਪੂਰਨ (*fn)(ਖਾਲੀ *), ਪੂਰਨ flags, ਖਾਲੀ *userdata);

/* 💣 Note that FORK_NEW_USERNS, FORK_NEW_MOUNTNS, FORK_NEW_NETNS or FORK_NEW_PIDNS should not be called in threaded
 * programs, because they cause us to use raw_clone() which does not synchronize the glibc malloc() locks,
 * and thus will cause deadlocks if the parent uses threads and the child does memory allocations. Hence: if
 * the parent is threaded these flags may not be used. These flags cannot be used if the parent uses threads
 * or the child uses malloc(). 💣 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ ForkFlags {
        FORK_RESET_ਸੰਕੇਤ      = 1 <<  0, /* Reset all signal handlers and signal mask */
        FORK_CLOSE_ALL_FDS      = 1 <<  1, /* Close all open file descriptors in the child, except for 0,1,2 */
        FORK_DEATHSIG_SIGTERM   = 1 <<  2, /* Set PR_DEATHSIG in the child to SIGTERM */
        FORK_DEATHSIG_SIGINT    = 1 <<  3, /* Set PR_DEATHSIG in the child to SIGINT */
        FORK_DEATHSIG_SIGKILL   = 1 <<  4, /* Set PR_DEATHSIG in the child to SIGKILL */
        FORK_REARRANGE_STDIO    = 1 <<  5, /* Connect 0,1,2 to specified fds or /dev/null */
        FORK_REOPEN_LOG         = 1 <<  6, /* Reopen log connection */
        FORK_LOG                = 1 <<  7, /* Log above LOG_DEBUG log level about failures */
        FORK_WAIT               = 1 <<  8, /* Wait until child exited */
        FORK_MOUNTNS_SLAVE      = 1 <<  9, /* Make child's mount namespace MS_SLAVE */
        FORK_ਨਿੱਜੀ_TMP        = 1 << 10, /* Mount new /tmp/ in the child (combine with FORK_NEW_MOUNTNS!) */
        FORK_RLIMIT_NOFILE_ਸੁਰੱਖਿਅਤ = 1 << 11, /* Set RLIMIT_NOFILE soft limit to 1K for select() compat */
        FORK_STDOUT_ਮੰਜ਼ਿਲ_STDERR   = 1 << 12, /* Make stdout a copy of stderr */
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

ਪੂਰਨ pidref_ਸੁਰੱਖਿਅਤ_fork_ਭਰਿਆ(
                ਅਚਲ ਅੱਖਰ *name,
                ਅਚਲ ਪੂਰਨ stdio_fds[3],
                ਪੂਰਨ except_fds[],
                size_t n_except_fds,
                ForkFlags flags,
                PidRef *ret);

ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ pidref_ਸੁਰੱਖਿਅਤ_fork(ਅਚਲ ਅੱਖਰ *name, ForkFlags flags, PidRef *ret) {
        ਵਾਪਸ pidref_ਸੁਰੱਖਿਅਤ_fork_ਭਰਿਆ(name, ਨਿਰਾ, ਨਿਰਾ, 0, flags, ret);
}

ਪੂਰਨ namespace_fork_ਭਰਿਆ(
                ਅਚਲ ਅੱਖਰ *outer_name,
                ਅਚਲ ਅੱਖਰ *inner_name,
                ਪੂਰਨ except_fds[],
                size_t n_except_fds,
                ForkFlags flags,
                ਪੂਰਨ pidns_fd,
                ਪੂਰਨ mntns_fd,
                ਪੂਰਨ netns_fd,
                ਪੂਰਨ userns_fd,
                ਪੂਰਨ ਜੜ੍ਹ_fd,
                NamespaceEnterFlags namespace_flags,
                PidRef *ret);

ਪੂਰਨ namespace_fork(
                ਅਚਲ ਅੱਖਰ *outer_name,
                ਅਚਲ ਅੱਖਰ *inner_name,
                ForkFlags flags,
                ਪੂਰਨ pidns_fd,
                ਪੂਰਨ mntns_fd,
                ਪੂਰਨ netns_fd,
                ਪੂਰਨ userns_fd,
                ਪੂਰਨ ਜੜ੍ਹ_fd,
                PidRef *ret);

bool oom_score_adjust_is_valid(ਪੂਰਨ oa);
ਪੂਰਨ ਸੈੱਟ_oom_score_adjust(ਪੂਰਨ ਮੁੱਲ);
ਪੂਰਨ get_oom_score_adjust(ਪੂਰਨ *ret);

/* The highest possibly (theoretic) pid_t value on this architecture. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PID_T_MAX ((pid_t) INT32_MAX)
/* The maximum number of concurrent processes Linux allows on this architecture, as well as the highest valid PID value
 * the kernel will potentially assign. This reflects a value compiled into the kernel (PID_MAX_LIMIT), and sets the
 * upper boundary on what may be written to the /proc/sys/kernel/pid_max sysctl (but do note that the sysctl is off by
 * 1, since PID 0 can never exist and there can hence only be one process less than the limit would suggest). Since
 * these values are documented in proc(5) we feel quite confident that they are stable enough for the near future at
 * least to define them here too. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TASKS_MAX 4194303U

assert_cc(TASKS_MAX <= (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ) PID_T_MAX);

ਪੂਰਨ setpriority_closest(ਪੂਰਨ priority);

_noreturn_ ਖਾਲੀ freeze(ਖਾਲੀ);

ਪੂਰਨ get_process_ਧਾਗੇ(pid_t pid);

ਪੂਰਨ is_reaper_process(ਖਾਲੀ);
ਪੂਰਨ make_reaper_process(bool b);

ਪੂਰਨ posix_spawn_wrapper(
                ਅਚਲ ਅੱਖਰ *ਰਸਤਾ,
                ਅੱਖਰ * ਅਚਲ *argv,
                ਅੱਖਰ * ਅਚਲ *envp,
                ਅਚਲ ਅੱਖਰ *cgroup,
                PidRef *ret_pidref);

ਪੂਰਨ proc_dir_open(DIR **ret);
ਪੂਰਨ proc_dir_read(DIR *d, pid_t *ret);
ਪੂਰਨ proc_dir_read_pidref(DIR *d, PidRef *ret);

ਪੂਰਨ ਸੁਰੱਖਿਅਤ_mlockall(ਪੂਰਨ flags);

_noreturn_ ਖਾਲੀ report_errno_and_exit(ਪੂਰਨ errno_fd, ਪੂਰਨ error);
ਪੂਰਨ read_errno(ਪੂਰਨ errno_fd);

ਪੂਰਨ prctl_ਸੁਰੱਖਿਅਤ(ਪੂਰਨ op, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ arg2, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ arg3, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ arg4, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ arg5);

ਪੂਰਨ proc_ਸੈੱਟ_comm(ਅਚਲ ਅੱਖਰ *comm);
ਪੂਰਨ proc_ਸੈੱਟ_nnp(ਖਾਲੀ);
nsec_t proc_get_timerslack(ਖਾਲੀ);
