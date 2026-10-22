/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ <signal.h>

#ଅନ୍ତର୍ଭାଵ "fileio.h"
#ଅନ୍ତର୍ଭାଵ "format-util.h"
#ଅନ୍ତର୍ଭାଵ "forward.h"
#ଅନ୍ତର୍ଭାଵ "string-util.h"

#define procfs_file_alloca(pid, field)                                  \
        ({                                                              \
                pid_t _pid_ = (pid);                                    \
                ଅଚଳ ଅକ୍ଷର *_field_ = (field);                          \
                ଅକ୍ଷର *_r_;                                              \
                ଯଦି (_pid_ == 0)                                         \
                        _r_ = strjoina("/proc/self/", _field_);         \
                ଅନ୍ୟଥା {                                                  \
                        ପ୍ରତିଜ୍ଞା(_pid_ > 0);                              \
                        _r_ = newa(ଅକ୍ଷର, STRLEN("/proc/") + DECIMAL_STR_MAX(pid_t) + 1 + strlen(_field_) + 1); \
                        sprintf(_r_, "/proc/" PID_FMT "/%s", _pid_, _field_); \
                }                                                       \
                (ଅଚଳ ଅକ୍ଷର*) _r_;                                      \
        })

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା procfs_file_get_field(pid_t pid, ଅଚଳ ଅକ୍ଷର *name, ଅଚଳ ଅକ୍ଷର *ଚାବି, ଅକ୍ଷର **ret) {
        ଫେରନ୍ତୁ get_proc_field(procfs_file_alloca(pid, name), ଚାବି, ret);
}

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା ProcessCmdlineFlags {
        PROCESS_CMDLINE_COMM_FALLBACK = 1 << 0,
        PROCESS_CMDLINE_USE_LOCALE    = 1 << 1,
        PROCESS_CMDLINE_QUOTE         = 1 << 2,
        PROCESS_CMDLINE_QUOTE_POSIX   = 1 << 3,
} ProcessCmdlineFlags;

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pid_get_comm(pid_t pid, ଅକ୍ଷର **ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidref_get_comm(ଅଚଳ PidRef *pid, ଅକ୍ଷର **ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା pid_get_cmdline(pid_t pid, size_t max_columns, ProcessCmdlineFlags flags, ଅକ୍ଷର **ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidref_get_cmdline(ଅଚଳ PidRef *pid, size_t max_columns, ProcessCmdlineFlags flags, ଅକ୍ଷର **ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା pid_get_cmdline_strv(pid_t pid, ProcessCmdlineFlags flags, ଅକ୍ଷର ***ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidref_get_cmdline_strv(ଅଚଳ PidRef *pid, ProcessCmdlineFlags flags, ଅକ୍ଷର ***ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_process_exe(pid_t pid, ଅକ୍ଷର **ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା pid_get_uid(pid_t pid, uid_t *ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidref_get_uid(ଅଚଳ PidRef *pid, uid_t *ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_process_gid(pid_t pid, gid_t *ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_process_cwd(pid_t pid, ଅକ୍ଷର **ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_process_ମୂଳ(pid_t pid, ଅକ୍ଷର **ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_process_environ(pid_t pid, ଅକ୍ଷର **ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା pid_get_ppid(pid_t pid, pid_t *ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidref_get_ppid(ଅଚଳ PidRef *pidref, pid_t *ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidref_get_ppid_as_pidref(ଅଚଳ PidRef *pidref, PidRef *ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା pid_get_start_time(pid_t pid, usec_t *ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidref_get_start_time(ଅଚଳ PidRef *pid, usec_t *ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_process_umask(pid_t pid, mode_t *ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidref_has_sigcgt(ଅଚଳ PidRef *pidref, ପୂର୍ଣ୍ଣସଂଖ୍ୟା sig);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidref_has_sigblk(ଅଚଳ PidRef *pidref, ପୂର୍ଣ୍ଣସଂଖ୍ୟା sig);

ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool SIGINFO_CODE_IS_DEAD(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) {
        ଫେରନ୍ତୁ IN_SET(code, CLD_EXITED, CLD_KILLED, CLD_DUMPED);
}

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା WaitFlags {
        WAIT_LOG_ABNORMAL             = 1 << 0,
        WAIT_LOG_NON_ZERO_EXIT_STATUS = 1 << 1,

        /* A shortcut for requesting the most complete logging */
        WAIT_LOG = WAIT_LOG_ABNORMAL|WAIT_LOG_NON_ZERO_EXIT_STATUS,
} WaitFlags;

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidref_wait_for_terminate_and_check(ଅଚଳ ଅକ୍ଷର *name, PidRef *pidref, WaitFlags flags);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା kill_and_sigcont(pid_t pid, ପୂର୍ଣ୍ଣସଂଖ୍ୟା sig);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା procfs_get_stat_flags(ଅଚଳ ଅକ୍ଷର *path, unsigned long long *ret);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pid_is_kernel_thread(pid_t pid);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidref_is_kernel_thread(ଅଚଳ PidRef *pid);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା getenv_for_pid(pid_t pid, ଅଚଳ ଅକ୍ଷର *field, ଅକ୍ଷର **ret);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pid_is_alive(pid_t pid);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidref_is_alive(ଅଚଳ PidRef *pidref);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା pid_is_unwaited(pid_t pid);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidref_is_unwaited(PidRef *pidref);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା pid_is_my_child(pid_t pid);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidref_is_my_child(PidRef *pidref);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidref_from_same_ମୂଳ_fs(PidRef *a, PidRef *b);

bool is_main_thread(ଶୂନ୍ୟ);

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

unsigned long personality_from_string(ଅଚଳ ଅକ୍ଷର *s);
ଅଚଳ ଅକ୍ଷର* personality_to_string(unsigned long p);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା safe_personality(unsigned long p);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା opinionated_personality(unsigned long *ret);

DECLARE_STRING_TABLE_LOOKUP(sigchld_code, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
DECLARE_STRING_TABLE_LOOKUP_WITH_FALLBACK(sched_policy, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);

ସ୍ଥିର ଧାଡ଼ିଭିତରେ pid_t PTR_TO_PID(ଅଚଳ ଶୂନ୍ୟ *p) {
        ଫେରନ୍ତୁ (pid_t) ((uintptr_t) p);
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ* PID_TO_PTR(pid_t pid) {
        ଫେରନ୍ତୁ (ଶୂନ୍ୟ*) ((uintptr_t) pid);
}

ଶୂନ୍ୟ valgrind_summary_hack(ଶୂନ୍ୟ);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pid_compare_func(ଅଚଳ pid_t *a, ଅଚଳ pid_t *b);

bool nice_is_valid(ପୂର୍ଣ୍ଣସଂଖ୍ୟା n) _const_;

bool sched_policy_is_valid(ପୂର୍ଣ୍ଣସଂଖ୍ୟା policy) _const_;
bool sched_policy_supported(ପୂର୍ଣ୍ଣସଂଖ୍ୟା policy);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା sched_get_priority_min_safe(ପୂର୍ଣ୍ଣସଂଖ୍ୟା policy);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା sched_get_priority_max_safe(ପୂର୍ଣ୍ଣସଂଖ୍ୟା policy);

#define PID_AUTOMATIC ((pid_t) INT_MIN) /* special value indicating "acquire pid from connection peer" */

ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool pid_is_valid(pid_t p) {
        ଫେରନ୍ତୁ p > 0;
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool pid_is_automatic(pid_t p) {
        ଫେରନ୍ତୁ p == PID_AUTOMATIC;
}

pid_t getpid_cached(ଶୂନ୍ୟ);
ଶୂନ୍ୟ reset_cached_pid(ଶୂନ୍ୟ);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା must_be_ମୂଳ(ଶୂନ୍ୟ);

pid_t clone_with_nested_ଗଦା(ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*fn)(ଶୂନ୍ୟ *), ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags, ଶୂନ୍ୟ *userdata);

/* 💣 Note that FORK_NEW_USERNS, FORK_NEW_MOUNTNS, FORK_NEW_NETNS or FORK_NEW_PIDNS should not be called in threaded
 * programs, because they cause us to use raw_clone() which does not synchronize the glibc malloc() locks,
 * and thus will cause deadlocks if the parent uses threads and the child does memory allocations. Hence: if
 * the parent is threaded these flags may not be used. These flags cannot be used if the parent uses threads
 * or the child uses malloc(). 💣 */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା ForkFlags {
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

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidref_safe_fork_full(
                ଅଚଳ ଅକ୍ଷର *name,
                ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା stdio_fds[3],
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା except_fds[],
                size_t n_except_fds,
                ForkFlags flags,
                PidRef *ret);

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidref_safe_fork(ଅଚଳ ଅକ୍ଷର *name, ForkFlags flags, PidRef *ret) {
        ଫେରନ୍ତୁ pidref_safe_fork_full(name, ଖାଲି, ଖାଲି, 0, flags, ret);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା namespace_fork_full(
                ଅଚଳ ଅକ୍ଷର *outer_name,
                ଅଚଳ ଅକ୍ଷର *inner_name,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା except_fds[],
                size_t n_except_fds,
                ForkFlags flags,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidns_fd,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା mntns_fd,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା netns_fd,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା userns_fd,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୂଳ_fd,
                NamespaceEnterFlags namespace_flags,
                PidRef *ret);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା namespace_fork(
                ଅଚଳ ଅକ୍ଷର *outer_name,
                ଅଚଳ ଅକ୍ଷର *inner_name,
                ForkFlags flags,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidns_fd,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା mntns_fd,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା netns_fd,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା userns_fd,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୂଳ_fd,
                PidRef *ret);

bool oom_score_adjust_is_valid(ପୂର୍ଣ୍ଣସଂଖ୍ୟା oa);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା set_oom_score_adjust(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୂଲ୍ୟ);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_oom_score_adjust(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ret);

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

ପୂର୍ଣ୍ଣସଂଖ୍ୟା setpriority_closest(ପୂର୍ଣ୍ଣସଂଖ୍ୟା priority);

_noreturn_ ଶୂନ୍ୟ freeze(ଶୂନ୍ୟ);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_process_threads(pid_t pid);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_reaper_process(ଶୂନ୍ୟ);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା make_reaper_process(bool b);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା posix_spawn_wrapper(
                ଅଚଳ ଅକ୍ଷର *path,
                ଅକ୍ଷର * ଅଚଳ *argv,
                ଅକ୍ଷର * ଅଚଳ *envp,
                ଅଚଳ ଅକ୍ଷର *cgroup,
                PidRef *ret_pidref);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା proc_dir_open(DIR **ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା proc_dir_read(DIR *d, pid_t *ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା proc_dir_read_pidref(DIR *d, PidRef *ret);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା safe_mlockall(ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags);

_noreturn_ ଶୂନ୍ୟ report_errno_and_exit(ପୂର୍ଣ୍ଣସଂଖ୍ୟା errno_fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା error);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା read_errno(ପୂର୍ଣ୍ଣସଂଖ୍ୟା errno_fd);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା prctl_safe(ପୂର୍ଣ୍ଣସଂଖ୍ୟା op, unsigned long arg2, unsigned long arg3, unsigned long arg4, unsigned long arg5);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା proc_set_comm(ଅଚଳ ଅକ୍ଷର *comm);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା proc_set_nnp(ଶୂନ୍ୟ);
nsec_t proc_get_timerslack(ଶୂନ୍ୟ);
