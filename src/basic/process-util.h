/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव <signal.h>

#अन्तर्भाव "fileio.h"
#अन्तर्भाव "format-util.h"
#अन्तर्भाव "forward.h"
#अन्तर्भाव "string-util.h"

#define procfs_file_alloca(pid, field)                                  \
        ({                                                              \
                pid_t _pid_ = (pid);                                    \
                ध्रुव वर्ण *_field_ = (field);                          \
                वर्ण *_r_;                                              \
                यदि (_pid_ == 0)                                         \
                        _r_ = strjoina("/proc/self/", _field_);         \
                अन्यथा {                                                  \
                        प्रतिज्ञा(_pid_ > 0);                              \
                        _r_ = newa(वर्ण, STRLEN("/proc/") + DECIMAL_STR_MAX(pid_t) + 1 + strlen(_field_) + 1); \
                        sprintf(_r_, "/proc/" PID_FMT "/%s", _pid_, _field_); \
                }                                                       \
                (ध्रुव वर्ण*) _r_;                                      \
        })

स्थिर अन्तःस्थ पूर्णाङ्क procfs_file_get_field(pid_t pid, ध्रुव वर्ण *name, ध्रुव वर्ण *कुंजी, वर्ण **ret) {
        प्रत्यागम get_proc_field(procfs_file_alloca(pid, name), कुंजी, ret);
}

प्रकारनिर्देश गणना ProcessCmdlineFlags {
        PROCESS_CMDLINE_COMM_FALLBACK = 1 << 0,
        PROCESS_CMDLINE_USE_LOCALE    = 1 << 1,
        PROCESS_CMDLINE_QUOTE         = 1 << 2,
        PROCESS_CMDLINE_QUOTE_POSIX   = 1 << 3,
} ProcessCmdlineFlags;

पूर्णाङ्क pid_get_comm(pid_t pid, वर्ण **ret);
पूर्णाङ्क pidref_get_comm(ध्रुव PidRef *pid, वर्ण **ret);
पूर्णाङ्क pid_get_cmdline(pid_t pid, size_t max_columns, ProcessCmdlineFlags flags, वर्ण **ret);
पूर्णाङ्क pidref_get_cmdline(ध्रुव PidRef *pid, size_t max_columns, ProcessCmdlineFlags flags, वर्ण **ret);
पूर्णाङ्क pid_get_cmdline_strv(pid_t pid, ProcessCmdlineFlags flags, वर्ण ***ret);
पूर्णाङ्क pidref_get_cmdline_strv(ध्रुव PidRef *pid, ProcessCmdlineFlags flags, वर्ण ***ret);
पूर्णाङ्क get_process_exe(pid_t pid, वर्ण **ret);
पूर्णाङ्क pid_get_uid(pid_t pid, uid_t *ret);
पूर्णाङ्क pidref_get_uid(ध्रुव PidRef *pid, uid_t *ret);
पूर्णाङ्क get_process_gid(pid_t pid, gid_t *ret);
पूर्णाङ्क get_process_cwd(pid_t pid, वर्ण **ret);
पूर्णाङ्क get_process_मूल(pid_t pid, वर्ण **ret);
पूर्णाङ्क get_process_environ(pid_t pid, वर्ण **ret);
पूर्णाङ्क pid_get_ppid(pid_t pid, pid_t *ret);
पूर्णाङ्क pidref_get_ppid(ध्रुव PidRef *pidref, pid_t *ret);
पूर्णाङ्क pidref_get_ppid_as_pidref(ध्रुव PidRef *pidref, PidRef *ret);
पूर्णाङ्क pid_get_start_time(pid_t pid, usec_t *ret);
पूर्णाङ्क pidref_get_start_time(ध्रुव PidRef *pid, usec_t *ret);
पूर्णाङ्क get_process_umask(pid_t pid, mode_t *ret);
पूर्णाङ्क pidref_has_sigcgt(ध्रुव PidRef *pidref, पूर्णाङ्क sig);
पूर्णाङ्क pidref_has_sigblk(ध्रुव PidRef *pidref, पूर्णाङ्क sig);

स्थिर अन्तःस्थ bool SIGINFO_CODE_IS_DEAD(पूर्णाङ्क code) {
        प्रत्यागम IN_SET(code, CLD_EXITED, CLD_KILLED, CLD_DUMPED);
}

प्रकारनिर्देश गणना WaitFlags {
        WAIT_LOG_ABNORMAL             = 1 << 0,
        WAIT_LOG_NON_ZERO_EXIT_STATUS = 1 << 1,

        /* A shortcut for requesting the most complete logging */
        WAIT_LOG = WAIT_LOG_ABNORMAL|WAIT_LOG_NON_ZERO_EXIT_STATUS,
} WaitFlags;

पूर्णाङ्क pidref_wait_for_terminate_and_check(ध्रुव वर्ण *name, PidRef *pidref, WaitFlags flags);

पूर्णाङ्क kill_and_sigcont(pid_t pid, पूर्णाङ्क sig);

पूर्णाङ्क procfs_get_stat_flags(ध्रुव वर्ण *path, unsigned long long *ret);

पूर्णाङ्क pid_is_kernel_thread(pid_t pid);
पूर्णाङ्क pidref_is_kernel_thread(ध्रुव PidRef *pid);

पूर्णाङ्क getenv_for_pid(pid_t pid, ध्रुव वर्ण *field, वर्ण **ret);

पूर्णाङ्क pid_is_alive(pid_t pid);
पूर्णाङ्क pidref_is_alive(ध्रुव PidRef *pidref);
पूर्णाङ्क pid_is_unwaited(pid_t pid);
पूर्णाङ्क pidref_is_unwaited(PidRef *pidref);
पूर्णाङ्क pid_is_my_child(pid_t pid);
पूर्णाङ्क pidref_is_my_child(PidRef *pidref);
पूर्णाङ्क pidref_from_same_मूल_fs(PidRef *a, PidRef *b);

bool is_main_thread(शून्य);

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

unsigned long personality_from_string(ध्रुव वर्ण *s);
ध्रुव वर्ण* personality_to_string(unsigned long p);

पूर्णाङ्क safe_personality(unsigned long p);
पूर्णाङ्क opinionated_personality(unsigned long *ret);

DECLARE_STRING_TABLE_LOOKUP(sigchld_code, पूर्णाङ्क);
DECLARE_STRING_TABLE_LOOKUP_WITH_FALLBACK(sched_policy, पूर्णाङ्क);

स्थिर अन्तःस्थ pid_t PTR_TO_PID(ध्रुव शून्य *p) {
        प्रत्यागम (pid_t) ((uintptr_t) p);
}

स्थिर अन्तःस्थ शून्य* PID_TO_PTR(pid_t pid) {
        प्रत्यागम (शून्य*) ((uintptr_t) pid);
}

शून्य valgrind_summary_hack(शून्य);

पूर्णाङ्क pid_compare_func(ध्रुव pid_t *a, ध्रुव pid_t *b);

bool nice_is_valid(पूर्णाङ्क n) _const_;

bool sched_policy_is_valid(पूर्णाङ्क policy) _const_;
bool sched_policy_supported(पूर्णाङ्क policy);
पूर्णाङ्क sched_get_priority_min_safe(पूर्णाङ्क policy);
पूर्णाङ्क sched_get_priority_max_safe(पूर्णाङ्क policy);

#define PID_AUTOMATIC ((pid_t) INT_MIN) /* special value indicating "acquire pid from connection peer" */

स्थिर अन्तःस्थ bool pid_is_valid(pid_t p) {
        प्रत्यागम p > 0;
}

स्थिर अन्तःस्थ bool pid_is_automatic(pid_t p) {
        प्रत्यागम p == PID_AUTOMATIC;
}

pid_t getpid_cached(शून्य);
शून्य reset_cached_pid(शून्य);

पूर्णाङ्क must_be_मूल(शून्य);

pid_t clone_with_nested_ढेर(पूर्णाङ्क (*fn)(शून्य *), पूर्णाङ्क flags, शून्य *userdata);

/* 💣 Note that FORK_NEW_USERNS, FORK_NEW_MOUNTNS, FORK_NEW_NETNS or FORK_NEW_PIDNS should not be called in threaded
 * programs, because they cause us to use raw_clone() which does not synchronize the glibc malloc() locks,
 * and thus will cause deadlocks if the parent uses threads and the child does memory allocations. Hence: if
 * the parent is threaded these flags may not be used. These flags cannot be used if the parent uses threads
 * or the child uses malloc(). 💣 */
प्रकारनिर्देश गणना ForkFlags {
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

पूर्णाङ्क pidref_safe_fork_full(
                ध्रुव वर्ण *name,
                ध्रुव पूर्णाङ्क stdio_fds[3],
                पूर्णाङ्क except_fds[],
                size_t n_except_fds,
                ForkFlags flags,
                PidRef *ret);

स्थिर अन्तःस्थ पूर्णाङ्क pidref_safe_fork(ध्रुव वर्ण *name, ForkFlags flags, PidRef *ret) {
        प्रत्यागम pidref_safe_fork_full(name, रिक्त, रिक्त, 0, flags, ret);
}

पूर्णाङ्क namespace_fork_full(
                ध्रुव वर्ण *outer_name,
                ध्रुव वर्ण *inner_name,
                पूर्णाङ्क except_fds[],
                size_t n_except_fds,
                ForkFlags flags,
                पूर्णाङ्क pidns_fd,
                पूर्णाङ्क mntns_fd,
                पूर्णाङ्क netns_fd,
                पूर्णाङ्क userns_fd,
                पूर्णाङ्क मूल_fd,
                NamespaceEnterFlags namespace_flags,
                PidRef *ret);

पूर्णाङ्क namespace_fork(
                ध्रुव वर्ण *outer_name,
                ध्रुव वर्ण *inner_name,
                ForkFlags flags,
                पूर्णाङ्क pidns_fd,
                पूर्णाङ्क mntns_fd,
                पूर्णाङ्क netns_fd,
                पूर्णाङ्क userns_fd,
                पूर्णाङ्क मूल_fd,
                PidRef *ret);

bool oom_score_adjust_is_valid(पूर्णाङ्क oa);
पूर्णाङ्क set_oom_score_adjust(पूर्णाङ्क मूल्य);
पूर्णाङ्क get_oom_score_adjust(पूर्णाङ्क *ret);

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

पूर्णाङ्क setpriority_closest(पूर्णाङ्क priority);

_noreturn_ शून्य freeze(शून्य);

पूर्णाङ्क get_process_threads(pid_t pid);

पूर्णाङ्क is_reaper_process(शून्य);
पूर्णाङ्क make_reaper_process(bool b);

पूर्णाङ्क posix_spawn_wrapper(
                ध्रुव वर्ण *path,
                वर्ण * ध्रुव *argv,
                वर्ण * ध्रुव *envp,
                ध्रुव वर्ण *cgroup,
                PidRef *ret_pidref);

पूर्णाङ्क proc_dir_open(DIR **ret);
पूर्णाङ्क proc_dir_read(DIR *d, pid_t *ret);
पूर्णाङ्क proc_dir_read_pidref(DIR *d, PidRef *ret);

पूर्णाङ्क safe_mlockall(पूर्णाङ्क flags);

_noreturn_ शून्य report_errno_and_exit(पूर्णाङ्क errno_fd, पूर्णाङ्क error);
पूर्णाङ्क read_errno(पूर्णाङ्क errno_fd);

पूर्णाङ्क prctl_safe(पूर्णाङ्क op, unsigned long arg2, unsigned long arg3, unsigned long arg4, unsigned long arg5);

पूर्णाङ्क proc_set_comm(ध्रुव वर्ण *comm);
पूर्णाङ्क proc_set_nnp(शून्य);
nsec_t proc_get_timerslack(शून्य);
