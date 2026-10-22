/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ <signal.h>

#అన్తర్భావ "fileio.h"
#అన్తర్భావ "format-util.h"
#అన్తర్భావ "forward.h"
#అన్తర్భావ "string-util.h"

#నిర్వచించు procfs_file_alloca(pid, క్షేత్రం)                                  \
        ({                                                              \
                pid_t _pid_ = (pid);                                    \
                స్థిరరాశి అక్షరం *క్షేత్రం_2 = (క్షేత్రం);                          \
                అక్షరం *_r_;                                              \
                ఒకవేళ (_pid_ == 0)                                         \
                        _r_ = strjoina("/proc/self/", క్షేత్రం_2);         \
                లేకపోతే {                                                  \
                        ప్రతిజ్ఞా(_pid_ > 0);                              \
                        _r_ = newa(అక్షరం, STRLEN("/proc/") + DECIMAL_STR_MAX(pid_t) + 1 + అక్షరశ్రేణి_పొడవు(క్షేత్రం_2) + 1); \
                        sprintf(_r_, "/proc/" PID_FMT "/%s", _pid_, క్షేత్రం_2); \
                }                                                       \
                (స్థిరరాశి అక్షరం*) _r_;                                      \
        })

స్థిర అంతఃవరుస పూర్ణాంకం procfs_file_get_క్షేత్రం(pid_t pid, స్థిరరాశి అక్షరం *name, స్థిరరాశి అక్షరం *కీలకం, అక్షరం **ret) {
        తిరిగి get_proc_క్షేత్రం(procfs_file_alloca(pid, name), కీలకం, ret);
}

రకనిర్వచనం గణన ProcessCmdlineFlags {
        PROCESS_CMDLINE_COMM_FALLBACK = 1 << 0,
        PROCESS_CMDLINE_USE_LOCALE    = 1 << 1,
        PROCESS_CMDLINE_QUOTE         = 1 << 2,
        PROCESS_CMDLINE_QUOTE_POSIX   = 1 << 3,
} ProcessCmdlineFlags;

పూర్ణాంకం pid_get_comm(pid_t pid, అక్షరం **ret);
పూర్ణాంకం pidref_get_comm(స్థిరరాశి PidRef *pid, అక్షరం **ret);
పూర్ణాంకం pid_get_cmdline(pid_t pid, size_t max_columns, ProcessCmdlineFlags flags, అక్షరం **ret);
పూర్ణాంకం pidref_get_cmdline(స్థిరరాశి PidRef *pid, size_t max_columns, ProcessCmdlineFlags flags, అక్షరం **ret);
పూర్ణాంకం pid_get_cmdline_strv(pid_t pid, ProcessCmdlineFlags flags, అక్షరం ***ret);
పూర్ణాంకం pidref_get_cmdline_strv(స్థిరరాశి PidRef *pid, ProcessCmdlineFlags flags, అక్షరం ***ret);
పూర్ణాంకం get_process_exe(pid_t pid, అక్షరం **ret);
పూర్ణాంకం pid_get_uid(pid_t pid, uid_t *ret);
పూర్ణాంకం pidref_get_uid(స్థిరరాశి PidRef *pid, uid_t *ret);
పూర్ణాంకం get_process_gid(pid_t pid, gid_t *ret);
పూర్ణాంకం get_process_cwd(pid_t pid, అక్షరం **ret);
పూర్ణాంకం get_process_మూలం(pid_t pid, అక్షరం **ret);
పూర్ణాంకం get_process_environ(pid_t pid, అక్షరం **ret);
పూర్ణాంకం pid_get_ppid(pid_t pid, pid_t *ret);
పూర్ణాంకం pidref_get_ppid(స్థిరరాశి PidRef *pidref, pid_t *ret);
పూర్ణాంకం pidref_get_ppid_as_pidref(స్థిరరాశి PidRef *pidref, PidRef *ret);
పూర్ణాంకం pid_get_ప్రారంభం_సమయం(pid_t pid, usec_t *ret);
పూర్ణాంకం pidref_get_ప్రారంభం_సమయం(స్థిరరాశి PidRef *pid, usec_t *ret);
పూర్ణాంకం get_process_umask(pid_t pid, mode_t *ret);
పూర్ణాంకం pidref_has_sigcgt(స్థిరరాశి PidRef *pidref, పూర్ణాంకం sig);
పూర్ణాంకం pidref_has_sigblk(స్థిరరాశి PidRef *pidref, పూర్ణాంకం sig);

స్థిర అంతఃవరుస bool SIGINFO_సంకేతం_IS_DEAD(పూర్ణాంకం సంకేతం_2) {
        తిరిగి లోపల_సమితి(సంకేతం_2, CLD_EXITED, CLD_KILLED, CLD_DUMPED);
}

రకనిర్వచనం గణన WaitFlags {
        WAIT_LOG_ABNORMAL             = 1 << 0,
        WAIT_LOG_NON_ZERO_EXIT_స్థితి = 1 << 1,

        /* A shortcut for requesting the most complete logging */
        WAIT_LOG = WAIT_LOG_ABNORMAL|WAIT_LOG_NON_ZERO_EXIT_స్థితి,
} WaitFlags;

పూర్ణాంకం pidref_wait_for_terminate_and_check(స్థిరరాశి అక్షరం *name, PidRef *pidref, WaitFlags flags);

పూర్ణాంకం kill_and_sigcont(pid_t pid, పూర్ణాంకం sig);

పూర్ణాంకం procfs_get_stat_flags(స్థిరరాశి అక్షరం *మార్గం, గుర్తులేని దీర్ఘ దీర్ఘ *ret);

పూర్ణాంకం pid_is_kernel_thread(pid_t pid);
పూర్ణాంకం pidref_is_kernel_thread(స్థిరరాశి PidRef *pid);

పూర్ణాంకం getenv_for_pid(pid_t pid, స్థిరరాశి అక్షరం *క్షేత్రం, అక్షరం **ret);

పూర్ణాంకం pid_is_alive(pid_t pid);
పూర్ణాంకం pidref_is_alive(స్థిరరాశి PidRef *pidref);
పూర్ణాంకం pid_is_unwaited(pid_t pid);
పూర్ణాంకం pidref_is_unwaited(PidRef *pidref);
పూర్ణాంకం pid_is_my_బిడ్డ(pid_t pid);
పూర్ణాంకం pidref_is_my_బిడ్డ(PidRef *pidref);
పూర్ణాంకం pidref_from_same_మూలం_fs(PidRef *a, PidRef *b);

bool is_main_thread(శూన్య);

#నిర్వచితం_కాకపోతే PERSONALITY_INVALID
/* personality(2) documents that 0xFFFFFFFFUL is used for querying the
 * current personality, hence let's use that here as error
 * indicator. */
#నిర్వచించు PERSONALITY_INVALID 0xFFFFFFFFUL
#అయితే_ముగింపు

/* The personality() syscall returns a 32-bit value where the top three bytes are reserved for flags that
 * emulate historical or architectural quirks, and only the least significant byte reflects the actual
 * personality we're interested in. */
#నిర్వచించు OPINIONATED_PERSONALITY_ముసుగు 0xFFUL

గుర్తులేని దీర్ఘ personality_from_string(స్థిరరాశి అక్షరం *s);
స్థిరరాశి అక్షరం* personality_గమ్యం_string(గుర్తులేని దీర్ఘ p);

పూర్ణాంకం సురక్షిత_personality(గుర్తులేని దీర్ఘ p);
పూర్ణాంకం opinionated_personality(గుర్తులేని దీర్ఘ *ret);

DECLARE_STRING_పట్టిక_LOOKUP(sigchld_సంకేతం, పూర్ణాంకం);
DECLARE_STRING_పట్టిక_LOOKUP_WITH_FALLBACK(sched_policy, పూర్ణాంకం);

స్థిర అంతఃవరుస pid_t PTR_గమ్యం_PID(స్థిరరాశి శూన్య *p) {
        తిరిగి (pid_t) ((uintptr_t) p);
}

స్థిర అంతఃవరుస శూన్య* PID_గమ్యం_PTR(pid_t pid) {
        తిరిగి (శూన్య*) ((uintptr_t) pid);
}

శూన్య valgrind_summary_hack(శూన్య);

పూర్ణాంకం pid_compare_func(స్థిరరాశి pid_t *a, స్థిరరాశి pid_t *b);

bool nice_is_valid(పూర్ణాంకం n) _const_;

bool sched_policy_is_valid(పూర్ణాంకం policy) _const_;
bool sched_policy_supported(పూర్ణాంకం policy);
పూర్ణాంకం sched_get_ప్రాధాన్యత_min_సురక్షిత(పూర్ణాంకం policy);
పూర్ణాంకం sched_get_ప్రాధాన్యత_max_సురక్షిత(పూర్ణాంకం policy);

#నిర్వచించు PID_AUTOMATIC ((pid_t) INT_MIN) /* special value indicating "acquire pid from connection peer" */

స్థిర అంతఃవరుస bool pid_is_valid(pid_t p) {
        తిరిగి p > 0;
}

స్థిర అంతఃవరుస bool pid_is_automatic(pid_t p) {
        తిరిగి p == PID_AUTOMATIC;
}

pid_t getpid_cached(శూన్య);
శూన్య reset_cached_pid(శూన్య);

పూర్ణాంకం must_be_మూలం(శూన్య);

pid_t clone_with_nested_దొంతర(పూర్ణాంకం (*fn)(శూన్య *), పూర్ణాంకం flags, శూన్య *userdata);

/* 💣 Note that FORK_NEW_USERNS, FORK_NEW_MOUNTNS, FORK_NEW_NETNS or FORK_NEW_PIDNS should not be called in threaded
 * programs, because they cause us to use raw_clone() which does not synchronize the glibc malloc() locks,
 * and thus will cause deadlocks if the parent uses threads and the child does memory allocations. Hence: if
 * the parent is threaded these flags may not be used. These flags cannot be used if the parent uses threads
 * or the child uses malloc(). 💣 */
రకనిర్వచనం గణన ForkFlags {
        FORK_RESET_సంకేతాలు      = 1 <<  0, /* Reset all signal handlers and signal mask */
        FORK_CLOSE_ALL_FDS      = 1 <<  1, /* Close all open file descriptors in the child, except for 0,1,2 */
        FORK_DEATHSIG_SIGTERM   = 1 <<  2, /* Set PR_DEATHSIG in the child to SIGTERM */
        FORK_DEATHSIG_SIGINT    = 1 <<  3, /* Set PR_DEATHSIG in the child to SIGINT */
        FORK_DEATHSIG_SIGKILL   = 1 <<  4, /* Set PR_DEATHSIG in the child to SIGKILL */
        FORK_REARRANGE_STDIO    = 1 <<  5, /* Connect 0,1,2 to specified fds or /dev/null */
        FORK_REOPEN_LOG         = 1 <<  6, /* Reopen log connection */
        FORK_LOG                = 1 <<  7, /* Log above LOG_DEBUG log level about failures */
        FORK_WAIT               = 1 <<  8, /* Wait until child exited */
        FORK_MOUNTNS_SLAVE      = 1 <<  9, /* Make child's mount namespace MS_SLAVE */
        FORK_వ్యక్తిగత_TMP        = 1 << 10, /* Mount new /tmp/ in the child (combine with FORK_NEW_MOUNTNS!) */
        FORK_RLIMIT_NOFILE_సురక్షిత = 1 << 11, /* Set RLIMIT_NOFILE soft limit to 1K for select() compat */
        FORK_STDOUT_గమ్యం_STDERR   = 1 << 12, /* Make stdout a copy of stderr */
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

పూర్ణాంకం pidref_సురక్షిత_fork_నిండిన(
                స్థిరరాశి అక్షరం *name,
                స్థిరరాశి పూర్ణాంకం stdio_fds[3],
                పూర్ణాంకం except_fds[],
                size_t n_except_fds,
                ForkFlags flags,
                PidRef *ret);

స్థిర అంతఃవరుస పూర్ణాంకం pidref_సురక్షిత_fork(స్థిరరాశి అక్షరం *name, ForkFlags flags, PidRef *ret) {
        తిరిగి pidref_సురక్షిత_fork_నిండిన(name, ఖాళీ, ఖాళీ, 0, flags, ret);
}

పూర్ణాంకం namespace_fork_నిండిన(
                స్థిరరాశి అక్షరం *outer_name,
                స్థిరరాశి అక్షరం *inner_name,
                పూర్ణాంకం except_fds[],
                size_t n_except_fds,
                ForkFlags flags,
                పూర్ణాంకం pidns_fd,
                పూర్ణాంకం mntns_fd,
                పూర్ణాంకం netns_fd,
                పూర్ణాంకం userns_fd,
                పూర్ణాంకం మూలం_fd,
                NamespaceEnterFlags namespace_flags,
                PidRef *ret);

పూర్ణాంకం namespace_fork(
                స్థిరరాశి అక్షరం *outer_name,
                స్థిరరాశి అక్షరం *inner_name,
                ForkFlags flags,
                పూర్ణాంకం pidns_fd,
                పూర్ణాంకం mntns_fd,
                పూర్ణాంకం netns_fd,
                పూర్ణాంకం userns_fd,
                పూర్ణాంకం మూలం_fd,
                PidRef *ret);

bool oom_score_adjust_is_valid(పూర్ణాంకం oa);
పూర్ణాంకం సమితి_oom_score_adjust(పూర్ణాంకం విలువ);
పూర్ణాంకం get_oom_score_adjust(పూర్ణాంకం *ret);

/* The highest possibly (theoretic) pid_t value on this architecture. */
#నిర్వచించు PID_T_MAX ((pid_t) INT32_MAX)
/* The maximum number of concurrent processes Linux allows on this architecture, as well as the highest valid PID value
 * the kernel will potentially assign. This reflects a value compiled into the kernel (PID_MAX_LIMIT), and sets the
 * upper boundary on what may be written to the /proc/sys/kernel/pid_max sysctl (but do note that the sysctl is off by
 * 1, since PID 0 can never exist and there can hence only be one process less than the limit would suggest). Since
 * these values are documented in proc(5) we feel quite confident that they are stable enough for the near future at
 * least to define them here too. */
#నిర్వచించు TASKS_MAX 4194303U

assert_cc(TASKS_MAX <= (గుర్తులేని దీర్ఘ) PID_T_MAX);

పూర్ణాంకం setpriority_closest(పూర్ణాంకం priority);

_noreturn_ శూన్య freeze(శూన్య);

పూర్ణాంకం get_process_దారాలు(pid_t pid);

పూర్ణాంకం is_reaper_process(శూన్య);
పూర్ణాంకం make_reaper_process(bool b);

పూర్ణాంకం posix_spawn_wrapper(
                స్థిరరాశి అక్షరం *మార్గం,
                అక్షరం * స్థిరరాశి *argv,
                అక్షరం * స్థిరరాశి *envp,
                స్థిరరాశి అక్షరం *cgroup,
                PidRef *ret_pidref);

పూర్ణాంకం proc_dir_open(DIR **ret);
పూర్ణాంకం proc_dir_read(DIR *d, pid_t *ret);
పూర్ణాంకం proc_dir_read_pidref(DIR *d, PidRef *ret);

పూర్ణాంకం సురక్షిత_mlockall(పూర్ణాంకం flags);

_noreturn_ శూన్య report_errno_and_exit(పూర్ణాంకం errno_fd, పూర్ణాంకం error);
పూర్ణాంకం read_errno(పూర్ణాంకం errno_fd);

పూర్ణాంకం prctl_సురక్షిత(పూర్ణాంకం op, గుర్తులేని దీర్ఘ arg2, గుర్తులేని దీర్ఘ arg3, గుర్తులేని దీర్ఘ arg4, గుర్తులేని దీర్ఘ arg5);

పూర్ణాంకం proc_సమితి_comm(స్థిరరాశి అక్షరం *comm);
పూర్ణాంకం proc_సమితి_nnp(శూన్య);
nsec_t proc_get_timerslack(శూన్య);
