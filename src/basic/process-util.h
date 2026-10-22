/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ <signal.h>

#અન્તર્ભાવ "fileio.h"
#અન્તર્ભાવ "format-util.h"
#અન્તર્ભાવ "forward.h"
#અન્તર્ભાવ "string-util.h"

#define procfs_file_alloca(pid, field)                                  \
        ({                                                              \
                pid_t _pid_ = (pid);                                    \
                અચલ અક્ષર *_field_ = (field);                          \
                અક્ષર *_r_;                                              \
                જો (_pid_ == 0)                                         \
                        _r_ = strjoina("/proc/self/", _field_);         \
                નહીંતર {                                                  \
                        પ્રતિજ્ઞા(_pid_ > 0);                              \
                        _r_ = newa(અક્ષર, STRLEN("/proc/") + DECIMAL_STR_MAX(pid_t) + 1 + strlen(_field_) + 1); \
                        sprintf(_r_, "/proc/" PID_FMT "/%s", _pid_, _field_); \
                }                                                       \
                (અચલ અક્ષર*) _r_;                                      \
        })

સ્થિર અંતઃરેખ પૂર્ણાંક procfs_file_get_field(pid_t pid, અચલ અક્ષર *name, અચલ અક્ષર *ચાવી, અક્ષર **ret) {
        પાછા get_proc_field(procfs_file_alloca(pid, name), ચાવી, ret);
}

પ્રકારવ્યાખ્યા ગણના ProcessCmdlineFlags {
        PROCESS_CMDLINE_COMM_FALLBACK = 1 << 0,
        PROCESS_CMDLINE_USE_LOCALE    = 1 << 1,
        PROCESS_CMDLINE_QUOTE         = 1 << 2,
        PROCESS_CMDLINE_QUOTE_POSIX   = 1 << 3,
} ProcessCmdlineFlags;

પૂર્ણાંક pid_get_comm(pid_t pid, અક્ષર **ret);
પૂર્ણાંક pidref_get_comm(અચલ PidRef *pid, અક્ષર **ret);
પૂર્ણાંક pid_get_cmdline(pid_t pid, size_t max_columns, ProcessCmdlineFlags flags, અક્ષર **ret);
પૂર્ણાંક pidref_get_cmdline(અચલ PidRef *pid, size_t max_columns, ProcessCmdlineFlags flags, અક્ષર **ret);
પૂર્ણાંક pid_get_cmdline_strv(pid_t pid, ProcessCmdlineFlags flags, અક્ષર ***ret);
પૂર્ણાંક pidref_get_cmdline_strv(અચલ PidRef *pid, ProcessCmdlineFlags flags, અક્ષર ***ret);
પૂર્ણાંક get_process_exe(pid_t pid, અક્ષર **ret);
પૂર્ણાંક pid_get_uid(pid_t pid, uid_t *ret);
પૂર્ણાંક pidref_get_uid(અચલ PidRef *pid, uid_t *ret);
પૂર્ણાંક get_process_gid(pid_t pid, gid_t *ret);
પૂર્ણાંક get_process_cwd(pid_t pid, અક્ષર **ret);
પૂર્ણાંક get_process_મૂળ(pid_t pid, અક્ષર **ret);
પૂર્ણાંક get_process_environ(pid_t pid, અક્ષર **ret);
પૂર્ણાંક pid_get_ppid(pid_t pid, pid_t *ret);
પૂર્ણાંક pidref_get_ppid(અચલ PidRef *pidref, pid_t *ret);
પૂર્ણાંક pidref_get_ppid_as_pidref(અચલ PidRef *pidref, PidRef *ret);
પૂર્ણાંક pid_get_start_time(pid_t pid, usec_t *ret);
પૂર્ણાંક pidref_get_start_time(અચલ PidRef *pid, usec_t *ret);
પૂર્ણાંક get_process_umask(pid_t pid, mode_t *ret);
પૂર્ણાંક pidref_has_sigcgt(અચલ PidRef *pidref, પૂર્ણાંક sig);
પૂર્ણાંક pidref_has_sigblk(અચલ PidRef *pidref, પૂર્ણાંક sig);

સ્થિર અંતઃરેખ bool SIGINFO_CODE_IS_DEAD(પૂર્ણાંક code) {
        પાછા IN_SET(code, CLD_EXITED, CLD_KILLED, CLD_DUMPED);
}

પ્રકારવ્યાખ્યા ગણના WaitFlags {
        WAIT_LOG_ABNORMAL             = 1 << 0,
        WAIT_LOG_NON_ZERO_EXIT_STATUS = 1 << 1,

        /* A shortcut for requesting the most complete logging */
        WAIT_LOG = WAIT_LOG_ABNORMAL|WAIT_LOG_NON_ZERO_EXIT_STATUS,
} WaitFlags;

પૂર્ણાંક pidref_wait_for_terminate_and_check(અચલ અક્ષર *name, PidRef *pidref, WaitFlags flags);

પૂર્ણાંક kill_and_sigcont(pid_t pid, પૂર્ણાંક sig);

પૂર્ણાંક procfs_get_stat_flags(અચલ અક્ષર *path, unsigned long long *ret);

પૂર્ણાંક pid_is_kernel_thread(pid_t pid);
પૂર્ણાંક pidref_is_kernel_thread(અચલ PidRef *pid);

પૂર્ણાંક getenv_for_pid(pid_t pid, અચલ અક્ષર *field, અક્ષર **ret);

પૂર્ણાંક pid_is_alive(pid_t pid);
પૂર્ણાંક pidref_is_alive(અચલ PidRef *pidref);
પૂર્ણાંક pid_is_unwaited(pid_t pid);
પૂર્ણાંક pidref_is_unwaited(PidRef *pidref);
પૂર્ણાંક pid_is_my_child(pid_t pid);
પૂર્ણાંક pidref_is_my_child(PidRef *pidref);
પૂર્ણાંક pidref_from_same_મૂળ_fs(PidRef *a, PidRef *b);

bool is_main_thread(શૂન્ય);

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

unsigned long personality_from_string(અચલ અક્ષર *s);
અચલ અક્ષર* personality_to_string(unsigned long p);

પૂર્ણાંક safe_personality(unsigned long p);
પૂર્ણાંક opinionated_personality(unsigned long *ret);

DECLARE_STRING_TABLE_LOOKUP(sigchld_code, પૂર્ણાંક);
DECLARE_STRING_TABLE_LOOKUP_WITH_FALLBACK(sched_policy, પૂર્ણાંક);

સ્થિર અંતઃરેખ pid_t PTR_TO_PID(અચલ શૂન્ય *p) {
        પાછા (pid_t) ((uintptr_t) p);
}

સ્થિર અંતઃરેખ શૂન્ય* PID_TO_PTR(pid_t pid) {
        પાછા (શૂન્ય*) ((uintptr_t) pid);
}

શૂન્ય valgrind_summary_hack(શૂન્ય);

પૂર્ણાંક pid_compare_func(અચલ pid_t *a, અચલ pid_t *b);

bool nice_is_valid(પૂર્ણાંક n) _const_;

bool sched_policy_is_valid(પૂર્ણાંક policy) _const_;
bool sched_policy_supported(પૂર્ણાંક policy);
પૂર્ણાંક sched_get_priority_min_safe(પૂર્ણાંક policy);
પૂર્ણાંક sched_get_priority_max_safe(પૂર્ણાંક policy);

#define PID_AUTOMATIC ((pid_t) INT_MIN) /* special value indicating "acquire pid from connection peer" */

સ્થિર અંતઃરેખ bool pid_is_valid(pid_t p) {
        પાછા p > 0;
}

સ્થિર અંતઃરેખ bool pid_is_automatic(pid_t p) {
        પાછા p == PID_AUTOMATIC;
}

pid_t getpid_cached(શૂન્ય);
શૂન્ય reset_cached_pid(શૂન્ય);

પૂર્ણાંક must_be_મૂળ(શૂન્ય);

pid_t clone_with_nested_ઢગલો(પૂર્ણાંક (*fn)(શૂન્ય *), પૂર્ણાંક flags, શૂન્ય *userdata);

/* 💣 Note that FORK_NEW_USERNS, FORK_NEW_MOUNTNS, FORK_NEW_NETNS or FORK_NEW_PIDNS should not be called in threaded
 * programs, because they cause us to use raw_clone() which does not synchronize the glibc malloc() locks,
 * and thus will cause deadlocks if the parent uses threads and the child does memory allocations. Hence: if
 * the parent is threaded these flags may not be used. These flags cannot be used if the parent uses threads
 * or the child uses malloc(). 💣 */
પ્રકારવ્યાખ્યા ગણના ForkFlags {
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

પૂર્ણાંક pidref_safe_fork_full(
                અચલ અક્ષર *name,
                અચલ પૂર્ણાંક stdio_fds[3],
                પૂર્ણાંક except_fds[],
                size_t n_except_fds,
                ForkFlags flags,
                PidRef *ret);

સ્થિર અંતઃરેખ પૂર્ણાંક pidref_safe_fork(અચલ અક્ષર *name, ForkFlags flags, PidRef *ret) {
        પાછા pidref_safe_fork_full(name, ખાલી, ખાલી, 0, flags, ret);
}

પૂર્ણાંક namespace_fork_full(
                અચલ અક્ષર *outer_name,
                અચલ અક્ષર *inner_name,
                પૂર્ણાંક except_fds[],
                size_t n_except_fds,
                ForkFlags flags,
                પૂર્ણાંક pidns_fd,
                પૂર્ણાંક mntns_fd,
                પૂર્ણાંક netns_fd,
                પૂર્ણાંક userns_fd,
                પૂર્ણાંક મૂળ_fd,
                NamespaceEnterFlags namespace_flags,
                PidRef *ret);

પૂર્ણાંક namespace_fork(
                અચલ અક્ષર *outer_name,
                અચલ અક્ષર *inner_name,
                ForkFlags flags,
                પૂર્ણાંક pidns_fd,
                પૂર્ણાંક mntns_fd,
                પૂર્ણાંક netns_fd,
                પૂર્ણાંક userns_fd,
                પૂર્ણાંક મૂળ_fd,
                PidRef *ret);

bool oom_score_adjust_is_valid(પૂર્ણાંક oa);
પૂર્ણાંક set_oom_score_adjust(પૂર્ણાંક કિંમત);
પૂર્ણાંક get_oom_score_adjust(પૂર્ણાંક *ret);

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

પૂર્ણાંક setpriority_closest(પૂર્ણાંક priority);

_noreturn_ શૂન્ય freeze(શૂન્ય);

પૂર્ણાંક get_process_threads(pid_t pid);

પૂર્ણાંક is_reaper_process(શૂન્ય);
પૂર્ણાંક make_reaper_process(bool b);

પૂર્ણાંક posix_spawn_wrapper(
                અચલ અક્ષર *path,
                અક્ષર * અચલ *argv,
                અક્ષર * અચલ *envp,
                અચલ અક્ષર *cgroup,
                PidRef *ret_pidref);

પૂર્ણાંક proc_dir_open(DIR **ret);
પૂર્ણાંક proc_dir_read(DIR *d, pid_t *ret);
પૂર્ણાંક proc_dir_read_pidref(DIR *d, PidRef *ret);

પૂર્ણાંક safe_mlockall(પૂર્ણાંક flags);

_noreturn_ શૂન્ય report_errno_and_exit(પૂર્ણાંક errno_fd, પૂર્ણાંક error);
પૂર્ણાંક read_errno(પૂર્ણાંક errno_fd);

પૂર્ણાંક prctl_safe(પૂર્ણાંક op, unsigned long arg2, unsigned long arg3, unsigned long arg4, unsigned long arg5);

પૂર્ણાંક proc_set_comm(અચલ અક્ષર *comm);
પૂર્ણાંક proc_set_nnp(શૂન્ય);
nsec_t proc_get_timerslack(શૂન્ય);
