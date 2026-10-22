/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश <signal.h>

#समावेश "fileio.h"
#समावेश "format-util.h"
#समावेश "forward.h"
#समावेश "string-util.h"

#घोषणा procfs_file_alloca(pid, field)                                  \
        ({                                                              \
                pid_t _pid_ = (pid);                                    \
                स्थिर अक्षर *_field_ = (field);                          \
                अक्षर *_r_;                                              \
                अगर (_pid_ == 0)                                         \
                        _r_ = strjoina("/proc/self/", _field_);         \
                अन्यथा {                                                  \
                        निश्चित(_pid_ > 0);                              \
                        _r_ = newa(अक्षर, STRLEN("/proc/") + DECIMAL_STR_MAX(pid_t) + 1 + म_माप(_field_) + 1); \
                        प्र_लिखो(_r_, "/proc/" PID_FMT "/%s", _pid_, _field_); \
                }                                                       \
                (स्थिर अक्षर*) _r_;                                      \
        })

अटल अंतरभूत पूर्णांक procfs_file_get_field(pid_t pid, स्थिर अक्षर *name, स्थिर अक्षर *कुंजी, अक्षर **ret) {
        वापस get_proc_field(procfs_file_alloca(pid, name), कुंजी, ret);
}

प्रकार क्रमागत ProcessCmdlineFlags {
        PROCESS_CMDLINE_COMM_FALLBACK = 1 << 0,
        PROCESS_CMDLINE_USE_LOCALE    = 1 << 1,
        PROCESS_CMDLINE_QUOTE         = 1 << 2,
        PROCESS_CMDLINE_QUOTE_POSIX   = 1 << 3,
} ProcessCmdlineFlags;

पूर्णांक pid_get_comm(pid_t pid, अक्षर **ret);
पूर्णांक pidref_get_comm(स्थिर PidRef *pid, अक्षर **ret);
पूर्णांक pid_get_cmdline(pid_t pid, माप_प्रकार max_columns, ProcessCmdlineFlags flags, अक्षर **ret);
पूर्णांक pidref_get_cmdline(स्थिर PidRef *pid, माप_प्रकार max_columns, ProcessCmdlineFlags flags, अक्षर **ret);
पूर्णांक pid_get_cmdline_strv(pid_t pid, ProcessCmdlineFlags flags, अक्षर ***ret);
पूर्णांक pidref_get_cmdline_strv(स्थिर PidRef *pid, ProcessCmdlineFlags flags, अक्षर ***ret);
पूर्णांक get_process_exe(pid_t pid, अक्षर **ret);
पूर्णांक pid_get_uid(pid_t pid, uid_t *ret);
पूर्णांक pidref_get_uid(स्थिर PidRef *pid, uid_t *ret);
पूर्णांक get_process_gid(pid_t pid, gid_t *ret);
पूर्णांक get_process_cwd(pid_t pid, अक्षर **ret);
पूर्णांक get_process_मूल(pid_t pid, अक्षर **ret);
पूर्णांक get_process_environ(pid_t pid, अक्षर **ret);
पूर्णांक pid_get_ppid(pid_t pid, pid_t *ret);
पूर्णांक pidref_get_ppid(स्थिर PidRef *pidref, pid_t *ret);
पूर्णांक pidref_get_ppid_as_pidref(स्थिर PidRef *pidref, PidRef *ret);
पूर्णांक pid_get_start_time(pid_t pid, usec_t *ret);
पूर्णांक pidref_get_start_time(स्थिर PidRef *pid, usec_t *ret);
पूर्णांक get_process_umask(pid_t pid, mode_t *ret);
पूर्णांक pidref_has_sigcgt(स्थिर PidRef *pidref, पूर्णांक sig);
पूर्णांक pidref_has_sigblk(स्थिर PidRef *pidref, पूर्णांक sig);

अटल अंतरभूत bool SIGINFO_CODE_IS_DEAD(पूर्णांक code) {
        वापस IN_SET(code, CLD_EXITED, CLD_KILLED, CLD_DUMPED);
}

प्रकार क्रमागत WaitFlags {
        WAIT_LOG_ABNORMAL             = 1 << 0,
        WAIT_LOG_NON_ZERO_EXIT_STATUS = 1 << 1,

        /* A shortcut for requesting the most complete logging */
        WAIT_LOG = WAIT_LOG_ABNORMAL|WAIT_LOG_NON_ZERO_EXIT_STATUS,
} WaitFlags;

पूर्णांक pidref_wait_for_terminate_and_check(स्थिर अक्षर *name, PidRef *pidref, WaitFlags flags);

पूर्णांक kill_and_sigcont(pid_t pid, पूर्णांक sig);

पूर्णांक procfs_get_stat_flags(स्थिर अक्षर *path, अचिन्हित दीर्घ दीर्घ *ret);

पूर्णांक pid_is_kernel_thread(pid_t pid);
पूर्णांक pidref_is_kernel_thread(स्थिर PidRef *pid);

पूर्णांक getenv_for_pid(pid_t pid, स्थिर अक्षर *field, अक्षर **ret);

पूर्णांक pid_is_alive(pid_t pid);
पूर्णांक pidref_is_alive(स्थिर PidRef *pidref);
पूर्णांक pid_is_unwaited(pid_t pid);
पूर्णांक pidref_is_unwaited(PidRef *pidref);
पूर्णांक pid_is_my_child(pid_t pid);
पूर्णांक pidref_is_my_child(PidRef *pidref);
पूर्णांक pidref_from_same_मूल_fs(PidRef *a, PidRef *b);

bool is_main_thread(व्योम);

#अगर_अघोषित PERSONALITY_INVALID
/* personality(2) documents that 0xFFFFFFFFUL is used for querying the
 * current personality, hence let's use that here as error
 * indicator. */
#घोषणा PERSONALITY_INVALID 0xFFFFFFFFUL
#पूर्ण_अगर

/* The personality() syscall returns a 32-bit value where the top three bytes are reserved for flags that
 * emulate historical or architectural quirks, and only the least significant byte reflects the actual
 * personality we're interested in. */
#घोषणा OPINIONATED_PERSONALITY_MASK 0xFFUL

अचिन्हित दीर्घ personality_from_string(स्थिर अक्षर *s);
स्थिर अक्षर* personality_to_string(अचिन्हित दीर्घ p);

पूर्णांक safe_personality(अचिन्हित दीर्घ p);
पूर्णांक opinionated_personality(अचिन्हित दीर्घ *ret);

DECLARE_STRING_TABLE_LOOKUP(sigchld_code, पूर्णांक);
DECLARE_STRING_TABLE_LOOKUP_WITH_FALLBACK(sched_policy, पूर्णांक);

अटल अंतरभूत pid_t PTR_TO_PID(स्थिर व्योम *p) {
        वापस (pid_t) ((uintptr_t) p);
}

अटल अंतरभूत व्योम* PID_TO_PTR(pid_t pid) {
        वापस (व्योम*) ((uintptr_t) pid);
}

व्योम valgrind_summary_hack(व्योम);

पूर्णांक pid_compare_func(स्थिर pid_t *a, स्थिर pid_t *b);

bool nice_is_valid(पूर्णांक n) _const_;

bool sched_policy_is_valid(पूर्णांक policy) _const_;
bool sched_policy_supported(पूर्णांक policy);
पूर्णांक sched_get_priority_min_safe(पूर्णांक policy);
पूर्णांक sched_get_priority_max_safe(पूर्णांक policy);

#घोषणा PID_AUTOMATIC ((pid_t) पूर्णांक_न्यून) /* special value indicating "acquire pid from connection peer" */

अटल अंतरभूत bool pid_is_valid(pid_t p) {
        वापस p > 0;
}

अटल अंतरभूत bool pid_is_automatic(pid_t p) {
        वापस p == PID_AUTOMATIC;
}

pid_t getpid_cached(व्योम);
व्योम reset_cached_pid(व्योम);

पूर्णांक must_be_मूल(व्योम);

pid_t clone_with_nested_ढेर(पूर्णांक (*fn)(व्योम *), पूर्णांक flags, व्योम *userdata);

/* 💣 Note that FORK_NEW_USERNS, FORK_NEW_MOUNTNS, FORK_NEW_NETNS or FORK_NEW_PIDNS should not be called in threaded
 * programs, because they cause us to use raw_clone() which does not synchronize the glibc malloc() locks,
 * and thus will cause deadlocks if the parent uses threads and the child does memory allocations. Hence: if
 * the parent is threaded these flags may not be used. These flags cannot be used if the parent uses threads
 * or the child uses malloc(). 💣 */
प्रकार क्रमागत ForkFlags {
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

पूर्णांक pidref_safe_fork_full(
                स्थिर अक्षर *name,
                स्थिर पूर्णांक stdio_fds[3],
                पूर्णांक except_fds[],
                माप_प्रकार n_except_fds,
                ForkFlags flags,
                PidRef *ret);

अटल अंतरभूत पूर्णांक pidref_safe_fork(स्थिर अक्षर *name, ForkFlags flags, PidRef *ret) {
        वापस pidref_safe_fork_full(name, शून्य, शून्य, 0, flags, ret);
}

पूर्णांक namespace_fork_full(
                स्थिर अक्षर *outer_name,
                स्थिर अक्षर *inner_name,
                पूर्णांक except_fds[],
                माप_प्रकार n_except_fds,
                ForkFlags flags,
                पूर्णांक pidns_fd,
                पूर्णांक mntns_fd,
                पूर्णांक netns_fd,
                पूर्णांक userns_fd,
                पूर्णांक मूल_fd,
                NamespaceEnterFlags namespace_flags,
                PidRef *ret);

पूर्णांक namespace_fork(
                स्थिर अक्षर *outer_name,
                स्थिर अक्षर *inner_name,
                ForkFlags flags,
                पूर्णांक pidns_fd,
                पूर्णांक mntns_fd,
                पूर्णांक netns_fd,
                पूर्णांक userns_fd,
                पूर्णांक मूल_fd,
                PidRef *ret);

bool oom_score_adjust_is_valid(पूर्णांक oa);
पूर्णांक set_oom_score_adjust(पूर्णांक मूल्य);
पूर्णांक get_oom_score_adjust(पूर्णांक *ret);

/* The highest possibly (theoretic) pid_t value on this architecture. */
#घोषणा PID_T_MAX ((pid_t) INT32_MAX)
/* The maximum number of concurrent processes Linux allows on this architecture, as well as the highest valid PID value
 * the kernel will potentially assign. This reflects a value compiled into the kernel (PID_MAX_LIMIT), and sets the
 * upper boundary on what may be written to the /proc/sys/kernel/pid_max sysctl (but do note that the sysctl is off by
 * 1, since PID 0 can never exist and there can hence only be one process less than the limit would suggest). Since
 * these values are documented in proc(5) we feel quite confident that they are stable enough for the near future at
 * least to define them here too. */
#घोषणा TASKS_MAX 4194303U

assert_cc(TASKS_MAX <= (अचिन्हित दीर्घ) PID_T_MAX);

पूर्णांक setpriority_closest(पूर्णांक priority);

_noreturn_ व्योम freeze(व्योम);

पूर्णांक get_process_threads(pid_t pid);

पूर्णांक is_reaper_process(व्योम);
पूर्णांक make_reaper_process(bool b);

पूर्णांक posix_spawn_wrapper(
                स्थिर अक्षर *path,
                अक्षर * स्थिर *argv,
                अक्षर * स्थिर *envp,
                स्थिर अक्षर *cgroup,
                PidRef *ret_pidref);

पूर्णांक proc_dir_open(सूची **ret);
पूर्णांक proc_dir_read(सूची *d, pid_t *ret);
पूर्णांक proc_dir_read_pidref(सूची *d, PidRef *ret);

पूर्णांक safe_mlockall(पूर्णांक flags);

_noreturn_ व्योम report_errno_and_exit(पूर्णांक errno_fd, पूर्णांक error);
पूर्णांक read_errno(पूर्णांक errno_fd);

पूर्णांक prctl_safe(पूर्णांक op, अचिन्हित दीर्घ arg2, अचिन्हित दीर्घ arg3, अचिन्हित दीर्घ arg4, अचिन्हित दीर्घ arg5);

पूर्णांक proc_set_comm(स्थिर अक्षर *comm);
पूर्णांक proc_set_nnp(व्योम);
nsec_t proc_get_timerslack(व्योम);
