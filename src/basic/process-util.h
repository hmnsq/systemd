/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ <signal.h>

#ಅನ್ತರ್ಭಾವ "fileio.h"
#ಅನ್ತರ್ಭಾವ "format-util.h"
#ಅನ್ತರ್ಭಾವ "forward.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"

#ವ್ಯಾಖ್ಯಾನಿಸು procfs_file_alloca(pid, ಕ್ಷೇತ್ರ)                                  \
        ({                                                              \
                pid_t _pid_ = (pid);                                    \
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕ್ಷೇತ್ರ_2 = (ಕ್ಷೇತ್ರ);                          \
                ಅಕ್ಷರ *_r_;                                              \
                ಆದರೆ (_pid_ == 0)                                         \
                        _r_ = strjoina("/proc/self/", ಕ್ಷೇತ್ರ_2);         \
                ಇಲ್ಲದಿದ್ದರೆ {                                                  \
                        ಪ್ರತಿಜ್ಞಾ(_pid_ > 0);                              \
                        _r_ = newa(ಅಕ್ಷರ, STRLEN("/proc/") + DECIMAL_STR_MAX(pid_t) + 1 + ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಕ್ಷೇತ್ರ_2) + 1); \
                        sprintf(_r_, "/proc/" PID_FMT "/%s", _pid_, ಕ್ಷೇತ್ರ_2); \
                }                                                       \
                (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*) _r_;                                      \
        })

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ procfs_file_get_ಕ್ಷೇತ್ರ(pid_t pid, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ, ಅಕ್ಷರ **ret) {
        ಹಿಂತಿರುಗಿ get_proc_ಕ್ಷೇತ್ರ(procfs_file_alloca(pid, name), ಕೀಲಿ, ret);
}

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ ProcessCmdlineFlags {
        PROCESS_CMDLINE_COMM_FALLBACK = 1 << 0,
        PROCESS_CMDLINE_USE_LOCALE    = 1 << 1,
        PROCESS_CMDLINE_QUOTE         = 1 << 2,
        PROCESS_CMDLINE_QUOTE_POSIX   = 1 << 3,
} ProcessCmdlineFlags;

ಪೂರ್ಣಾಂಕ pid_get_comm(pid_t pid, ಅಕ್ಷರ **ret);
ಪೂರ್ಣಾಂಕ pidref_get_comm(ಸ್ಥಿರಾಂಕ PidRef *pid, ಅಕ್ಷರ **ret);
ಪೂರ್ಣಾಂಕ pid_get_cmdline(pid_t pid, size_t max_columns, ProcessCmdlineFlags flags, ಅಕ್ಷರ **ret);
ಪೂರ್ಣಾಂಕ pidref_get_cmdline(ಸ್ಥಿರಾಂಕ PidRef *pid, size_t max_columns, ProcessCmdlineFlags flags, ಅಕ್ಷರ **ret);
ಪೂರ್ಣಾಂಕ pid_get_cmdline_strv(pid_t pid, ProcessCmdlineFlags flags, ಅಕ್ಷರ ***ret);
ಪೂರ್ಣಾಂಕ pidref_get_cmdline_strv(ಸ್ಥಿರಾಂಕ PidRef *pid, ProcessCmdlineFlags flags, ಅಕ್ಷರ ***ret);
ಪೂರ್ಣಾಂಕ get_process_exe(pid_t pid, ಅಕ್ಷರ **ret);
ಪೂರ್ಣಾಂಕ pid_get_uid(pid_t pid, uid_t *ret);
ಪೂರ್ಣಾಂಕ pidref_get_uid(ಸ್ಥಿರಾಂಕ PidRef *pid, uid_t *ret);
ಪೂರ್ಣಾಂಕ get_process_gid(pid_t pid, gid_t *ret);
ಪೂರ್ಣಾಂಕ get_process_cwd(pid_t pid, ಅಕ್ಷರ **ret);
ಪೂರ್ಣಾಂಕ get_process_ಬೇರು(pid_t pid, ಅಕ್ಷರ **ret);
ಪೂರ್ಣಾಂಕ get_process_environ(pid_t pid, ಅಕ್ಷರ **ret);
ಪೂರ್ಣಾಂಕ pid_get_ppid(pid_t pid, pid_t *ret);
ಪೂರ್ಣಾಂಕ pidref_get_ppid(ಸ್ಥಿರಾಂಕ PidRef *pidref, pid_t *ret);
ಪೂರ್ಣಾಂಕ pidref_get_ppid_as_pidref(ಸ್ಥಿರಾಂಕ PidRef *pidref, PidRef *ret);
ಪೂರ್ಣಾಂಕ pid_get_ಆರಂಭ_ಸಮಯ(pid_t pid, usec_t *ret);
ಪೂರ್ಣಾಂಕ pidref_get_ಆರಂಭ_ಸಮಯ(ಸ್ಥಿರಾಂಕ PidRef *pid, usec_t *ret);
ಪೂರ್ಣಾಂಕ get_process_umask(pid_t pid, mode_t *ret);
ಪೂರ್ಣಾಂಕ pidref_has_sigcgt(ಸ್ಥಿರಾಂಕ PidRef *pidref, ಪೂರ್ಣಾಂಕ sig);
ಪೂರ್ಣಾಂಕ pidref_has_sigblk(ಸ್ಥಿರಾಂಕ PidRef *pidref, ಪೂರ್ಣಾಂಕ sig);

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool SIGINFO_ಸಂಕೇತ_IS_DEAD(ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2) {
        ಹಿಂತಿರುಗಿ ಒಳ_ಗಣ(ಸಂಕೇತ_2, CLD_EXITED, CLD_KILLED, CLD_DUMPED);
}

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ WaitFlags {
        WAIT_LOG_ABNORMAL             = 1 << 0,
        WAIT_LOG_NON_ZERO_EXIT_ಸ್ಥಿತಿ = 1 << 1,

        /* A shortcut for requesting the most complete logging */
        WAIT_LOG = WAIT_LOG_ABNORMAL|WAIT_LOG_NON_ZERO_EXIT_ಸ್ಥಿತಿ,
} WaitFlags;

ಪೂರ್ಣಾಂಕ pidref_wait_for_terminate_and_check(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, PidRef *pidref, WaitFlags flags);

ಪೂರ್ಣಾಂಕ kill_and_sigcont(pid_t pid, ಪೂರ್ಣಾಂಕ sig);

ಪೂರ್ಣಾಂಕ procfs_get_stat_flags(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ದೀರ್ಘ *ret);

ಪೂರ್ಣಾಂಕ pid_is_kernel_thread(pid_t pid);
ಪೂರ್ಣಾಂಕ pidref_is_kernel_thread(ಸ್ಥಿರಾಂಕ PidRef *pid);

ಪೂರ್ಣಾಂಕ getenv_for_pid(pid_t pid, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕ್ಷೇತ್ರ, ಅಕ್ಷರ **ret);

ಪೂರ್ಣಾಂಕ pid_is_alive(pid_t pid);
ಪೂರ್ಣಾಂಕ pidref_is_alive(ಸ್ಥಿರಾಂಕ PidRef *pidref);
ಪೂರ್ಣಾಂಕ pid_is_unwaited(pid_t pid);
ಪೂರ್ಣಾಂಕ pidref_is_unwaited(PidRef *pidref);
ಪೂರ್ಣಾಂಕ pid_is_my_ಮಗು(pid_t pid);
ಪೂರ್ಣಾಂಕ pidref_is_my_ಮಗು(PidRef *pidref);
ಪೂರ್ಣಾಂಕ pidref_from_same_ಬೇರು_fs(PidRef *a, PidRef *b);

bool is_main_thread(ಶೂನ್ಯ);

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ PERSONALITY_INVALID
/* personality(2) documents that 0xFFFFFFFFUL is used for querying the
 * current personality, hence let's use that here as error
 * indicator. */
#ವ್ಯಾಖ್ಯಾನಿಸು PERSONALITY_INVALID 0xFFFFFFFFUL
#ಆದರೆ_ಅಂತ್ಯ

/* The personality() syscall returns a 32-bit value where the top three bytes are reserved for flags that
 * emulate historical or architectural quirks, and only the least significant byte reflects the actual
 * personality we're interested in. */
#ವ್ಯಾಖ್ಯಾನಿಸು OPINIONATED_PERSONALITY_ಮುಖವಾಡ 0xFFUL

ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ personality_from_string(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s);
ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* personality_ಗಮ್ಯ_string(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ p);

ಪೂರ್ಣಾಂಕ ಸುರಕ್ಷಿತ_personality(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ p);
ಪೂರ್ಣಾಂಕ opinionated_personality(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *ret);

DECLARE_STRING_ಕೋಷ್ಟಕ_LOOKUP(sigchld_ಸಂಕೇತ, ಪೂರ್ಣಾಂಕ);
DECLARE_STRING_ಕೋಷ್ಟಕ_LOOKUP_WITH_FALLBACK(sched_policy, ಪೂರ್ಣಾಂಕ);

ಸ್ಥಿರ ಸಾಲೊಳಗೆ pid_t PTR_ಗಮ್ಯ_PID(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *p) {
        ಹಿಂತಿರುಗಿ (pid_t) ((uintptr_t) p);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ* PID_ಗಮ್ಯ_PTR(pid_t pid) {
        ಹಿಂತಿರುಗಿ (ಶೂನ್ಯ*) ((uintptr_t) pid);
}

ಶೂನ್ಯ valgrind_summary_hack(ಶೂನ್ಯ);

ಪೂರ್ಣಾಂಕ pid_compare_func(ಸ್ಥಿರಾಂಕ pid_t *a, ಸ್ಥಿರಾಂಕ pid_t *b);

bool nice_is_valid(ಪೂರ್ಣಾಂಕ n) _const_;

bool sched_policy_is_valid(ಪೂರ್ಣಾಂಕ policy) _const_;
bool sched_policy_supported(ಪೂರ್ಣಾಂಕ policy);
ಪೂರ್ಣಾಂಕ sched_get_ಆದ್ಯತೆ_min_ಸುರಕ್ಷಿತ(ಪೂರ್ಣಾಂಕ policy);
ಪೂರ್ಣಾಂಕ sched_get_ಆದ್ಯತೆ_max_ಸುರಕ್ಷಿತ(ಪೂರ್ಣಾಂಕ policy);

#ವ್ಯಾಖ್ಯಾನಿಸು PID_AUTOMATIC ((pid_t) INT_MIN) /* special value indicating "acquire pid from connection peer" */

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool pid_is_valid(pid_t p) {
        ಹಿಂತಿರುಗಿ p > 0;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool pid_is_automatic(pid_t p) {
        ಹಿಂತಿರುಗಿ p == PID_AUTOMATIC;
}

pid_t getpid_cached(ಶೂನ್ಯ);
ಶೂನ್ಯ reset_cached_pid(ಶೂನ್ಯ);

ಪೂರ್ಣಾಂಕ must_be_ಬೇರು(ಶೂನ್ಯ);

pid_t clone_with_nested_ರಾಶಿ(ಪೂರ್ಣಾಂಕ (*fn)(ಶೂನ್ಯ *), ಪೂರ್ಣಾಂಕ flags, ಶೂನ್ಯ *userdata);

/* 💣 Note that FORK_NEW_USERNS, FORK_NEW_MOUNTNS, FORK_NEW_NETNS or FORK_NEW_PIDNS should not be called in threaded
 * programs, because they cause us to use raw_clone() which does not synchronize the glibc malloc() locks,
 * and thus will cause deadlocks if the parent uses threads and the child does memory allocations. Hence: if
 * the parent is threaded these flags may not be used. These flags cannot be used if the parent uses threads
 * or the child uses malloc(). 💣 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ ForkFlags {
        FORK_RESET_ಸಂಕೇತಗಳು      = 1 <<  0, /* Reset all signal handlers and signal mask */
        FORK_CLOSE_ALL_FDS      = 1 <<  1, /* Close all open file descriptors in the child, except for 0,1,2 */
        FORK_DEATHSIG_SIGTERM   = 1 <<  2, /* Set PR_DEATHSIG in the child to SIGTERM */
        FORK_DEATHSIG_SIGINT    = 1 <<  3, /* Set PR_DEATHSIG in the child to SIGINT */
        FORK_DEATHSIG_SIGKILL   = 1 <<  4, /* Set PR_DEATHSIG in the child to SIGKILL */
        FORK_REARRANGE_STDIO    = 1 <<  5, /* Connect 0,1,2 to specified fds or /dev/null */
        FORK_REOPEN_LOG         = 1 <<  6, /* Reopen log connection */
        FORK_LOG                = 1 <<  7, /* Log above LOG_DEBUG log level about failures */
        FORK_WAIT               = 1 <<  8, /* Wait until child exited */
        FORK_MOUNTNS_SLAVE      = 1 <<  9, /* Make child's mount namespace MS_SLAVE */
        FORK_ಖಾಸಗಿ_TMP        = 1 << 10, /* Mount new /tmp/ in the child (combine with FORK_NEW_MOUNTNS!) */
        FORK_RLIMIT_NOFILE_ಸುರಕ್ಷಿತ = 1 << 11, /* Set RLIMIT_NOFILE soft limit to 1K for select() compat */
        FORK_STDOUT_ಗಮ್ಯ_STDERR   = 1 << 12, /* Make stdout a copy of stderr */
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

ಪೂರ್ಣಾಂಕ pidref_ಸುರಕ್ಷಿತ_fork_ತುಂಬಿದ(
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name,
                ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ stdio_fds[3],
                ಪೂರ್ಣಾಂಕ except_fds[],
                size_t n_except_fds,
                ForkFlags flags,
                PidRef *ret);

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ pidref_ಸುರಕ್ಷಿತ_fork(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ForkFlags flags, PidRef *ret) {
        ಹಿಂತಿರುಗಿ pidref_ಸುರಕ್ಷಿತ_fork_ತುಂಬಿದ(name, ಖಾಲಿ, ಖಾಲಿ, 0, flags, ret);
}

ಪೂರ್ಣಾಂಕ namespace_fork_ತುಂಬಿದ(
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *outer_name,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *inner_name,
                ಪೂರ್ಣಾಂಕ except_fds[],
                size_t n_except_fds,
                ForkFlags flags,
                ಪೂರ್ಣಾಂಕ pidns_fd,
                ಪೂರ್ಣಾಂಕ mntns_fd,
                ಪೂರ್ಣಾಂಕ netns_fd,
                ಪೂರ್ಣಾಂಕ userns_fd,
                ಪೂರ್ಣಾಂಕ ಬೇರು_fd,
                NamespaceEnterFlags namespace_flags,
                PidRef *ret);

ಪೂರ್ಣಾಂಕ namespace_fork(
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *outer_name,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *inner_name,
                ForkFlags flags,
                ಪೂರ್ಣಾಂಕ pidns_fd,
                ಪೂರ್ಣಾಂಕ mntns_fd,
                ಪೂರ್ಣಾಂಕ netns_fd,
                ಪೂರ್ಣಾಂಕ userns_fd,
                ಪೂರ್ಣಾಂಕ ಬೇರು_fd,
                PidRef *ret);

bool oom_score_adjust_is_valid(ಪೂರ್ಣಾಂಕ oa);
ಪೂರ್ಣಾಂಕ ಗಣ_oom_score_adjust(ಪೂರ್ಣಾಂಕ ಮೌಲ್ಯ);
ಪೂರ್ಣಾಂಕ get_oom_score_adjust(ಪೂರ್ಣಾಂಕ *ret);

/* The highest possibly (theoretic) pid_t value on this architecture. */
#ವ್ಯಾಖ್ಯಾನಿಸು PID_T_MAX ((pid_t) INT32_MAX)
/* The maximum number of concurrent processes Linux allows on this architecture, as well as the highest valid PID value
 * the kernel will potentially assign. This reflects a value compiled into the kernel (PID_MAX_LIMIT), and sets the
 * upper boundary on what may be written to the /proc/sys/kernel/pid_max sysctl (but do note that the sysctl is off by
 * 1, since PID 0 can never exist and there can hence only be one process less than the limit would suggest). Since
 * these values are documented in proc(5) we feel quite confident that they are stable enough for the near future at
 * least to define them here too. */
#ವ್ಯಾಖ್ಯಾನಿಸು TASKS_MAX 4194303U

assert_cc(TASKS_MAX <= (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ) PID_T_MAX);

ಪೂರ್ಣಾಂಕ setpriority_closest(ಪೂರ್ಣಾಂಕ priority);

_noreturn_ ಶೂನ್ಯ freeze(ಶೂನ್ಯ);

ಪೂರ್ಣಾಂಕ get_process_ಎಳೆಗಳು(pid_t pid);

ಪೂರ್ಣಾಂಕ is_reaper_process(ಶೂನ್ಯ);
ಪೂರ್ಣಾಂಕ make_reaper_process(bool b);

ಪೂರ್ಣಾಂಕ posix_spawn_wrapper(
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ,
                ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *argv,
                ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *envp,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *cgroup,
                PidRef *ret_pidref);

ಪೂರ್ಣಾಂಕ proc_dir_open(DIR **ret);
ಪೂರ್ಣಾಂಕ proc_dir_read(DIR *d, pid_t *ret);
ಪೂರ್ಣಾಂಕ proc_dir_read_pidref(DIR *d, PidRef *ret);

ಪೂರ್ಣಾಂಕ ಸುರಕ್ಷಿತ_mlockall(ಪೂರ್ಣಾಂಕ flags);

_noreturn_ ಶೂನ್ಯ report_errno_and_exit(ಪೂರ್ಣಾಂಕ errno_fd, ಪೂರ್ಣಾಂಕ error);
ಪೂರ್ಣಾಂಕ read_errno(ಪೂರ್ಣಾಂಕ errno_fd);

ಪೂರ್ಣಾಂಕ prctl_ಸುರಕ್ಷಿತ(ಪೂರ್ಣಾಂಕ op, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ arg2, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ arg3, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ arg4, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ arg5);

ಪೂರ್ಣಾಂಕ proc_ಗಣ_comm(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *comm);
ಪೂರ್ಣಾಂಕ proc_ಗಣ_nnp(ಶೂನ್ಯ);
nsec_t proc_get_timerslack(ಶೂನ್ಯ);
