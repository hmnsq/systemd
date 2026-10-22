/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <linux/oom.h>
#include <pthread.h>
#include <spawn.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/mount.h>
#include <sys/personality.h>
#include <sys/prctl.h>
#include <sys/wait.h>
#include <syslog.h>
#include <threads.h>
#include <unistd.h>
#만약 HAVE_VALGRIND_VALGRIND_H
#include <valgrind/valgrind.h>
#endif

#include "sd-messages.h"

#include "alloc-util.h"
#include "architecture.h"
#include "argv-util.h"
#include "bitfield.h"
#include "capability-util.h"
#include "cgroup-util.h"
#include "dirent-util.h"
#include "dlfcn-util.h"
#include "errno-util.h"
#include "escape.h"
#include "fd-util.h"
#include "fileio.h"
#include "fs-util.h"
#include "io-util.h"
#include "iovec-util.h"
#include "locale-util.h"
#include "log.h"
#include "memory-util.h"
#include "mountpoint-util.h"
#include "namespace-util.h"
#include "nulstr-util.h"
#include "parse-util.h"
#include "path-util.h"
#include "pidfd-util.h"
#include "pidref.h"
#include "process-util.h"
#include "raw-clone.h"
#include "rlimit-util.h"
#include "signal-util.h"
#include "socket-util.h"
#include "stat-util.h"
#include "stdio-util.h"
#include "string-table.h"
#include "string-util.h"
#include "strv.h"
#include "time-util.h"
#include "user-util.h"

/* The kernel limits userspace processes to TASK_COMM_LEN (16 bytes), but allows higher values for its own
 * workers, e.g. "kworker/u9:3-kcryptd/253:0". Let's pick a fixed smallish limit that will work for the kernel.
 */
#define COMM_MAX_LEN 128

static 정수 get_process_state(pid_t pid) {
        _cleanup_free_ char *line = NULL;
        const char *p;
        char state;
        정수 r;

        assert(pid >= 0);

        /* Shortcut: if we are enquired about our own state, we are obviously running */
        만약 (pid == 0 || pid == getpid_cached())
                반환 (unsigned char) 'R';

        p = procfs_file_alloca(pid, "stat");

        r = read_one_line_file(p, &line);
        만약 (r == -ENOENT)
                반환 -ESRCH;
        만약 (r < 0)
                반환 r;

        p = strrchr(line, ')');
        만약 (!p)
                반환 -EIO;

        p++;

        만약 (sscanf(p, " %c", &state) != 1)
                반환 -EIO;

        반환 (unsigned char) state;
}

정수 pid_get_comm(pid_t pid, char **ret) {
        _cleanup_free_ char *escaped = NULL, *comm = NULL;
        정수 r;

        assert(pid >= 0);
        assert(ret);

        만약 (pid == 0 || pid == getpid_cached()) {
                comm = new0(char, TASK_COMM_LEN + 1); /* Must fit in 16 byte according to prctl(2) */
                만약 (!comm)
                        반환 -ENOMEM;

                r = prctl_safe(PR_GET_NAME, (unsigned long) comm, 0, 0, 0);
                만약 (r < 0)
                        반환 r;
        } 아니면 {
                const char *p;

                p = procfs_file_alloca(pid, "comm");

                /* Note that process names of kernel threads can be much longer than TASK_COMM_LEN */
                r = read_one_line_file(p, &comm);
                만약 (r == -ENOENT)
                        반환 -ESRCH;
                만약 (r < 0)
                        반환 r;
        }

        escaped = new(char, COMM_MAX_LEN);
        만약 (!escaped)
                반환 -ENOMEM;

        /* Escape unprintable characters, just in case, but don't grow the string beyond the underlying size */
        cellescape(escaped, COMM_MAX_LEN, comm);

        *ret = TAKE_PTR(escaped);
        반환 0;
}

정수 pidref_get_comm(const PidRef *pid, char **ret) {
        _cleanup_free_ char *comm = NULL;
        정수 r;

        만약 (!pidref_is_set(pid))
                반환 -ESRCH;

        만약 (pidref_is_remote(pid))
                반환 -EREMOTE;

        r = pid_get_comm(pid->pid, &comm);
        만약 (r < 0)
                반환 r;

        r = pidref_verify(pid);
        만약 (r < 0)
                반환 r;

        만약 (ret)
                *ret = TAKE_PTR(comm);
        반환 0;
}

static 정수 pid_get_cmdline_nulstr(
                pid_t pid,
                size_t max_size,
                ProcessCmdlineFlags flags,
                char **ret,
                size_t *ret_size) {

        _cleanup_free_ char *t = NULL;
        const char *p;
        size_t k;
        정수 r;

        /* Retrieves a process' command line as a "sized nulstr", i.e. possibly without the last NUL, but
         * with a specified size.
         *
         * If PROCESS_CMDLINE_COMM_FALLBACK is specified in flags and the process has no command line set
         * (the case for kernel threads), or has a command line that resolves to the empty string, will
         * return the "comm" name of the process instead. This will use at most _SC_ARG_MAX bytes of input
         * data.
         *
         * Returns an error, 0 if output was read but is truncated, 1 otherwise.
         */

        p = procfs_file_alloca(pid, "cmdline");
        r = read_virtual_file(p, max_size, &t, &k); /* Let's assume that each input byte results in >= 1
                                                     * columns of output. We ignore zero-width codepoints. */
        만약 (r == -ENOENT)
                반환 -ESRCH;
        만약 (r < 0)
                반환 r;

        만약 (k == 0) {
                만약 (!(flags & PROCESS_CMDLINE_COMM_FALLBACK))
                        반환 -ENOENT;

                /* Kernel threads have no argv[] */
                _cleanup_free_ char *comm = NULL;

                r = pid_get_comm(pid, &comm);
                만약 (r < 0)
                        반환 r;

                free(t);
                t = strjoin("[", comm, "]");
                만약 (!t)
                        반환 -ENOMEM;

                k = strlen(t);
                r = k <= max_size;
                만약 (r == 0) /* truncation */
                        t[max_size] = '\0';
        }

        만약 (ret)
                *ret = TAKE_PTR(t);
        만약 (ret_size)
                *ret_size = k;

        반환 r;
}

정수 pid_get_cmdline(pid_t pid, size_t max_columns, ProcessCmdlineFlags flags, char **ret) {
        _cleanup_free_ char *t = NULL;
        size_t k;
        char *ans;

        assert(pid >= 0);
        assert(ret);

        /* Retrieve and format a command line. See above for discussion of retrieval options.
         *
         * There are two main formatting modes:
         *
         * - when PROCESS_CMDLINE_QUOTE is specified, output is quoted in C/Python style. If no shell special
         *   characters are present, this output can be copy-pasted into the terminal to execute. UTF-8
         *   output is assumed.
         *
         * - otherwise, a compact non-roundtrippable form is returned. Non-UTF8 bytes are replaced by �. The
         *   returned string is of the specified console width at most, abbreviated with an ellipsis.
         *
         * Returns -ESRCH if the process doesn't exist, and -ENOENT if the process has no command line (and
         * PROCESS_CMDLINE_COMM_FALLBACK is not specified). Returns 0 and sets *line otherwise. */

        정수 full = pid_get_cmdline_nulstr(pid, max_columns, flags, &t, &k);
        만약 (full < 0)
                반환 full;

        만약 (flags & (PROCESS_CMDLINE_QUOTE | PROCESS_CMDLINE_QUOTE_POSIX)) {
                ShellEscapeFlags shflags = SHELL_ESCAPE_EMPTY |
                        FLAGS_SET(flags, PROCESS_CMDLINE_QUOTE_POSIX) * SHELL_ESCAPE_POSIX;

                assert(!(flags & PROCESS_CMDLINE_USE_LOCALE));

                _cleanup_strv_free_ char **args = NULL;

                /* Drop trailing NULs, otherwise strv_parse_nulstr() adds additional empty strings at the end.
                 * See also issue #21186. */
                args = strv_parse_nulstr_full(t, k, /* drop_trailing_nuls= */ true);
                만약 (!args)
                        반환 -ENOMEM;

                ans = quote_command_line(args, shflags);
                만약 (!ans)
                        반환 -ENOMEM;
        } 아니면 {
                /* Arguments are separated by NULs. Let's replace those with spaces. */
                위해 (size_t i = 0; i < k - 1; i++)
                        만약 (t[i] == '\0')
                                t[i] = ' ';

                delete_trailing_chars(t, WHITESPACE);

                bool eight_bit = (flags & PROCESS_CMDLINE_USE_LOCALE) && !is_locale_utf8();

                ans = escape_non_printable_full(t, max_columns,
                                                eight_bit * XESCAPE_8_BIT | !full * XESCAPE_FORCE_ELLIPSIS);
                만약 (!ans)
                        반환 -ENOMEM;

                ans = str_realloc(ans);
        }

        *ret = ans;
        반환 0;
}

정수 pidref_get_cmdline(const PidRef *pid, size_t max_columns, ProcessCmdlineFlags flags, char **ret) {
        _cleanup_free_ char *s = NULL;
        정수 r;

        만약 (!pidref_is_set(pid))
                반환 -ESRCH;

        만약 (pidref_is_remote(pid))
                반환 -EREMOTE;

        r = pid_get_cmdline(pid->pid, max_columns, flags, &s);
        만약 (r < 0)
                반환 r;

        r = pidref_verify(pid);
        만약 (r < 0)
                반환 r;

        만약 (ret)
                *ret = TAKE_PTR(s);
        반환 0;
}

정수 pid_get_cmdline_strv(pid_t pid, ProcessCmdlineFlags flags, char ***ret) {
        _cleanup_free_ char *t = NULL;
        char **args;
        size_t k;
        정수 r;

        assert(pid >= 0);
        assert((flags & ~PROCESS_CMDLINE_COMM_FALLBACK) == 0);
        assert(ret);

        r = pid_get_cmdline_nulstr(pid, SIZE_MAX, flags, &t, &k);
        만약 (r < 0)
                반환 r;

        args = strv_parse_nulstr_full(t, k, /* drop_trailing_nuls= */ true);
        만약 (!args)
                반환 -ENOMEM;

        *ret = args;
        반환 0;
}

정수 pidref_get_cmdline_strv(const PidRef *pid, ProcessCmdlineFlags flags, char ***ret) {
        _cleanup_strv_free_ char **args = NULL;
        정수 r;

        만약 (!pidref_is_set(pid))
                반환 -ESRCH;

        만약 (pidref_is_remote(pid))
                반환 -EREMOTE;

        r = pid_get_cmdline_strv(pid->pid, flags, &args);
        만약 (r < 0)
                반환 r;

        r = pidref_verify(pid);
        만약 (r < 0)
                반환 r;

        만약 (ret)
                *ret = TAKE_PTR(args);

        반환 0;
}

정수 procfs_get_stat_flags(const char *path, unsigned long long *ret) {
        정수 r;

        assert(path);
        assert(ret);

        /* Read the 'flags' field (9th field, task->flags) from the given /proc/[pid]/stat or
         * /proc/[pid]/task/[tid]/stat file. */

        _cleanup_free_ char *line = NULL;
        r = read_one_line_file(path, &line);
        만약 (r == -ENOENT)
                반환 -ESRCH;
        만약 (r < 0)
                반환 r;

        /* Skip past the comm field */
        char *q = strrchr(line, ')');
        만약 (!q)
                반환 -EINVAL;
        q++;

        /* Skip 6 fields to reach the flags field */
        위해 (size_t i = 0; i < 6; i++) {
                size_t l = strspn(q, WHITESPACE);
                만약 (l < 1)
                        반환 -EINVAL;
                q += l;

                l = strcspn(q, WHITESPACE);
                만약 (l < 1)
                        반환 -EINVAL;
                q += l;
        }

        /* Skip preceding whitespace */
        size_t l = strspn(q, WHITESPACE);
        만약 (l < 1)
                반환 -EINVAL;
        q += l;

        /* Truncate the rest */
        l = strcspn(q, WHITESPACE);
        만약 (l < 1)
                반환 -EINVAL;
        q[l] = 0;

        반환 safe_atollu(q, ret);
}

정수 pid_is_kernel_thread(pid_t pid) {
        정수 r;

        만약 (IN_SET(pid, 0, 1) || pid == getpid_cached()) /* pid 1, and we ourselves certainly aren't a kernel thread */
                반환 0;
        만약 (!pid_is_valid(pid))
                반환 -EINVAL;

        const char *p = procfs_file_alloca(pid, "stat");

        unsigned long long flags;
        r = procfs_get_stat_flags(p, &flags);
        만약 (r < 0)
                반환 r;

        반환 !!(flags & PF_KTHREAD);
}

정수 pidref_is_kernel_thread(const PidRef *pid) {
        정수 result, r;

        만약 (!pidref_is_set(pid))
                반환 -ESRCH;

        만약 (pidref_is_remote(pid))
                반환 -EREMOTE;

        result = pid_is_kernel_thread(pid->pid);
        만약 (result < 0)
                반환 result;

        r = pidref_verify(pid); /* Verify that the PID wasn't reused since */
        만약 (r < 0)
                반환 r;

        반환 result;
}

static 정수 get_process_link_contents(pid_t pid, const char *proc_file, char **ret) {
        const char *p;
        정수 r;

        assert(proc_file);

        p = procfs_file_alloca(pid, proc_file);

        r = readlink_malloc(p, ret);
        반환 (r == -ENOENT && proc_mounted() > 0) ? -ESRCH : r;
}

정수 get_process_exe(pid_t pid, char **ret) {
        char *d;
        정수 r;

        assert(pid >= 0);

        r = get_process_link_contents(pid, "exe", ret);
        만약 (r < 0)
                반환 r;

        만약 (ret) {
                d = endswith(*ret, " (deleted)");
                만약 (d)
                        *d = '\0';
        }

        반환 0;
}

정수 pid_get_uid(pid_t pid, uid_t *ret) {
        정수 r;

        assert(pid >= 0);
        assert(ret);

        만약 (pid == 0 || pid == getpid_cached()) {
                *ret = getuid();
                반환 0;
        }

        _cleanup_free_ char *v = NULL;
        r = procfs_file_get_field(pid, "status", "Uid", &v);
        만약 (r == -ENOENT)
                반환 -ESRCH;
        만약 (r < 0)
                반환 r;

        반환 parse_uid(v, ret);
}

정수 pidref_get_uid(const PidRef *pid, uid_t *ret) {
        정수 r;

        만약 (!pidref_is_set(pid))
                반환 -ESRCH;

        만약 (pidref_is_remote(pid))
                반환 -EREMOTE;

        만약 (pid->fd >= 0) {
                r = pidfd_get_uid(pid->fd, ret);
                만약 (!ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        반환 r;
        }

        uid_t uid;
        r = pid_get_uid(pid->pid, &uid);
        만약 (r < 0)
                반환 r;

        r = pidref_verify(pid);
        만약 (r < 0)
                반환 r;

        만약 (ret)
                *ret = uid;
        반환 0;
}

정수 get_process_gid(pid_t pid, gid_t *ret) {
        정수 r;

        assert(pid >= 0);
        assert(ret);

        만약 (pid == 0 || pid == getpid_cached()) {
                *ret = getgid();
                반환 0;
        }

        _cleanup_free_ char *v = NULL;
        r = procfs_file_get_field(pid, "status", "Gid", &v);
        만약 (r == -ENOENT)
                반환 -ESRCH;
        만약 (r < 0)
                반환 r;

        반환 parse_gid(v, ret);
}

정수 get_process_cwd(pid_t pid, char **ret) {
        assert(pid >= 0);

        만약 (pid == 0 || pid == getpid_cached())
                반환 safe_getcwd(ret);

        반환 get_process_link_contents(pid, "cwd", ret);
}

정수 get_process_root(pid_t pid, char **ret) {
        assert(pid >= 0);
        반환 get_process_link_contents(pid, "root", ret);
}

#define ENVIRONMENT_BLOCK_MAX (5U*1024U*1024U)

정수 get_process_environ(pid_t pid, char **ret) {
        _cleanup_fclose_ FILE *f = NULL;
        _cleanup_free_ char *outcome = NULL;
        size_t sz = 0;
        const char *p;
        정수 r;

        assert(pid >= 0);
        assert(ret);

        p = procfs_file_alloca(pid, "environ");

        r = fopen_unlocked(p, "re", &f);
        만약 (r == -ENOENT)
                반환 -ESRCH;
        만약 (r < 0)
                반환 r;

        위해 (;;) {
                char c;

                만약 (sz >= ENVIRONMENT_BLOCK_MAX)
                        반환 -ENOBUFS;

                만약 (!GREEDY_REALLOC(outcome, sz + 5))
                        반환 -ENOMEM;

                r = safe_fgetc(f, &c);
                만약 (r < 0)
                        반환 r;
                만약 (r == 0)
                        break;

                만약 (c == '\0')
                        outcome[sz++] = '\n';
                아니면
                        sz += cescape_char(c, outcome + sz);
        }

        outcome[sz] = '\0';
        *ret = TAKE_PTR(outcome);

        반환 0;
}

정수 pid_get_ppid(pid_t pid, pid_t *ret) {
        _cleanup_free_ char *line = NULL;
        unsigned long ppid;
        const char *p;
        정수 r;

        assert(pid >= 0);

        만약 (pid == 0)
                pid = getpid_cached();
        만약 (pid == 1) /* PID 1 has no parent, shortcut this case */
                반환 -EADDRNOTAVAIL;

        만약 (pid == getpid_cached()) {
                만약 (ret)
                        *ret = getppid();
                반환 0;
        }

        p = procfs_file_alloca(pid, "stat");
        r = read_one_line_file(p, &line);
        만약 (r == -ENOENT)
                반환 -ESRCH;
        만약 (r < 0)
                반환 r;

        /* Let's skip the pid and comm fields. The latter is enclosed in () but does not escape any () in its
         * value, so let's skip over it manually */

        p = strrchr(line, ')');
        만약 (!p)
                반환 -EIO;
        p++;

        만약 (sscanf(p, " "
                   "%*c "  /* state */
                   "%lu ", /* ppid */
                   &ppid) != 1)
                반환 -EIO;

        /* If ppid is zero the process has no parent. Which might be the case for PID 1 (caught above)
         * but also for processes originating in other namespaces that are inserted into a pidns.
         * Return a recognizable error in this case. */
        만약 (ppid == 0)
                반환 -EADDRNOTAVAIL;

        만약 ((pid_t) ppid < 0 || (unsigned long) (pid_t) ppid != ppid)
                반환 -ERANGE;

        만약 (ret)
                *ret = (pid_t) ppid;

        반환 0;
}

정수 pidref_get_ppid(const PidRef *pidref, pid_t *ret) {
        정수 r;

        만약 (!pidref_is_set(pidref))
                반환 -ESRCH;

        만약 (pidref_is_remote(pidref))
                반환 -EREMOTE;

        만약 (pidref->fd >= 0) {
                r = pidfd_get_ppid(pidref->fd, ret);
                만약 (!ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        반환 r;
        }

        pid_t ppid;
        r = pid_get_ppid(pidref->pid, ret ? &ppid : NULL);
        만약 (r < 0)
                반환 r;

        r = pidref_verify(pidref);
        만약 (r < 0)
                반환 r;

        만약 (ret)
                *ret = ppid;
        반환 0;
}

정수 pidref_get_ppid_as_pidref(const PidRef *pidref, PidRef *ret) {
        pid_t ppid;
        정수 r;

        POINTER_MAY_BE_NULL(pidref);
        assert(ret);

        r = pidref_get_ppid(pidref, &ppid);
        만약 (r < 0)
                반환 r;

        위해 (unsigned attempt = 0; attempt < 16; attempt++) {
                _cleanup_(pidref_done) PidRef parent = PIDREF_NULL;

                r = pidref_set_pid(&parent, ppid);
                만약 (r < 0)
                        반환 r;

                /* If we have a pidfd of the original PID, let's verify that the process we acquired really
                 * is the parent still */
                만약 (pidref->fd >= 0) {
                        r = pidref_get_ppid(pidref, &ppid);
                        만약 (r < 0)
                                반환 r;

                        /* Did the PPID change since we queried it? if so we might have pinned the wrong
                         * process, if its PID got reused by now. Let's try again */
                        만약 (parent.pid != ppid)
                                continue;
                }

                *ret = TAKE_PIDREF(parent);
                반환 0;
        }

        /* Give up after 16 tries */
        반환 -ENOTRECOVERABLE;
}

정수 pid_get_start_time(pid_t pid, usec_t *ret) {
        _cleanup_free_ char *line = NULL;
        const char *p;
        정수 r;

        assert(pid >= 0);

        p = procfs_file_alloca(pid, "stat");
        r = read_one_line_file(p, &line);
        만약 (r == -ENOENT)
                반환 -ESRCH;
        만약 (r < 0)
                반환 r;

        /* Let's skip the pid and comm fields. The latter is enclosed in () but does not escape any () in its
         * value, so let's skip over it manually */

        p = strrchr(line, ')');
        만약 (!p)
                반환 -EIO;
        p++;

        unsigned long llu;

        만약 (sscanf(p, " "
                   "%*c " /* state */
                   "%*u " /* ppid */
                   "%*u " /* pgrp */
                   "%*u " /* session */
                   "%*u " /* tty_nr */
                   "%*u " /* tpgid */
                   "%*u " /* flags */
                   "%*u " /* minflt */
                   "%*u " /* cminflt */
                   "%*u " /* majflt */
                   "%*u " /* cmajflt */
                   "%*u " /* utime */
                   "%*u " /* stime */
                   "%*u " /* cutime */
                   "%*u " /* cstime */
                   "%*i " /* priority */
                   "%*i " /* nice */
                   "%*u " /* num_threads */
                   "%*u " /* itrealvalue */
                   "%lu ", /* starttime */
                   &llu) != 1)
                반환 -EIO;

        만약 (ret)
                *ret = jiffies_to_usec(llu); /* CLOCK_BOOTTIME */

        반환 0;
}

정수 pidref_get_start_time(const PidRef *pid, usec_t *ret) {
        usec_t t;
        정수 r;

        만약 (!pidref_is_set(pid))
                반환 -ESRCH;

        만약 (pidref_is_remote(pid))
                반환 -EREMOTE;

        r = pid_get_start_time(pid->pid, ret ? &t : NULL);
        만약 (r < 0)
                반환 r;

        r = pidref_verify(pid);
        만약 (r < 0)
                반환 r;

        만약 (ret)
                *ret = t;

        반환 0;
}

정수 get_process_umask(pid_t pid, mode_t *ret) {
        _cleanup_free_ char *m = NULL;
        정수 r;

        assert(pid >= 0);
        assert(ret);

        r = procfs_file_get_field(pid, "status", "Umask", &m);
        만약 (r == -ENOENT)
                반환 -ESRCH;
        만약 (r < 0)
                반환 r;

        반환 parse_mode(m, ret);
}

static 정수 pid_get_sigmask(pid_t pid, const char *field_name, uint64_t *ret) {
        _cleanup_free_ char *field = NULL;
        정수 r;

        assert(pid >= 0);
        assert(field_name);
        assert(ret);

        r = procfs_file_get_field(pid, "status", field_name, &field);
        만약 (r == -ENOENT)
                반환 -ESRCH;
        만약 (r < 0)
                반환 r;

        반환 safe_atou64_full(field, 16, ret);
}

static 정수 pidref_has_sigmask(const PidRef *pidref, 정수 sig, const char *field_name) {
        uint64_t mask;
        정수 r;

        만약 (!pidref_is_set(pidref))
                반환 -ESRCH;
        만약 (pidref_is_remote(pidref))
                반환 -EREMOTE;
        만약 (!SIGNAL_VALID(sig))
                반환 -EINVAL;

        r = pid_get_sigmask(pidref->pid, field_name, &mask);
        만약 (r < 0)
                반환 r;

        r = pidref_verify(pidref);
        만약 (r < 0)
                반환 r;

        반환 BIT_SET(mask, sig - 1);
}

정수 pidref_has_sigcgt(const PidRef *pidref, 정수 sig) {
        반환 pidref_has_sigmask(pidref, sig, "SigCgt");
}

정수 pidref_has_sigblk(const PidRef *pidref, 정수 sig) {
        반환 pidref_has_sigmask(pidref, sig, "SigBlk");
}

/*
 * Return values:
 * < 0 : pidref_wait_for_terminate() failed to get the state of the
 *       process, the process was terminated by a signal, or
 *       failed for an unknown reason.
 * >=0 : The process terminated normally, and its exit code is
 *       returned.
 *
 * That is, success is indicated by a return value of zero, and an
 * error is indicated by a non-zero value.
 *
 * A warning is emitted if the process terminates abnormally,
 * and also if it returns non-zero unless check_exit_code is true.
 */
정수 pidref_wait_for_terminate_and_check(const char *name, PidRef *pidref, WaitFlags flags) {
        정수 r;

        만약 (!pidref_is_set(pidref))
                반환 -ESRCH;
        만약 (pidref_is_remote(pidref))
                반환 -EREMOTE;
        만약 (pidref->pid == 1 || pidref_is_self(pidref))
                반환 -ECHILD;

        _cleanup_free_ char *buffer = NULL;
        만약 (!name) {
                r = pidref_get_comm(pidref, &buffer);
                만약 (r < 0)
                        log_debug_errno(r, "Failed to acquire process name of " PID_FMT ", ignoring: %m", pidref->pid);
                아니면
                        name = buffer;
        }

        정수 prio = flags & WAIT_LOG_ABNORMAL ? LOG_ERR : LOG_DEBUG;

        siginfo_t status;
        r = pidref_wait_for_terminate(pidref, &status);
        만약 (r < 0)
                반환 log_full_errno(prio, r, "Failed to wait for '%s': %m", strna(name));

        만약 (status.si_code == CLD_EXITED) {
                만약 (status.si_status != EXIT_SUCCESS)
                        log_full(flags & WAIT_LOG_NON_ZERO_EXIT_STATUS ? LOG_ERR : LOG_DEBUG,
                                 "'%s' failed with exit status %i.", strna(name), status.si_status);
                아니면
                        log_debug("'%s' succeeded.", name);

                반환 status.si_status;

        } 아니면 만약 (IN_SET(status.si_code, CLD_KILLED, CLD_DUMPED))
                반환 log_full_errno(prio, SYNTHETIC_ERRNO(EPROTO),
                                      "'%s' terminated by signal %s.", strna(name), signal_to_string(status.si_status));

        반환 log_full_errno(prio, SYNTHETIC_ERRNO(EPROTO),
                              "'%s' failed due to unknown reason.", strna(name));
}

정수 kill_and_sigcont(pid_t pid, 정수 sig) {
        정수 r;

        r = RET_NERRNO(kill(pid, sig));

        /* If this worked, also send SIGCONT, unless we already just sent a SIGCONT, or SIGKILL was sent which isn't
         * affected by a process being suspended anyway. */
        만약 (r >= 0 && !IN_SET(sig, SIGCONT, SIGKILL))
                (void) kill(pid, SIGCONT);

        반환 r;
}

정수 getenv_for_pid(pid_t pid, const char *field, char **ret) {
        _cleanup_fclose_ FILE *f = NULL;
        const char *path;
        size_t sum = 0;
        정수 r;

        assert(pid >= 0);
        assert(field);
        assert(ret);

        만약 (pid == 0 || pid == getpid_cached())
                반환 strdup_to_full(ret, getenv(field));

        만약 (!pid_is_valid(pid))
                반환 -EINVAL;

        path = procfs_file_alloca(pid, "environ");

        r = fopen_unlocked(path, "re", &f);
        만약 (r == -ENOENT)
                반환 -ESRCH;
        만약 (r < 0)
                반환 r;

        위해 (;;) {
                _cleanup_free_ char *line = NULL;
                const char *match;

                만약 (sum > ENVIRONMENT_BLOCK_MAX) /* Give up searching eventually */
                        반환 -ENOBUFS;

                r = read_nul_string(f, LONG_LINE_MAX, &line);
                만약 (r < 0)
                        반환 r;
                만약 (r == 0)  /* EOF */
                        break;

                sum += r;

                match = startswith(line, field);
                만약 (match && *match == '=')
                        반환 strdup_to_full(ret, match + 1);
        }

        *ret = NULL;
        반환 0;
}

정수 pidref_is_my_child(PidRef *pid) {
        정수 r;

        만약 (!pidref_is_set(pid))
                반환 -ESRCH;

        만약 (pidref_is_remote(pid))
                반환 -EREMOTE;

        만약 (pid->pid == 1 || pidref_is_self(pid))
                반환 false;

        pid_t ppid;
        r = pidref_get_ppid(pid, &ppid);
        만약 (r == -EADDRNOTAVAIL) /* if this process is outside of our pidns, it is definitely not our child */
                반환 false;
        만약 (r < 0)
                반환 r;

        반환 ppid == getpid_cached();
}

정수 pid_is_my_child(pid_t pid) {

        만약 (pid == 0)
                반환 false;

        반환 pidref_is_my_child(&PIDREF_MAKE_FROM_PID(pid));
}

정수 pidref_is_unwaited(PidRef *pid) {
        정수 r;

        /* Checks whether a PID is still valid at all, including a zombie */

        만약 (!pidref_is_set(pid))
                반환 -ESRCH;

        만약 (pidref_is_remote(pid))
                반환 -EREMOTE;

        만약 (pid->pid == 1 || pidref_is_self(pid))
                반환 true;

        r = pidref_kill(pid, 0);
        만약 (r == -ESRCH)
                반환 false;
        만약 (r < 0)
                반환 r;

        반환 true;
}

정수 pid_is_unwaited(pid_t pid) {

        만약 (pid == 0)
                반환 true;

        반환 pidref_is_unwaited(&PIDREF_MAKE_FROM_PID(pid));
}

정수 pid_is_alive(pid_t pid) {
        정수 r;

        /* Checks whether a PID is still valid and not a zombie */

        만약 (pid < 0)
                반환 -ESRCH;

        만약 (pid <= 1) /* If we or PID 1 would be a zombie, this code would not be running */
                반환 true;

        만약 (pid == getpid_cached())
                반환 true;

        r = get_process_state(pid);
        만약 (r == -ESRCH)
                반환 false;
        만약 (r < 0)
                반환 r;

        반환 r != 'Z';
}

정수 pidref_is_alive(const PidRef *pidref) {
        정수 r, result;

        만약 (!pidref_is_set(pidref))
                반환 -ESRCH;

        만약 (pidref_is_remote(pidref))
                반환 -EREMOTE;

        result = pid_is_alive(pidref->pid);
        만약 (result < 0) {
                assert(result != -ESRCH);
                반환 result;
        }

        r = pidref_verify(pidref);
        만약 (r == -ESRCH)
                반환 false;
        만약 (r < 0)
                반환 r;

        반환 result;
}

정수 pidref_from_same_root_fs(PidRef *a, PidRef *b) {
        _cleanup_(pidref_done) PidRef self = PIDREF_NULL;
        정수 r;

        /* Checks if the two specified processes have the same root fs. Either can be specified as NULL in
         * which case we'll check against ourselves. */

        만약 (!a || !b) {
                r = pidref_set_self(&self);
                만약 (r < 0)
                        반환 r;
                만약 (!a)
                        a = &self;
                만약 (!b)
                        b = &self;
        }

        만약 (!pidref_is_set(a) || !pidref_is_set(b))
                반환 -ESRCH;

        /* If one of the two processes have the same root they cannot have the same root fs, but if both of
         * them do we don't know */
        만약 (pidref_is_remote(a) && pidref_is_remote(b))
                반환 -EREMOTE;
        만약 (pidref_is_remote(a) || pidref_is_remote(b))
                반환 false;

        만약 (pidref_equal(a, b))
                반환 true;

        const char *roota = procfs_file_alloca(a->pid, "root");
        const char *rootb = procfs_file_alloca(b->pid, "root");

        정수 result = inode_same(roota, rootb, 0);
        만약 (result == -ENOENT)
                반환 proc_mounted() == 0 ? -ENOSYS : -ESRCH;
        만약 (result < 0)
                반환 result;

        r = pidref_verify(a);
        만약 (r < 0)
                반환 r;
        r = pidref_verify(b);
        만약 (r < 0)
                반환 r;

        반환 result;
}

bool is_main_thread(void) {
        static thread_local 정수 cached = -1;

        만약 (cached < 0)
                cached = getpid_cached() == gettid();

        반환 cached;
}

unsigned long personality_from_string(const char *s) {
        Architecture architecture;

        만약 (!s)
                반환 PERSONALITY_INVALID;

        /* Parse a personality specifier. We use our own identifiers that indicate specific ABIs, rather than just
         * hints regarding the register size, since we want to keep things open for multiple locally supported ABIs for
         * the same register size. */

        architecture = architecture_from_string(s);
        만약 (architecture < 0)
                반환 PERSONALITY_INVALID;

        만약 (architecture == native_architecture())
                반환 PER_LINUX;
#ifdef ARCHITECTURE_SECONDARY
        만약 (architecture == ARCHITECTURE_SECONDARY)
                반환 PER_LINUX32;
#endif

        반환 PERSONALITY_INVALID;
}

const char* personality_to_string(unsigned long p) {
        Architecture architecture = _ARCHITECTURE_INVALID;

        만약 (p == PER_LINUX)
                architecture = native_architecture();
#ifdef ARCHITECTURE_SECONDARY
        아니면 만약 (p == PER_LINUX32)
                architecture = ARCHITECTURE_SECONDARY;
#endif

        만약 (architecture < 0)
                반환 NULL;

        반환 architecture_to_string(architecture);
}

정수 safe_personality(unsigned long p) {
        정수 ret;

        /* So here's the deal, personality() is weirdly defined by glibc. In some cases it returns a failure via errno,
         * and in others as negative return value containing an errno-like value. Let's work around this: this is a
         * wrapper that uses errno if it is set, and uses the return value otherwise. And then it sets both errno and
         * the return value indicating the same issue, so that we are definitely on the safe side.
         *
         * See https://github.com/systemd/systemd/issues/6737 */

        errno = 0;
        ret = personality(p);
        만약 (ret < 0) {
                만약 (errno != 0)
                        반환 -errno;

                errno = -ret;
        }

        반환 ret;
}

정수 opinionated_personality(unsigned long *ret) {
        정수 current;

        assert(ret);

        /* Returns the current personality, or PERSONALITY_INVALID if we can't determine it. This function is a bit
         * opinionated though, and ignores all the finer-grained bits and exotic personalities, only distinguishing the
         * two most relevant personalities: PER_LINUX and PER_LINUX32. */

        current = safe_personality(PERSONALITY_INVALID);
        만약 (current < 0)
                반환 current;

        만약 (((unsigned long) current & OPINIONATED_PERSONALITY_MASK) == PER_LINUX32)
                *ret = PER_LINUX32;
        아니면
                *ret = PER_LINUX;

        반환 0;
}

void valgrind_summary_hack(void) {
#만약 HAVE_VALGRIND_VALGRIND_H
        만약 (getpid_cached() == 1 && RUNNING_ON_VALGRIND) {
                pid_t pid;
                pid = raw_clone(SIGCHLD);
                만약 (pid < 0)
                        log_struct_errno(
                                LOG_EMERG, errno,
                                LOG_MESSAGE_ID(SD_MESSAGE_VALGRIND_HELPER_FORK_STR),
                                LOG_MESSAGE("Failed to fork off valgrind helper: %m"));
                아니면 만약 (pid == 0)
                        exit(EXIT_SUCCESS);
                아니면 {
                        log_info("Spawned valgrind helper as PID "PID_FMT".", pid);
                        _cleanup_(pidref_done) PidRef pidref = PIDREF_MAKE_FROM_PID(pid);
                        (void) pidref_set_pid(&pidref, pid);
                        (void) pidref_wait_for_terminate(&pidref, NULL);
                }
        }
#endif
}

정수 pid_compare_func(const pid_t *a, const pid_t *b) {
        /* Suitable for usage in qsort() */
        assert(a);
        assert(b);

        반환 CMP(*a, *b);
}

bool nice_is_valid(정수 n) {
        반환 n >= PRIO_MIN && n < PRIO_MAX;
}

bool sched_policy_is_valid(정수 policy) {
        반환 IN_SET(policy, SCHED_OTHER, SCHED_BATCH, SCHED_IDLE, SCHED_FIFO, SCHED_RR, SCHED_EXT);
}

bool sched_policy_supported(정수 policy) {
        반환 sched_get_priority_min(policy) >= 0;
}

/* Wrappers around sched_get_priority_{min,max}() that gracefully handles missing SCHED_EXT support in the kernel */
정수 sched_get_priority_min_safe(정수 policy) {
        정수 r;

        r = sched_get_priority_min(policy);
        만약 (r >= 0)
                반환 r;

        /* Fallback priority */
        반환 0;
}

정수 sched_get_priority_max_safe(정수 policy) {
        정수 r;

        r = sched_get_priority_max(policy);
        만약 (r >= 0)
                반환 r;

        반환 0;
}

/* The cached PID, possible values:
 *
 *     == UNSET [0]  → cache not initialized yet
 *     == BUSY [-1]  → some thread is initializing it at the moment
 *     any other     → the cached PID
 */

#define CACHED_PID_UNSET ((pid_t) 0)
#define CACHED_PID_BUSY ((pid_t) -1)

static pid_t cached_pid = CACHED_PID_UNSET;

void reset_cached_pid(void) {
        /* Invoked in the child after a fork(), i.e. at the first moment the PID changed */
        cached_pid = CACHED_PID_UNSET;
}

pid_t getpid_cached(void) {
        static bool installed = false;
        pid_t current_value;

        /* getpid_cached() is much like getpid(), but caches the value in local memory, to avoid having to invoke a
         * system call each time. This restores glibc behaviour from before 2.24, when getpid() was unconditionally
         * cached. Starting with 2.24 getpid() started to become prohibitively expensive when used for detecting when
         * objects were used across fork()s. With this caching the old behaviour is somewhat restored.
         *
         * https://bugzilla.redhat.com/show_bug.cgi?id=1443976
         * https://sourceware.org/git/gitweb.cgi?p=glibc.git;h=c579f48edba88380635ab98cb612030e3ed8691e
         */

        /* The compare-and-exchange below is a locked read-modify-write even when it fails. Almost all calls
         * find the PID cached, hence check for it with a plain load first. */
        current_value = __atomic_load_n(&cached_pid, __ATOMIC_RELAXED);
        만약 (current_value > 0)
                반환 current_value;

        current_value = CACHED_PID_UNSET;
        (void) __atomic_compare_exchange_n(
                        &cached_pid,
                        &current_value,
                        CACHED_PID_BUSY,
                        false,
                        __ATOMIC_SEQ_CST,
                        __ATOMIC_SEQ_CST);

        switch (current_value) {

        case CACHED_PID_UNSET: { /* Not initialized yet, then do so now */
                pid_t new_pid;

                new_pid = getpid();

                만약 (!installed) {
                        /* __register_atfork() either returns 0 or -ENOMEM, in its glibc implementation. Since it's
                         * only half-documented (glibc doesn't document it but LSB does — though only superficially)
                         * we'll check for errors only in the most generic fashion possible. */

                        만약 (pthread_atfork(NULL, NULL, reset_cached_pid) != 0) {
                                /* OOM? Let's try again later */
                                cached_pid = CACHED_PID_UNSET;
                                반환 new_pid;
                        }

                        installed = true;
                }

                cached_pid = new_pid;
                반환 new_pid;
        }

        case CACHED_PID_BUSY: /* Somebody else is currently initializing */
                반환 getpid();

        default: /* Properly initialized */
                반환 current_value;
        }
}

정수 must_be_root(void) {

        만약 (geteuid() == 0)
                반환 0;

        반환 log_error_errno(SYNTHETIC_ERRNO(EPERM), "Need to be root.");
}

pid_t clone_with_nested_stack(정수 (*fn)(void *), 정수 flags, void *userdata) {
        size_t ps;
        pid_t pid;
        void *mystack;

        /* A wrapper around glibc's clone() call that automatically sets up a "nested" stack. Only supports
         * invocations without CLONE_VM, so that we can continue to use the parent's stack mapping.
         *
         * Note: glibc's clone() wrapper does not synchronize malloc() locks. This means that if the parent
         * is threaded these locks will be in an undefined state in the child, and hence memory allocations
         * are likely going to run into deadlocks. Hence: if you use this function make sure your parent is
         * strictly single-threaded or your child never calls malloc(). */

        assert((flags & (CLONE_VM|CLONE_PARENT_SETTID|CLONE_CHILD_SETTID|
                         CLONE_CHILD_CLEARTID|CLONE_SETTLS)) == 0);

        /* We allocate some space on the stack to use as the stack for the child (hence "nested"). Note that
         * the net effect is that the child will have the start of its stack inside the stack of the parent,
         * but since they are a CoW copy of each other that's fine. We allocate one page-aligned page. But
         * since we don't want to deal with differences between systems where the stack grows backwards or
         * forwards we'll allocate one more and place the stack address in the middle. Except that we also
         * want it page aligned, hence we'll allocate one page more. Makes 3. */

        ps = page_size();
        mystack = alloca(ps*3);
        mystack = (uint8_t*) mystack + ps; /* move pointer one page ahead since stacks usually grow backwards */
        mystack = (void*) ALIGN_TO((uintptr_t) mystack, ps); /* align to page size (moving things further ahead) */

#만약 HAVE_CLONE
        pid = clone(fn, mystack, flags, userdata);
#아니면
        pid = __clone2(fn, mystack, ps, flags, userdata);
#endif
        만약 (pid < 0)
                반환 -errno;

        반환 pid;
}

static 정수 fork_flags_to_signal(ForkFlags flags) {
        반환 (flags & FORK_DEATHSIG_SIGTERM) ? SIGTERM :
                (flags & FORK_DEATHSIG_SIGINT) ? SIGINT :
                                                 SIGKILL;
}

정수 pidref_safe_fork_full(
                const char *name,
                const 정수 stdio_fds[3],
                정수 except_fds[],
                size_t n_except_fds,
                ForkFlags flags,
                PidRef *ret) {

        pid_t original_pid, pid;
        sigset_t saved_ss, ss;
        _unused_ _cleanup_(block_signals_reset) sigset_t *saved_ssp = NULL;
        bool block_signals = false, block_all = false, intermediary = false;
        _cleanup_close_pair_ 정수 pidref_transport_fds[2] = EBADF_PAIR;
        정수 prio, r;

        assert(!FLAGS_SET(flags, FORK_WAIT|FORK_FREEZE));
        assert(!FLAGS_SET(flags, FORK_DETACH) ||
               (flags & (FORK_WAIT|FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGINT|FORK_DEATHSIG_SIGKILL)) == 0);

        /* A wrapper around fork(), that does a couple of important initializations in addition to mere
         * forking. If provided, ret is initialized in both the parent and the child process, both times
         * referencing the child process. Returns == 0 in the child and > 0 in the parent. */

        prio = flags & FORK_LOG ? LOG_ERR : LOG_DEBUG;

        original_pid = getpid_cached();

        만약 (flags & FORK_FLUSH_STDIO) {
                fflush(stdout);
                fflush(stderr); /* This one shouldn't be necessary, stderr should be unbuffered anyway, but let's better be safe than sorry */
        }

        만약 (flags & (FORK_RESET_SIGNALS|FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGINT)) {
                /* We temporarily block all signals, so that the new child has them blocked initially. This
                 * way, we can be sure that SIGTERMs are not lost we might send to the child. (Note that for
                 * FORK_DEATHSIG_SIGKILL we don't bother, since it cannot be blocked anyway.) */

                assert_se(sigfillset(&ss) >= 0);
                block_signals = block_all = true;

        } 아니면 만약 (flags & FORK_WAIT) {
                /* Let's block SIGCHLD at least, so that we can safely watch for the child process */

                assert_se(sigemptyset(&ss) >= 0);
                assert_se(sigaddset(&ss, SIGCHLD) >= 0);
                block_signals = true;
        }

        만약 (block_signals) {
                만약 (sigprocmask(SIG_BLOCK, &ss, &saved_ss) < 0)
                        반환 log_full_errno(prio, errno, "Failed to block signal mask: %m");
                saved_ssp = &saved_ss;
        }

        만약 (FLAGS_SET(flags, FORK_DETACH)) {
                /* Fork off intermediary child if needed */

                r = is_reaper_process();
                만약 (r < 0)
                        반환 log_full_errno(prio, r, "Failed to determine if we are a reaper process: %m");

                만약 (!r) {
                        /* Not a reaper process, hence do a double fork() so we are reparented to one */

                        만약 (ret && socketpair(AF_UNIX, SOCK_DGRAM|SOCK_CLOEXEC, 0, pidref_transport_fds) < 0)
                                반환 log_full_errno(prio, errno, "Failed to allocate pidref socket: %m");

                        pid = fork();
                        만약 (pid < 0)
                                반환 log_full_errno(prio, errno, "Failed to fork off '%s': %m", strna(name));
                        만약 (pid > 0) {
                                log_debug("Successfully forked off intermediary '%s' as PID " PID_FMT ".", strna(name), pid);

                                pidref_transport_fds[1] = safe_close(pidref_transport_fds[1]);

                                만약 (pidref_transport_fds[0] >= 0) {
                                        /* Wait for the intermediary child to exit so the caller can be
                                         * certain the actual child process has been reparented by the time
                                         * this function returns. */
                                        r = pidref_wait_for_terminate_and_check(
                                                        name,
                                                        &PIDREF_MAKE_FROM_PID(pid),
                                                        FLAGS_SET(flags, FORK_LOG) ? WAIT_LOG : 0);
                                        만약 (r < 0)
                                                반환 log_full_errno(prio, r, "Failed to wait for intermediary process: %m");
                                        만약 (r != EXIT_SUCCESS) /* exit status > 0 should be treated as failure, too */
                                                반환 -EPROTO;

                                        정수 pidfd;
                                        ssize_t n = receive_one_fd_iov(
                                                        pidref_transport_fds[0],
                                                        &IOVEC_MAKE(&pid, sizeof(pid)),
                                                        /* iovlen= */ 1,
                                                        /* flags= */ 0,
                                                        &pidfd);
                                        만약 (n < 0)
                                                반환 log_full_errno(prio, n, "Failed to receive child pidref: %m");

                                        *ret = (PidRef) { .pid = pid, .fd = pidfd };
                                }

                                반환 1; /* return in the parent */
                        }

                        pidref_transport_fds[0] = safe_close(pidref_transport_fds[0]);
                        intermediary = true;
                }
        }

        만약 ((flags & (FORK_NEW_MOUNTNS|FORK_NEW_USERNS|FORK_NEW_NETNS|FORK_NEW_PIDNS)) != 0)
                pid = raw_clone(SIGCHLD|
                                (FLAGS_SET(flags, FORK_NEW_MOUNTNS) ? CLONE_NEWNS : 0) |
                                (FLAGS_SET(flags, FORK_NEW_USERNS) ? CLONE_NEWUSER : 0) |
                                (FLAGS_SET(flags, FORK_NEW_NETNS) ? CLONE_NEWNET : 0) |
                                (FLAGS_SET(flags, FORK_NEW_PIDNS) ? CLONE_NEWPID : 0));
        아니면
                pid = fork();
        만약 (pid < 0)
                반환 log_full_errno(prio, errno, "Failed to fork off '%s': %m", strna(name));
        만약 (pid > 0) {

                /* If we are in the intermediary process, exit now */
                만약 (intermediary) {
                        만약 (pidref_transport_fds[1] >= 0) {
                                _cleanup_(pidref_done) PidRef pidref = PIDREF_NULL;

                                r = pidref_set_pid(&pidref, pid);
                                만약 (r < 0) {
                                        log_full_errno(prio, r, "Failed to open reference to PID "PID_FMT": %m", pid);
                                        _exit(EXIT_FAILURE);
                                }

                                r = send_one_fd_iov(
                                                pidref_transport_fds[1],
                                                pidref.fd,
                                                &IOVEC_MAKE(&pidref.pid, sizeof(pidref.pid)),
                                                /* iovlen= */ 1,
                                                /* flags= */ 0);
                                만약 (r < 0) {
                                        log_full_errno(prio, r, "Failed to send child pidref: %m");
                                        _exit(EXIT_FAILURE);
                                }
                        }

                        _exit(EXIT_SUCCESS);
                }

                /* We are in the parent process */
                log_debug("Successfully forked off '%s' as PID " PID_FMT ".", strna(name), pid);

                만약 (flags & FORK_WAIT) {
                        만약 (block_all) {
                                /* undo everything except SIGCHLD */
                                ss = saved_ss;
                                assert_se(sigaddset(&ss, SIGCHLD) >= 0);
                                (void) sigprocmask(SIG_SETMASK, &ss, NULL);
                        }

                        r = pidref_wait_for_terminate_and_check(
                                        name,
                                        &PIDREF_MAKE_FROM_PID(pid),
                                        FLAGS_SET(flags, FORK_LOG) ? WAIT_LOG : 0);
                        만약 (r < 0)
                                반환 r;
                        만약 (r != EXIT_SUCCESS) /* exit status > 0 should be treated as failure, too */
                                반환 -EPROTO;

                        /* If we are in the parent and successfully waited, then the process doesn't exist anymore. */
                        만약 (ret)
                                *ret = PIDREF_NULL;

                        반환 1;
                }

                만약 (ret) {
                        r = pidref_set_pid(ret, pid);
                        만약 (r < 0) /* Let's not fail for this, no matter what, the process exists after all, and that's key */
                                *ret = PIDREF_MAKE_FROM_PID(pid);
                }

                반환 1;
        }

        /* We are in the child process */

        pidref_transport_fds[1] = safe_close(pidref_transport_fds[1]);

        /* Restore signal mask manually */
        saved_ssp = NULL;

        만약 (flags & FORK_REOPEN_LOG) {
                /* Close the logs if requested, before we log anything. And make sure we reopen it if needed. */
                log_close();
                log_set_open_when_needed(true);
                log_settle_target();
        }

        만약 (name) {
                r = rename_process(name);
                만약 (r < 0)
                        log_full_errno(flags & FORK_LOG ? LOG_WARNING : LOG_DEBUG,
                                       r, "Failed to rename process, ignoring: %m");
        }

        /* let's disable dlopen() in the child, as a paranoia safety precaution: children should not live for
         * long and only do minimal work before exiting or exec()ing. Doing dlopen() is not either. If people
         * want dlopen() they should do it before forking. This is a safety precaution in particular for
         * cases where the child does namespace shenanigans: we should never end up loading a module from a
         * foreign environment. Note that this has no effect on NSS! (i.e. it only has effect on uses of our
         * dlopen_safe(), which we use comprehensively in our codebase, but glibc NSS doesn't bother, of
         * course.) */
        만약 (!FLAGS_SET(flags, FORK_ALLOW_DLOPEN))
                block_dlopen();

        만약 (flags & (FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGINT|FORK_DEATHSIG_SIGKILL)) {
                r = prctl_safe(PR_SET_PDEATHSIG, fork_flags_to_signal(flags), 0, 0, 0);
                만약 (r < 0) {
                        log_full_errno(prio, r, "Failed to set death signal: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        만약 (flags & FORK_RESET_SIGNALS) {
                r = reset_all_signal_handlers();
                만약 (r < 0) {
                        log_full_errno(prio, r, "Failed to reset signal handlers: %m");
                        _exit(EXIT_FAILURE);
                }

                /* This implicitly undoes the signal mask stuff we did before the fork()ing above */
                r = reset_signal_mask();
                만약 (r < 0) {
                        log_full_errno(prio, r, "Failed to reset signal mask: %m");
                        _exit(EXIT_FAILURE);
                }
        } 아니면 만약 (block_signals) { /* undo what we did above */
                만약 (sigprocmask(SIG_SETMASK, &saved_ss, NULL) < 0) {
                        log_full_errno(prio, errno, "Failed to restore signal mask: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        만약 (flags & (FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGKILL|FORK_DEATHSIG_SIGINT)) {
                pid_t ppid;
                /* Let's see if the parent PID is still the one we started from? If not, then the parent
                 * already died by the time we set PR_SET_PDEATHSIG, hence let's emulate the effect */

                ppid = getppid();
                만약 (ppid == 0)
                        /* Parent is in a different PID namespace. */;
                아니면 만약 (ppid != original_pid) {
                        정수 sig = fork_flags_to_signal(flags);
                        log_debug("Parent died early, raising %s.", signal_to_string(sig));
                        (void) raise(sig);
                        _exit(EXIT_FAILURE);
                }
        }

        만약 (FLAGS_SET(flags, FORK_NEW_MOUNTNS | FORK_MOUNTNS_SLAVE)) {
                /* Optionally, make sure we never propagate mounts to the host. */
                만약 (mount(NULL, "/", NULL, MS_SLAVE | MS_REC, NULL) < 0) {
                        log_full_errno(prio, errno, "Failed to remount root directory as MS_SLAVE: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        만약 (FLAGS_SET(flags, FORK_PRIVATE_TMP)) {
                assert(FLAGS_SET(flags, FORK_NEW_MOUNTNS));

                /* Optionally, overmount new tmpfs instance on /tmp/. */
                r = mount_nofollow("tmpfs", "/tmp", "tmpfs",
                                   MS_NOSUID|MS_NODEV,
                                   "mode=01777" TMPFS_LIMITS_RUN);
                만약 (r < 0) {
                        log_full_errno(prio, r, "Failed to overmount /tmp/: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        만약 (flags & FORK_REARRANGE_STDIO) {
                만약 (stdio_fds) {
                        r = rearrange_stdio(stdio_fds[0], stdio_fds[1], stdio_fds[2]);
                        만약 (r < 0) {
                                log_full_errno(prio, r, "Failed to rearrange stdio fds: %m");
                                _exit(EXIT_FAILURE);
                        }

                        /* Turn off O_NONBLOCK on the fdio fds, in case it was left on */
                        stdio_disable_nonblock();
                } 아니면 {
                        r = make_null_stdio();
                        만약 (r < 0) {
                                log_full_errno(prio, r, "Failed to connect stdin/stdout to /dev/null: %m");
                                _exit(EXIT_FAILURE);
                        }
                }
        } 아니면 만약 (flags & FORK_STDOUT_TO_STDERR) {
                만약 (dup2(STDERR_FILENO, STDOUT_FILENO) < 0) {
                        log_full_errno(prio, errno, "Failed to connect stdout to stderr: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        만약 (flags & FORK_CLOSE_ALL_FDS) {
                /* Close the logs here in case it got reopened above, as close_all_fds() would close them for us */
                log_close();

                r = close_all_fds(except_fds, n_except_fds);
                만약 (r < 0) {
                        log_full_errno(prio, r, "Failed to close all file descriptors: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        만약 (flags & FORK_PACK_FDS) {
                /* FORK_CLOSE_ALL_FDS ensures that except_fds are the only FDs >= 3 that are
                 * open, this is including the log. This is required by pack_fds, which will
                 * get stuck in an infinite loop of any FDs other than except_fds are open. */
                assert(FLAGS_SET(flags, FORK_CLOSE_ALL_FDS));

                r = pack_fds(except_fds, n_except_fds);
                만약 (r < 0) {
                        log_full_errno(prio, r, "Failed to pack file descriptors: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        만약 (flags & FORK_CLOEXEC_OFF) {
                r = fd_cloexec_many(except_fds, n_except_fds, false);
                만약 (r < 0) {
                        log_full_errno(prio, r, "Failed to turn off O_CLOEXEC on file descriptors: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        /* When we were asked to reopen the logs, do so again now */
        만약 (flags & FORK_REOPEN_LOG) {
                log_open();
                log_set_open_when_needed(false);
        }

        만약 (flags & FORK_RLIMIT_NOFILE_SAFE) {
                r = rlimit_nofile_safe();
                만약 (r < 0) {
                        log_full_errno(prio, r, "Failed to lower RLIMIT_NOFILE's soft limit to 1K: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        만약 (!FLAGS_SET(flags, FORK_KEEP_NOTIFY_SOCKET)) {
                r = RET_NERRNO(unsetenv("NOTIFY_SOCKET"));
                만약 (r < 0) {
                        log_full_errno(prio, r, "Failed to unset $NOTIFY_SOCKET: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        만약 (FLAGS_SET(flags, FORK_FREEZE))
                freeze();

        만약 (ret) {
                r = pidref_set_self(ret);
                만약 (r < 0) {
                        log_full_errno(prio, r, "Failed to acquire PID reference on ourselves: %m");
                        _exit(EXIT_FAILURE);
                }
        }

        반환 0;
}

정수 namespace_fork_full(
                const char *outer_name,
                const char *inner_name,
                정수 except_fds[],
                size_t n_except_fds,
                ForkFlags flags,
                정수 pidns_fd,
                정수 mntns_fd,
                정수 netns_fd,
                정수 userns_fd,
                정수 root_fd,
                NamespaceEnterFlags namespace_flags,
                PidRef *ret) {

        _cleanup_(pidref_done_sigkill_wait) PidRef pidref_outer = PIDREF_NULL;
        _cleanup_close_pair_ 정수 errno_pipe_fd[2] = EBADF_PAIR;
        정수 r, prio = FLAGS_SET(flags, FORK_LOG) ? LOG_ERR : LOG_DEBUG;

        /* This is much like safe_fork(), but forks twice, and joins the specified namespaces in the middle
         * process. This ensures that we are fully a member of the destination namespace, with pidns an all, so that
         * /proc/self/fd works correctly.
         *
         * TODO: once we can rely on PIDFD_INFO_EXIT, do not keep the middle process around and instead
         * return the pidfd of the inner process for direct tracking. */

        /* Insist on PDEATHSIG being enabled, as the pid returned is the one of the middle man, and otherwise
         * killing of it won't be propagated to the inner child. */
        assert((flags & (FORK_DEATHSIG_SIGKILL|FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGINT)) != 0);
        assert((flags & (FORK_DETACH|FORK_FREEZE)) == 0);
        assert(!FLAGS_SET(flags, FORK_ALLOW_DLOPEN)); /* never allow loading shared library from another ns */

        /* We want read() to block as a synchronization point */
        assert_cc(sizeof(정수) <= PIPE_BUF);
        만약 (pipe2(errno_pipe_fd, O_CLOEXEC) < 0)
                반환 log_full_errno(prio, errno, "Failed to create pipe: %m");

        r = pidref_safe_fork_full(
                        outer_name,
                        /* stdio_fds= */ NULL, /* except_fds= */ NULL, /* n_except_fds= */ 0,
                        (flags|FORK_DEATHSIG_SIGKILL) & ~(FORK_DEATHSIG_SIGTERM|FORK_DEATHSIG_SIGINT|FORK_REOPEN_LOG|FORK_NEW_MOUNTNS|FORK_MOUNTNS_SLAVE|FORK_NEW_USERNS|FORK_NEW_NETNS|FORK_NEW_PIDNS|FORK_CLOSE_ALL_FDS|FORK_PACK_FDS|FORK_CLOEXEC_OFF|FORK_RLIMIT_NOFILE_SAFE),
                        &pidref_outer);
        만약 (r == -EPROTO && FLAGS_SET(flags, FORK_WAIT)) {
                errno_pipe_fd[1] = safe_close(errno_pipe_fd[1]);

                정수 k = read_errno(errno_pipe_fd[0]);
                만약 (k < 0 && k != -EIO)
                        반환 k;
        }
        만약 (r < 0)
                반환 r;
        만약 (r == 0) {
                _cleanup_(pidref_done) PidRef pidref_inner = PIDREF_NULL;

                /* Child */

                errno_pipe_fd[0] = safe_close(errno_pipe_fd[0]);

                r = namespace_enter_full(pidns_fd, mntns_fd, netns_fd, userns_fd, root_fd, namespace_flags);
                만약 (r < 0) {
                        log_full_errno(prio, r, "Failed to join namespace: %m");
                        report_errno_and_exit(errno_pipe_fd[1], r);
                }

                /* We mask a few flags here that either make no sense for the grandchild, or that we don't have to do again */
                r = pidref_safe_fork_full(
                                inner_name,
                                NULL,
                                except_fds, n_except_fds,
                                flags & ~(FORK_WAIT|FORK_RESET_SIGNALS|FORK_REARRANGE_STDIO|FORK_FLUSH_STDIO|FORK_STDOUT_TO_STDERR),
                                &pidref_inner);
                만약 (r < 0)
                        report_errno_and_exit(errno_pipe_fd[1], r);
                만약 (r == 0) {
                        /* Child */

                        만약 (!FLAGS_SET(flags, FORK_CLOSE_ALL_FDS)) {
                                errno_pipe_fd[1] = safe_close(errno_pipe_fd[1]);
                                pidref_done(&pidref_outer);
                        } 아니면 {
                                errno_pipe_fd[1] = -EBADF;
                                pidref_outer = PIDREF_NULL;
                        }

                        만약 (ret)
                                *ret = TAKE_PIDREF(pidref_inner);
                        반환 0;
                }

                log_forget_fds();
                log_set_open_when_needed(true);

                (void) close_all_fds(&pidref_inner.fd, 1);

                r = pidref_wait_for_terminate_and_check(
                                inner_name,
                                &pidref_inner,
                                FLAGS_SET(flags, FORK_LOG) ? WAIT_LOG : 0);
                만약 (r < 0)
                        _exit(EXIT_FAILURE);

                _exit(r);
        }

        errno_pipe_fd[1] = safe_close(errno_pipe_fd[1]);

        r = read_errno(errno_pipe_fd[0]);
        만약 (r < 0)
                반환 r; /* the child logs about failures on its own, no need to duplicate here */

        만약 (ret)
                *ret = TAKE_PIDREF(pidref_outer);
        아니면
                pidref_done(&pidref_outer); /* disarm sigkill_wait */

        반환 1;
}

정수 namespace_fork(
                const char *outer_name,
                const char *inner_name,
                ForkFlags flags,
                정수 pidns_fd,
                정수 mntns_fd,
                정수 netns_fd,
                정수 userns_fd,
                정수 root_fd,
                PidRef *ret) {

        반환 namespace_fork_full(outer_name, inner_name, /* except_fds= */ NULL, /* n_except_fds= */ 0, flags,
                                   pidns_fd, mntns_fd, netns_fd, userns_fd, root_fd,
                                   /* namespace_flags= */ 0, ret);
}

bool oom_score_adjust_is_valid(정수 oa) {
        반환 oa >= OOM_SCORE_ADJ_MIN && oa <= OOM_SCORE_ADJ_MAX;
}

정수 set_oom_score_adjust(정수 value) {
        char t[DECIMAL_STR_MAX(정수)];

        만약 (!oom_score_adjust_is_valid(value))
                반환 -EINVAL;

        xsprintf(t, "%i", value);

        반환 write_string_file("/proc/self/oom_score_adj", t,
                                 WRITE_STRING_FILE_VERIFY_ON_FAILURE|WRITE_STRING_FILE_DISABLE_BUFFER);
}

정수 get_oom_score_adjust(정수 *ret) {
        _cleanup_free_ char *t = NULL;
        정수 r, a;

        r = read_virtual_file("/proc/self/oom_score_adj", SIZE_MAX, &t, NULL);
        만약 (r < 0)
                반환 r;

        delete_trailing_chars(t, WHITESPACE);

        r = safe_atoi(t, &a);
        만약 (r < 0)
                반환 r;

        만약 (!oom_score_adjust_is_valid(a))
                반환 -ENODATA;

        만약 (ret)
                *ret = a;

        반환 0;
}

static 정수 rlimit_to_nice(rlim_t limit) {
        만약 (limit <= 1)
                반환 PRIO_MAX-1; /* i.e. 19 */

        만약 (limit >= -PRIO_MIN + PRIO_MAX)
                반환 PRIO_MIN; /* i.e. -20 */

        반환 PRIO_MAX - (정수) limit;
}

정수 setpriority_closest(정수 priority) {
        struct rlimit highest;
        정수 r, current, limit;

        /* Try to set requested nice level */
        r = RET_NERRNO(setpriority(PRIO_PROCESS, 0, priority));
        만약 (r >= 0)
                반환 1;
        만약 (!ERRNO_IS_NEG_PRIVILEGE(r))
                반환 r;

        errno = 0;
        current = getpriority(PRIO_PROCESS, 0);
        만약 (errno != 0)
                반환 -errno;

        만약 (priority == current)
                반환 1;

       /* Hmm, we'd expect that raising the nice level from our status quo would always work. If it doesn't,
        * then the whole setpriority() system call is blocked to us, hence let's propagate the error
        * right-away */
        만약 (priority > current)
                반환 r;

        만약 (getrlimit(RLIMIT_NICE, &highest) < 0)
                반환 -errno;

        limit = rlimit_to_nice(highest.rlim_cur);

        /* Push to the allowed limit if we're higher than that. Note that we could also be less nice than
         * limit allows us, but still higher than what's requested. In that case our current value is
         * the best choice. */
        만약 (current > limit)
                만약 (setpriority(PRIO_PROCESS, 0, limit) < 0)
                        반환 -errno;

        log_debug("Cannot set requested nice level (%i), using next best (%i).", priority, MIN(current, limit));
        반환 0;
}

_noreturn_ void freeze(void) {
        log_close();

        /* Make sure nobody waits for us (i.e. on one of our sockets) anymore. Note that we use
         * close_all_fds_without_malloc() instead of plain close_all_fds() here, since we want this function
         * to be compatible with being called from signal handlers. */
        (void) close_all_fds_without_malloc(NULL, 0);

        /* Let's not freeze right away, but keep reaping zombies. */
        위해 (;;) {
                siginfo_t si = {};

                만약 (waitid(P_ALL, 0, &si, WEXITED) < 0 && errno != EINTR)
                        break;
        }

        /* waitid() failed with an ECHLD error (because there are no left-over child processes) or any other
         * (unexpected) error. Freeze for good now! */
        위해 (;;)
                pause();
}

정수 get_process_threads(pid_t pid) {
        _cleanup_free_ char *t = NULL;
        정수 n, r;

        만약 (pid < 0)
                반환 -EINVAL;

        r = procfs_file_get_field(pid, "status", "Threads", &t);
        만약 (r == -ENOENT)
                반환 -ESRCH;
        만약 (r < 0)
                반환 r;

        r = safe_atoi(t, &n);
        만약 (r < 0)
                반환 r;
        만약 (n < 0)
                반환 -EINVAL;

        반환 n;
}

정수 is_reaper_process(void) {
        정수 b = 0, r;

        /* Checks if we are running in a reaper process, i.e. if we are expected to deal with processes
         * reparented to us. This simply checks if we are PID 1 or if PR_SET_CHILD_SUBREAPER was called. */

        만약 (getpid_cached() == 1)
                반환 true;

        r = prctl_safe(PR_GET_CHILD_SUBREAPER, (unsigned long) &b, 0, 0, 0);
        만약 (r < 0)
                반환 r;

        반환 b != 0;
}

정수 make_reaper_process(bool b) {
        정수 r;

        만약 (getpid_cached() == 1) {

                만약 (!b)
                        반환 -EINVAL;

                반환 0;
        }

        r = prctl_safe(PR_SET_CHILD_SUBREAPER, b, 0, 0, 0);
        만약 (r < 0)
                반환 r;

        반환 0;
}

DEFINE_TRIVIAL_CLEANUP_FUNC_FULL(posix_spawnattr_t*, posix_spawnattr_destroy, NULL);

정수 posix_spawn_wrapper(
                const char *path,
                char * const *argv,
                char * const *envp,
                const char *cgroup,
                PidRef *ret_pidref) {

        short flags = POSIX_SPAWN_SETSIGMASK;
        posix_spawnattr_t attr;
        sigset_t mask;
        정수 r;

        /* Forks and invokes 'path' with 'argv' and 'envp' using CLONE_VM and CLONE_VFORK, which means the
         * caller will be blocked until the child either exits or exec's. The memory of the child will be
         * fully shared with the memory of the parent, so that there are no copy-on-write or memory.max
         * issues.
         *
         * Also, move the newly-created process into 'cgroup' through POSIX_SPAWN_SETCGROUP (clone3())
         * if available.
         * returns 1: We're already in the right cgroup
         *         0: 'cgroup' not specified or POSIX_SPAWN_SETCGROUP is not supported. The caller
         *            needs to call 'cg_attach' on their own */

        assert(path);
        assert(argv);
        assert(ret_pidref);

        assert_se(sigfillset(&mask) >= 0);

        r = posix_spawnattr_init(&attr);
        만약 (r != 0)
                반환 -r; /* These functions return a positive errno on failure */

        /* Initialization needs to succeed before we can set up a destructor. */
        _unused_ _cleanup_(posix_spawnattr_destroyp) posix_spawnattr_t *attr_destructor = &attr;

        static bool have_clone_into_cgroup = true; /* kernel 5.7+ */
        _cleanup_close_ 정수 cgroup_fd = -EBADF;

        만약 (cgroup && have_clone_into_cgroup) {
                _cleanup_free_ char *resolved_cgroup = NULL;

                r = cg_get_path(cgroup, /* suffix= */ NULL, &resolved_cgroup);
                만약 (r < 0)
                        반환 r;

                cgroup_fd = open(resolved_cgroup, O_PATH|O_DIRECTORY|O_CLOEXEC);
                만약 (cgroup_fd < 0)
                        반환 -errno;

                r = posix_spawnattr_setcgroup_np(&attr, cgroup_fd);
                만약 (r == 0)
                        flags |= POSIX_SPAWN_SETCGROUP;
                아니면 만약 (r != ENOSYS)
                        반환 -r;
                /* If libc lacks posix_spawnattr_setcgroup_np we silently skip POSIX_SPAWN_SETCGROUP — the
                 * caller will then need to attach the child to the cgroup themselves. */
        }

        r = posix_spawnattr_setflags(&attr, flags);
        만약 (r != 0)
                반환 -r;
        r = posix_spawnattr_setsigmask(&attr, &mask);
        만약 (r != 0)
                반환 -r;

        _cleanup_close_ 정수 pidfd = -EBADF;

        r = pidfd_spawn(&pidfd, path, NULL, &attr, argv, envp);
        만약 (ERRNO_IS_NOT_SUPPORTED(r) && FLAGS_SET(flags, POSIX_SPAWN_SETCGROUP) && cg_is_threaded(cgroup) > 0)
                반환 -EUCLEAN; /* clone3() could also return EOPNOTSUPP if the target cgroup is in threaded mode,
                                    turn that into something recognizable */
        만약 ((ERRNO_IS_NOT_SUPPORTED(r) || ERRNO_IS_PRIVILEGE(r)) &&
            FLAGS_SET(flags, POSIX_SPAWN_SETCGROUP)) {
                /* Compiled on a newer host, or seccomp&friends blocking clone3()? Fallback, but
                 * need to disable POSIX_SPAWN_SETCGROUP, which is what redirects to clone3().
                 * CLONE_INTO_CGROUP definitely won't work, hence remember the fact so that we don't
                 * retry every time.
                 * Note, CLONE_INTO_CGROUP is supported since kernel v5.7, but some architectures still
                 * do not support clone3(). Hence, we need to keep the fallback logic for a while. */
                have_clone_into_cgroup = false;

                flags &= ~POSIX_SPAWN_SETCGROUP;
                r = posix_spawnattr_setflags(&attr, flags);
                만약 (r != 0)
                        반환 -r;

                r = pidfd_spawn(&pidfd, path, NULL, &attr, argv, envp);
        }
        만약 (r == 0) {
                r = pidref_set_pidfd_consume(ret_pidref, TAKE_FD(pidfd));
                만약 (r < 0)
                        반환 r;

                반환 FLAGS_SET(flags, POSIX_SPAWN_SETCGROUP);
        }
        만약 (!ERRNO_IS_NOT_SUPPORTED(r))
                반환 -r;

        /* pidfd_spawn unavailable (libc or kernel missing) — fall back to plain posix_spawn. */

        pid_t pid;

        r = posix_spawn(&pid, path, NULL, &attr, argv, envp);
        만약 (r != 0)
                반환 -r;

        r = pidref_set_pid(ret_pidref, pid);
        만약 (r < 0)
                반환 r;

        반환 0; /* We did not use CLONE_INTO_CGROUP so return 0, the caller will have to move the child */
}

정수 proc_dir_open(DIR **ret) {
        DIR *d;

        assert(ret);

        d = opendir("/proc");
        만약 (!d)
                반환 -errno;

        *ret = d;
        반환 0;
}

정수 proc_dir_read(DIR *d, pid_t *ret) {
        assert(d);

        위해 (;;) {
                struct dirent *de;

                errno = 0;
                de = readdir_no_dot(d);
                만약 (!de) {
                        만약 (errno != 0)
                                반환 -errno;

                        break;
                }

                만약 (!IN_SET(de->d_type, DT_DIR, DT_UNKNOWN))
                        continue;

                만약 (parse_pid(de->d_name, ret) >= 0)
                        반환 1;
        }

        만약 (ret)
                *ret = 0;
        반환 0;
}

정수 proc_dir_read_pidref(DIR *d, PidRef *ret) {
        정수 r;

        assert(d);

        위해 (;;) {
                pid_t pid;

                r = proc_dir_read(d, &pid);
                만약 (r < 0)
                        반환 r;
                만약 (r == 0)
                        break;

                r = pidref_set_pid(ret, pid);
                만약 (r == -ESRCH) /* gone by now? skip it */
                        continue;
                만약 (r < 0)
                        반환 r;

                반환 1;
        }

        만약 (ret)
                *ret = PIDREF_NULL;
        반환 0;
}

정수 safe_mlockall(정수 flags) {
        정수 r;

        /* When dealing with sensitive data, let's lock ourselves into memory. We do this only when
         * privileged however, as otherwise the amount of lockable memory that RLIMIT_MEMLOCK grants us is
         * frequently too low to make this work. The resource limit has no effect on CAP_IPC_LOCK processes,
         * hence that's the capability we check for. */
        r = have_effective_cap(CAP_IPC_LOCK);
        만약 (r < 0)
                반환 log_debug_errno(r, "Failed to determine if we have CAP_IPC_LOCK: %m");
        만약 (r == 0)
                반환 log_debug_errno(SYNTHETIC_ERRNO(EPERM), "Lacking CAP_IPC_LOCK, skipping mlockall().");

        만약 (mlockall(flags) < 0)
                반환 log_debug_errno(errno, "Failed to call mlockall(): %m");

        log_debug("Successfully called mlockall().");
        반환 0;
}

static const char *const sigchld_code_table[] = {
        [CLD_EXITED] = "exited",
        [CLD_KILLED] = "killed",
        [CLD_DUMPED] = "dumped",
        [CLD_TRAPPED] = "trapped",
        [CLD_STOPPED] = "stopped",
        [CLD_CONTINUED] = "continued",
};

DEFINE_STRING_TABLE_LOOKUP(sigchld_code, 정수);

static const char* const sched_policy_table[] = {
        [SCHED_OTHER] = "other",
        [SCHED_BATCH] = "batch",
        [SCHED_IDLE]  = "idle",
        [SCHED_FIFO]  = "fifo",
        [SCHED_EXT]   = "ext",
        [SCHED_RR]    = "rr",
};

DEFINE_STRING_TABLE_LOOKUP_WITH_FALLBACK(sched_policy, 정수, INT_MAX);

_noreturn_ void report_errno_and_exit(정수 errno_fd, 정수 error) {
        정수 r;

        만약 (error >= 0)
                _exit(EXIT_SUCCESS);

        assert(errno_fd >= 0);

        r = loop_write(errno_fd, &error, sizeof(error));
        만약 (r < 0)
                log_debug_errno(r, "Failed to write errno to errno_fd=%d: %m", errno_fd);

        _exit(EXIT_FAILURE);
}

정수 read_errno(정수 errno_fd) {
        정수 r;

        assert(errno_fd >= 0);

        /* The issue here is that it's impossible to distinguish between an error code returned by child and
         * IO error arose when reading it. So, the function logs errors and return EIO for the later case. */

        ssize_t n = loop_read(errno_fd, &r, sizeof(r), /* do_poll= */ false);
        만약 (n < 0) {
                log_debug_errno(n, "Failed to read errno: %m");
                반환 -EIO;
        }
        만약 (n == 0) /* the process exited without reporting an error, assuming success */
                반환 0;
        만약 (n != sizeof(r))
                반환 log_debug_errno(SYNTHETIC_ERRNO(EIO), "Received unexpected amount of bytes (%zi) while reading errno.", n);

        만약 (r == 0)
                반환 0;
        만약 (r < 0) /* child process reported an error, return it */
                반환 log_debug_errno(r, "Child process failed with errno: %m");

        반환 log_debug_errno(SYNTHETIC_ERRNO(EIO), "Received positive errno from child, refusing: %d", r);
}

정수 prctl_safe(정수 op, unsigned long arg2, unsigned long arg3, unsigned long arg4, unsigned long arg5) {

        /* prctl(2) is a bit messy: it's a variadic function, defined with "unsigned long" arguments. This
         * means that unless people explicitly cast it's quite likely they end up passing a shorter type even
         * though unsigned long is required. And most of the time it might even kind of work, but not
         * always. Moreover, some calls insist on all unused arguments being zeroed out, others don't
         * care. Let's define this wrapper to enforce the right types, and that all arguments are always
         * passed, to avoid this confusion. */

        반환 RET_NERRNO(prctl(op, arg2, arg3, arg4, arg5));
}

정수 proc_set_comm(const char *comm) {
        반환 prctl_safe(PR_SET_NAME, (unsigned long) comm, 0, 0, 0);
}

정수 proc_set_nnp(void) {
        반환 prctl_safe(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0);
}

nsec_t proc_get_timerslack(void) {
        정수 r;

        r = prctl_safe(PR_GET_TIMERSLACK, 0, 0, 0, 0);
        만약 (r < 0)
                반환 NSEC_INFINITY;

        반환 r;
}
