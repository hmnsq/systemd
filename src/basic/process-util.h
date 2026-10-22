/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ <signal.h>

#ಅನ್ತರ್ಭಾವ "fileio.h"
#ಅನ್ತರ್ಭಾವ "format-util.h"
#ಅನ್ತರ್ಭಾವ "forward.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"

#define procfs_file_alloca(pid, field)                                  \
        ({                                                              \
                pid_t _pid_ = (pid);                                    \
                ಧ್ರುವ ವರ್ಣ *_field_ = (field);                          \
                ವರ್ಣ *_r_;                                              \
                ಯದಿ (_pid_ == 0)                                         \
                        _r_ = strjoina("/proc/self/", _field_);         \
                ಅನ್ಯಥಾ {                                                  \
                        ಪ್ರತಿಜ್ಞಾ(_pid_ > 0);                              \
                        _r_ = newa(ವರ್ಣ, STRLEN("/proc/") + DECIMAL_STR_MAX(pid_t) + 1 + strlen(_field_) + 1); \
                        sprintf(_r_, "/proc/" PID_FMT "/%s", _pid_, _field_); \
                }                                                       \
                (ಧ್ರುವ ವರ್ಣ*) _r_;                                      \
        })

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ procfs_file_get_field(pid_t pid, ಧ್ರುವ ವರ್ಣ *name, ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ, ವರ್ಣ **ret) {
        ಪ್ರತ್ಯಾಗಮ get_proc_field(procfs_file_alloca(pid, name), ಕೀಲಿ, ret);
}

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ ProcessCmdlineFlags {
        PROCESS_CMDLINE_COMM_FALLBACK = 1 << 0,
        PROCESS_CMDLINE_USE_LOCALE    = 1 << 1,
        PROCESS_CMDLINE_QUOTE         = 1 << 2,
        PROCESS_CMDLINE_QUOTE_POSIX   = 1 << 3,
} ProcessCmdlineFlags;

ಪೂರ್ಣಾಙ್ಕ pid_get_comm(pid_t pid, ವರ್ಣ **ret);
ಪೂರ್ಣಾಙ್ಕ pidref_get_comm(ಧ್ರುವ PidRef *pid, ವರ್ಣ **ret);
ಪೂರ್ಣಾಙ್ಕ pid_get_cmdline(pid_t pid, size_t max_columns, ProcessCmdlineFlags flags, ವರ್ಣ **ret);
ಪೂರ್ಣಾಙ್ಕ pidref_get_cmdline(ಧ್ರುವ PidRef *pid, size_t max_columns, ProcessCmdlineFlags flags, ವರ್ಣ **ret);
ಪೂರ್ಣಾಙ್ಕ pid_get_cmdline_strv(pid_t pid, ProcessCmdlineFlags flags, ವರ್ಣ ***ret);
ಪೂರ್ಣಾಙ್ಕ pidref_get_cmdline_strv(ಧ್ರುವ PidRef *pid, ProcessCmdlineFlags flags, ವರ್ಣ ***ret);
ಪೂರ್ಣಾಙ್ಕ get_process_exe(pid_t pid, ವರ್ಣ **ret);
ಪೂರ್ಣಾಙ್ಕ pid_get_uid(pid_t pid, uid_t *ret);
ಪೂರ್ಣಾಙ್ಕ pidref_get_uid(ಧ್ರುವ PidRef *pid, uid_t *ret);
ಪೂರ್ಣಾಙ್ಕ get_process_gid(pid_t pid, gid_t *ret);
ಪೂರ್ಣಾಙ್ಕ get_process_cwd(pid_t pid, ವರ್ಣ **ret);
ಪೂರ್ಣಾಙ್ಕ get_process_ಬೇರು(pid_t pid, ವರ್ಣ **ret);
ಪೂರ್ಣಾಙ್ಕ get_process_environ(pid_t pid, ವರ್ಣ **ret);
ಪೂರ್ಣಾಙ್ಕ pid_get_ppid(pid_t pid, pid_t *ret);
ಪೂರ್ಣಾಙ್ಕ pidref_get_ppid(ಧ್ರುವ PidRef *pidref, pid_t *ret);
ಪೂರ್ಣಾಙ್ಕ pidref_get_ppid_as_pidref(ಧ್ರುವ PidRef *pidref, PidRef *ret);
ಪೂರ್ಣಾಙ್ಕ pid_get_start_time(pid_t pid, usec_t *ret);
ಪೂರ್ಣಾಙ್ಕ pidref_get_start_time(ಧ್ರುವ PidRef *pid, usec_t *ret);
ಪೂರ್ಣಾಙ್ಕ get_process_umask(pid_t pid, mode_t *ret);
ಪೂರ್ಣಾಙ್ಕ pidref_has_sigcgt(ಧ್ರುವ PidRef *pidref, ಪೂರ್ಣಾಙ್ಕ sig);
ಪೂರ್ಣಾಙ್ಕ pidref_has_sigblk(ಧ್ರುವ PidRef *pidref, ಪೂರ್ಣಾಙ್ಕ sig);

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool SIGINFO_CODE_IS_DEAD(ಪೂರ್ಣಾಙ್ಕ code) {
        ಪ್ರತ್ಯಾಗಮ IN_SET(code, CLD_EXITED, CLD_KILLED, CLD_DUMPED);
}

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ WaitFlags {
        WAIT_LOG_ABNORMAL             = 1 << 0,
        WAIT_LOG_NON_ZERO_EXIT_STATUS = 1 << 1,

        /* A shortcut for requesting the most complete logging */
        WAIT_LOG = WAIT_LOG_ABNORMAL|WAIT_LOG_NON_ZERO_EXIT_STATUS,
} WaitFlags;

ಪೂರ್ಣಾಙ್ಕ pidref_wait_for_terminate_and_check(ಧ್ರುವ ವರ್ಣ *name, PidRef *pidref, WaitFlags flags);

ಪೂರ್ಣಾಙ್ಕ kill_and_sigcont(pid_t pid, ಪೂರ್ಣಾಙ್ಕ sig);

ಪೂರ್ಣಾಙ್ಕ procfs_get_stat_flags(ಧ್ರುವ ವರ್ಣ *path, unsigned long long *ret);

ಪೂರ್ಣಾಙ್ಕ pid_is_kernel_thread(pid_t pid);
ಪೂರ್ಣಾಙ್ಕ pidref_is_kernel_thread(ಧ್ರುವ PidRef *pid);

ಪೂರ್ಣಾಙ್ಕ getenv_for_pid(pid_t pid, ಧ್ರುವ ವರ್ಣ *field, ವರ್ಣ **ret);

ಪೂರ್ಣಾಙ್ಕ pid_is_alive(pid_t pid);
ಪೂರ್ಣಾಙ್ಕ pidref_is_alive(ಧ್ರುವ PidRef *pidref);
ಪೂರ್ಣಾಙ್ಕ pid_is_unwaited(pid_t pid);
ಪೂರ್ಣಾಙ್ಕ pidref_is_unwaited(PidRef *pidref);
ಪೂರ್ಣಾಙ್ಕ pid_is_my_child(pid_t pid);
ಪೂರ್ಣಾಙ್ಕ pidref_is_my_child(PidRef *pidref);
ಪೂರ್ಣಾಙ್ಕ pidref_from_same_ಬೇರು_fs(PidRef *a, PidRef *b);

bool is_main_thread(ಶೂನ್ಯ);

#ifndef PERSONALITY_INVALID
/* personality(2) documents that 0xFFFFFFFFUL is used for querying the
 * current personality, hence let's use that here as error
 * indicator. */
#define PERSONALITY_INVALID 0xFFFFFFFFUL
#endif

/* The personality() syscall returns a 32-bit value where the top three bytes are reserved for flags that
 * emulate historical or architectural quirks, and only the least significant byte reflects the actual
 * personality we're interested in. */
#define OPINIONATED_PERSONALITY_MASK 0xFFUL

unsigned long personality_from_string(ಧ್ರುವ ವರ್ಣ *s);
ಧ್ರುವ ವರ್ಣ* personality_to_string(unsigned long p);

ಪೂರ್ಣಾಙ್ಕ safe_personality(unsigned long p);
ಪೂರ್ಣಾಙ್ಕ opinionated_personality(unsigned long *ret);

DECLARE_STRING_TABLE_LOOKUP(sigchld_code, ಪೂರ್ಣಾಙ್ಕ);
DECLARE_STRING_TABLE_LOOKUP_WITH_FALLBACK(sched_policy, ಪೂರ್ಣಾಙ್ಕ);

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ pid_t PTR_TO_PID(ಧ್ರುವ ಶೂನ್ಯ *p) {
        ಪ್ರತ್ಯಾಗಮ (pid_t) ((uintptr_t) p);
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ* PID_TO_PTR(pid_t pid) {
        ಪ್ರತ್ಯಾಗಮ (ಶೂನ್ಯ*) ((uintptr_t) pid);
}

ಶೂನ್ಯ valgrind_summary_hack(ಶೂನ್ಯ);

ಪೂರ್ಣಾಙ್ಕ pid_compare_func(ಧ್ರುವ pid_t *a, ಧ್ರುವ pid_t *b);

bool nice_is_valid(ಪೂರ್ಣಾಙ್ಕ n) _const_;

bool sched_policy_is_valid(ಪೂರ್ಣಾಙ್ಕ policy) _const_;
bool sched_policy_supported(ಪೂರ್ಣಾಙ್ಕ policy);
ಪೂರ್ಣಾಙ್ಕ sched_get_priority_min_safe(ಪೂರ್ಣಾಙ್ಕ policy);
ಪೂರ್ಣಾಙ್ಕ sched_get_priority_max_safe(ಪೂರ್ಣಾಙ್ಕ policy);

#define PID_AUTOMATIC ((pid_t) INT_MIN) /* special value indicating "acquire pid from connection peer" */

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool pid_is_valid(pid_t p) {
        ಪ್ರತ್ಯಾಗಮ p > 0;
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool pid_is_automatic(pid_t p) {
        ಪ್ರತ್ಯಾಗಮ p == PID_AUTOMATIC;
}

pid_t getpid_cached(ಶೂನ್ಯ);
ಶೂನ್ಯ reset_cached_pid(ಶೂನ್ಯ);

ಪೂರ್ಣಾಙ್ಕ must_be_ಬೇರು(ಶೂನ್ಯ);

pid_t clone_with_nested_ರಾಶಿ(ಪೂರ್ಣಾಙ್ಕ (*fn)(ಶೂನ್ಯ *), ಪೂರ್ಣಾಙ್ಕ flags, ಶೂನ್ಯ *userdata);

/* 💣 Note that FORK_NEW_USERNS, FORK_NEW_MOUNTNS, FORK_NEW_NETNS or FORK_NEW_PIDNS should not be called in threaded
 * programs, because they cause us to use raw_clone() which does not synchronize the glibc malloc() locks,
 * and thus will cause deadlocks if the parent uses threads and the child does memory allocations. Hence: if
 * the parent is threaded these flags may not be used. These flags cannot be used if the parent uses threads
 * or the child uses malloc(). 💣 */
ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ ForkFlags {
        FORK_RESET_SIGNALS      = 1 <<  0, /* Reset all signal handlers and signal mask */
        FORK_CLOSE_ALL_FDS      = 1 <<  1, /* Close all open file descriptors in the child, except for 0,1,2 */
        FORK_DEATHSIG_SIGTERM   = 1 <<  2, /* Set PR_DEATHSIG in the child to SIGTERM */
        FORK_DEATHSIG_SIGINT    = 1 <<  3, /* Set PR_DEATHSIG in the child to SIGINT */
        FORK_DEATHSIG_SIGKILL   = 1 <<  4, /* Set PR_DEATHSIG in the child to SIGKILL */
        FORK_REARRANGE_STDIO    = 1 <<  5, /* Connect 0,1,2 to specified fds or /dev/null */
        FORK_REOPEN_LOG         = 1 <<  6, /* Reopen log connection */
        FORK_LOG                = 1 <<  7, /* Log above LOG_DEBUG log level about failures */
        FORK_WAIT               = 1 <<  8, /* Wait until child exited */
        FORK_MOUNTNS_SLAVE      = 1 <<  9, /* Make child's mount namespace MS_SLAVE */
        FORK_PRIVATE_TMP        = 1 << 10, /* Mount new /tmp/ in the child (combine with FORK_NEW_MOUNTNS!) */
        FORK_RLIMIT_NOFILE_SAFE = 1 << 11, /* Set RLIMIT_NOFILE soft limit to 1K for select() compat */
        FORK_STDOUT_TO_STDERR   = 1 << 12, /* Make stdout a copy of stderr */
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

ಪೂರ್ಣಾಙ್ಕ pidref_safe_fork_full(
                ಧ್ರುವ ವರ್ಣ *name,
                ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ stdio_fds[3],
                ಪೂರ್ಣಾಙ್ಕ except_fds[],
                size_t n_except_fds,
                ForkFlags flags,
                PidRef *ret);

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ pidref_safe_fork(ಧ್ರುವ ವರ್ಣ *name, ForkFlags flags, PidRef *ret) {
        ಪ್ರತ್ಯಾಗಮ pidref_safe_fork_full(name, ರಿಕ್ತ, ರಿಕ್ತ, 0, flags, ret);
}

ಪೂರ್ಣಾಙ್ಕ namespace_fork_full(
                ಧ್ರುವ ವರ್ಣ *outer_name,
                ಧ್ರುವ ವರ್ಣ *inner_name,
                ಪೂರ್ಣಾಙ್ಕ except_fds[],
                size_t n_except_fds,
                ForkFlags flags,
                ಪೂರ್ಣಾಙ್ಕ pidns_fd,
                ಪೂರ್ಣಾಙ್ಕ mntns_fd,
                ಪೂರ್ಣಾಙ್ಕ netns_fd,
                ಪೂರ್ಣಾಙ್ಕ userns_fd,
                ಪೂರ್ಣಾಙ್ಕ ಬೇರು_fd,
                NamespaceEnterFlags namespace_flags,
                PidRef *ret);

ಪೂರ್ಣಾಙ್ಕ namespace_fork(
                ಧ್ರುವ ವರ್ಣ *outer_name,
                ಧ್ರುವ ವರ್ಣ *inner_name,
                ForkFlags flags,
                ಪೂರ್ಣಾಙ್ಕ pidns_fd,
                ಪೂರ್ಣಾಙ್ಕ mntns_fd,
                ಪೂರ್ಣಾಙ್ಕ netns_fd,
                ಪೂರ್ಣಾಙ್ಕ userns_fd,
                ಪೂರ್ಣಾಙ್ಕ ಬೇರು_fd,
                PidRef *ret);

bool oom_score_adjust_is_valid(ಪೂರ್ಣಾಙ್ಕ oa);
ಪೂರ್ಣಾಙ್ಕ set_oom_score_adjust(ಪೂರ್ಣಾಙ್ಕ ಮೌಲ್ಯ);
ಪೂರ್ಣಾಙ್ಕ get_oom_score_adjust(ಪೂರ್ಣಾಙ್ಕ *ret);

/* The highest possibly (theoretic) pid_t value on this architecture. */
#define PID_T_MAX ((pid_t) INT32_MAX)
/* The maximum number of concurrent processes Linux allows on this architecture, as well as the highest valid PID value
 * the kernel will potentially assign. This reflects a value compiled into the kernel (PID_MAX_LIMIT), and sets the
 * upper boundary on what may be written to the /proc/sys/kernel/pid_max sysctl (but do note that the sysctl is off by
 * 1, since PID 0 can never exist and there can hence only be one process less than the limit would suggest). Since
 * these values are documented in proc(5) we feel quite confident that they are stable enough for the near future at
 * least to define them here too. */
#define TASKS_MAX 4194303U

assert_cc(TASKS_MAX <= (unsigned long) PID_T_MAX);

ಪೂರ್ಣಾಙ್ಕ setpriority_closest(ಪೂರ್ಣಾಙ್ಕ priority);

_noreturn_ ಶೂನ್ಯ freeze(ಶೂನ್ಯ);

ಪೂರ್ಣಾಙ್ಕ get_process_threads(pid_t pid);

ಪೂರ್ಣಾಙ್ಕ is_reaper_process(ಶೂನ್ಯ);
ಪೂರ್ಣಾಙ್ಕ make_reaper_process(bool b);

ಪೂರ್ಣಾಙ್ಕ posix_spawn_wrapper(
                ಧ್ರುವ ವರ್ಣ *path,
                ವರ್ಣ * ಧ್ರುವ *argv,
                ವರ್ಣ * ಧ್ರುವ *envp,
                ಧ್ರುವ ವರ್ಣ *cgroup,
                PidRef *ret_pidref);

ಪೂರ್ಣಾಙ್ಕ proc_dir_open(DIR **ret);
ಪೂರ್ಣಾಙ್ಕ proc_dir_read(DIR *d, pid_t *ret);
ಪೂರ್ಣಾಙ್ಕ proc_dir_read_pidref(DIR *d, PidRef *ret);

ಪೂರ್ಣಾಙ್ಕ safe_mlockall(ಪೂರ್ಣಾಙ್ಕ flags);

_noreturn_ ಶೂನ್ಯ report_errno_and_exit(ಪೂರ್ಣಾಙ್ಕ errno_fd, ಪೂರ್ಣಾಙ್ಕ error);
ಪೂರ್ಣಾಙ್ಕ read_errno(ಪೂರ್ಣಾಙ್ಕ errno_fd);

ಪೂರ್ಣಾಙ್ಕ prctl_safe(ಪೂರ್ಣಾಙ್ಕ op, unsigned long arg2, unsigned long arg3, unsigned long arg4, unsigned long arg5);

ಪೂರ್ಣಾಙ್ಕ proc_set_comm(ಧ್ರುವ ವರ್ಣ *comm);
ಪೂರ್ಣಾಙ್ಕ proc_set_nnp(ಶೂನ್ಯ);
nsec_t proc_get_timerslack(ಶೂನ್ಯ);
