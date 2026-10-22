/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব <signal.h>

#অন্তর্ভাব "fileio.h"
#অন্তর্ভাব "format-util.h"
#অন্তর্ভাব "forward.h"
#অন্তর্ভাব "string-util.h"

#define procfs_file_alloca(pid, field)                                  \
        ({                                                              \
                pid_t _pid_ = (pid);                                    \
                ধ্রুবক অক্ষর *_field_ = (field);                          \
                অক্ষর *_r_;                                              \
                যদি (_pid_ == 0)                                         \
                        _r_ = strjoina("/proc/self/", _field_);         \
                অন্যথা {                                                  \
                        প্রতিজ্ঞা(_pid_ > 0);                              \
                        _r_ = newa(অক্ষর, STRLEN("/proc/") + DECIMAL_STR_MAX(pid_t) + 1 + strlen(_field_) + 1); \
                        sprintf(_r_, "/proc/" PID_FMT "/%s", _pid_, _field_); \
                }                                                       \
                (ধ্রুবক অক্ষর*) _r_;                                      \
        })

স্থির অন্তঃরেখ পূর্ণসংখ্যা procfs_file_get_field(pid_t pid, ধ্রুবক অক্ষর *name, ধ্রুবক অক্ষর *চাবি, অক্ষর **ret) {
        ফেরত get_proc_field(procfs_file_alloca(pid, name), চাবি, ret);
}

প্রকারলেখ গণনা ProcessCmdlineFlags {
        PROCESS_CMDLINE_COMM_FALLBACK = 1 << 0,
        PROCESS_CMDLINE_USE_LOCALE    = 1 << 1,
        PROCESS_CMDLINE_QUOTE         = 1 << 2,
        PROCESS_CMDLINE_QUOTE_POSIX   = 1 << 3,
} ProcessCmdlineFlags;

পূর্ণসংখ্যা pid_get_comm(pid_t pid, অক্ষর **ret);
পূর্ণসংখ্যা pidref_get_comm(ধ্রুবক PidRef *pid, অক্ষর **ret);
পূর্ণসংখ্যা pid_get_cmdline(pid_t pid, size_t max_columns, ProcessCmdlineFlags flags, অক্ষর **ret);
পূর্ণসংখ্যা pidref_get_cmdline(ধ্রুবক PidRef *pid, size_t max_columns, ProcessCmdlineFlags flags, অক্ষর **ret);
পূর্ণসংখ্যা pid_get_cmdline_strv(pid_t pid, ProcessCmdlineFlags flags, অক্ষর ***ret);
পূর্ণসংখ্যা pidref_get_cmdline_strv(ধ্রুবক PidRef *pid, ProcessCmdlineFlags flags, অক্ষর ***ret);
পূর্ণসংখ্যা get_process_exe(pid_t pid, অক্ষর **ret);
পূর্ণসংখ্যা pid_get_uid(pid_t pid, uid_t *ret);
পূর্ণসংখ্যা pidref_get_uid(ধ্রুবক PidRef *pid, uid_t *ret);
পূর্ণসংখ্যা get_process_gid(pid_t pid, gid_t *ret);
পূর্ণসংখ্যা get_process_cwd(pid_t pid, অক্ষর **ret);
পূর্ণসংখ্যা get_process_মূল(pid_t pid, অক্ষর **ret);
পূর্ণসংখ্যা get_process_environ(pid_t pid, অক্ষর **ret);
পূর্ণসংখ্যা pid_get_ppid(pid_t pid, pid_t *ret);
পূর্ণসংখ্যা pidref_get_ppid(ধ্রুবক PidRef *pidref, pid_t *ret);
পূর্ণসংখ্যা pidref_get_ppid_as_pidref(ধ্রুবক PidRef *pidref, PidRef *ret);
পূর্ণসংখ্যা pid_get_start_time(pid_t pid, usec_t *ret);
পূর্ণসংখ্যা pidref_get_start_time(ধ্রুবক PidRef *pid, usec_t *ret);
পূর্ণসংখ্যা get_process_umask(pid_t pid, mode_t *ret);
পূর্ণসংখ্যা pidref_has_sigcgt(ধ্রুবক PidRef *pidref, পূর্ণসংখ্যা sig);
পূর্ণসংখ্যা pidref_has_sigblk(ধ্রুবক PidRef *pidref, পূর্ণসংখ্যা sig);

স্থির অন্তঃরেখ bool SIGINFO_CODE_IS_DEAD(পূর্ণসংখ্যা code) {
        ফেরত IN_SET(code, CLD_EXITED, CLD_KILLED, CLD_DUMPED);
}

প্রকারলেখ গণনা WaitFlags {
        WAIT_LOG_ABNORMAL             = 1 << 0,
        WAIT_LOG_NON_ZERO_EXIT_STATUS = 1 << 1,

        /* A shortcut for requesting the most complete logging */
        WAIT_LOG = WAIT_LOG_ABNORMAL|WAIT_LOG_NON_ZERO_EXIT_STATUS,
} WaitFlags;

পূর্ণসংখ্যা pidref_wait_for_terminate_and_check(ধ্রুবক অক্ষর *name, PidRef *pidref, WaitFlags flags);

পূর্ণসংখ্যা kill_and_sigcont(pid_t pid, পূর্ণসংখ্যা sig);

পূর্ণসংখ্যা procfs_get_stat_flags(ধ্রুবক অক্ষর *path, unsigned long long *ret);

পূর্ণসংখ্যা pid_is_kernel_thread(pid_t pid);
পূর্ণসংখ্যা pidref_is_kernel_thread(ধ্রুবক PidRef *pid);

পূর্ণসংখ্যা getenv_for_pid(pid_t pid, ধ্রুবক অক্ষর *field, অক্ষর **ret);

পূর্ণসংখ্যা pid_is_alive(pid_t pid);
পূর্ণসংখ্যা pidref_is_alive(ধ্রুবক PidRef *pidref);
পূর্ণসংখ্যা pid_is_unwaited(pid_t pid);
পূর্ণসংখ্যা pidref_is_unwaited(PidRef *pidref);
পূর্ণসংখ্যা pid_is_my_child(pid_t pid);
পূর্ণসংখ্যা pidref_is_my_child(PidRef *pidref);
পূর্ণসংখ্যা pidref_from_same_মূল_fs(PidRef *a, PidRef *b);

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

unsigned long personality_from_string(ধ্রুবক অক্ষর *s);
ধ্রুবক অক্ষর* personality_to_string(unsigned long p);

পূর্ণসংখ্যা safe_personality(unsigned long p);
পূর্ণসংখ্যা opinionated_personality(unsigned long *ret);

DECLARE_STRING_TABLE_LOOKUP(sigchld_code, পূর্ণসংখ্যা);
DECLARE_STRING_TABLE_LOOKUP_WITH_FALLBACK(sched_policy, পূর্ণসংখ্যা);

স্থির অন্তঃরেখ pid_t PTR_TO_PID(ধ্রুবক শূন্য *p) {
        ফেরত (pid_t) ((uintptr_t) p);
}

স্থির অন্তঃরেখ শূন্য* PID_TO_PTR(pid_t pid) {
        ফেরত (শূন্য*) ((uintptr_t) pid);
}

শূন্য valgrind_summary_hack(শূন্য);

পূর্ণসংখ্যা pid_compare_func(ধ্রুবক pid_t *a, ধ্রুবক pid_t *b);

bool nice_is_valid(পূর্ণসংখ্যা n) _const_;

bool sched_policy_is_valid(পূর্ণসংখ্যা policy) _const_;
bool sched_policy_supported(পূর্ণসংখ্যা policy);
পূর্ণসংখ্যা sched_get_priority_min_safe(পূর্ণসংখ্যা policy);
পূর্ণসংখ্যা sched_get_priority_max_safe(পূর্ণসংখ্যা policy);

#define PID_AUTOMATIC ((pid_t) INT_MIN) /* special value indicating "acquire pid from connection peer" */

স্থির অন্তঃরেখ bool pid_is_valid(pid_t p) {
        ফেরত p > 0;
}

স্থির অন্তঃরেখ bool pid_is_automatic(pid_t p) {
        ফেরত p == PID_AUTOMATIC;
}

pid_t getpid_cached(শূন্য);
শূন্য reset_cached_pid(শূন্য);

পূর্ণসংখ্যা must_be_মূল(শূন্য);

pid_t clone_with_nested_স্তূপ(পূর্ণসংখ্যা (*fn)(শূন্য *), পূর্ণসংখ্যা flags, শূন্য *userdata);

/* 💣 Note that FORK_NEW_USERNS, FORK_NEW_MOUNTNS, FORK_NEW_NETNS or FORK_NEW_PIDNS should not be called in threaded
 * programs, because they cause us to use raw_clone() which does not synchronize the glibc malloc() locks,
 * and thus will cause deadlocks if the parent uses threads and the child does memory allocations. Hence: if
 * the parent is threaded these flags may not be used. These flags cannot be used if the parent uses threads
 * or the child uses malloc(). 💣 */
প্রকারলেখ গণনা ForkFlags {
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

পূর্ণসংখ্যা pidref_safe_fork_full(
                ধ্রুবক অক্ষর *name,
                ধ্রুবক পূর্ণসংখ্যা stdio_fds[3],
                পূর্ণসংখ্যা except_fds[],
                size_t n_except_fds,
                ForkFlags flags,
                PidRef *ret);

স্থির অন্তঃরেখ পূর্ণসংখ্যা pidref_safe_fork(ধ্রুবক অক্ষর *name, ForkFlags flags, PidRef *ret) {
        ফেরত pidref_safe_fork_full(name, ফাঁকা, ফাঁকা, 0, flags, ret);
}

পূর্ণসংখ্যা namespace_fork_full(
                ধ্রুবক অক্ষর *outer_name,
                ধ্রুবক অক্ষর *inner_name,
                পূর্ণসংখ্যা except_fds[],
                size_t n_except_fds,
                ForkFlags flags,
                পূর্ণসংখ্যা pidns_fd,
                পূর্ণসংখ্যা mntns_fd,
                পূর্ণসংখ্যা netns_fd,
                পূর্ণসংখ্যা userns_fd,
                পূর্ণসংখ্যা মূল_fd,
                NamespaceEnterFlags namespace_flags,
                PidRef *ret);

পূর্ণসংখ্যা namespace_fork(
                ধ্রুবক অক্ষর *outer_name,
                ধ্রুবক অক্ষর *inner_name,
                ForkFlags flags,
                পূর্ণসংখ্যা pidns_fd,
                পূর্ণসংখ্যা mntns_fd,
                পূর্ণসংখ্যা netns_fd,
                পূর্ণসংখ্যা userns_fd,
                পূর্ণসংখ্যা মূল_fd,
                PidRef *ret);

bool oom_score_adjust_is_valid(পূর্ণসংখ্যা oa);
পূর্ণসংখ্যা set_oom_score_adjust(পূর্ণসংখ্যা মান);
পূর্ণসংখ্যা get_oom_score_adjust(পূর্ণসংখ্যা *ret);

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

পূর্ণসংখ্যা setpriority_closest(পূর্ণসংখ্যা priority);

_noreturn_ শূন্য freeze(শূন্য);

পূর্ণসংখ্যা get_process_threads(pid_t pid);

পূর্ণসংখ্যা is_reaper_process(শূন্য);
পূর্ণসংখ্যা make_reaper_process(bool b);

পূর্ণসংখ্যা posix_spawn_wrapper(
                ধ্রুবক অক্ষর *path,
                অক্ষর * ধ্রুবক *argv,
                অক্ষর * ধ্রুবক *envp,
                ধ্রুবক অক্ষর *cgroup,
                PidRef *ret_pidref);

পূর্ণসংখ্যা proc_dir_open(DIR **ret);
পূর্ণসংখ্যা proc_dir_read(DIR *d, pid_t *ret);
পূর্ণসংখ্যা proc_dir_read_pidref(DIR *d, PidRef *ret);

পূর্ণসংখ্যা safe_mlockall(পূর্ণসংখ্যা flags);

_noreturn_ শূন্য report_errno_and_exit(পূর্ণসংখ্যা errno_fd, পূর্ণসংখ্যা error);
পূর্ণসংখ্যা read_errno(পূর্ণসংখ্যা errno_fd);

পূর্ণসংখ্যা prctl_safe(পূর্ণসংখ্যা op, unsigned long arg2, unsigned long arg3, unsigned long arg4, unsigned long arg5);

পূর্ণসংখ্যা proc_set_comm(ধ্রুবক অক্ষর *comm);
পূর্ণসংখ্যা proc_set_nnp(শূন্য);
nsec_t proc_get_timerslack(শূন্য);
