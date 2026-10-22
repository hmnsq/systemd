/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ <signal.h>

#അന്തര്ഭാവ "fileio.h"
#അന്തര്ഭാവ "format-util.h"
#അന്തര്ഭാവ "forward.h"
#അന്തര്ഭാവ "string-util.h"

#define procfs_file_alloca(pid, field)                                  \
        ({                                                              \
                pid_t _pid_ = (pid);                                    \
                സ്ഥിരം അക്ഷരം *_field_ = (field);                          \
                അക്ഷരം *_r_;                                              \
                എങ്കിൽ (_pid_ == 0)                                         \
                        _r_ = strjoina("/proc/self/", _field_);         \
                അല്ലെങ്കിൽ {                                                  \
                        പ്രതിജ്ഞാ(_pid_ > 0);                              \
                        _r_ = newa(അക്ഷരം, STRLEN("/proc/") + DECIMAL_STR_MAX(pid_t) + 1 + strlen(_field_) + 1); \
                        sprintf(_r_, "/proc/" PID_FMT "/%s", _pid_, _field_); \
                }                                                       \
                (സ്ഥിരം അക്ഷരം*) _r_;                                      \
        })

സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ procfs_file_get_field(pid_t pid, സ്ഥിരം അക്ഷരം *name, സ്ഥിരം അക്ഷരം *താക്കോൽ, അക്ഷരം **ret) {
        തിരികെ get_proc_field(procfs_file_alloca(pid, name), താക്കോൽ, ret);
}

തരനിർവചനം എണ്ണൽ ProcessCmdlineFlags {
        PROCESS_CMDLINE_COMM_FALLBACK = 1 << 0,
        PROCESS_CMDLINE_USE_LOCALE    = 1 << 1,
        PROCESS_CMDLINE_QUOTE         = 1 << 2,
        PROCESS_CMDLINE_QUOTE_POSIX   = 1 << 3,
} ProcessCmdlineFlags;

പൂർണ്ണസംഖ്യ pid_get_comm(pid_t pid, അക്ഷരം **ret);
പൂർണ്ണസംഖ്യ pidref_get_comm(സ്ഥിരം PidRef *pid, അക്ഷരം **ret);
പൂർണ്ണസംഖ്യ pid_get_cmdline(pid_t pid, size_t max_columns, ProcessCmdlineFlags flags, അക്ഷരം **ret);
പൂർണ്ണസംഖ്യ pidref_get_cmdline(സ്ഥിരം PidRef *pid, size_t max_columns, ProcessCmdlineFlags flags, അക്ഷരം **ret);
പൂർണ്ണസംഖ്യ pid_get_cmdline_strv(pid_t pid, ProcessCmdlineFlags flags, അക്ഷരം ***ret);
പൂർണ്ണസംഖ്യ pidref_get_cmdline_strv(സ്ഥിരം PidRef *pid, ProcessCmdlineFlags flags, അക്ഷരം ***ret);
പൂർണ്ണസംഖ്യ get_process_exe(pid_t pid, അക്ഷരം **ret);
പൂർണ്ണസംഖ്യ pid_get_uid(pid_t pid, uid_t *ret);
പൂർണ്ണസംഖ്യ pidref_get_uid(സ്ഥിരം PidRef *pid, uid_t *ret);
പൂർണ്ണസംഖ്യ get_process_gid(pid_t pid, gid_t *ret);
പൂർണ്ണസംഖ്യ get_process_cwd(pid_t pid, അക്ഷരം **ret);
പൂർണ്ണസംഖ്യ get_process_വേര്(pid_t pid, അക്ഷരം **ret);
പൂർണ്ണസംഖ്യ get_process_environ(pid_t pid, അക്ഷരം **ret);
പൂർണ്ണസംഖ്യ pid_get_ppid(pid_t pid, pid_t *ret);
പൂർണ്ണസംഖ്യ pidref_get_ppid(സ്ഥിരം PidRef *pidref, pid_t *ret);
പൂർണ്ണസംഖ്യ pidref_get_ppid_as_pidref(സ്ഥിരം PidRef *pidref, PidRef *ret);
പൂർണ്ണസംഖ്യ pid_get_start_time(pid_t pid, usec_t *ret);
പൂർണ്ണസംഖ്യ pidref_get_start_time(സ്ഥിരം PidRef *pid, usec_t *ret);
പൂർണ്ണസംഖ്യ get_process_umask(pid_t pid, mode_t *ret);
പൂർണ്ണസംഖ്യ pidref_has_sigcgt(സ്ഥിരം PidRef *pidref, പൂർണ്ണസംഖ്യ sig);
പൂർണ്ണസംഖ്യ pidref_has_sigblk(സ്ഥിരം PidRef *pidref, പൂർണ്ണസംഖ്യ sig);

സ്ഥിതം വരിയിൽ bool SIGINFO_CODE_IS_DEAD(പൂർണ്ണസംഖ്യ code) {
        തിരികെ IN_SET(code, CLD_EXITED, CLD_KILLED, CLD_DUMPED);
}

തരനിർവചനം എണ്ണൽ WaitFlags {
        WAIT_LOG_ABNORMAL             = 1 << 0,
        WAIT_LOG_NON_ZERO_EXIT_STATUS = 1 << 1,

        /* A shortcut for requesting the most complete logging */
        WAIT_LOG = WAIT_LOG_ABNORMAL|WAIT_LOG_NON_ZERO_EXIT_STATUS,
} WaitFlags;

പൂർണ്ണസംഖ്യ pidref_wait_for_terminate_and_check(സ്ഥിരം അക്ഷരം *name, PidRef *pidref, WaitFlags flags);

പൂർണ്ണസംഖ്യ kill_and_sigcont(pid_t pid, പൂർണ്ണസംഖ്യ sig);

പൂർണ്ണസംഖ്യ procfs_get_stat_flags(സ്ഥിരം അക്ഷരം *path, unsigned long long *ret);

പൂർണ്ണസംഖ്യ pid_is_kernel_thread(pid_t pid);
പൂർണ്ണസംഖ്യ pidref_is_kernel_thread(സ്ഥിരം PidRef *pid);

പൂർണ്ണസംഖ്യ getenv_for_pid(pid_t pid, സ്ഥിരം അക്ഷരം *field, അക്ഷരം **ret);

പൂർണ്ണസംഖ്യ pid_is_alive(pid_t pid);
പൂർണ്ണസംഖ്യ pidref_is_alive(സ്ഥിരം PidRef *pidref);
പൂർണ്ണസംഖ്യ pid_is_unwaited(pid_t pid);
പൂർണ്ണസംഖ്യ pidref_is_unwaited(PidRef *pidref);
പൂർണ്ണസംഖ്യ pid_is_my_child(pid_t pid);
പൂർണ്ണസംഖ്യ pidref_is_my_child(PidRef *pidref);
പൂർണ്ണസംഖ്യ pidref_from_same_വേര്_fs(PidRef *a, PidRef *b);

bool is_main_thread(ശൂന്യം);

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

unsigned long personality_from_string(സ്ഥിരം അക്ഷരം *s);
സ്ഥിരം അക്ഷരം* personality_to_string(unsigned long p);

പൂർണ്ണസംഖ്യ safe_personality(unsigned long p);
പൂർണ്ണസംഖ്യ opinionated_personality(unsigned long *ret);

DECLARE_STRING_TABLE_LOOKUP(sigchld_code, പൂർണ്ണസംഖ്യ);
DECLARE_STRING_TABLE_LOOKUP_WITH_FALLBACK(sched_policy, പൂർണ്ണസംഖ്യ);

സ്ഥിതം വരിയിൽ pid_t PTR_TO_PID(സ്ഥിരം ശൂന്യം *p) {
        തിരികെ (pid_t) ((uintptr_t) p);
}

സ്ഥിതം വരിയിൽ ശൂന്യം* PID_TO_PTR(pid_t pid) {
        തിരികെ (ശൂന്യം*) ((uintptr_t) pid);
}

ശൂന്യം valgrind_summary_hack(ശൂന്യം);

പൂർണ്ണസംഖ്യ pid_compare_func(സ്ഥിരം pid_t *a, സ്ഥിരം pid_t *b);

bool nice_is_valid(പൂർണ്ണസംഖ്യ n) _const_;

bool sched_policy_is_valid(പൂർണ്ണസംഖ്യ policy) _const_;
bool sched_policy_supported(പൂർണ്ണസംഖ്യ policy);
പൂർണ്ണസംഖ്യ sched_get_priority_min_safe(പൂർണ്ണസംഖ്യ policy);
പൂർണ്ണസംഖ്യ sched_get_priority_max_safe(പൂർണ്ണസംഖ്യ policy);

#define PID_AUTOMATIC ((pid_t) INT_MIN) /* special value indicating "acquire pid from connection peer" */

സ്ഥിതം വരിയിൽ bool pid_is_valid(pid_t p) {
        തിരികെ p > 0;
}

സ്ഥിതം വരിയിൽ bool pid_is_automatic(pid_t p) {
        തിരികെ p == PID_AUTOMATIC;
}

pid_t getpid_cached(ശൂന്യം);
ശൂന്യം reset_cached_pid(ശൂന്യം);

പൂർണ്ണസംഖ്യ must_be_വേര്(ശൂന്യം);

pid_t clone_with_nested_അട്ടി(പൂർണ്ണസംഖ്യ (*fn)(ശൂന്യം *), പൂർണ്ണസംഖ്യ flags, ശൂന്യം *userdata);

/* 💣 Note that FORK_NEW_USERNS, FORK_NEW_MOUNTNS, FORK_NEW_NETNS or FORK_NEW_PIDNS should not be called in threaded
 * programs, because they cause us to use raw_clone() which does not synchronize the glibc malloc() locks,
 * and thus will cause deadlocks if the parent uses threads and the child does memory allocations. Hence: if
 * the parent is threaded these flags may not be used. These flags cannot be used if the parent uses threads
 * or the child uses malloc(). 💣 */
തരനിർവചനം എണ്ണൽ ForkFlags {
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

പൂർണ്ണസംഖ്യ pidref_safe_fork_full(
                സ്ഥിരം അക്ഷരം *name,
                സ്ഥിരം പൂർണ്ണസംഖ്യ stdio_fds[3],
                പൂർണ്ണസംഖ്യ except_fds[],
                size_t n_except_fds,
                ForkFlags flags,
                PidRef *ret);

സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ pidref_safe_fork(സ്ഥിരം അക്ഷരം *name, ForkFlags flags, PidRef *ret) {
        തിരികെ pidref_safe_fork_full(name, ശൂന്യ, ശൂന്യ, 0, flags, ret);
}

പൂർണ്ണസംഖ്യ namespace_fork_full(
                സ്ഥിരം അക്ഷരം *outer_name,
                സ്ഥിരം അക്ഷരം *inner_name,
                പൂർണ്ണസംഖ്യ except_fds[],
                size_t n_except_fds,
                ForkFlags flags,
                പൂർണ്ണസംഖ്യ pidns_fd,
                പൂർണ്ണസംഖ്യ mntns_fd,
                പൂർണ്ണസംഖ്യ netns_fd,
                പൂർണ്ണസംഖ്യ userns_fd,
                പൂർണ്ണസംഖ്യ വേര്_fd,
                NamespaceEnterFlags namespace_flags,
                PidRef *ret);

പൂർണ്ണസംഖ്യ namespace_fork(
                സ്ഥിരം അക്ഷരം *outer_name,
                സ്ഥിരം അക്ഷരം *inner_name,
                ForkFlags flags,
                പൂർണ്ണസംഖ്യ pidns_fd,
                പൂർണ്ണസംഖ്യ mntns_fd,
                പൂർണ്ണസംഖ്യ netns_fd,
                പൂർണ്ണസംഖ്യ userns_fd,
                പൂർണ്ണസംഖ്യ വേര്_fd,
                PidRef *ret);

bool oom_score_adjust_is_valid(പൂർണ്ണസംഖ്യ oa);
പൂർണ്ണസംഖ്യ set_oom_score_adjust(പൂർണ്ണസംഖ്യ മൂല്യം);
പൂർണ്ണസംഖ്യ get_oom_score_adjust(പൂർണ്ണസംഖ്യ *ret);

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

പൂർണ്ണസംഖ്യ setpriority_closest(പൂർണ്ണസംഖ്യ priority);

_noreturn_ ശൂന്യം freeze(ശൂന്യം);

പൂർണ്ണസംഖ്യ get_process_threads(pid_t pid);

പൂർണ്ണസംഖ്യ is_reaper_process(ശൂന്യം);
പൂർണ്ണസംഖ്യ make_reaper_process(bool b);

പൂർണ്ണസംഖ്യ posix_spawn_wrapper(
                സ്ഥിരം അക്ഷരം *path,
                അക്ഷരം * സ്ഥിരം *argv,
                അക്ഷരം * സ്ഥിരം *envp,
                സ്ഥിരം അക്ഷരം *cgroup,
                PidRef *ret_pidref);

പൂർണ്ണസംഖ്യ proc_dir_open(DIR **ret);
പൂർണ്ണസംഖ്യ proc_dir_read(DIR *d, pid_t *ret);
പൂർണ്ണസംഖ്യ proc_dir_read_pidref(DIR *d, PidRef *ret);

പൂർണ്ണസംഖ്യ safe_mlockall(പൂർണ്ണസംഖ്യ flags);

_noreturn_ ശൂന്യം report_errno_and_exit(പൂർണ്ണസംഖ്യ errno_fd, പൂർണ്ണസംഖ്യ error);
പൂർണ്ണസംഖ്യ read_errno(പൂർണ്ണസംഖ്യ errno_fd);

പൂർണ്ണസംഖ്യ prctl_safe(പൂർണ്ണസംഖ്യ op, unsigned long arg2, unsigned long arg3, unsigned long arg4, unsigned long arg5);

പൂർണ്ണസംഖ്യ proc_set_comm(സ്ഥിരം അക്ഷരം *comm);
പൂർണ്ണസംഖ്യ proc_set_nnp(ശൂന്യം);
nsec_t proc_get_timerslack(ശൂന്യം);
