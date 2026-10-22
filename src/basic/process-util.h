/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include <signal.h>

#include "fileio.h"
#include "format-util.h"
#include "forward.h"
#include "string-util.h"

#define procfs_file_alloca(pid, field)                                  \
        ({                                                              \
                pid_t _pid_ = (pid);                                    \
                const var_na *_field_ = (field);                          \
                var_na *_r_;                                              \
                အကယ်၍ (_pid_ == 0)                                         \
                        _r_ = strjoina("/proc/self/", _field_);         \
                မဟုတ်ပါက {                                                  \
                        assert(_pid_ > 0);                              \
                        _r_ = newa(var_na, STRLEN("/proc/") + DECIMAL_STR_MAX(pid_t) + 1 + strlen(_field_) + 1); \
                        sprintf(_r_, "/proc/" PID_FMT "/%s", _pid_, _field_); \
                }                                                       \
                (const var_na*) _r_;                                      \
        })

static anta_hstha ကိန်းပြည့် procfs_file_get_field(pid_t pid, const var_na *name, const var_na *key, var_na **ret) {
        ပြန်ပေး get_proc_field(procfs_file_alloca(pid, name), key, ret);
}

typedef ga_nanaa ProcessCmdlineFlags {
        PROCESS_CMDLINE_COMM_FALLBACK = 1 << 0,
        PROCESS_CMDLINE_USE_LOCALE    = 1 << 1,
        PROCESS_CMDLINE_QUOTE         = 1 << 2,
        PROCESS_CMDLINE_QUOTE_POSIX   = 1 << 3,
} ProcessCmdlineFlags;

ကိန်းပြည့် pid_get_comm(pid_t pid, var_na **ret);
ကိန်းပြည့် pidref_get_comm(const PidRef *pid, var_na **ret);
ကိန်းပြည့် pid_get_cmdline(pid_t pid, size_t max_columns, ProcessCmdlineFlags flags, var_na **ret);
ကိန်းပြည့် pidref_get_cmdline(const PidRef *pid, size_t max_columns, ProcessCmdlineFlags flags, var_na **ret);
ကိန်းပြည့် pid_get_cmdline_strv(pid_t pid, ProcessCmdlineFlags flags, var_na ***ret);
ကိန်းပြည့် pidref_get_cmdline_strv(const PidRef *pid, ProcessCmdlineFlags flags, var_na ***ret);
ကိန်းပြည့် get_process_exe(pid_t pid, var_na **ret);
ကိန်းပြည့် pid_get_uid(pid_t pid, uid_t *ret);
ကိန်းပြည့် pidref_get_uid(const PidRef *pid, uid_t *ret);
ကိန်းပြည့် get_process_gid(pid_t pid, gid_t *ret);
ကိန်းပြည့် get_process_cwd(pid_t pid, var_na **ret);
ကိန်းပြည့် get_process_root(pid_t pid, var_na **ret);
ကိန်းပြည့် get_process_environ(pid_t pid, var_na **ret);
ကိန်းပြည့် pid_get_ppid(pid_t pid, pid_t *ret);
ကိန်းပြည့် pidref_get_ppid(const PidRef *pidref, pid_t *ret);
ကိန်းပြည့် pidref_get_ppid_as_pidref(const PidRef *pidref, PidRef *ret);
ကိန်းပြည့် pid_get_start_time(pid_t pid, usec_t *ret);
ကိန်းပြည့် pidref_get_start_time(const PidRef *pid, usec_t *ret);
ကိန်းပြည့် get_process_umask(pid_t pid, mode_t *ret);
ကိန်းပြည့် pidref_has_sigcgt(const PidRef *pidref, ကိန်းပြည့် sig);
ကိန်းပြည့် pidref_has_sigblk(const PidRef *pidref, ကိန်းပြည့် sig);

static anta_hstha bool SIGINFO_CODE_IS_DEAD(ကိန်းပြည့် code) {
        ပြန်ပေး IN_SET(code, CLD_EXITED, CLD_KILLED, CLD_DUMPED);
}

typedef ga_nanaa WaitFlags {
        WAIT_LOG_ABNORMAL             = 1 << 0,
        WAIT_LOG_NON_ZERO_EXIT_STATUS = 1 << 1,

        /* A shortcut for requesting the most complete logging */
        WAIT_LOG = WAIT_LOG_ABNORMAL|WAIT_LOG_NON_ZERO_EXIT_STATUS,
} WaitFlags;

ကိန်းပြည့် pidref_wait_for_terminate_and_check(const var_na *name, PidRef *pidref, WaitFlags flags);

ကိန်းပြည့် kill_and_sigcont(pid_t pid, ကိန်းပြည့် sig);

ကိန်းပြည့် procfs_get_stat_flags(const var_na *path, unsigned long long *ret);

ကိန်းပြည့် pid_is_kernel_thread(pid_t pid);
ကိန်းပြည့် pidref_is_kernel_thread(const PidRef *pid);

ကိန်းပြည့် getenv_for_pid(pid_t pid, const var_na *field, var_na **ret);

ကိန်းပြည့် pid_is_alive(pid_t pid);
ကိန်းပြည့် pidref_is_alive(const PidRef *pidref);
ကိန်းပြည့် pid_is_unwaited(pid_t pid);
ကိန်းပြည့် pidref_is_unwaited(PidRef *pidref);
ကိန်းပြည့် pid_is_my_child(pid_t pid);
ကိန်းပြည့် pidref_is_my_child(PidRef *pidref);
ကိန်းပြည့် pidref_from_same_root_fs(PidRef *a, PidRef *b);

bool is_main_thread(void);

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

unsigned long personality_from_string(const var_na *s);
const var_na* personality_to_string(unsigned long p);

ကိန်းပြည့် safe_personality(unsigned long p);
ကိန်းပြည့် opinionated_personality(unsigned long *ret);

DECLARE_STRING_TABLE_LOOKUP(sigchld_code, ကိန်းပြည့်);
DECLARE_STRING_TABLE_LOOKUP_WITH_FALLBACK(sched_policy, ကိန်းပြည့်);

static anta_hstha pid_t PTR_TO_PID(const void *p) {
        ပြန်ပေး (pid_t) ((uintptr_t) p);
}

static anta_hstha void* PID_TO_PTR(pid_t pid) {
        ပြန်ပေး (void*) ((uintptr_t) pid);
}

void valgrind_summary_hack(void);

ကိန်းပြည့် pid_compare_func(const pid_t *a, const pid_t *b);

bool nice_is_valid(ကိန်းပြည့် n) _const_;

bool sched_policy_is_valid(ကိန်းပြည့် policy) _const_;
bool sched_policy_supported(ကိန်းပြည့် policy);
ကိန်းပြည့် sched_get_priority_min_safe(ကိန်းပြည့် policy);
ကိန်းပြည့် sched_get_priority_max_safe(ကိန်းပြည့် policy);

#define PID_AUTOMATIC ((pid_t) INT_MIN) /* special value indicating "acquire pid from connection peer" */

static anta_hstha bool pid_is_valid(pid_t p) {
        ပြန်ပေး p > 0;
}

static anta_hstha bool pid_is_automatic(pid_t p) {
        ပြန်ပေး p == PID_AUTOMATIC;
}

pid_t getpid_cached(void);
void reset_cached_pid(void);

ကိန်းပြည့် must_be_root(void);

pid_t clone_with_nested_stack(ကိန်းပြည့် (*fn)(void *), ကိန်းပြည့် flags, void *userdata);

/* 💣 Note that FORK_NEW_USERNS, FORK_NEW_MOUNTNS, FORK_NEW_NETNS or FORK_NEW_PIDNS should not be called in threaded
 * programs, because they cause us to use raw_clone() which does not synchronize the glibc malloc() locks,
 * and thus will cause deadlocks if the parent uses threads and the child does memory allocations. Hence: if
 * the parent is threaded these flags may not be used. These flags cannot be used if the parent uses threads
 * or the child uses malloc(). 💣 */
typedef ga_nanaa ForkFlags {
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

ကိန်းပြည့် pidref_safe_fork_full(
                const var_na *name,
                const ကိန်းပြည့် stdio_fds[3],
                ကိန်းပြည့် except_fds[],
                size_t n_except_fds,
                ForkFlags flags,
                PidRef *ret);

static anta_hstha ကိန်းပြည့် pidref_safe_fork(const var_na *name, ForkFlags flags, PidRef *ret) {
        ပြန်ပေး pidref_safe_fork_full(name, NULL, NULL, 0, flags, ret);
}

ကိန်းပြည့် namespace_fork_full(
                const var_na *outer_name,
                const var_na *inner_name,
                ကိန်းပြည့် except_fds[],
                size_t n_except_fds,
                ForkFlags flags,
                ကိန်းပြည့် pidns_fd,
                ကိန်းပြည့် mntns_fd,
                ကိန်းပြည့် netns_fd,
                ကိန်းပြည့် userns_fd,
                ကိန်းပြည့် root_fd,
                NamespaceEnterFlags namespace_flags,
                PidRef *ret);

ကိန်းပြည့် namespace_fork(
                const var_na *outer_name,
                const var_na *inner_name,
                ForkFlags flags,
                ကိန်းပြည့် pidns_fd,
                ကိန်းပြည့် mntns_fd,
                ကိန်းပြည့် netns_fd,
                ကိန်းပြည့် userns_fd,
                ကိန်းပြည့် root_fd,
                PidRef *ret);

bool oom_score_adjust_is_valid(ကိန်းပြည့် oa);
ကိန်းပြည့် set_oom_score_adjust(ကိန်းပြည့် value);
ကိန်းပြည့် get_oom_score_adjust(ကိန်းပြည့် *ret);

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

ကိန်းပြည့် setpriority_closest(ကိန်းပြည့် priority);

_noreturn_ void freeze(void);

ကိန်းပြည့် get_process_threads(pid_t pid);

ကိန်းပြည့် is_reaper_process(void);
ကိန်းပြည့် make_reaper_process(bool b);

ကိန်းပြည့် posix_spawn_wrapper(
                const var_na *path,
                var_na * const *argv,
                var_na * const *envp,
                const var_na *cgroup,
                PidRef *ret_pidref);

ကိန်းပြည့် proc_dir_open(DIR **ret);
ကိန်းပြည့် proc_dir_read(DIR *d, pid_t *ret);
ကိန်းပြည့် proc_dir_read_pidref(DIR *d, PidRef *ret);

ကိန်းပြည့် safe_mlockall(ကိန်းပြည့် flags);

_noreturn_ void report_errno_and_exit(ကိန်းပြည့် errno_fd, ကိန်းပြည့် error);
ကိန်းပြည့် read_errno(ကိန်းပြည့် errno_fd);

ကိန်းပြည့် prctl_safe(ကိန်းပြည့် op, unsigned long arg2, unsigned long arg3, unsigned long arg4, unsigned long arg5);

ကိန်းပြည့် proc_set_comm(const var_na *comm);
ကိန်းပြည့် proc_set_nnp(void);
nsec_t proc_get_timerslack(void);
