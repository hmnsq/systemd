/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ <signal.h>

#அந்தர்பாவ "fileio.h"
#அந்தர்பாவ "format-util.h"
#அந்தர்பாவ "forward.h"
#அந்தர்பாவ "string-util.h"

#define procfs_file_alloca(pid, field)                                  \
        ({                                                              \
                pid_t _pid_ = (pid);                                    \
                மாறிலி எழுத்து *_field_ = (field);                          \
                எழுத்து *_r_;                                              \
                எனில் (_pid_ == 0)                                         \
                        _r_ = strjoina("/proc/self/", _field_);         \
                இல்லையெனில் {                                                  \
                        ப்ரதிஜ்ஞா(_pid_ > 0);                              \
                        _r_ = newa(எழுத்து, STRLEN("/proc/") + DECIMAL_STR_MAX(pid_t) + 1 + strlen(_field_) + 1); \
                        sprintf(_r_, "/proc/" PID_FMT "/%s", _pid_, _field_); \
                }                                                       \
                (மாறிலி எழுத்து*) _r_;                                      \
        })

நிலையான உள்வரி முழு procfs_file_get_field(pid_t pid, மாறிலி எழுத்து *name, மாறிலி எழுத்து *சாவி, எழுத்து **ret) {
        திரும்பு get_proc_field(procfs_file_alloca(pid, name), சாவி, ret);
}

வகைவரை எண்ணிக்கை ProcessCmdlineFlags {
        PROCESS_CMDLINE_COMM_FALLBACK = 1 << 0,
        PROCESS_CMDLINE_USE_LOCALE    = 1 << 1,
        PROCESS_CMDLINE_QUOTE         = 1 << 2,
        PROCESS_CMDLINE_QUOTE_POSIX   = 1 << 3,
} ProcessCmdlineFlags;

முழு pid_get_comm(pid_t pid, எழுத்து **ret);
முழு pidref_get_comm(மாறிலி PidRef *pid, எழுத்து **ret);
முழு pid_get_cmdline(pid_t pid, size_t max_columns, ProcessCmdlineFlags flags, எழுத்து **ret);
முழு pidref_get_cmdline(மாறிலி PidRef *pid, size_t max_columns, ProcessCmdlineFlags flags, எழுத்து **ret);
முழு pid_get_cmdline_strv(pid_t pid, ProcessCmdlineFlags flags, எழுத்து ***ret);
முழு pidref_get_cmdline_strv(மாறிலி PidRef *pid, ProcessCmdlineFlags flags, எழுத்து ***ret);
முழு get_process_exe(pid_t pid, எழுத்து **ret);
முழு pid_get_uid(pid_t pid, uid_t *ret);
முழு pidref_get_uid(மாறிலி PidRef *pid, uid_t *ret);
முழு get_process_gid(pid_t pid, gid_t *ret);
முழு get_process_cwd(pid_t pid, எழுத்து **ret);
முழு get_process_வேர்(pid_t pid, எழுத்து **ret);
முழு get_process_environ(pid_t pid, எழுத்து **ret);
முழு pid_get_ppid(pid_t pid, pid_t *ret);
முழு pidref_get_ppid(மாறிலி PidRef *pidref, pid_t *ret);
முழு pidref_get_ppid_as_pidref(மாறிலி PidRef *pidref, PidRef *ret);
முழு pid_get_start_time(pid_t pid, usec_t *ret);
முழு pidref_get_start_time(மாறிலி PidRef *pid, usec_t *ret);
முழு get_process_umask(pid_t pid, mode_t *ret);
முழு pidref_has_sigcgt(மாறிலி PidRef *pidref, முழு sig);
முழு pidref_has_sigblk(மாறிலி PidRef *pidref, முழு sig);

நிலையான உள்வரி bool SIGINFO_CODE_IS_DEAD(முழு code) {
        திரும்பு IN_SET(code, CLD_EXITED, CLD_KILLED, CLD_DUMPED);
}

வகைவரை எண்ணிக்கை WaitFlags {
        WAIT_LOG_ABNORMAL             = 1 << 0,
        WAIT_LOG_NON_ZERO_EXIT_STATUS = 1 << 1,

        /* A shortcut for requesting the most complete logging */
        WAIT_LOG = WAIT_LOG_ABNORMAL|WAIT_LOG_NON_ZERO_EXIT_STATUS,
} WaitFlags;

முழு pidref_wait_for_terminate_and_check(மாறிலி எழுத்து *name, PidRef *pidref, WaitFlags flags);

முழு kill_and_sigcont(pid_t pid, முழு sig);

முழு procfs_get_stat_flags(மாறிலி எழுத்து *path, unsigned long long *ret);

முழு pid_is_kernel_thread(pid_t pid);
முழு pidref_is_kernel_thread(மாறிலி PidRef *pid);

முழு getenv_for_pid(pid_t pid, மாறிலி எழுத்து *field, எழுத்து **ret);

முழு pid_is_alive(pid_t pid);
முழு pidref_is_alive(மாறிலி PidRef *pidref);
முழு pid_is_unwaited(pid_t pid);
முழு pidref_is_unwaited(PidRef *pidref);
முழு pid_is_my_child(pid_t pid);
முழு pidref_is_my_child(PidRef *pidref);
முழு pidref_from_same_வேர்_fs(PidRef *a, PidRef *b);

bool is_main_thread(வெறுமை);

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

unsigned long personality_from_string(மாறிலி எழுத்து *s);
மாறிலி எழுத்து* personality_to_string(unsigned long p);

முழு safe_personality(unsigned long p);
முழு opinionated_personality(unsigned long *ret);

DECLARE_STRING_TABLE_LOOKUP(sigchld_code, முழு);
DECLARE_STRING_TABLE_LOOKUP_WITH_FALLBACK(sched_policy, முழு);

நிலையான உள்வரி pid_t PTR_TO_PID(மாறிலி வெறுமை *p) {
        திரும்பு (pid_t) ((uintptr_t) p);
}

நிலையான உள்வரி வெறுமை* PID_TO_PTR(pid_t pid) {
        திரும்பு (வெறுமை*) ((uintptr_t) pid);
}

வெறுமை valgrind_summary_hack(வெறுமை);

முழு pid_compare_func(மாறிலி pid_t *a, மாறிலி pid_t *b);

bool nice_is_valid(முழு n) _const_;

bool sched_policy_is_valid(முழு policy) _const_;
bool sched_policy_supported(முழு policy);
முழு sched_get_priority_min_safe(முழு policy);
முழு sched_get_priority_max_safe(முழு policy);

#define PID_AUTOMATIC ((pid_t) INT_MIN) /* special value indicating "acquire pid from connection peer" */

நிலையான உள்வரி bool pid_is_valid(pid_t p) {
        திரும்பு p > 0;
}

நிலையான உள்வரி bool pid_is_automatic(pid_t p) {
        திரும்பு p == PID_AUTOMATIC;
}

pid_t getpid_cached(வெறுமை);
வெறுமை reset_cached_pid(வெறுமை);

முழு must_be_வேர்(வெறுமை);

pid_t clone_with_nested_அடுக்கு(முழு (*fn)(வெறுமை *), முழு flags, வெறுமை *userdata);

/* 💣 Note that FORK_NEW_USERNS, FORK_NEW_MOUNTNS, FORK_NEW_NETNS or FORK_NEW_PIDNS should not be called in threaded
 * programs, because they cause us to use raw_clone() which does not synchronize the glibc malloc() locks,
 * and thus will cause deadlocks if the parent uses threads and the child does memory allocations. Hence: if
 * the parent is threaded these flags may not be used. These flags cannot be used if the parent uses threads
 * or the child uses malloc(). 💣 */
வகைவரை எண்ணிக்கை ForkFlags {
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

முழு pidref_safe_fork_full(
                மாறிலி எழுத்து *name,
                மாறிலி முழு stdio_fds[3],
                முழு except_fds[],
                size_t n_except_fds,
                ForkFlags flags,
                PidRef *ret);

நிலையான உள்வரி முழு pidref_safe_fork(மாறிலி எழுத்து *name, ForkFlags flags, PidRef *ret) {
        திரும்பு pidref_safe_fork_full(name, வெற்று, வெற்று, 0, flags, ret);
}

முழு namespace_fork_full(
                மாறிலி எழுத்து *outer_name,
                மாறிலி எழுத்து *inner_name,
                முழு except_fds[],
                size_t n_except_fds,
                ForkFlags flags,
                முழு pidns_fd,
                முழு mntns_fd,
                முழு netns_fd,
                முழு userns_fd,
                முழு வேர்_fd,
                NamespaceEnterFlags namespace_flags,
                PidRef *ret);

முழு namespace_fork(
                மாறிலி எழுத்து *outer_name,
                மாறிலி எழுத்து *inner_name,
                ForkFlags flags,
                முழு pidns_fd,
                முழு mntns_fd,
                முழு netns_fd,
                முழு userns_fd,
                முழு வேர்_fd,
                PidRef *ret);

bool oom_score_adjust_is_valid(முழு oa);
முழு set_oom_score_adjust(முழு மதிப்பு);
முழு get_oom_score_adjust(முழு *ret);

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

முழு setpriority_closest(முழு priority);

_noreturn_ வெறுமை freeze(வெறுமை);

முழு get_process_threads(pid_t pid);

முழு is_reaper_process(வெறுமை);
முழு make_reaper_process(bool b);

முழு posix_spawn_wrapper(
                மாறிலி எழுத்து *path,
                எழுத்து * மாறிலி *argv,
                எழுத்து * மாறிலி *envp,
                மாறிலி எழுத்து *cgroup,
                PidRef *ret_pidref);

முழு proc_dir_open(DIR **ret);
முழு proc_dir_read(DIR *d, pid_t *ret);
முழு proc_dir_read_pidref(DIR *d, PidRef *ret);

முழு safe_mlockall(முழு flags);

_noreturn_ வெறுமை report_errno_and_exit(முழு errno_fd, முழு error);
முழு read_errno(முழு errno_fd);

முழு prctl_safe(முழு op, unsigned long arg2, unsigned long arg3, unsigned long arg4, unsigned long arg5);

முழு proc_set_comm(மாறிலி எழுத்து *comm);
முழு proc_set_nnp(வெறுமை);
nsec_t proc_get_timerslack(வெறுமை);
