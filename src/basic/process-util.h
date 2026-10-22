/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাৱ <signal.h>

#অন্তর্ভাৱ "fileio.h"
#অন্তর্ভাৱ "format-util.h"
#অন্তর্ভাৱ "forward.h"
#অন্তর্ভাৱ "string-util.h"

#define procfs_file_alloca(pid, field)                                  \
        ({                                                              \
                pid_t _pid_ = (pid);                                    \
                ধ্রুৱ ৱর্ণ *_field_ = (field);                          \
                ৱর্ণ *_r_;                                              \
                যদি (_pid_ == 0)                                         \
                        _r_ = strjoina("/proc/self/", _field_);         \
                অন্যথা {                                                  \
                        প্রতিজ্ঞা(_pid_ > 0);                              \
                        _r_ = newa(ৱর্ণ, STRLEN("/proc/") + DECIMAL_STR_MAX(pid_t) + 1 + strlen(_field_) + 1); \
                        sprintf(_r_, "/proc/" PID_FMT "/%s", _pid_, _field_); \
                }                                                       \
                (ধ্রুৱ ৱর্ণ*) _r_;                                      \
        })

স্থির অন্তঃস্থ পূর্ণাঙ্ক procfs_file_get_field(pid_t pid, ধ্রুৱ ৱর্ণ *name, ধ্রুৱ ৱর্ণ *চাবি, ৱর্ণ **ret) {
        ঘূৰাই get_proc_field(procfs_file_alloca(pid, name), চাবি, ret);
}

প্রকারনির্দেশ গণনা ProcessCmdlineFlags {
        PROCESS_CMDLINE_COMM_FALLBACK = 1 << 0,
        PROCESS_CMDLINE_USE_LOCALE    = 1 << 1,
        PROCESS_CMDLINE_QUOTE         = 1 << 2,
        PROCESS_CMDLINE_QUOTE_POSIX   = 1 << 3,
} ProcessCmdlineFlags;

পূর্ণাঙ্ক pid_get_comm(pid_t pid, ৱর্ণ **ret);
পূর্ণাঙ্ক pidref_get_comm(ধ্রুৱ PidRef *pid, ৱর্ণ **ret);
পূর্ণাঙ্ক pid_get_cmdline(pid_t pid, size_t max_columns, ProcessCmdlineFlags flags, ৱর্ণ **ret);
পূর্ণাঙ্ক pidref_get_cmdline(ধ্রুৱ PidRef *pid, size_t max_columns, ProcessCmdlineFlags flags, ৱর্ণ **ret);
পূর্ণাঙ্ক pid_get_cmdline_strv(pid_t pid, ProcessCmdlineFlags flags, ৱর্ণ ***ret);
পূর্ণাঙ্ক pidref_get_cmdline_strv(ধ্রুৱ PidRef *pid, ProcessCmdlineFlags flags, ৱর্ণ ***ret);
পূর্ণাঙ্ক get_process_exe(pid_t pid, ৱর্ণ **ret);
পূর্ণাঙ্ক pid_get_uid(pid_t pid, uid_t *ret);
পূর্ণাঙ্ক pidref_get_uid(ধ্রুৱ PidRef *pid, uid_t *ret);
পূর্ণাঙ্ক get_process_gid(pid_t pid, gid_t *ret);
পূর্ণাঙ্ক get_process_cwd(pid_t pid, ৱর্ণ **ret);
পূর্ণাঙ্ক get_process_মূল(pid_t pid, ৱর্ণ **ret);
পূর্ণাঙ্ক get_process_environ(pid_t pid, ৱর্ণ **ret);
পূর্ণাঙ্ক pid_get_ppid(pid_t pid, pid_t *ret);
পূর্ণাঙ্ক pidref_get_ppid(ধ্রুৱ PidRef *pidref, pid_t *ret);
পূর্ণাঙ্ক pidref_get_ppid_as_pidref(ধ্রুৱ PidRef *pidref, PidRef *ret);
পূর্ণাঙ্ক pid_get_start_time(pid_t pid, usec_t *ret);
পূর্ণাঙ্ক pidref_get_start_time(ধ্রুৱ PidRef *pid, usec_t *ret);
পূর্ণাঙ্ক get_process_umask(pid_t pid, mode_t *ret);
পূর্ণাঙ্ক pidref_has_sigcgt(ধ্রুৱ PidRef *pidref, পূর্ণাঙ্ক sig);
পূর্ণাঙ্ক pidref_has_sigblk(ধ্রুৱ PidRef *pidref, পূর্ণাঙ্ক sig);

স্থির অন্তঃস্থ bool SIGINFO_CODE_IS_DEAD(পূর্ণাঙ্ক code) {
        ঘূৰাই IN_SET(code, CLD_EXITED, CLD_KILLED, CLD_DUMPED);
}

প্রকারনির্দেশ গণনা WaitFlags {
        WAIT_LOG_ABNORMAL             = 1 << 0,
        WAIT_LOG_NON_ZERO_EXIT_STATUS = 1 << 1,

        /* A shortcut for requesting the most complete logging */
        WAIT_LOG = WAIT_LOG_ABNORMAL|WAIT_LOG_NON_ZERO_EXIT_STATUS,
} WaitFlags;

পূর্ণাঙ্ক pidref_wait_for_terminate_and_check(ধ্রুৱ ৱর্ণ *name, PidRef *pidref, WaitFlags flags);

পূর্ণাঙ্ক kill_and_sigcont(pid_t pid, পূর্ণাঙ্ক sig);

পূর্ণাঙ্ক procfs_get_stat_flags(ধ্রুৱ ৱর্ণ *path, unsigned long long *ret);

পূর্ণাঙ্ক pid_is_kernel_thread(pid_t pid);
পূর্ণাঙ্ক pidref_is_kernel_thread(ধ্রুৱ PidRef *pid);

পূর্ণাঙ্ক getenv_for_pid(pid_t pid, ধ্রুৱ ৱর্ণ *field, ৱর্ণ **ret);

পূর্ণাঙ্ক pid_is_alive(pid_t pid);
পূর্ণাঙ্ক pidref_is_alive(ধ্রুৱ PidRef *pidref);
পূর্ণাঙ্ক pid_is_unwaited(pid_t pid);
পূর্ণাঙ্ক pidref_is_unwaited(PidRef *pidref);
পূর্ণাঙ্ক pid_is_my_child(pid_t pid);
পূর্ণাঙ্ক pidref_is_my_child(PidRef *pidref);
পূর্ণাঙ্ক pidref_from_same_মূল_fs(PidRef *a, PidRef *b);

bool is_main_thread(শূন্য);

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

unsigned long personality_from_string(ধ্রুৱ ৱর্ণ *s);
ধ্রুৱ ৱর্ণ* personality_to_string(unsigned long p);

পূর্ণাঙ্ক safe_personality(unsigned long p);
পূর্ণাঙ্ক opinionated_personality(unsigned long *ret);

DECLARE_STRING_TABLE_LOOKUP(sigchld_code, পূর্ণাঙ্ক);
DECLARE_STRING_TABLE_LOOKUP_WITH_FALLBACK(sched_policy, পূর্ণাঙ্ক);

স্থির অন্তঃস্থ pid_t PTR_TO_PID(ধ্রুৱ শূন্য *p) {
        ঘূৰাই (pid_t) ((uintptr_t) p);
}

স্থির অন্তঃস্থ শূন্য* PID_TO_PTR(pid_t pid) {
        ঘূৰাই (শূন্য*) ((uintptr_t) pid);
}

শূন্য valgrind_summary_hack(শূন্য);

পূর্ণাঙ্ক pid_compare_func(ধ্রুৱ pid_t *a, ধ্রুৱ pid_t *b);

bool nice_is_valid(পূর্ণাঙ্ক n) _const_;

bool sched_policy_is_valid(পূর্ণাঙ্ক policy) _const_;
bool sched_policy_supported(পূর্ণাঙ্ক policy);
পূর্ণাঙ্ক sched_get_priority_min_safe(পূর্ণাঙ্ক policy);
পূর্ণাঙ্ক sched_get_priority_max_safe(পূর্ণাঙ্ক policy);

#define PID_AUTOMATIC ((pid_t) INT_MIN) /* special value indicating "acquire pid from connection peer" */

স্থির অন্তঃস্থ bool pid_is_valid(pid_t p) {
        ঘূৰাই p > 0;
}

স্থির অন্তঃস্থ bool pid_is_automatic(pid_t p) {
        ঘূৰাই p == PID_AUTOMATIC;
}

pid_t getpid_cached(শূন্য);
শূন্য reset_cached_pid(শূন্য);

পূর্ণাঙ্ক must_be_মূল(শূন্য);

pid_t clone_with_nested_স্তূপ(পূর্ণাঙ্ক (*fn)(শূন্য *), পূর্ণাঙ্ক flags, শূন্য *userdata);

/* 💣 Note that FORK_NEW_USERNS, FORK_NEW_MOUNTNS, FORK_NEW_NETNS or FORK_NEW_PIDNS should not be called in threaded
 * programs, because they cause us to use raw_clone() which does not synchronize the glibc malloc() locks,
 * and thus will cause deadlocks if the parent uses threads and the child does memory allocations. Hence: if
 * the parent is threaded these flags may not be used. These flags cannot be used if the parent uses threads
 * or the child uses malloc(). 💣 */
প্রকারনির্দেশ গণনা ForkFlags {
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

পূর্ণাঙ্ক pidref_safe_fork_full(
                ধ্রুৱ ৱর্ণ *name,
                ধ্রুৱ পূর্ণাঙ্ক stdio_fds[3],
                পূর্ণাঙ্ক except_fds[],
                size_t n_except_fds,
                ForkFlags flags,
                PidRef *ret);

স্থির অন্তঃস্থ পূর্ণাঙ্ক pidref_safe_fork(ধ্রুৱ ৱর্ণ *name, ForkFlags flags, PidRef *ret) {
        ঘূৰাই pidref_safe_fork_full(name, রিক্ত, রিক্ত, 0, flags, ret);
}

পূর্ণাঙ্ক namespace_fork_full(
                ধ্রুৱ ৱর্ণ *outer_name,
                ধ্রুৱ ৱর্ণ *inner_name,
                পূর্ণাঙ্ক except_fds[],
                size_t n_except_fds,
                ForkFlags flags,
                পূর্ণাঙ্ক pidns_fd,
                পূর্ণাঙ্ক mntns_fd,
                পূর্ণাঙ্ক netns_fd,
                পূর্ণাঙ্ক userns_fd,
                পূর্ণাঙ্ক মূল_fd,
                NamespaceEnterFlags namespace_flags,
                PidRef *ret);

পূর্ণাঙ্ক namespace_fork(
                ধ্রুৱ ৱর্ণ *outer_name,
                ধ্রুৱ ৱর্ণ *inner_name,
                ForkFlags flags,
                পূর্ণাঙ্ক pidns_fd,
                পূর্ণাঙ্ক mntns_fd,
                পূর্ণাঙ্ক netns_fd,
                পূর্ণাঙ্ক userns_fd,
                পূর্ণাঙ্ক মূল_fd,
                PidRef *ret);

bool oom_score_adjust_is_valid(পূর্ণাঙ্ক oa);
পূর্ণাঙ্ক set_oom_score_adjust(পূর্ণাঙ্ক মান);
পূর্ণাঙ্ক get_oom_score_adjust(পূর্ণাঙ্ক *ret);

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

পূর্ণাঙ্ক setpriority_closest(পূর্ণাঙ্ক priority);

_noreturn_ শূন্য freeze(শূন্য);

পূর্ণাঙ্ক get_process_threads(pid_t pid);

পূর্ণাঙ্ক is_reaper_process(শূন্য);
পূর্ণাঙ্ক make_reaper_process(bool b);

পূর্ণাঙ্ক posix_spawn_wrapper(
                ধ্রুৱ ৱর্ণ *path,
                ৱর্ণ * ধ্রুৱ *argv,
                ৱর্ণ * ধ্রুৱ *envp,
                ধ্রুৱ ৱর্ণ *cgroup,
                PidRef *ret_pidref);

পূর্ণাঙ্ক proc_dir_open(DIR **ret);
পূর্ণাঙ্ক proc_dir_read(DIR *d, pid_t *ret);
পূর্ণাঙ্ক proc_dir_read_pidref(DIR *d, PidRef *ret);

পূর্ণাঙ্ক safe_mlockall(পূর্ণাঙ্ক flags);

_noreturn_ শূন্য report_errno_and_exit(পূর্ণাঙ্ক errno_fd, পূর্ণাঙ্ক error);
পূর্ণাঙ্ক read_errno(পূর্ণাঙ্ক errno_fd);

পূর্ণাঙ্ক prctl_safe(পূর্ণাঙ্ক op, unsigned long arg2, unsigned long arg3, unsigned long arg4, unsigned long arg5);

পূর্ণাঙ্ক proc_set_comm(ধ্রুৱ ৱর্ণ *comm);
পূর্ণাঙ্ক proc_set_nnp(শূন্য);
nsec_t proc_get_timerslack(শূন্য);
