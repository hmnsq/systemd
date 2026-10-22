/* SPDX-License-Identifier: LGPL-2.1-or-later */
#هدايت once

#ضمن <signal.h>

#ضمن "fileio.h"
#ضمن "format-util.h"
#ضمن "forward.h"
#ضمن "string-util.h"

#وصف_ڪريو procfs_file_alloca(pid, ميدان)                                  \
        ({                                                              \
                pid_t _pid_ = (pid);                                    \
                ثابت حرف *ميدان_2 = (ميدان);                          \
                حرف *_r_;                                              \
                إذا (_pid_ == 0)                                         \
                        _r_ = strjoina("/proc/self/", ميدان_2);         \
                وإلا {                                                  \
                        أكد(_pid_ > 0);                              \
                        _r_ = newa(حرف, STRLEN("/proc/") + DECIMAL_STR_MAX(pid_t) + 1 + لڙي_ڊيگهه(ميدان_2) + 1); \
                        sprintf(_r_, "/proc/" PID_FMT "/%s", _pid_, ميدان_2); \
                }                                                       \
                (ثابت حرف*) _r_;                                      \
        })

ساكن مضمن صحيح procfs_file_get_ميدان(pid_t pid, ثابت حرف *name, ثابت حرف *ڪنجي, حرف **ret) {
        أرجع get_proc_ميدان(procfs_file_alloca(pid, name), ڪنجي, ret);
}

تعريفنوع تعداد ProcessCmdlineFlags {
        PROCESS_CMDLINE_COMM_FALLBACK = 1 << 0,
        PROCESS_CMDLINE_USE_LOCALE    = 1 << 1,
        PROCESS_CMDLINE_QUOTE         = 1 << 2,
        PROCESS_CMDLINE_QUOTE_POSIX   = 1 << 3,
} ProcessCmdlineFlags;

صحيح pid_get_comm(pid_t pid, حرف **ret);
صحيح pidref_get_comm(ثابت PidRef *pid, حرف **ret);
صحيح pid_get_cmdline(pid_t pid, size_t max_columns, ProcessCmdlineFlags flags, حرف **ret);
صحيح pidref_get_cmdline(ثابت PidRef *pid, size_t max_columns, ProcessCmdlineFlags flags, حرف **ret);
صحيح pid_get_cmdline_strv(pid_t pid, ProcessCmdlineFlags flags, حرف ***ret);
صحيح pidref_get_cmdline_strv(ثابت PidRef *pid, ProcessCmdlineFlags flags, حرف ***ret);
صحيح get_process_exe(pid_t pid, حرف **ret);
صحيح pid_get_uid(pid_t pid, uid_t *ret);
صحيح pidref_get_uid(ثابت PidRef *pid, uid_t *ret);
صحيح get_process_gid(pid_t pid, gid_t *ret);
صحيح get_process_cwd(pid_t pid, حرف **ret);
صحيح get_process_پاڙ(pid_t pid, حرف **ret);
صحيح get_process_environ(pid_t pid, حرف **ret);
صحيح pid_get_ppid(pid_t pid, pid_t *ret);
صحيح pidref_get_ppid(ثابت PidRef *pidref, pid_t *ret);
صحيح pidref_get_ppid_as_pidref(ثابت PidRef *pidref, PidRef *ret);
صحيح pid_get_شروعات_وقت(pid_t pid, usec_t *ret);
صحيح pidref_get_شروعات_وقت(ثابت PidRef *pid, usec_t *ret);
صحيح get_process_umask(pid_t pid, mode_t *ret);
صحيح pidref_has_sigcgt(ثابت PidRef *pidref, صحيح sig);
صحيح pidref_has_sigblk(ثابت PidRef *pidref, صحيح sig);

ساكن مضمن bool SIGINFO_ڪوڊ_IS_DEAD(صحيح ڪوڊ) {
        أرجع اندريون_سيٽ(ڪوڊ, CLD_EXITED, CLD_KILLED, CLD_DUMPED);
}

تعريفنوع تعداد WaitFlags {
        WAIT_LOG_ABNORMAL             = 1 << 0,
        WAIT_LOG_NON_ZERO_EXIT_حالت = 1 << 1,

        /* A shortcut for requesting the most complete logging */
        WAIT_LOG = WAIT_LOG_ABNORMAL|WAIT_LOG_NON_ZERO_EXIT_حالت,
} WaitFlags;

صحيح pidref_wait_for_terminate_and_check(ثابت حرف *name, PidRef *pidref, WaitFlags flags);

صحيح kill_and_sigcont(pid_t pid, صحيح sig);

صحيح procfs_get_stat_flags(ثابت حرف *رستو, بنانشان ڊگهو ڊگهو *ret);

صحيح pid_is_kernel_thread(pid_t pid);
صحيح pidref_is_kernel_thread(ثابت PidRef *pid);

صحيح getenv_for_pid(pid_t pid, ثابت حرف *ميدان, حرف **ret);

صحيح pid_is_alive(pid_t pid);
صحيح pidref_is_alive(ثابت PidRef *pidref);
صحيح pid_is_unwaited(pid_t pid);
صحيح pidref_is_unwaited(PidRef *pidref);
صحيح pid_is_my_ٻار(pid_t pid);
صحيح pidref_is_my_ٻار(PidRef *pidref);
صحيح pidref_from_same_پاڙ_fs(PidRef *a, PidRef *b);

bool is_main_thread(فراغ);

#جيڪڏهن_وصف_نه_ٿيل PERSONALITY_INVALID
/* personality(2) documents that 0xFFFFFFFFUL is used for querying the
 * current personality, hence let's use that here as error
 * indicator. */
#وصف_ڪريو PERSONALITY_INVALID 0xFFFFFFFFUL
#جيڪڏهن_پورو

/* The personality() syscall returns a 32-bit value where the top three bytes are reserved for flags that
 * emulate historical or architectural quirks, and only the least significant byte reflects the actual
 * personality we're interested in. */
#وصف_ڪريو OPINIONATED_PERSONALITY_نقاب 0xFFUL

بنانشان ڊگهو personality_from_string(ثابت حرف *s);
ثابت حرف* personality_منزل_string(بنانشان ڊگهو p);

صحيح محفوظ_personality(بنانشان ڊگهو p);
صحيح opinionated_personality(بنانشان ڊگهو *ret);

DECLARE_STRING_جدول_LOOKUP(sigchld_ڪوڊ, صحيح);
DECLARE_STRING_جدول_LOOKUP_WITH_FALLBACK(sched_policy, صحيح);

ساكن مضمن pid_t PTR_منزل_PID(ثابت فراغ *p) {
        أرجع (pid_t) ((uintptr_t) p);
}

ساكن مضمن فراغ* PID_منزل_PTR(pid_t pid) {
        أرجع (فراغ*) ((uintptr_t) pid);
}

فراغ valgrind_summary_hack(فراغ);

صحيح pid_compare_func(ثابت pid_t *a, ثابت pid_t *b);

bool nice_is_valid(صحيح n) _const_;

bool sched_policy_is_valid(صحيح policy) _const_;
bool sched_policy_supported(صحيح policy);
صحيح sched_get_ترجيح_min_محفوظ(صحيح policy);
صحيح sched_get_ترجيح_max_محفوظ(صحيح policy);

#وصف_ڪريو PID_AUTOMATIC ((pid_t) INT_MIN) /* special value indicating "acquire pid from connection peer" */

ساكن مضمن bool pid_is_valid(pid_t p) {
        أرجع p > 0;
}

ساكن مضمن bool pid_is_automatic(pid_t p) {
        أرجع p == PID_AUTOMATIC;
}

pid_t getpid_cached(فراغ);
فراغ reset_cached_pid(فراغ);

صحيح must_be_پاڙ(فراغ);

pid_t clone_with_nested_ڍير(صحيح (*fn)(فراغ *), صحيح flags, فراغ *userdata);

/* 💣 Note that FORK_NEW_USERNS, FORK_NEW_MOUNTNS, FORK_NEW_NETNS or FORK_NEW_PIDNS should not be called in threaded
 * programs, because they cause us to use raw_clone() which does not synchronize the glibc malloc() locks,
 * and thus will cause deadlocks if the parent uses threads and the child does memory allocations. Hence: if
 * the parent is threaded these flags may not be used. These flags cannot be used if the parent uses threads
 * or the child uses malloc(). 💣 */
تعريفنوع تعداد ForkFlags {
        FORK_RESET_سگنل      = 1 <<  0, /* Reset all signal handlers and signal mask */
        FORK_CLOSE_ALL_FDS      = 1 <<  1, /* Close all open file descriptors in the child, except for 0,1,2 */
        FORK_DEATHSIG_SIGTERM   = 1 <<  2, /* Set PR_DEATHSIG in the child to SIGTERM */
        FORK_DEATHSIG_SIGINT    = 1 <<  3, /* Set PR_DEATHSIG in the child to SIGINT */
        FORK_DEATHSIG_SIGKILL   = 1 <<  4, /* Set PR_DEATHSIG in the child to SIGKILL */
        FORK_REARRANGE_STDIO    = 1 <<  5, /* Connect 0,1,2 to specified fds or /dev/null */
        FORK_REOPEN_LOG         = 1 <<  6, /* Reopen log connection */
        FORK_LOG                = 1 <<  7, /* Log above LOG_DEBUG log level about failures */
        FORK_WAIT               = 1 <<  8, /* Wait until child exited */
        FORK_MOUNTNS_SLAVE      = 1 <<  9, /* Make child's mount namespace MS_SLAVE */
        FORK_ذاتي_TMP        = 1 << 10, /* Mount new /tmp/ in the child (combine with FORK_NEW_MOUNTNS!) */
        FORK_RLIMIT_NOFILE_محفوظ = 1 << 11, /* Set RLIMIT_NOFILE soft limit to 1K for select() compat */
        FORK_STDOUT_منزل_STDERR   = 1 << 12, /* Make stdout a copy of stderr */
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

صحيح pidref_محفوظ_fork_ڀريل(
                ثابت حرف *name,
                ثابت صحيح stdio_fds[3],
                صحيح except_fds[],
                size_t n_except_fds,
                ForkFlags flags,
                PidRef *ret);

ساكن مضمن صحيح pidref_محفوظ_fork(ثابت حرف *name, ForkFlags flags, PidRef *ret) {
        أرجع pidref_محفوظ_fork_ڀريل(name, عدم, عدم, 0, flags, ret);
}

صحيح namespace_fork_ڀريل(
                ثابت حرف *outer_name,
                ثابت حرف *inner_name,
                صحيح except_fds[],
                size_t n_except_fds,
                ForkFlags flags,
                صحيح pidns_fd,
                صحيح mntns_fd,
                صحيح netns_fd,
                صحيح userns_fd,
                صحيح پاڙ_fd,
                NamespaceEnterFlags namespace_flags,
                PidRef *ret);

صحيح namespace_fork(
                ثابت حرف *outer_name,
                ثابت حرف *inner_name,
                ForkFlags flags,
                صحيح pidns_fd,
                صحيح mntns_fd,
                صحيح netns_fd,
                صحيح userns_fd,
                صحيح پاڙ_fd,
                PidRef *ret);

bool oom_score_adjust_is_valid(صحيح oa);
صحيح سيٽ_oom_score_adjust(صحيح قدر);
صحيح get_oom_score_adjust(صحيح *ret);

/* The highest possibly (theoretic) pid_t value on this architecture. */
#وصف_ڪريو PID_T_MAX ((pid_t) INT32_MAX)
/* The maximum number of concurrent processes Linux allows on this architecture, as well as the highest valid PID value
 * the kernel will potentially assign. This reflects a value compiled into the kernel (PID_MAX_LIMIT), and sets the
 * upper boundary on what may be written to the /proc/sys/kernel/pid_max sysctl (but do note that the sysctl is off by
 * 1, since PID 0 can never exist and there can hence only be one process less than the limit would suggest). Since
 * these values are documented in proc(5) we feel quite confident that they are stable enough for the near future at
 * least to define them here too. */
#وصف_ڪريو TASKS_MAX 4194303U

assert_cc(TASKS_MAX <= (بنانشان ڊگهو) PID_T_MAX);

صحيح setpriority_closest(صحيح priority);

_noreturn_ فراغ freeze(فراغ);

صحيح get_process_ڌاڳا(pid_t pid);

صحيح is_reaper_process(فراغ);
صحيح make_reaper_process(bool b);

صحيح posix_spawn_wrapper(
                ثابت حرف *رستو,
                حرف * ثابت *argv,
                حرف * ثابت *envp,
                ثابت حرف *cgroup,
                PidRef *ret_pidref);

صحيح proc_dir_open(DIR **ret);
صحيح proc_dir_read(DIR *d, pid_t *ret);
صحيح proc_dir_read_pidref(DIR *d, PidRef *ret);

صحيح محفوظ_mlockall(صحيح flags);

_noreturn_ فراغ report_errno_and_exit(صحيح errno_fd, صحيح error);
صحيح read_errno(صحيح errno_fd);

صحيح prctl_محفوظ(صحيح op, بنانشان ڊگهو arg2, بنانشان ڊگهو arg3, بنانشان ڊگهو arg4, بنانشان ڊگهو arg5);

صحيح proc_سيٽ_comm(ثابت حرف *comm);
صحيح proc_سيٽ_nnp(فراغ);
nsec_t proc_get_timerslack(فراغ);
